#ifndef __TASKTHREAD_H
#define __TASKTHREAD_H
#include "motor.h"
#include "Globalconfig.h"


//��������״̬
enum task_state
{
	 Ready,
	 Running,
	 Finish,
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
	GP_move_glass, //�ƶ����ǲ�Ƭλ��
	GP_sucker_down, //�����½�
	GP_suck_glass, //������ȡ�ǲ�Ƭ
	GP_sucker_up, //��������
	GP_move_package, //�ƶ�����Ƭλ��
	GP_spray, //�ƶ����罺λ��
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
	GO_move_out, //��Ƭ�����ƶ����յ�λ��
	GO_out, //��Ƭ����
	GO_next, //�ƶ�����һ�洢��
	GO_finish,
  GO_error,//�쳣�ϱ�����
};

struct glass_enter_struct{
	enum glass_enter_task_index task;
	enum task_state sta;
	motor_struct * motor;
	u32 GE_box_len;
	u32 GE_box_dis;
};

struct glass_claw_struct{
	enum glass_claw_task_index task;
	enum task_state sta;
	motor_struct * motor_v;
	motor_struct * motor_h;
	u16 GCR_hor_pos;
	u16 GCR_ver_pos;
	u32 GCV_down_pos;
};

struct glass_package_struct{
	enum glass_package_task_index task;
	enum task_state sta;
	motor_struct * motor;
	u16 delay_before;
	u16 delay_after;
	u16 sucker_delay;
	u32 sucker_pos;
	u32 spray_pos;
	u32 spray_len;
	u16 spray_speed;
	u8 spray_pressure;
};

struct glass_out_struct{
	enum glass_out_task_index task;
	enum task_state sta;
	motor_struct * motor_h;
	motor_struct * motor_v;
	u32 GOH_mid_pos;
	u32 GOH_end_pos;
	u32 GOV_box_dis;
	u32 GOV_slot_dis;
	u32 GOV_box_len;
};


extern struct glass_enter_struct GE;
extern struct glass_claw_struct GC;
extern struct glass_package_struct GP;
extern struct glass_out_struct GO;

void TaskThread_Init(void);


#endif