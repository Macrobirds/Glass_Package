#ifndef __GLOBALCONFIG_H
#define __GLOBALCONFIG_H
#include "stm32f10x.h"
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
#include "mydelay.h"
#include "math.h"
#include "uart3_dataHandle.h"


// �����������

#define BIG_CYLINDER




//#define BIG_CYLINDER_MOTOR
//#define DEBUG_MODE
//#define GE_DOWN_SENSOR_BEFORE


//SPI Flash ����
#define SpiFlashAddr_initFlag 				0x0000			// �״γ�ʼ����־λ
#define SpiFlashAddr_DeviceType 				0x0001			// �豸����
#define SpiFlashAddr_motorData 		0x0100			// �������������
#define SpiFlashAddr_clawsupportData 			0x0600			// ������ַ�Ƭ���̲���
#define SpiFlashAddr_packageData 			0x0700			// �����Ƭ����
#define SpiFlashAddr_inoutData				0x0800 			//�洢����۲���
#define SpiFlashAddr_bakParam 			0x0900			// ���ò���
#define FlashSIZE_bakParam  			60

#define SCALE 10



//ӳ��λ�ô���������˿�

#define GE_start_Sen PEin(10) // ��Ƭ���й����㴫����
#define GE_up_Sen PEin(9) //��Ƭ���в�Ƭ����϶˴�����
#define GE_down_Sen PEin(8) //��Ƭ���в�Ƭ����¶˴�����
#define GC_rot_Sen PEin(11) //��Ƭ������ת������ 
#define GC_ver_Sen PEin(12) //��Ƭ���ִ�ֱ������
#define GP_start_Sen PEin(13) //��Ƭ��Ƭ��㴫����
#define GOH_start_Sen PDin(3) //��Ƭ������㴫����
#define GOH_end_Sen PDin(5)  //Ƭ�����յ㴫����
#define GOV_start_Sen PDin(6) //��Ƭ���й��ԭ�㴫����
#define GOV_glass_Sen PDin(0) //��Ƭ���в�Ƭ������
#ifdef BIG_CYLINDER_MOTOR
#define GP_big_cyl_Sen PDin(7)
#endif

//#define GOH_mid_Sen PDin(0) //��Ƭ���ط�Ƭ������

#ifdef BIG_CYLINDER
#define GP_big_cyl_Sen PEin(0) //�����״�����
#endif


#define GP_sucker_Sen  PEin(15)//��Ƭ��Ƭ��Ƭ��ȡ��⴫����
//#define GP_sucker_Sen Sen_Block //Ĭ����ȡ����Ƭ

#define GP_spray_Sen PDin(1) //��Ƭ��Ƭ������ͷ��⴫����
//#define GP_spray_Sen Sen_Block //Ĭ�Ϸ�������ͷ

#define GC_claw_Sen PCin(2) //��Ƭ���ּ���״̬������




#define GP_small_cyl_Sen PDin(4) //С���״�����

#define GOV_box_Sen PEin(14)  //��Ƭ���в�Ƭ�д�����


//ӳ���������ÿ��ƶ˿�
#define Main_Pump PDout(8)  //������
#define Main_in_Cyl PDout(9) //�����ô�����
#define Main_out_Cyl PDout(10) //�����ó�����

#define GP_sucker1_Cyl PDout(14) //����1�� 
#define GP_sucker2_Cyl PDout(15) //����2��
#define GP_sucker_Pump PBout(0) //���̱�

#ifdef BIG_CYLINDER
#define GP_big_Cyl PDout(11) //������
#endif

#define GP_small_Cyl PDout(12) //С����
#define GC_claw_Cyl PDout(13) //����

#define GP_spray_Cyl PBout(1) //��ͷ
#define GCV_motor_Break PEout(7) //GCV�����բ

#define GP_ITV1100 PEout(3) //ITV��Դ����



#define DeveiceType_package 3

// Ĭ���豸ѡ�� 
#define DeviceType_Default								DeveiceType_package	// Ĭ���Ȳ������豸����
#define Program_Versions									5						// ����汾��
#define Enable_Check_ProgramPirate				1						// ��������⿪��

//���ܱ����궨��
#define Developer_QQ  										396376922   // ���ڴ������
#define RomFlashAddr_codeGuard	 					0x0807F000	// ������뱣������,����512K Flash������оƬ��Ч
#define Enable_AutoOpen_FlashReadProtect	0						// �Զ�����Flash������
#define ReadyZero_moveDistance						2						// ��λmm����λʱ��׼������λ��
#define OvershootDistance									50					// ��λmm����λ��ȥ�յ�ʱ�Ĺ�����룬����ȷ��������λ
#define BoxChannel												6						// �ϲ�����

enum Carrier_Versions {BMH = 1, BP = 2, FPJ=3};
// ����-����-�����
#define	DeviceType_dbj_zw_bmh 1
#define	DeviceType_dbj_zw_bp 2
#define DeviceType_fpj 3
// ����δ����
#define	DeviceType_none 0

//����궨��
#define TRUE 1
#define FALSE 0

#define GAS_ENABLE 0
#define GAS_DISABLE 1

#define LED1 						PCout(4)
#define LED2 						PCout(5)
#define LED_ON 					0

#define ITV1100_DISABLE 0
#define ITV1100_ENABLE 1


#define EN_ENABLE				0
#define LED_ON 					0
#define BUZZER_ON 			1
#define POWER_ON 				0
#define FAN_ON 					1
#define LAMP_ON 				1
#define UVLAMP_ON				1
#define HEATER_ON 			0

enum sensor_index //�ܹ�17��������
{
	sensor_none=0,
	GE_start_sensor,
	GE_up_sensor ,
	GE_down_sensor,
	GC_rot_sensor ,
	GC_ver_sensor ,
	GP_start_sensor,
	GOH_start_sensor ,
	GOH_mid_sensor ,
	GOH_end_sensor ,
	GOV_start_sensor ,
	GOV_glass_sensor,
	GP_sucker_sensor ,
	GP_spray_sensor,
	GP_big_cyl_sensor ,
	GP_small_cyl_sensor ,
	GC_claw_sensor,
	GOV_box_sensor ,
};


typedef struct{
	u16 GE_speed;
	u16 GCV_speed;
	u16 GCR_speed;
	u16 GP_speed;
	u16 GOH_speed;
	u16 GOV_speed;
	u16 GOV_dir;
	u16 GOH_dir;
	u16 GP_dir;
	u16 GC_rot_dir;
	u16 GC_ver_dir;
	u16 GE_dir;
}Motor_Data; 


typedef struct{
	u16 GCR_hor_pos;
	u16 GCR_ver_pos;
	u16 GCV_down_pos;
	u16 GCV_glass_len;
	u16 GOH_mid_pos;
	u16 GOH_end_pos;
	u16 GCV_package_pos;
}Glass_ClawSupport__data;

typedef struct{
	u16 delay_before;
	u16 delay_after;
	u16 sucker_pos;
	u16 spray_pos;
	u16 spray_len;
	u16 spray_speed;
	u16 spray_pressure;
}Glass_Package_Data;

typedef struct{
	u16 GOV_box_dis;
	u16 GOV_slot_dis;
	u16 GOV_box_len;
	u16 GOV_adjust;
	u16 GOV_adjust_start;
	u16 GE_box_len;
	u16 GE_box_speed;
}Glass_In_Out_Data;


/////////////////Golbal Parameter///////////////////
struct Global_Parameter_struct{
	Motor_Data * MOT;
	Glass_ClawSupport__data * GCS;
	Glass_Package_Data * GP;
	Glass_In_Out_Data * GIO;
};

extern struct Global_Parameter_struct Global_Parm; 
extern  u32 sensor_error_idx;


extern u32 deviceSnNumber;
extern u8 DeviceType ;
extern volatile u32 nowRtcTime;
void Set_Global_Parameter_Default(void);
void setBCC(u8 *data, u8 dataLength);
void getDeviceSnNumber(void);
void checkPirate(void);
void set_carrierVersions_Default(void);

extern OS_CPU_SR cpu_sr;

//motor struct
extern motor_struct GE_motor_struct;
extern motor_struct GC_rot_motor_struct;
extern motor_struct GC_ver_motor_struct;
extern motor_struct GP_motor_struct;
extern motor_struct GO_hor_motor_struct;
extern motor_struct GO_ver_motor_struct;

#ifdef BIG_CYLINDER_MOTOR
extern motor_struct GP_motor_big_cyl;

#endif

#ifdef BIG_CYLINDER_MOTOR
struct glass_package_struct
{
	volatile enum glass_package_task_index task;
	enum glass_package_task_index main_task;
	volatile enum task_state sta;
	enum glass_package_task_index resume_task;
	motor_struct *motor;
	motor_struct *motor_cyl;
	volatile u32 running_tim;
	u16 delay_before;
	u16 delay_after;
	u16 sucker_delay;
	u32 sucker_pos;
	u32 spray_pos;
	u32 spray_len;
	u16 spray_speed;
	u32 sucker_down_pos;
	u32 sucker_package_pos;
	u32 sucker_finish_pos;
	u8 spray_pressure;
	u8 subtask;
	u8 main_subtask;
};
#else
struct glass_package_struct
{
	volatile enum glass_package_task_index task;
	enum glass_package_task_index main_task;
	volatile enum task_state sta;
	enum glass_package_task_index resume_task;
	motor_struct *motor;
	volatile u32 running_tim;
	u16 delay_before;
	u16 delay_after;
	u16 sucker_delay;
	u32 sucker_pos;
	u32 spray_pos;
	u32 spray_len;
	u16 spray_speed;
	u8 spray_pressure;
	u8 subtask;
	u8 main_subtask;
};
#endif

extern struct glass_package_struct GP;



#endif