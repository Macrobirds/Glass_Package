#include "p6847.h"
#include "adc.h"
#include "usart.h"

void P6847_Init(void)
{
	u8 temp;
	//初始化i2c
	IIC_Init();
	temp=IIC_ReadOneByte(P6847_ADDR,0xa5);//Read ASIC Sys_config（读取系统配置值)
	temp&=0xfd;//（Raw_data_on: 0: output calibrated data，输出为校准后的值
	IIC_WriteOneByte(P6847_ADDR,0xa5,temp);//Set ADC output calibrated Data
	IIC_WriteOneByte(P6847_ADDR,0x30,0x0a);//indicate a combined conversion
	while((IIC_ReadOneByte(P6847_ADDR,0x30)&0x08)>0);
}

float P6847_GetPressure(void)
{
	u8 yali[3]={0};
	int32_t ad=0;
	u8 temp;
	temp=IIC_ReadOneByte(P6847_ADDR,0xa5);//Read ASIC Sys_config（读取系统配置值)
	temp&=0xfd;//（Raw_data_on: 0: output calibrated data，输出为校准后的值
	IIC_WriteOneByte(P6847_ADDR,0xa5,temp);//Set ADC output calibrated Data
	IIC_WriteOneByte(P6847_ADDR,0x30,0x0a);//indicate a combined conversion
	while((IIC_ReadOneByte(P6847_ADDR,0x30)&0x08)>0);
	yali[0]=IIC_ReadOneByte(P6847_ADDR,0x06);
	yali[1]=IIC_ReadOneByte(P6847_ADDR,0x07);
	yali[2]=IIC_ReadOneByte(P6847_ADDR,0x08);
	 ad = yali[0] * 65536 + yali[1] * 256+ yali[2];
			if (ad > 8388608) //超过 8388606 为负压值，需在显示终端做处理
      {
         return ((ad - 16777216)/8192.0*-1)*2; //单位为 pa  
      }
     else 
      {
        return (ad/8192.0)*2; //单位为 pa
      }
	
}

//ADC2初始化  通过ADC2读取气压计模拟输出
void P6847_adc_init(void)
{
	adc2_init();
}

//通过adc2读取气压计压力 返回单位为Kpa 
float P6847_adc_GetPressure(void)
{
	int16_t adc_val=0;
	adc_val=adc2_getvalue();
	adc_val-=410;
	if(adc_val>0)
	{
	//	printf("Pressure %f kpa", (adc_val/4096.0*1.25*1000));
		return (adc_val/4096.0*1.25*1000);
	}
	return 0;
}