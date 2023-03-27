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
		case GE_reset_on:break;
		case GE_reset_off:break;
		/////////////////////װ�ز۸�λ��ԭ��//////////////////////////////////
		case GE_move_start: 
			if(GE_start_Sen==Sen_Block)
			{
				GE.motor->postion=0;
				GE.sta=Ready;
				GE.task=GE_move_front;
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
		if((GE_up_Sen==Sen_Block)&&(GE_down_Sen==Sen_Block	))
		{
			GE.sta=Ready;
			GE.task=GE_move_glass;
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
		if((GE_up_Sen!=Sen_Block)&&(GE_down_Sen!=Sen_Block	))	 //
		{
			GE.sta=Ready;
			GE.task=GE_move_front;
		}else
		{
				if(!GE.glass_Exist) //װ�ز����Ƿ�����ز�Ƭ
				{
					GE.sta=Ready;
					GE.task=GE_error;
					GE.error|=Error_Slide_Glass;
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
		case GE_reset_on:break;
		case GE_reset_off:break;
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
				GE.sta=Ready;
				GE.task=GE_error;
				GE.error|=Error_Sensor;
				sensor_error_idx|=GE_start_sensor;
			}
		}
		break;
			/////////////////////�ȴ�װ�ز�ǰ���ƶ����������·�///////////////////////////////
		case GE_move_front:
		if((GE_up_Sen==Sen_Block)&&(GE_down_Sen==Sen_Block	))
		{
			stepperMotorStop(GE.motor);
			GE.sta=Finish;
			GE.box_Exist=TRUE;
		}else
		{
			if(GE.motor->motion==Stop) //GE ���¶˴�����һֱδ��Ӧ
			{
				if(GE.box_Exist)
				{
					GE.sta=Ready;
					GE.task=GE_finish;
				}else //ȱ��װ�ز�
				{
					GE.sta=Ready;
					GE.task=GE_error;
					GE.error|=Error_In_Box;
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
		}else
		{
			if(GE.motor->motion==Stop) //���¶˴���������
			{
				GE.sta=Ready;
				GE.task=GE_error;
				sensor_error_idx|=GE_up_sensor;
				sensor_error_idx|=GE_down_Sen;
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
				GE.sta=Ready;
				GE.task=GE_error;
				sensor_error_idx|=GE_up_sensor;
				sensor_error_idx|=GE_down_sensor;
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
		case GE_reset_on:break;
		case GE_reset_off:break;
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
					GE.glass_Exist=TRUE;
				}
				if((GE_up_Sen==Sen_Block)&&(GE_down_Sen==Sen_Block	)) //����װ�زۺ��
				{
					GE.sta=Ready;
					GE.task=GE_move_back;
				}
		}
		break;
		case GE_move_back:
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
	}
	
	if(GE.task==Running)
	{
		GE_RunningTask();
		//��ʱ ����ʱ���г�ʱ�����ϱ�
	}
	
	if(GE.task==Finish)
	{
		GE_FinishTask();
	}
	
}