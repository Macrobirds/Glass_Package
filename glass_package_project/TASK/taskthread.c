#include "taskthread.h"

struct glass_enter_struct GE={
	.task=GE_none,
	.sta=Ready,
	.motor=&GE_motor_struct,
	.GE_box_len=212*120,
	.GE_box_dis=212*80,
	.glass_Exist=FALSE,
	.box_Exist=FALSE,
	.subtask=0,

};


struct glass_claw_struct GC={
	.task=GC_none,
	.sta=Ready,
	.motor_v=&GC_ver_motor_struct,
	.motor_r=&GC_rot_motor_struct,
	.GCV_down_pos=33000,
	.GCR_hor_pos=120,
	.GCR_ver_pos=1720,
	.main_subtask=0,
	.main_task=GC_none,
};


struct glass_package_struct GP={
	.task=GP_none,
	.sta=Ready,
	.motor=&GP_motor_struct,
	.sucker_pos=20000,
	.spray_pos=14000,
	.spray_len=268*20,
	.spray_speed=268*30,
};


struct glass_out_struct GO={
	.task=GO_none,
	.sta=Ready,
	.motor_h=&GO_hor_motor_struct,
	.motor_v=&GO_ver_motor_struct,
	.GOH_mid_pos=9300,
	.GOH_end_pos=268*210,
	.GOV_box_dis=800*25,
	.GOV_box_len=800*241+600,
	.GOV_slot_dis=800*35/10,
	
};

enum taskthread_state TaskThread_State=taskthread_boost; //�����߳�����״̬ 
enum taskthread_state OldTaskThread_State=taskthread_boost; //
volatile enum task_error error_type=Error_none;

volatile u32 sensor_filter=0;


u8 TaskThread_CheckIdle(void)
{
	if(GE.task==GE_none&&GC.task==GC_none&&
		GP.task==GP_none&&GO.task==GO_none)
		{
			return TRUE;
		}
	else if(GE.task==GE_finish&&GC.task==GC_finish&&
		GP.task==GP_finish&&GO.task==GO_finish)
		{
			return TRUE;
		}

		return FALSE;
}

void Error_Set(enum task_error error,u32 error_sen)
{

//	printf("error occur:%d,%d\r\n",error,error_sen);
	#ifndef DEBUG_MODE
	//��ͣ��ǰ����
	Pause_TaskThread();

	//���뱨����� ����
	GE.subtask=0;
	GE.sta=Running;
	GE.task=GE_error;

	GC.subtask=0;
	GC.sta=Running;
	GC.task=GC_error;

	GP.subtask=0;
	GP.sta=Running;
	GP.task=GP_error;

	GO.subtask=0;
	GO.sta=Running;
	GO.task=GO_error;

	error_type|=error;
	if(error_sen)
	{
		sensor_error_idx|=error_sen;
	}
	#endif
}

void TaskThread_Parm_Init(void)
{
	//set GE task parm
	GE.GE_box_dis=Global_Parm.GIO.GE_box_dis;
	GE.GE_box_len=Global_Parm.GIO.GE_box_len;
	//set GC task parm
	GC.GCR_hor_pos=Global_Parm.GCS.GCR_hor_pos;
	GC.GCR_ver_pos=Global_Parm.GCS.GCR_ver_pos;
	GC.GCV_down_pos=Global_Parm.GCS.GCV_down_pos;
	//set GP parm
	GP.delay_after=Global_Parm.GP.delay_after;
	GP.delay_before=Global_Parm.GP.delay_before;
	GP.sucker_delay=Global_Parm.GP.sucker_delay;
	GP.sucker_pos=Global_Parm.GP.sucker_pos;
	GP.spray_pos=Global_Parm.GP.spray_pos;
	GP.spray_len=Global_Parm.GP.spray_len;
	GP.spray_speed=Global_Parm.GP.spray_speed;
	GP.spray_pressure=Global_Parm.GP.spray_pressure;
	//set GO parm
	GO.GOH_end_pos=Global_Parm.GCS.GOH_end_pos;
	GO.GOH_mid_pos=Global_Parm.GCS.GOH_mid_pos;
	GO.GOV_box_dis=Global_Parm.GIO.GOV_box_dis;
	GO.GOV_slot_dis=Global_Parm.GIO.GOV_slot_dis;
	GO.GOV_box_len=Global_Parm.GIO.GOV_box_len;
}

void TaskThread_Init(void)
{
	printf("TaskThread_Init\r\n");
	//Init TIM6 Interrupt  1ms
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM6��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 10-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM6�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM6,ENABLE);
	#ifndef DEBUG_MODE
	TaskThread_Parm_Init();
	#endif
}

void TIM6_IRQHandler(void) //TIM6�ж�
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{

		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		if(TaskThread_State!=taskthread_pause) //��������ͣ״̬
		{
			//GE ��Ƭ���������߳�
			GE_TaskThread();
			//GC ��Ƭ���������߳�
			GC_TaskThread();
			//GP ��Ƭ��װ�����߳�
			GP_TaskThread();
			//GO ��Ƭ�洢�����߳�
			GO_TaskThread();		
		}else
		{
			if(error_type) //��Ϊ�����������ͣ ���д��������
			{
				TaskThread_State=taskthread_error; 
			}
		}

		

	}

}

//��ͣ����
void Pause_TaskThread(void)
{
	//pause all step motor
	OldTaskThread_State=TaskThread_State; //���浱ǰ����ģʽ
	TaskThread_State=taskthread_pause;
	stepperMotorStop(&GE_motor_struct);
	stepperMotorStop(&GC_ver_motor_struct);
	stepperMotorStop(&GC_rot_motor_struct);
	stepperMotorStop(&GP_motor_struct);
	stepperMotorStop(&GO_hor_motor_struct);
	stepperMotorStop(&GO_ver_motor_struct);
	//���浱ǰ����״̬
	GE.resume_task=GE.task;
	GC.resume_task=GC.task;
	GP.resume_task=GP.task;
	GO.resume_task=GO.task;


}
//�ָ�����
void Resume_TaskThread(void)
{
	if(error_type<Error_Slide_Glass) //δ����Ԫ������������
	{
		GE.sta=Ready;
		GE.task=GE.resume_task;
		GC.sta=Ready;
		GC.task=GC.resume_task;
		GP.sta=Ready;
		GP.task=GP.resume_task;
		GO.sta=Ready;
		GP.task=GP.resume_task;
		TaskThread_State=OldTaskThread_State; //�ָ�֮ǰ������ģʽ
	}


}

enum taskthread_state TaskThread_IsReady(void)
{
	
	//����Ƿ���ڴ洢��
	if(GOV_box_Sen!=Sen_Block)
	{
		error_type|=Error_Out_Box; 
	}
	//���洢���Ƿ��⵽��Ƭ �洢���Ƿ���ȷ����
	if(GOV_glass_Sen==Sen_Block)
	{
		error_type|=Error_Sensor;
		sensor_error_idx|=GOV_glass_sensor; 
	}
	//����Ƿ���ڵν�ͷ
	if(GP_spray_Sen!=Sen_Block)
	{
		error_type|=Error_Spray;
	}
	if(error_type)
	{
		Error_Set(Error_none,0); //�ϱ�����
		return taskthread_error;
	}

	//�����ѹ�Ƿ�ﵽ����״̬
	if(Gas_State==gas_pumped)
	{
		return taskthread_boost;
	}
	//���������λ���Ƿ�У׼
	if(GE_motor_struct.postion<0&&GC_rot_motor_struct.postion<0&&
		GC_ver_motor_struct.postion<0&&GP_motor_struct.postion<0&&
		GO_hor_motor_struct.postion<0&&GO_ver_motor_struct.postion<0)
		{

			return taskthread_boost;
		}

	return taskthread_ready;
	
}

//������λ����
void Boot_ResetTaskThread(void)
{
	GE.sta=Ready;
	GE.task=GE_reset_on;
	
	GC.sta=Ready;
	GC.task=GC_reset_on;
	
	GP.sta=Ready;
	GP.task=GP_reset_on;
	
	GO.sta=Ready;
	GO.task=GO_reset_on;



}

//��ʼ��������
void Start_TaskThread(void)
{
	if(TaskThread_State==taskthread_ready)
	{
		GE.sta=Ready;
		GE.task=GE_start;

		GC.sta=Ready;
		GC.task=GC_rot_down;

		GP.sta=Ready;
		GP.task=GP_start;

		GO.sta=Ready;
		GO.task=GO_start;

		TaskThread_State=taskthread_running;
	}

}
//�ر���������
void Close_TaskThread(void)
{
	if(TaskThread_State==taskthread_finsih) //������н��� ֱ�ӹػ���λ
	{
		GE.sta=Ready;
		GE.task=GE_none;

		GC.sta=Ready;
		GC.task=GC_reset_off;

		GP.sta=Ready;
		GP.task=GP_reset_off;

		GO.sta=Ready;
		GO.task=GO_none;
	}

	TaskThread_State=taskthread_close;

}