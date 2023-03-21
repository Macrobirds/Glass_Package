#include "position.h"

static void Exti_8_15_GPIO_Init(void) 
{ 
	// PE8-15
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|
		 GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置成上拉输入 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

static void Exti_0_7_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC,ENABLE);
	//PD 0 1 3-7
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|
	GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	//PC2
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

}

static void Exti_Init_0_7(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	//GPIO 初始化
	Exti_0_7_GPIO_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	if(IoInput_0_Enable)
	{
		//GPIOD 0
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_Init(&EXTI_InitStructure);
	}
		if(IoInput_1_Enable)
	{
		//GPIOD 1
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_2_Enable)
	{
		//GPIOC 2
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line2;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_3_Enable)
	{
		//GPIOD 3
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource3);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line3;
		EXTI_Init(&EXTI_InitStructure);
	}
	
	if(IoInput_4_Enable)
	{
		//GPIOD 4
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource4);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line4;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_5_Enable)
	{
		//GPIOD 5
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource5);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_6_Enable)
	{
		//GPIOD 6
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource6);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line6;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_7_Enable)
	{
		//GPIOD 7
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line7;
		EXTI_Init(&EXTI_InitStructure);
	}
	
	
	
}
static void Exti_Init_8_15(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	//	GPIO初始化
	Exti_8_15_GPIO_Init();	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	//  中断线以及中断初始化配置
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	if(IoInput_15_Enable)
	{
		//GPIOE.15
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource15);
		//上升沿触发 EXTI_Trigger_Rising_Falling; 双边沿触发
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_Line = EXTI_Line15;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_14_Enable)
	{
		//GPIOE.14
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource14);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line14;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_13_Enable)
	{
		//GPIOE.13
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource13);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line13;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_12_Enable)
	{
		//GPIOE.12
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource12);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line12;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_11_Enable)
	{
		//GPIOE.11
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource11);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line11;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_10_Enable)
	{
		//GPIOE.10
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource10);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line10;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_9_Enable)
	{
		//GPIOE.9
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource9);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line9;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_8_Enable)
	{
		//GPIOE.8
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource8);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //双边沿触发
		EXTI_InitStructure.EXTI_Line = EXTI_Line8;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_15_Enable
		|| IoInput_14_Enable
		|| IoInput_13_Enable
		|| IoInput_12_Enable
		|| IoInput_11_Enable
		|| IoInput_10_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_7_Enable
		|| IoInput_8_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
}


void postions_sensor_Init(void)
{
	 NVIC_InitTypeDef NVIC_InitStructure;
	Exti_Init_0_7();
	Exti_Init_8_15();
	//中断控制器配置
		if(IoInput_15_Enable
		|| IoInput_14_Enable
		|| IoInput_13_Enable
		|| IoInput_12_Enable
		|| IoInput_11_Enable
		|| IoInput_10_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
		if(IoInput_5_Enable
		||IoInput_6_Enable
		||IoInput_7_Enable
		|| IoInput_8_Enable
		||IoInput_9_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_0_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_1_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_2_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
		if(IoInput_3_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
		if(IoInput_4_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
}

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);