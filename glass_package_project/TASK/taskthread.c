#include "Globalconfig.h"

#define TASK_PRIORIRY 3

/*�����߳̽ṹ���ʼ��*/
struct glass_enter_struct GE = {
	.task = GE_none,
	.sta = Ready,
	.motor = &GE_motor_struct,
	.GE_box_len = 212 * 330,
	.GE_box_speed = 20 * 212,
	.glass_Exist = FALSE,
	.box_Exist = FALSE,
	.subtask = 0,

};

struct glass_claw_struct GC = {
	.task = GC_none,
	.sta = Ready,
	.motor_v = &GC_ver_motor_struct,
	.motor_r = &GC_rot_motor_struct,
#ifdef GC_ROT_PACKAGE
	.GCV_down_pos = 33300,
	.GCR_hor_pos = 130,
	.GCR_ver_pos = 1720,
#endif
#ifdef GC_VER_PACKAGE
	.GCV_down_pos = 33300 + 900,
	.GCR_hor_pos = 1645,
	.GCR_ver_pos = 45,
#endif

	.GCV_glass_len = 16000,
	.main_subtask = 0,
	.main_task = GC_none,
	.Glass_Ready = FALSE,
	.GCV_pacakge_pos = 880};

#ifdef BIG_CYLINDER_MOTOR
struct glass_package_struct GP = {
	.task = GP_none,
	.sta = Ready,
	.motor = &GP_motor_struct,
	.motor_cyl = &GP_motor_big_cyl,
	.sucker_pos = 20000 + 268 * 5,
	.spray_pos = 19000,
	.spray_len = 268 * 15,
	.spray_speed = 268 * 30,
	.sucker_down_pos = 1,
	.sucker_package_pos = 1,
	.sucker_finish_pos = 1,
	.delay_before = 420,
	.delay_after = 120,
	.spray_pressure = 3,
	.main_subtask = 0,
	.main_task = GP_none,
};
#else
struct glass_package_struct GP = {
	.task = GP_none,
	.sta = Ready,
	.motor = &GP_motor_struct,
	.sucker_pos = 20000 + 268 * 10,
	.spray_pos = 19000,
	.spray_len = 268 * 15,
	.spray_speed = 268 * 30,
	.delay_before = 800,
	.delay_after = 120,
	.spray_pressure = 3,
	.main_subtask = 0,
	.main_task = GP_none,
	.cyl_pos_flag=FALSE,
};
#endif

struct glass_out_struct GO = {
	.task = GO_none,
	.sta = Ready,
	.motor_h = &GO_hor_motor_struct,
	.motor_v = &GO_ver_motor_struct,
	.GOH_mid_pos = 9210 + 400,
	.GOH_end_pos = 268 * 210,
	.GOV_box_dis = 800 * 10,
	.GOV_box_len = 800 * 241 + 600,
	.GOV_slot_dis = 800 * 4,
	.GOV_adjust = 600,
	.GOV_adjust_start = 800,
	.Storage_full = TRUE, // Ĭ�ϴ洢���� ��Ҫͨ�������������װ�������´洢��״̬
	.main_subtask = 0,
	.main_task = GO_none,
	.GOH_waste_pos = 268 * 36 + 9210 + 400,
};

enum taskthread_state TaskThread_State = taskthread_boost; // �����߳�����״̬
volatile enum task_error error_type = Error_none;

// �����߳��Ƿ����
u8 TaskThread_CheckIdle(void)
{
	if (TaskThread_State == taskthread_emergency)
	{
		return FALSE;
	}

	if (GE.task == GE_none && GC.task == GC_none &&
		GP.task == GP_none && GO.task == GO_none)
	{
		return TRUE;
	}
	else if (GE.task == GE_finish && GC.task == GC_finish &&
			 GP.task == GP_finish && GO.task == GO_finish)
	{
		return TRUE;
	}

	if (TaskThread_State == taskthread_boost || TaskThread_State == taskthread_pause || TaskThread_State == taskthread_finsih )
	{
		if (GE.motor->motion == Stop && GC.motor_r->motion == Stop && GC.motor_v->motion == Stop && GP.motor->motion == Stop && GO.motor_h->motion == Stop && GO.motor_v->motion == Stop)
		{
			return TRUE;
		}
		
	}

	return FALSE;
}

// ���ʹ�������Ϣ
void Error_Mesg_Send(void)
{
	// ���ʹ�����Ϣ
	/************************/
	printf("error:%d error_sensor%d\r\n", error_type, sensor_error_idx);
	// ����������
	if (error_type & Error_Sensor)
	{
		ack_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_sensor, sensor_error_idx);
		return;
	}
	// ȱ�ٸǲ�Ƭ
	if (error_type & Error_Cover_Glass)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_coverglass);
		return;
	}
	// ȱ���罺ͷ
	if (error_type & Error_Spray)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_spray);
		return;
	}
	// ȱ�ٴ洢��
	if (error_type & Error_Out_Box)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_OutBox);
		return;
	}
	// �洢����
	if (error_type & Error_StorageFull)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_full);
		return;
	}
	// ��ȡʧ��
	if (error_type & Error_Grap)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_grap);
		return;
	}
	// ��ȡ��Ƭʧ��
	if (error_type & Error_Sucker)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_suck);
		return;
	}
	/***********************/
}

void Error_OverTime_Set(enum Error error_task, u8 task_index)
{
	TaskThread_Emergency(); // �������ֹͣ
	GE.subtask = 0;
	GE.sta = Ready;
	GE.task = GE_error;

	GC.subtask = 0;
	GC.sta = Ready;
	GC.task = GC_error;

	GP.subtask = 0;
	GP.sta = Ready;
	GP.task = GP_error;

	GO.subtask = 0;
	GO.sta = Ready;
	GO.task = GO_error;

	error_type |= Error_OverTime;
	// ���ʹ�����Ϣ
	/***************/
	ack_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, error_task, task_index);
	/***************/
}

// ���������� �������������
void Error_Set(enum task_error error, u8 error_sen)
{
#ifndef DEBUG_MODE

	// ���±�����״̬
	error_type |= error;
	sensor_error_idx = error_sen;

	// ���Ϊ�����������������ʱ���� ��Ҫ����ֹͣ����
	if (error_type & (Error_Sensor))
	{
		TaskThread_Emergency(); // ����ֹͣ
		Error_Mesg_Send();
		// ���뱨����� ����
		GE.subtask = 0;
		GE.sta = Finish;
		GE.task = GE_error;

		GC.subtask = 0;
		GC.sta = Finish;
		GC.task = GC_error;

		GP.subtask = 0;
		GP.sta = Finish;
		GP.task = GP_error;

		GO.subtask = 0;
		GO.sta = Finish;
		GO.task = GO_error;
		return;
	}

	Error_Mesg_Send();
	// ���ݲ�ͬ�Ĵ������ý�������ָ����к�ִ�е�����
	// ȱ�ٸǲ�Ƭ
	if (error_type & Error_Cover_Glass)
	{
		GP.resume_task = GP_sucker_down;
		GP.sta = Ready;
		GP.task = GP_error;
		//����ȱ�ٸǲ�Ƭ����������
		if(Global_Parm.Warn->GP_cover_buzzer)
		{
			Buzzer=BUZZER_ENABLE;
		}
	}
	// ȱ���罺ͷ
	if (error_type & Error_Spray)
	{
		GP.resume_task = GP.task;
		GP.sta = Ready;
		GP.task = GP_error;
		if(Global_Parm.Warn->GP_spray_buzzer)
		{
			Buzzer=BUZZER_ENABLE;
		}
	}
	// ȱ�ٴ洢��
	if (error_type & Error_Out_Box)
	{
		GO.resume_task = GO.task;
		GO.sta = Ready;
		GO.task = GO_error;
	}
	// �洢����
	if (error_type & Error_StorageFull)
	{
		GO.resume_task = GO.task;
		GO.sta = Ready;
		GO.task = GO_error;
		if(Global_Parm.Warn->GO_storage_buzzer)
		{
			Buzzer=BUZZER_ENABLE;
		}
	}
	// ��ȡʧ��
	if (error_type & Error_Grap)
	{
		GC.resume_task = GC_move_down;
		GC.sta = Ready;
		GC.task = GC_error;
	}
	// ��ȡ��Ƭʧ��
	if (error_type & Error_Sucker)
	{
		GP.resume_task = GP_start;
		GP.sta = Ready;
		GP.task = GP_error;
		GP.subtask = 0;

		GO.resume_task = GO_start;
		GO.sta = Ready;
		GO.task = GO_error;
		GO.subtask = 0;

		if (GC.task > GC_move_up)
		{
			GC.resume_task = GC_move_up;
			GC.sta = Ready;
			GC.task = GC_error;
			GC.subtask = 0;
		}
		if(Global_Parm.Warn->GP_sucker_buzzer)
		{
			Buzzer=BUZZER_ENABLE;
		}
	}

	TaskThread_State = taskthread_error; // �������������

#else
	printf("Errror:%d,Error_sensor:%d", error, error_sen);
	stepperMotorStop(&GE_motor_struct);
	stepperMotorStop(&GC_ver_motor_struct);
	stepperMotorStop(&GC_rot_motor_struct);
	stepperMotorStop(&GP_motor_struct);
	stepperMotorStop(&GO_hor_motor_struct);
	stepperMotorStop(&GO_ver_motor_struct);
	// ���뱨����� ����
	GE.subtask = 0;
	GE.sta = Ready;
	GE.task = GE_none;

	GC.subtask = 0;
	GC.sta = Ready;
	GC.task = GC_none;

	GP.subtask = 0;
	GP.sta = Ready;
	GP.task = GP_none;

	GO.subtask = 0;
	GO.sta = Ready;
	GO.task = GO_none;

#endif
}

void TaskThread_Parm_Init(void)
{
	if (TaskThread_State != taskthread_running)
	{
		// set GE task parm
		GE.GE_box_len = Global_Parm.GIO->GE_box_len * GE_motor_struct.pulse_1mm / SCALE;
		GE.GE_box_speed = Global_Parm.GIO->GE_box_speed * GE_motor_struct.pulse_1mm;
		// set GC task parm
		GC.GCR_hor_pos = Global_Parm.GCS->GCR_hor_pos;
		GC.GCR_ver_pos = Global_Parm.GCS->GCR_ver_pos;
		GC.GCV_down_pos = Global_Parm.GCS->GCV_down_pos * GC_ver_motor_struct.pulse_1mm / SCALE;
		GC.GCV_glass_len = Global_Parm.GCS->GCV_glass_len * GC_ver_motor_struct.pulse_1mm / SCALE;
		GC.GCV_pacakge_pos = Global_Parm.GCS->GCV_package_pos;
		// set GP parm
		GP.delay_after = Global_Parm.GP->delay_after;
		GP.delay_before = Global_Parm.GP->delay_before;
		GP.sucker_pos = Global_Parm.GP->sucker_pos * GP_motor_struct.pulse_1mm / SCALE;
		GP.spray_pos = Global_Parm.GP->spray_pos * GP_motor_struct.pulse_1mm / SCALE;
		GP.spray_len = Global_Parm.GP->spray_len * GP_motor_struct.pulse_1mm / SCALE;
		GP.spray_speed = Global_Parm.GP->spray_speed * GP_motor_struct.pulse_1mm;
		GP.spray_pressure = Global_Parm.GP->spray_pressure;
		// set GO parm
		GO.GOH_end_pos = Global_Parm.GCS->GOH_end_pos * GO_hor_motor_struct.pulse_1mm / SCALE;
		GO.GOH_mid_pos = Global_Parm.GCS->GOH_mid_pos * GO_hor_motor_struct.pulse_1mm / SCALE;
		GO.GOV_box_dis = Global_Parm.GIO->GOV_box_dis * GO_ver_motor_struct.pulse_1mm / SCALE;
		GO.GOV_slot_dis = Global_Parm.GIO->GOV_slot_dis * GO_ver_motor_struct.pulse_1mm / SCALE;
		GO.GOV_box_len = Global_Parm.GIO->GOV_box_len * GO_ver_motor_struct.pulse_1mm / SCALE;
		GO.GOV_adjust = Global_Parm.GIO->GOV_adjust;
		GO.GOV_adjust_start = Global_Parm.GIO->GOV_adjust_start;
	}
}

void TaskThread_Init(void)
{
	printf("TaskThread_Init\r\n");
	// Init TIM6 Interrupt  1ms
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// ��ʼ���������
	motor_parameter_Init();
	// ��ʼ���������
	TaskThread_Parm_Init();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); // ʱ��ʹ��

	// ��ʱ��TIM6��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 10 - 1;					// ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;				// ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);				// ����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); // ʹ��ָ����TIM6�ж�,��������ж�

	// �ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;						  // TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TASK_PRIORIRY; // ��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					  // �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						  // IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);										  // ��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIM6, ENABLE);
}

void TIM6_IRQHandler(void) // TIM6�ж�
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{

		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		if (TaskThread_State != taskthread_emergency) // �����ڽ�����ͣ״̬
		{
			// GE ��Ƭ���������߳�
			GE_TaskThread();
			// GC ��Ƭ���������߳�
			GC_TaskThread();
			// GP ��Ƭ��װ�����߳�
			GP_TaskThread();
			// GO ��Ƭ�洢�����߳�
			GO_TaskThread();
		}
		else
		{
			if (error_type) // ���ʱ��Ϊ�������������ֹͣ
			{
				// ȷ������������Ϊ����������
				TaskThread_Emergency();
				GE.subtask = 0;
				GE.sta = Ready;
				GE.task = GE_error;

				GC.subtask = 0;
				GC.sta = Ready;
				GC.task = GC_error;

				GP.subtask = 0;
				GP.sta = Ready;
				GP.task = GP_error;

				GO.subtask = 0;
				GO.sta = Ready;
				GO.task = GO_error;

				TaskThread_State = taskthread_error;
			}
		}
	}
}

// ��ͣ����
u8 TaskThread_Pause(void)
{
	if (TaskThread_State == taskthread_running)
	{
		TaskThread_State = taskthread_pause;
		return TRUE;
	}

	return FALSE;
}

// ������ͣ����
void TaskThread_Emergency(void)
{
	// ֹͣ���е��������
	stepperMotorStop(&GE_motor_struct);
	stepperMotorStop(&GC_ver_motor_struct);
	stepperMotorStop(&GC_rot_motor_struct);
	stepperMotorStop(&GP_motor_struct);
	stepperMotorStop(&GO_hor_motor_struct);
	stepperMotorStop(&GO_ver_motor_struct);
#ifdef BIG_CYLINDER_MOTOR
	stepperMotorStop(&GP_motor_big_cyl);
#endif

	TaskThread_State = taskthread_emergency; // ����״̬Ϊ����ֹͣ״̬
}
// ����ͣ�����лָ�����
u8 TaskThread_Resume(void)
{
	if (TaskThread_State == taskthread_emergency)
	{
		return FALSE;
	}

	if (!TaskThread_CheckIdle())
	{
		return FALSE;
	}

	TIM_Cmd(TIM6, DISABLE);
	GE.sta = Finish;
	GE.task = GE.resume_task;
	GC.sta = Finish;
	GC.task = GC_rot_down;
	GP.sta = Finish;
	GP.task = GP_start;
	GO.sta = Finish;
	GO.task = GO_start;
	TIM_Cmd(TIM6, ENABLE);
	TaskThread_State = taskthread_running;
	return TRUE;
}

// �Ӵ�����ͣ�лָ�����
u8 TaskThread_Resume_Error(void)
{
	if (TaskThread_State == taskthread_emergency)
	{
		printf("emergency error\r\n");
		return FALSE;
	}

	if (!TaskThread_Check_ErrorDone())
	{
		printf(" error not done\r\n");
		return FALSE;
	}

	if (error_type & (Error_Sensor + Error_OverTime))
	{
		printf("serious error\r\n");
		return FALSE;
	}

	// ���ݲ�ͬ�Ĵ������ý�������ָ����к�ִ�е�����
	TIM_Cmd(TIM6, DISABLE);
	// ȱ�ٸǲ�Ƭ ȱ���罺ͷ
	if (error_type & (Error_Cover_Glass + Error_Spray))
	{
		error_type &= ~(Error_Cover_Glass + Error_Spray); // �����Ӧ������
		GP.task = GP.resume_task;
		GP.sta = Ready;
		GP.subtask = 0;
	}
	// ��ȡ�ǲ�Ƭ����
	if (error_type & (Error_Sucker))
	{
		error_type &= ~(Error_Sucker); // �����Ӧ������
		GP.task = GP.resume_task;
		GP.sta = Ready;
		GP.subtask = 0;

		GO.task = GO.resume_task;
		GO.sta = Ready;
		GO.subtask = 0;

		if (GC.task == GC_error)
		{
			GC.task = GC.resume_task;
			GC.sta = Ready;
			GC.subtask = 0;
		}
	}

	// ȱ�ٴ洢�� �洢����
	if (error_type & (Error_Out_Box + Error_StorageFull))
	{
		error_type &= ~(Error_Out_Box + Error_StorageFull);
		GO.task = GO.resume_task;
		GO.sta = Ready;
		GO.subtask = 0;
	}
	// ��ȡʧ��
	if (error_type & Error_Grap)
	{
		error_type &= ~(Error_Grap);
		GC.task = GC.resume_task;
		GC.sta = Ready;
		GC.subtask = 0;
	}
	TIM_Cmd(TIM6, ENABLE);
	TaskThread_State = taskthread_running;
	Buzzer=BUZZER_DISABLE;
	return TRUE;
}

// ����豸״̬�Ƿ����о���
enum taskthread_state TaskThread_IsReady(void)
{
	// �����ѹ�Ƿ�ﵽ����״̬
	if (Gas_State != gas_pumped)
	{
		return taskthread_boost;
	}
	// ���������λ���Ƿ�У׼
	if (GE_motor_struct.postion < 0 || GC_rot_motor_struct.postion < 0 ||
		GC_ver_motor_struct.postion < 0 || GP_motor_struct.postion < 0 ||
		GO_hor_motor_struct.postion < 0 || GO_ver_motor_struct.postion < 0)
	{
		return taskthread_boost;
	}

	// ����Ƿ���ڴ洢��
	if (GOV_box_Sen != Sen_Block)
	{
		error_type |= Error_Out_Box;
	}
	else
	{
		error_type &= ~Error_Out_Box;
		// δ���ô洢��
		if (GO.Storage_full == TRUE)
		{
			error_type |= Error_StorageFull;
		}
		else
		{
			error_type &= ~Error_StorageFull;
		}
	}
	// ����Ƿ���ڵν�ͷ
	if (GP_spray_Sen != Sen_Block)
	{
		error_type |= Error_Spray;
	}
	else
	{
		error_type &= ~Error_Spray;
	}

	if (error_type > Error_OverTime)
	{
		Error_Mesg_Send(); // �ϱ�����
		return taskthread_error;
	}

	return taskthread_ready;
}

// ������λ����
u8 TaskThread_BootReset(void)
{
	// ����״̬Ϊ����ֹͣ״̬
	if (TaskThread_State == taskthread_emergency)
	{
		return FALSE;
	}
	// ����״̬�Ƿ����
	if (!TaskThread_CheckIdle())
	{
		return FALSE;
	}
	// ����״̬��ʼ��
	Main_out_Cyl = GAS_DISABLE;
#ifdef BIG_CYLINDER
	GP_big_Cyl = GAS_DISABLE; // �������Ƴ�
#endif
	GP_small_Cyl = GAS_DISABLE; // С�����ƻ�
	GP_sucker1_Cyl = GAS_DISABLE;
	GP_sucker2_Cyl = GAS_DISABLE;
	GP_sucker_Pump=SUCKER_DISABLE;//GP_sucker_Pump = GAS_DISABLE;
	GP_spray_Cyl = GAS_DISABLE;
	GC_claw_Cyl = GAS_DISABLE;
	// ������λ����
	TIM_Cmd(TIM6, DISABLE);
	// ��մ��󾯱�
	error_type = 0;
	sensor_error_idx = 0;

	GE.subtask = 0;
	GE.sta = Ready;
	GE.task = GE_reset_on;

	GC.subtask = 0;
	GC.sta = Ready;
	GC.task = GC_reset_on;

	GP.subtask = 0;
	GP.sta = Ready;
	GP.task = GP_reset_on;

	GO.subtask = 0;
	GO.sta = Ready;
	GO.task = GO_reset_on;
	TIM_Cmd(TIM6, ENABLE);
	// ����״̬Ϊ����״̬
	TaskThread_State = taskthread_boost;
	return TRUE;
}

// ��ʼ��������
u8 TaskThread_Start(void)
{
	if (TaskThread_State == taskthread_emergency)
	{
		return FALSE;
	}

	// ���ϵͳ�Ƿ����
	if (TaskThread_IsReady() != taskthread_ready)
	{
		return FALSE;
	}
	else
	{
		TIM_Cmd(TIM6, DISABLE);
		GE.subtask = 0;
		GE.sta = Ready;
		GE.task = GE_start;

		GC.subtask = 0;
		GC.sta = Ready;
		GC.task = GC_start;

		GP.subtask = 0;
		GP.sta = Ready;
		GP.task = GP_start;

		GO.subtask = 0;
		GO.sta = Ready;
		GO.task = GO_start;
		TIM_Cmd(TIM6, ENABLE);
		TaskThread_State = taskthread_running;
		return TRUE;
		Buzzer=BUZZER_DISABLE;
	}

	return FALSE;
}
// �ر���������
u8 TaskThread_Close(void)
{
	if (TaskThread_State != taskthread_running) // �����������״̬ ֱ�ӹػ���λ
	{
		TIM_Cmd(TIM6, DISABLE);
		GE.subtask = 0;
		GE.sta = Ready;
		GE.task = GE_reset_off;

		GC.subtask = 0;
		GC.sta = Ready;
		GC.task = GC_reset_off;

		GP.subtask = 0;
		GP.sta = Ready;
		GP.task = GP_reset_off;

		GO.subtask = 0;
		GO.sta = Ready;
		GO.task = GO_reset_off;
		TIM_Cmd(TIM6, ENABLE);
		TaskThread_State = taskthread_close;
		Buzzer=BUZZER_DISABLE;
		return TRUE;
		
	}
	return FALSE;
}

// װ�ز۽���
u8 TaskThread_GEIn(void)
{
	if (TaskThread_State == taskthread_running)
	{
		return FALSE;
	}
	if (GE.task == GE_BOx_Out && GE.sta == Finish)
	{
		TIM_Cmd(TIM6, DISABLE);
		GE.sta = Ready;
		GE.task = GE_Box_In;
		TIM_Cmd(TIM6, ENABLE);
		return TRUE;
	}
	return FALSE;
}
// װ�ز��Ƴ�
u8 TaskThread_GEOut(void)
{
	if (TaskThread_State == taskthread_running)
	{
		return FALSE;
	}
	if (TaskThread_CheckIdle())
	{
		TIM_Cmd(TIM6, DISABLE);
		GE.sta = Ready;
		GE.task = GE_BOx_Out;
		TIM_Cmd(TIM6, ENABLE);
		return TRUE;
	}

	return FALSE;
}

// �洢�н���
u8 TaskThread_GOIn(void)
{
	if (TaskThread_State == taskthread_running)
	{
		return FALSE;
	}
	if (GO.task == GO_Box_Out && GO.sta == Finish)
	{
		TIM_Cmd(TIM6, DISABLE);
		GO.sta = Ready;
		GO.task = GO_Box_In;
		TIM_Cmd(TIM6, ENABLE);
		return TRUE;
	}
	return FALSE;
}
// �洢���Ƴ�
u8 TaskThread_GOOut(void)
{
	if (TaskThread_State == taskthread_running)
	{
		return 0;
	}
	if (TaskThread_CheckStop())
	{
		TIM_Cmd(TIM6, DISABLE);
		GO.sta = Ready;
		GO.task = GO_Box_Out;
		TIM_Cmd(TIM6, ENABLE);
		return 1;
	}

	return 0;
}

u8 TaskThread_Check_ErrorDone(void)
{
	if (TaskThread_State == taskthread_error)
	{
		if ((GE.sta == Finish || GE.task == GE_finish) && GC.sta == Finish &&
			GP.sta == Finish && GO.sta == Finish)
		{
			return TRUE;
		}

		if (error_type | Error_StorageFull)
		{
			if (GO.motor_v->motion == Stop)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

u8 TaskThread_CheckStop(void)
{

	if (TaskThread_CheckIdle())
	{
		return TRUE;
	}
	else
	{
		if (TaskThread_Check_ErrorDone())
		{
			return TRUE;
		}
	}

	return FALSE;
}
