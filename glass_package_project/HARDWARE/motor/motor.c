#include "Globalconfig.h"

#define SCALE 10

motor_struct GE_motor_struct = {
	.name = GE_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 1,
	.TIM = TIM1,
	.AccPeriodArray = NULL,
	.postion = -10000,
	.pulse_1mm = 212,
	.maxfeq = 200 * 212,
	.startfeq = 30 * 212,
	.defaultfeq = 60 * 212,
	.max_pos = 212 * 460,

};

motor_struct GC_rot_motor_struct = {
	.name = GC_rot_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 1,
	.TIM = TIM5,
	.AccPeriodArray = NULL,
	.postion = -10000,
	.pulse_1mm = 1,
	.maxfeq = 1600,
	.startfeq = 400,
	.defaultfeq = 600,
	.max_pos = 2400,

};

motor_struct GC_ver_motor_struct = {
	.name = GC_ver_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 0,
	.TIM = TIM3,
	.AccPeriodArray = NULL,
	.postion = 0,
	.pulse_1mm = 211,
	.maxfeq = 211 * 100,
	.startfeq = 211 * 30,
	.defaultfeq = 211 * 30,
	.max_pos = 36000,

};

motor_struct GP_motor_struct = {
	.name = GP_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 0,
	.TIM = TIM8,
	.AccPeriodArray = NULL,
	.postion = -10000,
	.pulse_1mm = 268,
	.maxfeq = 268 * 100,
	.startfeq = 268 * 30,
	.defaultfeq = 268 * 30,
	.max_pos = 30000,

};
motor_struct GO_hor_motor_struct = {
	.name = GO_hor_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 0,
	.TIM = TIM2,
	.AccPeriodArray = NULL,
	.postion = -10000,
	.pulse_1mm = 268,
	.maxfeq = 268 * 100,
	.startfeq = 268 * 30,
	.defaultfeq = 268 * 60,
	.max_pos = 60000,

};
motor_struct GO_ver_motor_struct = {
	.name = GO_ver_motor,
	.motion = Stop,
	.dir = Front,
	.FRONT = 1,
	.TIM = TIM4,
	.AccPeriodArray = NULL,
	.postion = -100000,
	.pulse_1mm = 800,
	.maxfeq = 800 * 30,
	.startfeq = 800 * 10,
	.defaultfeq = 800 * 20,
	.max_pos = 800 * 260,
};


#ifdef BIG_CYLINDER_MOTOR
motor_struct GP_motor_big_cyl={
	.name=GP_big_cyl_motor,
	.motion=Stop,
	.FRONT=0,
	.TIM=TIM3,
	.AccPeriodArray=NULL,
	.postion = 0,
	.pulse_1mm = 0,
	.maxfeq = 0,
	.startfeq = 0,
	.defaultfeq = 0,
	.max_pos = 0,
	
};

#endif

void motor_parameter_Init(void)
{
	// set GE motor parm

	GE_motor_struct.maxfeq = Global_Parm.MOT.GE_max_speed * GE_motor_struct.pulse_1mm/SCALE;
	GE_motor_struct.startfeq = Global_Parm.MOT.GE_min_speed * GE_motor_struct.pulse_1mm/SCALE;
	GE_motor_struct.curvature = 2;
	GE_motor_struct.t_m = (GE_motor_struct.timerfeq / GE_motor_struct.defaultfeq);
	GE_motor_struct.accStepNumber = GE_motor_struct.pulse_1mm * 10;

	// set GCV motor parm

	GC_ver_motor_struct.maxfeq = Global_Parm.MOT.GCV_max_speed * GC_ver_motor_struct.pulse_1mm/SCALE;
	GC_ver_motor_struct.startfeq = Global_Parm.MOT.GCV_min_speed * GC_ver_motor_struct.pulse_1mm/SCALE;
	GC_ver_motor_struct.curvature = 2;
	GC_ver_motor_struct.t_m = (GC_ver_motor_struct.timerfeq / GC_ver_motor_struct.defaultfeq);
	GC_ver_motor_struct.accStepNumber = GC_ver_motor_struct.pulse_1mm * 10;

	// set GCR motor parm
	GC_rot_motor_struct.pulse_1mm = 1; // unit be angel
	GC_rot_motor_struct.maxfeq = Global_Parm.MOT.GCR_max_speed;
	GC_rot_motor_struct.startfeq = Global_Parm.MOT.GCR_min_speed;
	GC_rot_motor_struct.curvature = 1;
	GC_rot_motor_struct.t_m = (GC_rot_motor_struct.timerfeq / GC_rot_motor_struct.defaultfeq);
	GC_rot_motor_struct.accStepNumber = GC_rot_motor_struct.pulse_1mm * 10;

	// set GP motor parm
	GP_motor_struct.maxfeq = Global_Parm.MOT.GP_max_speed * GP_motor_struct.pulse_1mm/SCALE;
	GP_motor_struct.startfeq = Global_Parm.MOT.GP_min_speed * GP_motor_struct.pulse_1mm/SCALE;
	GP_motor_struct.curvature = 2;
	GP_motor_struct.t_m = (GP_motor_struct.timerfeq / GP_motor_struct.defaultfeq);
	GP_motor_struct.accStepNumber = GP_motor_struct.pulse_1mm * 10;

	// set GOH motor parm
	GO_hor_motor_struct.maxfeq = Global_Parm.MOT.GOH_max_speed/SCALE;
	GO_hor_motor_struct.startfeq = Global_Parm.MOT.GOH_min_speed/SCALE;
	GO_hor_motor_struct.curvature = 2;
	GO_hor_motor_struct.t_m = (GO_hor_motor_struct.timerfeq / GO_hor_motor_struct.defaultfeq);
	GO_hor_motor_struct.accStepNumber = GO_hor_motor_struct.pulse_1mm * 10;

	// set GOV motor parm
	GO_ver_motor_struct.maxfeq = Global_Parm.MOT.GOV_max_speed * GO_ver_motor_struct.pulse_1mm/SCALE;
	GO_ver_motor_struct.startfeq = Global_Parm.MOT.GOV_min_speed * GO_ver_motor_struct.pulse_1mm/SCALE;
	GO_ver_motor_struct.curvature = 2;
	GO_ver_motor_struct.t_m = (GO_ver_motor_struct.timerfeq / GO_ver_motor_struct.defaultfeq);
	GO_ver_motor_struct.accStepNumber = GO_ver_motor_struct.pulse_1mm * 10;
}

// �������ڼ�������ʱ��ʱ��Ƶ�ʣ�����Ƶ�ʣ�Ŀ��Ƶ�ʣ�����ϵ��
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

// �������ڼ�������ʱ��ʱ��Ƶ�ʣ�����Ƶ�ʣ�Ŀ��Ƶ�ʣ�����ϵ��
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

/*	��ȡ�������ߵ��������顣
 *   �����������ߵ�Ƶ��ֵ��ת��ΪPWM������ֵ�����ڶ�ʱ����ʱ�ж��ڼ䣬������ֵ��䵽PWM���ڼĴ�����.
 *   len: ���ٹ��̵Ĳ���������Ϊ��������int���ܻᷢ���������
 *   i: ��ȡ��len���еĵ�i��Ƶ�ʡ�
 *		pwm_clock: ��ʱ��Ƶ��
 *   fre_max: ���Ƶ��	�������Ƶ��72000000�����������Ƶ��100000���ο��������Ƶ��40000
 *   fre_min: ��СƵ��.  mind : 1000000/65535 = 15.259021 Ƶ�ʱ������16.
 *   flexible: ���ʣ����ڵ�������. ͨ��ֵ4-6��Խ�������ٶ�Խ���µ�Խ����ԽСԽ�ӽ��ȼ���
 */
// �������飬��������ʱ��ʱ��Ƶ�ʣ�����Ƶ�ʣ�Ŀ��Ƶ�ʣ�����ϵ��
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

// �������飬��������ʱ��ʱ��Ƶ�ʣ�����Ƶ�ʣ�Ŀ��Ƶ�ʣ�����ϵ��
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

// ��ȡ��Ӧ�������ԭ�㴫����״̬
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

// ���ö�Ӧ����ķ���
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
		GCV_motor_Break = GAS_DISABLE; // �ر�ɲ��
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
		GCV_motor_Break = GAS_ENABLE; // ��ɲ��
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
		GCV_motor_Break = GAS_ENABLE; // ��ɲ��
		my_delay_ms(100);
		TIM_SetCompare1(motor->TIM, (motor->TIM->CNT + arr) % 0xffff);
		break;
	default:
		break;
	}
	TIM_Cmd(motor->TIM, ENABLE);
	
#endif
}



// ���ٿ��Ƶ������ ����ṹ�� ���״̬ �����˶�Ƶ�� ���ٶ�Ƶ��Ϊ0ʱ��Ĭ�������˶�Ƶ��
void motorGo(motor_struct *motor, long planPosition, u32 freq)
{
	enum motor_direction OldDirection = Front;
	long planStepNumber = 0;
	if (TaskThread_State == taskthread_emergency) // ��ͣ״̬ ���������
	{
		return;
	}

	if (planPosition > 0) // Ŀ��λ�ò���ԭ��
	{
		motor->planpostion = planPosition;
		planStepNumber = planPosition - motor->postion;
	}
	else // Ŀ��λ����ԭ��
	{
		motor->planpostion = 0;
		if (motor_Get_Start_Sen(motor->name) == Sen_Block) // ԭ�㴫������Ӧ λ��У׼�ɹ� ֱ�ӷ���
		{
			stepperMotorStop(motor);
			motor->postion = 0;
			motor->planSetpNumber = 0;
			return;
		}
		else // λ��δ��ʼ���ɹ�
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
	motor->motion = ConstMove;				// ����˶�״̬Ϊ�����˶�
	motor->planSetpNumber = planStepNumber; // ����Ŀ�경��
	motor->step = 0;						// ���õ�ǰ����
	if (freq > 16)
	{
		motor->t_m = motor->timerfeq / freq; // ����Ŀ�����ֵ�趨Ƶ��
	}
	else
	{
		if (motor->defaultfeq > 16)
		{
			motor->t_m = motor->timerfeq / motor->defaultfeq; // ����Ŀ�����ֵΪĬ��Ƶ��
		}
		else
			return;
	}
	stepperMotorStart(motor, motor->t_m);
}

// ���û�ϲ���  Լ15mm
void setMixtureData(motor_struct *motor)
{
	if (motor->AccPeriodArray != NULL)
	{
		myfree(SRAMIN, motor->AccPeriodArray);
	}

	if (motor->planSetpNumber >= motor->pulse_1mm * 20)
	{
		motor->AccPeriodArray = mymalloc(SRAMIN, motor->accStepNumber * 2);
		motor->t_m = motor->timerfeq / motor->maxfeq;
		GetStepPeriodArrayOnAcc(motor->AccPeriodArray, motor->accStepNumber, motor->timerfeq, motor->startfeq, motor->maxfeq, motor->curvature);
		motor->motion = AccMove; // ���õ���˶�״̬Ϊ�Ӽ���״̬
		printf("acc move\r\n");
	}
	else
	{
		motor->motion = ConstMove; // ���õ���˶�״̬Ϊ����״̬
		motor->t_m = motor->timerfeq / motor->defaultfeq;
		printf("const move\r\n");
	}
}

// ���ٿ��Ƶ������
u8 motorGo_acc(motor_struct *motor, long planPosition)
{
	enum motor_direction OldDirection = Front;
	long planStepNumber = 0;

	if (TaskThread_State == taskthread_emergency) // ��ͣ״̬ ���������
	{
		return FALSE;
	}
	if (motor->motion != Stop) // ������Ǵ��ھ�ֹ״̬������
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

// ��ָ���������Ծ���  freq=0 �ٶ�λdefaultfeq
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

// volatile int32_t GE_motor_struct_postion=-1;
// volatile u32 GE_motor_struct_step=0;
// volatile int32_t GC_rot_motor_struct_postion=-1;
// volatile u32 GC_rot_motor_struct_step=0;
// volatile int32_t GC_ver_motor_struct_postion=-1;
// volatile u32 GC_ver_motor_struct_step=0;
// volatile int32_t GP_motor_struct_postion=-1;
// volatile u32 GP_motor_struct_step=0;
// volatile int32_t GO_hor_motor_struct_postion=-1;
// volatile u32 GO_hor_motor_struct_step=0;
// volatile int32_t GO_ver_motor_struct_postion=-1;
// volatile u32 GO_ver_motor_struct_step=0;

// void stepperMotor_Set_planstep(motor_struct * motor,u32 planstep)
//{
//	switch(motor->name)
//	{
//		case GE_motor:
//		GE_motor_struct_step=planstep;
//		break;
//		case GC_rot_motor:
//		GC_rot_motor_struct_step=planstep;
//		break;
//		case GC_ver_motor:
//		GC_ver_motor_struct_step=planstep;
//		break;
//		case GP_motor:
//		GP_motor_struct_step=planstep;
//		break;
//		case GO_hor_motor:
//		GO_hor_motor_struct_step=planstep;
//		break;
//		case GO_ver_motor:
//		GO_ver_motor_struct_step=planstep;
//		break;
//		default: break;
//
//	}
// }
// void stepperMotor_Reset_Step(motor_struct * motor)
//{
//
// }

// volatile int32_t stepperMotor_Get_position(motor_struct *motor)
//{

//}