#include "usmart.h"
#include "usmart_str.h"

////////////////////////////用户配置区///////////////////////////////////////////////
// 这下面要包含所用到的函数所申明的头文件(用户自己添加)
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
//	TaskThread_State = taskthread_debug; // 进入debug 模式 不允许任务自动跳转
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

// 函数名列表初始化(用户自己添加)
// 用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[] =
	{
#if USMART_USE_WRFUNS == 1 // 如果使能了读写操作
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
// 函数控制管理器初始化
// 得到各个受控函数的名字
// 得到函数总数量
struct _m_usmart_dev usmart_dev =
	{
		usmart_nametab,
		usmart_init,
		usmart_cmd_rec,
		usmart_exe,
		usmart_scan,
		sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), // 函数数量
		0,														   // 参数数量
		0,														   // 函数ID
		1,														   // 参数显示类型,0,10进制;1,16进制
		0,														   // 参数类型.bitx:,0,数字;1,字符串
		0,														   // 每个参数的长度暂存表,需要MAX_PARM个0初始化
		0,														   // 函数的参数,需要PARM_LEN个0初始化
};
