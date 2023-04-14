#include "taskthread.h"

static enum glass_enter_task_index next_task = GE_none;

// 夹手位于垂直原点，垂直静止
static u8 check_GC(void)
{
	#ifdef DEBUG_MODE
	return TRUE;
	#else
	return (GC_ver_Sen == Sen_Block) && (GC.motor_v->motion == Stop);
	#endif
}

void GE_ReadyTask(void)
{
	switch (GE.task)
	{
	case GE_none:
		break;
	/////////////////开机复位//////////////////////////////
	case GE_reset_on:
		printf("GE_reset_on\r\n");
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
			  GE.motor->postion=-1;// 标记位置未校准 前往原点
				motorGo(GE.motor, 0, GE.motor->pulse_1mm * 50);
				GE.sta = Running;
			}
		}

		break;
	/////////////关机复位///////////////////////////
	case GE_reset_off:
		GE.sta=Finish;
	break;
	/////////////////装载槽进入//////////////////////////////
	case GE_Box_In:
		printf("GE_box_in\r\n");
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
				motorGo_acc(GE.motor, 0);
				GE.sta = Running;
			}
		}
		break;
	/////////////////装载槽推出//////////////////////////////
	case GE_BOx_Out:
		printf("GE_box_out\r\n");
		motorGo_acc(GE.motor, GE.motor->max_pos - 100);
		GE.sta = Running;
		break;

	/////////////////////装载槽复位到原点//////////////////////////////////
	case GE_move_start:
		if (GE_start_Sen == Sen_Block)
		{
			GE.motor->postion = 0;
			GE.sta = Finish;
		}
		else
		{
			if (check_GC()) // 夹手位于垂直原点，垂直静止
			{
				motorGo_acc(GE.motor, 0);
				GE.sta = Running;
			}
		}
		break;
	////////////////////装载槽前端移动到夹手正下方////////////////////////
	case GE_move_front:
		if ((GE_up_Sen == Sen_Block) && (GE_down_Sen == Sen_Block))
		{
			GE.sta = Finish;
			GE.box_Exist = TRUE;
		}
		else
		{
			if (check_GC()) // 夹手位于垂直原点，垂直静止
			{
				motorGo_acc(GE.motor, (GE.GE_box_dis + GE.GE_box_len) * 2);
				GE.sta = Running;
			}
		}
		break;
	//////////////////////玻片移动到夹手正下方//////////////////////////////////
	case GE_move_glass:
		if (GE_down_Sen == Sen_Block)
		{
			GE.sta = Finish;
		}
		else
		{
			if (check_GC()) // 夹手位于垂直原点，垂直静止
			{
				motorGo(GE.motor, GE.GE_box_len, 0);
				GE.sta = Running;
			}
		}
		break;
	///////////////////装载槽后端离开夹手下方////////////////////////////////////////////
	case GE_move_back:
		if ((GE_up_Sen != Sen_Block) && (GE_down_Sen != Sen_Block)) // 已离开装载槽后端
		{
			GE.sta = Ready;
			GE.task = GE_move_front;
		}
		else
		{
			// if(!GE.glass_Exist) //装载槽内是否存在载玻片
			// {
			// 	Error_Set(Error_Slide_Glass,0);
			// }
			if (check_GC()) // 夹手位于垂直原点，垂直静止
			{
				motorGo_acc(GE.motor, GE.GE_box_dis);
				GE.sta = Running;
				GE.glass_Exist = FALSE;
			}
		}
		break;
		///////////////////////////////进料任务完成//////////////////////////////
	case GE_finish:

		break;
	////////////////////////////////异常处理///////////////////////////////
	case GE_error:
		// 缺少装载槽

		// 装载槽缺少载玻片

		GE.sta = Ready;
		GE.task = GE_none;
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
			if (GE.motor->motion == Stop)
			{
				if (GE_start_Sen == Sen_Block)
				{
					GE.sta = Finish;
					GE.subtask = 0;
				}
				else
				{
					Error_Set(Error_Sensor, GE_start_sensor);
				}
			}
		}
		break;
	//////////////关机复位//////////////////////////
	case GE_reset_off:

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
	case GE_move_start:
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
		if ((GE_up_Sen == Sen_Block) && (GE_down_Sen == Sen_Block))
		{
			GE.sta = Finish;
			GE.box_Exist = TRUE;
		}
		else
		{
			if (GE.motor->motion == Stop) // GE 上下端传感器一直未感应
			{
				if (GE.box_Exist) // 已处理完所有装载槽 结束任务
				{
					GE.box_Exist = FALSE;
					GE.sta = Finish;
					GE.task = GE_finish;
				}
				else // 缺少装载槽
				{
					Error_Set(Error_In_Box, 0); // 报错 缺少装载槽
				}
			}
		}
		break;
	/////////////////////玻片移动到夹手正下方//////////////////////////////////
	case GE_move_glass:
		if ((GE_up_Sen == Sen_Block) || (GE_down_Sen == Sen_Block))
		{
			GE.sta = Finish;
			if (GE_up_Sen != Sen_Block && GE_down_Sen == Sen_Block)
			{
				GE.glass_Exist = TRUE;
			}
		}
		else
		{
			if (GE.motor->motion == Stop) // 上下端传感器错误
			{
				Error_Set(Error_Sensor, GE_up_sensor | GE_down_sensor);
			}
		}
		break;
		/////////////////////装载槽后端离开夹手正下方//////////////////////////////////
	case GE_move_back:
		if ((GE_up_Sen != Sen_Block) && (GE_down_Sen != Sen_Block))
		{
			GE.sta = Finish;
		}
		else
		{
			if (GE.motor->motion == Stop) // 上下端传感器错误
			{
				Error_Set(Error_Sensor, GE_up_sensor | GE_down_sensor);
			}
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
	switch (GE.task)
	{
	case GE_none:
		break;
	case GE_reset_on:
		GE.task = GE_none;
		GE.sta = Ready;
		GE.subtask = 0;
		break;
	case GE_reset_off:
		GE.task = GE_none;
		GE.sta = Ready;
		GE.subtask = 0;
		break;
	case GE_Box_In:
		GE.task = GE_none;
		GE.sta = Ready;
		GE.subtask = 0;
		break;
	case GE_BOx_Out:
		GE.task = GE_none;
		GE.sta = Ready;
		GE.subtask = 0;
	  break;
	case GE_move_start: // 跳转至装载槽前端移动到夹手正下方
		GE.sta = Ready;
		GE.task = GE_move_front;
		break;
	case GE_move_front: // 跳转至玻片前端移动到夹手正下方
		GE.sta = Ready;
		GE.task = GE_move_glass;
		break;
	case GE_move_glass:
		if (check_GC()) // 夹手位于垂直原点，垂直静止
		{
			if (GE_down_Sen != Sen_Block) // 玻片被夹取 下端无感应
			{
				GE.sta = Ready;
			}
		}
		if ((GE_up_Sen == Sen_Block) && (GE_down_Sen == Sen_Block)) // 到达装载槽后端
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
			Error_Set(Error_OverTime, 0);
		}
	}

	if (GE.sta == Finish)
	{
		if (TaskThread_State != taskthread_debug) // debug 模式下不进行任务跳转
		{
			GE_FinishTask();
		}
	}
}