#include "taskthread.h"
#define ADJUST_DIS 2

// 检测是否夹手阻挡了托盘
static u8 Check_GC(void)
{
#ifdef DEBUG_MODE
	return TRUE;
#else
	if (GC.motor_r->postion == GC.GCR_hor_pos)
	{
		return FALSE;
	}
	else
	{
		if (GC.motor_r->motion == Stop) // 夹手旋转电机静止
		{
			return TRUE;
		}
	}
	return FALSE;
#endif
}

static u8 Check_box(void)
{
	if (GOV_box_Sen != Sen_Block) // 未检测到存储盒
	{
		Error_Set(Error_Out_Box, 0);
		return FALSE;
	}
	return TRUE;
}

static void Resume_Task(void)
{
	GO.sta = Ready;
	GO.subtask = GO.main_subtask;
	GO.task = GO.main_task;
}
// 跳转到指定任务中执行 Task 主任务 Next_task 下一子任务
static void Next_Task(enum glass_out_task_index Resume_task, enum glass_out_task_index Next_task)
{
	GO.main_task = Resume_task; // 指定恢复主任务

	if (Resume_task == GO.task) // 主任务是否变换
	{
		GO.main_subtask = GO.subtask + 1;
	}
	else
	{
		GO.main_subtask = 0;
	}

	GO.sta = Ready; // 跳转到下一任务执行
	GO.task = Next_task;
}

void GO_ReadyTask(void)
{
	switch (GO.task)
	{
	case GO_none:
		break;
	////////////////////开机复位///////////////
	case GO_reset_on:
		if (GO.subtask == 0) // 检测水平原点传感器 若感应 离开原点 重新校准水平位置
		{
			if (GOH_start_Sen == Sen_Block)
			{
				GO.motor_h->dir = Front;
				motor_Set_Direction(GO.motor_h);
				motorGO_Debug(GO.motor_h, GO.motor_h->pulse_1mm * 10, 0);
				GO.sta = Running;
			}
			else
			{
				GO.subtask = 1;
			}
		}
		else if (GO.subtask == 1) // 复位水平原点
		{
			if (Check_GC())
			{
				Next_Task(GO.task, GOH_start);
			}
		}
		else if (GO.subtask == 2) /// 检测垂直原点传感器 若感应 离开原点 重新校准垂直位置
		{
			if (GOV_start_Sen == Sen_Block)
			{
				GO.motor_v->dir = Front;
				motor_Set_Direction(GO.motor_v);
				motorGO_Debug(GO.motor_v, GO.motor_v->pulse_1mm * 5, 0);
				GO.sta = Running;
			}
			else
			{
				GO.subtask = 3;
			}
		}
		else if (GO.subtask == 3) // 复位垂直原点
		{
			Next_Task(GO_none, GOV_start);
		}
		break;
	//////////////////关机复位/////////////////
	case GO_reset_off:
		if (GO.subtask == 0) // 回到水平原点
		{
			if (GOH_start_Sen == Sen_Block)
			{
				GO.sta = Ready;
				GO.subtask = 1;
			}
			else
			{
				if (Check_GC())
				{
					Next_Task(GO.task, GOH_start);
				}
			}
		}
		else if (GO.subtask == 1)
		{
			if (GC_claw_Sen == Sen_Block) // 夹手处于夹紧状态  到水平工作点
			{
				if (GC.motor_r->postion == 0 && GC.motor_r->motion == Stop) // 等待夹手复位到原点
				{
					motorGo_acc(GO.motor_h, GO.GOH_mid_pos);
				}
			}
			else // 夹手释放 结束任务
			{
				GO.sta = Finish;
			}
		}
		else if (GO.subtask == 2) // 等待夹手释放 回到水平原点
		{
			if (Check_GC())
			{
				Next_Task(GO_none, GOH_start);
			}
		}
		break;
	//////////////////存储器推出////////////////
	case GO_Box_Out:
		motorGo_acc(GO.motor_v, GO.GOV_box_dis);
		break;
	/////////////////存储器进入////////////////
	case GO_Box_In:
		if (GO.subtask == 0) // 存储器进入
		{
			if (Check_box())
			{
				motorGo_acc(GO.motor_v, 0);
				GO.sta = Running;
			}
		}
		else if (GO.subtask == 1) // 对准存储器槽
		{
			motorGo(GO.motor_v, GO.GOV_box_len, 0);
			GO.sta = Running;
		}
		else if (GO.subtask == 2) // 调整存储器位置
		{
			Next_Task(GO_none, GO_adjust);
		}
		break;
	////////////////开始运行///////////////////////
	case GO_start:

		if (Check_box()) // 水平复位到原点
		{
			if (Check_GC())
			{
				Next_Task(GOH_package, GOH_start);
			}
		}

		break;
	///////////////复位至水平原点///////////////////
	case GOH_start:
		if (GOH_start_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			motorGo_acc(GO.motor_h, 0);
			GO.sta = Running;
		}
		break;
	//////////////移动至封片工作点////////////////////
	case GOH_package:
		if (GOH_mid_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GC.main_task == GC_start && GC.subtask == 6) // 等待夹手夹取玻片到旋转原点
			{
				GO.sta = Running;
				motorGo_acc(GO.motor_h, GO.GOH_mid_pos); // 移动到封片工作点
			}
		}
		break;
	////////////移动到水平终点 玻片放置到存储槽////////////////
	case GOH_end:
		if (GOH_end_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			// 封片工作结束 夹手释放
			if (Check_GC())
			{
				GO.sta = Running;
				motorGo_acc(GO.motor_h, GO.GOH_end_pos);
			}
		}
		break;
	///////////复位到垂直原点位置/////////////////////
	case GOV_start:
		if (GOV_start_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			GO.sta = Running;
			motorGo_acc(GO.motor_v, 0);
		}
		break;
	///////////将玻片移出托盘,放入存储槽///////////////
	case GO_out:
		if (GOV_glass_Sen == Sen_Block)
		{

			GO.sta = Running;
			motorGo(GO.motor_v, GO.GOV_slot_dis, 0);
		}
		else
		{
			GO.sta = Finish;
		}
		break;
	////////////移动到下一存储盒///////////////////
	case GO_next:
		if (GOV_glass_Sen == Sen_Block)
		{
			GO.sta = Running;
			motorGo(GO.motor_v, GO.GOV_box_dis, 0);
		}
		else
		{
			GO.sta = Finish;
		}
		break;
	///////////微调存储器位置让玻片对准槽下端///////////////
	case GO_adjust:
		GO.sta = Running;
		motorGo(GO.motor_v, GO.motor_v->postion + GO.motor_v->pulse_1mm * ADJUST_DIS, 0);
		break;
	case GO_finish:
		break;
	case GO_error:
		break;
	}
}

void GO_RunningTask(void)
{
	switch (GO.task)
	{
	case GO_none:
		break;
	////////////////开机复位///////////////////////////
	case GO_reset_on:
		if (GO.subtask == 0) // 检测水平原点传感器 若感应 离开原点 重新校准水平位置
		{
			if (GO.motor_h->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 1;
			}
		}
		else if (GO.subtask == 1) // 复位水平原点
		{
		}
		else if (GO.subtask == 2) /// 检测垂直原点传感器 若感应 离开原点 重新校准垂直位置
		{
			if (GO.motor_v->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 3;
			}
		}
		else if (GO.subtask == 3) // 复位垂直原点
		{
		}
		break;
	////////////////关机复位///////////////////////
	case GO_reset_off:
		if (GO.subtask == 0) // 回到水平原点
		{
		}
		else if (GO.subtask == 1) // 夹手处于夹紧状态  到水平工作点
		{

			if (GOH_mid_Sen == Sen_Block)
			{
				GO.sta = Ready;
				GO.subtask = 2;
			}
			else
			{
				if (GO.motor_h->motion == Stop)
				{
					Error_Set(Error_Sensor, GOH_mid_sensor);
				}
			}
		}
		else if (GO.subtask == 2) // 等待夹手释放 回到水平原点
		{
		}
		break;
	///////////////出槽////////////////////////////
	case GO_Box_Out:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
		}
		break;
	////////////////////进槽/////////////////////
	case GO_Box_In:
		if (GO.subtask == 0) // 存储器进入
		{
			if (GO.motor_v->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 1;
			}
		}
		else if (GO.subtask == 1) // 对准存储器槽
		{
			if (GO.motor_v->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 2;
			}
		}
		else if (GO.subtask == 2) // 调整存储器位置
		{

		}
		break;
	///////////////复位至水平原点///////////////////
	case GOH_start:
		if (GOH_start_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_h->motion == Stop)
			{
				Error_Set(Error_Sensor, GOH_start_sensor);
			}
		}
		break;
	//////////////移动至封片工作点////////////////////
	case GOH_package:
		if (GOH_mid_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_h->motion == Stop)
			{
				Error_Set(Error_Sensor, GOH_mid_sensor);
			}
		}
		break;
	////////////移动到水平终点 玻片放置到存储槽////////////////
	case GOH_end:
		if (GOH_end_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_h->motion == Stop)
			{
				Error_Set(Error_Sensor, GOH_end_sensor);
			}
		}
		break;
	case GO_out:
		if (GOV_glass_Sen != Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_v->motion == Stop && GO.motor_v->postion <= GO.GOV_box_len) // 走完一个槽的距离后 仍然有感应
			{
				GO.sta = Ready;
				GO.task = GO_next;
			}
			else if (GO.motor_v->postion > GO.GOV_box_len) //存储槽满 结束任务
			{
				GO.sta = Ready;
				GO.task = GO_finish;
			}
		}
		break;
	case GO_next:
		if (GOV_glass_Sen != Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_v->motion == Stop)
			{
				if (GO.motor_v->postion < GO.GOV_box_len)
				{
					GO.sta=Finish;
				}
				else
				{
					OS_ENTER_CRITICAL();
					GO.box_num++; // 槽个数加1
					OS_EXIT_CRITICAL();
					GO.sta = Ready;
					GO.task = GO_finish;
					Error_Set(Error_Out_Box, 0); // 报错 存储槽满
				}
			}
		}
		break;
	case GO_adjust:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
		}
		break;
	case GO_finish:
		break;
	case GO_error:
		break;
	}
}

void GO_FinishTask(void)
{
	switch (GO.task)
	{
	case GO_none:
		break;
	case GO_reset_on:
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_reset_off:
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_Box_Out:
		break;
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
	case GO_Box_In:

		break;
	case GO_start:

	break;
	case GOH_start:
	Resume_Task();	
		break;
	case GOH_package:
		if (Check_GC()) // 当夹手离开封片区域
		{
			GO.sta = Ready;
			GO.task = GOH_end;
		}
		break;
	case GOH_end:

			OS_ENTER_CRITICAL();
			GO.glass_num++; // 封片玻片数量+1
			OS_EXIT_CRITICAL();
			GO.sta = Ready;
			GO.task = GO_out;
		break;
	case GO_out:
		Next_Task(GO_start,GO_adjust);
		break;
	case GO_next:
		OS_ENTER_CRITICAL();
		GO.box_num++;
		OS_EXIT_CRITICAL();
		Next_Task(GO_start,GO_adjust);
		break;
	case GO_adjust:
		Resume_Task();
		break;
	case GO_finish:
		break;
	case GO_error:
		break;
	}
}

void GO_TaskThread(void)
{

	if (GO.sta == Ready)
	{
		GO_ReadyTask();
		GO.running_tim = 0;
	}
	if (GO.sta == Running)
	{
		GO_RunningTask();
		GO.running_tim++;
		if (GO.running_tim > OVERTIME)
		{
			Error_Set(Error_OverTime, 0);
		}
	}
	if (GO.sta == Finish)
	{
		if (TaskThread_State != taskthread_debug) // debug 模式下不进行任务跳转
		{
			GO_FinishTask();
		}
	}
}

// if (GO.subtask == 0) // 水平复位到原点
// {
// 	if (GOH_start_Sen == Sen_Block)
// 	{
// 		GO.sta = Ready;
// 		GO.subtask = 1;
// 	}
// 	else
// 	{
// 		motorGo_acc(GO.motor_h, 0);
// 		GO.sta = Running;
// 	}
// }
// else if (GO.subtask == 1) // 存储盒进入
// {
// 	if (GOV_box_Sen == Sen_Block)
// 	{
// 		motorGo_acc(GO.motor_v, 0);
// 		GO.sta = Running;
// 	}
// 	else
// 	{
// 		Error_Set(Error_Out_Box, 0); // 报错没有存储盒
// 	}
// }
// else if (GO.subtask == 2) // 对准存储槽
// {
// 	motorGo(GO.motor_v, GO.motor_v->postion + GO.GOV_box_dis, 0);
// 	GO.sta = Running;
// }
// else if (GO.subtask == 3) // 调整存储槽位置
// {
// }