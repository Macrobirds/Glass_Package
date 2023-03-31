#ifndef __UART3_DATAHANDLE_H
#define __UART3_DATAHANDLE_H
#include "sys.h" 
#include <stdbool.h>
/* *************************************** 函数声明 *********************************** */
void screenUart_protocolParse(void);
void ack(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 ackByte);
void ack_0data(u8 Index, u8 TYPE, u8 FC, u8 Extra);
void ack_2data(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 ackByte, u8 ackByte_2);
void ack_3data(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 ackByte, u8 ackByte_2, u8 ackByte_3);
void screenUart_ack_array(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 *data, u8 dataLength);
void sendProtocol_2data(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 data1, u8 data2);
void control_debug(u8 Index, u8 Extra);
void query_param_motor(u8 Index);
bool set_param_motor(u8* data, u8 dataLength);
bool set_param_printf(u8* data, u8 dataLength);
bool queryAck_data_1step(u8* data, u8 dataLength);
void query_param_printf(u8 Index);
void query_param_bak(u8 Index);
bool set_param_bak(u8* data, u8 dataLength);

//协议类型
enum Type
{
	Type_query = 1,
	Type_queryAck = 2,
	Type_set = 3,
	Type_setAck = 4,
	Type_control = 5,
	Type_controlAck = 6,
};

//功能码
enum Fc
{
	Fc_state = 1,
	Fc_param = 2,
	Fc_data = 3,
	Fc_debug = 4,
	Fc_run = 5,
	Fc_error = 6,
};

//状态
enum State
{
	Extra_state_online = 1,
	Extra_state_deviceState = 2,
	Extra_state_position = 3,
	Extra_state_gas = 4,
	Extra_state_sen=5,
};
//参数
enum Param
{
	Extra_param_motor = 1,
	Extra_param_deviceInfo = 2,
	Extra_param_serialNumber = 3,
	Extra_param_claw=4,
	Extra_param_package=5,
	Extra_param_outin=6,
};

//数据
enum Data
{
	Extra_data_package = 1,
	Extra_data_box = 2,
};

//调试
enum Debug
{
	Extra_debug_reset = 1,
	Extra_debug_GE=2,
	Extra_debug_GC=2,
	Extra_debug_GP=2,
	Extra_debug_GO=2,
	

};
//运行
enum Run
{
	Extra_run_Reset = 1,
	Extra_run_GEIn=2,
	Extra_run_GEOut=3,
	Extra_run_GOIn=4,
	Extra_run_GOOut=5,
	Extra_run
};

//警报
enum Error
{
	Extra_error_tempSensor = 1,
//	Extra_error_doorIsOpen = 2,
//	Extra_error_coverNotExist = 3,
};

// 开关状态
enum OnOffState
{
	OnOffState_off = 0,
	OnOffState_on = 1,
};

enum AckResult
{
	AckResult_ackSucceed = 0,
	AckResult_ackFailed = 1,
	AckResult_executeSucceed = 2,
	AckResult_executeFailed = 3,
};

// 执行失败失败原因
enum Reason
{
	Reason_deviceBusy = 0,
	Reason_timeOut = 1,
//	Reason_GSL_withoutBox = 2,
	Reason_XEG_withoutBox = 3,
};

#endif
