#include "p6847.h"
#include "adc.h"
#include "usart.h"

void P6847_Init(void)
{
	u8 temp;
	//��ʼ��i2c
	IIC_Init();
	temp=IIC_ReadOneByte(P6847_ADDR,0xa5);//Read ASIC Sys_config����ȡϵͳ����ֵ)
	temp&=0xfd;//��Raw_data_on: 0: output calibrated data�����ΪУ׼���ֵ
	IIC_WriteOneByte(P6847_ADDR,0xa5,temp);//Set ADC output calibrated Data
	IIC_WriteOneByte(P6847_ADDR,0x30,0x0a);//indicate a combined conversion
	while((IIC_ReadOneByte(P6847_ADDR,0x30)&0x08)>0);
}

float P6847_GetPressure(void)
{
	u8 yali[3]={0};
	int32_t ad=0;
	u8 temp;
	temp=IIC_ReadOneByte(P6847_ADDR,0xa5);//Read ASIC Sys_config����ȡϵͳ����ֵ)
	temp&=0xfd;//��Raw_data_on: 0: output calibrated data�����ΪУ׼���ֵ
	IIC_WriteOneByte(P6847_ADDR,0xa5,temp);//Set ADC output calibrated Data
	IIC_WriteOneByte(P6847_ADDR,0x30,0x0a);//indicate a combined conversion
	while((IIC_ReadOneByte(P6847_ADDR,0x30)&0x08)>0);
	yali[0]=IIC_ReadOneByte(P6847_ADDR,0x06);
	yali[1]=IIC_ReadOneByte(P6847_ADDR,0x07);
	yali[2]=IIC_ReadOneByte(P6847_ADDR,0x08);
	 ad = yali[0] * 65536 + yali[1] * 256+ yali[2];
			if (ad > 8388608) //���� 8388606 Ϊ��ѹֵ��������ʾ�ն�������
      {
         return ((ad - 16777216)/8192.0*-1)*2; //��λΪ pa  
      }
     else 
      {
        return (ad/8192.0)*2; //��λΪ pa
      }
	
}

//ADC2��ʼ��  ͨ��ADC2��ȡ��ѹ��ģ�����
void P6847_adc_init(void)
{
	adc2_init();
}

//ͨ��adc2��ȡ��ѹ��ѹ�� ���ص�λΪKpa 
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