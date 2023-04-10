#ifndef __ITV0011_H
#define __ITV0011_H
//#include "dac.h"
#include "adc.h"

//void ITV0011_Init();
//void ITV0011_Enable();
//void ITV0011_Disable();
//u32 ITV0011_GetPressure();

void ITV0011_IIC_Init(void);
void ITV0011_IIC_Enable(u8 volt);
void ITV0011_IIC_Disable(void);
u32 ITV0011_IIC_GetPressure(void);


#endif