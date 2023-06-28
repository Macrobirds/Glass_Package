#include "Globalconfig.h"

#define TASK_PRIORIRY 3

/*任务线程结构体初始化*/
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
	.Storage_full = TRUE, // 默认存储器满 需要通过出槽入槽重新装填来更新存储器状态
	.main_subtask = 0,
	.main_task = GO_none,
	.GOH_waste_pos = 268 * 36 + 9210 + 400,
};

enum taskthread_state TaskThread_State = taskthread_boost; // 任务线程运行状态
volatile enum task_error error_type = Error_none;

// 任务线程是否空闲
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

// 发送错误码信息
void Error_Mesg_Send(void)
{
	// 发送错误信息
	/************************/
	printf("error:%d error_sensor%d\r\n", error_type, sensor_error_idx);
	// 传感器错误
	if (error_type & Error_Sensor)
	{
		ack_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_sensor, sensor_error_idx);
		return;
	}
	// 缺少盖玻片
	if (error_type & Error_Cover_Glass)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_coverglass);
		return;
	}
	// 缺少喷胶头
	if (error_type & Error_Spray)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_spray);
		return;
	}
	// 缺少存储盒
	if (error_type & Error_Out_Box)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_OutBox);
		return;
	}
	// 存储盒满
	if (error_type & Error_StorageFull)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_full);
		return;
	}
	// 夹取失败
	if (error_type & Error_Grap)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_grap);
		return;
	}
	// 吸取玻片失败
	if (error_type & Error_Sucker)
	{
		ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, Extra_error_suck);
		return;
	}
	/***********************/
}

void Error_OverTime_Set(enum Error error_task, u8 task_index)
{
	TaskThread_Emergency(); // 进入紧急停止
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
	// 发送错误信息
	/***************/
	ack_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_error, error_task, task_index);
	/***************/
}

// 错误码设置 进入错误处理任务
void Error_Set(enum task_error error, u8 error_sen)
{
#ifndef DEBUG_MODE

	// 更新报错警报状态
	error_type |= error;
	sensor_error_idx = error_sen;

	// 如果为传感器错误或者任务超时错误 需要紧急停止任务
	if (error_type & (Error_Sensor))
	{
		TaskThread_Emergency(); // 紧急停止
		Error_Mesg_Send();
		// 进入报错程序 处理
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
	// 根据不同的错误设置解除错误后恢复运行后执行的任务
	// 缺少盖玻片
	if (error_type & Error_Cover_Glass)
	{
		GP.resume_task = GP_sucker_down;
		GP.sta = Ready;
		GP.task = GP_error;
		//开启缺少盖玻片蜂鸣器提醒
		if(Global_Parm.Warn->GP_cover_buzzer)
		{
			Buzzer=BUZZER_ENABLE;
		}
	}
	// 缺少喷胶头
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
	// 缺少存储盒
	if (error_type & Error_Out_Box)
	{
		GO.resume_task = GO.task;
		GO.sta = Ready;
		GO.task = GO_error;
	}
	// 存储盒满
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
	// 夹取失败
	if (error_type & Error_Grap)
	{
		GC.resume_task = GC_move_down;
		GC.sta = Ready;
		GC.task = GC_error;
	}
	// 吸取玻片失败
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

	TaskThread_State = taskthread_error; // 进入错误处理任务

#else
	printf("Errror:%d,Error_sensor:%d", error, error_sen);
	stepperMotorStop(&GE_motor_struct);
	stepperMotorStop(&GC_ver_motor_struct);
	stepperMotorStop(&GC_rot_motor_struct);
	stepperMotorStop(&GP_motor_struct);
	stepperMotorStop(&GO_hor_motor_struct);
	stepperMotorStop(&GO_ver_motor_struct);
	// 进入报错程序 处理
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

	// 初始化电机参数
	motor_parameter_Init();
	// 初始化任务参数
	TaskThread_Parm_Init();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); // 时钟使能

	// 定时器TIM6初始化
	TIM_TimeBaseStructure.TIM_Period = 10 - 1;					// 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;				// 设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);				// 根据指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); // 使能指定的TIM6中断,允许更新中断

	// 中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;						  // TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TASK_PRIORIRY; // 先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					  // 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						  // IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);										  // 初始化NVIC寄存器
	TIM_Cmd(TIM6, ENABLE);
}

void TIM6_IRQHandler(void) // TIM6中断
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{

		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		if (TaskThread_State != taskthread_emergency) // 不等于紧急暂停状态
		{
			// GE 玻片入料任务线程
			GE_TaskThread();
			// GC 玻片夹手任务线程
			GC_TaskThread();
			// GP 玻片封装任务线程
			GP_TaskThread();
			// GO 玻片存储任务线程
			GO_TaskThread();
		}
		else
		{
			if (error_type) // 如果时因为发生错误而紧急停止
			{
				// 确保将任务设置为错误处理任务
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

// 暂停运行
u8 TaskThread_Pause(void)
{
	if (TaskThread_State == taskthread_running)
	{
		TaskThread_State = taskthread_pause;
		return TRUE;
	}

	return FALSE;
}

// 紧急暂停运行
void TaskThread_Emergency(void)
{
	// 停止所有电机的运行
	stepperMotorStop(&GE_motor_struct);
	stepperMotorStop(&GC_ver_motor_struct);
	stepperMotorStop(&GC_rot_motor_struct);
	stepperMotorStop(&GP_motor_struct);
	stepperMotorStop(&GO_hor_motor_struct);
	stepperMotorStop(&GO_ver_motor_struct);
#ifdef BIG_CYLINDER_MOTOR
	stepperMotorStop(&GP_motor_big_cyl);
#endif

	TaskThread_State = taskthread_emergency; // 设置状态为紧急停止状态
}
// 从暂停任务中恢复运行
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

// 从错误暂停中恢复运行
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

	// 根据不同的错误设置解除错误后恢复运行后执行的任务
	TIM_Cmd(TIM6, DISABLE);
	// 缺少盖玻片 缺少喷胶头
	if (error_type & (Error_Cover_Glass + Error_Spray))
	{
		error_type &= ~(Error_Cover_Glass + Error_Spray); // 清除对应错误码
		GP.task = GP.resume_task;
		GP.sta = Ready;
		GP.subtask = 0;
	}
	// 吸取盖玻片错误
	if (error_type & (Error_Sucker))
	{
		error_type &= ~(Error_Sucker); // 清除对应错误码
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

	// 缺少存储盒 存储盒满
	if (error_type & (Error_Out_Box + Error_StorageFull))
	{
		error_type &= ~(Error_Out_Box + Error_StorageFull);
		GO.task = GO.resume_task;
		GO.sta = Ready;
		GO.subtask = 0;
	}
	// 夹取失败
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

// 检测设备状态是否运行就绪
enum taskthread_state TaskThread_IsReady(void)
{
	// 检测气压是否达到工作状态
	if (Gas_State != gas_pumped)
	{
		return taskthread_boost;
	}
	// 检测各个电机位置是否校准
	if (GE_motor_struct.postion < 0 || GC_rot_motor_struct.postion < 0 ||
		GC_ver_motor_struct.postion < 0 || GP_motor_struct.postion < 0 ||
		GO_hor_motor_struct.postion < 0 || GO_ver_motor_struct.postion < 0)
	{
		return taskthread_boost;
	}

	// 检测是否存在存储盒
	if (GOV_box_Sen != Sen_Block)
	{
		error_type |= Error_Out_Box;
	}
	else
	{
		error_type &= ~Error_Out_Box;
		// 未放置存储盒
		if (GO.Storage_full == TRUE)
		{
			error_type |= Error_StorageFull;
		}
		else
		{
			error_type &= ~Error_StorageFull;
		}
	}
	// 检测是否存在滴胶头
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
		Error_Mesg_Send(); // 上报错误
		return taskthread_error;
	}

	return taskthread_ready;
}

// 开机复位任务
u8 TaskThread_BootReset(void)
{
	// 任务状态为紧急停止状态
	if (TaskThread_State == taskthread_emergency)
	{
		return FALSE;
	}
	// 任务状态是否空闲
	if (!TaskThread_CheckIdle())
	{
		return FALSE;
	}
	// 气阀状态初始化
	Main_out_Cyl = GAS_DISABLE;
#ifdef BIG_CYLINDER
	GP_big_Cyl = GAS_DISABLE; // 大气缸推出
#endif
	GP_small_Cyl = GAS_DISABLE; // 小气缸推回
	GP_sucker1_Cyl = GAS_DISABLE;
	GP_sucker2_Cyl = GAS_DISABLE;
	GP_sucker_Pump=SUCKER_DISABLE;//GP_sucker_Pump = GAS_DISABLE;
	GP_spray_Cyl = GAS_DISABLE;
	GC_claw_Cyl = GAS_DISABLE;
	// 开启复位任务
	TIM_Cmd(TIM6, DISABLE);
	// 清空错误警报
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
	// 任务状态为启动状态
	TaskThread_State = taskthread_boost;
	return TRUE;
}

// 开始运行任务
u8 TaskThread_Start(void)
{
	if (TaskThread_State == taskthread_emergency)
	{
		return FALSE;
	}

	// 检测系统是否就绪
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
// 关闭运行任务
u8 TaskThread_Close(void)
{
	if (TaskThread_State != taskthread_running) // 如果不在运行状态 直接关机复位
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

// 装载槽进入
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
// 装载槽推出
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

// 存储盒进入
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
// 存储盒推出
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
