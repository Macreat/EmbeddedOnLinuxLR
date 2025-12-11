#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <mosquitto.h>

// Librería para GPIO (Necesaria para DHT11, KY-026, etc.)
#include <pigpio.h>

// TUS HEADERS DE SENSORES
#include "../sensor/sensor.h" // Tu generador genérico (si lo usas)
#include "../sensor/mq2.h"    // Gas/Humo
#include "../sensor/mq135.h"  // Calidad Aire
#include "../sensor/dht11.h"  // Temperatura y Humedad
#include "../sensor/ky026.h"  // Fuego/Llama

// (Los actuadores locales los incluimos pero NO los usamos, la ESP32 es quien actúa)
#include "../actuators/actuator.h"
#include "../actuators/led_actuator.h"

// --- CONFIGURACIÓN MQTT AWS ---
#define MQTT_HOST "3.134.86.43"
#define MQTT_PORT 1883
#define MQTT_USER "esp32"
#define MQTT_PASS "12345678"

#define TOPIC_STATE "sistema/estado"     // Payload: ALARM, WARNING, NORMAL
#define TOPIC_SENSORS "sistema/sensores" // Payload: JSON completo

// --- ESTRUCTURA DE DATOS EXPANDIDA ---
typedef struct
{
    double mq2_gas;     // Humo/Gas inflamable
    double mq135_air;   // Calidad del aire (CO2/Amoniaco)
    double temperature; // DHT11 Temp
    double humidity;    // DHT11 Humedad
    int flame_detected; // KY-026 (1=Fuego, 0=No Fuego)
} SystemData;

// --- FUNCIÓN DE LECTURA DE SENSORES ---
void read_all_sensors(SystemData *data)
{
    // IMPORTANTE: Aquí debes llamar a las funciones reales definidas en tus .h
    // Si tus .h solo tienen simulaciones, usa sensor_read().
    // Si tienen lógica real con pigpio, usa sus funciones especificas.

    // 1. Lectura MQ-2 (Humo)
    // data->mq2_gas = mq2_read_analog(); <--- Usa esto si tienes la función real
    data->mq2_gas = sensor_read(); // Simulado por ahora (basado en tu sensor.h)

    // 2. Lectura MQ-135 (Aire)
    // data->mq135_air = mq135_read_ppm();
    data->mq135_air = sensor_read(); // Simulado (usando generador aleatorio)

    // 3. Lectura DHT11 (Temp y Humedad)
    // dht11_read_data(&data->temperature, &data->humidity);
    data->temperature = 20.0 + (sensor_read() / 10.0); // Simulado rango 20-30C
    data->humidity = 40.0 + (sensor_read() / 5.0);     // Simulado rango 40-60%

    // 4. Lectura KY-026 (Fuego - Digital)
    // data->flame_detected = ky026_is_fire();
    // Simulemos que si el valor aleatorio es > 95, hay fuego
    double val = sensor_read();
    if (val > 95.0)
        data->flame_detected = 1;
    else
        data->flame_detected = 0;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv; // Evitar warnings

    struct mosquitto *mosq;
    int rc;

    // 1. Inicializar Hardware (Pigpio es necesario para los sensores reales)
    if (gpioInitialise() < 0)
    {
        fprintf(stderr, "Fallo al inicializar pigpio\n");
        return 1;
    }

    // Inicializar sensores (si tus librerías lo requieren)
    sensor_init();
    // mq2_init();   <-- Descomenta si tus headers tienen init
    // dht11_init();

    // 2. Inicializar MQTT
    mosquitto_lib_init();
    mosq = mosquitto_new("raspi_brain_full", true, NULL);
    mosquitto_username_pw_set(mosq, MQTT_USER, MQTT_PASS);

    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc != 0)
    {
        printf("Error MQTT: %s\n", mosquitto_strerror(rc));
        return 1;
    }
    printf("--- MONITOR AMBIENTAL AVANZADO INICIADO ---\n");

    mosquitto_loop_start(mosq);

    SystemData data;
    char payload_sensors[512]; // Aumentamos tamaño para caber todo el JSON
    char *system_state = "NORMAL";

    while (1)
    {
        // A. LEER TODOS LOS SENSORES
        read_all_sensors(&data);

        // B. LÓGICA DE NEGOCIO (EL CEREBRO)
        // Jerarquía de Alarmas: FUEGO > GAS/HUMO > CALIDAD AIRE/TEMP

        if (data.flame_detected == 1)
        {
            system_state = "ALARM"; // Prioridad máxima: FUEGO
            printf("[PELIGRO] ¡FUEGO DETECTADO! Activando sistema remoto.\n");
        }
        else if (data.mq2_gas > 80.0)
        {
            system_state = "ALARM"; // Prioridad alta: Fuga de gas o humo denso
            printf("[PELIGRO] Niveles criticos de Gas/Humo: %.2f\n", data.mq2_gas);
        }
        else if (data.mq135_air > 70.0 || data.temperature > 35.0)
        {
            system_state = "WARNING"; // Advertencia: Mala calidad aire o calor
            printf("[ATENCION] Ambiente degradado. Aire: %.2f | Temp: %.2f\n", data.mq135_air, data.temperature);
        }
        else
        {
            system_state = "NORMAL";
            printf("[OK] Parametros nominales.\n");
        }

        // C. PUBLICAR ESTADO (Para la ESP32)
        mosquitto_publish(mosq, NULL, TOPIC_STATE, strlen(system_state), system_state, 0, 0);

        // D. PUBLICAR TELEMETRÍA COMPLETA (Para la Web)
        // Construimos un JSON robusto
        sprintf(payload_sensors,
                "{"
                "\"mq2\": %.2f, "
                "\"mq135\": %.2f, "
                "\"temp\": %.2f, "
                "\"hum\": %.2f, "
                "\"fire\": %d, "
                "\"status\": \"%s\""
                "}",
                data.mq2_gas,
                data.mq135_air,
                data.temperature,
                data.humidity,
                data.flame_detected,
                system_state);

        mosquitto_publish(mosq, NULL, TOPIC_SENSORS, strlen(payload_sensors), payload_sensors, 0, 0);

        sleep(2); // Muestreo cada 2 segundos
    }

    mosquitto_lib_cleanup();
    gpioTerminate(); // Cerrar pigpio correctamente al salir
    return 0;
}