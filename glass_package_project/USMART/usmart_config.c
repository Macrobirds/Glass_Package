#include "usmart.h"
#include "usmart_str.h"

////////////////////////////�û�������///////////////////////////////////////////////
// ������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����)
#include "delay.h"
#include "sys.h"
#include "motor.h"

static void Set_motor_dir(u8 dir)
{
	if (dir)
	{
		GOH_DIR = 1;
	}
	else
	{
		GOH_DIR = 0;
	}
}

void gpio_test(u8 state, u8 pin)
{
	if (pin >= 8 && pin <= 15)
	{
		if (state)
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_0 << pin);
		}
		else
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_0 << pin);
		}
	}
	else if (pin <= 1 || pin == 12)
	{
		if (state)
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_0 << pin);
		}
		else
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_0 << pin);
		}
	}
	else if (pin == 7)
	{
		if (state)
		{
			GPIO_SetBits(GPIOE, GPIO_Pin_0 << pin);
		}
		else
		{
			GPIO_ResetBits(GPIOE, GPIO_Pin_0 << pin);
		}
	}
}

void close_device(void)
{
	Pause_TaskThread();
	TaskThread_State = taskthread_close;
}

void boost_device_test(u8 task)
{
	TaskThread_State = taskthread_boost;
}

void motor_test(u8 dir, u8 motor, u32 pulse, u32 freq)
{
	switch (motor)
	{
	case 1: // GE motor
		if (dir)
		{
			GE_motor_struct.dir = Front;
		}
		else
		{
			GE_motor_struct.dir = Back;
		}
		// set the dir
		motor_Set_Direction(&GE_motor_struct);

		motorGO_Debug(&GE_motor_struct, pulse, freq);

		break;
	case 2: // GCV motor
		if (dir)
		{
			GC_ver_motor_struct.dir = Front;
		}
		else
		{
			GC_ver_motor_struct.dir = Back;
		}
		motor_Set_Direction(&GC_ver_motor_struct);

		motorGO_Debug(&GC_ver_motor_struct, pulse, freq);
		break;
	case 3: // GCV_motor
		if (dir)
		{
			GC_rot_motor_struct.dir = Front;
		}
		else
		{
			GC_rot_motor_struct.dir = Back;
		}
		motor_Set_Direction(&GC_rot_motor_struct);

		motorGO_Debug(&GC_rot_motor_struct, pulse, freq);

		break;
	case 4: // GP_motor
		if (dir)
		{
			GP_motor_struct.dir = Front;
		}
		else
		{
			GP_motor_struct.dir = Back;
		}
		motor_Set_Direction(&GP_motor_struct);
		motorGO_Debug(&GP_motor_struct, pulse, freq);
		break;
	case 5: // GOH_motor
		if (dir)
		{
			GO_hor_motor_struct.dir = Front;
		}
		else
		{
			GO_hor_motor_struct.dir = Back;
		}
		motor_Set_Direction(&GO_hor_motor_struct);
		motorGO_Debug(&GO_hor_motor_struct, pulse, freq);
		break;
	case 6: // GOV_motor
		if (dir)
		{
			GO_ver_motor_struct.dir = Front;
		}
		else
		{
			GO_ver_motor_struct.dir = Back;
		}
		motor_Set_Direction(&GO_ver_motor_struct);
		motorGO_Debug(&GO_ver_motor_struct, pulse, freq);
		break;
	}
}
void motoracc_test(u8 dir, u8 motor, u32 pulse)
{
	switch (motor)
	{
	case 1: // GE motor
		if (dir)
		{
			GE_motor_struct.dir = Front;
		}
		else
		{
			GE_motor_struct.dir = Back;
		}
		// set the dir
		motor_Set_Direction(&GE_motor_struct);

		motorAccGO_Debug(&GE_motor_struct, pulse);

		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	}
}

void boost_test(u8 task, u8 task_index)
{
	switch (task)
	{
	case 1: // GE task
		if (task_index <= GE_BOx_Out)
		{

			GE.sta = Ready;
			GE.subtask = 0;
			GE.task = task_index;

		}

		break;
	case 2:
		if (task_index <= GC_reset_off)
		{
			OS_ENTER_CRITICAL();
			GC.sta = Ready;
			GC.subtask = 0;
			GC.task = task_index;
			OS_EXIT_CRITICAL();
		}
		break;
	case 3:
		if (task_index <= GP_reset_off)
		{
			GP.sta = Ready;
			GP.subtask = 0;
			GP.task = task_index;
		}
		break;
	case 4:
		if (task_index <= GO_Box_Out)
		{
			debug_flag=FALSE;
			GO.sta = Ready;
			GO.subtask = 0;
			GO.task = task_index;
		}
		break;
	}
}

void debug_test(u8 task, u8 task_index)
{
//	TaskThread_State = taskthread_debug; // ����debug ģʽ �����������Զ���ת
	switch (task)
	{
	case 1: // GE task
//		if (task_index > GE_BOx_Out)
//		{
//			GE.sta = Ready;
//			GE.subtask = 0;
//			GE.task = task_index;
//		}
		break;
	case 2:
		break;
	case 3:
		break;
	case 4://GO task
		if(task_index>GO_Box_Out)
		{
			debug_flag=TRUE;
			GO.sta=Ready;
			GO.subtask=0;
			GO.task=task_index;
		}
		break;
	default:
		break;
	}
}

void thread_test(u8 task)
{
	TaskThread_State=taskthread_ready;
	switch(task)
	{
		case 1:
		GE.sta=Ready;
		GE.subtask=0;
		GE.task=GE_start;
		break;
		case 2:
		GC.sta=Ready;
		GC.subtask=0;
		GC.task=GC_start;
		break;
		case 3:
		break;
		case 4:
		GO.sta=0;
		GO.subtask=0;
		GO.task=GO_start;
		break;
	}

}

// �������б��ʼ��(�û��Լ����)
// �û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[] =
	{
#if USMART_USE_WRFUNS == 1 // ���ʹ���˶�д����
		(void *)read_addr,
		"u32 read_addr(u32 addr)",
		(void *)write_addr,
		"void write_addr(u32 addr,u32 val)",
#endif
		(void *)delay_ms,
		"void delay_ms(u16 nms)",
		(void *)delay_us,
		"void delay_us(u32 nus)",
		(void *)Set_motor_dir,
		"void Set_motor_dir(u8 dir)",
		(void *)gpio_test,
		"void gpio_test(u8 state,u8 pin)",
		(void *)close_device,
		"void close_device(void)",
		(void *)boost_device_test,
		"void boost_device_test(u8 task)",
		(void *)motor_test,
		"void motor_test(u8 dir,u8 motor,u32 pulse,u32 freq)",
		(void *)motoracc_test,
		"void motoracc_test(u8 dir,u8 motor,u32 pulse)",
		(void *)boost_test,
		"void boost_test(u8 task,u8 task_index)",
		(void *)debug_test,
		"void debug_test(u8 task, u8 task_index)",
		(void *)thread_test,
		"void thread_test(u8 task)",
};
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// �������ƹ�������ʼ��
// �õ������ܿغ���������
// �õ�����������
struct _m_usmart_dev usmart_dev =
	{
		usmart_nametab,
		usmart_init,
		usmart_cmd_rec,
		usmart_exe,
		usmart_scan,
		sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), // ��������
		0,														   // ��������
		0,														   // ����ID
		1,														   // ������ʾ����,0,10����;1,16����
		0,														   // ��������.bitx:,0,����;1,�ַ���
		0,														   // ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
		0,														   // �����Ĳ���,��ҪPARM_LEN��0��ʼ��
};
