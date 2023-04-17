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

//SPI Flash ����
#define SpiFlashAddr_initFlag 				0x0000			// �״γ�ʼ����־λ
#define SpiFlashAddr_DeviceType 				0x0001			// �豸����
#define SpiFlashAddr_motorData 		0x0100			// �������������
#define SpiFlashAddr_clawsupportData 			0x0600			// ������ַ�Ƭ���̲���
#define SpiFlashAddr_packageData 			0x0700			// �����Ƭ����
#define SpiFlashAddr_inoutData				0x0800 			//�洢����۲���
#define FlashSIZE_printfParam  			40
#define SpiFlashAddr_bakParam 			0x0800			// ���ò���
#define FlashSIZE_bakParam  			60

#define TRUE 1
#define FALSE 0

#define GAS_ENABLE 0
#define GAS_DISABLE 1


#define LED1 						PCout(4)
#define LED2 						PCout(5)
#define LED_ON 					0
//ӳ��λ�ô���������˿�

#define GE_start_Sen PEin(10) // ��Ƭ���й����㴫����
#define GE_up_Sen PEin(9) //��Ƭ���в�Ƭ����϶˴�����
#define GE_down_Sen PEin(8) //��Ƭ���в�Ƭ����¶˴�����
#define GC_rot_Sen PEin(11) //��Ƭ������ת������ 
#define GC_ver_Sen PEin(12) //��Ƭ���ִ�ֱ������
#define GP_start_Sen PEin(13) //��Ƭ��Ƭ��㴫����
#define GOH_start_Sen PDin(3) //��Ƭ������㴫����
#define GOH_mid_Sen PDin(0) //��Ƭ���ط�Ƭ������
#define GOH_end_Sen PDin(5)  //Ƭ�����յ㴫����
#define GOV_start_Sen PDin(6) //��Ƭ���й��ԭ�㴫����
#define GOV_glass_Sen PDin(7) //��Ƭ���в�Ƭ������

#define GP_sucker_Sen  PEin(15)//��Ƭ��Ƭ��Ƭ��ȡ��⴫����
#define GP_spray_Sen PDin(1) //��Ƭ��Ƭ������ͷ��⴫����
#define GC_claw_Sen PCin(2) //��Ƭ���ּ���״̬������
#define GP_big_cyl_Sen PEin(0) //�����״�����
#define GP_small_cyl_Sen PDin(4) //С���״�����

#define GOV_box_Sen PEin(14)  //��Ƭ���в�Ƭ�д�����


//ӳ���������ÿ��ƶ˿�
#define Main_Pump PDout(8)  //������
#define Main_in_Cyl PDout(9) //�����ô�����
#define Main_out_Cyl PDout(10) //�����ó�����

#define GP_sucker1_Cyl PDout(14) //����1�� 
#define GP_sucker2_Cyl PDout(15) //����2��
#define GP_sucker_Pump PBout(0) //���̱�

#define GP_big_Cyl PDout(11) //������
#define GP_small_Cyl PDout(12) //С����
#define GC_claw_Cyl PDout(13) //����

#define GP_spray_Cyl PBout(1) //��ͷ
#define GCV_motor_Break PEout(7) //GCV�����բ

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

enum Carrier_Versions {BMH = 1, BP = 2};
// ����-����-�����
#define	DeviceType_dbj_zw_bmh 1
#define	DeviceType_dbj_zw_bp 2
// ����δ����
#define	DeviceType_none 0

enum sensor_index //�ܹ�17��������
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