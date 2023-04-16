#ifndef __TASKTHREAD_H
#define __TASKTHREAD_H
#include "motor.h"
#include "Globalconfig.h"

#define OVERTIME 10000

//�����߳�״̬
enum taskthread_state
{
	taskthread_boost,//����״̬
	taskthread_pause,//��ͣ״̬
	taskthread_debug,//����״̬
	taskthread_error,//����״̬
	taskthread_ready,//����׼��״̬
	taskthread_running,//���н���״̬
	taskthread_finsih, //���н���״̬
	taskthread_close,//�ػ�״̬
	taskthread_reset,//��λ״̬
	
};


//������������״̬
enum task_state
{
	 Ready,
	 Running,
	 Finish,
};

enum task_error
{
	Error_none=0,
	Error_Cover_Glass=1<<0, //ȱ�ٸǲ�Ƭ
	Error_Spray=1<<1, //ȱ�ٵν�ͷ
	Error_Slide_Glass=1<<2, //ȱ���ز�Ƭ
	Error_In_Box=1<<3, //ȱ�ٽ��ϲ�
	Error_Out_Box=1<<4, //ȱ��װ�زۻ�װ�ز�����
	Error_Sensor=1<<5, //����������
	Error_OverTime=1<<6, //����ʱ����
	
};

//��Ƭ����������������
enum glass_enter_task_index
{
	GE_none=0,// none ������ 
	GE_reset_on,//  ������λ
	GE_reset_off,// �ػ���λ
	GE_Box_In, //����
	GE_BOx_Out, //����
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
	GC_none=0,
	GC_reset_on, //������λ
	GC_reset_off, //�ػ���λ
	GC_run, //��ʼ����
	GC_ver_start, //���ָ�λ����ֱԭ��λ��
	GC_rot_start, //���ָ�λ����תԭ��λ��
	GC_rot_down, //������ת����ֱλ��
	GC_move_down, //�����½������ز�Ƭ��
	GC_grab,	//���ּ�ȡ
	GC_release, //�����ɿ��ͷ�
	GC_rot_hor, //������ת��ˮƽλ��
	GC_finish, 
  	GC_error,//�쳣�ϱ�����
};

//��Ƭ��װ�����������
enum glass_package_task_index
{
	GP_none=0,
	GP_reset_on,//������λ
	GP_reset_off,//�ػ���λ
	GP_lack_glass, //ȱ�ٸǲ�Ƭ��λ 
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
	GO_none=0,
	GO_reset_on,//������λ
	GO_reset_off, //�ػ���λ
	GO_Box_In,//����
	GO_Box_Out,//����
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
	enum glass_enter_task_index task; //��������
	enum task_state sta; //����״̬
	motor_struct * motor; //����ṹ��ָ��
	u32 running_tim; //����ʱ�� ms
	u32 GE_box_len; //װ�ز۳���
	u32 GE_box_dis; //װ�زۼ��
	u8 box_Exist; //װ�زۼ���ź�
	u8 glass_Exist; //�ز�Ƭ����ź�
	u8 subtask; //������
	enum glass_enter_task_index resume_task; //�ָ���������
	u8 Index;
	u8 WaitAck;
};

struct glass_claw_struct{
	volatile enum glass_claw_task_index task;
	volatile enum task_state sta;
	motor_struct * motor_v; //��ֱ���
	motor_struct * motor_r; //��ת���
	volatile u32 running_tim; //����ʱ��
	u16 GCR_hor_pos; //��תˮƽλ��
	u16 GCR_ver_pos; //��ת��ֱλ��
	u32 GCV_down_pos; //��ֱ�½�λ��
	u8 subtask; //������
	enum glass_claw_task_index resume_task; //�ָ���������
}; 

struct glass_package_struct{
	volatile enum glass_package_task_index task;
	volatile enum task_state sta;
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
	enum glass_package_task_index resume_task; //�ָ��������� 
};

struct glass_out_struct{
	volatile enum glass_out_task_index task;
	volatile enum task_state sta;
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
	enum glass_out_task_index resume_task; //�ָ���������
	u8 Index;
	u8 WaitAck;
};



extern volatile enum task_error error_type;
extern enum taskthread_state TaskThread_State; //�����߳�����״̬ ����/��ͣ
extern enum taskthread_state OldTaskThread_State;

extern struct glass_enter_struct GE;
extern struct glass_claw_struct GC;
extern struct glass_package_struct GP;
extern struct glass_out_struct GO;
//�����߳̽ṹ���ʼ�� ����ʱ��TIM6��ʼ��
void TaskThread_Init(void); 
//�������Ƿ����׼������
enum taskthread_state TaskThread_IsReady(void);
void GE_TaskThread(void);
void GC_TaskThread(void);
void GP_TaskThread(void);
void GO_TaskThread(void);


void Error_Set(enum task_error error,u32 error_sen);

//�����Ƿ��ڿ���״̬
u8 TaskThread_CheckIdle(void);

//������λ����
void Boot_ResetTaskThread(void);
//��ʼ��������
void Start_TaskThread(void);
//�ر���������
void Close_TaskThread(void);

//��ͣ����
void Pause_TaskThread(void);
//�ָ�����
void Resume_TaskThread(void);
//���������ʼ��
void TaskThread_Parm_Init(void);


#endif