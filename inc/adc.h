#ifndef APP_ADC_H
#define APP_ADC_H

#include <stdint.h>

int adc_init(void);
int adc_sample(int16_t *value);
void adc_start_periodic_sampling(void);

#endif