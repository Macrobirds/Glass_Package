#include "gas.h"


#define PRESSURE_MAX 320
#define PRESSURE_MIN 250

enum gas_state Gas_State=gas_pumping; //主气路状态

void Gas_Init(void)
{
	P6847_adc_init();
	ITV0011_IIC_Init();
	//开启气泵充气
	Main_Pump=GAS_ENABLE;
	Main_in_Cyl=GAS_ENABLE;
	Main_out_Cyl=GAS_DISABLE;
	
	GP_big_Cyl=GAS_DISABLE; //大气缸推出
	GP_small_Cyl=GAS_DISABLE; //小气缸推回
	GP_sucker1_Cyl=GAS_DISABLE;
	GP_sucker2_Cyl=GAS_DISABLE;
	GP_sucker_Pump=GAS_DISABLE;
	GP_spray_Cyl=GAS_DISABLE;
	GC_claw_Cyl=GAS_DISABLE;
}

void Gas_Spray_Enable(void)
{
	ITV0011_IIC_Enable(GP.spray_pressure);
	GP_spray_Cyl=GAS_ENABLE;
}

void Gas_Spray_Disable(void)
{
	GP_spray_Cyl=GAS_DISABLE;
	ITV0011_IIC_Disable();
}


//控制主气泵维持气压稳定 控制喷胶气泵气压
enum gas_state Gas_pump_Func(void)
{
	static float Pressure=0;
	Pressure=P6847_adc_GetPressure();
	//printf("pressure %f",Pressure);
	//达到最大气压关闭气泵
	if(Pressure>PRESSURE_MAX)
	{
		Main_in_Cyl=GAS_DISABLE;
		delay_ms(100);
		Main_Pump=GAS_DISABLE;
		return gas_pumped;
	}
	//达到最下气压打开气泵
	if(Pressure<PRESSURE_MIN)
	{
		Main_in_Cyl=GAS_ENABLE;
		Main_Pump=GAS_ENABLE;
		return gas_pumping;
	}
//	 ITV0011_IIC_Enable(ITV_value);
	return 0;
}


enum gas_state Gas_release_Func(void)
{
		float Pressure=0;
	//关闭所有气缸气阀
		GP_big_Cyl=GAS_ENABLE;
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