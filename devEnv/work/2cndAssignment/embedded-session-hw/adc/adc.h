#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_init();
int adc_read_channel(uint8_t channel);

#endif
