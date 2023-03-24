#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"

void adc1_init(void);
void adc2_init(void);
u16 adc1_getvalue(void);
u16 adc2_getvalue(void);



#endif