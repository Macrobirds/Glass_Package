#include "Globalconfig.h"

// ����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
static u8 check_GC(void)
{
	return (GC_ver_Sen == Sen_Block) && (GC.motor_v->motion == Stop);
}

void GE_ReadyTask(void)
{

	switch (GE.task)
	{
	case GE_none:
		break;
	/////////////////������λ//////////////////////////////
	case GE_reset_on:
		if (GE.subtask == 0)
		{
			if (GE_start_Sen == Sen_Block) // ԭ���Ӧ  �뿪��Ӧ�� �ٻص�ԭ��
			{
				GE.motor->dir = Front;
				motor_Set_Direction(GE.motor);
				motorGO_Debug(GE.motor, GE.motor->pulse_1mm * 10, 0);
				GE.sta = Running;
			}
			else
			{
				GE.subtask = 1;
			}
		}
		else if (GE.subtask == 1)
		{
			if (check_GC())
			{
				motorGo(GE.motor, 0, 0);
				GE.sta = Running;
			}
		}
		break;
	/////////////�ػ���λ///////////////////////////
	case GE_reset_off:
		if (check_GC())
		{
			motorGo(GE.motor, 0, 0);
			GE.sta = Running;
		}
		break;
	/////////////////װ�ز۽���//////////////////////////////
	case GE_Box_In:
		if (GE_start_Sen == Sen_Block) // ԭ���Ӧ װ�ز��ѽ���
		{
			stepperMotorStop(GE.motor);
			GE.motor->postion = 0;
			GE.sta = Finish;
		}
		else
		{
			if (check_GC()) // ǰ��ԭ��
			{
				motorGo(GE.motor, 0, 0);
				GE.sta = Running;
			}
		}
		break;
	/////////////////װ�ز��Ƴ�//////////////////////////////
	case GE_BOx_Out:
		if (check_GC())
		{
			motorGo_acc(GE.motor, GE.motor->max_pos - 100);
			GE.sta = Running;
		}

		break;

	/////////////////////װ�ز۸�λ��ԭ��//////////////////////////////////
	case GE_start:
		GE.box_Exist = FALSE;
		if (GE_start_Sen == Sen_Block)
		{
			stepperMotorStop(GE.motor);
			GE.motor->postion = 0;
			GE.sta = Finish;
		}
		else
		{
			if (check_GC()) // ����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
			{
				motorGo(GE.motor, 0, 0);
				GE.sta = Running;
			}
		}
		break;
	////////////////////װ�ز�ǰ���ƶ����������·�////////////////////////
	case GE_move_front:
		if (GE_up_Sen == Sen_Block)
		{
			GE.sta = Finish;
		}
		else
		{
			if (check_GC()) // ����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
			{
				motorGo(GE.motor, (GE.GE_box_len) * 2 - (50 * GE.motor->pulse_1mm), 0);
				GE.sta = Running;
			}
		}
		break;
	//////////////////////��Ƭ�ƶ����������·�//////////////////////////////////
	case GE_move_glass:
		if (check_GC()) // ����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
		{
			GE.motor->dir = Front;
			motor_Set_Direction(GE.motor);
			motorGO_Debug(GE.motor, GE.GE_box_len, GE.GE_box_speed);
			GE.sta = Running;
		}
		break;
	///////////////////װ�زۺ���뿪�����·�////////////////////////////////////////////
	case GE_move_back:
		if (check_GC()) // ����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
		{
			GE.motor->dir = Front;
			motor_Set_Direction(GE.motor);
			motorGO_Debug(GE.motor, GE.motor->pulse_1mm * 30, 0);
			GE.sta = Running;
			GE.glass_Exist = FALSE;
		}
		break;
		///////////////////////////////�����������//////////////////////////////
	case GE_finish:

		break;
	////////////////////////////////�쳣����///////////////////////////////
	case GE_error:
		// ȱ��װ�ز�

		// װ�ز�ȱ���ز�Ƭ

		GE.sta = Finish;

		break;
	}
}

void GE_RunningTask(void)
{
	switch (GE.task)
	{
	case GE_none:
		break;
	/////////////////������λ////////////////////////////////
	case GE_reset_on:
		if (GE.subtask == 0)
		{
			// �뿪ԭ���Ӧ
			if (GE.motor->motion == Stop)
			{
				GE.sta = Ready;
				GE.subtask = 1;
			}
		}
		else if (GE.subtask == 1)
		{
			if (GE_start_Sen == Sen_Block)
			{
				GE.sta = Finish;
				GE.subtask = 0;
			}
			else
			{
				if (GE.motor->motion == Stop)
				{
					Error_Set(Error_Sensor, GE_start_sensor);
				}
			}
		}

		break;
	//////////////�ػ���λ//////////////////////////
	case GE_reset_off:
		if (GE_start_Sen == Sen_Block)
		{
			GE.sta = Finish;
			GE.subtask = 0;
		}
		else
		{
			if (GE.motor->motion == Stop)
			{
				Error_Set(Error_Sensor, GE_start_sensor);
			}
		}
		break;
	/////////////////װ�ز۽���//////////////////////////////
	case GE_Box_In:
		if (GE_start_Sen == Sen_Block)
		{
			GE.sta = Finish;
		}
		else
		{
			if (GE.motor->motion == Stop)
			{
				Error_Set(Error_Sensor, GE_start_sensor);
			}
		}
		break;
	/////////////////װ�ز��Ƴ�//////////////////////////////
	case GE_BOx_Out:
		if (GE.motor->motion == Stop)
		{
			GE.sta = Finish;
		}
		break;
	/////////////////////װ�ز۸�λ��ԭ��//////////////////////////////////
	case GE_start:
		if (GE_start_Sen == Sen_Block)
		{
			GE.sta = Finish;
		}
		else
		{
			if (GE.motor->motion == Stop) // GEԭ�㴫��������
			{
				Error_Set(Error_Sensor, GE_start_sensor);
			}
		}
		break;
	/////////////////////�ȴ�װ�ز�ǰ���ƶ����������·�///////////////////////////////
	case GE_move_front:
		if (GE.motor->motion == Stop) // GE ���¶˴�����һֱδ��Ӧ
		{
			if (GE_up_Sen == Sen_Block)
			{
				GE.box_Exist = TRUE;
				GE.sta = Finish;
			}
			else
			{
				GE.sta = Ready;
				GE.task = GE_finish;
				TaskThread_State = taskthread_finsih;
			}
		}
		break;
	/////////////////////��Ƭ�ƶ����������·�//////////////////////////////////
	case GE_move_glass:
		if (GE.motor->motion == Stop) // ��������Ӧֹͣ
		{
			if (GE_up_Sen != Sen_Block && GE_down_Sen == Sen_Block)
			{
				GE.sta = Finish;
				GE.glass_Exist = TRUE; // ��⵽��Ƭ
			}
			else if (GE_up_Sen == Sen_Block)
			{
				GE.sta = Finish;
				GE.glass_Exist = FALSE; // ����β�� δ��⵽��Ƭ
			}
			else
			{
				Error_Set(Error_Sensor, GE_up_sensor | GE_down_sensor);
			}
		}
		break;
		/////////////////////װ�زۺ���뿪�������·�//////////////////////////////////
	case GE_move_back:
		if (GE.motor->motion == Stop) // ���¶˴���������
		{
			GE.sta = Finish;
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
	if (TaskThread_State == taskthread_pause) // ������ͣ ���浱ǰ����״̬����ת����������
	{
		GE.resume_task = GE.task;
		GE.task = GE_finish;
		GE.sta = Ready;
	}

	switch (GE.task)
	{
	case GE_none:
		break;
	case GE_reset_on:
		if (TaskThread_CheckIdle())
		{
			/*���͸�λ��ɵ���Ϣ*/
			if (GE.WaitAck)
			{
				GE.WaitAck = 0;
				ack_task(GE.Index, Type_controlAck, Fc_run, Extra_run_Reset, AckResult_executeSucceed);
			}

			GE.task = GE_none;
			GE.sta = Ready;
			GE.subtask = 0;
		}
		break;
	case GE_reset_off:
		if (TaskThread_CheckIdle())
		{
			/*���͹ػ���λ�����Ϣ*/
			if (GE.WaitAck)
			{
				GE.WaitAck = 0;
				ack_task(GE.Index, Type_controlAck, Fc_run, Extra_run_Close, AckResult_executeSucceed);
			}

			GE.task = GE_none;
			GE.sta = Ready;
			GE.subtask = 0;
		}

		break;
	case GE_Box_In:
		GE.task = GE_none;
		GE.sta = Ready;
		break;
	case GE_BOx_Out: // ��������״̬
#ifdef DEBUG_MODE
		GE.task = GE_none;
		GE.sta = Ready;
#endif
		break;
	case GE_start: // ��ת��װ�ز�ǰ���ƶ����������·�
		GE.sta = Ready;
		GE.task = GE_move_front;
		break;
	case GE_move_front: // ��ת����Ƭǰ���ƶ����������·�
		GE.sta = Ready;
		GE.task = GE_move_glass;
		GE.glass_Exist = FALSE;
		GE.box_Exist = TRUE; // ��⵽��Ƭ��
		break;
	case GE_move_glass:
		if (GE.glass_Exist) // ��⵽��Ƭ
		{
			if (check_GC()) // ����λ�ڴ�ֱԭ�㣬��ֱ��ֹ
			{
				if (GE_down_Sen != Sen_Block && GE_up_Sen != Sen_Block) // ��Ƭ����ȡ ���¶��޸�Ӧ
				{
					GE.sta = Ready;
				}
			}
		}
		else // ������
		{
			GE.sta = Ready;
			GE.task = GE_move_back;
		}

		break;
	case GE_move_back:
		GE.glass_Exist = FALSE;
		GE.sta = Ready;
		GE.task = GE_move_front;
		break;
	case GE_finish:
		break;
	case GE_error:
		break;
	}
}

void GE_TaskThread(void)
{
	if (GE.sta == Ready)
	{
		GE_ReadyTask();
		// ���ؼ�ʱ
		GE.running_tim = 0;
	}

	if (GE.sta == Running)
	{
		GE_RunningTask();
		// ��ʱ ����ʱ���г�ʱ�����ϱ�
		GE.running_tim++;
		if (GE.running_tim > OVERTIME)
		{
			Error_OverTime_Set(GE_idx, GE.task);
			// Error_Set(Error_OverTime, 0);
		}
	}

	if (GE.sta == Finish)
	{
		if (GE.task <= GE_BOx_Out)
		{
			GE_FinishTask();
		}
		else
		{
			if (TaskThread_State != taskthread_debug) // debug ģʽ�²�����������ת
			{
				GE_FinishTask();
			}
		}
	}
}