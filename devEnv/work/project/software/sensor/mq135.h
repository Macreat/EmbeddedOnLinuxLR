#ifndef MQ135_H
#define MQ135_H

typedef struct
{
    int triggered; // 1 si el comparador indica gas
} MQ135_Data;

void mq135_init(int gpio_pin, int active_high);
MQ135_Data mq135_read(void);

#endif
