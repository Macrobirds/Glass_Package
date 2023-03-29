#ifndef __TASKTHREAD_H
#define __TASKTHREAD_H
#include "motor.h"
#include "Globalconfig.h"

#define OVERTIME 3000


//��������״̬
enum task_state
{
	 Ready,
	 Running,
	 Finish,
};

enum task_error
{
	Error_Slide_Glass=1<<0,
	Error_Cover_Glass=1<<1,
	Error_In_Box=1<<2,
	Error_Out_Box=1<<3,
	Error_Sensor=1<<4,
	Error_Spray=1<<5,
	Error_OverTime=1<<6,
};

//��Ƭ����������������
enum glass_enter_task_index
{
	GE_none,// none ������ 
	GE_reset_on,//������λ ����
	GE_reset_off,//�ػ���λ ����
	GE_move_start, //�ƶ���ԭ��λ��
	GE_move_front, //�ƶ���װ�ز�ǰ��
	GE_move_glass, //�ƶ����ز�Ƭ
	GE_move_back, //�ƶ���װ�زۺ��
	GE_finish, //��������
  GE_error,//�쳣�ϱ�����
    
};

//��Ƭ���������������
enum glass_claw_task_index
{
	GC_none,
	GC_reset_on, //������λ
	GC_reset_off, //�ػ���λ
	GC_ver_start, //���ָ�λ����ֱԭ��λ��
	GC_rot_start, //���ָ�λ����תԭ��λ��
	GC_rot_down, //������ת����ֱλ��
	GC_move_down, //�����ƶ������ز�Ƭ��
	GC_grab,	//���ּ�ȡ�ز�Ƭ
	GC_move_up,	//���ֻص���ֱԭ��
	GC_rot_up, //������ת����תԭ��λ��
	GC_rot_hor, //������ת��ˮƽλ��
	GC_release, //�����ɿ��ͷ�
	GC_finish, 
  GC_error,//�쳣�ϱ�����
};

//��Ƭ��װ�����������
enum glass_package_task_index
{
	GP_none,
	GP_reset_on,
	GP_reset_off,
	GP_start, //��Ƭ��ʼ��
	GP_move_start, //�ƶ������ԭ��
	GP_move_glass, //�ƶ����ǲ�Ƭλ��
	GP_sucker_down, //�����½�
	GP_suck_glass, //������ȡ�ǲ�Ƭ
	GP_sucker_up, //��������
	GP_move_spray, //�ƶ����罺λ��
	GP_move_package, //�ƶ�����Ƭλ��
	GP_package, //��ʼ��Ƭ
	GP_finish, 
  GP_error,//�쳣�ϱ�����
};


//��Ƭ�������������
enum glass_out_task_index
{
	GO_none,
	GO_reset_on,
	GO_reset_off,
	GO_start, //��Ƭ�����ƶ���ԭ��λ��
	GO_package, //��Ƭ�����ƶ�����Ƭλ��
	GO_end, //��Ƭ�����ƶ����յ�λ��
	GO_out, //��Ƭ����
	GO_next, //�ƶ�����һ�洢��
	GO_adjust,  //�����洢�۶�׼��Ƭ
	GO_finish,
  GO_error,//�쳣�ϱ�����
};

struct glass_enter_struct{
	volatile enum glass_enter_task_index task; //��������
	volatile enum task_state sta; //����״̬
	volatile enum task_error error; //������
	motor_struct * motor; //����ṹ��ָ��
	volatile u32 running_tim; //����ʱ�� ms
	u32 GE_box_len; //װ�ز۳���
	u32 GE_box_dis; //װ�زۼ��
	u8 box_Exist; //װ�زۼ���ź�
	u8 glass_Exist; //�ز�Ƭ����ź�
	u8 subtask; //������
	enum glass_enter_task_index resume_task; //�ָ���������
};

struct glass_claw_struct{
	volatile enum glass_claw_task_index task;
	volatile enum task_state sta;
	volatile enum task_error error;
	motor_struct * motor_v; //��ֱ���
	motor_struct * motor_r; //��ת���
	volatile u32 running_tim; //����ʱ��
	u16 GCR_hor_pos; //��תˮƽλ��
	u16 GCR_ver_pos; //��ת��ֱλ��
	u32 GCV_down_pos; //��ֱ�½�λ��
	u8 subtask; //������
	enum glass_claw_task_index resume_task;//�ָ���������
}; 

struct glass_package_struct{
	volatile enum glass_package_task_index task;
	volatile enum task_state sta;
	volatile enum task_error error;
	motor_struct * motor;
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
	enum glass_package_task_index resume_task;//�ָ���������
};

struct glass_out_struct{
	volatile enum glass_out_task_index task;
	volatile enum task_state sta;
	volatile enum task_error error;
	motor_struct * motor_h;
	motor_struct * motor_v;
	volatile u32 running_tim;
	u32 GOH_mid_pos;
	u32 GOH_end_pos;
	u32 GOV_box_dis;
	u32 GOV_slot_dis;
	u32 GOV_box_len;
	u8 subtask; //������
	volatile u8 glass_num;
	volatile u8 box_num;
	enum glass_out_task_index resume_task;//�ָ���������
};

//����׼��״̬
u8 Task_IsReady=FALSE;


extern struct glass_enter_struct GE;
extern struct glass_claw_struct GC;
extern struct glass_package_struct GP;
extern struct glass_out_struct GO;
//�����߳̽ṹ���ʼ�� ����ʱ��TIM6��ʼ��
void TaskThread_Init(void); 
//�������Ƿ����׼������
void TaskThread_IsReady(void); 
void GE_TaskThread(void);
void GC_TaskThread(void);
void GP_TaskThread(void);
void GO_TaksThread(void);

void Pause_TaskThread(void);
void Resume_TaskThread(void);



#endif