#ifndef dth11_H
#define dth11_H

typedef struct {
    double temp;
    double hum;
    int valid;
} DHT11_Data;

void dht11_init(int gpio_pin);
DHT11_Data dht11_read();

#endif
