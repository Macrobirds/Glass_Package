#include "taskthread.h"
#define ADJUST_DIS 2

static enum glass_out_task_index next_task=GO_none; //调整



void GO_ReadyTask(void)	
{
	switch(GO.task)
	{
		case GO_none:break;
		////////////////////开机复位///////////////
		case GO_reset_on:
		if(GO.subtask==0)
		{
			if(GOH_start_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_h);
				GO.motor_h->postion=0;
				GO.subtask=1;
			}else
			{
				motorGo_acc(GO.motor_h,0);
				GO.sta=Running;
			}
		}else if(GO.subtask==1)
		{
			if(GOV_start_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_v);
				GO.motor_v->postion=0;
				GO.sta=Finish;
			}else
			{
				motorGo_acc(GO.motor_v,0);
				GO.sta=Running;
			}
		}
		break;
		//////////////////关机复位/////////////////
		case GO_reset_off:
		if(GO.subtask==0) //回到水平原点
		{
			if(GOH_start_Sen==Sen_Block)
			{
				GO.sta=Ready;
				GO.subtask=1;
			}else
			{
				if(GC.motor_r->motion==Stop&&GC.motor_r->postion!=GC.GCR_hor_pos)
				{
					motorGo_acc(GO.motor_h,0);
					GO.sta=Running;
				}
			}
		}else if(GO.subtask==1) //到工作点
		{
			if(GOH_mid_Sen==Sen_Block)
			{
				GO.sta=Ready;
				GO.subtask=2;
			}else
			{	
				if(GC.motor_r->motion==Stop&&GC.motor_r->postion!=GC.GCR_hor_pos)
				{
					motorGo_acc(GO.motor_h,GO.GOH_mid_pos);
					GO.sta=Running;
				}

			}
		}else if (GO.subtask==2)
		{
			if(GC.motor_r->postion==GC.GCR_ver_pos)
			{
				motorGo_acc(GO.motor_h,0);
				GO.sta=Running;
			}
		}
		break;
		//////////////////存储器推出////////////////
		case GO_Box_Out:
		if(GO.subtask==0)
		{
			if(GOH_start_Sen==Sen_Block)
			{
				GO.sta=Ready;
				GO.subtask=1;
			}else
			{
				motorGo_acc(GO.motor_h,0);
				GO.sta=Running;
			}
		}else if(GO.subtask==1)
		{
			motorGo_acc(GO.motor_v,GO.GOV_box_len*2);
			GO.sta=Running;
		}		
		break;
		/////////////////存储器进入////////////////
		case GO_Box_In:
		if(GO.subtask==0) //水平复位到原点
		{
			if(GOH_start_Sen==Sen_Block)
			{
				GO.sta=Ready;
				GO.subtask=1;
			}else
			{
				motorGo_acc(GO.motor_h,0);
				GO.sta=Running;
			}
		}else if(GO.subtask==1) //存储盒进入
		{
			if(GOV_box_Sen==Sen_Block)
			{
				motorGo_acc(GO.motor_v,0);
				GO.sta=Running;
			}else
			{
				Error_Set(Error_Out_Box,0); //报错没有存储盒
			}
		}else if(GO.subtask==2) //向上运动到玻片槽
		{
			motorGo(GO.motor_v,GO.motor_v->postion+GO.GOV_box_dis,0);
			GO.sta=Running;
			
		}		
		break;
		///////////////复位至水平原点///////////////////
		case GO_start:
		if(GOV_box_Sen==Sen_Block) //检测是否存在存储盒
		{
			if(GOH_start_Sen==Sen_Block) //检查是否处于水平原点
			{
				GO.sta=Finish;
			}else
			{
				GO.sta=Running;
				motorGo_acc(GO.motor_h,0);
			}
		}else
		{
			Error_Set(Error_Out_Box,0); //报错 没有存储盒
		}	
		break;
		//////////////移动至封片工作点////////////////////
		case GO_package:
		if(GOH_mid_Sen==Sen_Block)
		{
			GO.sta=Finish;
		}else
		{
			if((GC.motor_r->postion)==0&&(GC_claw_Sen==Sen_Block)) //夹手夹取玻片到旋转原点位置
			{
				GO.sta=Running;
				motorGo_acc(GO.motor_h,GO.GOH_mid_pos); //移动到封片工作点
			}
		}
		break;
		////////////移动到水平终点 玻片放置到存储槽////////////////
		case GO_end:
		if(GOH_end_Sen==Sen_Block)
		{
			GO.sta=Finish;
		}else
		{
			if(GOV_box_Sen==Sen_Block&&GOV_glass_Sen!=Sen_Block) //检测到存储器 存储器槽空
			{
				GO.sta=Running;
				motorGo_acc(GO.motor_h,GO.GOH_end_pos);
			}else if(GOV_box_Sen==Sen_Block&&GOV_glass_Sen==Sen_Block) //存储槽满
			{
				Error_Set(Error_Out_Box,0);
			}
		}
		break;
		///////////将玻片移出托盘,放入存储槽///////////////
		case GO_out:
		if(GOV_glass_Sen==Sen_Block)
		{
			
			GO.sta=Running;
			motorGo(GO.motor_v,GO.GOV_slot_dis,0);
		}else
		{
			GO.sta=Finish;
		}
		break;
		////////////移动到下一存储盒///////////////////
		case GO_next:
		if(GOV_glass_Sen==Sen_Block)
		{
			GO.sta=Running; 
			motorGo(GO.motor_v,GO.GOV_box_dis,0);
		}else
		{
			GO.sta=Finish;
		}
		break;
		///////////微调存储器位置让玻片对准槽下端///////////////
		case GO_adjust:
		GO.sta=Running;
		motorGo(GO.motor_v,GO.motor_v->postion+GO.motor_v->pulse_1mm*ADJUST_DIS,0);
		break;
		case GO_finish:break;
		case GO_error:break;
	}
}

void GO_RunningTask(void)
{
	switch(GO.task)
	{
		case GO_none:break;
		////////////////开机复位///////////////////////////
		case GO_reset_on:
		if(GO.subtask==0)
		{
			if(GOH_start_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_h);
				GO.motor_h->postion=0;
				GO.sta=Ready;
				GO.subtask=1;
			}else
			{
				if(GO.motor_h->motion==Stop)
				{
					Error_Set(Error_Sensor,GOH_start_sensor);
				}
			}	
		}else if (GO.subtask==1)
		{
			if(GOV_start_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_v);
				GO.motor_v->postion=0;
				GO.sta=Finish;
				GO.subtask=0;
			}else
			{
				if(GO.motor_v->motion==Stop)
				{
					Error_Set(Error_Sensor,GOV_start_sensor);
				}
			}
		}
		break;
		////////////////关机复位///////////////////////
		case GO_reset_off:
		if(GO.subtask==0) //回到水平原点
		{
			if(GOH_start_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_h);
				GO.motor_h->postion=0;
				GO.sta=Ready;
				GO.subtask=1;
			}else
			{
				if(GO.motor_h->motion==Stop)
				{
					Error_Set(Error_Sensor,GOH_start_sensor);
				}
			}
		}else if(GO.subtask==1) //到工作点
		{
			if(GOH_mid_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_h);
				GO.motor_h->postion=GO.GOH_mid_pos;
				GO.sta=Ready;
				GO.subtask=2;
			}else
			{
				if(GO.motor_h->motion==Stop)
				{
					Error_Set(Error_Sensor,GOH_mid_sensor);
				}			
			}
		}else if (GO.subtask==2)
		{
			if(GOH_start_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_h);
				GO.motor_h->postion=0;
				GO.sta=Finish;
				GO.subtask=0;
			}else
			{
				if(GO.motor_h->motion==Stop)
				{
					Error_Set(Error_Sensor,GOH_start_sensor);
				}
			}
		}
		break;
		///////////////出槽////////////////////////////
		case GO_Box_Out:
		if(GO.subtask==0)
		{
			if(GOH_start_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_h);
				GO.motor_h->postion=0;
				GO.sta=Ready;
				GO.subtask=1;
			}else
			{
				if(GO.motor_h->motion==Stop)
				{
					Error_Set(Error_Sensor,GOH_start_sensor);
				}
			}
		}else if(GO.subtask==1)
		{
			if(GO.motor_v->motion==Stop)
			{
				GO.sta=Finish;
			}
		}
		break;
		////////////////////进槽/////////////////////
		case GO_Box_In:
		if(GO.subtask==0)
		{
			if(GOH_start_Sen==Sen_Block)
			{
				stepperMotorStop(GO.motor_h);
				GO.motor_h->postion=0;
				GO.sta=Ready;
				GO.subtask=1;
			}else
			{
				if(GO.motor_h->motion==Stop)
				{
					Error_Set(Error_Sensor,GOH_start_sensor);
				}
			}
		}else if(GO.subtask==1)
		{
			if(GOV_start_Sen==Sen_Block||GOV_box_Sen!=Sen_Block)
			{
				stepperMotorStop(GO.motor_v);
				GO.subtask=2;
				GO.sta=Ready;
			}
		}else if(GO.subtask==2)
		{
			if(GO.motor_v->motion==Stop)
			{
				GO.sta=Finish;
			}
		}
		break;
		///////////////复位至水平原点///////////////////
		case GO_start:
		if(GOH_start_Sen==Sen_Block)
		{
			stepperMotorStop(GO.motor_h);
			GO.motor_h->postion=0;
			GO.sta=Finish;
		}else
		{
			if(GO.motor_h->motion==Stop)
			{
				Error_Set(Error_Sensor,GOH_start_sensor);
			}
		}
		break;
		//////////////移动至封片工作点////////////////////
		case GO_package:
		if(GOH_mid_Sen==Sen_Block)
		{
			stepperMotorStop(GO.motor_h);
			GO.motor_h->postion=GO.GOH_mid_pos;
			GO.sta=Finish;
		}else
		{
			if(GO.motor_h->motion==Stop)
			{
				Error_Set(Error_Sensor,GOH_mid_sensor);
			}			
		}
		break;
		////////////移动到水平终点 玻片放置到存储槽////////////////
		case GO_end:
		if(GOH_end_Sen==Sen_Block)
		{
			stepperMotorStop(GO.motor_h);
			GO.motor_h->postion=GO.GOH_end_pos;
			GO.sta=Finish;
		}else
		{
			if(GO.motor_h->motion==Stop)
			{
				Error_Set(Error_Sensor,GOH_end_sensor);
			}			
		}
		break;
		case GO_out:
		if(GOV_glass_Sen!=Sen_Block)
		{
			stepperMotorStop(GO.motor_v);
			GO.sta=Finish;
		}else
		{
			if(GO.motor_v->motion==Stop&&GO.motor_v->postion<GO.GOV_box_len) //走完一个存储槽的距离后 仍然未到下一个槽
			{
				GO.sta=Ready;
				GO.task=GO_next; 
			}else if(GO.motor_v->postion>GO.GOV_box_len)
			{
				GO.sta=Ready;
				GO.task=GO_finish; 
			}
		}
		break;
		case GO_next:
		if(GOV_glass_Sen!=Sen_Block)
		{
			stepperMotorStop(GO.motor_v);
			GO.sta=Finish;
		}else
		{
			if(GO.motor_v->motion==Stop)
			{
				if(GO.motor_v->postion<GO.motor_v->max_pos)
				{
					GO.sta=Ready;
					GO.task=GO_finish;
				}else
				{
					GO.box_num++; //槽个数加1
					Error_Set(Error_Out_Box,0); //报错 存储槽满
				}
				
			}
		}
		break;
		case GO_adjust:
		if(GO.motor_v->motion==Stop)
		{
			GO.sta=Stop;
		}
		break;
		case GO_finish:break;
		case GO_error:break;
	}

}

void GO_FinishTask(void)
{
	switch(GO.task)
	{
		case GO_none:break;
		case GO_reset_on:
		GO.sta=Ready;
		GO.task=GO_none;
		GO.subtask=0;
		break;
		case GO_reset_off:
		GO.sta=Ready;
		GO.task=GO_none;
		GO.subtask=0;
		break;
		case GO_Box_Out:break;
		GO.sta=Ready;
		GO.task=GO_none;
		GO.subtask=0;
		case GO_Box_In:
		next_task=GO_none;
		GO.sta=Ready;
		GO.task=GO_adjust; //微调到槽下端
		GO.subtask=0;
		break;
		case GO_start:
		if(GC.task==GC_rot_up&&GC.sta==Finish) //等待夹手夹取玻片回到旋转原点
		{
			GO.sta=Ready;
			GO.task=GO_package;
		}else if(GC.task>=GC_finish) //夹手任务结束 存储任务也结束
		{
			GO.sta=Finish;
			GO.task=GO_finish;
		}
		break;
		case GO_package:
		if(GC.task<=GC_move_up) //当夹手离开封片区域 
		{
			GO.sta=Ready;
			GO.task=GO_end;
		}
		break;
		case GO_end:
		if(GOV_glass_Sen==Sen_Block) //移动到水平终点 且检测到玻片
		{
			GO.glass_num++; //封片玻片数量+1
			GO.sta=Ready;
			GO.task=GO_out;			
		}else
		{
			Error_Set(Error_Sensor,GOV_glass_sensor);
		}
		break;
		case GO_out:
		next_task=GO_start;
		GO.sta=Ready;
		GO.task=GO_adjust;
		break;
		case GO_next:
		GO.box_num++;
		next_task=GO_start;
		GO.sta=Ready;
		GO.task=GO_adjust;
		break;
		case GO_adjust:
		GO.sta=Ready;
		GO.task=next_task;
		next_task=GO_none;
		break;
		case GO_finish:break;
		case GO_error:break;
	}
}

void GO_TaskThread(void)
{

	if(GO.sta==Ready)
	{
		GO_ReadyTask();
		GO.running_tim=0;
	}
	if(GO.sta==Running)
	{
		GO_RunningTask();
		GO.running_tim++;
		if(GO.running_tim>OVERTIME)
		{
			Error_Set(Error_OverTime,0);
		}
	}
	if(GO.sta==Finish)
	{
		if(TaskThread_State!=taskthread_debug) //debug 模式下不进行任务跳转
		{
			GO_FinishTask();
		}
		
	}
	
}