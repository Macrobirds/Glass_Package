#include "taskthread.h"
#define ADJUST_DIS 2

// ����Ƿ�����赲������
static u8 Check_GC(void)
{
#ifdef DEBUG_MODE
	return TRUE;
#else
	if (GC.motor_r->postion == GC.GCR_hor_pos)
	{
		return FALSE;
	}
	else
	{
		if (GC.motor_r->motion == Stop) // ������ת�����ֹ
		{
			return TRUE;
		}
	}
	return FALSE;
#endif
}

static u8 Check_box(void)
{
	if (GOV_box_Sen != Sen_Block) // δ��⵽�洢��
	{
		Error_Set(Error_Out_Box, 0);
		return FALSE;
	}
	return TRUE;
}

static void Resume_Task(void)
{
	GO.sta = Ready;
	GO.subtask = GO.main_subtask;
	GO.task = GO.main_task;
}
// ��ת��ָ��������ִ�� Task ������ Next_task ��һ������
static void Next_Task(enum glass_out_task_index Resume_task, enum glass_out_task_index Next_task)
{
	GO.main_task = Resume_task; // ָ���ָ�������

	if (Resume_task == GO.task) // �������Ƿ�任
	{
		GO.main_subtask = GO.subtask + 1;
	}
	else
	{
		GO.main_subtask = 0;
	}

	GO.sta = Ready; // ��ת����һ����ִ��
	GO.task = Next_task;
}

void GO_ReadyTask(void)
{
	switch (GO.task)
	{
	case GO_none:
		break;
	////////////////////������λ///////////////
	case GO_reset_on:
		if (GO.subtask == 0) // ���ˮƽԭ�㴫���� ����Ӧ �뿪ԭ�� ����У׼ˮƽλ��
		{
			if (GOH_start_Sen == Sen_Block)
			{
				GO.motor_h->dir = Front;
				motor_Set_Direction(GO.motor_h);
				motorGO_Debug(GO.motor_h, GO.motor_h->pulse_1mm * 10, 0);
				GO.sta = Running;
			}
			else
			{
				GO.subtask = 1;
			}
		}
		else if (GO.subtask == 1) // ��λˮƽԭ��
		{
			if (Check_GC())
			{
				Next_Task(GO.task, GOH_start);
			}
		}
		else if (GO.subtask == 2) /// ��ⴹֱԭ�㴫���� ����Ӧ �뿪ԭ�� ����У׼��ֱλ��
		{
			if (GOV_start_Sen == Sen_Block)
			{
				GO.motor_v->dir = Front;
				motor_Set_Direction(GO.motor_v);
				motorGO_Debug(GO.motor_v, GO.motor_v->pulse_1mm * 5, 0);
				GO.sta = Running;
			}
			else
			{
				GO.subtask = 3;
			}
		}
		else if (GO.subtask == 3) // ��λ��ֱԭ��
		{
			Next_Task(GO_none, GOV_start);
		}
		break;
	//////////////////�ػ���λ/////////////////
	case GO_reset_off:
		if (GO.subtask == 0) // �ص�ˮƽԭ��
		{
			if (GOH_start_Sen == Sen_Block)
			{
				GO.sta = Ready;
				GO.subtask = 1;
			}
			else
			{
				if (Check_GC())
				{
					Next_Task(GO.task, GOH_start);
				}
			}
		}
		else if (GO.subtask == 1)
		{
			if (GC_claw_Sen == Sen_Block) // ���ִ��ڼн�״̬  ��ˮƽ������
			{
				if (GC.motor_r->postion == 0 && GC.motor_r->motion == Stop) // �ȴ����ָ�λ��ԭ��
				{
					motorGo_acc(GO.motor_h, GO.GOH_mid_pos);
				}
			}
			else // �����ͷ� ��������
			{
				GO.sta = Finish;
			}
		}
		else if (GO.subtask == 2) // �ȴ������ͷ� �ص�ˮƽԭ��
		{
			if (Check_GC())
			{
				Next_Task(GO_none, GOH_start);
			}
		}
		break;
	//////////////////�洢���Ƴ�////////////////
	case GO_Box_Out:
		motorGo_acc(GO.motor_v, GO.GOV_box_dis);
		break;
	/////////////////�洢������////////////////
	case GO_Box_In:
		if (GO.subtask == 0) // �洢������
		{
			if (Check_box())
			{
				motorGo_acc(GO.motor_v, 0);
				GO.sta = Running;
			}
		}
		else if (GO.subtask == 1) // ��׼�洢����
		{
			motorGo(GO.motor_v, GO.GOV_box_len, 0);
			GO.sta = Running;
		}
		else if (GO.subtask == 2) // �����洢��λ��
		{
			Next_Task(GO_none, GO_adjust);
		}
		break;
	////////////////��ʼ����///////////////////////
	case GO_start:

		if (Check_box()) // ˮƽ��λ��ԭ��
		{
			if (Check_GC())
			{
				Next_Task(GOH_package, GOH_start);
			}
		}

		break;
	///////////////��λ��ˮƽԭ��///////////////////
	case GOH_start:
		if (GOH_start_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			motorGo_acc(GO.motor_h, 0);
			GO.sta = Running;
		}
		break;
	//////////////�ƶ�����Ƭ������////////////////////
	case GOH_package:
		if (GOH_mid_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GC.main_task == GC_start && GC.subtask == 6) // �ȴ����ּ�ȡ��Ƭ����תԭ��
			{
				GO.sta = Running;
				motorGo_acc(GO.motor_h, GO.GOH_mid_pos); // �ƶ�����Ƭ������
			}
		}
		break;
	////////////�ƶ���ˮƽ�յ� ��Ƭ���õ��洢��////////////////
	case GOH_end:
		if (GOH_end_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			// ��Ƭ�������� �����ͷ�
			if (Check_GC())
			{
				GO.sta = Running;
				motorGo_acc(GO.motor_h, GO.GOH_end_pos);
			}
		}
		break;
	///////////��λ����ֱԭ��λ��/////////////////////
	case GOV_start:
		if (GOV_start_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			GO.sta = Running;
			motorGo_acc(GO.motor_v, 0);
		}
		break;
	///////////����Ƭ�Ƴ�����,����洢��///////////////
	case GO_out:
		if (GOV_glass_Sen == Sen_Block)
		{

			GO.sta = Running;
			motorGo(GO.motor_v, GO.GOV_slot_dis, 0);
		}
		else
		{
			GO.sta = Finish;
		}
		break;
	////////////�ƶ�����һ�洢��///////////////////
	case GO_next:
		if (GOV_glass_Sen == Sen_Block)
		{
			GO.sta = Running;
			motorGo(GO.motor_v, GO.GOV_box_dis, 0);
		}
		else
		{
			GO.sta = Finish;
		}
		break;
	///////////΢���洢��λ���ò�Ƭ��׼���¶�///////////////
	case GO_adjust:
		GO.sta = Running;
		motorGo(GO.motor_v, GO.motor_v->postion + GO.motor_v->pulse_1mm * ADJUST_DIS, 0);
		break;
	case GO_finish:
		break;
	case GO_error:
		break;
	}
}

void GO_RunningTask(void)
{
	switch (GO.task)
	{
	case GO_none:
		break;
	////////////////������λ///////////////////////////
	case GO_reset_on:
		if (GO.subtask == 0) // ���ˮƽԭ�㴫���� ����Ӧ �뿪ԭ�� ����У׼ˮƽλ��
		{
			if (GO.motor_h->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 1;
			}
		}
		else if (GO.subtask == 1) // ��λˮƽԭ��
		{
		}
		else if (GO.subtask == 2) /// ��ⴹֱԭ�㴫���� ����Ӧ �뿪ԭ�� ����У׼��ֱλ��
		{
			if (GO.motor_v->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 3;
			}
		}
		else if (GO.subtask == 3) // ��λ��ֱԭ��
		{
		}
		break;
	////////////////�ػ���λ///////////////////////
	case GO_reset_off:
		if (GO.subtask == 0) // �ص�ˮƽԭ��
		{
		}
		else if (GO.subtask == 1) // ���ִ��ڼн�״̬  ��ˮƽ������
		{

			if (GOH_mid_Sen == Sen_Block)
			{
				GO.sta = Ready;
				GO.subtask = 2;
			}
			else
			{
				if (GO.motor_h->motion == Stop)
				{
					Error_Set(Error_Sensor, GOH_mid_sensor);
				}
			}
		}
		else if (GO.subtask == 2) // �ȴ������ͷ� �ص�ˮƽԭ��
		{
		}
		break;
	///////////////����////////////////////////////
	case GO_Box_Out:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
		}
		break;
	////////////////////����/////////////////////
	case GO_Box_In:
		if (GO.subtask == 0) // �洢������
		{
			if (GO.motor_v->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 1;
			}
		}
		else if (GO.subtask == 1) // ��׼�洢����
		{
			if (GO.motor_v->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 2;
			}
		}
		else if (GO.subtask == 2) // �����洢��λ��
		{

		}
		break;
	///////////////��λ��ˮƽԭ��///////////////////
	case GOH_start:
		if (GOH_start_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_h->motion == Stop)
			{
				Error_Set(Error_Sensor, GOH_start_sensor);
			}
		}
		break;
	//////////////�ƶ�����Ƭ������////////////////////
	case GOH_package:
		if (GOH_mid_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_h->motion == Stop)
			{
				Error_Set(Error_Sensor, GOH_mid_sensor);
			}
		}
		break;
	////////////�ƶ���ˮƽ�յ� ��Ƭ���õ��洢��////////////////
	case GOH_end:
		if (GOH_end_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_h->motion == Stop)
			{
				Error_Set(Error_Sensor, GOH_end_sensor);
			}
		}
		break;
	case GO_out:
		if (GOV_glass_Sen != Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_v->motion == Stop && GO.motor_v->postion <= GO.GOV_box_len) // ����һ���۵ľ���� ��Ȼ�и�Ӧ
			{
				GO.sta = Ready;
				GO.task = GO_next;
			}
			else if (GO.motor_v->postion > GO.GOV_box_len) //�洢���� ��������
			{
				GO.sta = Ready;
				GO.task = GO_finish;
			}
		}
		break;
	case GO_next:
		if (GOV_glass_Sen != Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_v->motion == Stop)
			{
				if (GO.motor_v->postion < GO.GOV_box_len)
				{
					GO.sta=Finish;
				}
				else
				{
					OS_ENTER_CRITICAL();
					GO.box_num++; // �۸�����1
					OS_EXIT_CRITICAL();
					GO.sta = Ready;
					GO.task = GO_finish;
					Error_Set(Error_Out_Box, 0); // ���� �洢����
				}
			}
		}
		break;
	case GO_adjust:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
		}
		break;
	case GO_finish:
		break;
	case GO_error:
		break;
	}
}

void GO_FinishTask(void)
{
	switch (GO.task)
	{
	case GO_none:
		break;
	case GO_reset_on:
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_reset_off:
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_Box_Out:
		break;
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
	case GO_Box_In:

		break;
	case GO_start:

	break;
	case GOH_start:
	Resume_Task();	
		break;
	case GOH_package:
		if (Check_GC()) // �������뿪��Ƭ����
		{
			GO.sta = Ready;
			GO.task = GOH_end;
		}
		break;
	case GOH_end:

			OS_ENTER_CRITICAL();
			GO.glass_num++; // ��Ƭ��Ƭ����+1
			OS_EXIT_CRITICAL();
			GO.sta = Ready;
			GO.task = GO_out;
		break;
	case GO_out:
		Next_Task(GO_start,GO_adjust);
		break;
	case GO_next:
		OS_ENTER_CRITICAL();
		GO.box_num++;
		OS_EXIT_CRITICAL();
		Next_Task(GO_start,GO_adjust);
		break;
	case GO_adjust:
		Resume_Task();
		break;
	case GO_finish:
		break;
	case GO_error:
		break;
	}
}

void GO_TaskThread(void)
{

	if (GO.sta == Ready)
	{
		GO_ReadyTask();
		GO.running_tim = 0;
	}
	if (GO.sta == Running)
	{
		GO_RunningTask();
		GO.running_tim++;
		if (GO.running_tim > OVERTIME)
		{
			Error_Set(Error_OverTime, 0);
		}
	}
	if (GO.sta == Finish)
	{
		if (TaskThread_State != taskthread_debug) // debug ģʽ�²�����������ת
		{
			GO_FinishTask();
		}
	}
}

// if (GO.subtask == 0) // ˮƽ��λ��ԭ��
// {
// 	if (GOH_start_Sen == Sen_Block)
// 	{
// 		GO.sta = Ready;
// 		GO.subtask = 1;
// 	}
// 	else
// 	{
// 		motorGo_acc(GO.motor_h, 0);
// 		GO.sta = Running;
// 	}
// }
// else if (GO.subtask == 1) // �洢�н���
// {
// 	if (GOV_box_Sen == Sen_Block)
// 	{
// 		motorGo_acc(GO.motor_v, 0);
// 		GO.sta = Running;
// 	}
// 	else
// 	{
// 		Error_Set(Error_Out_Box, 0); // ����û�д洢��
// 	}
// }
// else if (GO.subtask == 2) // ��׼�洢��
// {
// 	motorGo(GO.motor_v, GO.motor_v->postion + GO.GOV_box_dis, 0);
// 	GO.sta = Running;
// }
// else if (GO.subtask == 3) // �����洢��λ��
// {
// }