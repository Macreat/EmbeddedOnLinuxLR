#include "MQTT.h"
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct mosquitto *mosq = NULL;
static void (*message_callback)(const char *, const char *) = NULL;

// -------------------------
// Callback interno de Mosquitto
// -------------------------
static void on_message(struct mosquitto *mosq, void *userdata,
                       const struct mosquitto_message *msg)
{
    if (message_callback)
    {
        message_callback(msg->topic, (const char *)msg->payload);
    }
}

// -------------------------
// Inicialización
// -------------------------
void mqtt_init(const char *host, int port)
{
    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq)
    {
        fprintf(stderr, "Error creando cliente MQTT\n");
        return;
    }

    mosquitto_message_callback_set(mosq, on_message);

    int ret = mosquitto_connect(mosq, host, port, 60);
    if (ret != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Error al conectar MQTT: %s\n",
                mosquitto_strerror(ret));
    }
    else
    {
        printf("MQTT conectado a %s:%d\n", host, port);
    }
}

// -------------------------
// Publicación
// -------------------------
void mqtt_publish(const char *topic, const char *payload)
{
    if (!mosq)
        return;

    int ret = mosquitto_publish(mosq, NULL,
                                topic,
                                strlen(payload),
                                payload,
                                0, false);

    if (ret != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Error al publicar MQTT: %s\n",
                mosquitto_strerror(ret));
    }
}

// -------------------------
// Loop
// -------------------------
void mqtt_loop()
{
    if (mosq)
        mosquitto_loop(mosq, 10, 1);
}

// -------------------------
// Registrar callback
// -------------------------
void mqtt_set_callback(void (*cb)(const char *, const char *))
{
    message_callback = cb;
}
