#include "taskthread.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define DELAY_SUCKER 500
#define DELAY_BIG_CYLIDER 1500
#define DELAY_SMALL_CYLIDER 1500

static u8 Check_Spray(void)
{
	if (GP_spray_Sen != Sen_Block) // 未放置喷胶头 报错
	{
		Error_Set(Error_Spray, 0);
		return FALSE;
	}
	return TRUE;
}

static void Resume_Task(void)
{
	GP.sta = Ready;
	GP.subtask = GP.main_subtask;
	GP.task = GP.main_task;
}
// 跳转到指定任务中执行 Task 主任务 Next_task 下一子任务
static void Next_Task(enum glass_package_task_index Resume_task, enum glass_package_task_index Next_task)
{
	GP.main_task = Resume_task; // 指定恢复主任务

	if (Resume_task == GP.task) // 主任务是否变换
	{
		GP.main_subtask = GP.subtask + 1;
	}
	else
	{
		GP.main_subtask = 0;
	}

	GP.sta = Ready; // 跳转到下一任务执行
	GP.task = Next_task;
}

void GP_ReadyTask(void)
{
	switch (GP.task)
	{
	case GP_none:
		break;
	/////////////开机复位///////////////
	case GP_reset_on:
		if (GP.subtask == 0) // 大小气缸复位
		{
			Next_Task(GP.task, GP_cyl_start);
		}
		else if (GP.subtask == 1)
		{
			if (GP_start_Sen == Sen_Block) // 若原点传感器感应 离开原点 重新校准位置
			{
				GP.motor->dir = Front;
				motor_Set_Direction(GP.motor);
				motorGO_Debug(GP.motor, GP.motor->pulse_1mm * 10, 0);
				GP.sta = Running;
			}
			else
			{
				GP.subtask = 2;
			}
		}
		else if (GP.subtask == 2) // 复位至原点
		{
			Next_Task(GP_none, GP_move_start);
		}

		break;
	/////////////关机复位//////////////
	case GP_reset_off:
		if (GP.subtask == 0) // 关闭吸盘
		{
			GP_sucker_Pump = GAS_DISABLE;
			GP_sucker1_Cyl = GAS_DISABLE;
			GP_sucker2_Cyl = GAS_DISABLE;
			GP.sta=Running;
		}
		else if (GP.subtask == 1) // 气缸复位
		{
			Next_Task(GP.task, GP_cyl_start);
		}
		else if (GP.subtask == 2) // 原点复位
		{
			if(GC_rot_Sen!=Sen_Block&&GOH_start_Sen==Sen_Block&&GC_claw_Sen!=Sen_Block) //夹手释放回到旋转原点并且托盘回到原点
			{
				Next_Task(GP_none, GP_move_start);
			}else //离开原点位 等待夹手释放复位
			{
				motorGo_acc(GP.motor,GP.sucker_pos);
				GP.sta=Running;
			}
			
		}else if(GP.subtask==3)
		{
			if(GC_claw_Sen!=Sen_Block)
			{
				Next_Task(GP_none, GP_move_start);
			}
		}

		break;
	/////////开始运行/////////////////
	case GP_start:
		if (Check_Spray())
		{
			if (GP.subtask == 0)
			{
				Next_Task(GP.task, GP_cyl_start); // 气缸复位
			}
			else if (GP.subtask == 1)
			{
				Next_Task(GP_move_glass, GP_move_start); // 原点复位
			}
		}
		break;
	////////////大小气缸复位/////////////// basic
	case GP_cyl_start:
		if(GP_big_cyl_Sen==Sen_Block&&GP_small_cyl_Sen==Sen_Block)
		{
			GP.sta=Finish;
		}else
		{
				GP_small_Cyl = GAS_DISABLE; // 小气缸失能
				GP_big_Cyl = GAS_DISABLE;	// 大气缸失能
				GP.sta = Running;
		}

		break;
	//////////复位至原点位置/////////////// basic
	case GP_move_start:
		if (GP_start_Sen == Sen_Block)
		{
			GP.sta = Finish;
		}
		else
		{
			GP.sta = Running;
			motorGo_acc(GP.motor, 0);
		}
		break;
	///////////到达玻片吸取位置////////////
	case GP_move_glass:
			GP.sta = Running;
			motorGo_acc(GP.motor, GP.sucker_pos);
		break;
	//////////玻片吸盘下降/////////////
	case GP_sucker_down:
		if (GP.subtask == 0)
		{
			GP_small_Cyl = GAS_ENABLE;
			GP.sta = Running;
		}
		else if (GP.subtask == 1)
		{
			GP_big_Cyl = GAS_ENABLE;
			GP.sta = Running;
		}
		break;
	/////////吸取盖玻片///////////////
	case GP_suck_glass:
		if (GP_sucker_Sen == Sen_Block)
		{
			GP_sucker_Pump = GAS_ENABLE; // 开启吸盘气泵
			GP_sucker1_Cyl = GAS_ENABLE; // 开启吸盘1 吸盘2
			GP_sucker2_Cyl = GAS_ENABLE;
			GP.sta = Running;
		}
		else
		{
			/// 缺少盖玻片
			Error_Set(Error_Cover_Glass, 0);
		}
		break;
	///////////玻片吸盘上升///////////
	case GP_sucker_up:
		if ((GC.motor_r->postion == GC.GCR_hor_pos) && (GC.motor_r->motion == Stop)) // 夹手位于旋转水平位置静止
		{
				GP_big_Cyl = GAS_DISABLE;
				GP.sta = Running;
		}

		break;
	/////////////移动到喷胶位置/////////
	case GP_move_spray:
			GP.sta = Running;
			motorGo_acc(GP.motor, GP.spray_pos);
		break;
		/////////////移动到封片位置///////// 封片位置和原点位置重合
	case GP_move_package:
		if (GP_start_Sen == Sen_Block)
		{
			GP.sta = Finish;
		}
		else
		{
			GP.sta = Running;
			GP_spray_Cyl = GAS_ENABLE;			  // 打开喷胶阀
			motorGo(GP.motor, 0, GP.spray_speed); // 按喷胶速度匀速运动到封片点
		}
		break;
	////////////////封片工作/////////////////////
	case GP_package:
		if (GP.subtask == 0)
		{
			GP_big_Cyl = GAS_ENABLE; // 大气缸下降
			GP.sta=Running;
		}
		else if (GP.subtask == 1)
		{
			GP_small_Cyl = GAS_DISABLE; // 小气缸上升
			GP.sta=Running;
		}
		
		else if (GP.subtask == 2)
		{
			GP_sucker1_Cyl = GAS_DISABLE; // 关闭吸盘1气阀
			GP.sta=Running;
		}
		
		else if (GP.subtask == 3)
		{
			GP_sucker_Pump = GAS_DISABLE; //关闭吸盘2气阀 吸盘气泵
			GP_sucker1_Cyl = GAS_DISABLE;
			GP_sucker2_Cyl = GAS_DISABLE;
			GP.sta=Running;
		}
		break;
	case GP_finish:
		break;
	case GP_error:
		break;
	}
}

void GP_RunningTask(void)
{
	switch (GP.task)
	{
	case GP_none:
		break;
	case GP_reset_on:
		if (GP.subtask == 0) // 大小气缸复位
		{

		}
		else if (GP.subtask == 1)
		{
			if(GP.motor->motion==Stop)// 若原点传感器感应 离开原点 重新校准位置
			{
				GP.subtask=2;
				GP.sta=Ready;
			}
		}
		else if (GP.subtask == 2) // 复位至原点
		{
			
		}
		break;
	case GP_reset_off:
		if (GP.subtask == 0) // 关闭吸盘
		{
			if(GP.running_tim>DELAY_SUCKER)
			{
				GP.subtask=1;
				GP.sta=Ready;
			}
		}
		else if (GP.subtask == 1) // 气缸复位
		{

		}
		else if (GP.subtask == 2) // 原点复位
		{
			
			if(GP.motor->motion==Stop)
			{
				GP.subtask=3;
				GP.sta=Ready;
			}
			
		}else if(GP.subtask==3)
		{
			
		}
		break;

	////////////////大小气缸复位///////////////// basic
	case GP_start:

		break;
	case GP_cyl_start:
		if (GP.running_tim > MAX(DELAY_BIG_CYLIDER, DELAY_SMALL_CYLIDER))
		{
			if ((GP_big_cyl_Sen == Sen_Block) && (GP_small_cyl_Sen == Sen_Block))
			{
				GP.sta = Finish;
			}
			else
			{
				Error_Set(Error_Sensor, GP_big_cyl_sensor | GP_small_cyl_sensor);
			}
		}
		break;
	//////////复位至原点位置/////////////// basic
	case GP_move_start:
		if (GP_start_Sen == Sen_Block)
		{
			GP.sta = Finish;
		}
		else
		{
			if(GP.motor->motion==Stop)
			{
				Error_Set(Error_Sensor, GP_start_sensor);
			}

		}
		break;
	case GP_move_glass:
		if (GP.motor->motion == Stop)
		{
			GP.sta = Finish;
		}
		break;
	//////////玻片吸盘下降/////////////
	case GP_sucker_down:
		if (GP.subtask == 0)
		{
			if (GP.running_tim > DELAY_SMALL_CYLIDER) // 等待小气缸先下降
			{
				if (GP_small_cyl_Sen != Sen_Block)
				{
					GP.subtask = 1;
					GP.sta = Ready;
				}
			}
		}
		else if (GP.subtask == 1)
		{
			if (GP.running_tim > DELAY_BIG_CYLIDER) // 等待大气缸下降
			{
				if (GP_big_cyl_Sen != Sen_Block)
				{
					GP.subtask = 0;
					GP.sta = Finish;
				}
			}
		}
		break;
	/////////吸取盖玻片///////////////
	case GP_suck_glass:
		if (GP.running_tim > DELAY_SUCKER)
		{
			if(GP_sucker_Sen==Sen_Block)
			{
				GP.sta = Finish;
			}else
			{
				Error_Set(Error_Sensor,GP_sucker_sensor);
			}

		}
		break;
	///////////玻片吸盘上升///////////
	case GP_sucker_up:
		if (GP.running_tim > DELAY_BIG_CYLIDER)
		{
			if (GP_big_cyl_Sen == Sen_Block)
			{
				GP.sta = Finish;
			}else
			{
				Error_Set(Error_Sensor,GP_big_cyl_sensor);
			}
		}
		break;
	/////////////移动到喷胶位置/////////
	case GP_move_spray:
		if (GP.motor->motion == Stop)
		{
			GP.sta = Finish;
		}
		break;
	/////////////移动到封片位置///////// 封片位置和原点位置重合
	case GP_move_package:
		if (GP.motor->postion < (GP.spray_pos - GP.spray_len)) // 到达喷胶长度 关闭喷胶阀
		{
			GP_spray_Cyl = GAS_DISABLE;
		}

		if (GP_start_Sen == Sen_Block) // 达到封片工作原点
		{
			GP_spray_Cyl = GAS_DISABLE;
			GP.sta = Finish;
		}
		else
		{
			if (GP.motor->motion == Stop)
			{
				GP_spray_Cyl = GAS_DISABLE;
				Error_Set(Error_Sensor, GP_start_sensor); // 报错
			}
		}
		break;
	////////////////封片工作/////////////////////
	case GP_package:
		if (GP.subtask == 0)
		{
			if (GP.running_tim > GP.delay_before) // 等待大气缸下降 盖玻片与载玻片接触
			{
				GP.sta = Ready;
				GP.subtask = 1;
			}
		}
		else if (GP.subtask == 1) // 等待小气缸上升 盖玻片与载玻片水平
		{
			if (GP.running_tim > GP.delay_after)
			{
				GP.sta = Ready;
				GP.subtask = 2;
			}
		}
		else if (GP.subtask == 2) // 等待吸盘1关闭
		{
			if (GP.running_tim > DELAY_SUCKER)
			{
				GP.sta = Ready;
				GP.subtask = 3;
			}
		}
		else if (GP.subtask == 3) // 等待吸盘2 吸盘气泵关闭
		{
			if (GP.running_tim > DELAY_SUCKER)
			{
				GP.subtask = 0;
				GP.sta = Finish;
			}
		}
		break;
	case GP_finish:
		break;
	case GP_error:
		break;
	}
}

void GP_FinishTask(void)
{
	switch (GP.task)
	{
	case GP_none:
		break;
	case GP_reset_on:
		
		GP.subtask=0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_reset_off:
		GP.subtask=0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_start:
		GP.subtask=0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_cyl_start:
		Resume_Task();
		break;
	case GP_move_start:
		Resume_Task();
		break;
	case GP_move_glass:
		GP.sta = Ready;
		GP.task = GP_sucker_down;
		GP.subtask = 0;
		break;
	case GP_sucker_down:
		if (GP_sucker_Sen == Sen_Block) // 检测到玻片
		{
			GP.sta = Ready;
			GP.task = GP_suck_glass;
			GP.subtask=0;
		}
		else
		{
			Error_Set(Error_Cover_Glass, 0); // 报错 没有盖玻片
		}
		break;
	case GP_suck_glass:
		if (GP_sucker_Sen == Sen_Block) // 检测到玻片
		{
			if ((GC.task == GC_rot_hor) && (GC.sta == Finish)) // 等待夹手将玻片放置到承托盘上
			{
						GP.sta = Ready;
						GP.task = GP_sucker_up;
						GP.subtask=0;
			}
		}
		else
		{
			if(GC.task==GC_finish)
			{
				GP.task=GP_finish;
				GP.sta=Ready;
				GP.subtask=0;
			}else
			{
				Error_Set(Error_Cover_Glass, 0); // 报错 没有盖玻片
			}

		}
		break;
	case GP_sucker_up:
			GP.sta = Ready;
			GP.task = GP_move_spray;
			GP.subtask=0;
		break;
	case GP_move_spray:
		GP.sta = Ready;
		GP.task = GP_move_package;
		GP.subtask=0;
		break;
	case GP_move_package:
		GP.sta = Ready;
		GP.task = GP_package;
		GP.subtask = 0;
		break;
	case GP_package:
		if (GP_big_cyl_Sen != Sen_Block && GP_small_cyl_Sen == Sen_Block) // 等待大小气缸到位
		{
			GP.subtask = 0;
			GP.sta = Ready;
			GP.task = GP_start;
		}
		break;
	case GP_finish:
		break;
	case GP_error:
		break;
	}
}

void GP_TaskThread(void)
{
	if (GP.sta == Ready)
	{
		GP_ReadyTask();
		GP.running_tim = 0;
	}
	if (GP.sta == Running)
	{
		GP_RunningTask();
		GP.running_tim++;
		if (GP.running_tim > OVERTIME)
		{
			Error_Set(Error_OverTime, 0);
		}
	}
	if (GP.sta == Finish)
	{
		if (TaskThread_State != taskthread_debug) // debug 模式下不进行任务跳转
		{
			GP_FinishTask();
		}
	}
}