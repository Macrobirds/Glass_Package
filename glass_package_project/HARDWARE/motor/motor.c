#include "motor.h"
#include "math.h"
#include "Globalconfig.h"
#include "position.h"
#include "mydelay.h"
#include "malloc.h"



motor_struct GE_motor_struct={
	.name=GE_motor,
	.motion=Stop,
	.dir=Front,
	.FRONT=1,
	.TIM=TIM1,
	.AccPeriodArray=NULL,
	.postion=-1, //开机上电时位置未知
	
};
motor_struct GC_rot_motor_struct={
	.name=GC_rot_motor,
	.motion=Stop,
	.dir=Front,
	.FRONT=1,
	.TIM=TIM5,
	.AccPeriodArray=NULL,
	.postion=-1,	
};
motor_struct GC_ver_motor_struct={
	.name=GC_ver_motor,
	.motion=Stop,
	.dir=Front,
	.FRONT=1,
	.TIM=TIM3,
	.AccPeriodArray=NULL,
	.postion=-1,
};
motor_struct GP_motor_struct={
	.name=GP_motor,
	.motion=Stop,
	.dir=Front,
	.FRONT=1,
	.TIM=TIM8,
	.AccPeriodArray=NULL,
	.postion=-1,
};
motor_struct GO_hor_motor_struct={
	.name=GO_hor_motor,
	.motion=Stop,
	.dir=Front,
	.FRONT=1,
	.TIM=TIM2,
	.AccPeriodArray=NULL,
	.postion=-1,
};
motor_struct GO_ver_motor_struct={
	.name=GO_ver_motor,
	.motion=Stop,
	.dir=Front,
	.FRONT=1,
	.TIM=TIM4,
	.AccPeriodArray=NULL,
	.postion=-1,
};

void motor_parameter_Init(void)
{
	//set GE motor parm
	GE_motor_struct.pulse_1mm=Global_Parm.MOT.GE_1mm;
	GE_motor_struct.maxfeq=Global_Parm.MOT.GE_max_speed*GE_motor_struct.pulse_1mm;
	GE_motor_struct.startfeq=Global_Parm.MOT.GE_min_speed*GE_motor_struct.pulse_1mm;
	GE_motor_struct.max_pos=Global_Parm.GIO.GE_max_pos;
	GE_motor_struct.defaultfeq=GE_motor_struct.pulse_1mm*30;
	GE_motor_struct.curvature=2;
	GE_motor_struct.t_m=(GE_motor_struct.timerfeq/GE_motor_struct.defaultfeq);
	GE_motor_struct.accStepNumber=GE_motor_struct.pulse_1mm*10;
	
	//set GCV motor parm
	GC_ver_motor_struct.pulse_1mm=Global_Parm.MOT.GCV_1mm;
	GC_ver_motor_struct.maxfeq=Global_Parm.MOT.GCV_max_speed*GC_ver_motor_struct.pulse_1mm;
	GC_ver_motor_struct.startfeq=Global_Parm.MOT.GCV_min_speed*GC_ver_motor_struct.pulse_1mm;
	GC_ver_motor_struct.max_pos=Global_Parm.GCS.GCV_max_pos;
	GC_ver_motor_struct.defaultfeq=GC_ver_motor_struct.pulse_1mm*30;
	GC_ver_motor_struct.curvature=2;
	GC_ver_motor_struct.t_m=(GC_ver_motor_struct.timerfeq/GC_ver_motor_struct.defaultfeq);
	GC_ver_motor_struct.accStepNumber=GC_ver_motor_struct.pulse_1mm*10;
	
	//set GCR motor parm
	GC_rot_motor_struct.pulse_1mm=1; //unit be angel
	GC_rot_motor_struct.maxfeq=Global_Parm.MOT.GCR_max_speed;
	GC_rot_motor_struct.startfeq=Global_Parm.MOT.GCR_min_speed;
	GC_rot_motor_struct.max_pos=Global_Parm.GCS.GCR_max_pos;
	GC_rot_motor_struct.defaultfeq=200;
	GC_rot_motor_struct.curvature=1;
	GC_rot_motor_struct.t_m=(GC_rot_motor_struct.timerfeq/GC_rot_motor_struct.defaultfeq);
	GC_rot_motor_struct.accStepNumber=GC_rot_motor_struct.pulse_1mm*10;
	
	//set GP motor parm
	GP_motor_struct.pulse_1mm=Global_Parm.MOT.GP_1mm;
	GP_motor_struct.maxfeq=Global_Parm.MOT.GP_max_speed*GP_motor_struct.pulse_1mm;
	GP_motor_struct.startfeq=Global_Parm.MOT.GP_min_speed*GP_motor_struct.pulse_1mm;
	GP_motor_struct.defaultfeq=GP_motor_struct.pulse_1mm*30;
	GP_motor_struct.max_pos=Global_Parm.GP.max_pos;
	GP_motor_struct.curvature=2;
	GP_motor_struct.t_m=(GP_motor_struct.timerfeq/GP_motor_struct.defaultfeq);
	GP_motor_struct.accStepNumber=GP_motor_struct.pulse_1mm*10;
	
	
	//set GOH motor parm
	GO_hor_motor_struct.pulse_1mm=Global_Parm.MOT.GOH_1mm;
	GO_hor_motor_struct.maxfeq=Global_Parm.MOT.GOH_max_speed;
	GO_hor_motor_struct.startfeq=Global_Parm.MOT.GOH_min_speed;
	GO_hor_motor_struct.defaultfeq=GO_hor_motor_struct.pulse_1mm*30;
	GO_hor_motor_struct.max_pos=Global_Parm.GCS.GOH_max_pos;
	GO_hor_motor_struct.curvature=2;
	GO_hor_motor_struct.t_m=(GO_hor_motor_struct.timerfeq/GO_hor_motor_struct.defaultfeq);
	GO_hor_motor_struct.accStepNumber=GO_hor_motor_struct.pulse_1mm*10;
	
	//set GOV motor parm
	GO_ver_motor_struct.pulse_1mm=Global_Parm.MOT.GOV_1mm;
	GO_ver_motor_struct.maxfeq=Global_Parm.MOT.GOV_max_speed*GO_ver_motor_struct.pulse_1mm;
	GO_ver_motor_struct.startfeq=Global_Parm.MOT.GOV_min_speed*GO_ver_motor_struct.pulse_1mm;
	GO_ver_motor_struct.defaultfeq=GO_ver_motor_struct.pulse_1mm*30;
	GO_ver_motor_struct.max_pos=Global_Parm.GIO.GOV_max_pos;
	GO_ver_motor_struct.curvature=2;
	GO_ver_motor_struct.t_m=(GO_ver_motor_struct.timerfeq/GO_ver_motor_struct.defaultfeq);
	GO_ver_motor_struct.accStepNumber=GO_ver_motor_struct.pulse_1mm*10;

}

// 步数，第几步，定时器时钟频率，启动频率，目标频率，曲线系数
unsigned short GetStepPeriodOnAcc(float len, u16 i, float pwm_clock, float fre_min, float fre_max,  float flexible)
{
  float deno ;
  float melo ;
	float fre	 ;
	unsigned short period;
  float delt = fre_max-fre_min;
	melo = flexible * (i-len/2) / (len/2);
	deno = 1.0 / (1 + expf(-melo)); 
	fre = delt * deno + fre_min;
	period = (unsigned short)(pwm_clock / fre); 
  return period;
}

// 步数，第几步，定时器时钟频率，启动频率，目标频率，曲线系数
unsigned short GetStepPeriodOnDec(float len, u16 i, float pwm_clock, float fre_max, float fre_min,  float flexible)
{
  float deno ;
  float melo ;
	float fre	 ;
	unsigned short period;
  float delt = fre_max-fre_min;
	melo = flexible * (i-len/2) / (len/2);
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
void GetStepPeriodArrayOnAcc(unsigned short period[], float len,  float pwm_clock, float fre_min, float fre_max,  float flexible)
{
    int i=0;
    float deno ;
    float melo ;
		float fre	 ;
    float delt = fre_max-fre_min;
    for(i=0; i<len; i++)
    {
        melo = flexible * (i-len/2) / (len/2);
        deno = 1.0 / (1 + expf(-melo)); 
        fre = delt * deno + fre_min;
        period[i] = (unsigned short)(pwm_clock / fre); 
    }
}

// 周期数组，步数，定时器时钟频率，启动频率，目标频率，曲线系数
void GetStepPeriodArrayOnDec(unsigned short period[], float len,  float pwm_clock, float fre_max, float fre_min,  float flexible)
{
    int i=0;
    float deno ;
    float melo ;
		float fre	 ;
    float delt = fre_max-fre_min;
    for(; i<len; i++)
    {
        melo = flexible * (i-len/2) / (len/2);
        deno = 1.0 / (1 + expf(-melo)); 
        fre = fre_max - delt * deno ;
        period[i] = (unsigned short)(pwm_clock / fre); 
    }
}



//获取对应电机的轴原点传感器状态
u8 motor_Get_Start_Sen(enum motor_index motor_name)
{
	switch(motor_name)
	{
		case GE_motor:
		return GE_start_Sen;
		break;
		case GC_rot_motor:
		return GC_rot_Sen;	
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
		default: break;
	}
}


//设置对应电机的方向
void motor_Set_Direction(motor_struct * motor,enum motor_direction dir)
{
	 u8 direction=0;
	 if(dir==Front)
	 {
		direction=motor->FRONT;
	 }
	 else
	 {
		 direction=!motor->FRONT;
	 }
		switch(motor->name)
	{
		case GE_motor:
		GE_DIR=direction;
		break;
		case GC_rot_motor:
		GCR_DIR=direction;
		break;
		case GC_ver_motor:
		GCV_DIR=direction;
		break;
		case GP_motor:
		GP_DIR=direction;
		break;
		case GO_hor_motor:
		GOH_DIR=direction;
		break;
		case GO_ver_motor:
		GOV_DIR=direction;
		break;
		default: break;
	}
}

void stepperMotorStop(motor_struct * motor)
{
	TIM_Cmd(motor->TIM,DISABLE);
	motor->motion=Stop;
}

void stepperMotorStart(motor_struct * motor,u16 arr)
{
		switch(motor->name)
		{
			//TIM1 TIM8
			case GE_motor:
			case GP_motor:
			TIM_SetAutoreload(motor->TIM,arr);
			TIM_SetCompare1(motor->TIM,arr/2);
			break;
			//TIM2-5
			case GC_ver_motor:
			case GC_rot_motor: 
			case GO_hor_motor:
			case GO_ver_motor:
			TIM_SetCompare1(motor->TIM,(motor->TIM->CNT+arr)%0xffff);
			break;
			default: break;
		}
		TIM_Cmd(motor->TIM,ENABLE);
}

// 匀速控制电机运行 电机结构体 电机状态 匀速运动频率 当速度频率为0时用默认匀速运动频率
void motorGo(motor_struct * motor, long planPosition,u32 freq)
{
	enum motor_direction OldDirection=Front;
	long planStepNumber=0;
	if(TaskThread_State==taskthread_pause) //暂停状态 电机不启动
	{
		return;
	}
	
	if(planPosition>0) //目标位置不在原点
	{
		motor->planpostion=planPosition;
		planStepNumber=planPosition-motor->postion;
		
	}
	else//目标位置在原点
	{ 
		motor->planpostion=0;
		if(motor_Get_Start_Sen(motor->name)==Sen_Block)  //原点传感器感应 位置校准成功 直接返回
		{
			stepperMotorStop(motor);
			motor->postion=0;
			motor->planSetpNumber=0;
			return;
		}
		else//位置未初始化成功
		{
			if(motor->postion<=0) motor->postion=motor->max_pos; 
			planStepNumber=planPosition-motor->postion;
		}
	
	}
		OldDirection=motor->dir;
		if(planStepNumber>0)
		{
			motor->dir=Front;
			motor_Set_Direction(motor,Front);
		}
		else if(planStepNumber<0)
		{
			motor->dir=Back;
			motor_Set_Direction(motor,Back);
			planStepNumber*=-1;
		}
		else
		{
			stepperMotorStop(motor);
			return;
		}
		if(OldDirection!=motor->dir)
		{
			my_delay_us(5);
		}
		motor->motion=ConstMove; //标记运动状态为匀速运动
		motor->planSetpNumber=planStepNumber; //设置目标步数
		motor->step=0; //设置当前步数
		if(freq) 
		{
			motor->t_m=motor->timerfeq/freq;//设置目标计数值设定频率
		}else 
		{
			motor->t_m=motor->timerfeq/motor->defaultfeq;//设置目标计数值为默认频率
		}
		stepperMotorStart(motor,motor->t_m);
		
}




// 设置混合参数  约15mm
void setMixtureData(motor_struct * motor)
{
	if(motor->AccPeriodArray!=NULL)
	{
		myfree(SRAMIN,motor->AccPeriodArray);
	}
	
	if(motor->planSetpNumber>=motor->pulse_1mm*20)
	{
		motor->AccPeriodArray=mymalloc(SRAMIN,motor->accStepNumber*2);
		motor->t_m=motor->timerfeq/motor->maxfeq;
		GetStepPeriodArrayOnAcc(motor->AccPeriodArray,motor->accStepNumber,motor->timerfeq,motor->startfeq,motor->maxfeq,motor->curvature);
		motor->motion=AccMove; //设置电机运动状态为加减速状态
	}
	else
	{
		motor->motion=ConstMove;//设置电机运动状态为匀速状态
		motor->t_m=motor->timerfeq/motor->defaultfeq;
	}
	

}


// 变速控制电机运行
u8 motorGo_acc(motor_struct * motor, long planPosition)
{
	enum motor_direction OldDirection=Front;
	long planStepNumber=0;
	
	if(TaskThread_State==taskthread_pause) //暂停状态 电机不启动
	{
		return FALSE;
	}

	OldDirection=motor->dir;
	if(planPosition>0)
	{
		motor->planpostion=planPosition;
		planStepNumber=planPosition-motor->postion;
	}
	else
	{
		motor->planpostion=0;
		if(motor_Get_Start_Sen(motor->name)==Sen_Block)
		{
			stepperMotorStop(motor);
			motor->postion=0;
			motor->planSetpNumber=0;
			return TRUE;
		}
		else
		{
			if(motor->postion<=0) motor->postion=motor->max_pos;
			planStepNumber=planPosition-motor->postion;
		}
	}
	if(planStepNumber>0)
	{
		motor->dir=Front;
		motor_Set_Direction(motor,Front);
	}
	else if(planStepNumber<0)
	{
		motor->dir=Back;
		motor_Set_Direction(motor,Back);
		planStepNumber*=-1;
	}
	else
	{
		stepperMotorStop(motor);
		return TRUE;
	}
	
	motor->planSetpNumber=planStepNumber;
	motor->step=0;
	if(OldDirection!=motor->dir)
	{
			my_delay_us(5);
	}
	setMixtureData(motor);
	if(motor->AccPeriodArray!=NULL)
	{

		stepperMotorStart(motor,motor->AccPeriodArray[0]);
		
	}else
	{
		motor->motion=Stop;
		return FALSE;
	}

	return TRUE;
}
