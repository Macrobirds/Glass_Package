#ifndef __P6847_H
#define __P6847_H
#include "myiic.h"
#define P6847_ADDR  (0x6D << 1)

void P6847_Init(void);
float P6847_GetPressure(void);

void P6847_adc_init(void);
float P6847_adc_GetPressure(void);

#endif