#include "Globalconfig.h"
#define ADJUST_DIS 700 // pulse value
#define ADJUST_DIS_full 200

#define SLOT_NUM 30
#define WASTE_POS 268 * 36
// #define GLASS_OUT_DIS 2300
#define GLASS_OUT_DIS GP.delay_before

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
//		if(GO.task!=GO_none)
//		{
//			printf("GO task:%d subtask:%d\r\n",GO.task,GO.subtask);
//		}

void GO_ReadyTask(void)
{

	if (slot_num <= 0)
	{
		GO.Storage_full = TRUE;
	}

	switch (GO.task)
	{
	case GO_none:
		break;
	/*��λ����ֱԭ��λ�� baisc*/
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
		}
		break;
	/*��λ��ˮƽԭ�� basic*/
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
				motorGo(GO.motor_h, 0, 0);
				GO.sta = Running;
			}
		}
		break;
	/*΢���洢��λ���ò�Ƭ��׼�� basic*/
	case GO_adjust:
		GO.sta = Running;
		GO.motor_v->dir = Front;
		motor_Set_Direction(GO.motor_v);
		if (GOV_start_Sen == Sen_Block)
		{
			motorGO_Debug(GO.motor_v, GO.GOV_adjust_start, 0);
		}
		else
		{
			motorGO_Debug(GO.motor_v, GO.GOV_adjust, 0);
		}
		break;
	/*������λ*/
	case GO_reset_on:
		GO.Storage_full = TRUE;
		slot_num = 0;
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
		else if (GO.subtask == 4) //�˳��洢��
		{
			//Next_Task(GO_none, GO_adjust);
			GO.Storage_full = TRUE;
			slot_num = 0;
			motorGo_acc(GO.motor_v, GO.GOV_box_len);
			GO.sta = Running;

		}
		break;
	/*�ػ���λ*/
	case GO_reset_off:
#ifdef GC_ROT_PACKAGE
		if (GO.subtask == 0) // �ص�ˮƽԭ��
		{
			if (Check_GC())
			{
				if (GOH_start_Sen == Sen_Block)
				{
					GO.subtask++;
				}
				else
				{
					if (GC.motor_r->postion == GC.GCR_ver_pos && GC.motor_r->motion == Stop)
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
				if (GC_rot_Sen != Sen_Block && GC.motor_r->motion == Stop) // �ȴ����ָ�λ��ԭ��
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
				if (GC.motor_r->postion == GC.GCR_ver_pos && GC.motor_r->motion == Stop)
				{
					Next_Task(GO_none, GOH_start);
				}
			}
		}
#endif
#ifdef GC_VER_PACKAGE
		if (GO.subtask == 0)
		{
			if (GC_claw_Sen == Sen_Block) // ���ּн�
			{
				if (GC.motor_r->postion == GC.GCR_hor_pos && GC.motor_r->motion == Stop)
				{
					GO.subtask = 1;
				}
				else
				{
					if (GO.motor_h->motion == Stop && GO.motor_v->motion == Stop)
					{
						Next_Task(GO.task, GOH_start);
					}
				}
			}
			else
			{
				GO.subtask = 2;
			}
		}
		else if (GO.subtask == 1)
		{
			if (GC_claw_Sen == Sen_Block)
			{
				if (GC.motor_r->postion == GC.GCR_hor_pos && GC_ver_Sen == Sen_Block) // ����λ����תˮƽλ�� ���ڴ�ֱԭ��
				{
					motorGo_acc(GO.motor_h, GO.GOH_mid_pos); // �ƶ�����Ƭλ��
					GO.sta = Running;
				}
			}
			else
			{
				GO.subtask = 2;
			}
		}
		else if (GO.subtask == 2) // ��λ��ˮƽԭ��
		{
			if (GC_rot_Sen != Sen_Block) // ���ֻص�ԭ��
			{
				Next_Task(GO.task, GOH_start);
			}
			if (GOH_start_Sen == Sen_Block)
			{
				GO.sta = Finish;
			}
		}
		else if (GO.subtask == 3)
		{
			GO.sta = Finish;
		}
#endif
		break;
	/*�洢���Ƴ�*/
	case GO_Box_Out:
		GO.Storage_full = TRUE;
		slot_num = 0;
		motorGo_acc(GO.motor_v, GO.GOV_box_len);
		GO.sta = Running;
		break;
	/*�洢������*/
	case GO_Box_In:
		if (GO.subtask == 0) // �洢������
		{
			if (GOV_box_Sen == Sen_Block)
			{
				motorGo(GO.motor_v, 0, 0);
				GO.sta = Running;
			}
			else
			{
				GO.sta = Finish;
				GO.task = GO_Box_Out; // ����ȱ�ٴ洢�еĴ�����Ϣ
				error_type |= Error_Out_Box;
				Error_Mesg_Send();
				error_type &= (~Error_Out_Box);
			}
		}
		else if (GO.subtask == 1) // ��׼�洢����
		{
			if (GOV_start_Sen == Sen_Block)
			{
				GO.subtask = 2;
			}
			else
			{ /*�����ƶ���׼�洢���϶�*/
				GO.motor_v->dir = Front;
				motor_Set_Direction(GO.motor_v);
				motorGo(GO.motor_v, GO.GOV_box_len, 0);
				GO.sta = Running;
			}
		}
		else if (GO.subtask == 2) // �����洢��λ��
		{
			if (GOV_box_Sen == Sen_Block)
			{
				// �洢�۸���+1
				ack_task(screenUart_lastRecvIndex++, Type_set, Fc_data, Extra_data_box, 0);
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
			else
			{
				GO.task = GO_none;
				GO.sta = Ready;
				Error_Set(Error_Out_Box, 0);
			}
		}
		break;
	/*��ʼ����*/
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
#ifdef GC_ROT_PACKAGE
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
														 // motorGo(GO.motor_h,GO.GOH_mid_pos,0);
			}
		}
		break;
#endif
#ifdef GC_VER_PACKAGE
	/*�ƶ�����Ƭ������*/
	case GOH_package:
		if (GO.motor_h->postion == GO.GOH_mid_pos)
		{
			stepperMotorStop(GO.motor_h);
			GO.sta = Finish;
		}
		else
		{
			if (GC_ver_Sen == Sen_Block && GC.motor_r->postion == GC.GCR_hor_pos && GC.motor_r->motion == Stop) // �ȴ�����ˮƽ
			{
				GO.sta = Running;
				motorGo_acc(GO.motor_h, GO.GOH_mid_pos); // �ƶ�����Ƭ������
			}
		}
		break;
#endif
	/*�ƶ���ˮƽ�յ� ��Ƭ���õ��洢��*/
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
					Error_Set(Error_Sensor, GOV_glass_sensor);
				}
			}
		}
		break;
	/*����Ƭ�Ƴ�����,���̸�λ*/
	case GOH_back:
		if (GO.subtask == 0)
		{
			GO.motor_v->dir = Front;
			motor_Set_Direction(GO.motor_v);
			motorGO_Debug(GO.motor_v, GLASS_OUT_DIS, 0);
			GO.sta = Running;
		}
		else if (GO.subtask == 1)
		{
			GO.sta = Running;
		}
		else if (GO.subtask == 2)
		{
			Next_Task(GO_out, GOH_start);
		}
		break;
	/*��׼��һ�洢��*/
	case GO_out:
		if (GO.subtask == 0)
		{
			motorGo(GO.motor_v, GO.motor_v->max_pos - 800, 0);
			GO.sta = Running;
		}
		else if (GO.subtask == 1)
		{
			Next_Task(GO_start, GO_adjust);
		}

		break;

	case GO_finish:
		break;
	case GO_error:
		if (error_type & Error_Out_Box)
		{
			GO.sta = Finish;
		}
		else if (error_type & Error_StorageFull)
		{
			GO.sta = Finish;
		}
		else if (error_type & Error_Sucker) // ��ȡ��Ƭ����
		{
			if (GO.subtask == 0) // ���̸�λ��ԭ��
			{
				if (GOH_start_Sen == Sen_Block && GO.motor_h->motion == Stop)
				{
					GO.subtask = 1;
				}
				else
				{
#ifdef GC_ROT_PACKAGE
					if (GC_rot_Sen != Sen_Block && GC.motor_r->motion == Stop)
					{
						motorGo(GO.motor_h, 0, 0);
						GO.sta = Running;
					}
#endif
#ifdef GC_VER_PACKAGE
					if (GC_ver_Sen == Sen_Block && GC.motor_v->motion == Stop) // ���ָ�λ����ֱԭ��
					{
						motorGo(GO.motor_h, 0, 0);
						GO.sta = Running;
					}
#endif
				}
			}
			else if (GO.subtask == 1) // �����ƶ������ϵ�
			{
				if (GC.motor_r->postion == GC.GCR_ver_pos && GC.motor_r->motion == Stop)
				{
					motorGo(GO.motor_h, GO.GOH_waste_pos, 0);
					GO.sta = Running;
				}
			}
			else if (GO.subtask == 2) // �ص�ԭ��
			{
				if (GP.sta == Finish && GP.task == GP_error)
				{
					motorGo(GO.motor_h, 0, 0);
					GO.sta = Running;
				}
			}
		}
		else
		{
			GO.sta = Finish;
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
	/*��λ����ֱԭ��λ�� basic*/
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
	/*��λ��ˮƽԭ�� basic*/
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
	/*΢���洢��λ���ò�Ƭ��׼���¶� basic*/
	case GO_adjust:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
		}
		break;
	/*������λ*/
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
			
		}else if (GO.subtask == 4) // 
		{
			if (GO.motor_v->motion == Stop)
			{
				GO.sta = Finish;
				GO.subtask = 0;
			}
		}
		break;
	/*�ػ���λ*/
	case GO_reset_off:
#ifdef GC_ROT_PACKAGE
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
#endif
#ifdef GC_VER_PACKAGE
		if (GO.subtask == 0)
		{
		}
		else if (GO.subtask == 1)
		{
			if (GO.motor_h->motion == Stop)
			{
				GO.sta = Ready;
				GO.subtask++;
			}
		}
		else if (GO.subtask == 2)
		{
		}
		else if (GO.subtask == 3)
		{
		}
#endif
		break;
	/*�洢�۳���*/
	case GO_Box_Out:
		if (GO.motor_v->motion == Stop)
		{
			GO.sta = Finish;
		}
		break;
	/*�洢�۽���*/
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
	/*��ʼ����*/
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

	/*�ƶ�����Ƭ������*/
	case GOH_package:
		if (GO.motor_h->motion == Stop)
		{
			GO.sta = Finish;
		}
		break;
	/*�ƶ���ˮƽ�յ� ��Ƭ���õ��洢��*/
	case GOH_end:
		if (GOH_end_Sen == Sen_Block)
		{
			printf("end box pos:%d\r\n", GO.motor_v->postion);
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
	case GOH_back:
		if (GO.subtask == 0)
		{
			if (GO.motor_h->motion == Stop)
			{
				GO.subtask++;
				GO.sta = Ready;
			}
		}
		else if (GO.subtask == 1)
		{
			if (GO.running_tim > 500)
			{
				GO.subtask++;
				GO.sta = Ready;
			}
		}
		else if (GO.subtask == 2)
		{
		}
		break;
	/*��Ƭ�洢 �洢�������ƶ�*/
	case GO_out:
		// if (GO.motor_v->motion == Stop) // ����һ���۵ľ���� ��Ȼ�и�Ӧ
		// {
		// 	printf("out box pos:%d\r\n", GO.motor_v->postion);

		// 	GO.sta = Finish;
		// }

		if (GO.subtask == 0)
		{
			if (GO.motor_v->motion == Stop) // ����һ���۵ľ���� ��Ȼ�и�Ӧ
			{
				printf("out box pos:%d\r\n", GO.motor_v->postion);
				GO.subtask++;
				GO.sta = Ready;
			}
		}
		else if (GO.subtask == 1)
		{
		}

		break;
	case GO_finish:
		break;
	case GO_error:
		if (error_type & Error_Out_Box)
		{
			GO.sta = Finish;
		}
		else if (error_type & Error_StorageFull)
		{
			GO.sta = Finish;
		}
		else if (error_type & Error_Sucker) // ��ȡ�ǲ�Ƭʧ��
		{
			if (GO.subtask == 0) // ���̸�λ��ԭ��
			{
				if (GOH_start_Sen == Sen_Block)
				{
					GO.sta = Ready;
					GO.subtask = 1;
				}
				else
				{
					if (GO.motor_h->motion == Stop)
					{
						Error_Set(Error_Sensor, GOH_start_sensor);
					}
				}
			}
			else if (GO.subtask == 1) // �����ƶ������ϵ�
			{
				if (GO.motor_h->motion == Stop)
				{
					GO.sta = Ready;
					GO.subtask = 2;
				}
			}
			else if (GO.subtask == 2) // �ص�ԭ��
			{
				if (GOH_start_Sen == Sen_Block)
				{
					GO.sta = Finish;
					GO.subtask = 0;
				}
				else
				{
					if (GO.motor_h->motion == Stop)
					{
						Error_Set(Error_Sensor, GOH_start_sensor);
					}
				}
			}
		}
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
		if (GO.WaitAck)
		{
			GO.WaitAck = 0;
			ack_task(GO.Index, Type_controlAck, Fc_run, Extra_run_Close, AckResult_executeSucceed);
		}
		break;
	case GO_Box_Out:
		GO.subtask = 0;
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		if (GO.WaitAck)
		{
			GO.WaitAck = 0;
			ack_task(GO.Index, Type_controlAck, Fc_run, Extra_run_GOInOut, AckResult_executeSucceed);
		}
#ifdef DEBUG_MDOE
		GO.sta = Ready;
		GO.task = GO_none;
#endif
		break;
	case GO_Box_In:
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		GO.subtask = 0;
		GO.sta = Ready;
		GO.task = GO_none;
		if (GO.WaitAck)
		{
			GO.WaitAck = 0;
			ack_task(GO.Index, Type_controlAck, Fc_run, Extra_run_GOInOut, AckResult_executeSucceed);
		}

		break;
	case GO_start:
#ifdef GC_ROT_PACKAGE
		if (GC.task == GC_rot_up && GC.sta == Finish)
		{
			GO.sta = Ready;
			GO.task = GOH_package;
		}
		else
		{
			if (GC.task == GC_finish)
			{
				GO.task = GO_finish;
				GO.sta = Ready;
				// ������������ͣ��������ʱ���������������Ϣ
				if (TaskThread_State != taskthread_pause)
				{
					// �������������Ϣ �������������״̬
					ack_0data_task(screenUart_lastRecvIndex++, Type_controlAck, Fc_run, Extar_run_Finish);
					TaskThread_State = taskthread_finsih;
				}
			}
		}
		break;
#endif
#ifdef GC_VER_PACKAGE
		if (GC.task == GC_rot_hor && GC.sta == Finish)
		{
			GO.sta = Ready;
			GO.task = GOH_package;
		}
		else
		{
			if (GC.task == GC_finish)
			{
				GO.task = GO_finish;
				GO.sta = Ready;
				// ������������ͣ��������ʱ���������������Ϣ
				if (TaskThread_State != taskthread_pause)
				{
					// �������������Ϣ
					ack_0data_task(screenUart_lastRecvIndex++, Type_control, Fc_run, Extar_run_Finish);
					TaskThread_State = taskthread_finsih;
					if(Global_Parm.Warn->Finish_buzzer)
					{
						Buzzer=BUZZER_ENABLE;
					}
				}
			}
		}
		break;
#endif

	case GOH_package:
#ifdef BIG_CYLINDER
		if (GC.motor_r->postion == GC.GCR_ver_pos && GP_big_cyl_Sen == Sen_Block) // �����ִ�ֱ �Ҵ����׸�λ
		{
			GO.sta = Ready;
			GO.task = GOH_end;
			GO.main_subtask = 0;
			GO.main_task = GO_none;
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
		// ��Ƭ��Ƭ����+1
		slot_num--;
		ack_task(screenUart_lastRecvIndex++, Type_set, Fc_data, Extra_data_package, 0);
		GO.sta = Ready;
		GO.task = GOH_back;
		GO.main_subtask = 0;
		GO.main_task = GO_none;
		break;
	case GOH_back:
		break;
	case GO_out:

		break;
	case GO_finish:

	
		break;
	case GO_error:
			if (error_type & Error_Sucker) // ��ȡ�ǲ�Ƭʧ��
			{
				if(!Global_Parm.Warn->GP_sucker_stop)
				{
					TaskThread_Resume_Error();
				}
			}
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
			Error_OverTime_Set(Extra_error_overtime_GO, GO.task);
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
