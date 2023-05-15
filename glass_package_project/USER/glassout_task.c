#include "Globalconfig.h"
#define ADJUST_DIS 700 // pulse value
#define ADJUST_DIS_full 200

#define SLOT_NUM 30

static u8 slot_num = 0;

// 检测是否夹手阻挡了托盘
static u8 Check_GC(void)
{
	if (GO.task <= GO_reset_on)
	{
		if (GC_claw_Sen != Sen_Block)
		{
			return TRUE;
		}

		if (GC.motor_r->postion != GC.GCR_hor_pos && GC.motor_r->motion == Stop)
		{
			return TRUE;
		}
	}
	else
	{
		if (GC.motor_r->postion != GC.GCR_hor_pos && GC.motor_r->motion == Stop)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// 检查是否存在存储盒
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

	if (slot_num <= 0)
	{
		GO.Storage_full = TRUE;
	}
//		if(GO.task!=GO_none)
//		{
//			printf("GO task:%d subtask:%d\r\n",GO.task,GO.subtask);
//		}
	switch (GO.task)
	{
	case GO_none:
		break;
	///////////复位到垂直原点位置///////////////////// basic
	case GOV_start:
		if (GOV_start_Sen == Sen_Block)
		{
			stepperMotorStop(GO.motor_v);
			GO.sta = Finish;
			GO.motor_v->postion = 0;
		}
		else
		{
			GO.sta = Running;
			motorGo(GO.motor_v, 0, 0);
			//	motorGo_acc(GO.motor_v, 0);
		}
		break;
	///////////////复位至水平原点/////////////////// basic
	case GOH_start:
		if (GOH_start_Sen == Sen_Block)
		{
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
			GO.motor_h->postion = 0;
		}
		else
		{
			if (Check_GC())
			{
				motorGo(GO.motor_h, 0, 0);
				GO.sta = Running;
			}
		}
		break;
	///////////微调存储器位置让玻片对准槽下端/////////////// basic
	case GO_adjust:
		GO.sta = Running;
		GO.motor_v->dir = Front;
		motor_Set_Direction(GO.motor_v);
		if (GOV_start_Sen == Sen_Block)
		{
			printf("dual box\r\n");
			motorGO_Debug(GO.motor_v, GO.GOV_adjust_start, 0);
		}
		else
		{
			printf("singal box\r\n");
			motorGO_Debug(GO.motor_v,GO.GOV_adjust, 0);
		}
		break;
	////////////////////开机复位///////////////
	case GO_reset_on:
		GO.Storage_full = TRUE;
		slot_num=0;		
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
				motorGO_Debug(GO.motor_v, GO.motor_v->pulse_1mm * 10, 800 * 10);
				GO.sta = Running;
			}
			else
			{
				GO.subtask = 3;
			}
		}
		else if (GO.subtask == 3) // 复位垂直原点
		{
			Next_Task(GO.task, GOV_start);
		}
		else if (GO.subtask == 4)
		{
			Next_Task(GO_none, GO_adjust);
		}
		break;
	//////////////////关机复位/////////////////
	case GO_reset_off:
		if (GO.subtask == 0) // 回到水平原点
		{
				if (Check_GC())
				{		
						if(GOH_start_Sen==Sen_Block)
						{
							GO.subtask++;
						}else
						{
							if(GC.motor_r->postion==GC.GCR_ver_pos&&GC.motor_r->motion==Stop)
							{
								Next_Task(GO_none, GOH_start);
							}

						}

				}
		}
		else if (GO.subtask == 1)
		{
			if (GC_claw_Sen == Sen_Block) // 夹手处于夹紧状态  到水平工作点
			{
				if (GC_rot_Sen!=Sen_Block&& GC.motor_r->motion == Stop) // 等待夹手复位到原点
				{
					motorGo_acc(GO.motor_h, GO.GOH_mid_pos);
					GO.sta = Running;
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
				if(GC.motor_r->postion==GC.GCR_ver_pos&&GC.motor_r->motion==Stop)
				{
						Next_Task(GO_none, GOH_start);
				}

			}
		}
		break;
	//////////////////存储器推出////////////////
	case GO_Box_Out:
		GO.Storage_full = TRUE;
		slot_num=0;	
		motorGo_acc(GO.motor_v, GO.GOV_box_len);
		GO.sta = Running;
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
			if (GOV_start_Sen == Sen_Block)
			{
				GO.subtask = 2;
			}
			else
			{
				GO.motor_v->dir = Front;
				motor_Set_Direction(GO.motor_v);
				motorGO_Debug(GO.motor_v, GO.GOV_box_len, 0);
				GO.sta = Running;
			}
		}
		else if (GO.subtask == 2) // 调整存储器位置
		{
			if(GOV_box_Sen==Sen_Block)
			{
				//存储槽个数+1
				OS_ENTER_CRITICAL();
				GO.box_num++;
				OS_EXIT_CRITICAL();
				// 存储器状态更新
				if (GOV_start_Sen == Sen_Block)
				{
					slot_num = SLOT_NUM * 2;
				}
				else
				{
					slot_num = SLOT_NUM;
				}
				GO.Storage_full = FALSE;
				Next_Task(GO_none, GO_adjust);
			}else
			{
				GO.task=GO_none;
				GO.sta=Ready;
				Error_Set(Error_Out_Box,0);
			}
		}
		break;
	////////////////开始运行///////////////////////
	case GO_start: // 水平复位到原点
		if (GOH_start_Sen == Sen_Block)
		{
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
			GO.motor_h->postion = 0;
		}
		else
		{
			if (Check_GC())
			{
				motorGo(GO.motor_h, 0, 268 * 80);
				GO.sta = Running;
			}
		}
		break;
	//////////////移动至封片工作点////////////////////
	case GOH_package:
		if (GO.motor_h->postion == GO.GOH_mid_pos)
		{
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
			GO.motor_h->postion = GO.GOH_mid_pos;
		}
		else
		{
			if (GC_rot_Sen != Sen_Block && GC_claw_Sen == Sen_Block) // 等待夹手夹取玻片到旋转原点
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
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
			GO.motor_h->postion = GO.GOH_end_pos;
		}
		else
		{
			if (GO.Storage_full == TRUE) // 存储槽已满
			{
				Error_Set(Error_StorageFull, 0); // 警告 存储槽满
			}
			else
			{
				if (Check_box() && GOV_glass_Sen != Sen_Block) // 检测是否有存储盒 且存储槽空
				{
					if (Check_GC()) // 封片工作结束 夹手释放
					{
						GO.sta = Running;
						motorGo_acc(GO.motor_h, GO.GOH_end_pos);
					}
				}
				else
				{
					Error_Set(Error_Out_Box|Error_Sensor, GOV_glass_sensor);
				}
			}
		}
		break;

	///////////将玻片移出托盘,放入存储槽///////////////
	case GO_out:
		GO.motor_v->dir = Front;
		motor_Set_Direction(GO.motor_v);
		motorGO_Debug(GO.motor_v, GO.GOV_slot_dis, 0);
		GO.sta = Running;
		break;
	////////////移动到下一存储盒///////////////////
	case GO_next:
		if (GOV_glass_Sen == Sen_Block)
		{
			GO.motor_v->dir = Front;
			motor_Set_Direction(GO.motor_v);
			motorGO_Debug(GO.motor_v, GO.GOV_box_dis, 0);
			GO.sta = Running;
		}
		else
		{
			GO.sta = Finish;
		}
		break;
	case GO_finish:
		break;
	case GO_error:
		if(error_type&Error_Out_Box)
		{
			GO.sta=Finish;
		}else if(error_type&Error_StorageFull)
		{
			GO.sta=Finish;
		}
		break;
	}
}

void GO_RunningTask(void)
{
	switch (GO.task)
	{
	case GO_none:
		break;
	///////////复位到垂直原点位置///////////////////// basic
	case GOV_start:
		if (GOV_start_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_v->motion == Stop)
			{
				Error_Set(Error_Sensor, GOV_start_sensor);
			}
		}
		break;
	///////////////复位至水平原点/////////////////// basic
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
	///////////微调存储器位置让玻片对准槽下端/////////////// basic	
	case GO_adjust:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
		}
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

			if (GO.motor_h->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 2;
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
	////////////开始运行//////////////////
	case GO_start:
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
		if (GO.motor_h->motion == Stop)
		{
			GO.sta = Finish;
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

		if (GO.motor_v->motion == Stop) // 走完一个槽的距离后 仍然有感应
		{
			printf("out box pos:%d\r\n", GO.motor_v->postion);
			if (GOV_glass_Sen != Sen_Block)
			{
				GO.sta = Finish;
			}
			else
			{
				GO.sta = Ready; // 到达下一个槽或者槽满
				GO.task = GO_next;
			}
		}

		break;
	case GO_next:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
			printf("next box pos:%d\r\n", GO.motor_v->postion);
			//				if (GOV_glass_Sen!=Sen_Block)
			//				{
			//					GO.sta=Finish;
			//				}else //槽满
			//				{
			//					GO.sta=Ready;
			//					GO.task=GO_finish;
			//				}
		}
		break;

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
	case GOV_start:
		Resume_Task();
		break;
	case GOH_start:
		Resume_Task();
		break;
	case GO_adjust:
		printf("adj box pos:%d\r\n", GO.motor_v->postion);
		Resume_Task();
		break;
	case GO_reset_on:
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_reset_off:
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_Box_Out: 
		GO.subtask = 0;
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		#ifdef DEBUG_MDOE
		GO.sta = Ready;
		GO.task = GO_none;
		#endif
		break;
	case GO_Box_In:
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		GO.subtask = 0;

		GO.sta = Ready;
		GO.task = GO_none;

		break;
	case GO_start:
		if (GC.task == GC_rot_up && GC.sta == Finish)
		{
			GO.sta = Ready;
			GO.task = GOH_package;
		}else
		{
			if(GC.task==GC_finish)
			{
				GO.task=GO_finish;
				GO.sta=Ready;
				//当不是由于暂停结束任务时，发送任务完成信息
				if(TaskThread_State!=taskthread_pause)
				{
					//发送任务完成信息
				}
			}

		}
		break;
	case GOH_package:
		#ifdef BIG_CYLINDER
		if (GC.motor_r->postion == GC.GCR_ver_pos && GP_big_cyl_Sen == Sen_Block) // 当夹手垂直 且大气缸复位
		{
			GO.sta = Ready;
			GO.task = GOH_end;
		}
		#endif
		#ifdef BIG_CYLINDER_MOTOR
		if (GC.motor_r->postion == GC.GCR_ver_pos && GP_big_cyl_Sen == Sen_Block) // 当夹手垂直 且大气缸复位
		{
			GO.sta = Ready;
			GO.task = GOH_end;
		}
		#endif
		break;
	case GOH_end:
		OS_ENTER_CRITICAL();
		GO.glass_num++; // 封片玻片数量+1
		slot_num--;
		OS_EXIT_CRITICAL();
		GO.sta = Ready;
		GO.task = GO_out;
		break;
	case GO_out:
		Next_Task(GO_start, GO_adjust);
		break;
	case GO_next:
		OS_ENTER_CRITICAL();
		GO.box_num++;
		OS_EXIT_CRITICAL();
		Next_Task(GO_start, GO_adjust);
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
			Error_OverTime_Set(GO_idx,GO.task);
			//Error_Set(Error_OverTime, 0);
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

