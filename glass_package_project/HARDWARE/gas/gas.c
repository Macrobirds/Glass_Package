#include "gas.h"


#define PRESSURE_MAX 320
#define PRESSURE_MIN 300

void Gas_Init(void)
{
	P6847_adc_init();
	ITV0011_IIC_Init();
	//�������ó���
	Main_Pump=GAS_ENABLE;
	Main_in_Cyl=GAS_ENABLE;
	Main_out_Cyl=GAS_DISABLE;
	
	GP_big_Cyl=GAS_ENABLE; //�������Ƴ�
	GP_small_Cyl=GAS_DISABLE; //С�����ƻ�
	GP_sucker1_Cyl=GAS_DISABLE;
	GP_sucker2_Cyl=GAS_DISABLE;
	GP_sucker_Pump=GAS_DISABLE;
	GP_spray_Cyl=GAS_DISABLE;
	GC_claw_Cyl=GAS_DISABLE;
	
}
//����������ά����ѹ�ȶ� �����罺������ѹ
enum gas_state Gas_pump_Func(u8 ITV_value)
{
	static float Pressure=0;
	Pressure=P6847_adc_GetPressure();
	//�ﵽ�����ѹ�ر�����
	if(Pressure>PRESSURE_MAX)
	{
		Main_in_Cyl=GAS_DISABLE;
		Main_Pump=GAS_DISABLE;
		return gas_pumped;
	}
	//�ﵽ������ѹ������
	if(Pressure<PRESSURE_MIN)
	{
		Main_in_Cyl=GAS_ENABLE;
		Main_Pump=GAS_ENABLE;
		return gas_pumping;
	}
	 ITV0011_IIC_Enable(ITV_value);
}


enum gas_state Gas_release_Func(void)
{
		float Pressure=0;
	//�ر�������������
		GP_big_Cyl=GAS_DISABLE;
		GP_small_Cyl=GAS_ENABLE;
		GP_sucker1_Cyl=GAS_DISABLE;
		GP_sucker2_Cyl=GAS_DISABLE;
		GP_sucker_Pump=GAS_DISABLE;
		GP_spray_Cyl=GAS_DISABLE;
		GC_claw_Cyl=GAS_DISABLE;
	
		Pressure=P6847_adc_GetPressure();
		if(Pressure<10)
		{
				Main_out_Cyl=GAS_DISABLE;
				return gas_released;
		}else
		{
			Main_out_Cyl=GAS_ENABLE;
			return gas_releasing;
		}
		
}