#ifndef __GAS_H
#define __GAS_H
#include "sys.h"
#include "PCF8591.h"
#include "p6847.h"
#include "ITV0011.h"
#include "Globalconfig.h"

enum gas_state{
	gas_pumping=0,//打气进行状态
	gas_pumped=1,//大气完成状态
	gas_releasing=2,//放气进行状态
	gas_released=3,//放气完成
};

extern enum gas_state Gas_State;

void Gas_Init(void);
enum gas_state Gas_pump_Func(u8 ITV_value);
enum gas_state Gas_release_Func(void);


#endif