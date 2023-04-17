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
#include "RingBuffer.h"

#define DEBUG_MODE

//SPI Flash 参数
#define SpiFlashAddr_initFlag 				0x0000			// 首次初始化标志位
#define SpiFlashAddr_DeviceType 				0x0001			// 设备类型
#define SpiFlashAddr_motorData 		0x0100			// 储存电机脉冲参数
#define SpiFlashAddr_clawsupportData 			0x0600			// 储存夹手封片托盘参数
#define SpiFlashAddr_packageData 			0x0700			// 储存封片参数
#define SpiFlashAddr_inoutData				0x0800 			//存储出入槽参数
#define FlashSIZE_printfParam  			40
#define SpiFlashAddr_bakParam 			0x0800			// 备用参数
#define FlashSIZE_bakParam  			60

#define TRUE 1
#define FALSE 0

#define GAS_ENABLE 0
#define GAS_DISABLE 1


#define LED1 						PCout(4)
#define LED2 						PCout(5)
#define LED_ON 					0
//映射位置传感器输入端口

#define GE_start_Sen PEin(10) // 玻片进盒轨道起点传感器
#define GE_up_Sen PEin(9) //玻片进盒玻片检测上端传感器
#define GE_down_Sen PEin(8) //玻片进盒玻片检测下端传感器
#define GC_rot_Sen PEin(11) //玻片夹手旋转传感器 
#define GC_ver_Sen PEin(12) //玻片夹手垂直传感器
#define GP_start_Sen PEin(13) //玻片封片起点传感器
#define GOH_start_Sen PDin(3) //玻片承载起点传感器
#define GOH_mid_Sen PDin(0) //玻片承载封片传感器
#define GOH_end_Sen PDin(5)  //片承载终点传感器
#define GOV_start_Sen PDin(6) //玻片出盒轨道原点传感器
#define GOV_glass_Sen PDin(7) //玻片出盒玻片传感器

#define GP_sucker_Sen  PEin(15)//玻片封片玻片吸取检测传感器
#define GP_spray_Sen PDin(1) //玻片封片出油喷头检测传感器
#define GC_claw_Sen PCin(2) //玻片夹手夹手状态传感器
#define GP_big_cyl_Sen PEin(0) //大气缸传感器
#define GP_small_cyl_Sen PDin(4) //小气缸传感器

#define GOV_box_Sen PEin(14)  //玻片出盒玻片盒传感器


//映射气阀气泵控制端口
#define Main_Pump PDout(8)  //主气泵
#define Main_in_Cyl PDout(9) //主气泵打气阀
#define Main_out_Cyl PDout(10) //主气泵出气阀

#define GP_sucker1_Cyl PDout(14) //吸盘1阀 
#define GP_sucker2_Cyl PDout(15) //吸盘2阀
#define GP_sucker_Pump PBout(0) //吸盘泵

#define GP_big_Cyl PDout(11) //大气缸
#define GP_small_Cyl PDout(12) //小气缸
#define GC_claw_Cyl PDout(13) //夹手

#define GP_spray_Cyl PBout(1) //喷头
#define GCV_motor_Break PEout(7) //GCV电机抱闸

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

enum Carrier_Versions {BMH = 1, BP = 2};
// 打标机-紫外-包埋盒
#define	DeviceType_dbj_zw_bmh 1
#define	DeviceType_dbj_zw_bp 2
// 机型未设置
#define	DeviceType_none 0

enum sensor_index //总共17个传感器
{
	GE_sensor_none=0,
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
	u8 GE_max_speed;
	u8 GE_min_speed;
	u16 GCV_1mm;
	u8 GCV_max_speed;
	u8 GCV_min_speed;
	u16 GCR_max_speed;
	u16 GCR_min_speed;
	u16 GP_1mm;
	u8 GP_max_speed;
	u8 GP_min_speed;
	u16 GOH_1mm;
	u8 GOH_max_speed;
	u8 GOH_min_speed;
	u16 GOV_1mm;
	u8 GOV_max_speed;
	u8 GOV_min_speed;
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


extern u32 deviceSnNumber;
extern u8 DeviceType ;
extern volatile u32 nowRtcTime;
void Set_Global_Parameter_Default(void);
void setBCC(u8 *data, u8 dataLength);

extern OS_CPU_SR cpu_sr;

#endif