#include "taskthread.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define DELAY_SUCKER 500
#define DELAY_BIG_CYLIDER 1500
#define DELAY_SMALL_CYLIDER 1500

static u8 Check_Spray(void)
{
	if (GP_spray_Sen != Sen_Block) // δ�����罺ͷ ����
	{
		Error_Set(Error_Spray, 0);
		return FALSE;
	}
	return TRUE;
}

static void Resume_Task(void)
{
	GP.sta = Ready;
	GP.subtask = GP.main_subtask;
	GP.task = GP.main_task;
}
// ��ת��ָ��������ִ�� Task ������ Next_task ��һ������
static void Next_Task(enum glass_package_task_index Resume_task, enum glass_package_task_index Next_task)
{
	GP.main_task = Resume_task; // ָ���ָ�������

	if (Resume_task == GP.task) // �������Ƿ�任
	{
		GP.main_subtask = GP.subtask + 1;
	}
	else
	{
		GP.main_subtask = 0;
	}

	GP.sta = Ready; // ��ת����һ����ִ��
	GP.task = Next_task;
}

void GP_ReadyTask(void)
{
	switch (GP.task)
	{
	case GP_none:
		break;
	/////////////������λ///////////////
	case GP_reset_on:
		if (GP.subtask == 0) // ��С���׸�λ
		{
			Next_Task(GP.task, GP_cyl_start);
		}
		else if (GP.subtask == 1)
		{
			if (GP_start_Sen == Sen_Block) // ��ԭ�㴫������Ӧ �뿪ԭ�� ����У׼λ��
			{
				GP.motor->dir = Front;
				motor_Set_Direction(GP.motor);
				motorGO_Debug(GP.motor, GP.motor->pulse_1mm * 10, 0);
				GP.sta = Running;
			}
			else
			{
				GP.subtask = 2;
			}
		}
		else if (GP.subtask == 2) // ��λ��ԭ��
		{
			Next_Task(GP_none, GP_move_start);
		}

		break;
	/////////////�ػ���λ//////////////
	case GP_reset_off:
		if (GP.subtask == 0) // �ر�����
		{
			GP_sucker_Pump = GAS_DISABLE;
			GP_sucker1_Cyl = GAS_DISABLE;
			GP_sucker2_Cyl = GAS_DISABLE;
			GP.sta=Running;
		}
		else if (GP.subtask == 1) // ���׸�λ
		{
			Next_Task(GP.task, GP_cyl_start);
		}
		else if (GP.subtask == 2) // ԭ�㸴λ
		{
			if(GC_rot_Sen!=Sen_Block&&GOH_start_Sen==Sen_Block&&GC_claw_Sen!=Sen_Block) //�����ͷŻص���תԭ�㲢�����̻ص�ԭ��
			{
				Next_Task(GP_none, GP_move_start);
			}else //�뿪ԭ��λ �ȴ������ͷŸ�λ
			{
				motorGo_acc(GP.motor,GP.sucker_pos);
				GP.sta=Running;
			}
			
		}else if(GP.subtask==3)
		{
			if(GC_claw_Sen!=Sen_Block)
			{
				Next_Task(GP_none, GP_move_start);
			}
		}

		break;
	/////////��ʼ����/////////////////
	case GP_start:
		if (Check_Spray())
		{
			if (GP.subtask == 0)
			{
				Next_Task(GP.task, GP_cyl_start); // ���׸�λ
			}
			else if (GP.subtask == 1)
			{
				Next_Task(GP_move_glass, GP_move_start); // ԭ�㸴λ
			}
		}
		break;
	////////////��С���׸�λ/////////////// basic
	case GP_cyl_start:
		if(GP_big_cyl_Sen==Sen_Block&&GP_small_cyl_Sen==Sen_Block)
		{
			GP.sta=Finish;
		}else
		{
				GP_small_Cyl = GAS_DISABLE; // С����ʧ��
				GP_big_Cyl = GAS_DISABLE;	// ������ʧ��
				GP.sta = Running;
		}

		break;
	//////////��λ��ԭ��λ��/////////////// basic
	case GP_move_start:
		if (GP_start_Sen == Sen_Block)
		{
			GP.sta = Finish;
		}
		else
		{
			GP.sta = Running;
			motorGo_acc(GP.motor, 0);
		}
		break;
	///////////���ﲣƬ��ȡλ��////////////
	case GP_move_glass:
			GP.sta = Running;
			motorGo_acc(GP.motor, GP.sucker_pos);
		break;
	//////////��Ƭ�����½�/////////////
	case GP_sucker_down:
		if (GP.subtask == 0)
		{
			GP_small_Cyl = GAS_ENABLE;
			GP.sta = Running;
		}
		else if (GP.subtask == 1)
		{
			GP_big_Cyl = GAS_ENABLE;
			GP.sta = Running;
		}
		break;
	/////////��ȡ�ǲ�Ƭ///////////////
	case GP_suck_glass:
		if (GP_sucker_Sen == Sen_Block)
		{
			GP_sucker_Pump = GAS_ENABLE; // ������������
			GP_sucker1_Cyl = GAS_ENABLE; // ��������1 ����2
			GP_sucker2_Cyl = GAS_ENABLE;
			GP.sta = Running;
		}
		else
		{
			/// ȱ�ٸǲ�Ƭ
			Error_Set(Error_Cover_Glass, 0);
		}
		break;
	///////////��Ƭ��������///////////
	case GP_sucker_up:
		if ((GC.motor_r->postion == GC.GCR_hor_pos) && (GC.motor_r->motion == Stop)) // ����λ����תˮƽλ�þ�ֹ
		{
				GP_big_Cyl = GAS_DISABLE;
				GP.sta = Running;
		}

		break;
	/////////////�ƶ����罺λ��/////////
	case GP_move_spray:
			GP.sta = Running;
			motorGo_acc(GP.motor, GP.spray_pos);
		break;
		/////////////�ƶ�����Ƭλ��///////// ��Ƭλ�ú�ԭ��λ���غ�
	case GP_move_package:
		if (GP_start_Sen == Sen_Block)
		{
			GP.sta = Finish;
		}
		else
		{
			GP.sta = Running;
			GP_spray_Cyl = GAS_ENABLE;			  // ���罺��
			motorGo(GP.motor, 0, GP.spray_speed); // ���罺�ٶ������˶�����Ƭ��
		}
		break;
	////////////////��Ƭ����/////////////////////
	case GP_package:
		if (GP.subtask == 0)
		{
			GP_big_Cyl = GAS_ENABLE; // �������½�
			GP.sta=Running;
		}
		else if (GP.subtask == 1)
		{
			GP_small_Cyl = GAS_DISABLE; // С��������
			GP.sta=Running;
		}
		
		else if (GP.subtask == 2)
		{
			GP_sucker1_Cyl = GAS_DISABLE; // �ر�����1����
			GP.sta=Running;
		}
		
		else if (GP.subtask == 3)
		{
			GP_sucker_Pump = GAS_DISABLE; //�ر�����2���� ��������
			GP_sucker1_Cyl = GAS_DISABLE;
			GP_sucker2_Cyl = GAS_DISABLE;
			GP.sta=Running;
		}
		break;
	case GP_finish:
		break;
	case GP_error:
		break;
	}
}

void GP_RunningTask(void)
{
	switch (GP.task)
	{
	case GP_none:
		break;
	case GP_reset_on:
		if (GP.subtask == 0) // ��С���׸�λ
		{

		}
		else if (GP.subtask == 1)
		{
			if(GP.motor->motion==Stop)// ��ԭ�㴫������Ӧ �뿪ԭ�� ����У׼λ��
			{
				GP.subtask=2;
				GP.sta=Ready;
			}
		}
		else if (GP.subtask == 2) // ��λ��ԭ��
		{
			
		}
		break;
	case GP_reset_off:
		if (GP.subtask == 0) // �ر�����
		{
			if(GP.running_tim>DELAY_SUCKER)
			{
				GP.subtask=1;
				GP.sta=Ready;
			}
		}
		else if (GP.subtask == 1) // ���׸�λ
		{

		}
		else if (GP.subtask == 2) // ԭ�㸴λ
		{
			
			if(GP.motor->motion==Stop)
			{
				GP.subtask=3;
				GP.sta=Ready;
			}
			
		}else if(GP.subtask==3)
		{
			
		}
		break;

	////////////////��С���׸�λ///////////////// basic
	case GP_start:

		break;
	case GP_cyl_start:
		if (GP.running_tim > MAX(DELAY_BIG_CYLIDER, DELAY_SMALL_CYLIDER))
		{
			if ((GP_big_cyl_Sen == Sen_Block) && (GP_small_cyl_Sen == Sen_Block))
			{
				GP.sta = Finish;
			}
			else
			{
				Error_Set(Error_Sensor, GP_big_cyl_sensor | GP_small_cyl_sensor);
			}
		}
		break;
	//////////��λ��ԭ��λ��/////////////// basic
	case GP_move_start:
		if (GP_start_Sen == Sen_Block)
		{
			GP.sta = Finish;
		}
		else
		{
			if(GP.motor->motion==Stop)
			{
				Error_Set(Error_Sensor, GP_start_sensor);
			}

		}
		break;
	case GP_move_glass:
		if (GP.motor->motion == Stop)
		{
			GP.sta = Finish;
		}
		break;
	//////////��Ƭ�����½�/////////////
	case GP_sucker_down:
		if (GP.subtask == 0)
		{
			if (GP.running_tim > DELAY_SMALL_CYLIDER) // �ȴ�С�������½�
			{
				if (GP_small_cyl_Sen != Sen_Block)
				{
					GP.subtask = 1;
					GP.sta = Ready;
				}
			}
		}
		else if (GP.subtask == 1)
		{
			if (GP.running_tim > DELAY_BIG_CYLIDER) // �ȴ��������½�
			{
				if (GP_big_cyl_Sen != Sen_Block)
				{
					GP.subtask = 0;
					GP.sta = Finish;
				}
			}
		}
		break;
	/////////��ȡ�ǲ�Ƭ///////////////
	case GP_suck_glass:
		if (GP.running_tim > DELAY_SUCKER)
		{
			if(GP_sucker_Sen==Sen_Block)
			{
				GP.sta = Finish;
			}else
			{
				Error_Set(Error_Sensor,GP_sucker_sensor);
			}

		}
		break;
	///////////��Ƭ��������///////////
	case GP_sucker_up:
		if (GP.running_tim > DELAY_BIG_CYLIDER)
		{
			if (GP_big_cyl_Sen == Sen_Block)
			{
				GP.sta = Finish;
			}else
			{
				Error_Set(Error_Sensor,GP_big_cyl_sensor);
			}
		}
		break;
	/////////////�ƶ����罺λ��/////////
	case GP_move_spray:
		if (GP.motor->motion == Stop)
		{
			GP.sta = Finish;
		}
		break;
	/////////////�ƶ�����Ƭλ��///////// ��Ƭλ�ú�ԭ��λ���غ�
	case GP_move_package:
		if (GP.motor->postion < (GP.spray_pos - GP.spray_len)) // �����罺���� �ر��罺��
		{
			GP_spray_Cyl = GAS_DISABLE;
		}

		if (GP_start_Sen == Sen_Block) // �ﵽ��Ƭ����ԭ��
		{
			GP_spray_Cyl = GAS_DISABLE;
			GP.sta = Finish;
		}
		else
		{
			if (GP.motor->motion == Stop)
			{
				GP_spray_Cyl = GAS_DISABLE;
				Error_Set(Error_Sensor, GP_start_sensor); // ����
			}
		}
		break;
	////////////////��Ƭ����/////////////////////
	case GP_package:
		if (GP.subtask == 0)
		{
			if (GP.running_tim > GP.delay_before) // �ȴ��������½� �ǲ�Ƭ���ز�Ƭ�Ӵ�
			{
				GP.sta = Ready;
				GP.subtask = 1;
			}
		}
		else if (GP.subtask == 1) // �ȴ�С�������� �ǲ�Ƭ���ز�Ƭˮƽ
		{
			if (GP.running_tim > GP.delay_after)
			{
				GP.sta = Ready;
				GP.subtask = 2;
			}
		}
		else if (GP.subtask == 2) // �ȴ�����1�ر�
		{
			if (GP.running_tim > DELAY_SUCKER)
			{
				GP.sta = Ready;
				GP.subtask = 3;
			}
		}
		else if (GP.subtask == 3) // �ȴ�����2 �������ùر�
		{
			if (GP.running_tim > DELAY_SUCKER)
			{
				GP.subtask = 0;
				GP.sta = Finish;
			}
		}
		break;
	case GP_finish:
		break;
	case GP_error:
		break;
	}
}

void GP_FinishTask(void)
{
	switch (GP.task)
	{
	case GP_none:
		break;
	case GP_reset_on:
		
		GP.subtask=0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_reset_off:
		GP.subtask=0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_start:
		GP.subtask=0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_cyl_start:
		Resume_Task();
		break;
	case GP_move_start:
		Resume_Task();
		break;
	case GP_move_glass:
		GP.sta = Ready;
		GP.task = GP_sucker_down;
		GP.subtask = 0;
		break;
	case GP_sucker_down:
		if (GP_sucker_Sen == Sen_Block) // ��⵽��Ƭ
		{
			GP.sta = Ready;
			GP.task = GP_suck_glass;
			GP.subtask=0;
		}
		else
		{
			Error_Set(Error_Cover_Glass, 0); // ���� û�иǲ�Ƭ
		}
		break;
	case GP_suck_glass:
		if (GP_sucker_Sen == Sen_Block) // ��⵽��Ƭ
		{
			if ((GC.task == GC_rot_hor) && (GC.sta == Finish)) // �ȴ����ֽ���Ƭ���õ���������
			{
						GP.sta = Ready;
						GP.task = GP_sucker_up;
						GP.subtask=0;
			}
		}
		else
		{
			if(GC.task==GC_finish)
			{
				GP.task=GP_finish;
				GP.sta=Ready;
				GP.subtask=0;
			}else
			{
				Error_Set(Error_Cover_Glass, 0); // ���� û�иǲ�Ƭ
			}

		}
		break;
	case GP_sucker_up:
			GP.sta = Ready;
			GP.task = GP_move_spray;
			GP.subtask=0;
		break;
	case GP_move_spray:
		GP.sta = Ready;
		GP.task = GP_move_package;
		GP.subtask=0;
		break;
	case GP_move_package:
		GP.sta = Ready;
		GP.task = GP_package;
		GP.subtask = 0;
		break;
	case GP_package:
		if (GP_big_cyl_Sen != Sen_Block && GP_small_cyl_Sen == Sen_Block) // �ȴ���С���׵�λ
		{
			GP.subtask = 0;
			GP.sta = Ready;
			GP.task = GP_start;
		}
		break;
	case GP_finish:
		break;
	case GP_error:
		break;
	}
}

void GP_TaskThread(void)
{
	if (GP.sta == Ready)
	{
		GP_ReadyTask();
		GP.running_tim = 0;
	}
	if (GP.sta == Running)
	{
		GP_RunningTask();
		GP.running_tim++;
		if (GP.running_tim > OVERTIME)
		{
			Error_Set(Error_OverTime, 0);
		}
	}
	if (GP.sta == Finish)
	{
		if (TaskThread_State != taskthread_debug) // debug ģʽ�²�����������ת
		{
			GP_FinishTask();
		}
	}
}