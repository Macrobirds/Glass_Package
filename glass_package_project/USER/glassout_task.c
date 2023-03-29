#include "taskthread.h"


static enum glass_out_task_index next_task=GO_none; //����


static void Error_Set(enum task_error error,u32 error_sen)
{
	GO.subtask=0;
	GO.sta=Finish;
	GO.task=GO_error;
	GO.error|=error;
	if(error_sen)
	{
		sensor_error_idx|=error_sen;
	}
}

void GO_ReadyTask(void)	
{
	switch(GO.task)
	{
		case GO_none:break;
		//////////////////�洢������////////////////
		case GO_reset_on:
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
			if(GOV_box_Sen==Sen_Block&&GOV_glass_Sen!=Sen_Block)
			{
				GO.sta=Finish;
				GO.subtask=0;
			}else
			{
				motorGo(GO.motor_v,0,0);
				GO.sta=Running;
			}
		}
		break;
		/////////////////�洢���Ƴ�////////////////
		case GO_reset_off:break;
		///////////////��λ��ˮƽԭ��///////////////////
		case GO_start:
		if(GOV_box_Sen==Sen_Block) //����Ƿ���ڴ洢��
		{
			if(GOH_start_Sen==Sen_Block) //����Ƿ���ˮƽԭ��
			{
				GO.sta=Finish;
			}else
			{
				GO.sta=Running;
				motorGo_acc(GO.motor_h,0);
			}
		}else
		{
			Error_Set(Error_Out_Box,0); //���� û�д洢��
		}	
		break;
		//////////////�ƶ�����Ƭ������////////////////////
		case GO_package:
		if(GOH_mid_Sen==Sen_Block)
		{
			GO.sta=Finish;
		}else
		{
			if((GC.motor_r->postion)==0&&(GC_claw_Sen==Sen_Block)) //���ּ�ȡ��Ƭ����תԭ��λ��
			{
				GO.sta=Running;
				motorGo_acc(GO.motor_h,GO.GOH_mid_pos); //�ƶ�����Ƭ������
			}
		}
		break;
		////////////�ƶ���ˮƽ�յ� ��Ƭ���õ��洢��////////////////
		case GO_end:
		if(GOH_end_Sen==Sen_Block)
		{
			GO.sta=Finish;
		}else
		{
			if(GOV_box_Sen==Sen_Block&&GOV_glass_Sen!=Sen_Block) //��⵽�洢�� �洢���ۿ�
			{
				GO.sta=Running;
				motorGo_acc(GO.motor_h,GO.GOH_end_pos);
			}else
			{
				Error_Set(Error_Sensor,GOV_glass_sensor|GOV_box_sensor); //���� ������
			}
		}
		break;
		///////////����Ƭ�Ƴ�����ǰ��һ��洢��///////////////
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
		////////////�ƶ�����һ�洢��///////////////////
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
		case GO_adjust:break;
		case GO_finish:break;
		case GO_error:break;
	}
}

void GO_RunningTask(void)
{
	switch(GO.task)
	{
		case GO_none:break;
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
		}else if(GO.subtask==1)
		{
			if(GOV_start_Sen!=Sen_Block)
			{
				if(GOV_box_Sen==Sen_Block&&GOV_glass_Sen!=Sen_Block)
				{
					GO.sta=Finish;
					GO.subtask=0;
				}else
				{
					if(GOV_box_Sen!=Sen_Block)
					{
						Error_Set(Error_Sensor,GOV_box_sensor);
					}
				}
			}else
			{
				stepperMotorStop(GO.motor_h);
				GO.motor_h->postion=GO.GOH_mid_pos;
				GO.sta=Finish;
			}
		}
		break;
		case GO_reset_off:break;
		///////////////��λ��ˮƽԭ��///////////////////
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
		//////////////�ƶ�����Ƭ������////////////////////
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
		////////////�ƶ���ˮƽ�յ� ��Ƭ���õ��洢��////////////////
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
			if(GO.motor_v->motion==Stop) //����һ���洢�۵ľ���� ��Ȼδ����һ����
			{
				GO.sta=Ready;
				GO.task=GO_next; 
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
				if(GO.motor_v->postion>GO.GOV_box_len)
				{
					GO.sta=Ready;
					GO.task=GO_finish;
				}else
				{
					Error_Set(Error_Sensor,GOV_glass_sensor);
				}
			}
		}
		break;
		case GO_adjust:break;
		case GO_finish:break;
		case GO_error:break;
	}

}

void GO_FinishTask(void)
{
	switch(GO.task)
	{
		case GO_none:break;
		
		case GO_reset_on:break;
		GO.sta=Ready;
		GO.task=GO_none;
		GO.subtask=0;
		case GO_reset_off:
		GO.sta=Ready;
		GO.task=GO_none;
		GO.subtask=0;
		break;
		case GO_start:
		if(GC.task==GC_rot_up&&GC.sta==Finish) //�ȴ����ּ�ȡ��Ƭ�ص���תԭ��
		{
			GO.sta=Ready;
			GO.task=GO_package;
		}else if(GC.task>=GC_finish) //����������� �洢����Ҳ����
		{
			GO.sta=Finish;
			GO.task=GO_finish;
		}
		break;
		case GO_package:
		if(GC.task<=GC_move_up) //�������뿪��Ƭ���� 
		{
			GO.sta=Ready;
			GO.task=GO_end;
		}
		break;
		case GO_end:
		if(GOV_glass_Sen==Sen_Block) //�ƶ���ˮƽ�յ� �Ҽ�⵽��Ƭ
		{
			GO.glass_num++; //��Ƭ��Ƭ����+1
			GO.sta=Ready;
			GO.task=GO_out;			
		}else
		{
			Error_Set(Error_Sensor,GOV_glass_sensor);
		}
		break;
		case GO_out:
		GO.sta=Ready;
		GO.task=GO_start;
		break;
		case GO_next:
		GO.box_num++;
		GO.sta=Ready;
		GO.task=GO_start;
		break;
		case GO_adjust:break;
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
		GO_FinishTask();
	}
	
}