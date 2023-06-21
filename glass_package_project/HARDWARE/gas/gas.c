#include "gas.h"


#define PRESSURE_MAX 320
#define PRESSURE_MIN 250

enum gas_state Gas_State=gas_pumping; //����·״̬

void Gas_Init(void)
{
	P6847_adc_init();
	ITV0011_IIC_Init();
	//�������ó���
	Main_Pump=GAS_ENABLE;
	Main_in_Cyl=GAS_ENABLE;
	Main_out_Cyl=GAS_DISABLE;
#ifdef BIG_CYLINDER
	GP_big_Cyl=GAS_DISABLE; //�������Ƴ�
#endif
	GP_small_Cyl=GAS_DISABLE; //С�����ƻ�
	GP_sucker1_Cyl=GAS_DISABLE;
	GP_sucker2_Cyl=GAS_DISABLE;
	GP_sucker_Pump=SUCKER_DISABLE;//GP_sucker_Pump=GAS_DISABLE;
	GP_spray_Cyl=GAS_DISABLE;
	GC_claw_Cyl=GAS_DISABLE;
}

//�罺ѹ������
void Gas_Spray_Enable(void)
{
	ITV0011_IIC_Enable(GP.spray_pressure);
	GP_spray_Cyl=GAS_ENABLE; //�����罺��
}

//�罺ѹ���ر�
void Gas_Spray_Disable(void)
{
	GP_spray_Cyl=GAS_DISABLE; //�ر��罺��
	ITV0011_IIC_Disable();
}


//����������ά����ѹ�ȶ� �����罺������ѹ
enum gas_state Gas_pump_Func(void)
{
	static float Pressure=0;
	Pressure=P6847_adc_GetPressure();
	//printf("pressure %f",Pressure);
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
//	 ITV0011_IIC_Enable(ITV_value);
	return Gas_State;
}

//������· ���й������
enum gas_state Gas_release_Func(void)
{
		float Pressure=0;
	//�ر�������������
	#ifdef BIG_CYLINDER
		GP_big_Cyl=GAS_ENABLE;
	#endif
		GP_small_Cyl=GAS_ENABLE;
		GP_sucker1_Cyl=GAS_DISABLE;
		GP_sucker2_Cyl=GAS_DISABLE;
		GP_sucker_Pump=SUCKER_DISABLE;//GP_sucker_Pump=GAS_DISABLE;
		GP_spray_Cyl=GAS_DISABLE;
		GC_claw_Cyl=GAS_DISABLE;
		Main_Pump=GAS_DISABLE;
		Gas_Spray_Disable();
		Pressure=P6847_adc_GetPressure();
		if(Pressure<10)
		{
				Main_out_Cyl=GAS_DISABLE;
				return gas_released;
		}else if(Pressure>50)
		{
			Main_Pump=GAS_DISABLE;
			Main_out_Cyl=GAS_ENABLE;
			return gas_releasing;
		}
		
}