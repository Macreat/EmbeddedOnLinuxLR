#ifndef MQ135_H
#define MQ135_H

typedef struct {
    float rs_ro;
    float ppm;
} MQ135_Data;

void mq135_init(int adc_channel, float ro_value);
void mq135_read_publish();

#endif
