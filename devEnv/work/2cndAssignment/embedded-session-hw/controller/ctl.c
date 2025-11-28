#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pigpio.h>

#include "ctl.h"
#include "mqtt.h"

// Sensores
#include "sensors/dht11.h"
#include "sensors/mq2.h"
#include "sensors/mq135.h"
#include "sensors/ky026.h"
#include "sensors/adc.h"

// Actuadores (tu sistema)
#include "../actuators/actuator.h"

extern Actuator LED;
extern Actuator BUZZER;

void led_init(void);
void buzzer_init(void);

// ===========================
// Estructura general
// ===========================
typedef struct {
    struct timespec timestamp;
    double temperature;
    double humidity;

    double mq2_smoke;
    double mq2_lpg;
    double mq2_methane;

    double mq135_co2;
    double mq135_nh3;
    double mq135_co;
    double mq135_benzene;
    double mq135_alcohol;

    int flame_detected;
    double flame_level;

    int valid;
} Measurement;


// ===========================
// Obtener mediciones completas
// ===========================
Measurement read_all_sensors(void)
{
    Measurement m;
    clock_gettime(CLOCK_MONOTONIC, &m.timestamp);

    // ---- DHT11 ----
    DHT11_Data dht = dht11_read();
    if (!dht.valid) {
        m.valid = 0;
        return m;
    }
    m.temperature = dht.temp;
    m.humidity = dht.hum;

    // ---- MQ-2 ----
    MQ2_Data mq2 = mq2_read();
    m.mq2_smoke = mq2.ppm_smoke;
    m.mq2_lpg = mq2.ppm_lpg;
    m.mq2_methane = mq2.ppm_methane;

    // ---- MQ-135 ----
    MQ135_Data mq135 = mq135_read();
    m.mq135_co2     = mq135.co2;
    m.mq135_nh3     = mq135.nh3;
    m.mq135_co      = mq135.co;
    m.mq135_benzene = mq135.benzene;
    m.mq135_alcohol = mq135.alcohol;

    // ---- KY-026 ----
    KY026_Data fl = ky026_read();
    m.flame_detected = fl.flame_detected;
    m.flame_level = fl.flame_level;

    m.valid = 1;
    return m;
}


// ===========================
// Inicialización
// ===========================
void controller_init()
{
    printf("Inicializando ADC...\n");
    adc_init();

    printf("Inicializando sensores...\n");
    dht11_init(4);        // GPIO4 (BCM)
    mq2_init(0, 10.0);    // canal ADC 0, R0 calibrado
    mq135_init(1, 22.0);  // canal ADC 1, R0 calibrado
    ky026_init(2, 17);    // canal ADC 2, GPIO 17 digital

    printf("Inicializando actuadores...\n");
    led_init();
    buzzer_init();

    printf("Inicializando MQTT...\n");
    mqtt_init("192.168.1.120", 1883); // Cambia IP del broker
    mqtt_set_callback([](const char *topic, const char *msg){
        printf("Recibido desde MQTT: [%s] %s\n", topic, msg);
    });
}


// ===========================
// Control principal
// ===========================
void controller_loop()
{
    double temp_threshold = 40.0;   // ejemplo
    double smoke_threshold = 300.0; // ppm humo MQ2

    while (1)
    {
        Measurement m = read_all_sensors();
        if (!m.valid) {
            printf("Error en lectura de sensores.\n");
            sleep(1);
            continue;
        }

        // =====================
        // LOG LOCAL
        // =====================
        printf("\n======= MEDICIÓN =======\n");
        printf("Temp: %.1f°C  Hum: %.1f%%\n", m.temperature, m.humidity);

        printf("MQ-2 Smoke=%.2f ppm  LPG=%.2f ppm  CH4=%.2f ppm\n",
            m.mq2_smoke, m.mq2_lpg, m.mq2_methane);

        printf("MQ-135 CO2=%.2f ppm  NH3=%.2f ppm CO=%.2f ppm Bzn=%.2f Alc=%.2f\n",
            m.mq135_co2, m.mq135_nh3, m.mq135_co, m.mq135_benzene, m.mq135_alcohol);

        printf("Flama: %d  Nivel: %.2f\n",
            m.flame_detected, m.flame_level);


        // =====================
        // CONTROL DE ACTUADORES
        // =====================
        int danger = 0;

        if (m.temperature > temp_threshold) danger = 1;
        if (m.mq2_smoke > smoke_threshold)  danger = 1;
        if (m.flame_detected == 1)          danger = 1;

        if (danger)
        {
            LED.activate();
            BUZZER.activate();
            printf("→ ALERTA: ACTUADORES ACTIVADOS\n");
        }
        else
        {
            LED.deactivate();
            BUZZER.deactivate();
            printf("→ Estado normal\n");
        }


        // =====================
        // MQTT – envío a dashboard
        // =====================
        char json[512];
        snprintf(json, sizeof(json),
            "{"
              "\"temp\":%.2f,"
              "\"hum\":%.2f,"
              "\"mq2_smoke\":%.2f,"
              "\"mq2_lpg\":%.2f,"
              "\"mq2_methane\":%.2f,"
              "\"co2\":%.2f,"
              "\"nh3\":%.2f,"
              "\"co\":%.2f,"
              "\"benzene\":%.2f,"
              "\"alcohol\":%.2f,"
              "\"flame\":%d,"
              "\"flame_lvl\":%.2f"
            "}",
            m.temperature, m.humidity,
            m.mq2_smoke, m.mq2_lpg, m.mq2_methane,
            m.mq135_co2, m.mq135_nh3, m.mq135_co, m.mq135_benzene, m.mq135_alcohol,
            m.flame_detected, m.flame_level
        );

        mqtt_publish("sensores/casa", json);
        mqtt_loop(); // procesa callbacks si las hay

        sleep(2);
    }
}
