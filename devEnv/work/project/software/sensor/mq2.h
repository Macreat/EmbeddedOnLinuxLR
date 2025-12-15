#ifndef MQ2_H
#define MQ2_H

typedef struct
{
    int triggered; // 1 si el comparador indica gas/humo
} MQ2_Data;

void mq2_init(int gpio_pin, int active_high); // active_high=0 si el módulo saca LOW al detectar
MQ2_Data mq2_read(void);

#endif
