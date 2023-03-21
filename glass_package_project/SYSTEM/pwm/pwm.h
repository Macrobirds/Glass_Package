#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x_conf.h"

#define PSC_FEQ 72

void TIM1_PWM_Init(u16 psc);
void TIM4_PWM_Init(u16 psc);
void TIM3_PWM_Init(u16 psc);
void TIM8_PWM_Init(u16 psc);
void TIM5_PWM_Init(u16 psc);
void TIM2_PWM_Init(u16 psc);

void Motor_PWM_Init(void);

#endif