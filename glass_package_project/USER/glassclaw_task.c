#include "taskthread.h"
#define DELAY_MS 100



void GC_ReadyTask(void)
{
	switch(GC.task)
	{
		case GC_none:break;
		///////////////������λ//////////////////////
		case GC_reset_on:
		GC_claw_Cyl=GAS_DISABLE; //�����ͷ�
		GC.sta=Running;
		break;
		////////////////���ֹػ���λ//////////////////
		case GC_reset_off:
		if(GC.subtask==0) //��λ����ֱԭ��
		{
			if(GC_ver_Sen!=Sen_Block)
			{
				GC.subtask=1;
			}else
			{
				motorGo_acc(GC.motor_v,0);
				GC.sta=Running;
			}
		}else if(GC.subtask==1) //��ת����ת��ֱ��
		{
			if(GC.motor_r->postion==GC.GCR_ver_pos)
			{
				GC.subtask=2;
			}else
			{
				if(GOH_start_Sen==Sen_Block&&GO.motor_h->motion==Stop) //�ȴ���Ƭ���̻ص�ԭ��
				{
					motorGo_acc(GC.motor_r,GC.GCR_ver_pos);
					GC.sta=Running;
				}
			}
		}else if(GC.subtask==2) //��ת����תԭ��
		{
			if(GC_rot_Sen==Sen_Block)
			{
				GC.subtask=3;
			}else
			{
				if(GOH_start_Sen==Sen_Block&&GO.motor_h->motion==Stop) //�ȴ���Ƭ����װ���뿪ԭ��
				{
					motorGo_acc(GC.motor_r,0);
					GC.sta=Running;
				}
			}
		}else if(GC.subtask==3) //��ת����תˮƽ��
		{
			if(GOH_mid_Sen==Sen_Block&&GO.motor_h->motion==Stop) //�ȴ���Ƭ���̵�������
			{
				motorGo_acc(GC.motor_r,GC.GCR_hor_pos);
				GC.sta=Running;
			}
		}else if(GC.subtask==4) //�����ͷ�
		{
			GC_claw_Cyl=GAS_DISABLE;

		}else if(GC.subtask==5) //��ת����ת��ֱ��
		{
			if(GC_claw_Sen!=Sen_Block) 
			{
				motorGo_acc(GC.motor_r,GC.GCR_ver_pos); 
				GC.sta=Running;
			}

		}
		break;
		///////////////////��ֱ����λ��ԭ��/////////////////////
		case GC_ver_start:
		if(GC_ver_Sen==Sen_Block) //��ֱԭ���Ӧ
		{
			stepperMotorStop(GC.motor_v);
			GC.motor_v->postion=0;
			GC.sta=Finish;
		}else
		{
			GC_claw_Cyl=GAS_DISABLE; //�����ͷ�
			motorGo_acc(GC.motor_v,0); //��ֱ����λ��ԭ��
			GC.sta=Running;
		}
		break;
		///////////////////��ת����λ��ԭ��/////////////////////
		case GC_rot_start:
		if(GC.subtask==0)
		{
			if(GC_rot_Sen!=Sen_Block) //������ԭ��λ ����Ӧ
			{
				GC.motor_r->dir=Front;
				motor_Set_Direction(GC.motor_r);
				motorGO_Debug(GC.motor_r,GC.motor_r->pulse_1mm*400,0);
				GC.sta=Running;
			}else
			{
				GC.subtask=1;
			}
		}else if(GC.subtask==1)
		{
			if(GC_claw_Sen!=Sen_Block)
			{
				GC.motor_r->postion=-1;
				motorGo(GC.motor_r,0,0);
				GC.sta=Running;
			}
		}
		
		break;
		//////////////////������ת����ֱλ��//////////////////
		case GC_rot_down:
		GC_claw_Cyl=GAS_DISABLE; //�����ͷ�	
		motorGo_acc(GC.motor_r,GC.GCR_ver_pos);//������ת����ֱ
		GC.sta=Running;
		break;
		////////////////�����½�����Ƭװ�ز�////////////////
		case GC_move_down:
		if((GE_down_Sen==Sen_Block)&&(GE_up_Sen!=Sen_Block)) //��Ƭ�ƶ����������·�
		{
			motorGo_acc(GC.motor_v,GC.GCV_down_pos);
			GC.sta=Running;
		}
		break;
		//////////////////���ּн�/////////////////////////////
		case GC_grab:
		GC_claw_Cyl=GAS_ENABLE; //���ּн�	
		GC.sta=Running;
		break;
		//////////////////������������ֱԭ��////////////////////
		case GC_move_up:
		if(GC_ver_Sen==Sen_Block)
		{
			GC.sta=Finish;
		}else
		{
			motorGo_acc(GC.motor_v,0);
			GC.sta=Running;
		}
		break;
		/////////////////������ת����תԭ��///////////////
		case GC_rot_up:
		if(GC_rot_Sen==Sen_Block)
		{
			GC.sta=Finish;
		}else
		{
			if((GOH_start_Sen==Sen_Block)&&(GO.motor_h->motion==Stop)) //��Ƭ���̴���ԭ�㾲ֹ ��Ƭװ���ڸǲ�Ƭ���ȴ�
			{
				motorGo_acc(GC.motor_r,0);
				GC.sta=Running;
			}	
		}
		break;
		////////////////������ת��ˮƽλ��/////////////////////
		case GC_rot_hor:
		if((GOH_mid_Sen==Sen_Block)&&(GO.motor_h->motion==Stop)) //��Ƭ���̴��ڷ�Ƭ�����㾲ֹ 
		{
			if(GP.task<=GP_sucker_up) //��Ƭ����δ��ʼ
			{
				motorGo(GC.motor_r,GC.GCR_hor_pos,0);
				GC.sta=Running;
			}
		}
		break;
		////////////////////�����ͷ�////////////////////////////
		case GC_release:
		if(GP.task<=GP_sucker_up) //��Ƭ����δ��ʼ
		{
			GC_claw_Cyl=GAS_DISABLE; //�����ͷ�
			GC.sta=Running;
		}

		break;
		case GC_finish:break;
		case GC_error:break;
	}

}

void GC_RunningTask(void)
{
	switch(GC.task)
	{
		case GC_none:break;
		///////////////������λ///////////////////////////////
		case GC_reset_on:
		if(GC.running_tim>DELAY_MS) //��ʱ �ȴ������ͷ�
		{
			if(GC_claw_Sen!=Sen_Block)
			{
				GC.sta=Finish;
			}else
			{
				Error_Set(Error_Sensor,GC_claw_sensor);
			}
		}
		break;
		////////////////////////���ֹػ���λ/////////////////
		case GC_reset_off:
		if(GC.subtask==0) //�ȴ���λ����ֱԭ��
		{
			if(GC_ver_Sen==Sen_Block)
			{
				stepperMotorStop(GC.motor_v);
				GC.motor_v->postion=0;
				GC.sta=Ready;
				GC.subtask=1;
			}else
			{
				if(GC.motor_v->motion==Stop) //��ֱ���������� ��������
				{
					Error_Set(Error_Sensor,GC_ver_sensor);
				}
			}
		}else if(GC.subtask==1) //�ȴ���ת����ת��ֱλ��
		{
			if(GC.motor_r->motion==Stop)
			{
				GC.sta=Ready;
				GC.subtask=2;
			}

		}else if(GC.subtask==2) //�ȴ���ת����תԭ��
		{
			if(GC_rot_Sen==Sen_Block)
			{
				stepperMotorStop(GC.motor_r);
				GC.motor_r->postion=0;
				GC.sta=Ready;
				GC.subtask=3;
			}else
			{
				if(GC.motor_r->motion==Stop)
				{
					Error_Set(Error_Sensor,GC_rot_sensor);
				}
			}
		}else if(GC.subtask==3) //�ȴ���ת����תˮƽ
		{
			if(GC.motor_r->motion==Stop)
			{
				GC.sta=Ready;
				GC.subtask=4;
			}
		}else if(GC.subtask==4) //�ȴ������ͷ�
		{
			if(GC.running_tim>DELAY_MS)
			{
				if(GC_claw_Sen!=Sen_Block) //�����ͷ�
				{
					GC.sta=Ready;
					GC.subtask=5;
				}

			}

		}else if(GC.subtask==5) //�ȴ���ת����ת��ֱλ��
		{
			if(GC.motor_r->motion==Stop)
			{
				GC.sta=Finish;
				GC.subtask=0;
			}

		}
		
		break;
		case GC_ver_start:
		////////////���ָ�λ����ֱԭ��////////////////
		if(GC_ver_Sen==Sen_Block)
		{
			GC.sta=Finish;
		}else
		{
			if(GC.motor_v->motion==Stop) //��ֱ���������� ��������
			{
				Error_Set(Error_Sensor,GC_ver_sensor);
			}
		}
		break;
		///////////���ָ�λ����תԭ��////////////////
		case GC_rot_start:
		if(GC.subtask==0)
		{
			if(GC.motor_r->motion==Stop)
			{
				GC.subtask=1,
				GC.sta=Ready;
			}
		}else if(GC.subtask==1)
		{
			if(GC_rot_Sen!=Sen_Block)
			{
				GC.subtask=0;
				GC.sta=Finish;
			}else
			{
				if(GC.motor_r->motion==Stop)
				{
					Error_Set(Error_Sensor,GC_rot_sensor);
				}
			}
		}
		break;
		///////////////////������ת����ֱλ��/////////////////
		case GC_rot_down:
		if(GC.motor_r->motion==Stop) //������ת��ˮƽ
		{
			GC.sta=Finish;
		}			
		break;
		////////////////�����½�����Ƭװ�ز�////////////////
		case GC_move_down:
		if(GC.motor_v->motion==Stop) //�����½�����Ƭλ��
		{
			GC.sta=Finish;
		}			
		break;
		//////////////////�ȴ����ּн�///////////////////
		case GC_grab:
		if(GC.running_tim>DELAY_MS) //�ȴ���ʱ��ʱ ���ּн�
		{
			if(GC_claw_Sen==Sen_Block)
			{
				GC.sta=Finish;
			}else
			{
				Error_Set(Error_Sensor,GC_claw_sensor);
			}
		}
		break;
		////////////������������ֱԭ��////////////////
		case GC_move_up:
		if(GC_ver_Sen==Sen_Block)
		{
			stepperMotorStop(GC.motor_v);
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
		///////////////������ת����תԭ��////////////
		case GC_rot_up:
		if(GC_rot_Sen==Sen_Block)
		{
			stepperMotorStop(GC.motor_r);
			GC.motor_r->postion=0;
			GC.sta=Finish;
		}else
		{
			if(GC.motor_r->motion==Stop)
			{
				Error_Set(Error_Sensor,GC_rot_sensor);
			}
		}
		break;
		///////////////������ת��ˮƽλ��//////////
		case GC_rot_hor:
		if(GC.motor_r==Stop)
		{
			GC.sta=Finish;
		}
		break;
		////////////////////�����ͷ���ʱ��ʱ///////////////////////
		case GC_release:
		if(GC.running_tim>DELAY_MS)
		{
			GC.sta=Finish;
		}
		
		break;
		case GC_finish:break;
		case GC_error:break;
	}
}

void GC_FinishTask(void)
{
	switch(GC.task)
	{
		case GC_none:break;
		case GC_reset_on:
		GC.sta=Ready;
		GC.task=GC_ver_start;
		break;
		case GC_reset_off:
		GC.sta=Ready;
		GC.task=GC_none;
		break;
		case GC_ver_start:
		GC_claw_Cyl=GAS_DISABLE;
		if(GC_claw_Sen!=Sen_Block) //�ȴ������ͷ�
		{
			GC.sta=Ready;
			GC.task=GC_rot_start;
		}
		break;
		case GC_rot_start:
		GC_claw_Cyl=GAS_DISABLE;
		if(GC_claw_Sen!=Sen_Block) //�ȴ������ͷ�
		{
			GC.sta=Ready;
			GC.task=GC_none;
		}
		break;
		case GC_rot_down:
		GC_claw_Cyl=GAS_DISABLE;
		if((GC_claw_Sen!=Sen_Block)) //�ȴ������ͷ�
		{
			if((GE.task==GE_move_glass)&&(GE.sta==Finish))//�ȴ���Ƭ�ƶ����������·�
			{
				GC.sta=Ready;
				GC.task=GC_move_down;
			}else if(GE.task==GE_finish)//����Ƿ񻹴��ڲ�Ƭδ���� ��������� ���������
			{
				GC.sta=Finish;
				GC.task=GC_finish;
			}
		}
		break;
		case GC_move_down:
		GC.sta=Ready;
		GC.task=GC_grab;
		break;
		case GC_grab:
		if(GC_claw_Sen==Sen_Block) //�ȴ����ּн�
		{
				GC.sta=Ready;
				GC.task=GC_move_up;
		}
		break;
		case GC_move_up:
		if((GO.task==GO_start)&&(GO.sta==Finish)) //�ȴ���Ƭ���̻ص�ԭ��
		{
			GC.sta=Ready;
			GC.task=GC_rot_up;
		}
		break;
		case GC_rot_up:
		if((GO.task==GO_package)&&(GO.sta==Finish)) //�ȴ���Ƭ���̵���Ƭ������
		{
			GC.sta=Ready;
			GC.task=GC_rot_hor;
		}
		break;
		case GC_rot_hor:
		if(GP.task<=GP_sucker_up) //�ȴ���Ƭ������� ��ʼ��һ����
		{
			GC.sta=Ready;
			GC.task=GC_release;
		}
		break;
		case GC_release:
		if(GC_claw_Sen!=Sen_Block)
		{
			GC.sta=Ready;
			GC.task=GC_rot_down;
		}
		break;
		case GC_finish:break;
		case GC_error:break;
	}
}

void GC_TaskThread(void)
{
	if(GC.sta==Ready)
	{
		GC_ReadyTask();
		//���ؼ���ֵ
		GC.running_tim=0;
	}
	if(GC.sta==Running)
	{
		GC_RunningTask();
		//����ֵ���� ��ʱ����
		GC.running_tim++;
		if(GC.running_tim>OVERTIME)
		{
			Error_Set(Error_OverTime,0);
		}
	}
	if(GC.sta==Finish)
	{
		if(TaskThread_State!=taskthread_debug) //debug ģʽ�²����������Զ���ת
		{
			GC_FinishTask();
		}
		
	}
}