#include "taskthread.h"

#define DELAY_SUCKER 1000
#define DELAY_BIG_CYLIDER 2000
#define DELAY_SMALL_CYLIDER 2000


void GP_ReadyTask(void)
{
	switch(GP.task)
	{
		case GP_none:break;
		/////////////开机大小气缸复位///////////////
		case GP_reset_on:
		GP_small_Cyl=GAS_DISABLE;
		GP_big_Cyl=GAS_ENABLE;
		GP.sta=Running;
		break;
		/////////////关机大小气缸复位//////////////
		case GP_reset_off:
		GP_small_Cyl=GAS_ENABLE;
		GP_big_Cyl=GAS_ENABLE;
		GP.sta=Running;
		break;
		case GP_lack_glass:
		if(GP.subtask==0) //大小气缸上升
		{
			GP_big_Cyl=GAS_ENABLE;
			GP_small_Cyl=GAS_DISABLE;
			GP.sta=Running;
		}else if(GP.subtask==1)//复位回原点
		{
			motorGo_acc(GP.motor,0);
			GP.sta=Running;
		}

		break;
		////////////大小气缸复位///////////////
		case GP_start:
		if(GP_spray_Sen!=Sen_Block) //未放置喷胶头 报错
		{
			Error_Set(Error_Spray,0);
		}else
		{
			GP_big_Cyl=GAS_ENABLE;
			GP_small_Cyl=GAS_DISABLE;
			GP.sta=Running;
		}
		break;
		//////////复位至原点位置///////////////
		case GP_move_start:
		if(GP_start_Sen==Sen_Block)
		{
			GP.sta=Finish;
		}else
		{
			GP.sta=Running;
			motorGo_acc(GP.motor,0);
		}
		break;
		///////////到达玻片吸取位置////////////
		case GP_move_glass:
		GP.sta=Running;
		motorGo_acc(GP.motor,GP.sucker_pos);	
		break;
		//////////玻片吸盘下降/////////////
		case GP_sucker_down:
		if(GP.subtask==0)
		{
			GP_small_Cyl=GAS_ENABLE;
			GP.sta=Running;
		}else if(GP.subtask==1)
		{
			GP_big_Cyl=GAS_DISABLE;
			GP.sta=Running;
		}
		break;
		/////////吸取盖玻片///////////////
		case GP_suck_glass:
		if(GP_sucker_Sen==Sen_Block)
		{
			GP_sucker_Pump=GAS_ENABLE; //开启吸盘气泵
			GP_sucker1_Cyl=GAS_ENABLE; //开启吸盘1 吸盘2
			GP_sucker2_Cyl=GAS_ENABLE;
			GP.sta=Running;
		}else
		{
			///缺少盖玻片
			Error_Set(Error_Cover_Glass,0);
		}
		break;
		///////////玻片吸盘上升///////////
		case GP_sucker_up:
		GP_big_Cyl=GAS_ENABLE;	
		GP.sta=Running;
		break;
		/////////////移动到喷胶位置/////////
		case GP_move_spray:
		if((GC.motor_r->postion==GC.GCR_hor_pos)&&(GC.motor_r->motion==Stop)) //夹手位于旋转水平位置静止
		{
			GP.sta=Running;
			motorGo_acc(GP.motor,GP.spray_pos);
		}
		break;
			/////////////移动到封片位置///////// 封片位置和原点位置重合
		case GP_move_package:
		if(GP_start_Sen==Sen_Block)
		{
			GP.sta=Finish;
		}else
		{
			GP.sta=Running;
			GP_spray_Cyl=GAS_ENABLE; //打开喷胶阀
			motorGo(GP.motor,0,GP.spray_speed); //按喷胶速度匀速运动到封片点
		}
		break;
		////////////////封片工作/////////////////////
		case GP_package:
		if(GP.subtask==0)
		{
			GP_big_Cyl=GAS_DISABLE; //大气缸下降
			
		}else if(GP.subtask==1)
		{
			GP_small_Cyl=GAS_DISABLE; //小气缸上升
		}else if(GP.subtask==2)
		{
			GP_sucker1_Cyl=GAS_DISABLE; //关闭吸盘1气阀
		}else if(GP.subtask==3) 
		{
			GP_sucker2_Cyl=GAS_DISABLE; //关闭吸盘2气阀 吸盘气泵
			GP_sucker_Pump=GAS_DISABLE;
		}
		break;
		case GP_finish:break;
		case GP_error:break;
	}
	
}

void GP_RunningTask(void)
{
	switch(GP.task)
	{
		case GP_none:break;
		case GP_reset_on:
		if(GP.running_tim>DELAY_BIG_CYLIDER)
		{
			GP.sta=Finish;
		}
		break;
		case GP_reset_off:
		if(GP.running_tim>DELAY_BIG_CYLIDER)
		{
			GP.sta=Finish;
		}
		break;
		case GP_lack_glass:
		if(GP.subtask==0)
		{
			if(GP.running_tim>DELAY_BIG_CYLIDER*2)
			{
				if(GP_big_cyl_Sen==Sen_Block&&GP_small_cyl_Sen==Sen_Block)
				{
					GP.sta=Ready;
					GP.subtask=1;
				}
			}
		}else if(GP.subtask==1)
		{
			if(GP_start_Sen==Sen_Block)
			{
				stepperMotorStop(GP.motor);
				GP.motor->postion=0;
				GP.subtask=0;
				GP.sta=Finish;
			}else
			{
				Error_Set(Error_Sensor,GP_start_sensor);
			}
		}

		break;

		////////////////大小气缸复位/////////////////
		case GP_start:
		if(GP.running_tim>DELAY_BIG_CYLIDER)
		{
			if((GP_big_cyl_Sen==Sen_Block)&&(GP_small_cyl_Sen==Sen_Block))
			{
				GP.sta=Finish;
			}
			
		}
		break;
		//////////复位至原点位置///////////////
		case GP_move_start:
		if(GP_start_Sen==Sen_Block)
		{
			stepperMotorStop(GP.motor);
			GP.motor->postion=0;
			GP.sta=Finish;
		}else
		{
			Error_Set(Error_Sensor,GP_start_sensor);
		}
		break;
		case GP_move_glass:
		if(GP.motor->motion==Stop)
		{
			GP.sta=Finish;
		}
		break;
		//////////玻片吸盘下降/////////////
		case GP_sucker_down:
		if(GP.subtask==0)
		{
			if(GP.running_tim>DELAY_SMALL_CYLIDER) //等待小气缸先下降
			{
				if(GP_small_cyl_Sen!=Sen_Block)
				{
					GP.subtask=1;
					GP.sta=Ready;
				}
			}
		}else if(GP.subtask==1)
		{
			if(GP.running_tim>DELAY_BIG_CYLIDER)//等待大气缸下降
			{
				if(GP_big_cyl_Sen!=Sen_Block)
				{
					GP.subtask=0;
					GP.sta=Finish;
				}
			}
		}
		break;
		/////////吸取盖玻片///////////////
		case GP_suck_glass:
		if(GP.running_tim>DELAY_SUCKER)
		{
			GP.sta=Finish;
		}
		break;
		///////////玻片吸盘上升///////////
		case GP_sucker_up:
		if(GP.running_tim>DELAY_BIG_CYLIDER)
		{
			if(GP_big_cyl_Sen==Sen_Block)
			{
				GP.sta=Finish;
			}

		}
		break;
		/////////////移动到喷胶位置/////////
		case GP_move_spray:
		if(GP.motor->motion==Stop)
		{
			GP.sta=Finish;
		}
		break;
		/////////////移动到封片位置///////// 封片位置和原点位置重合
		case GP_move_package:
		if(GP.motor->postion<(GP.spray_pos-GP.spray_len)) //到达喷胶长度 关闭喷胶阀
		{
			GP_spray_Cyl=GAS_DISABLE;
		}
		
		if(GP_start_Sen==Sen_Block) //达到封片工作原点 
		{
			GP_spray_Cyl=GAS_DISABLE;
			stepperMotorStop(GP.motor);
			GP.motor->postion=0;
			GP.sta=Finish;
		}else
		{
			if(GP.motor->motion==Stop)
			{
				GP_spray_Cyl=GAS_DISABLE;
				Error_Set(Error_Sensor,GP_start_sensor); //报错
			}
		}			
		break;
		////////////////封片工作/////////////////////
		case GP_package:
		if(GP.subtask==0)
		{
			if(GP.running_tim>GP.delay_before) //等待大气缸下降 盖玻片与载玻片接触
			{
				GP.sta=Ready;
				GP.subtask=1;
			}
			
		}else if(GP.subtask==1) //等待小气缸上升 盖玻片与载玻片水平
		{
			if(GP.running_tim>GP.delay_after)
			{
				GP.sta=Ready;
				GP.subtask=2;
			}
		}else if(GP.subtask==2) //等待吸盘1关闭
		{
			if(GP.running_tim>DELAY_SUCKER)
			{
				GP.sta=Ready;
				GP.subtask=3;
			}
		}else if(GP.subtask==3) //等待吸盘2 吸盘气泵关闭 
		{
			if(GP.running_tim>DELAY_SUCKER)
			{
				GP.subtask=0;
				GP.sta=Finish;
			}

		}
		break;
		case GP_finish:break;
		case GP_error:break;
	}
}

void GP_FinishTask(void)
{
	switch(GP.task)
	{
		case GP_none:break;
		case GP_reset_on:
		GP.sta=Ready;
		GP.task=GP_none;
		break;
		case GP_reset_off:
		GP.sta=Ready;
		GP.task=GP_none;
		break;
		case GP_lack_glass:
		GP.task=GP_start; //保存当前状态为原点复位状态
		Error_Set(Error_Cover_Glass,0); //复位完成 报错 缺少盖玻片
		break;

		case GP_start:
		GP.sta=Ready;
		GP.task=GP_move_start;
		break;
		case GP_move_start:
		if(GC.task>=GC_move_down&&GC.task<GC_finish) //夹手处于夹取玻片的任务区间
		{
			GP.sta=Ready;
			GP.task=GP_move_glass;
		}else
		{
			GP.task=GP_finish; //当抓手任务出错 或结束时 封片任务也结束
			GP.sta=Finish;
		}
		break;
		case GP_move_glass:
		GP.sta=Ready;
		GP.task=GP_sucker_down;
		GP.subtask=0;
		break;
		case GP_sucker_down:
		if(GP_sucker_Sen==Sen_Block) //检测到玻片
		{
			GP.sta=Ready;
			GP.task=GP_suck_glass;
		}else
		{
			Error_Set(Error_Cover_Glass,0); //报错 没有盖玻片
		}
		break;
		case GP_suck_glass:
		if(GP_sucker_Sen==Sen_Block) //检测到玻片
		{
			GP.sta=Ready;
			GP.task=GP_sucker_up;
		}else
		{
			Error_Set(Error_Cover_Glass,0); //报错 没有盖玻片
		}			
		break;
		case GP_sucker_up:
		if((GC.task==GC_rot_hor)&&(GC.sta==Finish)) //等待夹手将玻片放置到承托盘上
		{
			GP.sta=Ready;
			GP.task=GP_move_spray;
		}
		break;
		case GP_move_spray:
		GP.sta=Ready;
		GP.task=GP_move_package;
		break;
		case GP_move_package:
		GP.sta=Ready;
		GP.task=GP_package;
		GP.subtask=0;
		break;
		case GP_package:
		if(GP_big_cyl_Sen!=Sen_Block&&GP_small_cyl_Sen==Sen_Block)  //等待大小气缸到位
		{
			GP.subtask=0;
			GP.sta=Ready;
			GP.task=GP_start;
		}
		break;
		case GP_finish:break;
		case GP_error:break;
	}
}

void GP_TaskThread(void)
{
	if(GP.sta==Ready)
	{
		GP_ReadyTask();
		GP.running_tim=0;
	}
	if(GP.sta==Running)
	{
		GP_RunningTask();
		GP.running_tim++;
		if(GP.running_tim>OVERTIME)
		{
			Error_Set(Error_OverTime,0);
		}
	}
	if(GP.sta==Finish)
	{
		GP_FinishTask();
	}

}