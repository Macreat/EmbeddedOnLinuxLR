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
#define PIN_DHT11 16
#define PIN_MQ2 22
#define PIN_MQ135 23
#define PIN_KY026 27

// --- CONFIGURACIÓN MQTT AWS ---
#define MQTT_HOST "3.134.86.43"
#define MQTT_PORT 1883
#define MQTT_USER "esp32"
#define MQTT_PASS "12345678"

#define TOPIC_STATE "sistema/estado"
#define TOPIC_SENSORS "sistema/sensores"

// --- ESTRUCTURA DE DATOS ---
typedef struct
{
    double mq2_gas;   // 0.0 = Limpio, 1.0 = Detectado
    double mq135_air; // 0.0 = Limpio, 1.0 = Detectado
    double temperature;
    double humidity;
    int flame_detected; // 0 = No, 1 = Si
} SystemData;

// --- FUNCIÓN DE LECTURA DE SENSORES CORREGIDA ---
void read_all_sensors(SystemData *data)
{
    // 1. Lectura MQ-2 (Devuelve struct MQ2_Data)
    MQ2_Data mq2_result = mq2_read();
    // Convertimos el digital (0 o 1) a double para la gráfica (simulamos PPM: 10 o 90)
    data->mq2_gas = mq2_result.triggered ? 90.0 : 10.0;

    // 2. Lectura MQ-135 (Devuelve struct MQ135_Data)
    MQ135_Data mq135_result = mq135_read();
    data->mq135_air = mq135_result.triggered ? 85.0 : 15.0;

    // 3. Lectura DHT11 (Devuelve struct DHT11_Data)
    DHT11_Data dht_result = dht11_read();

    // Validamos si la lectura fue correcta
    if (dht_result.valid)
    {
        data->temperature = (double)dht_result.temp_c;
        data->humidity = (double)dht_result.hum;
    }
    else
    {
        // Si falla, mantenemos el valor anterior o ponemos error
        // Para este ejemplo, no actualizamos si falla
        printf("[Sensor] Error leyendo DHT11\n");
    }

    // 4. Lectura KY-026 (Devuelve struct KY026_Data)
    KY026_Data fire_result = ky026_read();
    data->flame_detected = fire_result.flame_detected;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    struct mosquitto *mosq;
    int rc;

    // 1. Inicializar Hardware
    if (gpioInitialise() < 0)
    {
        fprintf(stderr, "Fallo al inicializar pigpio\n");
        return 1;
    }

    // --- INICIALIZACIÓN CORRECTA DE SENSORES ---s
    // Pasamos el PIN y el modo (1=Active High, 0=Active Low)

    mq2_init(PIN_MQ2, 0); // MQ2 suele dar 1 al detectar humo (ajustar si es al reves)
    mq135_init(PIN_MQ135, 0);
    dht11_init(PIN_DHT11);
    ky026_init(PIN_KY026, 0); // KY-026 suele dar 0 (LOW) cuando detecta fuego, por eso puse 0

    // 2. Inicializar MQTT
    mosquitto_lib_init();
    mosq = mosquitto_new("raspi_brain_real", true, NULL);
    mosquitto_username_pw_set(mosq, MQTT_USER, MQTT_PASS);

    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc != 0)
    {
        printf("Error MQTT: %s\n", mosquitto_strerror(rc));
        return 1;
    }
    printf("--- MONITOR AMBIENTAL: PINES %d, %d, %d, %d ---\n",
           PIN_DHT11, PIN_MQ2, PIN_MQ135, PIN_KY026);

    mosquitto_loop_start(mosq);

    SystemData data = {0}; // Inicializar a ceros
    char payload_sensors[512];
    char *system_state = "NORMAL";

    while (1)
    {
        read_all_sensors(&data);

        // --- LÓGICA CORREGIDA ---
        // Usamos '&&' para AND lógico, no '&'
        // Usamos 'data.temperature', no 'temperature' sola

        if (data.flame_detected == 1 || data.temperature > 30.0)
        {
            system_state = "ALARM";
            printf("[PELIGRO] ¡FUEGO O CALOR EXTREMO! Temp: %.1f\n", data.temperature);
        }
        else if (data.mq2_gas > 50.0) // Recordar que simulamos >50 como detectado
        {
            system_state = "ALARM";
            printf("[PELIGRO] GAS DETECTADO.\n");
        }
        else if (data.mq135_air > 50.0 || data.temperature > 35.0)
        {
            system_state = "WARNING";
            printf("[ATENCION] Calidad aire mala o calor. Temp: %.1f\n", data.temperature);
        }
        else
        {
            system_state = "NORMAL";
            printf("[OK] T:%.1f H:%.1f Gas:%d Aire:%d Fuego:%d\n",
                   data.temperature, data.humidity,
                   (int)data.mq2_gas, (int)data.mq135_air, data.flame_detected);
        }

        // Publicar
        mosquitto_publish(mosq, NULL, TOPIC_STATE, strlen(system_state), system_state, 0, 0);

        sprintf(payload_sensors,
                "{"
                "\"mq2\": %.1f, "
                "\"mq135\": %.1f, "
                "\"temp\": %.1f, "
                "\"hum\": %.1f, "
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

        sleep(2);
    }

    mosquitto_lib_cleanup();
    gpioTerminate();
    return 0;
}