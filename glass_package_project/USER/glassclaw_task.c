#include "taskthread.h"
#define DELAY_MS 1000

static void Error_Set(enum task_error error,u32 error_sen)
{
	GC.sta=Ready;
	GC.task=GC_error;
	GC.error|=error;
	if(!error_sen)
	{
		sensor_error_idx|=error_sen;
	}
}


void GC_ReadyTask(void)
{
	switch(GC.task)
	{
		case GC_none:break;
		case GC_reset_on:break;
		case GC_reset_off:break;
		///////////////////垂直方向复位到原点/////////////////////
		case GC_ver_start:
		if(GC_ver_Sen==Sen_Block)
		{
			GC.sta=Ready;
			GC.task=GC_rot_start;
		}else
		{
			GC_claw_Cyl=GAS_DISABLE; //夹手释放
			motorGo_acc(GC.motor_v,0); //垂直方向复位至原点
			GC.sta=Running;
		}
		break;
				///////////////////旋转方向复位到原点/////////////////////
		case GC_rot_start:
		if(GC_rot_Sen==Sen_Block)
		{
			GC.sta=Ready;
			GC.task=GC_rot_down;
		}else
		{
			GC_claw_Cyl=GAS_DISABLE; //夹手释放
			motorGo_acc(GC.motor_r,0); //旋转方向复位至原点
			GC.sta=Running;
		}
		break;
		case GC_rot_down:
		GC_claw_Cyl=GAS_DISABLE; //夹手释放	
		motorGo_acc(GC.motor_r,GC.GCR_ver_pos);//夹手旋转至垂直
		GC.sta=Running;
		break;
		case GC_move_down:
		if((GE_down_Sen==Sen_Block)&&(GE_up_Sen!=Sen_Block)) //玻片移动至夹手正下方
		{
			motorGo_acc(GC.motor_r,GC.GCV_down_pos);
			GC.sta=Running;
		}
		break;
		case GC_grab:
		GC_claw_Cyl=GAS_ENABLE; //夹手夹紧	
		break;
		case GC_move_up:
		if(GC_ver_Sen==Sen_Block)
		{
			GC.sta=Ready;
			GC.task=GC_rot_up;
		}else
		{
			motorGo_acc(GC.motor_v,0);
			GC.sta=Running;
		}
		break;
		case GC_rot_up:break;
		case GC_rot_hor:break;
		case GC_release:break;
		case GC_finish:break;
		case GC_error:break;
	}

}

void GC_RunningTask(void)
{
	switch(GC.task)
	{
		case GC_none:break;
		case GC_reset_on:break;
		case GC_reset_off:break;
		case GC_ver_start:
		if(GC_ver_Sen==Sen_Block)
		{
			stepperMotorStop(GC.motor_v);
			GC.motor_v->motion=Stop;
			GC.motor_v->postion=0;
			GC.sta=Finish;
		}else
		{
			if(GC.motor_v->motion==Stop)
			{
				Error_Set(Error_Sensor,GC_ver_sensor);
			}
		}
		break;
		case GC_rot_start:
		if(GC_rot_Sen==Sen_Block)
		{
			stepperMotorStop(GC.motor_r);
			GC.motor_r->motion=Stop;
			GC.motor_r->postion=0;
		}else
		{
			if(GC.motor_r->motion==Stop)
			{
					GC.sta=Ready;
					GC.task=GC_error;
					GC.error|=Error_Sensor;
					sensor_error_idx|=GC_rot_sensor;
			}
		}
		break;
		case GC_rot_down:
		if(GC.motor_r->motion==Stop) //夹手旋转至水平
		{
			GE.sta=Finish;
		}			
		break;
		case GC_move_down:
		if(GC.motor_v->motion==Stop) //夹手下降至玻片位置
		{
			GE.sta=Finish;
		}			
		break;
		case GC_grab:
		if(GC.running_tim>DELAY_MS) //等待延时超时 夹手夹紧
		{
			GC.sta=Finish;
		}
		break;
		case GC_move_up:
		if(GC_ver_Sen==Sen_Block)
		{
			stepperMotorStop(GC.motor_v);
			GC.motor_v->motion=Stop;
			GC.motor_v->postion=0;
			GC.sta=Finish;
		}else
		{
		
		}
		break;
		case GC_rot_up:break;
		case GC_rot_hor:break;
		case GC_release:break;
		case GC_finish:break;
		case GC_error:break;
	}
}

void GC_FinishTask(void)
{
	switch(GC.task)
	{
		case GC_none:break;
		case GC_reset_on:break;
		case GC_reset_off:break;
		case GC_ver_start:
		if(GC_claw_Sen!=Sen_Block) //等待夹手释放
		{
			GC.sta=Ready;
			GC.task=GC_rot_start;
		}
		break;
		case GC_rot_start:
		if(GC_claw_Sen!=Sen_Block) //等待夹手释放
		{
			GC.sta=Ready;
			GC.task=GC_rot_down;
		}
		break;
		case GC_rot_down:
		if((GC_claw_Sen!=Sen_Block)) //等待夹手释放
		{
			if((GE.task==GE_move_glass)&&(GE.sta==Finish))//等待玻片移动至夹手正下方
			{
				GC.sta=Ready;
				GC.task=GC_move_down;
			}
		}
		break;
		case GC_move_down:
			GC.sta=Ready;
			GC.task=GC_grab;
			break;
		case GC_grab:
		if(GC_claw_Sen==Sen_Block)
		{
				GC.sta=Ready;
				GC.task=GC_move_up;
		}
		break;
		case GC_move_up:break;
		case GC_rot_up:break;
		case GC_rot_hor:break;
		case GC_release:break;
		case GC_finish:break;
		case GC_error:break;
	}
}

void GC_TaskThread(void)
{
	if(GC.sta==Ready)
	{
	
	}
	if(GC.sta==Running)
	{
	
	}
	if(GC.sta==Finish)
	{
	
	}
}