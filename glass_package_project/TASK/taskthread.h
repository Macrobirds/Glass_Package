#ifndef __TASKTHREAD_H
#define __TASKTHREAD_H
#include "motor.h"
#include "uart3_dataHandle.h"

#define GC_ROT_PACKAGE
//#define GC_VER_PACKAGE
#define GE_UP_DOWN_SENSOR
//#define GE_UP_SENSOR_BEFORE 

#define OVERTIME 20000 // 20 Second

// �����߳�״̬
enum taskthread_state
{
	taskthread_emergency, //������ͣ״̬ 
	taskthread_pause,	// ��ͣ״̬
	taskthread_debug,	// ����״̬
	taskthread_error,	// ����״̬
	taskthread_boost,	// ����״̬
	taskthread_ready,	// ����׼��״̬
	taskthread_running, // ���н���״̬
	taskthread_finsih,	// ���н���״̬
	taskthread_close,	// �ػ�״̬
};

// ������������״̬
enum task_state
{
	Ready,
	Running,
	Finish,
};

enum task_error
{
	Error_none = 0,
	Error_Sensor = 1 << 0, //  ����������
	Error_OverTime = 1 << 1,		//  ����ʱ����
	Error_Slide_Glass = 1 << 2, // ȱ���ز�Ƭ
	Error_In_Box = 1 << 3,		// ȱ��װ�غ�
	Error_Out_Box = 1 << 4,		// ȱ�ٴ洢��
	Error_Cover_Glass = 1 << 5,		// ȱ�ٸǲ�Ƭ
	Error_Spray = 1 << 6,	// ȱ�ٵν�ͷ
	Error_StorageFull = 1 << 7, // �洢����
	Error_Grap=1<<8, //��ȡʧ��
	Error_Sucker=1<<9, //��ȡʧ�� ��Ƭ�𻵻��������� 
};



// ��Ƭ����������������
enum glass_enter_task_index
{
	GE_none = 0,  // none ������
	/*************��������***********/
	GE_reset_on,  //  ������λ
	GE_reset_off, // �ػ���λ
	GE_Box_In,	  // ����
	GE_BOx_Out,	  // ����
	/****************************/
	GE_start,	   // �ƶ���ԭ��λ��
	#ifdef GE_UP_SENSOR_BEFORE
	GE_move_front, // �ƶ���װ�ز�ǰ��
	GE_move_glass, // �ƶ����ز�Ƭ
	GE_move_back,  // �ƶ���װ�زۺ��
	#endif
	
	#ifdef GE_UP_DOWN_SENSOR
	GE_move_glass,
	#endif
	GE_finish,	   // ��������
	GE_error,	   // �쳣��������

};

// ��Ƭ���������������
enum glass_claw_task_index
{
	GC_none = 0,
	/*************��������***********/
	GC_ver_start, // ���ָ�λ����ֱԭ��λ�� basic
	GC_rot_start, // ���ָ�λ����תԭ��λ�� basic
	GC_release,	  // �����ɿ��ͷ� basic
	GC_grab,	  // ���ּ�ȡ basic
	GC_reset_on,  // ������λ
	GC_reset_off, // �ػ���λ
	/*****************************/
	GC_start,		// ��ʼ����
	GC_rot_down,	// ������ת����ֱλ��
	GC_move_down,	// �����½������ز�Ƭ��
	GC_check_glass, // ����Ƿ��ȡ����Ƭ
	GC_move_up,		// ���������ش�ֱԭ��
	#ifdef GC_ROT_PACKAGE
	GC_rot_up,		// ���ֻص���תԭ��
	GC_rot_hor,		// ������ת��ˮƽλ��
	#endif
	#ifdef GC_VER_PACKAGE
	GC_rot_hor,
	GC_move_package,
	#endif
	
	GC_finish,
	GC_error, // �쳣��������
};

// ��Ƭ��װ�����������
enum glass_package_task_index
{
	GP_none = 0,
	/**************��������****************/
	GP_spray_test,	 // ���еν����� basic
	GP_move_start,	 // ��λ�����ԭ�� basic
	GP_cyl_start,	 // ���׸�λ��ԭ��λ�� basic
	GP_sucker_start, // �ر����� basic
	GP_reset_on,	 // ������λ
	GP_reset_off,	 // �ػ���λ
	/*************************************/
	GP_start,		  // ��Ƭ��ʼ��
	GP_move_glass,	  // �ƶ����ǲ�Ƭλ��
	GP_sucker_down,	  // �����½�
	GP_suck_glass,	  // ������ȡ�ǲ�Ƭ
	GP_sucker_up,	  // ��������
	GP_move_spray,	  // �ƶ����罺λ��
	GP_move_package,  // �ƶ�����Ƭλ��
	GP_package,		  // ��ʼ��Ƭ
	GP_finish,
	GP_error, // �쳣��������

};

// ��Ƭ�������������
enum glass_out_task_index
{
	GO_none = 0,
	/************��������************/
	GOV_start, // ��λ����ֱԭ��λ�� basic
	GOH_start, // ��Ƭ��λ��ˮƽԭ��λ�� basic
	GO_adjust, // �����洢�۶�׼��Ƭ basic
	GO_reset_on,  // ������λ
	GO_reset_off, // �ػ���λ
	GO_Box_In,	  // ����
	GO_Box_Out,	  // ����
	/*******************************/
	GO_start,	  // ��ʼ����
	GOH_package,  // ��Ƭ�����ƶ���ˮƽ��Ƭλ��
	GOH_end,	  // ��Ƭ�����ƶ���ˮƽ�յ�λ��
	GO_out,		  // ��Ƭ����
	GO_next,	  // �ƶ�����һ�洢��
	GO_finish,
	GO_error, // �쳣��������
};

struct glass_enter_struct
{
	volatile enum glass_enter_task_index task; // ��������
	volatile enum task_state sta;			   // ����״̬
	volatile motor_struct *motor;			   // ����ṹ��ָ��
	enum glass_enter_task_index resume_task;   // �ָ���������
	u32 running_tim;						   // ����ʱ�� ms
	u32 GE_box_len;							   // װ�ز۳���
	u32 GE_box_speed;
	u8 box_Exist;							   // װ�زۼ���ź�
	u8 glass_Exist;							   // �ز�Ƭ����ź�
	u8 subtask;								   // ������
	u8 Index;
	u8 WaitAck;
};

struct glass_claw_struct
{
	volatile enum glass_claw_task_index task;
	enum glass_claw_task_index main_task;
	enum glass_claw_task_index resume_task;
	volatile enum task_state sta;
	volatile motor_struct *motor_v; // ��ֱ���
	volatile motor_struct *motor_r; // ��ת���
	volatile u32 running_tim;		// ����ʱ��
	u16 GCR_hor_pos;				// ��תˮƽλ��
	u16 GCR_ver_pos;				// ��ת��ֱλ��
	u32 GCV_down_pos;				// ��ֱ�½�λ��
	u32 GCV_glass_len;		//����Ƿ��ȡ��Ƭ�ɹ������߶�
	u32 GCV_pacakge_pos;
	u8 subtask;						// ������
	u8 main_subtask;
	volatile u8 Glass_Ready;
};





struct glass_out_struct
{
	volatile enum glass_out_task_index task;
	enum glass_out_task_index main_task;
	volatile enum task_state sta;
	enum glass_out_task_index resume_task;
	volatile motor_struct *motor_h;
	volatile motor_struct *motor_v;
	volatile u32 running_tim;
	u32 GOH_mid_pos;
	u32 GOH_end_pos;
	u32 GOV_box_dis;
	u32 GOV_slot_dis;
	u32 GOV_box_len;
	u16 GOV_adjust;
	u16 GOV_adjust_start;
	u8 subtask;		 // ������
	u8 main_subtask; //
	u8 Index;
	u8 WaitAck;
	u8 Storage_full;
	u32 GOH_waste_pos; //��Ƭλ��
};

extern volatile enum task_error error_type;
extern enum taskthread_state TaskThread_State; // �����߳�����״̬ 


extern struct glass_enter_struct GE;
extern struct glass_claw_struct GC;

extern struct glass_out_struct GO;

extern volatile u8 glass_signal;
extern volatile u8 box_signal;

// �����߳̽ṹ���ʼ�� ����ʱ��TIM6��ʼ��
void TaskThread_Init(void);
//GE�����߳�
void GE_TaskThread(void);
//GC�����߳�
void GC_TaskThread(void);
//GP�����߳�
void GP_TaskThread(void);
//GO�����߳�
void GO_TaskThread(void);

//������
void Error_Set(enum task_error error, u8 error_sen);

//��ʱ������
void Error_OverTime_Set(enum Error error_task,u8 task_index);

//�Ӵ�����ͣ�лָ�����
u8 TaskThread_Resume_Error(void);

// �����Ƿ��ڿ���״̬
u8 TaskThread_CheckIdle(void);

//�������Ƿ����
u8 TaskThread_Check_ErrorDone(void);

//�����Ƿ�ֹͣ ������������
u8 TaskThread_CheckStop(void);

// �������Ƿ����׼������
enum taskthread_state TaskThread_IsReady(void);

// ������λ����
u8 TaskThread_BootReset(void);

// ��ʼ��������
u8 TaskThread_Start(void);

// �ر���������
u8 TaskThread_Close(void);

// ������ͣ����
void TaskThread_Emergency(void);

//��ͣ����
u8 TaskThread_Pause(void);

// �ָ�����
u8 TaskThread_Resume(void);

// ���������ʼ��
void TaskThread_Parm_Init(void);

//װ�ز۽���
u8 TaskThread_GEIn(void);
//װ�ز��Ƴ�
u8 TaskThread_GEOut(void);
//�洢�н���
u8 TaskThread_GOIn(void);
//�洢���Ƴ�
u8 TaskThread_GOOut(void);

//���ʹ�����Ϣ
void Error_Mesg_Send(void);

#endif