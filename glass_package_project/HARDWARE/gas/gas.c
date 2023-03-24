#include "gas.h"


#define PRESSURE_MAX 320
#define PRESSURE_MIN 300

void Gas_Init(void)
{
	P6847_adc_init();
	ITV0011_IIC_Init();
	//开启气泵充气
	Main_Pump=GAS_ENABLE;
	Main_in_Cyl=GAS_ENABLE;
	
	GP_big_Cyl=GAS_DISABLE;
	GP_small_Cyl=GAS_DISABLE;
	GP_sucker1_Cyl=GAS_DISABLE;
	GP_sucker2_Cyl=GAS_DISABLE;
	GP_sucker_Pump=GAS_DISABLE;
	GP_spray_Cyl=GAS_DISABLE;
	GC_claw_Cyl=GAS_DISABLE;
	Main_out_Cyl=GAS_DISABLE;
}
//控制主气泵维持气压稳定 控制喷胶气泵气压
void Gas_lmaintain_Func(void)
{
	float Pressure=0;
	Pressure=P6847_adc_GetPressure();
	//达到最大气压关闭气泵
	if(Pressure>PRESSURE_MAX)
	{
		Main_in_Cyl=GAS_DISABLE;
		Main_Pump=GAS_DISABLE;
	}
	//达到最下气压打开气泵
	if(Pressure<PRESSURE_MIN)
	{
		Main_in_Cyl=GAS_ENABLE;
		Main_Pump=GAS_ENABLE;
	}
	

	 
	 ITV0011_IIC_Enable(50);
		
	
		delay_ms(500);
}


void Gas_release_Func(void)
{
		float Pressure=0;
		 Main_Pump=GAS_DISABLE;
		 Main_in_Cyl=GAS_DISABLE;
		 Main_out_Cyl=GAS_ENABLE;
		
		while(1)
		{
			Pressure=P6847_adc_GetPressure();
			if(Pressure<10)
			{
				break;
			}
			delay_ms(500);
		}
		Main_out_Cyl=GAS_DISABLE;
}