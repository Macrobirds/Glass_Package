#include "taskthread.h"
#define DELAY_MS 100

// 主任务号
static enum glass_claw_task_index main_task = GC_none;
// 主子任务号
static u8 main_subtask = 0;

// 检测是否阻挡夹手旋转
static u8 Check_GP(void)
{
	if (GC_claw_Sen == Sen_Block) // 夹手夹紧状态
	{
		if (GO.motor_h->postion == 0 && GO.motor_h->motion == Stop && GP.motor->postion == GP.sucker_pos && GP.motor->motion == Stop)
		{
			return TRUE;
		}
	}
	else
	{
		if (GO.motor_h->postion == 0 && GO.motor_h->motion == Stop)
		{
			return TRUE;
		}
	}
	return FALSE;
}


static void Resume_Task(void)
{
	GC.sta=Ready;
	GC.subtask=main_subtask;
	GC.task=main_task;
}
// 跳转到指定任务中执行 Task 主任务 Next_task 下一子任务
static void Next_Task(enum glass_claw_task_index Resume_Task, enum glass_claw_task_index Next_task)
{
	main_task = Resume_Task; // 指定恢复主任务

	if (Resume_Task != GC.task) // 主任务是否变换
	{
		main_subtask = GC.subtask + 1;
	}
	else
	{
		main_subtask = 0;
	}

	GC.sta = Ready; // 跳转到下一任务执行
	GC.task = Next_task;
}

void GC_ReadyTask(void)
{
	switch (GC.task)
	{
	case GC_none:
		break;
	///////////////开机复位//////////////////////
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
			if (Check_GP())
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
		}
		else if (GC.subtask == 3) // 复位至旋转原点
		{
			if (GC_claw_Sen == Sen_Block)
			{
				Next_Task(GC_none, GC_rot_start);
			}
		}

		break;
	////////////////夹手关机复位//////////////////
	case GC_reset_off:
		if (GC.subtask == 0) // 复位至垂直原点
		{
			Next_Task(GC.task, GC_ver_start);
		}
		else if (GC.subtask == 1) // 复位至旋转原点
		{
			if (Check_GP())
			{
				Next_Task(GC.task, GC_rot_start);
			}
		}
		else if (GC.subtask == 2)
		{
			if (GC_claw_Sen == Sen_Block) // 夹手夹紧状态 等待托盘移动到工作点
			{
				if (GO.motor_h->postion == GO.GOH_mid_pos && GO.motor_h->motion == Stop) // 夹手旋转到水平
				{
					Next_Task(GC.task, GC_rot_hor);
				}
			}
			else
			{
				GC.sta = Finish;
			}
		}
		else if (GC.subtask == 3) // 夹手释放
		{
			Next_Task(GC.task, GC_release);
		}
		else if (GC.subtask == 4) // 复位至旋转原点
		{
			Next_Task(GC_none, GC_rot_start);
		}
		break;
	///////////////////开始运行任务//////////////////////////
	
	///////////////////垂直方向复位到原点/////////////////////basic
	case GC_ver_start:
		if (GC_ver_Sen == Sen_Block) // 垂直原点感应
		{
			stepperMotorStop(GC.motor_v);
			GC.motor_v->postion = 0;
			GC.sta = Finish;
		}
		else
		{
			motorGo_acc(GC.motor_v, 0); // 垂直方向复位至原点
			GC.sta = Running;
		}
		break;
	///////////////////旋转方向复位到原点///////////////////// basic
	case GC_rot_start:
		if (GC_rot_Sen != Sen_Block)
		{
			stepperMotorStop(GC.motor_r);
			GC.motor_r->postion = 0;
			GC.sta = Finish;
		}
		else
		{
			motorGo_acc(GC.motor_r, 0);
			GC.sta = Running;
		}

		break;
	//////////////////夹手旋转至垂直位置////////////////// basic
	case GC_rot_down:
		motorGo_acc(GC.motor_r, GC.GCR_ver_pos); // 夹手旋转至垂直
		GC.sta = Running;
		break;
	////////////////夹手下降至玻片装载槽//////////////// basic
	case GC_move_down:
		if ((GE_down_Sen == Sen_Block) && (GE_up_Sen != Sen_Block)) // 玻片移动至夹手正下方
		{
			motorGo_acc(GC.motor_v, GC.GCV_down_pos);
			GC.sta = Running;
		}
		break;
	//////////////////夹手夹紧///////////////////////////// basic
	case GC_grab:
		GC_claw_Cyl = GAS_ENABLE; // 夹手夹紧
		GC.sta = Running;
		break;
	////////////////夹手旋转至水平位置///////////////////// basic
	case GC_rot_hor:
		if ((GOH_mid_Sen == Sen_Block) && (GO.motor_h->motion == Stop)) // 玻片托盘处于封片工作点静止
		{
			if (GP.task <= GP_sucker_up) // 封片工作未开始
			{
				motorGo(GC.motor_r, GC.GCR_hor_pos, 0);
				GC.sta = Running;
			}
		}
		break;
	////////////////////夹手释放//////////////////////////// basic
	case GC_release:
		GC_claw_Cyl = GAS_DISABLE; // 夹手释放
		GC.sta = Running;
		break;
	case GC_finish:
		break;
	case GC_error:
		break;
	}
}

void GC_RunningTask(void)
{
	switch (GC.task)
	{
	case GC_none:
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
			if(GC.motor_r->motion==Stop) 
			{
				GC.sta=Ready;
				GC.subtask=3;
			}
		}
		else if (GC.subtask == 3) // 复位至旋转原点
		{

		}
		break;
	////////////////////////夹手关机复位/////////////////
	case GC_reset_off:
		if (GC.subtask == 0) // 复位至垂直原点
		{

		}
		else if (GC.subtask == 1) // 复位至旋转原点
		{

		}
		else if (GC.subtask == 2)
		{

		}
		else if (GC.subtask == 3) // 夹手释放
		{

		}
		else if (GC.subtask == 4) // 复位至旋转原点
		{

		}
		break;
	case GC_ver_start:
		////////////夹手复位至垂直原点////////////////
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
	///////////夹手复位至旋转原点////////////////
	case GC_rot_start:
		if(GC_rot_Sen!=Sen_Block)
		{
			GC.sta=Finish;
		}else
		{
			if(GC.motor_r->motion==Stop)
			{
				Error_Set(Error_Sensor,GC_rot_sensor);
			}
		}
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
	//////////////////等待夹手夹紧///////////////////
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
	case GC_rot_hor:
		if (GC.motor_r == Stop)
		{
			GC.sta = Finish;
		}
		break;
	////////////////////夹手释放延时超时///////////////////////
	case GC_release:
		if (GC.running_tim > DELAY_MS)
		{
			if(GC_claw_Sen!=Sen_Block)
			{
				GC.sta = Finish;
			}

		}

		break;
	case GC_finish:
		break;
	case GC_error:
		break;
	}
}

void GC_FinishTask(void)
{
	switch (GC.task)
	{
	case GC_none:
		break;
	case GC_reset_on:
		GC.sta = Ready;
		GC.task = GC_none;
		break;
	case GC_reset_off:
		GC.sta = Ready;
		GC.task = GC_none;
		break;
	case GC_ver_start:
		Resume_Task();
		break;
	case GC_rot_start:
		Resume_Task();
		break;
	case GC_rot_down:
		Resume_Task();
		break;
	case GC_move_down:
		Resume_Task();
		break;
	case GC_grab:
		Resume_Task();
		break;
	// case GC_move_up:
	// 	if ((GO.task == GO_start) && (GO.sta == Finish)) // 等待玻片托盘回到原点
	// 	{
	// 		GC.sta = Ready;
	// 		GC.task = GC_rot_up;
	// 	}
	// 	break;
	// case GC_rot_up:
	// 	if ((GO.task == GO_package) && (GO.sta == Finish)) // 等待玻片托盘到封片工作点
	// 	{
	// 		GC.sta = Ready;
	// 		GC.task = GC_rot_hor;
	// 	}
	// 	break;
	case GC_rot_hor:
		Resume_Task();
		// if (GP.task <= GP_sucker_up) // 等待封片工作完成 开始下一任务
		// {
		// 	GC.sta = Ready;
		// 	GC.task = GC_release;
		// }
		break;
	case GC_release:
		Resume_Task();
		// if (GC_claw_Sen != Sen_Block)
		// {
		// 	GC.sta = Ready;
		// 	GC.task = GC_rot_down;
		// }
		break;
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
			Error_Set(Error_OverTime, 0);
		}
	}
	if (GC.sta == Finish)
	{
		if (TaskThread_State != taskthread_debug) // debug 模式下不进行任务自动跳转
		{
			GC_FinishTask();
		}
	}
}

// if(GC.subtask==0) //复位至垂直原点
// {
// 	if(GC_ver_Sen!=Sen_Block)
// 	{
// 		GC.subtask=1;
// 	}else
// 	{
// 		motorGo_acc(GC.motor_v,0);
// 		GC.sta=Running;
// 	}
// }else if(GC.subtask==1) //旋转到旋转垂直点
// {
// 	if(GC.motor_r->postion==GC.GCR_ver_pos)
// 	{
// 		GC.subtask=2;
// 	}else
// 	{
// 		if(GOH_start_Sen==Sen_Block&&GO.motor_h->motion==Stop) //等待玻片托盘回到原点
// 		{
// 			motorGo_acc(GC.motor_r,GC.GCR_ver_pos);
// 			GC.sta=Running;
// 		}
// 	}
// }else if(GC.subtask==2) //旋转到旋转原点
// {
// 	if(GC_rot_Sen==Sen_Block)
// 	{
// 		GC.subtask=3;
// 	}else
// 	{
// 		if(GOH_start_Sen==Sen_Block&&GO.motor_h->motion==Stop) //等待玻片吸盘装置离开原点
// 		{
// 			motorGo_acc(GC.motor_r,0);
// 			GC.sta=Running;
// 		}
// 	}
// }else if(GC.subtask==3) //旋转到旋转水平点
// {
// 	if(GOH_mid_Sen==Sen_Block&&GO.motor_h->motion==Stop) //等待玻片托盘到工作点
// 	{
// 		motorGo_acc(GC.motor_r,GC.GCR_hor_pos);
// 		GC.sta=Running;
// 	}
// }else if(GC.subtask==4) //夹手释放
// {
// 	GC_claw_Cyl=GAS_DISABLE;

// }else if(GC.subtask==5) //旋转到旋转垂直点
// {
// 	if(GC_claw_Sen!=Sen_Block)
// 	{
// 		motorGo_acc(GC.motor_r,GC.GCR_ver_pos);
// 		GC.sta=Running;
// 	}

// }

// if(GC.subtask==0) //等待复位到垂直原点
// {
// 	if(GC_ver_Sen==Sen_Block)
// 	{
// 		stepperMotorStop(GC.motor_v);
// 		GC.motor_v->postion=0;
// 		GC.sta=Ready;
// 		GC.subtask=1;
// 	}else
// 	{
// 		if(GC.motor_v->motion==Stop) //垂直传感器错误 或电机错误
// 		{
// 			Error_Set(Error_Sensor,GC_ver_sensor);
// 		}
// 	}
// }else if(GC.subtask==1) //等待旋转的旋转垂直位置
// {
// 	if(GC.motor_r->motion==Stop)
// 	{
// 		GC.sta=Ready;
// 		GC.subtask=2;
// 	}

// }else if(GC.subtask==2) //等待旋转到旋转原点
// {
// 	if(GC_rot_Sen==Sen_Block)
// 	{
// 		stepperMotorStop(GC.motor_r);
// 		GC.motor_r->postion=0;
// 		GC.sta=Ready;
// 		GC.subtask=3;
// 	}else
// 	{
// 		if(GC.motor_r->motion==Stop)
// 		{
// 			Error_Set(Error_Sensor,GC_rot_sensor);
// 		}
// 	}
// }else if(GC.subtask==3) //等待旋转到旋转水平
// {
// 	if(GC.motor_r->motion==Stop)
// 	{
// 		GC.sta=Ready;
// 		GC.subtask=4;
// 	}
// }else if(GC.subtask==4) //等待夹手释放
// {
// 	if(GC.running_tim>DELAY_MS)
// 	{
// 		if(GC_claw_Sen!=Sen_Block) //夹手释放
// 		{
// 			GC.sta=Ready;
// 			GC.subtask=5;
// 		}

// 	}

// }else if(GC.subtask==5) //等待旋转到旋转垂直位置
// {
// 	if(GC.motor_r->motion==Stop)
// 	{
// 		GC.sta=Finish;
// 		GC.subtask=0;
// 	}

// }
