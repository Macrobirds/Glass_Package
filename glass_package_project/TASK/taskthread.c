#include "taskthread.h"

struct glass_enter_struct GE={
	.task=GE_none,
	.sta=Ready,
	.motor=&GE_motor_struct,
};


struct glass_claw_struct GC={
	.task=GC_none,
	.sta=Ready,
	.motor_v=&GC_ver_motor_struct,
	.motor_r=&GC_rot_motor_struct,
};


struct glass_package_struct GP={
	.task=GP_none,
	.sta=Ready,
	.motor=&GP_motor_struct,
};


struct glass_out_struct GO={
	.task=GO_none,
	.sta=Ready,
	.motor_h=&GO_hor_motor_struct,
	.motor_v=&GO_ver_motor_struct,
};

void TaskThread_Init(void)
{
	//Init TIM6 Interrupt  1ms
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能
	
	//定时器TIM6初始化
	TIM_TimeBaseStructure.TIM_Period = 10-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //使能指定的TIM6中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM6,ENABLE);
	//set GE task parm
	GE.GE_box_dis=Global_Parm.GIO.GE_box_dis;
	GE.GE_box_len=Global_Parm.GIO.GE_box_len;
	//set GC task parm
	GC.GCR_hor_pos=Global_Parm.GCS.GCR_hor_pos;
	GC.GCR_ver_pos=Global_Parm.GCS.GCR_ver_pos;
	GC.GCV_down_pos=Global_Parm.GCS.GCV_down_pos;
	//set GP parm
	GP.delay_after=Global_Parm.GP.delay_after;
	GP.delay_before=Global_Parm.GP.delay_before;
	GP.sucker_delay=Global_Parm.GP.sucker_delay;
	GP.sucker_pos=Global_Parm.GP.sucker_pos;
	GP.spray_pos=Global_Parm.GP.spray_pos;
	GP.spray_len=Global_Parm.GP.spray_len;
	GP.spray_speed=Global_Parm.GP.spray_speed;
	GP.spray_pressure=Global_Parm.GP.spray_pressure;
	//set GO parm
	GO.GOH_end_pos=Global_Parm.GCS.GOH_end_pos;
	GO.GOH_mid_pos=Global_Parm.GCS.GOH_mid_pos;
	GO.GOV_box_dis=Global_Parm.GIO.GOV_box_dis;
	GO.GOV_slot_dis=Global_Parm.GIO.GOV_slot_dis;
	GO.GOV_box_len=Global_Parm.GIO.GOV_box_len;
}

void TIM6_IRQHandler(void) //TIM6中断
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{
		//GE 玻片入料任务线程
		GE_TaskThread();
		//GC 玻片夹手任务线程
		GC_TaskThread();
		//GP 玻片封装任务线程
		GP_TaskThread();
		//GO 玻片存储任务线程
		GO_TaksThread();
	}

}

//暂停运行
void Pause_TaskThread(void)
{
	//pause all step motor
	stepperMotorStop(&GE_motor_struct);
	stepperMotorStop(&GC_ver_motor_struct);
	stepperMotorStop(&GC_rot_motor_struct);
	stepperMotorStop(&GP_motor_struct);
	stepperMotorStop(&GO_hor_motor_struct);
	stepperMotorStop(&GO_ver_motor_struct);

	GE.resume_task=GE.task;
	GC.resume_task=GC.task;
	GP.resume_task=GP.task;
	GO.resume_task=GO.task;

	GE.task=GE_none;
	GC.task=GC_none;
	GP.task=GP_none;
	GO.task=GO_none;

}
//恢复运行
void Resume_TaskThread(void)
{
	GE.task=GE.resume_task;
	GE.sta=Ready;

	GC.task=GC.resume_task;
	GC.sta=Ready;

	GP.task=GP.resume_task;
	GP.sta=Ready;

	GO.task=GO.resume_task;
	GO.sta=Ready;
}

void TaskThread_IsReady(void);
{
	//检测是否存在存储盒

	//检测存储槽是否检测到玻片

	//检测是否存在滴胶头

	//检测气压是否达到工作状态

}