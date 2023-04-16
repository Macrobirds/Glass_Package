#include "taskthread.h"
#define DELAY_MS 100

// �������
static enum glass_claw_task_index main_task = GC_none;
// ���������
static u8 main_subtask = 0;

// ����Ƿ��赲������ת
static u8 Check_GP(void)
{
	if (GC_claw_Sen == Sen_Block) // ���ּн�״̬
	{
		if (GO.motor_h->postion == 0 && GO.motor_h->motion == Stop && GP.motor->postion == GP.sucker_pos && GP.motor->motion == Stop)
		{
			return TRUE;
		}
	}
	else
	{
		if (GO.motor_h->postion == 0 && GO.motor_h->motion == Stop)
		{
			return TRUE;
		}
	}
	return FALSE;
}


static void Resume_Task(void)
{
	GC.sta=Ready;
	GC.subtask=main_subtask;
	GC.task=main_task;
}
// ��ת��ָ��������ִ�� Task ������ Next_task ��һ������
static void Next_Task(enum glass_claw_task_index Resume_Task, enum glass_claw_task_index Next_task)
{
	main_task = Resume_Task; // ָ���ָ�������

	if (Resume_Task != GC.task) // �������Ƿ�任
	{
		main_subtask = GC.subtask + 1;
	}
	else
	{
		main_subtask = 0;
	}

	GC.sta = Ready; // ��ת����һ����ִ��
	GC.task = Next_task;
}

void GC_ReadyTask(void)
{
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
			if (Check_GP())
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
		}
		else if (GC.subtask == 3) // ��λ����תԭ��
		{
			if (GC_claw_Sen == Sen_Block)
			{
				Next_Task(GC_none, GC_rot_start);
			}
		}

		break;
	////////////////���ֹػ���λ//////////////////
	case GC_reset_off:
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
					Next_Task(GC.task, GC_rot_hor);
				}
			}
			else
			{
				GC.sta = Finish;
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
	
	///////////////////��ֱ����λ��ԭ��/////////////////////basic
	case GC_ver_start:
		if (GC_ver_Sen == Sen_Block) // ��ֱԭ���Ӧ
		{
			stepperMotorStop(GC.motor_v);
			GC.motor_v->postion = 0;
			GC.sta = Finish;
		}
		else
		{
			motorGo_acc(GC.motor_v, 0); // ��ֱ����λ��ԭ��
			GC.sta = Running;
		}
		break;
	///////////////////��ת����λ��ԭ��///////////////////// basic
	case GC_rot_start:
		if (GC_rot_Sen != Sen_Block)
		{
			stepperMotorStop(GC.motor_r);
			GC.motor_r->postion = 0;
			GC.sta = Finish;
		}
		else
		{
			motorGo_acc(GC.motor_r, 0);
			GC.sta = Running;
		}

		break;
	//////////////////������ת����ֱλ��////////////////// basic
	case GC_rot_down:
		motorGo_acc(GC.motor_r, GC.GCR_ver_pos); // ������ת����ֱ
		GC.sta = Running;
		break;
	////////////////�����½�����Ƭװ�ز�//////////////// basic
	case GC_move_down:
		if ((GE_down_Sen == Sen_Block) && (GE_up_Sen != Sen_Block)) // ��Ƭ�ƶ����������·�
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
	////////////////������ת��ˮƽλ��///////////////////// basic
	case GC_rot_hor:
		if ((GOH_mid_Sen == Sen_Block) && (GO.motor_h->motion == Stop)) // ��Ƭ���̴��ڷ�Ƭ�����㾲ֹ
		{
			if (GP.task <= GP_sucker_up) // ��Ƭ����δ��ʼ
			{
				motorGo(GC.motor_r, GC.GCR_hor_pos, 0);
				GC.sta = Running;
			}
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
			if(GC.motor_r->motion==Stop) 
			{
				GC.sta=Ready;
				GC.subtask=3;
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

		}
		else if (GC.subtask == 3) // �����ͷ�
		{

		}
		else if (GC.subtask == 4) // ��λ����תԭ��
		{

		}
		break;
	case GC_ver_start:
		////////////���ָ�λ����ֱԭ��////////////////
		if (GC_ver_Sen == Sen_Block)
		{
			GC.sta = Finish;
		}
		else
		{
			if (GC.motor_v->motion == Stop) // ��ֱ���������� ��������
			{
				Error_Set(Error_Sensor, GC_ver_sensor);
			}
		}
		break;
	///////////���ָ�λ����תԭ��////////////////
	case GC_rot_start:
		if(GC_rot_Sen!=Sen_Block)
		{
			GC.sta=Finish;
		}else
		{
			if(GC.motor_r->motion==Stop)
			{
				Error_Set(Error_Sensor,GC_rot_sensor);
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
	//////////////////�ȴ����ּн�///////////////////
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
	case GC_rot_hor:
		if (GC.motor_r == Stop)
		{
			GC.sta = Finish;
		}
		break;
	////////////////////�����ͷ���ʱ��ʱ///////////////////////
	case GC_release:
		if (GC.running_tim > DELAY_MS)
		{
			if(GC_claw_Sen!=Sen_Block)
			{
				GC.sta = Finish;
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
		GC.sta = Ready;
		GC.task = GC_none;
		break;
	case GC_reset_off:
		GC.sta = Ready;
		GC.task = GC_none;
		break;
	case GC_ver_start:
		Resume_Task();
		break;
	case GC_rot_start:
		Resume_Task();
		break;
	case GC_rot_down:
		Resume_Task();
		break;
	case GC_move_down:
		Resume_Task();
		break;
	case GC_grab:
		Resume_Task();
		break;
	// case GC_move_up:
	// 	if ((GO.task == GO_start) && (GO.sta == Finish)) // �ȴ���Ƭ���̻ص�ԭ��
	// 	{
	// 		GC.sta = Ready;
	// 		GC.task = GC_rot_up;
	// 	}
	// 	break;
	// case GC_rot_up:
	// 	if ((GO.task == GO_package) && (GO.sta == Finish)) // �ȴ���Ƭ���̵���Ƭ������
	// 	{
	// 		GC.sta = Ready;
	// 		GC.task = GC_rot_hor;
	// 	}
	// 	break;
	case GC_rot_hor:
		Resume_Task();
		// if (GP.task <= GP_sucker_up) // �ȴ���Ƭ������� ��ʼ��һ����
		// {
		// 	GC.sta = Ready;
		// 	GC.task = GC_release;
		// }
		break;
	case GC_release:
		Resume_Task();
		// if (GC_claw_Sen != Sen_Block)
		// {
		// 	GC.sta = Ready;
		// 	GC.task = GC_rot_down;
		// }
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
