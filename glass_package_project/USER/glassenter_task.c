#include "Globalconfig.h"

// 夹手位于垂直原点，垂直静止
static u8 check_GC(void)
{
	return (GC_ver_Sen == Sen_Block) && (GC.motor_v->motion == Stop);
}

void GE_ReadyTask(void)
{

	switch (GE.task)
	{
	case GE_none:
		break;
	/////////////////开机复位//////////////////////////////
	case GE_reset_on:
		if (GE.subtask == 0)
		{
			if (GE_start_Sen == Sen_Block) // 原点感应  离开感应器 再回到原点
			{
				GE.motor->dir = Front;
				motor_Set_Direction(GE.motor);
				motorGO_Debug(GE.motor, GE.motor->pulse_1mm * 10, 0);
				GE.sta = Running;
			}
			else
			{
				GE.subtask = 1;
			}
		}
		else if (GE.subtask == 1)
		{
			if (check_GC())
			{
				motorGo(GE.motor, 0, 0);
				GE.sta = Running;
			}
		}
		break;
	/////////////关机复位///////////////////////////
	case GE_reset_off:
		if (check_GC())
		{
			motorGo(GE.motor, 0, 0);
			GE.sta = Running;
		}
		break;
	/////////////////装载槽进入//////////////////////////////
	case GE_Box_In:
		if (GE_start_Sen == Sen_Block) // 原点感应 装载槽已进入
		{
			stepperMotorStop(GE.motor);
			GE.motor->postion = 0;
			GE.sta = Finish;
		}
		else
		{
			if (check_GC()) // 前往原点
			{
				motorGo(GE.motor, 0, 0);
				GE.sta = Running;
			}
		}
		break;
	/////////////////装载槽推出//////////////////////////////
	case GE_BOx_Out:
		if (check_GC())
		{
			motorGo_acc(GE.motor, GE.motor->max_pos - 100);
			GE.sta = Running;
		}

		break;

	/////////////////////装载槽复位到原点//////////////////////////////////
	case GE_start:
		GE.box_Exist = FALSE;
		if (GE_start_Sen == Sen_Block)
		{
			stepperMotorStop(GE.motor);
			GE.motor->postion = 0;
			GE.sta = Finish;
		}
		else
		{
			if (check_GC()) // 夹手位于垂直原点，垂直静止
			{
				motorGo(GE.motor, 0, 0);
				GE.sta = Running;
			}
		}
		break;
	////////////////////装载槽前端移动到夹手正下方////////////////////////
	case GE_move_front:
		if (GE_up_Sen == Sen_Block)
		{
			GE.sta = Finish;
		}
		else
		{
			if (check_GC()) // 夹手位于垂直原点，垂直静止
			{
				motorGo(GE.motor, (GE.GE_box_len) * 2 - (50 * GE.motor->pulse_1mm), 0);
				GE.sta = Running;
			}
		}
		break;
	//////////////////////玻片移动到夹手正下方//////////////////////////////////
	case GE_move_glass:
		if (check_GC()) // 夹手位于垂直原点，垂直静止
		{
			GE.motor->dir = Front;
			motor_Set_Direction(GE.motor);
			motorGO_Debug(GE.motor, GE.GE_box_len, GE.GE_box_speed);
			GE.sta = Running;
		}
		break;
	///////////////////装载槽后端离开夹手下方////////////////////////////////////////////
	case GE_move_back:
		if (check_GC()) // 夹手位于垂直原点，垂直静止
		{
			GE.motor->dir = Front;
			motor_Set_Direction(GE.motor);
			motorGO_Debug(GE.motor, GE.motor->pulse_1mm * 30, 0);
			GE.sta = Running;
			GE.glass_Exist = FALSE;
		}
		break;
		///////////////////////////////进料任务完成//////////////////////////////
	case GE_finish:

		break;
	////////////////////////////////异常处理///////////////////////////////
	case GE_error:
		// 缺少装载槽

		// 装载槽缺少载玻片

		GE.sta = Finish;

		break;
	}
}

void GE_RunningTask(void)
{
	switch (GE.task)
	{
	case GE_none:
		break;
	/////////////////开机复位////////////////////////////////
	case GE_reset_on:
		if (GE.subtask == 0)
		{
			// 离开原点感应
			if (GE.motor->motion == Stop)
			{
				GE.sta = Ready;
				GE.subtask = 1;
			}
		}
		else if (GE.subtask == 1)
		{
			if (GE_start_Sen == Sen_Block)
			{
				GE.sta = Finish;
				GE.subtask = 0;
			}
			else
			{
				if (GE.motor->motion == Stop)
				{
					Error_Set(Error_Sensor, GE_start_sensor);
				}
			}
		}

		break;
	//////////////关机复位//////////////////////////
	case GE_reset_off:
		if (GE_start_Sen == Sen_Block)
		{
			GE.sta = Finish;
			GE.subtask = 0;
		}
		else
		{
			if (GE.motor->motion == Stop)
			{
				Error_Set(Error_Sensor, GE_start_sensor);
			}
		}
		break;
	/////////////////装载槽进入//////////////////////////////
	case GE_Box_In:
		if (GE_start_Sen == Sen_Block)
		{
			GE.sta = Finish;
		}
		else
		{
			if (GE.motor->motion == Stop)
			{
				Error_Set(Error_Sensor, GE_start_sensor);
			}
		}
		break;
	/////////////////装载槽推出//////////////////////////////
	case GE_BOx_Out:
		if (GE.motor->motion == Stop)
		{
			GE.sta = Finish;
		}
		break;
	/////////////////////装载槽复位到原点//////////////////////////////////
	case GE_start:
		if (GE_start_Sen == Sen_Block)
		{
			GE.sta = Finish;
		}
		else
		{
			if (GE.motor->motion == Stop) // GE原点传感器错误
			{
				Error_Set(Error_Sensor, GE_start_sensor);
			}
		}
		break;
	/////////////////////等待装载槽前端移动到夹手正下方///////////////////////////////
	case GE_move_front:
		if (GE.motor->motion == Stop) // GE 上下端传感器一直未感应
		{
			if (GE_up_Sen == Sen_Block)
			{
				GE.box_Exist = TRUE;
				GE.sta = Finish;
			}
			else
			{
				GE.sta = Ready;
				GE.task = GE_finish;
				TaskThread_State = taskthread_finsih;
			}
		}
		break;
	/////////////////////玻片移动到夹手正下方//////////////////////////////////
	case GE_move_glass:
		if (GE.motor->motion == Stop) // 传感器感应停止
		{
			if (GE_up_Sen != Sen_Block && GE_down_Sen == Sen_Block)
			{
				GE.sta = Finish;
				GE.glass_Exist = TRUE; // 检测到玻片
			}
			else if (GE_up_Sen == Sen_Block)
			{
				GE.sta = Finish;
				GE.glass_Exist = FALSE; // 到达尾部 未检测到玻片
			}
			else
			{
				Error_Set(Error_Sensor, GE_up_sensor | GE_down_sensor);
			}
		}
		break;
		/////////////////////装载槽后端离开夹手正下方//////////////////////////////////
	case GE_move_back:
		if (GE.motor->motion == Stop) // 上下端传感器错误
		{
			GE.sta = Finish;
		}
		break;

	case GE_finish:

		break;

	case GE_error:

		break;
	}
}

void GE_FinishTask(void)
{
	if (TaskThread_State == taskthread_pause) // 任务暂停 保存当前任务状态后跳转到结束任务
	{
		GE.resume_task = GE.task;
		GE.task = GE_finish;
		GE.sta = Ready;
	}

	switch (GE.task)
	{
	case GE_none:
		break;
	case GE_reset_on:
		if (TaskThread_CheckIdle())
		{
			/*发送复位完成的信息*/
			if (GE.WaitAck)
			{
				GE.WaitAck = 0;
				ack_task(GE.Index, Type_controlAck, Fc_run, Extra_run_Reset, AckResult_executeSucceed);
			}

			GE.task = GE_none;
			GE.sta = Ready;
			GE.subtask = 0;
		}
		break;
	case GE_reset_off:
		if (TaskThread_CheckIdle())
		{
			/*发送关机复位完成信息*/
			if (GE.WaitAck)
			{
				GE.WaitAck = 0;
				ack_task(GE.Index, Type_controlAck, Fc_run, Extra_run_Close, AckResult_executeSucceed);
			}

			GE.task = GE_none;
			GE.sta = Ready;
			GE.subtask = 0;
		}

		break;
	case GE_Box_In:
		GE.task = GE_none;
		GE.sta = Ready;
		break;
	case GE_BOx_Out: // 保留出盒状态
#ifdef DEBUG_MODE
		GE.task = GE_none;
		GE.sta = Ready;
#endif
		break;
	case GE_start: // 跳转至装载槽前端移动到夹手正下方
		GE.sta = Ready;
		GE.task = GE_move_front;
		break;
	case GE_move_front: // 跳转至玻片前端移动到夹手正下方
		GE.sta = Ready;
		GE.task = GE_move_glass;
		GE.glass_Exist = FALSE;
		GE.box_Exist = TRUE; // 检测到玻片盒
		break;
	case GE_move_glass:
		if (GE.glass_Exist) // 检测到玻片
		{
			if (check_GC()) // 夹手位于垂直原点，垂直静止
			{
				if (GE_down_Sen != Sen_Block && GE_up_Sen != Sen_Block) // 玻片被夹取 上下端无感应
				{
					GE.sta = Ready;
				}
			}
		}
		else // 到达后端
		{
			GE.sta = Ready;
			GE.task = GE_move_back;
		}

		break;
	case GE_move_back:
		GE.glass_Exist = FALSE;
		GE.sta = Ready;
		GE.task = GE_move_front;
		break;
	case GE_finish:
		break;
	case GE_error:
		break;
	}
}

void GE_TaskThread(void)
{
	if (GE.sta == Ready)
	{
		GE_ReadyTask();
		// 重载计时
		GE.running_tim = 0;
	}

	if (GE.sta == Running)
	{
		GE_RunningTask();
		// 计时 若超时进行超时错误上报
		GE.running_tim++;
		if (GE.running_tim > OVERTIME)
		{
			Error_OverTime_Set(GE_idx, GE.task);
			// Error_Set(Error_OverTime, 0);
		}
	}

	if (GE.sta == Finish)
	{
		if (GE.task <= GE_BOx_Out)
		{
			GE_FinishTask();
		}
		else
		{
			if (TaskThread_State != taskthread_debug) // debug 模式下不进行任务跳转
			{
				GE_FinishTask();
			}
		}
	}
}