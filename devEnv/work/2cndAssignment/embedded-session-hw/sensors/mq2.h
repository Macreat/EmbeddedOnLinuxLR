#ifndef MQ2_H
#define MQ2_H

typedef struct {
    double rs;
    double ratio;
    double ppm_smoke;
    double ppm_lpg;
    double ppm_methane;
} MQ2_Data;

void mq2_init(int adc_channel, double r0);
MQ2_Data mq2_read();

#endif
