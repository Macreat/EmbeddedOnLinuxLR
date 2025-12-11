#define _POSIX_C_SOURCE 199309L // enable POSIX time functions

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <unistd.h>
#include <pigpio.h>

// PROCEEd with communication protocol
#include <mosquitto.h>
#include <string.h>

#include "../sensor/sensor.h"
#include "../sensor/mq2.h"

#include "../sensor/mq135.h"

#include "../sensor/dht11.h"

#include "../sensor/ky026.h"

#include "../actuators/actuator.h"

#include "../actuators/led_actuator.h"

// --- CONFIGURACIÓN MQTT AWS ---
#define MQTT_HOST "3.134.86.43"
#define MQTT_PORT 1883
#define MQTT_USER "raspi"
#define MQTT_PASS "12345678"

#define TOPIC_STATE "sistema/estado"     // Payload: ALARM, WARNING, NORMAL
#define TOPIC_SENSORS "sistema/sensores" // Payload: JSON completo

// --- ESTRUCTURA DE DATOS ---
typedef struct
{
    double gas_mq2;
    double temp;
    // Puedes agregar más aquí si tu sensor.h lo soporta
} SystemData;

// Función wrapper para leer tus sensores
void read_all_sensors(SystemData *data)
{
    // LLAMADA A TUS FUNCIONES REALES DE SENSOR.H
    // Si tu sensor_read() devuelve un solo valor, ajusta esto.
    // Aquí asumo que sensor_read devuelve un double genérico por ahora.

    data->gas_mq2 = sensor_read(); // Lee sensor 1
    data->temp = sensor_read();    // Lee sensor 2 (Simulado o real)

    // NOTA: Si sensor_read() es random en tu implementación actual,
    // los valores cambiarán en cada llamada.
}

int main(int argc, char *argv[])
{
    // 1. Corrección de Warnings: Indicar que no usamos argumentos de consola
    (void)argc;
    (void)argv;

    struct mosquitto *mosq;
    int rc;

    // 2. Inicializar Sensores (Función de tu sensor.h)
    // Si sensor.h NO tiene sensor_init(), borra esta línea.
    sensor_init();

    // --- ELIMINADO: buzzer_init() ---
    // Ya no iniciamos buzzer local porque la ESP32 es la alarma remota.

    // 3. Inicializar MQTT
    mosquitto_lib_init();
    mosq = mosquitto_new("raspi_brain_v4", true, NULL);

    // Autenticación
    mosquitto_username_pw_set(mosq, MQTT_USER, MQTT_PASS);

    // Conectar
    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc != 0)
    {
        printf("Error conectando al broker: %s\n", mosquitto_strerror(rc));
        return 1;
    }
    printf("--- Raspi Cerebro Iniciado ---\n");
    printf("Modo: Lectura de sensores -> Publicación AWS\n");

    // Iniciar loop de MQTT en segundo plano
    mosquitto_loop_start(mosq);

    SystemData currentData;
    char payload_sensors[256];

    // Corrección Warning: 'last_state' no se usaba, ahora usamos 'system_state'
    char *system_state = "NORMAL";

    while (1)
    {
        // A. LEER ENTORNO
        read_all_sensors(&currentData);

        // B. PROCESAR Y DECIDIR (Lógica de Negocio)
        // Ejemplo: Gas mayor a 80 O Temperatura mayor a 50
        if (currentData.gas_mq2 > 80.0 || currentData.temp > 50.0)
        {
            system_state = "ALARM";
            printf("[ALERTA] Valores criticos detectados. Gas: %.2f\n", currentData.gas_mq2);
        }
        else if (currentData.gas_mq2 > 40.0)
        {
            system_state = "WARNING";
            printf("[PRECAUCION] Niveles elevados. Gas: %.2f\n", currentData.gas_mq2);
        }
        else
        {
            system_state = "NORMAL";
            printf("[OK] Sistema estable. Gas: %.2f\n", currentData.gas_mq2);
        }

        // C. PUBLICAR ESTADO (Para que la ESP32 remota encienda el LED)
        mosquitto_publish(mosq, NULL, TOPIC_STATE, strlen(system_state), system_state, 0, 0);

        // D. PUBLICAR DATOS (Para que la Web grafique)
        sprintf(payload_sensors,
                "{\"mq2\": %.2f, \"temp\": %.2f, \"status\": \"%s\"}",
                currentData.gas_mq2, currentData.temp, system_state);

        mosquitto_publish(mosq, NULL, TOPIC_SENSORS, strlen(payload_sensors), payload_sensors, 0, 0);

        // Esperar 2 segundos antes de la siguiente lectura
        sleep(2);
    }

    mosquitto_lib_cleanup();
    return 0;
}