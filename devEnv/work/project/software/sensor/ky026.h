#ifndef KY026_H
#define KY026_H

typedef struct
{
    int flame_detected; // 1 si la salida digital indica flama
} KY026_Data;

void ky026_init(int gpio_pin, int active_high);
KY026_Data ky026_read(void);

#endif
