#include "ITV0011.h"
#include "delay.h"
#include "usart.h"
#include "myiic.h"
#include "PCF8591.h"
#include "Globalconfig.h"

#define TIME 5

void ITV0011_IIC_Init()
{
	GP_ITV1100=ITV1100_DISABLE;
	IIC_Init();
	PCF8591_DAC_Output(0x90,0x40,0);//DAC输出为0
}

void ITV0011_IIC_Enable(u8 volt)
{

	GP_ITV1100=ITV1100_ENABLE;
	PCF8591_DAC_Output(0x90,0x40,volt);
}
void ITV0011_IIC_Disable()
{

	GP_ITV1100=ITV1100_DISABLE;
	PCF8591_DAC_Output(0x90,0x40,0);
}
u32 ITV0011_IIC_GetPressure()
{	
	u8 i=0;
	int32_t sum=0;
	for(i=0;i<TIME;i++)
	{
		sum+=PCF8591_ADC_Input(0x90 ,0x41);
		delay_ms(50);
	}
	sum/=TIME;
	if(sum-53<0)
	{
			return 0;
	}else
	{
		 return (sum-53)/204*90+10;
	}
}


/*
void ITV0011_Init()
{
	DAC1_Init();
	DAC_SetVolt(0);
	adc1_init();
}
void ITV0011_Enable()
{
	DAC_SetVolt(3000);
	printf("enable\r\n");
	
}
void ITV0011_Disable()
{
	DAC_SetVolt(0);
	printf("disable\r\n");
}

//返回单位为Kpa

u32 ITV0011_GetPressure()
{
	u8 i=0;
	int32_t sum=0;
	for(i=0;i<TIME;i++)
	{
		sum+=adc1_getvalue();
		delay_ms(5);
	}
	sum/=TIME;
	printf("adc value %d",sum);
	if(sum-850<0)
	{
		return 0;
	}else
	{
		printf("pressure%d kpa",(u32)((sum-850)/3276.0*90+10));
		 return (sum-819)/3276.0*90+10;
	}
}
*/


