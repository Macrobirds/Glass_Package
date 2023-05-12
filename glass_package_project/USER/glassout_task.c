#include "Globalconfig.h"
#define ADJUST_DIS 700 // pulse value
#define ADJUST_DIS_full 200

#define SLOT_NUM 30

static u8 slot_num = 0;

// ����Ƿ�����赲������
static u8 Check_GC(void)
{
	if (GO.task <= GO_reset_on)
	{
		if (GC_claw_Sen != Sen_Block)
		{
			return TRUE;
		}

		if (GC.motor_r->postion != GC.GCR_hor_pos && GC.motor_r->motion == Stop)
		{
			return TRUE;
		}
	}
	else
	{
		if (GC.motor_r->postion != GC.GCR_hor_pos && GC.motor_r->motion == Stop)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ����Ƿ���ڴ洢��
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

	if (slot_num <= 0)
	{
		GO.Storage_full = TRUE;
	}
//		if(GO.task!=GO_none)
//		{
//			printf("GO task:%d subtask:%d\r\n",GO.task,GO.subtask);
//		}
	switch (GO.task)
	{
	case GO_none:
		break;
	///////////��λ����ֱԭ��λ��///////////////////// basic
	case GOV_start:
		if (GOV_start_Sen == Sen_Block)
		{
			stepperMotorStop(GO.motor_v);
			GO.sta = Finish;
			GO.motor_v->postion = 0;
		}
		else
		{
			GO.sta = Running;
			motorGo(GO.motor_v, 0, 0);
			//	motorGo_acc(GO.motor_v, 0);
		}
		break;
	///////////////��λ��ˮƽԭ��/////////////////// basic
	case GOH_start:
		if (GOH_start_Sen == Sen_Block)
		{
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
			GO.motor_h->postion = 0;
		}
		else
		{
			if (Check_GC())
			{
				motorGo(GO.motor_h, 0, 268 * 80);
				GO.sta = Running;
			}
		}
		break;
	///////////΢���洢��λ���ò�Ƭ��׼���¶�/////////////// basic
	case GO_adjust:
		GO.sta = Running;
		GO.motor_v->dir = Front;
		motor_Set_Direction(GO.motor_v);
		if (GOV_start_Sen == Sen_Block)
		{
			printf("dual box\r\n");
			motorGO_Debug(GO.motor_v, ADJUST_DIS - ADJUST_DIS_full, 0);
		}
		else
		{
			printf("singal box\r\n");
			motorGO_Debug(GO.motor_v, ADJUST_DIS, 0);
		}
		break;
	////////////////////������λ///////////////
	case GO_reset_on:
		GO.Storage_full = TRUE;
		slot_num=0;		
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
				motorGO_Debug(GO.motor_v, GO.motor_v->pulse_1mm * 10, 800 * 10);
				GO.sta = Running;
			}
			else
			{
				GO.subtask = 3;
			}
		}
		else if (GO.subtask == 3) // ��λ��ֱԭ��
		{
			Next_Task(GO.task, GOV_start);
		}
		else if (GO.subtask == 4)
		{
			Next_Task(GO_none, GO_adjust);
		}
		break;
	//////////////////�ػ���λ/////////////////
	case GO_reset_off:
		if (GO.subtask == 0) // �ص�ˮƽԭ��
		{
				if (Check_GC())
				{		
						if(GOH_start_Sen==Sen_Block)
						{
							GO.subtask++;
						}else
						{
							if(GC.motor_r->postion==GC.GCR_ver_pos&&GC.motor_r->motion==Stop)
							{
								Next_Task(GO_none, GOH_start);
							}

						}

				}
		}
		else if (GO.subtask == 1)
		{
			if (GC_claw_Sen == Sen_Block) // ���ִ��ڼн�״̬  ��ˮƽ������
			{
				if (GC_rot_Sen!=Sen_Block&& GC.motor_r->motion == Stop) // �ȴ����ָ�λ��ԭ��
				{
					motorGo_acc(GO.motor_h, GO.GOH_mid_pos);
					GO.sta = Running;
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
				if(GC.motor_r->postion==GC.GCR_ver_pos&&GC.motor_r->motion==Stop)
				{
						Next_Task(GO_none, GOH_start);
				}

			}
		}
		break;
	//////////////////�洢���Ƴ�////////////////
	case GO_Box_Out:
		GO.Storage_full = TRUE;
		slot_num=0;	
		motorGo_acc(GO.motor_v, GO.GOV_box_len);
		GO.sta = Running;
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
			if (GOV_start_Sen == Sen_Block)
			{
				GO.subtask = 2;
			}
			else
			{
				GO.motor_v->dir = Front;
				motor_Set_Direction(GO.motor_v);
				motorGO_Debug(GO.motor_v, GO.GOV_box_len, 0);
				GO.sta = Running;
			}
		}
		else if (GO.subtask == 2) // �����洢��λ��
		{
			// �洢��״̬����
			if (GOV_start_Sen == Sen_Block)
			{
				slot_num = SLOT_NUM * 2;
			}
			else
			{
				slot_num = SLOT_NUM;
			}
			GO.Storage_full = FALSE;
			Next_Task(GO_none, GO_adjust);
		}
		break;
	////////////////��ʼ����///////////////////////
	case GO_start: // ˮƽ��λ��ԭ��
		if (GOH_start_Sen == Sen_Block)
		{
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
			GO.motor_h->postion = 0;
		}
		else
		{
			if (Check_GC())
			{
				motorGo(GO.motor_h, 0, 268 * 80);
				GO.sta = Running;
			}
		}
		break;
	//////////////�ƶ�����Ƭ������////////////////////
	case GOH_package:
		if (GO.motor_h->postion == GO.GOH_mid_pos)
		{
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
			GO.motor_h->postion = GO.GOH_mid_pos;
		}
		else
		{
			if (GC_rot_Sen != Sen_Block && GC_claw_Sen == Sen_Block) // �ȴ����ּ�ȡ��Ƭ����תԭ��
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
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
			GO.motor_h->postion = GO.GOH_end_pos;
		}
		else
		{
			if (GO.Storage_full == TRUE) // �洢������
			{
				Error_Set(Error_StorageFull, 0); // ���� �洢����
			}
			else
			{
				if (Check_box() && GOV_glass_Sen != Sen_Block) // ����Ƿ��д洢�� �Ҵ洢�ۿ�
				{
					if (Check_GC()) // ��Ƭ�������� �����ͷ�
					{
						GO.sta = Running;
						motorGo_acc(GO.motor_h, GO.GOH_end_pos);
					}
				}
				else
				{
					Error_Set(Error_Out_Box|Error_Sensor, GOV_glass_sensor);
				}
			}
		}
		break;

	///////////����Ƭ�Ƴ�����,����洢��///////////////
	case GO_out:
		GO.motor_v->dir = Front;
		motor_Set_Direction(GO.motor_v);
		motorGO_Debug(GO.motor_v, GO.GOV_slot_dis, 0);
		GO.sta = Running;
		break;
	////////////�ƶ�����һ�洢��///////////////////
	case GO_next:
		if (GOV_glass_Sen == Sen_Block)
		{
			GO.motor_v->dir = Front;
			motor_Set_Direction(GO.motor_v);
			motorGO_Debug(GO.motor_v, GO.GOV_box_dis, 0);
			GO.sta = Running;
		}
		else
		{
			GO.sta = Finish;
		}
		break;
	case GO_finish:
		break;
	case GO_error:
		if(error_type&Error_Out_Box)
		{
			GO.sta=Finish;
		}else if(error_type&Error_StorageFull)
		{
			GO.sta=Finish;
		}
		break;
	}
}

void GO_RunningTask(void)
{
	switch (GO.task)
	{
	case GO_none:
		break;
	///////////��λ����ֱԭ��λ��///////////////////// basic
	case GOV_start:
		if (GOV_start_Sen == Sen_Block)
		{
			GO.sta = Finish;
		}
		else
		{
			if (GO.motor_v->motion == Stop)
			{
				Error_Set(Error_Sensor, GOV_start_sensor);
			}
		}
		break;
	///////////////��λ��ˮƽԭ��/////////////////// basic
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
	///////////΢���洢��λ���ò�Ƭ��׼���¶�/////////////// basic	
	case GO_adjust:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
		}
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

			if (GO.motor_h->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask = 2;
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
	////////////��ʼ����//////////////////
	case GO_start:
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
		if (GO.motor_h->motion == Stop)
		{
			GO.sta = Finish;
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

		if (GO.motor_v->motion == Stop) // ����һ���۵ľ���� ��Ȼ�и�Ӧ
		{
			printf("out box pos:%d\r\n", GO.motor_v->postion);
			if (GOV_glass_Sen != Sen_Block)
			{
				GO.sta = Finish;
			}
			else
			{
				GO.sta = Ready; // ������һ���ۻ��߲���
				GO.task = GO_next;
			}
		}

		break;
	case GO_next:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
			printf("next box pos:%d\r\n", GO.motor_v->postion);
			//				if (GOV_glass_Sen!=Sen_Block)
			//				{
			//					GO.sta=Finish;
			//				}else //����
			//				{
			//					GO.sta=Ready;
			//					GO.task=GO_finish;
			//				}
		}
		break;

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
	case GOV_start:
		Resume_Task();
		break;
	case GOH_start:
		Resume_Task();
		break;
	case GO_adjust:
		printf("adj box pos:%d\r\n", GO.motor_v->postion);
		Resume_Task();
		break;
	case GO_reset_on:
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_reset_off:
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_Box_Out: 
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_Box_In:
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		GO.sta = Ready;
		GO.task = GO_none;
		GO.subtask = 0;
		break;
	case GO_start:
		if (GC.task == GC_rot_up && GC.sta == Finish)
		{
			GO.sta = Ready;
			GO.task = GOH_package;
		}else
		{
			if(GC.task==GC_finish)
			{
				GO.task=GO_finish;
				GO.sta=Ready;
				//������������ͣ��������ʱ���������������Ϣ
				if(TaskThread_State!=taskthread_pause)
				{
					//�������������Ϣ
				}
			}

		}
		break;
	case GOH_package:
		#ifdef BIG_CYLINDER
		if (GC.motor_r->postion == GC.GCR_ver_pos && GP_big_cyl_Sen == Sen_Block) // �����ִ�ֱ �Ҵ����׸�λ
		{
			GO.sta = Ready;
			GO.task = GOH_end;
		}
		#endif
		#ifdef BIG_CYLINDER_MOTOR
		if (GC.motor_r->postion == GC.GCR_ver_pos && GP_big_cyl_Sen == Sen_Block) // �����ִ�ֱ �Ҵ����׸�λ
		{
			GO.sta = Ready;
			GO.task = GOH_end;
		}
		#endif
		break;
	case GOH_end:
		OS_ENTER_CRITICAL();
		GO.glass_num++; // ��Ƭ��Ƭ����+1
		slot_num--;
		OS_EXIT_CRITICAL();
		GO.sta = Ready;
		GO.task = GO_out;
		break;
	case GO_out:
		Next_Task(GO_start, GO_adjust);
		break;
	case GO_next:
		OS_ENTER_CRITICAL();
		GO.box_num++;
		OS_EXIT_CRITICAL();
		Next_Task(GO_start, GO_adjust);
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

