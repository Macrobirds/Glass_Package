#ifndef __GPIO_H
#define __GPIO_H
#include "sys.h"
void LED_GPIO_Init(void);
void RELAY_GPIO_Init(void);
void Heater_Gpio_Init(void);
void Motor_Dir_GpioInit(void);
void IoInput_GpioInit(void);
void ALL_GPIO_Init(void);
#endif