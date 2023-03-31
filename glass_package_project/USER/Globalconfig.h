#ifndef __GLOBALCONFIG_H
#define __GLOBALCONFIG_H
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "position.h"
#include "pwm.h"
#include "gpio.h"
#include "gas.h"
#include "motor.h"
#include "malloc.h"
#include "RingBuffer.h"
#include "taskthread.h"
#include "includes.h"
#include "codeGuard.h"
#include "rtc.h"
#include "w25qxx.h"


//SPI Flash 参数
#define SpiFlashAddr_initFlag 				0x0000			// 首次初始化标志位
#define SpiFlashAddr_DeviceType 				0x0001			// 设备类型
#define SpiFlashAddr_motorPositionData 		0x0100			// 储存电机脉冲参数
#define SpiFlashAddr_hardwareState 			0x0600			// 储存开关状态
#define SpiFlashAddr_printfParam 			0x0700			// 储存打印参数
#define FlashSIZE_printfParam  			40
#define SpiFlashAddr_bakParam 			0x0800			// 备用参数
#define FlashSIZE_bakParam  			60

#define TRUE 1
#define FALSE 0

#define GAS_ENABLE 0
#define GAS_DISABLE 1


#define LED1 						PCout(4)
#define LED2 						PCout(5)

//映射位置传感器输入端口

#define GE_start_Sen PEin(8) // read the 封片资源.md 
#define GE_up_Sen PEin(9)
#define GE_down_Sen PEin(10)
#define GC_rot_Sen PEin(11)
#define GC_ver_Sen PEin(12)
#define GP_start_Sen PEin(13)
#define GOH_start_Sen PEin(14)
#define GOH_mid_Sen PEin(15)
#define GOH_end_Sen PDin(5)
#define GOV_start_Sen PDin(6)
#define GOV_glass_Sen PDin(7)

#define GP_sucker_Sen PDin(0)
#define GP_spray_Sen PDin(1)
#define GP_big_cyl_Sen PDin(3)
#define GP_small_cyl_Sen PDin(4)
#define GC_claw_Sen PEin(0)
#define GOV_box_Sen PBin(5)


//映射气阀气泵控制端口
#define Main_Pump PDout(8)
#define Main_in_Cyl PDout(9)
#define Main_out_Cyl PDout(10)

#define GP_sucker1_Cyl PDout(14)
#define GP_sucker2_Cyl PDout(15)
#define GP_sucker_Pump PBout(0)

#define GP_big_Cyl PDout(11)
#define GP_small_Cyl PDout(12)
#define GC_claw_Cyl PDout(13)

#define GP_spray_Cyl PBout(1)

#define DeveiceType_package 3

// 默认设备选择 
#define DeviceType_Default								DeveiceType_package	// 默认先不设置设备类型
#define Program_Versions									5						// 程序版本号
#define Enable_Check_ProgramPirate				1						// 程序盗版检测开关

//加密保护宏定义
#define Developer_QQ  										396376922   // 用于代码加密
#define RomFlashAddr_codeGuard	 					0x0807F000	// 储存代码保护密文,对于512K Flash储存器芯片有效
#define Enable_AutoOpen_FlashReadProtect	0						// 自动开启Flash读保护
#define ReadyZero_moveDistance						2						// 单位mm，复位时的准备回零位置
#define OvershootDistance									50					// 单位mm，复位、去终点时的过冲距离，用于确保到达光电位
#define BoxChannel												6						// 料槽数量


enum sensor_index //总共17个传感器
{
	GE_start_sensor=(1<<0),
	GE_up_sensor =(1<<1),
	GE_down_sensor =(1<<2),
	GC_rot_sensor =(1<<3),
	GC_ver_sensor =(1<<4),
	GP_start_sensor =(1<<5),
	GOH_start_sensor =(1<<6),
	GOH_mid_sensor =(1<<7),
	GOH_end_sensor =(1<<8),
	GOV_start_sensor =(1<<9),
	GOV_glass_sensor=(1<<10),

	GP_sucker_sensor =(1<<11),
	GP_spray_sensor=(1<<12),
	GP_big_cyl_sensor =(1<<13),
	GP_small_cyl_sensor =(1<<14),
	GC_claw_sensor=(1<<15),
	GOV_box_sensor =(1<<16),
};


typedef struct{
	u16 GE_1mm;
	u32 GE_max_speed;
	u32 GE_min_speed;
	u16 GCV_1mm;
	u32 GCV_max_speed;
	u32 GCV_min_speed;
	u16 GCR_max_speed;
	u16 GCR_min_speed;
	u16 GP_1mm;
	u32 GP_max_speed;
	u32 GP_min_speed;
	u16 GOH_1mm;
	u32 GOH_max_speed;
	u32 GOH_min_speed;
	u16 GOV_1mm;
	u32 GOV_max_speed;
	u32 GOV_min_speed;
}Motor_Data; 


typedef struct{
	u16 GCR_hor_pos;
	u16 GCR_ver_pos;
	u16 GCR_max_pos;
	u32 GCV_down_pos;
	u32 GCV_max_pos;
	u32 GOH_mid_pos;
	u32 GOH_end_pos;
	u32 GOH_max_pos;
}Glass_ClawSupport__data;

typedef struct{
	u16 delay_before;
	u16 delay_after;
	u16 sucker_delay;
	u32 max_pos;
	u32 sucker_pos;
	u32 spray_pos;
	u32 spray_len;
	u16 spray_speed;
	u8 spray_pressure;
}Glass_Package_Data;

typedef struct{
	u32 GOV_box_dis;
	u32 GOV_slot_dis;
	u32 GOV_box_len;
	u32 GOV_max_pos;
	u32 GE_box_len;
	u32 GE_box_dis;
	u32 GE_max_pos;
}Glass_In_Out_Date;


/////////////////Golbal Parameter///////////////////
struct Global_Parameter_struct{
	Motor_Data MOT;
	Glass_ClawSupport__data GCS;
	Glass_Package_Data GP;
	Glass_In_Out_Date GIO;
};

extern struct Global_Parameter_struct Global_Parm; 
extern enum sensor_index sensor_error_idx;
void Set_Global_Parameter_Default(void);




#endif