#include "taskthread.h"

//����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
static u8 check_GC(void)
{
	return (GC_ver_Sen==Sen_Block)&&(GC.motor_v->motion==Stop);
}



void GE_ReadyTask(void)
{
	switch(GE.task)
	{
		case GE_none:break;
		/////////////////������λ//////////////////////////////
		/////////////////װ�ز۽���//////////////////////////////
		case GE_reset_on:
		if(GE_start_Sen==Sen_Block)
		{
			stepperMotorStop(GE.motor);
			GE.motor->postion=0;
			GE.sta=Finish;
		}else
		{
			if(check_GC())
			{
				motorGo_acc(GE.motor,0);
				GE.sta=Running;
			}
		}
		break;
		//////////////װ�ز��Ƴ�///////////////////////////
		case GE_reset_off:
		if(GE.subtask==0)
		{
			if(GE_start_Sen==Sen_Block)
			{
				GE.subtask=1;
			}else
			{
				if(check_GC())
				{
					motorGo_acc(GE.motor,0);
					GE.sta=Running;
				}

			}
		}else if(GE.subtask==1)
		{
			if(check_GC())
			{
				motorGo_acc(GE.motor,GE.box_Exist);
				GE.sta=Running;
			}
			
		}
		break;
		/////////////////////װ�ز۸�λ��ԭ��//////////////////////////////////
		case GE_move_start: 
			if(GE_start_Sen==Sen_Block)
			{
				GE.motor->postion=0;
				GE.sta=Finish;
			}else
			{
				if(check_GC()) //����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
				{
					motorGo_acc(GE.motor,0);
					GE.sta=Running;
				}
			}
		break;
		////////////////////װ�ز�ǰ���ƶ����������·�////////////////////////
		case GE_move_front: 
		if((GE_up_Sen==Sen_Block)&&(GE_down_Sen==Sen_Block))
		{
			GE.sta=Finish;
			GE.box_Exist=TRUE;
		}else
		{
			if(check_GC()) //����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
			{
				motorGo_acc(GE.motor,(GE.GE_box_dis+GE.GE_box_len)*2);
				GE.sta=Running;
			}
		}
		break;
		//////////////////////��Ƭ�ƶ����������·�//////////////////////////////////
		case GE_move_glass: 
		if(GE_down_Sen==Sen_Block)
		{
			GE.sta=Finish;
		}else
		{
			if(check_GC()) //����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
			{
				motorGo(GE.motor,GE.GE_box_len,0);
				GE.sta=Running;
			}
		}
		break;
		///////////////////װ�زۺ���뿪�����·�////////////////////////////////////////////
		case GE_move_back:
		if((GE_up_Sen!=Sen_Block)&&(GE_down_Sen!=Sen_Block))	 //���뿪װ�زۺ��
		{
			GE.sta=Ready;
			GE.task=GE_move_front;
		}else
		{
				if(!GE.glass_Exist) //װ�ز����Ƿ�����ز�Ƭ
				{
					Error_Set(Error_Slide_Glass,0);
				}
			
				if(check_GC()) //����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
				{
					motorGo_acc(GE.motor,GE.GE_box_dis);
					GE.sta=Running;
					GE.glass_Exist=FALSE;
				}
		}
		break;
			///////////////////////////////�����������//////////////////////////////
		case GE_finish:
			
		break;
		////////////////////////////////�쳣����///////////////////////////////
		case GE_error:
		//ֹͣ����
			stepperMotorStop(GE.motor);
		//ȱ��װ�ز�
		
		//װ�ز�ȱ���ز�Ƭ
		
		//����������
		
		
		GE.sta=Ready;
		GE.task=GE_none;
		break;
	}
}

void GE_RunningTask(void)
{
	switch(GE.task)
	{
		case GE_none:break;
		break;
		/////////////////װ�ز۽���//////////////////////////////		
		case GE_reset_on:
		if(GE_start_Sen==Sen_Block)
		{
			stepperMotorStop(GE.motor);
			GE.motor->postion=0;
			GE.sta=Finish;
		}else
		{
			Error_Set(Error_Sensor,GE_start_sensor);
		}
		break;
		//////////////װ�ز��Ƴ�///////////////////////////	
		case GE_reset_off:
		if(GE.subtask==0)
		{
			if(GE_start_Sen==Sen_Block)
			{
				stepperMotorStop(GE.motor);
				GE.motor->postion=0;
				GE.sta=Ready;
				GE.subtask=1;
			}else
			{
				if(GE.motor->motion==Stop) //GEԭ�㴫��������
				{
					Error_Set(Error_Sensor,GE_start_sensor);
				}
			}
		}else if(GE.subtask==1)
		{
			if(GE.motor->motion==Stop)
			{
				GE.sta=Finish;
				GE.subtask=0;
			}
		}
		break;
			/////////////////////װ�ز۸�λ��ԭ��//////////////////////////////////
		case GE_move_start:
		if(GE_start_Sen==Sen_Block)
		{
				stepperMotorStop(GE.motor);
				GE.motor->postion=0;
				GE.sta=Finish;
		}else
		{
			if(GE.motor->motion==Stop) //GEԭ�㴫��������
			{
				Error_Set(Error_Sensor,GE_start_sensor);
			}
		}
		break;
			/////////////////////�ȴ�װ�ز�ǰ���ƶ����������·�///////////////////////////////
		case GE_move_front:
		if((GE_up_Sen==Sen_Block)&&(GE_down_Sen==Sen_Block))
		{
			stepperMotorStop(GE.motor);
			GE.sta=Finish;
			GE.box_Exist=TRUE;
		}else
		{
			if(GE.motor->motion==Stop) //GE ���¶˴�����һֱδ��Ӧ
			{
				if(GE.box_Exist) //�����Ѵ���������װ�ز� ��������
				{
					GE.sta=Finish;
					GE.task=GE_finish;
				}else //ȱ��װ�ز�
				{
					Error_Set(Error_In_Box,0);
				}
			}
		}
		break;
					/////////////////////��Ƭ�ƶ����������·�//////////////////////////////////
		case GE_move_glass:
		if((GE_up_Sen==Sen_Block)||(GE_down_Sen==Sen_Block))
		{
				stepperMotorStop(GE.motor);
				GE.sta=Finish;
				if(GE_up_Sen!=Sen_Block&&GE_down_Sen==Sen_Block)
				{
					GE.glass_Exist=TRUE;
				}
		}else
		{
			if(GE.motor->motion==Stop) //���¶˴���������
			{
				Error_Set(Error_Sensor,GE_up_sensor|GE_down_sensor);
			}
		
		}	
		break;
					/////////////////////װ�زۺ���뿪�������·�//////////////////////////////////
		case GE_move_back:
		if((GE_up_Sen!=Sen_Block)&&(GE_down_Sen!=Sen_Block))
		{
			stepperMotorStop(GE.motor);
			GE.sta=Finish;
		}else
		{
			if(GE.motor->motion==Stop) //���¶˴���������
			{
				Error_Set(Error_Sensor,GE_up_sensor|GE_down_sensor);
			}
		}
		break;
	
		case GE_finish:
			
		break;
		
		case GE_error:
			
		break;
	}
}

void GE_FinishTask(void)
{
	switch(GE.task)
	{
		case GE_none:break;
		case GE_reset_on:
		GE.task=GE_none;
		GE.sta=Ready;
		break;
		case GE_reset_off:
		GE.task=GE_none;
		GE.sta=Ready;
		GE.subtask=0;
		break;
		case GE_move_start: //��ת��װ�ز�ǰ���ƶ����������·�
		GE.sta=Ready;
		GE.task=GE_move_front;
		break;
		case GE_move_front://��ת����Ƭǰ���ƶ����������·�
		GE.sta=Ready;
		GE.task=GE_move_glass;
		break;
		case GE_move_glass:
		if(check_GC()) //����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
		{
			if(GE_down_Sen!=Sen_Block) //��Ƭ����ȡ �¶��޸�Ӧ
			{
				GE.sta=Ready;
			}
		}
		if((GE_up_Sen==Sen_Block)&&(GE_down_Sen==Sen_Block)) //����װ�زۺ��
		{
			GE.sta=Ready;
			GE.task=GE_move_back;
		}		
		break;
		case GE_move_back:
		GE.glass_Exist=FALSE;
		GE.sta=Ready;
		GE.task=GE_move_front;
		break;
		case GE_finish:break;
		case GE_error:break;
	}
}

void GE_TaskThread(void)
{
	if(GE.task==Ready)
	{
		GE_ReadyTask();
		//���ؼ�ʱ
		GE.running_tim=0;
	}
	
	if(GE.task==Running)
	{
		GE_RunningTask();
		//��ʱ ����ʱ���г�ʱ�����ϱ�
		GE.running_tim++;
		if(GE.running_tim>OVERTIME)
		{
			Error_Set(Error_OverTime,0);
		}
	}
	
	if(GE.task==Finish)
	{
		GE_FinishTask();
	}
	
}