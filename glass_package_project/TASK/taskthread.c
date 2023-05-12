#include "Globalconfig.h"

#define TASK_PRIORIRY 3

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
	.GCV_down_pos=33000+211*2,
	.GCR_hor_pos=130,
	.GCR_ver_pos=1720,
	.main_subtask=0,
	.main_task=GC_none,
	.Glass_Ready=FALSE,
};



#ifdef BIG_CYLINDER_MOTOR
struct glass_package_struct GP={
	.task=GP_none,
	.sta=Ready,
	.motor=&GP_motor_struct,
	.motor_cyl=&GP_motor_big_cyl,
	.sucker_pos=20000+268*5,
	.spray_pos=19000,
	.spray_len=268*15,
	.spray_speed=268*30,
	.sucker_down_pos=1,
	.sucker_package_pos=1,
	.sucker_finish_pos=1,
	.delay_before=420,
	.delay_after=120,
	.spray_pressure=3,
	.main_subtask=0,
	.main_task=GP_none,
};
#else
struct glass_package_struct GP={
	.task=GP_none,
	.sta=Ready,
	.motor=&GP_motor_struct,
	.sucker_pos=20000+268*5,
	.spray_pos=19000,
	.spray_len=268*15,
	.spray_speed=268*30,
	.delay_before=420,
	.delay_after=120,
	.spray_pressure=3,
	.main_subtask=0,
	.main_task=GP_none,
};
#endif


struct glass_out_struct GO={
	.task=GO_none,
	.sta=Ready,
	.motor_h=&GO_hor_motor_struct,
	.motor_v=&GO_ver_motor_struct,
	.GOH_mid_pos=9210+400,
	.GOH_end_pos=268*210,
	.GOV_box_dis=800*10,
	.GOV_box_len=800*241+600,
	.GOV_slot_dis=800*4,
	.Storage_full=TRUE, //Ĭ�ϴ洢���� ��Ҫͨ�������������װ�������´洢��״̬
	.main_subtask=0,
	.main_task=GO_none,
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


//���ʹ�������Ϣ
static void Error_Mesg_Send(void)
{
		//���ʹ�����Ϣ
		/************************/
		printf("error:%d error_sensor%d\r\n",error_type,sensor_error_idx);
		/***********************/
}


//���������� �������������
void Error_Set(enum task_error error,u32 error_sen)
{
	#ifndef DEBUG_MODE
	
	//���±�����״̬
	error_type|=error;
	sensor_error_idx|=error_sen;
	

	//���Ϊ�����������������ʱ���� ��Ҫ����ֹͣ����
	if(error_type&(Error_OverTime+Error_Sensor))
	{
		Emergency_TaskThread();		//�������ֹͣģʽ
		Error_Mesg_Send();
		//���뱨����� ����
		GE.subtask=0;
		GE.sta=Ready;
		GE.task=GE_error;

		GC.subtask=0;
		GC.sta=Ready;
		GC.task=GC_error;

		GP.subtask=0;
		GP.sta=Ready;
		GP.task=GP_error;

		GO.subtask=0;
		GO.sta=Ready;
		GO.task=GO_error;
		return;
	}
	Error_Mesg_Send();
	
	//���ݲ�ͬ�Ĵ������ý�������ָ����к�ִ�е�����
	//ȱ�ٸǲ�Ƭ
	if(error_type&Error_Cover_Glass)
	{
		GP.resume_task=GP_sucker_down;
		GP.sta=Ready;
		GP.task=GP_error;
	}
	//ȱ���罺ͷ
	if(error_type&Error_Spray)
	{
		GP.resume_task=GP.task;
		GP.sta=Ready;
		GP.task=GP_error;
	}
	//ȱ�ٴ洢��
	if(error_type&Error_Out_Box)
	{
		GO.resume_task=GO.task;
		GO.sta=Ready;
		GO.task=GO_error;
	}
	//�洢����
	if(error_type&Error_StorageFull)
	{
		GO.resume_task=GO.task;
		GO.sta=Ready;
		GO.task=GO_error;
	}
	//��ȡʧ��
	if(error_type&Error_Grap)
	{
		GC.resume_task=GC_move_down;
		GC.sta=Ready;
		GC.task=GC_error;
	}
	//��ȡ��Ƭʧ��
	if(error_type&Error_Sucker)
	{
		GP.resume_task=GP_sucker_down;
		GP.sta=Ready;
		GP.task=GP_error;
	}
	
	TaskThread_State=taskthread_error; //�������������



	#else
	printf("Errror:%d,Error_sensor:%d",error,error_sen);
	stepperMotorStop(&GE_motor_struct);
	stepperMotorStop(&GC_ver_motor_struct);
	stepperMotorStop(&GC_rot_motor_struct);
	stepperMotorStop(&GP_motor_struct);
	stepperMotorStop(&GO_hor_motor_struct);
	stepperMotorStop(&GO_ver_motor_struct);
	//���뱨����� ����
	GE.subtask=0;
	GE.sta=Ready;
	GE.task=GE_none;

	GC.subtask=0;
	GC.sta=Ready;
	GC.task=GC_none;

	GP.subtask=0;
	GP.sta=Ready;
	GP.task=GP_none;

	GO.subtask=0;
	GO.sta=Ready;
	GO.task=GO_none;
	
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TASK_PRIORIRY;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM6,ENABLE);
//	#ifndef DEBUG_MODE
//	TaskThread_Parm_Init();
//	#endif
}

void TIM6_IRQHandler(void) //TIM6�ж�
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{

		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		if(TaskThread_State!=taskthread_emergency) //�����ڽ�����ͣ״̬
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
			if(error_type) //���ʱ��Ϊ�������������ֹͣ
			{
				//ȷ������������Ϊ����������
				Emergency_TaskThread();		
				GE.subtask=0;
				GE.sta=Ready;
				GE.task=GE_error;

				GC.subtask=0;
				GC.sta=Ready;
				GC.task=GC_error;

				GP.subtask=0;
				GP.sta=Ready;
				GP.task=GP_error;

				GO.subtask=0;
				GO.sta=Ready;
				GO.task=GO_error;  
				
				TaskThread_State=taskthread_error;
				
			}
		}
	}

}


//��ͣ����
void Pause_TaskThread(void)
{
	TaskThread_State=taskthread_pause;
}

//������ͣ����
void Emergency_TaskThread(void)
{
	//ֹͣ���е��������
	stepperMotorStop(&GE_motor_struct);
	stepperMotorStop(&GC_ver_motor_struct);
	stepperMotorStop(&GC_rot_motor_struct);
	stepperMotorStop(&GP_motor_struct);
	stepperMotorStop(&GO_hor_motor_struct);
	stepperMotorStop(&GO_ver_motor_struct);
	
	TaskThread_State=taskthread_emergency; //����״̬Ϊ����ֹͣ״̬
}
//����ͣ�����лָ�����
void Resume_TaskThread(void)
{

		GE.sta=Finish;
		GE.task=GE.resume_task;
		GC.sta=Finish;
		GC.task=GC_rot_down;
		GP.sta=Finish;
		GP.task=GP_start;
		GO.sta=Finish;
		GO.task=GOH_start;
	
		TaskThread_State=taskthread_running;
}


//�Ӵ�����ͣ�лָ�����
void Resume_Error_TaskThread(void)
{
	//���ݲ�ͬ�Ĵ������ý�������ָ����к�ִ�е�����

	//ȱ�ٸǲ�Ƭ ȱ���罺ͷ
	if(error_type&(Error_Cover_Glass+Error_Spray+Error_Sucker))
	{
		error_type&=~(Error_Cover_Glass+Error_Spray+Error_Sucker); //�����Ӧ������
		GP.task=GP.resume_task;
		GP.sta=Ready;
	}

	//ȱ�ٴ洢�� �洢����
	if(error_type&(Error_Out_Box+Error_StorageFull))
	{
		error_type&=~(Error_Out_Box+Error_StorageFull);
		GO.task=GO.resume_task;
		GO.sta=Ready;
	}
	//��ȡʧ��
	if(error_type&Error_Grap)
	{
		error_type&=~(Error_Grap);
		GC.task=GC.resume_task;
		GC.sta=Ready;
	}
	
		TaskThread_State=taskthread_running;
}

//����豸״̬�Ƿ����о���
enum taskthread_state TaskThread_IsReady(void)
{
	//�����ѹ�Ƿ�ﵽ����״̬
	if(Gas_State!=gas_pumped)
	{
		return taskthread_boost;
	}
	//���������λ���Ƿ�У׼
	if(GE_motor_struct.postion<0||GC_rot_motor_struct.postion<0||
		GC_ver_motor_struct.postion<0||GP_motor_struct.postion<0||
		GO_hor_motor_struct.postion<0||GO_ver_motor_struct.postion<0)
		{
			return taskthread_boost;
		}	
	
	//����Ƿ���ڴ洢��
	if(GOV_box_Sen!=Sen_Block)
	{
		error_type|=Error_Out_Box; 
	}else
	{
		error_type&=~Error_Out_Box;
	}
	//����Ƿ���ڵν�ͷ
	if(GP_spray_Sen!=Sen_Block)
	{
		error_type|=Error_Spray;
	}else
	{
		error_type&=~Error_Spray;
	}
	//δ���ô洢��
	if(GO.Storage_full==TRUE)
	{
		error_type|=Error_StorageFull;
	}else
	{
		error_type&=~Error_StorageFull;
	}
	
	if(error_type>Error_OverTime) 
	{
		Error_Mesg_Send(); //�ϱ�����
		return taskthread_error;
	}

	return taskthread_ready;
	
}

//������λ����
void Boot_ResetTaskThread(void)
{
	//��մ��󾯱�
	error_type=0;
	sensor_error_idx=0;
	//������λ����

	if(Gas_State==gas_pumped)
	{
		GE.subtask=0;
		GE.sta=Ready;
		GE.task=GE_reset_on;
		
		GC.subtask=0;
		GC.sta=Ready;
		GC.task=GC_reset_on;
		
		GP.subtask=0;
		GP.sta=Ready;
		GP.task=GP_reset_on;
		
		GO.subtask=0;
		GO.sta=Ready;
		GO.task=GO_reset_on;
	}
	TaskThread_State=taskthread_boost;
}

//��ʼ��������
void Start_TaskThread(void)
{
	//���ϵͳ�Ƿ����
	if(TaskThread_IsReady()==taskthread_boost)
	{
		if(TaskThread_CheckIdle())
		{
			Boot_ResetTaskThread();
		}
		return;
	}else if(TaskThread_IsReady()==taskthread_error)
	{
		return;
	}else
	{
		TaskThread_State=taskthread_ready;
	}
	
	
	if(TaskThread_State==taskthread_ready)
	{
		GE.subtask=0;
		GE.sta=Ready;
		GE.task=GE_start;

		GC.subtask=0;
		GC.sta=Ready;
		GC.task=GC_start;

		GP.subtask=0;
		GP.sta=Ready;
		GP.task=GP_start;

		GO.subtask=0;
		GO.sta=Ready;
		GO.task=GO_start;

		TaskThread_State=taskthread_running;
	}

}
//�ر���������
void Close_TaskThread(void)
{
	if(TaskThread_State!=taskthread_running) //�����������״̬ ֱ�ӹػ���λ
	{
		GE.subtask=0;
		GE.sta=Ready;
		GE.task=GE_reset_off;

		GC.subtask=0;
		GC.sta=Ready;
		GC.task=GC_reset_off;

		GP.subtask=0;
		GP.sta=Ready;
		GP.task=GP_reset_off;

		GO.subtask=0;
		GO.sta=Ready;
		GO.task=GO_reset_off;
	}

	TaskThread_State=taskthread_close;

}