#ifndef MQTT_H
#define MQTT_H

// Inicializa el cliente MQTT
void mqtt_init(const char *host, int port);

// Publica un mensaje
void mqtt_publish(const char *topic, const char *payload);

// Mantiene el loop del cliente MQTT
void mqtt_loop();

// Permite registrar un callback opcional para mensajes entrantes
void mqtt_set_callback(void (*cb)(const char *, const char *));

#endif
