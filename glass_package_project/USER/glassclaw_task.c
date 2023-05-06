#include "taskthread.h"
#define DELAY_MS 100
#define GLASS_DIS 16000

// �����Ƿ��赲
static u8 Check_GP(void)
{
//#ifdef DEBUG_MODE
//	return TRUE;
//#else
	if (GC_claw_Sen == Sen_Block) // ���ּн�״̬
	{
		if (GOH_start_Sen==Sen_Block&& GO.motor_h->motion == Stop)/* && GP.motor->postion == GP.sucker_pos && GP.motor->motion == Stop*/
		{
			return TRUE;
		}
	}
	else
	{
		if(GO.motor_h->motion==Stop)
		{
				return TRUE;
		}

	}
	return FALSE;
//#endif
}

static void Resume_Task(void)
{
	GC.sta = Ready;
	GC.subtask = GC.main_subtask;
	GC.task = GC.main_task;
}
// ��ת��ָ��������ִ�� Task ������ Next_task ��һ������
static void Next_Task(enum glass_claw_task_index Resume_task,enum glass_claw_task_index Next_task)
{
	GC.main_task = Resume_task; // ָ���ָ�������

	if (Resume_task == GC.task) // �������Ƿ�任
	{
		GC.main_subtask = GC.subtask + 1;
	}
	else
	{
		GC.main_subtask = 0;
	}

	GC.sta = Ready; // ��ת����һ����ִ��
	GC.task = Next_task;

}

void GC_ReadyTask(void)
{
//	if(GC.task!=GC_none)
//	{
//			printf("GC task:%d\r\n ",GC.task);
//	}
	switch (GC.task)
	{
	case GC_none:
		break;
	///////////////������λ//////////////////////
	case GC_reset_on:

		if (GC.subtask == 0) // �����ͷ�
		{
			Next_Task(GC.task, GC_release);
		}
		else if (GC.subtask == 1) // ��λ����ֱԭ��
		{
			Next_Task(GC.task, GC_ver_start);
		}
		else if (GC.subtask == 2) // �������תԭ��λ�� �뿪ԭ��λ�� ����У׼λ��
		{

			if (GC_rot_Sen != Sen_Block)
			{
				GC.motor_r->dir = Front;
				motor_Set_Direction(GC.motor_r);
				motorGO_Debug(GC.motor_r, GC.motor_r->pulse_1mm * 400, 0);
				GC.sta = Running;
			}
			else
			{
				GC.subtask = 3;
			}
		}
		else if (GC.subtask == 3) // ��λ����תԭ��
		{
			Next_Task(GC_none, GC_rot_start);
		}

		break;
	////////////////���ֹػ���λ//////////////////
	case GC_reset_off:
		printf("GC reset on:%d\r\n ",GC.subtask);
		if (GC.subtask == 0) // ��λ����ֱԭ��
		{
			Next_Task(GC.task, GC_ver_start);
		}
		else if (GC.subtask == 1) // ��λ����תԭ��
		{
			if (Check_GP())
			{
				Next_Task(GC.task, GC_rot_start);
			}
		}
		else if (GC.subtask == 2)
		{
			if (GC_claw_Sen == Sen_Block) // ���ּн�״̬ �ȴ������ƶ���������
			{
				if (GO.motor_h->postion == GO.GOH_mid_pos && GO.motor_h->motion == Stop) // ������ת��ˮƽ
				{
					/*****************************/
					GC.sta=Running;
					motorGo(GC.motor_r,GC.GCR_hor_pos,0);
					/*****************************/
				}
			}
			else
			{
				GC.sta=Finish; //��������
			}
		}
		else if (GC.subtask == 3) // �����ͷ�
		{
			Next_Task(GC.task, GC_release);
		}
		else if (GC.subtask == 4) // ��λ����תԭ��
		{
			Next_Task(GC_none, GC_rot_start);
		}
		break;
	///////////////////��ʼ��������//////////////////////////
	case GC_start:
		//�ͷż��� ����ת����ת��ֱ����
		GC.sta=Finish;
		break;
	///////////////////��ֱ����λ��ԭ��/////////////////////basic
	case GC_ver_start:
			motorGo_acc(GC.motor_v, 0); // ��ֱ����λ��ԭ��
			GC.sta = Running;
		break;
	///////////////////��ת����λ��ԭ��///////////////////// basic
	case GC_rot_start:
		if (GC_rot_Sen != Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if(Check_GP())
			{
				motorGo_acc(GC.motor_r, 0);
				GC.sta = Running;
			}

		}

		break;
	//////////////////������ת����ֱλ��////////////////// 
	case GC_rot_down:
		if(GC.motor_r->postion==GC.GCV_down_pos)
		{
			GC.sta=Finish;
		}else
		{
			if(Check_GP())
			{
					motorGo_acc(GC.motor_r, GC.GCR_ver_pos); // ������ת����ֱ
					GC.sta = Running;
			}
		}

		break;
	////////////////�����½�����Ƭװ�ز�//////////////// 
	case GC_move_down:
			// ��Ƭ�ƶ����������·�		
			if(GE_up_Sen!=Sen_Block&&GE_down_Sen==Sen_Block)
			{
					motorGo_acc(GC.motor_v, GC.GCV_down_pos);
					GC.sta = Running;
			}

		break;
	//////////////////���ּн�///////////////////////////// basic
	case GC_grab:
		GC_claw_Cyl = GAS_ENABLE; // ���ּн�
		GC.sta = Running;
		break;
	////////////////������ת��ˮƽλ��///////////////////// 
	case GC_check_glass:
		motorGo_acc(GC.motor_v,GC.GCV_down_pos-GLASS_DIS); //�����ƶ�һ������ ����Ƿ���ڲ�Ƭ
		GC.sta=Running;
		break;
	case GC_move_up:
		motorGo_acc(GC.motor_v,0);
		GC.sta=Running;
		break;
	case GC_rot_up:
		if (GC_rot_Sen != Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if(Check_GP())
			{
				motorGo_acc(GC.motor_r, 0);
				GC.sta = Running;
			}
		}
		break;
	case GC_rot_hor:
		if(GO.motor_h->postion==GO.GOH_mid_pos &&GO.motor_h->motion==Stop)
		{
			motorGo(GC.motor_r, GC.GCR_hor_pos, 0);
			GC.sta = Running;
		}
		break;
	////////////////////�����ͷ�//////////////////////////// basic
	case GC_release:
		GC_claw_Cyl = GAS_DISABLE; // �����ͷ�
		GC.sta = Running;
		break;
	case GC_finish:
		break;
	case GC_error:
		break;
	}
}

void GC_RunningTask(void)
{
	switch (GC.task)
	{
	case GC_none:
		break;
	///////////////������λ///////////////////////////////
	case GC_reset_on:
		if (GC.subtask == 0) // �����ͷ�
		{
		}
		else if (GC.subtask == 1) // ��λ����ֱԭ��
		{
		}
		else if (GC.subtask == 2) // �������תԭ��λ�� �뿪ԭ��λ�� ����У׼λ��
		{
			if (GC.motor_r->motion == Stop)
			{
				GC.sta = Ready;
				GC.subtask = 3;
			}
		}
		else if (GC.subtask == 3) // ��λ����תԭ��
		{
		}
		break;
	////////////////////////���ֹػ���λ/////////////////
	case GC_reset_off:
		if (GC.subtask == 0) // ��λ����ֱԭ��
		{
		}
		else if (GC.subtask == 1) // ��λ����תԭ��
		{
		}
		else if (GC.subtask == 2)
		{
			if(GC.motor_r->motion==Stop)
			{
				GC.sta=Ready;
				GC.subtask++;
			}
		}
		else if (GC.subtask == 3) // �����ͷ�
		{
		}
		else if (GC.subtask == 4) // ��λ����תԭ��
		{
		}
		break;
	/////////////////��ʼ����////////////////////
	case GC_start:

		break;
	case GC_ver_start:
		////////////���ָ�λ����ֱԭ��////////////////basic
		if (GC_ver_Sen == Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (GC.motor_v->motion == Stop) // ��ֱ���������� ��������
			{
				//�������� �����󱨴�
				//Error_Set(Error_Sensor, GC_ver_sensor);
			}
		}
		break;
	///////////���ָ�λ����תԭ��//////////////// basic
	case GC_rot_start:
		if (GC_rot_Sen != Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (GC.motor_r->motion == Stop)
			{
				//������ز���  �����󱨴�
				//	Error_Set(Error_Sensor, GC_rot_sensor);
			}
		}
		break;
	///////////////////������ת����ֱλ��/////////////////
	case GC_rot_down:
		if (GC.motor_r->motion == Stop) // ������ת��ˮƽ
		{
			GC.sta = Finish;
		}
		break;
	////////////////�����½�����Ƭװ�ز�////////////////
	case GC_move_down:
		if (GC.motor_v->motion == Stop) // �����½�����Ƭλ��
		{
			GC.sta = Finish;
		}
		break;
	//////////////////�ȴ����ּн�/////////////////// basic
	case GC_grab:
		if (GC.running_tim > DELAY_MS) // �ȴ���ʱ��ʱ ���ּн�
		{
			if (GC_claw_Sen == Sen_Block)
			{
				GC.sta = Finish;
			}
			else
			{
				Error_Set(Error_Sensor, GC_claw_sensor);
			}
		}
		break;
	///////////////������ת��ˮƽλ��//////////
	case GC_check_glass:
			if(GC.motor_v->motion==Stop)
			{
				GC.sta=Finish;
			}
		break;
	case GC_move_up:
			if(GC_ver_Sen==Sen_Block)
			{
				GC.sta=Finish;
			}else
			{
				if(GC.motor_v->motion==Stop)
				{
					//Error_Set(Error_Sensor,GC_ver_sensor);
				}
			}
		break;
	case GC_rot_up:
		if(GC_rot_Sen!=Sen_Block)
		{
			GC.sta=Finish;
		}else
		{
			if(GC.motor_r->motion==Stop)
			{
				//Error_Set(Error_Sensor,GC_rot_sensor);
			}
		}
	case GC_rot_hor:
		if (GC.motor_r->motion == Stop)
		{
			GC.sta = Finish;
		}
		break;
	////////////////////�����ͷ���ʱ��ʱ///////////////////////
	case GC_release:
		if (GC.running_tim > DELAY_MS)
		{
			if (GC_claw_Sen != Sen_Block)
			{
				GC.sta = Finish;
			}
			else
			{
				Error_Set(Error_Sensor, GC_claw_sensor);
			}
		}

		break;
	case GC_finish:
		break;
	case GC_error:
		break;
	}
}

void GC_FinishTask(void)
{
	switch (GC.task)
	{
	case GC_none:
		break;
	case GC_reset_on:
		GC.main_task = GC_none;
		GC.main_subtask = 0;
		GC.subtask = 0;
		GC.sta = Ready;
		GC.task = GC_none;
		break;
	case GC_reset_off:
		GC.main_task = GC_none;
		GC.main_subtask = 0;
		GC.subtask = 0;
		GC.sta = Ready;
		GC.task = GC_none;
		break;
	case GC_start: 
		if(GP.task<=GP_suck_glass) //δ��ʼ��Ƭ
		{
				Next_Task(GC_rot_down,GC_release);
		}	
		break;
	case GC_ver_start:
		Resume_Task();
		break;
	case GC_rot_start:
		Resume_Task();
		break;
	case GC_rot_down:
		if (GE.task==GE_move_glass&&GE.sta==Finish) //�ȴ���Ƭ�ƶ����������·�
		{
				GC.sta=Ready;
				GC.subtask=0;
				GC.task=GC_move_down;
		}
		break;
	case GC_move_down:
		Next_Task(GC_check_glass,GC_grab);
		break;
	case GC_grab:
		Resume_Task();
		break;
	case GC_check_glass:
		GC.sta=Ready;
		GC.task=GC_move_up;
//		if(GE_up_Sen==Sen_Block)	//��ȡ����Ƭ
//		{
//			GC.sta=Ready;
//			GC.task=GC_move_up;
//		}else
//		{
//			//Error_Set(Error_Slide_Glass,0); �ز�Ƭ����
//			//GC_claw_Cyl=GAS_DISABLE; //�����ͷ�
//			printf("glass error\r\n");
//			Next_Task(GC_none,GC_ver_start); //��λ�ش�ֱԭ�� ���¼�ȡ��Ƭ
//		}
		break;
	case GC_move_up:
		if(GO.task==GO_start&&GO.sta==Finish)
		{
			GC.sta=Ready;
			GC.task=GC_rot_up;
		}
		break;
	case GC_rot_up:
		if(GO.task==GOH_package&&GO.sta==Finish)
		{
			GC.sta=Ready;
			GC.task=GC_rot_hor;
		}
		break;		
	
	case GC_rot_hor:
		if(GP.task==GP_move_glass) //��Ƭ����
		{
				GC.sta=Ready;
				GC.task=GC_start;
		}
		break;
	case GC_release:
		Resume_Task();
		break;
	case GC_finish:
		break;
	case GC_error:
		break;
	}
}

void GC_TaskThread(void)
{
	if (GC.sta == Ready)
	{
		GC_ReadyTask();
		// ���ؼ���ֵ
		GC.running_tim = 0;
	}
	if (GC.sta == Running)
	{
		GC_RunningTask();
		// ����ֵ���� ��ʱ����
		GC.running_tim++;
		if (GC.running_tim > OVERTIME)
		{
			Error_Set(Error_OverTime, 0);
		}
	}
	if (GC.sta == Finish)
	{
		if (TaskThread_State != taskthread_debug) // debug ģʽ�²����������Զ���ת
		{
			GC_FinishTask();
		}
	}
}

// if(GC.subtask==0) //��λ����ֱԭ��
// {
// 	if(GC_ver_Sen!=Sen_Block)
// 	{
// 		GC.subtask=1;
// 	}else
// 	{
// 		motorGo_acc(GC.motor_v,0);
// 		GC.sta=Running;
// 	}
// }else if(GC.subtask==1) //��ת����ת��ֱ��
// {
// 	if(GC.motor_r->postion==GC.GCR_ver_pos)
// 	{
// 		GC.subtask=2;
// 	}else
// 	{
// 		if(GOH_start_Sen==Sen_Block&&GO.motor_h->motion==Stop) //�ȴ���Ƭ���̻ص�ԭ��
// 		{
// 			motorGo_acc(GC.motor_r,GC.GCR_ver_pos);
// 			GC.sta=Running;
// 		}
// 	}
// }else if(GC.subtask==2) //��ת����תԭ��
// {
// 	if(GC_rot_Sen==Sen_Block)
// 	{
// 		GC.subtask=3;
// 	}else
// 	{
// 		if(GOH_start_Sen==Sen_Block&&GO.motor_h->motion==Stop) //�ȴ���Ƭ����װ���뿪ԭ��
// 		{
// 			motorGo_acc(GC.motor_r,0);
// 			GC.sta=Running;
// 		}
// 	}
// }else if(GC.subtask==3) //��ת����תˮƽ��
// {
// 	if(GOH_mid_Sen==Sen_Block&&GO.motor_h->motion==Stop) //�ȴ���Ƭ���̵�������
// 	{
// 		motorGo_acc(GC.motor_r,GC.GCR_hor_pos);
// 		GC.sta=Running;
// 	}
// }else if(GC.subtask==4) //�����ͷ�
// {
// 	GC_claw_Cyl=GAS_DISABLE;

// }else if(GC.subtask==5) //��ת����ת��ֱ��
// {
// 	if(GC_claw_Sen!=Sen_Block)
// 	{
// 		motorGo_acc(GC.motor_r,GC.GCR_ver_pos);
// 		GC.sta=Running;
// 	}

// }

// if(GC.subtask==0) //�ȴ���λ����ֱԭ��
// {
// 	if(GC_ver_Sen==Sen_Block)
// 	{
// 		stepperMotorStop(GC.motor_v);
// 		GC.motor_v->postion=0;
// 		GC.sta=Ready;
// 		GC.subtask=1;
// 	}else
// 	{
// 		if(GC.motor_v->motion==Stop) //��ֱ���������� ��������
// 		{
// 			Error_Set(Error_Sensor,GC_ver_sensor);
// 		}
// 	}
// }else if(GC.subtask==1) //�ȴ���ת����ת��ֱλ��
// {
// 	if(GC.motor_r->motion==Stop)
// 	{
// 		GC.sta=Ready;
// 		GC.subtask=2;
// 	}

// }else if(GC.subtask==2) //�ȴ���ת����תԭ��
// {
// 	if(GC_rot_Sen==Sen_Block)
// 	{
// 		stepperMotorStop(GC.motor_r);
// 		GC.motor_r->postion=0;
// 		GC.sta=Ready;
// 		GC.subtask=3;
// 	}else
// 	{
// 		if(GC.motor_r->motion==Stop)
// 		{
// 			Error_Set(Error_Sensor,GC_rot_sensor);
// 		}
// 	}
// }else if(GC.subtask==3) //�ȴ���ת����תˮƽ
// {
// 	if(GC.motor_r->motion==Stop)
// 	{
// 		GC.sta=Ready;
// 		GC.subtask=4;
// 	}
// }else if(GC.subtask==4) //�ȴ������ͷ�
// {
// 	if(GC.running_tim>DELAY_MS)
// 	{
// 		if(GC_claw_Sen!=Sen_Block) //�����ͷ�
// 		{
// 			GC.sta=Ready;
// 			GC.subtask=5;
// 		}

// 	}

// }else if(GC.subtask==5) //�ȴ���ת����ת��ֱλ��
// {
// 	if(GC.motor_r->motion==Stop)
// 	{
// 		GC.sta=Finish;
// 		GC.subtask=0;
// 	}

// }
