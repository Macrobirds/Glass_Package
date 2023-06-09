#include "Globalconfig.h"
#include "motor_array.h"


motor_struct GE_motor_struct = {
	.name = GE_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 1,
	.TIM = TIM1,
	.AccPeriodArray = Accarray_motor_GE,
	.postion = -10000,
	.pulse_1mm = 212,
	.defaultfeq = 60 * 212,
	.max_pos = 212 * 460,
	.maxfeq = 200 * 212,

};
	//.maxfeq = 200 * 212,
	//.startfeq = 30 * 212,
motor_struct GC_rot_motor_struct = {
	.name = GC_rot_motor,
	.motion = Stop,
	.dir = Front,
#ifdef GC_ROT_PACKAGE
	.FRONT = 1,
#endif
#ifdef GC_VER_PACKAGE
		.FRONT = 0,
#endif	
	.TIM = TIM5,
	.AccPeriodArray = Accarray_motor_GCR,
	.postion = -10000,
	.pulse_1mm = 1,
	.defaultfeq = 1000,
	.max_pos = 3600,
	.maxfeq = 1600,

};
	//.maxfeq = 1600,
	//.startfeq = 400,
motor_struct GC_ver_motor_struct = {
	.name = GC_ver_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 0,
	.TIM = TIM3,
	.AccPeriodArray = Accarray_motor_GCV,
	.postion = 0,
	.pulse_1mm = 211,
	.defaultfeq = 211 * 30,
	.max_pos = 36000,
	.maxfeq = 211 * 100,
};
	//.maxfeq = 211 * 100,
	//.startfeq = 211 * 30,
motor_struct GP_motor_struct = {
	.name = GP_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 0,
	.TIM = TIM8,
	.AccPeriodArray = Accarray_motor_GP,
	.postion = -10000,
	.pulse_1mm = 268,
	.defaultfeq = 268 * 30,
	.max_pos = 30000,
	.maxfeq = 268 * 100,

};
	//.maxfeq = 268 * 100,
	//.startfeq = 268 * 30,
motor_struct GO_hor_motor_struct = {
	.name = GO_hor_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 0,
	.TIM = TIM2,
	.AccPeriodArray = Accarray_motor_GOH,
	.postion = -10000,
	.pulse_1mm = 268,
	.defaultfeq = 268 * 60,
	.max_pos = 60000,
	.maxfeq = 268 * 60,

};
	//.maxfeq = 268 * 60,
	//.startfeq = 268 * 30,
motor_struct GO_ver_motor_struct = {
	.name = GO_ver_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 1,
	.TIM = TIM4,
	.AccPeriodArray = Accarray_motor_GOV,
	.postion = -100000,
	.pulse_1mm = 800,
	.defaultfeq = 800 * 20,
	.max_pos = 800 * 260,
	.maxfeq = 800 * 30,
};
	//.maxfeq = 800 * 30,
	//.startfeq = 800 * 10,

#ifdef BIG_CYLINDER_MOTOR
motor_struct GP_motor_big_cyl={
	.name=GP_big_cyl_motor,
	.motion=Stop,
	.FRONT=0,
	.TIM=TIM3,
	.AccPeriodArray=NULL,
	.postion = 0,
	.pulse_1mm = 0,
	.defaultfeq = 0,
	.max_pos = 0,
	
};

#endif

void motor_parameter_Init(void)
{
		if(TaskThread_State==taskthread_boost)
	{
	// set GE motor parm
	GE_motor_struct.defaultfeq = Global_Parm.MOT->GE_speed * GE_motor_struct.pulse_1mm;
	GE_motor_struct.curvature = 2;
	GE_motor_struct.t_m = (GE_motor_struct.timerfeq / GE_motor_struct.defaultfeq);
	GE_motor_struct.accStepNumber = GE_motor_struct.pulse_1mm * 10;
	GE_motor_struct.FRONT=Global_Parm.MOT->GE_dir;

	// set GCV motor parm

	GC_ver_motor_struct.defaultfeq = Global_Parm.MOT->GCV_speed * GC_ver_motor_struct.pulse_1mm;
	GC_ver_motor_struct.curvature = 2;
	GC_ver_motor_struct.t_m = (GC_ver_motor_struct.timerfeq / GC_ver_motor_struct.defaultfeq);
	GC_ver_motor_struct.accStepNumber = GC_ver_motor_struct.pulse_1mm * 10;
	GC_ver_motor_struct.FRONT=Global_Parm.MOT->GC_ver_dir;

	// set GCR motor parm
	GC_rot_motor_struct.pulse_1mm = 1; // unit be pulse
	GC_rot_motor_struct.defaultfeq = Global_Parm.MOT->GCR_speed;
	GC_rot_motor_struct.curvature = 2;
	GC_rot_motor_struct.t_m = (GC_rot_motor_struct.timerfeq / GC_rot_motor_struct.defaultfeq);
	GC_rot_motor_struct.accStepNumber = GC_rot_motor_struct.pulse_1mm * 100;
	GC_rot_motor_struct.FRONT=Global_Parm.MOT->GC_rot_dir;

	// set GP motor parm
	GP_motor_struct.defaultfeq = Global_Parm.MOT->GP_speed * GP_motor_struct.pulse_1mm;
	GP_motor_struct.curvature = 2;
	GP_motor_struct.t_m = (GP_motor_struct.timerfeq / GP_motor_struct.defaultfeq);
	GP_motor_struct.accStepNumber = GP_motor_struct.pulse_1mm * 10;
	GP_motor_struct.FRONT=Global_Parm.MOT->GP_dir;

	// set GOH motor parm
	GO_hor_motor_struct.defaultfeq = Global_Parm.MOT->GOH_speed*GO_hor_motor_struct.pulse_1mm;
	GO_hor_motor_struct.curvature = 2;
	GO_hor_motor_struct.t_m = (GO_hor_motor_struct.timerfeq / GO_hor_motor_struct.defaultfeq);
	GO_hor_motor_struct.accStepNumber = GO_hor_motor_struct.pulse_1mm * 10;
	GO_hor_motor_struct.FRONT=Global_Parm.MOT->GOH_dir;

	// set GOV motor parm
	GO_ver_motor_struct.defaultfeq = Global_Parm.MOT->GOV_speed * GO_ver_motor_struct.pulse_1mm;
	GO_ver_motor_struct.curvature = 2;
	GO_ver_motor_struct.t_m = (GO_ver_motor_struct.timerfeq / GO_ver_motor_struct.defaultfeq);
	GO_ver_motor_struct.accStepNumber = GO_ver_motor_struct.pulse_1mm * 5;
	GO_ver_motor_struct.FRONT=Global_Parm.MOT->GOV_dir;
	}
	
}

// 步数，第几步，定时器时钟频率，启动频率，目标频率，曲线系数
unsigned short GetStepPeriodOnAcc(float len, u16 i, float pwm_clock, float fre_min, float fre_max, float flexible)
{
	float deno;
	float melo;
	float fre;
	unsigned short period;
	float delt = fre_max - fre_min;
	melo = flexible * (i - len / 2) / (len / 2);
	deno = 1.0 / (1 + expf(-melo));
	fre = delt * deno + fre_min;
	period = (unsigned short)(pwm_clock / fre);
	return period;
}

// 步数，第几步，定时器时钟频率，启动频率，目标频率，曲线系数
unsigned short GetStepPeriodOnDec(float len, u16 i, float pwm_clock, float fre_max, float fre_min, float flexible)
{
	float deno;
	float melo;
	float fre;
	unsigned short period;
	float delt = fre_max - fre_min;
	melo = flexible * (i - len / 2) / (len / 2);
	deno = 1.0 / (1 + expf(-melo));
	fre = fre_max - delt * deno;
	period = (unsigned short)(pwm_clock / fre);
	return period;
}

/*	获取上升曲线的周期数组。
 *   计算上升曲线的频率值，转换为PWM的周期值，用在定时器定时中断期间，将周期值填充到PWM周期寄存器中.
 *   len: 加速过程的步数，类型为浮点数，int可能会发生传输错误。
 *   i: 获取在len步中的第i步频率。
 *		pwm_clock: 定时器频率
 *   fre_max: 最大频率	函数最大频率72000000，驱动器最大频率100000，参考程序最大频率40000
 *   fre_min: 最小频率.  mind : 1000000/65535 = 15.259021 频率必须大于16.
 *   flexible: 曲率，用于调整曲线. 通常值4-6，越大代表加速度越大，坡道越陡，越小越接近匀加速
 */
// 周期数组，步数，定时器时钟频率，启动频率，目标频率，曲线系数
void GetStepPeriodArrayOnAcc(unsigned short period[], float len, float pwm_clock, float fre_min, float fre_max, float flexible)
{
	int i = 0;
	float deno;
	float melo;
	float fre;
	float delt = fre_max - fre_min;
	for (i = 0; i < len; i++)
	{
		melo = flexible * (i - len / 2) / (len / 2);
		deno = 1.0 / (1 + expf(-melo));
		fre = delt * deno + fre_min;
		period[i] = (unsigned short)(pwm_clock / fre);
	}
}

// 周期数组，步数，定时器时钟频率，启动频率，目标频率，曲线系数
void GetStepPeriodArrayOnDec(unsigned short period[], float len, float pwm_clock, float fre_max, float fre_min, float flexible)
{
	int i = 0;
	float deno;
	float melo;
	float fre;
	float delt = fre_max - fre_min;
	for (; i < len; i++)
	{
		melo = flexible * (i - len / 2) / (len / 2);
		deno = 1.0 / (1 + expf(-melo));
		fre = fre_max - delt * deno;
		period[i] = (unsigned short)(pwm_clock / fre);
	}
}

// 获取对应电机的轴原点传感器状态
u8 motor_Get_Start_Sen(enum motor_index motor_name)
{
	switch (motor_name)
	{
	case GE_motor:
		return GE_start_Sen;
		break;
	case GC_rot_motor:
		return (!GC_rot_Sen);
		break;
	case GC_ver_motor:
		return GC_ver_Sen;
		break;
	case GP_motor:
		return GP_start_Sen;
		break;
	case GO_hor_motor:
		return GOH_start_Sen;
		break;
	case GO_ver_motor:
		return GOV_start_Sen;
		break;
	case GP_big_cyl_motor:
		return GP_big_cyl_Sen;
	default:
		break;
	}
	return 0;
}

// 设置对应电机的方向
void motor_Set_Direction(motor_struct *motor)
{
	u8 direction = 0;
	if (motor->dir == Front)
	{
		direction = motor->FRONT;
	}
	else
	{
		direction = !motor->FRONT;
	}
	switch (motor->name)
	{
	case GE_motor:
		GE_DIR = direction;
		break;
	case GC_rot_motor:
		GCR_DIR = direction;
		break;
	case GC_ver_motor:
		GCV_DIR = direction;
		break;
	case GP_motor:
		GP_DIR = direction;
		break;
	case GO_hor_motor:
		GOH_DIR = direction;
		break;
	case GO_ver_motor:
		GOV_DIR = direction;
		break;
	case GP_big_cyl_motor:
		GP_CYL_DIR=direction;
	break;
	default:
		break;
	}
}

void stepperMotorStop(motor_struct *motor)
{
	#ifdef BIG_CYLINDER_MOTOR
	if(motor->TIM==TIM3)
	{
		if(motor->name==GC_ver_motor)
		{
			TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
			GCV_motor_Break=GAS_DISABLE;
		}else if(motor->name==GP_big_cyl_motor)
		{
			TIM_ITConfig(TIM3,TIM_IT_CC3,DISABLE);
		}
	}else
	{
		TIM_Cmd(motor->TIM, DISABLE);
	}

	motor->motion = Stop;
	#else	
	TIM_Cmd(motor->TIM, DISABLE);
	motor->motion = Stop;
	if (motor->name == GC_ver_motor)
	{
		GCV_motor_Break = GAS_DISABLE; // 关闭刹车
	}
	#endif
}

void stepperMotorStart(motor_struct *motor, u16 arr)
{
	
#ifdef BIG_CYLINDER_MOTOR
	
	switch (motor->name)
	{
	// TIM1 TIM8
	case GE_motor:
	case GP_motor:
		TIM_SetAutoreload(motor->TIM, arr);
		TIM_SetCompare1(motor->TIM, arr / 2);
		TIM_Cmd(motor->TIM, ENABLE);
		break;
	// TIM2-5
	case GC_rot_motor:
	case GO_hor_motor:
	case GO_ver_motor:
		TIM_SetCompare1(motor->TIM, (motor->TIM->CNT + arr) % 0xffff);
		TIM_Cmd(motor->TIM, ENABLE);
		break;
	case GC_ver_motor:
		GCV_motor_Break = GAS_ENABLE; // 打开刹车
		my_delay_ms(100);
		TIM_SetCompare1(motor->TIM, (motor->TIM->CNT + arr) % 0xffff);
		TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
		break;
	case GP_big_cyl_motor:
		TIM_SetCompare3(motor->TIM, (motor->TIM->CNT + arr) % 0xffff);
		TIM_ITConfig(TIM3,TIM_IT_CC3,ENABLE);
	default:
		break;
	}
	
#else	
	
	switch (motor->name)
	{
	// TIM1 TIM8
	case GE_motor:
	case GP_motor:
		TIM_SetAutoreload(motor->TIM, arr);
		TIM_SetCompare1(motor->TIM, arr / 2);
		break;
	// TIM2-5
	case GC_rot_motor:
	case GO_hor_motor:
	case GO_ver_motor:
		TIM_SetCompare1(motor->TIM, (motor->TIM->CNT + arr) % 0xffff);
		break;
	case GC_ver_motor:
		GCV_motor_Break = GAS_ENABLE; // 打开刹车
		my_delay_ms(100);
		TIM_SetCompare1(motor->TIM, (motor->TIM->CNT + arr) % 0xffff);
		break;
	default:
		break;
	}
	TIM_Cmd(motor->TIM, ENABLE);
	
#endif
}



// 匀速控制电机运行 电机结构体 电机状态 匀速运动频率 当速度频率为0时用默认匀速运动频率
void motorGo(motor_struct *motor, long planPosition, u32 freq)
{
	enum motor_direction OldDirection = Front;
	long planStepNumber = 0;
	if (TaskThread_State == taskthread_emergency) // 暂停状态 电机不启动
	{
		return;
	}

	if (planPosition > 0) // 目标位置不在原点
	{
		motor->planpostion = planPosition;
		planStepNumber = planPosition - motor->postion;
	}
	else // 目标位置在原点
	{
		motor->planpostion = 0;
		if (motor_Get_Start_Sen(motor->name) == Sen_Block) // 原点传感器感应 位置校准成功 直接返回
		{
			stepperMotorStop(motor);
			motor->postion = 0;
			motor->planSetpNumber = 0;
			return;
		}
		else // 位置未初始化成功
		{
			motor->postion = motor->max_pos - 100;
			planStepNumber = planPosition - motor->postion;
		}
	}
	OldDirection = motor->dir;
	if (planStepNumber > 0)
	{
		motor->dir = Front;
		motor_Set_Direction(motor);
	}
	else if (planStepNumber < 0)
	{
		motor->dir = Back;
		motor_Set_Direction(motor);
		planStepNumber *= -1;
	}
	else
	{
		stepperMotorStop(motor);
		return;
	}
	if (OldDirection != motor->dir)
	{
		my_delay_us(5);
	}
	motor->motion = ConstMove;				// 标记运动状态为匀速运动
	motor->planSetpNumber = planStepNumber; // 设置目标步数
	motor->step = 0;						// 设置当前步数
	if (freq > 16)
	{
		motor->t_m = motor->timerfeq / freq; // 设置目标计数值设定频率
	}
	else
	{
		if (motor->defaultfeq > 16)
		{
			motor->t_m = motor->timerfeq / motor->defaultfeq; // 设置目标计数值为默认频率
		}
		else
			return;
	}
	stepperMotorStart(motor, motor->t_m);
}

// 设置混合参数  约15mm
void setMixtureData(motor_struct *motor)
{
//	if (motor->AccPeriodArray != NULL)
//	{
//		myfree(SRAMIN, motor->AccPeriodArray);
//	}

	if (motor->planSetpNumber >= motor->pulse_1mm * 20)
	{
//		motor->AccPeriodArray = mymalloc(SRAMIN, motor->accStepNumber * 2);
		motor->t_m = motor->timerfeq / motor->maxfeq;
//		GetStepPeriodArrayOnAcc(motor->AccPeriodArray, motor->accStepNumber, motor->timerfeq, motor->startfeq, motor->maxfeq, motor->curvature);
		motor->motion = AccMove; // 设置电机运动状态为加减速状态
		printf("acc move\r\n");
	}
	else
	{
		motor->motion = ConstMove; // 设置电机运动状态为匀速状态
		motor->t_m = motor->timerfeq / motor->defaultfeq;
		printf("const move\r\n");
	}
}

// 变速控制电机运行
u8 motorGo_acc(motor_struct *motor, long planPosition)
{
	enum motor_direction OldDirection = Front;
	long planStepNumber = 0;

	if (TaskThread_State == taskthread_emergency) // 暂停状态 电机不启动
	{
		return FALSE;
	}
	if (motor->motion != Stop) // 电机不是处于静止状态来启动
	{
		return FALSE;
	}

	OldDirection = motor->dir;
	if (planPosition > 0)
	{
		motor->planpostion = planPosition;
		planStepNumber = planPosition - motor->postion;
	}
	else
	{
		motor->planpostion = 0;
		if (motor_Get_Start_Sen(motor->name) == Sen_Block)
		{
			stepperMotorStop(motor);
			motor->postion = 0;
			motor->planSetpNumber = 0;
			return TRUE;
		}
		else
		{
			planStepNumber = planPosition - motor->postion;
		}
	}
	if (planStepNumber > 0)
	{
		motor->dir = Front;
		motor_Set_Direction(motor);
	}
	else if (planStepNumber < 0)
	{
		motor->dir = Back;
		motor_Set_Direction(motor);
		planStepNumber *= -1;
	}
	else
	{
		stepperMotorStop(motor);
		return TRUE;
	}

	motor->planSetpNumber = planStepNumber;
	motor->step = 0;
	if (OldDirection != motor->dir)
	{
		my_delay_us(5);
	}
	setMixtureData(motor);
	stepperMotorStart(motor, motor->AccPeriodArray[0]);
	return TRUE;
}

// 走指定脉冲的相对距离  freq=0 速度位defaultfeq
void motorGO_Debug(motor_struct *motor, u32 pulse, u32 freq)
{
	if (!pulse)
		return;
	motor->motion = ConstMove;
	motor->planSetpNumber = pulse;
	motor->step = 0;
	if (freq > 16)
	{
		motor->t_m = motor->timerfeq / freq;
	}
	else
	{
		if (motor->defaultfeq > 16)
		{
			motor->t_m = motor->timerfeq / motor->defaultfeq;
		}
		else
			return;
	}
	stepperMotorStart(motor, motor->t_m);
}

void motorAccGO_Debug(motor_struct *motor, u32 pulse)
{
	motor->motion = AccMove;
	motor->planSetpNumber = pulse;
	motor->step = 0;
	setMixtureData(motor);
	stepperMotorStart(motor, motor->t_m);
}

