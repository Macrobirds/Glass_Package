#ifndef __DAC_H
#define __DAC_H
#include "sys.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dac.h"

void DAC1_Init(void);
void DAC_SetVolt(u16 vol);




#endif