#ifndef KY026_H
#define KY026_H

typedef struct {
    int flame_detected;
    double flame_level;
} KY026_Data;

void ky026_init(int adc_ch, int digital_pin);
KY026_Data ky026_read();

#endif

