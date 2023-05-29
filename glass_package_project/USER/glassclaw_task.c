#include "Globalconfig.h"
#define DELAY_MS 100
#define GLASS_DIS 16000
#define GC_PACKAGE_POS 3 * 211

// 夹手是否被阻挡
static u8 Check_GP(void)
{

	if (GC_claw_Sen == Sen_Block) // 夹手夹紧状态
	{
		if (GOH_start_Sen == Sen_Block && GO.motor_h->motion == Stop &&
			GP.motor->postion == GP.sucker_pos && GP.motor->motion == Stop) /*夹手托盘位于原点 封片装置在吸片位置*/
		{
			return TRUE;
		}
	}
	else
	{
		if (GO.motor_h->motion == Stop) //夹手托盘静止
		{
			return TRUE;
		}
	}
	return FALSE;
}

//从子任务恢复到主任务
static void Resume_Task(void)
{
	GC.sta = Ready;
	GC.subtask = GC.main_subtask;
	GC.task = GC.main_task;
}

// 跳转到指定任务中执行 Task 主任务 Next_task 下一子任务
static void Next_Task(enum glass_claw_task_index Resume_task, enum glass_claw_task_index Next_task)
{
	GC.main_task = Resume_task; // 指定恢复主任务

	if (Resume_task == GC.task) // 主任务是否变换
	{
		GC.main_subtask = GC.subtask + 1;
	}
	else
	{
		GC.main_subtask = 0;
	}

	GC.sta = Ready; // 跳转到下一任务执行
	GC.task = Next_task;
}

void GC_ReadyTask(void)
{
	//	if(GC.task!=GC_none)
	//	{
	//			printf("GC task:%d\r\n ",GC.task);
	//	}
	switch (GC.task)
	{
	case GC_none:
		break;
	/*垂直方向复位到原点 basic*/
	case GC_ver_start:
		motorGo(GC.motor_v, 0, 0);
		GC.sta = Running;
		break;
	/*旋转方向复位到原点 basic*/
	case GC_rot_start:
		if (GC_rot_Sen != Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (Check_GP())
			{
				motorGo(GC.motor_r, 0, 0);
				GC.sta = Running;
			}
		}
		break;
	/*夹手释放 basic*/
	case GC_release:
		GC_claw_Cyl = GAS_DISABLE; // 夹手释放
		GC.sta = Running;
		break;
	/*夹手夹紧 basic*/
	case GC_grab:
		GC_claw_Cyl = GAS_ENABLE; // 夹手夹紧
		GC.sta = Running;
		break;
	/*开机复位*/
	case GC_reset_on:
		if (GC.subtask == 0) // 夹手释放
		{
			Next_Task(GC.task, GC_release);
		}
		else if (GC.subtask == 1) // 复位至垂直原点
		{
			Next_Task(GC.task, GC_ver_start);
		}
		else if (GC.subtask == 2) // 如果在旋转原点位置 离开原点位置 重新校准位置
		{

			if (GC_rot_Sen != Sen_Block)
			{
				GC.motor_r->dir = Front;
				motor_Set_Direction(GC.motor_r);
				motorGO_Debug(GC.motor_r, GC.motor_r->pulse_1mm * 400, 0);
				GC.sta = Running;
			}
			else
			{
				GC.subtask = 3;
			}
		}
		else if (GC.subtask == 3) // 复位至旋转原点
		{
			Next_Task(GC_none, GC_rot_start);
		}

		break;
	/*夹手关机复位*/
	case GC_reset_off:
#ifdef GC_ROT_PACKAGE
		if (GC.subtask == 0) // 复位至垂直原点
		{
			Next_Task(GC.task, GC_ver_start);
		}
		else if (GC.subtask == 1) // 复位至旋转原点
		{
			if (GOH_start_Sen == Sen_Block)
			{
				Next_Task(GC.task, GC_rot_start);
			}
			else if (GO.motor_h->postion == GO.GOH_mid_pos && GO.motor_h->motion == Stop)
			{
				GC.subtask = 3;
			}
		}
		else if (GC.subtask == 2)
		{
			if (GC_claw_Sen == Sen_Block) // 夹手夹紧状态 等待托盘移动到工作点
			{
				if (GO.motor_h->postion == GO.GOH_mid_pos && GO.motor_h->motion == Stop) // 夹手旋转到水平
				{
					/*****************************/
					GC.sta = Running;
					motorGo(GC.motor_r, GC.GCR_hor_pos, 0);
					/*****************************/
				}
			}
			else
			{
				GC.subtask = 3;
			}
		}
		else if (GC.subtask == 3) // 夹手释放
		{
			Next_Task(GC.task, GC_release);
		}
		else if (GC.subtask == 4) // 夹手旋转到垂直位置
		{
			motorGo(GC.motor_r, GC.GCR_ver_pos, 0);
			GC.sta = Running;
		}
		else if (GC.subtask == 5) // 夹手旋转回原点
		{
			if (GOH_start_Sen == Sen_Block)
			{
				Next_Task(GC_none, GC_rot_start);
			}
		}
#endif
#ifdef GC_VER_PACKAGE
	if(GC.subtask==0)
	{
		if(GC_claw_Sen==Sen_Block) //夹手夹紧
		{
				if(GC.motor_r->postion==GC.GCR_hor_pos&&GC.motor_r->motion==Stop) //夹手水平停止
				{
					GC.subtask=2;
				}else
				{
					Next_Task(GC.task,GC_ver_start); //夹手复位至垂直原点
				}
		}else
		{
			GC.subtask=5; //夹手复位至垂直原点
		}
	}else if(GC.subtask==1)
	{
		if(GOH_start_Sen==Sen_Block&&GO.motor_h->motion==Stop) //玻片托盘在原点停止
		{
			motorGo_acc(GC.motor_r,GC.GCR_hor_pos);
			GC.sta=Running;
		}
	}else if(GC.subtask==2)
	{
		if(GO.motor_h->postion==GO.GOH_mid_pos&&GO.motor_h->motion==Stop) //玻片托盘在封片位置停止
		{
			motorGo(GC.motor_v,GC.GCV_pacakge_pos,0);
			GC.sta=Running;
		}
	}else if(GC.subtask==3)
	{
		Next_Task(GC.task,GC_release); //夹手释放
	}else if(GC.subtask==4)
	{
		Next_Task(GC.task,GC_rot_start); //夹手复位旋转原点
	}else if(GC.subtask==5)
	{
		Next_Task(GC.task,GC_ver_start); //夹手复位至垂直原点
	}else if(GC.subtask==6)
	{
		if(GOH_start_Sen==Sen_Block&&GO.motor_h->motion==Stop) //玻片托盘复位至原点
		{
			motorGo_acc(GC.motor_r,GC.GCR_hor_pos);
			GC.sta=Running;
		}
	}
#endif
		break;
	/*开始运行任务*/
	case GC_start:
#ifdef GC_ROT_PACKAGE
		// 释放夹手 并跳转到旋转垂直任务
		GC.sta = Finish;
#endif

#ifdef GC_VER_PACKAGE
		if (GC.Glass_Ready == FALSE)
		{
			if (GC.subtask == 0) // 释放夹手
			{
				Next_Task(GC.task, GC_release);
			}
			else if (GC.subtask == 1)
			{
				Next_Task(GC_rot_down, GC_rot_start);
			}
		}
#endif
		break;
	/*夹手旋转至垂直位置*/
	case GC_rot_down:
		if (GC.motor_r->postion == GC.GCV_down_pos)
		{
			GC.sta = Finish;
		}
		else
		{
			if (Check_GP())
			{
				motorGo_acc(GC.motor_r, GC.GCR_ver_pos); // 夹手旋转至垂直
				GC.sta = Running;
			}
		}
		break;
	/*夹手下降至玻片装载槽*/
	case GC_move_down:
		// 玻片移动至夹手正下方
		if (GE_up_Sen != Sen_Block && GE_down_Sen == Sen_Block && GE.motor->motion == Stop)
		{
			motorGo_acc(GC.motor_v, GC.GCV_down_pos);
			GC.sta = Running;
		}
		break;
	/*夹手旋转至水平位置*/
	case GC_check_glass:
		motorGo_acc(GC.motor_v, GC.GCV_down_pos - GC.GCV_glass_len); // 向上移动一定距离 检测是否存在玻片
		GC.sta = Running;
		break;
	/*夹手上升至原点*/
	case GC_move_up:
		motorGo(GC.motor_v, 0, GC.motor_v->defaultfeq * 2);
		GC.sta = Running;
		break;

#ifdef GC_ROT_PACKAGE
	case GC_rot_up:
		if (GC_rot_Sen != Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (Check_GP())
			{
				motorGo(GC.motor_r, 0, GC.motor_r->defaultfeq * 2);
				GC.sta = Running;
			}
		}
		break;
	case GC_rot_hor:
		if (GO.motor_h->postion == GO.GOH_mid_pos && GO.motor_h->motion == Stop)
		{
			// motorGo(GC.motor_r, GC.GCR_hor_pos, 0);
			motorGo_acc(GC.motor_r, GC.GCR_hor_pos);
			GC.sta = Running;
		}
		break;

#endif
#ifdef GC_VER_PACKAGE
	/*夹手旋转至水平位置*/
	case GC_rot_hor:
		if (GOH_start_Sen == Sen_Block && GO.motor_h->motion == Stop)
		{
			motorGo_acc(GC.motor_r, GC.GCR_hor_pos);
			GC.sta = Running;
		}
		break;
	/*夹手垂直移动至封片位置*/
	case GC_move_package:
		if (GO.motor_h->postion == GO.GOH_mid_pos && GO.motor_h->motion == Stop)
		{
			motorGo(GC.motor_v, GC.GCV_pacakge_pos, 0);
			GC.sta = Running;
		}
		break;
#endif

	case GC_finish:
		break;
	case GC_error:
		// 发生夹取错误
		if (error_type & Error_Grap)
		{
			motorGo(GC.motor_v, 0, GC.motor_v->defaultfeq * 2);
			GC.sta = Running;
		}
		else if (error_type & Error_Sucker)
		{
#ifdef GC_ROT_PACKAGE
			if (GC.subtask == 0)
			{
				if (GC.motor_r->postion == GC.GCR_ver_pos && GC.motor_r->motion == Stop)
				{
					GC.sta = Finish;
				}
				else
				{
					if (GO.motor_h->motion == Stop)
					{
						motorGo(GC.motor_r, 0, 0);
						GC.sta = Running;
					}
				}
			}
			else if (GC.subtask == 1)
			{
				if (GOH_start_Sen == Sen_Block && GO.motor_h->motion == Stop)
				{
					motorGo(GC.motor_r, GC.GCR_ver_pos, 0);
					GC.sta = Running;
				}
			}
#endif

#ifdef GC_VER_PACKAGE
			if(GC.subtask==0)
			{
				if(GC.motor_r->postion<=GC.GCR_ver_pos&&GC.motor_r->motion==Stop)
				{
					GC.sta=Finish;
				}else
				{
					if(GC.motor_r->motion==Stop&&GC.motor_v->motion==Stop)
					{
						motorGo(GC.motor_v,0,0);
						GC.sta=Running;
					}
				}
			}else if(GC.subtask==1)
			{
				if (GOH_start_Sen == Sen_Block && GO.motor_h->motion == Stop)
				{
					motorGo(GC.motor_r, GC.GCR_ver_pos, 0);
					GC.sta = Running;
				}
			}
			
#endif			
		}else{
			GC.sta=Finish;
		}
		break;
	}
}

void GC_RunningTask(void)
{
	switch (GC.task)
	{
	case GC_none:
		break;
		////////////夹手复位至垂直原点////////////////basic
	case GC_ver_start:
		if (GC_ver_Sen == Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (GC.motor_v->motion == Stop) // 垂直传感器错误 或电机错误
			{
				Error_Set(Error_Sensor, GC_ver_sensor);
			}
		}
		break;
	///////////夹手复位至旋转原点//////////////// basic
	case GC_rot_start:
		if (GC_rot_Sen != Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (GC.motor_r->motion == Stop)
			{

				//	Error_Set(Error_Sensor, GC_rot_sensor);
			}
		}
		break;
	///////////////开机复位///////////////////////////////
	case GC_reset_on:
		if (GC.subtask == 0) // 夹手释放
		{
		}
		else if (GC.subtask == 1) // 复位至垂直原点
		{
		}
		else if (GC.subtask == 2) // 如果在旋转原点位置 离开原点位置 重新校准位置
		{
			if (GC.motor_r->motion == Stop)
			{
				GC.sta = Ready;
				GC.subtask = 3;
			}
		}
		else if (GC.subtask == 3) // 复位至旋转原点
		{
		}
		break;
	////////////////////////夹手关机复位/////////////////
	case GC_reset_off:
#ifdef GC_ROT_PACKAGE
		if (GC.subtask == 0) // 复位至垂直原点
		{
		}
		else if (GC.subtask == 1) // 复位至旋转原点
		{
		}
		else if (GC.subtask == 2)
		{
			if (GC.motor_r->motion == Stop)
			{
				GC.sta = Ready;
				GC.subtask++;
			}
		}
		else if (GC.subtask == 3) // 夹手释放
		{
		}
		else if (GC.subtask == 4) // 夹手旋转到垂直位置
		{
			if (GC.motor_r->motion == Stop)
			{
				GC.sta = Ready;
				GC.subtask++;
			}
		}
		else if (GC.subtask == 5)
		{
		}
#endif
#ifdef GC_VER_PACKAGE
	if(GC.subtask==0)
	{

	}else if(GC.subtask==1)
	{
		if(GC.motor_r->motion==Stop)
		{
			GC.sta=Ready;
			GC.subtask++;
		}
	}else if(GC.subtask==2)
	{
		if(GC.motor_v->motion==Stop)
		{
			GC.sta=Ready;
			GC.subtask++;
		}
	}else if(GC.subtask==3)
	{

	}else if(GC.subtask==4)
	{

	}else if(GC.subtask==5)
	{

	}else if(GC.subtask==6)
	{
		if(GC.motor_r->motion==Stop)
		{
			GC.sta=Finish;
		}	
	}
#endif
		break;
	/////////////////开始运行////////////////////
	case GC_start:

		break;
	///////////////////夹手旋转至垂直位置/////////////////
	case GC_rot_down:
		if (GC.motor_r->motion == Stop) // 夹手旋转至水平
		{
			GC.sta = Finish;
		}
		break;
	////////////////夹手下降至玻片装载槽////////////////
	case GC_move_down:
		if (GC.motor_v->motion == Stop) // 夹手下降至玻片位置
		{
			GC.sta = Finish;
		}
		break;
	//////////////////等待夹手夹紧/////////////////// basic
	case GC_grab:
		if (GC.running_tim > DELAY_MS) // 等待延时超时 夹手夹紧
		{
			if (GC_claw_Sen == Sen_Block)
			{
				GC.sta = Finish;
			}
			else
			{
				Error_Set(Error_Sensor, GC_claw_sensor);
			}
		}
		break;
	///////////////夹手旋转至水平位置//////////
	case GC_check_glass:
		if (GC.motor_v->motion == Stop)
		{
			GC.sta = Finish;
		}
		break;
	case GC_move_up:
		if (GC_ver_Sen == Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (GC.motor_v->motion == Stop)
			{
				Error_Set(Error_Sensor, GC_ver_sensor);
			}
		}
		break;
#ifdef GC_ROT_PACKAGE
	case GC_rot_up:
		if (GC_rot_Sen != Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (GC.motor_r->motion == Stop)
			{
				//	Error_Set(Error_Sensor,GC_rot_sensor);
			}
		}
	case GC_rot_hor:
		if (GC.motor_r->motion == Stop)
		{
			GC.Glass_Ready = TRUE;
			GC.sta = Finish;
		}
		break;
#endif
#ifdef GC_VER_PACKAGE
	case GC_rot_hor:
		if (GC.motor_r->motion == Stop)
		{
			GC.sta = Finish;
		}
		break;
	case GC_move_package:
		if (GC.motor_v->motion == Stop)
		{
			GC.Glass_Ready = TRUE;
			GC.sta = Finish;
		}
		break;
#endif
	////////////////////夹手释放延时超时///////////////////////
	case GC_release:
		if (GC.running_tim > DELAY_MS)
		{
			if (GC_claw_Sen != Sen_Block)
			{
				GC.sta = Finish;
			}
			else
			{
				Error_Set(Error_Sensor, GC_claw_sensor);
			}
		}

		break;
	case GC_finish:
		break;
	case GC_error:
		if (error_type & Error_Grap)
		{
			if (GC_ver_Sen == Sen_Block)
			{
				GC.sta = Finish;
			}
			else
			{
				if (GC.motor_v->motion == Stop) // 垂直传感器错误 或电机错误
				{
					Error_Set(Error_Sensor, GC_ver_sensor);
				}
			}
		}
		else if (error_type & Error_Sucker)
		{
#ifdef GC_ROT_PACKAGE
			if (GC.subtask == 0)
			{
				if (GC_rot_Sen != Sen_Block)
				{
					GC.sta = Ready;
					GC.subtask = 1;
				}
				else
				{
					if (GC.motor_r->motion == Stop)
					{

						//	Error_Set(Error_Sensor, GC_rot_sensor);
					}
				}
			}
			else if (GC.subtask == 1)
			{
				if (GC.motor_r->motion == Stop)
				{
					GC.sta = Finish;
					GC.subtask = 0;
				}
			}
#endif

#ifdef GC_VER_PACKAGE
			if(GC.subtask==0)
			{
				if (GC_ver_Sen == Sen_Block)
				{
					GC.sta = Ready;
					GC.subtask = 1;
				}
				else
				{
					if (GC.motor_v->motion == Stop)
					{

							Error_Set(Error_Sensor, GC_ver_sensor);
					}
				}
			}else if(GC.subtask==1)
			{
				if (GC.motor_r->motion == Stop)
				{
					GC.sta = Finish;
					GC.subtask = 0;
				}
			}
#endif

		}

		break;
	}
}

void GC_FinishTask(void)
{
	switch (GC.task)
	{
	case GC_none:
		break;
	case GC_ver_start:
		Resume_Task();
		break;
	case GC_rot_start:
		Resume_Task();
		break;
	case GC_grab:
		Resume_Task();
		break;
	case GC_release:
		Resume_Task();
		break;
	case GC_reset_on:
		GC.main_task = GC_none;
		GC.main_subtask = 0;
		GC.subtask = 0;
		GC.sta = Ready;
		GC.task = GC_none;
		break;
	case GC_reset_off:
		GC.main_task = GC_none;
		GC.main_subtask = 0;
		GC.subtask = 0;
		GC.sta = Ready;
		GC.task = GC_none;
		break;
	case GC_start:
#ifdef GC_ROT_PACKAGE
		if (GC.Glass_Ready == FALSE) // 未开始封片
		{
			Next_Task(GC_rot_down, GC_release);
		}
#endif
		break;

	case GC_rot_down:
		if (GE.task == GE_move_glass && GE.sta == Finish) // 等待玻片移动到夹手正下方
		{
			GC.sta = Ready;
			GC.subtask = 0;
			GC.task = GC_move_down;
		}
		else
		{
			if (GE.task == GE_finish)
			{
				GC.subtask = 0;
				GC.task = GC_finish;
				GC.sta = Ready;
			}
		}
		break;
	case GC_move_down:
		Next_Task(GC_check_glass, GC_grab);
		break;

	case GC_check_glass:
		GC.sta = Ready;
		GC.task = GC_move_up;
		GC.subtask = 0;
		//		if(GE_up_Sen==Sen_Block)	//夹取到玻片
		//		{
		//			GC.sta=Ready;
		//			GC.task=GC_move_up;
		//		}else
		//		{
		//			//Error_Set(Error_Slide_Glass,0); 载玻片错误
		//			//GC_claw_Cyl=GAS_DISABLE; //夹手释放
		//			printf("glass error\r\n");
		//			Next_Task(GC_none,GC_ver_start); //复位回垂直原点 重新夹取玻片
		//		}
		break;
#ifdef GC_ROT_PACKAGE
	case GC_move_up:
		if (GO.task == GO_start && GO.sta == Finish && GP.motor->postion == GP.sucker_pos) // 托盘位于原点 且封片装置在吸片位置
		{
			GC.subtask = 0;
			GC.sta = Ready;
			GC.task = GC_rot_up;
		}
		break;
	case GC_rot_up:
		if (GO.task == GOH_package && GO.sta == Finish && GP.motor->postion == GP.sucker_pos) // 托盘位于工作点 且封片装置在吸片位置
		{
			GC.subtask = 0;
			GC.sta = Ready;
			GC.task = GC_rot_hor;
		}
		break;

	case GC_rot_hor:
		if (GC.Glass_Ready == FALSE) // 封片结束
		{
			GC.subtask = 0;
			GC.sta = Ready;
			GC.task = GC_start;
		}
		break;
#endif
#ifdef GC_VER_PACKAGE
	case GC_move_up:
		if (GO.task == GO_start && GO.sta == Finish && GP.motor->postion == GP.sucker_pos) // 托盘位于原点 且封片装置在吸片位置
		{
			GC.subtask = 0;
			GC.sta = Ready;
			GC.task = GC_rot_hor;
		}
		break;
	case GC_rot_hor:
		if (GO.task == GOH_package && GO.sta == Finish)
		{
			GC.subtask = 0;
			GC.sta = Ready;
			GC.task = GC_move_package;
			;
		}

		break;
	case GC_move_package:
		if (GC.Glass_Ready == FALSE) // 封片结束
		{
			GC.subtask = 0;
			GC.sta = Ready;
			GC.task = GC_start;
		}
		break;
#endif
	case GC_finish:
		break;
	case GC_error:
		break;
	}
}

void GC_TaskThread(void)
{
	if (GC.sta == Ready)
	{
		GC_ReadyTask();
		// 重载计数值
		GC.running_tim = 0;
	}
	if (GC.sta == Running)
	{
		GC_RunningTask();
		// 计数值增加 超时报错
		GC.running_tim++;
		if (GC.running_tim > OVERTIME)
		{
			Error_OverTime_Set(Extra_error_overtime_GC, GC.task);
			// Error_Set(Error_OverTime, 0);
		}
	}
	if (GC.sta == Finish)
	{
		if (GC.task <= GC_reset_off)
		{
			GC_FinishTask();
		}
		else
		{
			if (TaskThread_State != taskthread_debug) // debug 模式下不进行任务自动跳转
			{
				GC_FinishTask();
			}
		}
	}
}
