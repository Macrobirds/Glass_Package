#include "Globalconfig.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define DELAY_SUCKER 100
#define DELAY_BIG_CYLIDER 1500
#define DELAY_SMALL_CYLIDER 300
#define DELAY_SPRAY 2000
#define DELAY_MOVE_PACKAGE1 1000
#define DELAY_MOVE_PACKAGE2 100

// ����Ƿ�����罺ͷ
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
	//	if(GP.task!=GP_none)
	//	{
	//		printf("GP task:%d subtask:%d\r\n",GP.task,GP.subtask);
	//	}
	switch (GP.task)
	{
	case GP_none:
		break;
	/////////////�ν�����/////////////// basic
	case GP_spray_test:
		Gas_Spray_Enable();
		GP.sta = Running;
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
			motorGo(GP.motor, 0, 0);
		}
		break;
	////////////��С���׸�λ/////////////// basic
	case GP_cyl_start:
		if (GP_big_cyl_Sen == Sen_Block && GP_small_cyl_Sen == Sen_Block)
		{
			GP.sta = Finish;
		}
		else
		{
			GP_small_Cyl = GAS_DISABLE; // С����ʧ��
			#ifdef BIG_CYLINDER
			GP_big_Cyl = GAS_DISABLE;	// ������ʧ��
			#endif
			#ifdef BIG_CYLINDER_MOTOR
			motorGo(GP.motor_cyl,0,0);
			#endif
			GP.sta = Running;
		}
		

		break;
	//////////////���̸�λ//////////////////// basic
	case GP_sucker_start:
		GP_sucker_Pump = GAS_DISABLE;
		GP_sucker1_Cyl = GAS_DISABLE;
		GP_sucker2_Cyl = GAS_DISABLE;
		Gas_Spray_Disable();
		GP.sta = Running;
		break;
	/////////////������λ///////////////
	case GP_reset_on:
		if (GP.subtask == 0) // �����ͷ�
		{
			Next_Task(GP.task, GP_sucker_start);
		}
		else if (GP.subtask == 1) // ��С���׸�λ
		{
			Next_Task(GP.task, GP_cyl_start);
		}
		else if (GP.subtask == 2)
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
				GP.subtask = 3;
			}
		}
		else if (GP.subtask == 3) // ��λ��ԭ��
		{
			Next_Task(GP_none, GP_move_start);
		}
		break;
	/////////////�ػ���λ//////////////
	case GP_reset_off:
		if(GP.subtask==0)
		{
			if(GP_sucker_Sen==Sen_Block)
			{
				motorGo_acc(GP.motor,GP.sucker_pos);
				GP.sta=Running;
			}else
			{
				GP.subtask=2;
			}
		}else if(GP.subtask==1)
		{
			GP_big_Cyl=GAS_ENABLE;
			GP.sta=Running;
		}else if (GP.subtask == 2) // �ر�����
		{
			Next_Task(GP.task, GP_sucker_start);
		}
		else if (GP.subtask == 3) // ���׸�λ
		{
			Next_Task(GP.task, GP_cyl_start);
		}
		else if (GP.subtask == 4) 
		{
			if (GC_claw_Sen != Sen_Block) // �����ͷ�
			{
				Next_Task(GP_none, GP_move_start);
			}
			else // �뿪ԭ��λ �ȴ������ͷŸ�λ
			{
				motorGo_acc(GP.motor, GP.sucker_pos);
				GP.sta = Running;
			}
		}
		else if (GP.subtask == 5)
		{
			if (GC_claw_Sen != Sen_Block) //�ȴ������ͷ�
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
				Next_Task(GP.task, GP_move_start); // ԭ�㸴λ
			}
			else if(GP.subtask==2)
			{
				GC.Glass_Ready = FALSE; //��Ƭ�������
				GP.sta=Finish;
			}
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
			#ifdef BIG_CYLINDER
			GP_big_Cyl = GAS_ENABLE;
			#endif
			#ifdef BIG_CYLINDER_MOTOR
			motorGo(GP.motor_cyl,GP.sucker_down_pos,0);
			#endif
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
		#ifdef BIG_CYLINDER
		GP_big_Cyl = GAS_DISABLE;
		#endif
		#ifdef BIG_CYLINDER_MOTOR
		motorGo(GP.motor_cyl,0,0);
		#endif
	
		GP.sta = Running;
		break;
	/////////////�ƶ����罺λ��/////////
	case GP_move_spray:
		if ((GC.motor_r->postion == GC.GCR_hor_pos) && (GC.motor_r->motion == Stop)) // ����λ����תˮƽλ�þ�ֹ
		{
			GP.sta = Running;
			motorGo_acc(GP.motor, GP.spray_pos);
		}

		break;
		/////////////�ƶ�����Ƭλ��///////// ��Ƭλ�ú�ԭ��λ���غ�
	case GP_move_package:
		if (GP_start_Sen == Sen_Block)
		{
			GP.sta = Finish;
		}
		else
		{

			if (GP.subtask == 0) // ��ʱ1000ms�ȴ�ITV0011��Ӧ
			{
				GP.sta = Running;
				Gas_Spray_Enable(); // ��ʼ�罺
			}
			else if (GP.subtask == 1)
			{
				GP.sta = Running;
				motorGo(GP.motor, GP.spray_pos - GP.spray_len, GP.spray_speed); // ���罺�ٶ������˶�����Ƭ��
			}
			else if (GP.subtask == 2) // ��ʱ100ms�ȴ���ȫ�����
			{
				GP.sta = Running;
				Gas_Spray_Disable(); // ֹͣ�罺
			}
			else if (GP.subtask == 3)
			{
				GP.sta = Running;
				motorGo(GP.motor, 0, 0);
			}
		}
		break;
	////////////////��Ƭ����/////////////////////
	case GP_package:
		if (GP.subtask == 0)
		{
			#ifdef BIG_CYLINDER
			GP_big_Cyl = GAS_ENABLE; // �������½�
			#endif
			#ifdef BIG_CYLINDER_MOTOR
			motorGo(GP.motor_cyl,GP.sucker_package_pos,0);
			#endif
			
			GP.sta = Running;
		}
		else if (GP.subtask == 1)
		{
			#ifdef BIG_CYLINDER_MOTOR
			motorGo(GP.motor_cyl,GP.sucker_finish_pos,0);
			#endif
			GP_small_Cyl = GAS_DISABLE; // С��������
			GP.sta = Running;
		}

		else if (GP.subtask == 2)
		{
			GP_sucker1_Cyl = GAS_DISABLE; // �ر�����1����
			GP.sta = Running;
		}

		else if (GP.subtask == 3)
		{
			GP_sucker_Pump = GAS_DISABLE; // �ر�����2���� ��������
			GP_sucker1_Cyl = GAS_DISABLE;
			GP_sucker2_Cyl = GAS_DISABLE;
			GP.sta = Running;
		}
		break;
	case GP_finish:
		break;
	case GP_error:
		//ȱ�ٸǲ�Ƭ
		if(error_type&Error_Cover_Glass)
		{
			//����������
			#ifdef BIG_CYLINDER
			GP_big_Cyl=GAS_DISABLE;
			#endif
			#ifdef BIG_CYLINDER_MOTOR
			motorGo(GP.motor_cyl,0,0);
			#endif
			
			GP.sta=Running;
		}else if(error_type&Error_Spray)
		{
			GP.sta=Finish;
		}else if(error_type&Error_Sucker)
		{
			GP_sucker_Pump = GAS_DISABLE;
			GP_sucker1_Cyl = GAS_DISABLE;
			GP_sucker2_Cyl = GAS_DISABLE;
			Gas_Spray_Disable();
			GP.sta = Finish;
		}
		break;
	}
}

void GP_RunningTask(void)
{
	switch (GP.task)
	{
	case GP_none:
		break;
	/////////////�ν�����///////////////
	case GP_spray_test:
		if (GP.running_tim > DELAY_SPRAY)
		{
			Gas_Spray_Disable();
			GP.sta = Finish;
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
			if (GP.motor->motion == Stop)
			{
				Error_Set(Error_Sensor, GP_start_sensor);
			}
		}
		break;
	////////////////��С���׸�λ///////////////// basic
	case GP_cyl_start:
		#ifdef BIG_CYLINDER
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
		#endif
		#ifdef BIG_CYLINDER_MOTOR
		if(GP_big_cyl_Sen==Sen_Block&&GP_small_cyl_Sen)
		{
			GP.sta=Finish;
		}else
		{
			if(GP.motor_cyl->motion==Stop&&GP_small_cyl_Sen==Sen_Block)
			{
				Error_Set(Error_Sensor,GP_big_cyl_sensor);
			}
		}
		#endif
	
	
		break;
	//////////////���̸�λ//////////////////// basic
	case GP_sucker_start:
		if (GP.running_tim > DELAY_SUCKER)
		{
			GP.sta = Finish;
		}
		break;
	case GP_reset_on:
		if (GP.subtask == 0) // ��С���׸�λ
		{
		}
		else if (GP.subtask == 1) // �����ͷ�
		{
		}
		else if (GP.subtask == 2)
		{
			if (GP.motor->motion == Stop) // ��ԭ�㴫������Ӧ �뿪ԭ�� ����У׼λ��
			{
				GP.subtask = 2;
				GP.sta = Ready;
			}
		}
		else if (GP.subtask == 3) // ��λ��ԭ��
		{
		}
		break;
	case GP_reset_off:
		if(GP.subtask==0)
		{
			if (GP.motor->motion == Stop)
			{
				GP.subtask = 1;
				GP.sta = Ready;
			}		
		}else if(GP.subtask==1)
		{
			if(GP.running_tim>DELAY_BIG_CYLIDER)
			{
				GP.subtask=2;
				GP.sta=Ready;
			}
		
		}else if (GP.subtask == 2) // �ر�����
		{
		}
		else if (GP.subtask == 3) // ���׸�λ
		{
		}
		else if (GP.subtask == 4) // �˶�����Ƭ��
		{

			if (GP.motor->motion == Stop)
			{
				GP.subtask = 3;
				GP.sta = Ready;
			}
		}
		else if (GP.subtask == 5)
		{
		}
		break;

	case GP_start:

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
			#ifdef BIG_CYLINDER
			if (GP.running_tim > DELAY_BIG_CYLIDER) // �ȴ��������½�
			{
				if (GP_big_cyl_Sen != Sen_Block)
				{
					GP.subtask = 0;
					GP.sta = Finish;
				}
			}
			#endif
			#ifdef BIG_CYLINDER_MOTOR
			if(GP.motor_cyl->motion==Stop)
			{
				GP.subtask=0;
				GP.sta=Finish;
			}	
			#endif
		}
		
		break;
	/////////��ȡ�ǲ�Ƭ///////////////
	case GP_suck_glass:
		if (GP.running_tim > DELAY_SUCKER)
		{
			if (GP_sucker_Sen == Sen_Block)
			{
				GP.sta = Finish;
			}
			else
			{
				Error_Set(Error_Sensor, GP_sucker_sensor);
			}
		}
		break;
	///////////��Ƭ��������///////////
	case GP_sucker_up:
		#ifdef BIG_CYLINDER
		if (GP.running_tim > DELAY_BIG_CYLIDER)
		{
			if (GP_big_cyl_Sen == Sen_Block)
			{
				GP.sta = Finish;
			}
			else
			{
				Error_Set(Error_Sensor, GP_big_cyl_sensor);
			}
		}
		#endif
		#ifdef BIG_CYLINDER_MOTOR
		if(GP_big_cyl_Sen==Sen_Block)
		{
			GP.sta=Finish;
		}else
		{
			if(GP.motor_cyl->motion==Stop)
			{
				Error_Set(Error_Sensor,GP_big_cyl_sensor);
			}
		}
		
		#endif
	
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
		if (GP.subtask == 0) // ��ʱ200ms�ȴ�ITV0011��Ӧ
		{
			if (GP.running_tim > DELAY_MOVE_PACKAGE1)
			{
				GP.sta = Ready;
				GP.subtask = 1;
			}
		}
		else if (GP.subtask == 1)
		{
			if (GP.motor->motion == Stop)
			{
				GP.sta = Ready;
				GP.subtask = 2;
			}
		}
		else if (GP.subtask == 2) // ��ʱ200ms�ȴ���ȫ�����
		{
			if (GP.running_tim > DELAY_MOVE_PACKAGE2)
			{
				GP.sta = Ready;
				GP.subtask = 3;
			}
		}
		else if (GP.subtask == 3)
		{
			if (GP_start_Sen == Sen_Block)
			{
				GP.subtask = 0;
				GP.sta = Finish;
			}
			else
			{
				if (GP.motor->motion == Stop)
				{
					Error_Set(Error_Sensor, GP_start_sensor);
				}
			}
		}
		break;
	////////////////��Ƭ����/////////////////////
	case GP_package:
		if (GP.subtask == 0)
		{
			#ifdef BIG_CYLINDER
			if (GP.running_tim > GP.delay_before) // �ȴ��������½� �ǲ�Ƭ���ز�Ƭ�Ӵ�
			{
				GP.sta = Ready;
				GP.subtask = 1;
				GP.running_tim = 0;
			}
			#endif
			#ifdef BIG_CYLINDER_MOTOR
			if(GP.motor_cyl->motion==Stop)
			{
				GP.sta=Ready;
				GP.subtask=1;
			}
			#endif
		}
		else if (GP.subtask == 1) // �ȴ�С�������� �ǲ�Ƭ���ز�Ƭˮƽ
		{
			if (GP.running_tim > GP.delay_after)
			{
				GP.sta = Ready;
				GP.subtask = 2;
				GP.running_tim = 0;
			}
		}
		else if (GP.subtask == 2) // �ȴ�����1�ر�
		{
			if (GP.running_tim > DELAY_SUCKER)
			{
				GP.sta = Ready;
				GP.subtask = 3;
				GP.running_tim = 0;
			}
		}
		else if (GP.subtask == 3) // �ȴ�����2 �������ùر�
		{
			#ifdef BIG_CYLINDER_MOTOR
			if (GP.running_tim > 1000&&GP.motor_cyl->motion==Stop)
			{
				GP.subtask = 0;
				GP.sta = Finish;
				GP.running_tim = 0;
			}
			#endif
			#ifdef BIG_CYLINDER
			if (GP.running_tim > 1000)
			{
				GP.subtask = 0;
				GP.sta = Finish;
				GP.running_tim = 0;
			}
			#endif
			
		}
		break;
	case GP_finish:
		break;
	case GP_error:
		#ifdef BIG_CYLINDER
		if(GP.running_tim>DELAY_BIG_CYLIDER)
		{
			if(GP_big_cyl_Sen==Sen_Block)
			{
				GP.sta=Finish;
			}else
			{
				Error_Set(Error_Sensor,GP_big_cyl_sensor);
			}
		}
		#endif
		#ifdef BIG_CYLINDER_MOTOR
			if(GP_big_cyl_Sen==Sen_Block)
			{
				GP.sta=Finish;
			}else
			{
				if(GP.motor_cyl->motion==Stop)
				{
					Error_Set(Error_Sensor,GP_big_cyl_sensor);
				}

			}
		#endif
	
		break;
	}
}

void GP_FinishTask(void)
{
	switch (GP.task)
	{
	case GP_none:
		break;
	case GP_spray_test:
		Gas_Spray_Disable();
		GP.main_subtask=0;
		GP.main_task=GP_none;
		GP.subtask = 0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_move_start:
		Resume_Task();
		break;
	case GP_cyl_start:
		Resume_Task();
		break;
	case GP_sucker_start:
		Resume_Task();
		break;
	case GP_reset_on:
		GP.main_subtask=0;
		GP.main_task=GP_none;
		GP.subtask = 0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_reset_off:
		GP.main_subtask=0;
		GP.main_task=GP_none;
		GP.subtask = 0;
		GP.sta = Ready;
		GP.task = GP_none;
		break;
	case GP_start:
		if(GC.task>=GC_move_down&&GC.task<=GC_move_up)
		{
			GP.subtask = 0;
			GP.sta = Ready;
			GP.task = GP_move_glass;
		}else
		{
			if(GC.task==GC_finish)
			{
				GP.task=GP_finish;
				GP.sta=Ready;
				GP.subtask = 0;
			}
		}
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
			GP.subtask = 0;
		}
		else
		{
			Error_Set(Error_Cover_Glass, 0); // ���� û�иǲ�Ƭ
		}
		break;
	case GP_suck_glass:
		if (GP_sucker_Sen == Sen_Block) // ��⵽��Ƭ
		{
			GP.sta = Ready;
			GP.task = GP_sucker_up;
			GP.subtask = 0;
		}
		else
		{
			Error_Set(Error_Cover_Glass, 0); // ���� û�иǲ�Ƭ
		}
		break;
	case GP_sucker_up:
		if ((GC.task == GC_rot_hor) && (GC.sta == Finish)) // �ȴ����ֽ���Ƭ���õ���������
		{
			if(GP_sucker_Sen==Sen_Block)
			{
				GP.sta = Ready;
				GP.task = GP_move_spray;
				GP.subtask = 0;
			}else
			{
				Error_Set(Error_Sucker,0);
			}

		}
		else if (GC.task == GC_finish)
		{
			GP.task = GP_finish;
			GP.sta = Ready;
			GP.subtask = 0;
		}
		break;
	case GP_move_spray:
		GP.sta = Ready;
		GP.task = GP_move_package;
		GP.subtask = 0;
		break;
	case GP_move_package:
		GP.sta = Ready;
		GP.task = GP_package;
		GP.subtask = 0;
		break;
	case GP_package:
		#ifdef BIG_CYLINDER
		if (GP_big_cyl_Sen != Sen_Block && GP_small_cyl_Sen == Sen_Block) // �ȴ���С���׵�λ
		{
			GP.subtask = 0;
			GP.sta = Ready;
			GP.task = GP_start;
		}
		#endif
		#ifdef BIG_CYLINDER_MOTOR
		if (GP.motor_cyl->motion==Stop && GP_small_cyl_Sen == Sen_Block) 
		{
			GP.subtask = 0;
			GP.sta = Ready;
			GP.task = GP_start;
		}
		#endif
		
	
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
			Error_OverTime_Set(GP_idx,GP.task);
			//Error_Set(Error_OverTime, 0);
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
//! debug_flag

	
//		if (GP.subtask == 0) // �ر�����
//		{
//			Next_Task(GP.task, GP_sucker_start);
//		}
//		else if (GP.subtask == 1) // ���׸�λ
//		{
//			Next_Task(GP.task, GP_cyl_start);
//		}
//		else if (GP.subtask == 2) 
//		{
//			if (GC_claw_Sen != Sen_Block) // �����ͷ�
//			{
//				Next_Task(GP_none, GP_move_start);
//			}
//			else // �뿪ԭ��λ �ȴ������ͷŸ�λ
//			{
//				motorGo_acc(GP.motor, GP.sucker_pos);
//				GP.sta = Running;
//			}
//		}
//		else if (GP.subtask == 3)
//		{
//			if (GC_claw_Sen != Sen_Block) //�ȴ������ͷ�
//			{
//				Next_Task(GP_none, GP_move_start);
//			}
//		}