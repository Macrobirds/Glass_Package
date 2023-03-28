#include "taskthread.h"

#define DELAY_S 1000
#define DELAY_BIG_CYLIDER 2000
#define DELAY_SMALL_CYLIDER 2000


static void Error_Set(enum task_error error,u32 error_sen)
{
	GP.sta=Finish;
	GP.task=GP_error;
	GP.error|=error;
	if(error_sen)
	{
		sensor_error_idx|=error_sen;
	}
}

void GP_ReadyTask(void)
{
	switch(GP.task)
	{
		case GP_none:break;
		case GP_reset_on:break;
		case GP_reset_off:break;
		////////////��С���׸�λ///////////////
		case GP_start:
		if(GP_spray_Sen!=Sen_Block) //δ�����罺ͷ ����
		{
			Error_Set(Error_Spray,0);
		}else
		{
			GP_big_Cyl=GAS_ENABLE;
			GP_small_Cyl=GAS_DISABLE;
			GP.sta=Running;
		}
		break;
		//////////��λ��ԭ��λ��///////////////
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
		///////////���ﲣƬ��ȡλ��////////////
		case GP_move_glass:
		GP.sta=Running;
		motorGo_acc(GP.motor,GP.sucker_pos);	
		break;
		//////////��Ƭ�����½�/////////////
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
		/////////��ȡ�ǲ�Ƭ///////////////
		case GP_suck_glass:
		if(GP_sucker_Sen==Sen_Block)
		{
			GP_sucker_Pump=GAS_ENABLE; //������������
			GP_sucker1_Cyl=GAS_ENABLE; //��������1 ����2
			GP_sucker2_Cyl=GAS_ENABLE;
			GP.sta=Running;
		}else
		{
			Error_Set(Error_Cover_Glass,0);
		}
		break;
		///////////��Ƭ��������///////////
		case GP_sucker_up:
		GP_big_Cyl=GAS_ENABLE;	
		GP.sta=Running;
		break;
		/////////////�ƶ����罺λ��/////////
		case GP_move_spray:
		if((GC.motor_r->postion==GC.GCR_hor_pos)&&(GC.motor_r->motion==Stop))
		{
			GP.sta=Running;
			motorGo_acc(GP.motor,GP.spray_pos);
		}
		break;
			/////////////�ƶ�����Ƭλ��///////// ��Ƭλ�ú�ԭ��λ���غ�
		case GP_move_package:
		if(GP_start_Sen==Sen_Block)
		{
			GP.sta=Finish;
		}else
		{
			GP.sta=Running;
			GP_spray_Cyl=GAS_ENABLE; //���罺��
			motorGo(GP.motor,0,GP.spray_speed); //���罺�ٶ������˶�����Ƭ��
		}
		break;
		////////////////��Ƭ����/////////////////////
		case GP_package:
		if(GP.subtask==0)
		{
			GP_big_Cyl=GAS_DISABLE; //�������½�
			
		}else if(GP.subtask==1)
		{
			GP_small_Cyl=GAS_DISABLE; //С��������
		}else if(GP.subtask==2)
		{
			GP_sucker1_Cyl=GAS_DISABLE; //�ر�����1����
		}else if(GP.subtask==3) 
		{
			GP_sucker2_Cyl=GAS_DISABLE; //�ر�����2���� ��������
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
		case GP_reset_on:break;
		case GP_reset_off:break;
		////////////////��С���׸�λ/////////////////
		case GP_start:
		if(GP.running_tim>DELAY_S)
		{
			if((GP_big_cyl_Sen==Sen_Block)&&(GP_small_cyl_Sen==Sen_Block))
			{
				GP.sta=Finish;
			}
			
		}
		break;
		//////////��λ��ԭ��λ��///////////////
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
		//////////��Ƭ�����½�/////////////
		case GP_sucker_down:
		if(GP.subtask==0)
		{
			if(GP.running_tim>DELAY_S) //�ȴ�С�������½�
			{
				if(GP_small_cyl_Sen!=Sen_Block)
				{
					GP.subtask=1;
					GP.sta=Ready;
				}
			}
		}else if(GP.subtask==1)
		{
			if(GP.running_tim>DELAY_S)//�ȴ��������½�
			{
				if(GP_big_cyl_Sen!=Sen_Block)
				{
					GP.subtask=0;
					GP.sta=Finish;
				}
			}
		}
		break;
		/////////��ȡ�ǲ�Ƭ///////////////
		case GP_suck_glass:
		if(GP.running_tim>DELAY_S)
		{
			GP.sta=Finish;
		}
		break;
		///////////��Ƭ��������///////////
		case GP_sucker_up:
		if(GP.running_tim>DELAY_S)
		{
			if(GP_big_cyl_Sen==Sen_Block)
			{
				GP.sta=Finish;
			}

		}
		break;
		/////////////�ƶ����罺λ��/////////
		case GP_move_spray:
		if(GP.motor->motion==Stop)
		{
			GP.sta=Finish;
		}
		break;
		/////////////�ƶ�����Ƭλ��///////// ��Ƭλ�ú�ԭ��λ���غ�
		case GP_move_package:
		if(GP.motor->postion<(GP.spray_pos-GP.spray_len)) //�����罺���� �ر��罺��
		{
			GP_spray_Cyl=GAS_DISABLE;
		}
		
		if(GP_start_Sen==Sen_Block) //�ﵽ��Ƭ����ԭ�� 
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
				Error_Set(Error_Sensor,GP_start_sensor); //����
			}
		}			
		break;
		////////////////��Ƭ����/////////////////////
		case GP_package:
		if(GP.subtask==0)
		{
			if(GP.running_tim>GP.delay_before)
			{
				GP.sta=Ready;
				GP.subtask=1;
			}
			
		}else if(GP.subtask==1)
		{
			if(GP.running_tim>GP.delay_after)
			{
				GP.sta=Ready;
				GP.subtask=2;
			}
		}else if(GP.subtask==2)
		{
			if(GP.running_tim>DELAY_S)
			{
				GP.sta=Ready;
				GP.subtask=3;
			}
		}else if(GP.subtask==3) 
		{
				GP.subtask=0;
				GP.sta=Finish;
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
		case GP_reset_on:break;
		case GP_reset_off:break;
		case GP_start:
		if(GC_claw_Sen!=Sen_Block&&GC.task==GC_rot_hor) //�ȴ�����ˮƽ״̬�ļ����ͷ�
		{
			GP.sta=Ready;
			GP.task=GP_move_start;
		}
		break;
		case GP_move_start:
		GP.sta=Ready;
		GP.task=GP_move_glass;
		break;
		case GP_move_glass:
		GP.sta=Ready;
		GP.task=GP_sucker_down;
		GP.subtask=0;
		break;
		case GP_sucker_down:
		if(GP_sucker_Sen==Sen_Block) //��⵽��Ƭ
		{
			GP.sta=Ready;
			GP.task=GP_suck_glass;
		}else
		{
			Error_Set(Error_Cover_Glass,0); //���� û�иǲ�Ƭ
		}
		break;
		case GP_suck_glass:
		GP.sta=Ready;
		GP.task=GP_sucker_up;
		break;
		case GP_sucker_up:
		if((GC.task==GC_rot_hor)&&(GC.sta==Finish)) //�ȴ����ֽ���Ƭ���õ���������
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
		case GP_package:break;
		case GP_finish:break;
		case GP_error:break;
	}
}

void GP_TaskThread(void)
{
	if(GP.sta==Ready)
	{
		GP_ReadyTask();
	}
	if(GP.sta==Running)
	{
		GP_RunningTask();
	}
	if(GP.sta==Finish)
	{
		GP_FinishTask();
	}

}