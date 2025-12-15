#include "sensor.h"
#include "dht11.h"
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Definimos un máximo de ciclos para evitar bucles infinitos
#define MAX_TIMINGS 85

static int dht_pin = 16;

void dht11_init(int gpio_pin)
{
    dht_pin = gpio_pin;
    // Inicialización básica, aunque la mayor parte se hace en read()
    // Es importante que en ctl.c ya se haya llamado a gpioInitialise()
}

DHT11_Data dht11_read(void)
{
    DHT11_Data result = {0};
    int data[5] = {0, 0, 0, 0, 0};
    int laststate = 1; // Asumimos que empieza en HIGH por el Pull-Up
    int counter = 0;
    int j = 0, i;

    result.valid = 0; // Por defecto inválido

    // 1. SEÑAL DE INICIO (Host to Sensor)
    // Ponemos el pin en salida y bajamos la línea al menos 18ms
    gpioSetMode(dht_pin, PI_OUTPUT);
    gpioWrite(dht_pin, 0);
    gpioDelay(18000); // 18 milisegundos

    // Subimos la línea y esperamos 40us
    gpioWrite(dht_pin, 1);
    gpioDelay(40); // 40 microsegundos

    // Cambiamos a entrada para escuchar al sensor
    gpioSetMode(dht_pin, PI_INPUT);
    // IMPORTANTE: DHT11 requiere Pull-Up. Si tu módulo lo tiene, PI_PUD_OFF.
    // Si es el sensor pelado, PI_PUD_UP. Por seguridad usamos UP.
    gpioSetPullUpDown(dht_pin, PI_PUD_UP);

    // 2. LECTURA DE DATOS (Sensor to Host)
    // Detectamos los 85 cambios de estado (aprox) que envía el sensor
    for (i = 0; i < MAX_TIMINGS; i++)
    {
        counter = 0;
        // Bucle "Tight Loop": Contamos ciclos de CPU mientras el estado no cambie
        // NO USAR gpioDelay AQUÍ
        while (gpioRead(dht_pin) == laststate)
        {
            counter++;
            gpioDelay(1); // Pequeño delay de 1us es aceptable con pigpio si el contador es pequeño,
                          // pero idealmente confiamos en la velocidad de gpioRead.
            if (counter == 255)
            {
                break; // Timeout, el sensor no respondió
            }
        }
        laststate = gpioRead(dht_pin);

        if (counter == 255)
            break; // Si hubo timeout, salimos

        // Los primeros 3 cambios son la respuesta de presencia del sensor, los ignoramos.
        // A partir de ahí, cada bit son 2 transiciones (Bajada -> Subida).
        // Procesamos solo cuando 'i' es par y mayor o igual a 4.
        if ((i >= 4) && (i % 2 == 0))
        {
            // Shifteamos el bit 0 al registro
            data[j / 8] <<= 1;

            // Lógica del DHT11:
            // Si el estado ALTO duró poco (counter bajo) es un 0.
            // Si el estado ALTO duró mucho (counter alto) es un 1.
            // El umbral empírico suele ser un contador > 16 (depende de la velocidad de la CPU)
            // Con pigpio y gpioDelay(1), un contador > 16 suele indicar un "1".
            if (counter > 16)
            {
                data[j / 8] |= 1;
            }
            j++;
        }
    }

    // 3. VERIFICACIÓN (Checksum)
    // Debemos haber leído 40 bits (5 bytes)
    // Checksum: data[0] + data[1] + data[2] + data[3] == data[4]

    // 3. VERIFICACIÓN (Checksum)
    if ((j >= 40) &&
        (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
    {
        result.hum = (float)data[0];    // Byte 0: Humedad Entera
        result.temp_c = (float)data[2]; // Byte 2: Temperatura Entera
        result.valid = 1;

        // --- AGREGA ESTO PARA VERIFICAR ---
        printf("[DEBUG DHT11] Raw: Hum=%d.%d%% Temp=%d.%dC Checksum=%d\n",
               data[0], data[1], data[2], data[3], data[4]);
        // ----------------------------------
    }
    else
    {
        result.valid = 0;
        // Opcional: ver por qué falló
        // printf("[DEBUG DHT11] Fallo Checksum o Timeout. Bits leidos: %d\n", j);
    }

    return result;
}