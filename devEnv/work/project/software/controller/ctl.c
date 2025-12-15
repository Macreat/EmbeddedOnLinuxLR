#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <mosquitto.h>
#include <pigpio.h>

// HEADERS DE TUS SENSORES
#include "../sensor/sensor.h"
#include "../sensor/mq2.h"
#include "../sensor/mq135.h"
#include "../sensor/dht11.h"
#include "../sensor/ky026.h"

// --- CONFIGURACIÓN DE PINES (GPIO BCM) ---
// NOTA: Como el botón está en la ESP32, no hay conflicto de pines aquí.
#define PIN_DHT11 16
#define PIN_MQ2 22
#define PIN_MQ135 23
#define PIN_KY026 27

// --- CONFIGURACIÓN MQTT AWS ---
#define MQTT_HOST "3.134.86.43"
#define MQTT_PORT 1883
#define MQTT_USER "esp32" // Ajustado para identificar a la Raspi
#define MQTT_PASS "12345678"

#define TOPIC_STATE "sistema/estado"     // Salida: NORMAL, WARNING, ALARM
#define TOPIC_SENSORS "sistema/sensores" // Salida: JSON completo
#define TOPIC_EVENT "sistema/evento"     // Entrada: EVENT_CLICK_1, EVENT_CLICK_2

// --- ESTADO GLOBAL DEL SISTEMA ---
int manual_mode = 0;               // 0 = Automático, 1 = Manual
char manual_status[20] = "NORMAL"; // Estado forzado cuando estamos en manual

// --- ESTRUCTURA DE DATOS ---
typedef struct
{
    double mq2_gas;
    double mq135_air;
    double temperature;
    double humidity;
    int flame_detected;
} SystemData;

// --- CALLBACK MQTT: EL CEREBRO DEL BOTÓN ---
// Aquí recibimos los clicks de la ESP32 o la Web
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    char *payload = (char *)msg->payload;
    printf("[MQTT EVENT] Recibido: %s\n", payload);

    if (strcmp(payload, "EVENT_CLICK_2") == 0)
    {
        // DOBLE CLICK: Prioridad -> MANUAL PELIGRO (ROJO)
        manual_mode = 1;
        strcpy(manual_status, "ALARM");
        printf(" -> ACCIÓN: Forzando MANUAL ALARM (ROJO)\n");
    }
    else if (strcmp(payload, "EVENT_CLICK_1") == 0)
    {
        // UN CLICK: Toggle
        if (manual_mode == 0)
        {
            // Si estaba Auto -> Pasa a MANUAL WARNING (AZUL)
            manual_mode = 1;
            strcpy(manual_status, "WARNING");
            printf(" -> ACCIÓN: Cambiando a MANUAL WARNING (AZUL)\n");
        }
        else
        {
            // Si estaba en CUALQUIER Manual -> Vuelve a AUTO
            manual_mode = 0;
            printf(" -> ACCIÓN: Volviendo a MODO AUTOMÁTICO\n");
        }
    }
}

// --- FUNCIÓN DE LECTURA DE SENSORES ---
void read_all_sensors(SystemData *data)
{
    // 1. Lectura MQ-2
    MQ2_Data mq2_result = mq2_read();
    data->mq2_gas = mq2_result.triggered ? 90.0 : 10.0;

    // 2. Lectura MQ-135
    MQ135_Data mq135_result = mq135_read();
    data->mq135_air = mq135_result.triggered ? 85.0 : 15.0;

    // 3. Lectura DHT11
    DHT11_Data dht_result = dht11_read();
    if (dht_result.valid)
    {
        data->temperature = (double)dht_result.temp_c;
        data->humidity = (double)dht_result.hum;
    }

    // 4. Lectura KY-026
    KY026_Data fire_result = ky026_read();
    data->flame_detected = fire_result.flame_detected;
}

int main(int argc, char *argv[])
{
    struct mosquitto *mosq;
    int rc;

    // 1. Inicializar Hardware
    if (gpioInitialise() < 0)
        return 1;

    // Inicializar Sensores
    mq2_init(PIN_MQ2, 0);
    mq135_init(PIN_MQ135, 0);
    dht11_init(PIN_DHT11);
    ky026_init(PIN_KY026, 0);

    // 2. Inicializar MQTT
    mosquitto_lib_init();
    mosq = mosquitto_new("raspi_brain_v2", true, NULL);
    mosquitto_username_pw_set(mosq, MQTT_USER, MQTT_PASS);

    // REGISTRAR CALLBACK PARA ESCUCHAR AL BOTÓN
    mosquitto_message_callback_set(mosq, on_message);

    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc != 0)
    {
        printf("Error MQTT: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    // SUSCRIBIRSE AL CANAL DE EVENTOS
    mosquitto_subscribe(mosq, NULL, TOPIC_EVENT, 0);
    mosquitto_loop_start(mosq);

    SystemData data = {0};
    char payload_sensors[512];
    char *system_state = "NORMAL"; // Estado final decidido

    printf("--- SISTEMA INICIADO EN MODO AUTO ---\n");

    while (1)
    {
        // A. SIEMPRE LEEMOS SENSORES (Para graficar en web)
        read_all_sensors(&data);

        // B. CEREBRO: DECIDIR ESTADO
        if (manual_mode == 1)
        {
            // --- MODO MANUAL (Botón activado) ---
            // Ignoramos sensores para el color, usamos el estado forzado
            if (strcmp(manual_status, "ALARM") == 0)
                system_state = "ALARM";
            else
                system_state = "WARNING";

            // (Opcional) Si detecta fuego real, podríamos forzar alarma igual por seguridad,
            // pero bajo tu pedido estricto, el manual manda.
        }
        else
        {
            // --- MODO AUTOMÁTICO (Sensores mandan) ---
            if (data.flame_detected == 1 || data.temperature > 30.0)
            {
                system_state = "ALARM";
            }
            else if (data.mq2_gas > 50.0)
            {
                system_state = "ALARM";
            }
            else if (data.mq135_air > 50.0 || data.temperature > 35.0)
            {
                system_state = "WARNING";
            }
            else
            {
                system_state = "NORMAL";
            }
        }

        // C. PUBLICAR ESTADO (Para que la ESP32 prenda el LED)
        mosquitto_publish(mosq, NULL, TOPIC_STATE, strlen(system_state), system_state, 0, 0);

        // D. PUBLICAR DATOS (Para la Web)
        // Incluimos "mode" para que la web sepa si pintar el botón de Azul/Rojo/Verde
        sprintf(payload_sensors,
                "{"
                "\"mq2\": %.1f, "
                "\"mq135\": %.1f, "
                "\"temp\": %.1f, "
                "\"hum\": %.1f, "
                "\"fire\": %d, "
                "\"status\": \"%s\", "
                "\"mode\": \"%s\""
                "}",
                data.mq2_gas,
                data.mq135_air,
                data.temperature,
                data.humidity,
                data.flame_detected,
                system_state,
                (manual_mode ? "MANUAL" : "AUTO"));

        mosquitto_publish(mosq, NULL, TOPIC_SENSORS, strlen(payload_sensors), payload_sensors, 0, 0);

        printf("[Estado: %s] [Modo: %s] T:%.1f\n", system_state, (manual_mode ? "MAN" : "AUTO"), data.temperature);
        sleep(1); // Ciclo de 1 segundo para mejor respuesta
    }

    mosquitto_lib_cleanup();
    gpioTerminate();
    return 0;
}