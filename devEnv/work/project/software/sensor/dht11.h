#ifndef DHT11_H
#define DHT11_H

typedef struct
{
    double temp_c;
    double hum;
    int valid;
} DHT11_Data;

void dht11_init(int gpio_pin);
DHT11_Data dht11_read(void);

#endif
