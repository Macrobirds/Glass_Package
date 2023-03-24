#ifndef __PCF8591_H
#define __PCF8591_H

#include "myiic.h"   

#define PCF8591Addr_W 0x90	//PCF8591的器件地址和写操作
#define PCF8591Addr_R 0x91

void PCF8591_Init(void); //初始化IIC

u8 PCF8591_ReadOneByte(u8 ReadAddr);							//指定地址读取一个字节
void PCF8591_WriteOneByte(u8 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节

u8 PCF8591_DAC_Output(unsigned char WriteAddr,unsigned char c,  unsigned char Val);
u8 PCF8591_ADC_Input(unsigned char WriteAddr,unsigned char Channel);

#endif

