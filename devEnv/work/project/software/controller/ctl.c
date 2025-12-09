#define _POSIX_C_SOURCE 199309L // enable POSIX time functions

#include <stdio.h>
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

//
// Ajusta a tu cableado (BCM)
#define PIN_MQ2 5
#define PIN_MQ135 6
#define PIN_FLAME 13
#define PIN_DHT11 17

#define PIN_LED_R 22
#define PIN_LED_G 27
#define PIN_LED_B 24

#define PIN_BUZZER 18
#define PIN_GATE1 23
#define PIN_GATE2 25

static struct mosquitto *mq = NULL;

typedef struct
{
    int gas_mq2;
    int gas_mq135;
    int flame;
    double temp_c;
    double hum;
    int dht_ok;
} Measurements;

static void actuators_set_normal(void)
{
    led_actuator_set(LED_GREEN);
    gpioWrite(PIN_BUZZER, 0);
    gpioWrite(PIN_GATE1, 0);
    gpioWrite(PIN_GATE2, 0);
}

static void actuators_set_warning(void)
{
    led_actuator_set(LED_BLUE);
    gpioWrite(PIN_BUZZER, 1);
    gpioWrite(PIN_GATE1, 1);
    gpioWrite(PIN_GATE2, 0);
}

static void actuators_set_alarm(void)
{
    led_actuator_set(LED_RED);
    gpioWrite(PIN_BUZZER, 1);
    gpioWrite(PIN_GATE1, 1);
    gpioWrite(PIN_GATE2, 1);
}

void controller_init(void)
{
    if (gpioInitialise() < 0)
    {
        perror("pigpio init failed");
        return;
    }

    mq2_init(PIN_MQ2, 0); // 0 si el módulo entrega LOW al detectar
    mq135_init(PIN_MQ135, 0);
    ky026_init(PIN_FLAME, 0); // muchos KY-026 son LOW al detectar
    dht11_init(PIN_DHT11);

    led_actuator_init(PIN_LED_R, PIN_LED_G, PIN_LED_B, 1); // 1 para cátodo común
    gpioSetMode(PIN_BUZZER, PI_OUTPUT);
    gpioSetMode(PIN_GATE1, PI_OUTPUT);
    gpioSetMode(PIN_GATE2, PI_OUTPUT);

    actuators_set_normal();
}

static Measurements read_all_sensors(void)
{
    Measurements m = {0};

    MQ2_Data mq2 = mq2_read();
    MQ135_Data mq135 = mq135_read();
    KY026_Data fl = ky026_read();
    DHT11_Data dht = dht11_read();

    m.gas_mq2 = mq2.triggered;
    m.gas_mq135 = mq135.triggered;
    m.flame = fl.flame_detected;
    m.dht_ok = dht.valid;
    m.temp_c = dht.temp_c;
    m.hum = dht.hum;
    return m;
}

static void apply_logic(const Measurements *m)
{
    int gas = m->gas_mq2 || m->gas_mq135;
    int heat = m->dht_ok && (m->temp_c >= 35.0 || m->hum >= 80.0);
    if (m->flame || (gas && heat))
    {
        actuators_set_alarm(); // Rojo: flama o gas+calor
    }
    else if (gas || heat)
    {
        actuators_set_warning(); // Azul: gas o calor
    }
    else
    {
        actuators_set_normal(); // Verde
    }
}

// for monitor actuators
static const char *logic_color(const Measurements *m)
{
    int gas = m->gas_mq2 || m->gas_mq135;
    int heat = m->dht_ok && (m->temp_c >= 35.0 || m->hum >= 80.0);
    if (m->flame || (gas && heat))
        return "RED";
    if (gas || heat)
        return "BLUE";
    return "GREEN";
}
void controller_loop(void)
{
    for (;;)
    {
        Measurements m = read_all_sensors();
        apply_logic(&m);
        const char *color = logic_color(&m);
        printf("MQ2=%d MQ135=%d FLAME=%d | ", m.gas_mq2, m.gas_mq135, m.flame);
        if (m.dht_ok)
            printf("T=%.1fC H=%.1f%% | ", m.temp_c, m.hum);
        else
            printf("DHT11 invalid | ");

        printf("LED=%s BUZZER=%d GATE1=%d GATE2=%d\n",
               color,
               gpioRead(PIN_BUZZER),
               gpioRead(PIN_GATE1),
               gpioRead(PIN_GATE2));

        gpioDelay(5000000); // 500 ms
    }
}

int main(void)
{
    controller_init();
    controller_loop(); // bucle infinito dentro
    return 0;
}
