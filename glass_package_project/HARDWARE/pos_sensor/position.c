#include "Globalconfig.h"

#define DELAY_US 50
#define POSITION_PRIORITY 2

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
	//GPIO 初始化
	Exti_0_7_GPIO_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	if(IoInput_0_Enable)
	{
		//GPIOD 0
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //上升沿触发
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
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //下降沿触发
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
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //双边沿沿触发
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
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //上升沿触发
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
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
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
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = POSITION_PRIORITY;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_7_Enable
		|| IoInput_8_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = POSITION_PRIORITY;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
}

//位置传感器外部中断初始化
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
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = POSITION_PRIORITY;	//抢占优先级
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
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = POSITION_PRIORITY;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_0_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_1_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = POSITION_PRIORITY;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_2_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = POSITION_PRIORITY;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
		if(IoInput_3_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = POSITION_PRIORITY;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
		if(IoInput_4_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//中断向量
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = POSITION_PRIORITY;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
	}
}

void EXTI0_IRQHandler(void)
{
//GOV_galss_Sen
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET) //检测到存储槽是否存在玻片
	{
		delay_us(DELAY_US);
		if(GO.task==GO_out)
		{
			if(GOV_glass_Sen!=Sen_Block) //下降沿触发
			{
//				if(GO_ver_motor_struct.step>200) //确保移动到下一个槽
//				{
////					GO_ver_motor_struct.step=GO_ver_motor_struct.planpostion-100;
//					TIM_Cmd(TIM4,DISABLE);
//					GO_ver_motor_struct.motion=Stop;
//					GO_ver_motor_struct.step=0;
//				}
					TIM_Cmd(TIM4,DISABLE);
					GO_ver_motor_struct.motion=Stop;
					GO_ver_motor_struct.step=0;
			}
		}else if(GO.task==GO_Box_In&&GO.subtask==1)
		{
			if(GOV_glass_Sen!=Sen_Block) //下降沿触发
			{
				TIM_Cmd(TIM4,DISABLE);
				GO_ver_motor_struct.motion=Stop;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}	
	

}
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void)
{
	//GOH_start_Sen
	if(EXTI_GetITStatus(EXTI_Line3)!=RESET) //封片设备到达原点
	{
		delay_us(DELAY_US);
		if(GOH_start_Sen==Sen_Block)//封片设备停止
		{
			TIM_Cmd(TIM2,DISABLE);
			GO_hor_motor_struct.postion=0;
			GO_hor_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}
void EXTI4_IRQHandler(void)
{
	//GP_small_cyl_Sen
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET) //封片设备到达原点
	{
		delay_us(DELAY_US);
		if(GP_small_cyl_Sen==Sen_Block)//封片设备停止
		{
			if(GP.task==GP_package)
			{
				GP.cyl_pos_flag=TRUE; //标识气缸到位需要动作
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}

}


void EXTI9_5_IRQHandler(void)
{
	//GOH_end_Sen
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET) //水平出料到达终点
	{
		delay_us(DELAY_US);
		if(GOH_end_Sen==Sen_Block) 
		{
			TIM_Cmd(TIM2,DISABLE);
			GO_hor_motor_struct.postion=GO.GOH_end_pos;
			GO_hor_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line5);
		printf("GOH_end_sen\r\n");
	}
	//GOV_start_Sen
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET) //垂直出料槽到达原点
	{
		delay_us(DELAY_US);
		if(GOV_start_Sen==Sen_Block)
		{
			TIM_Cmd(TIM4,DISABLE);
			GO_ver_motor_struct.postion=0;
			GO_ver_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line6);
		printf("GOV_start_Sen\r\n");
	}
	#ifdef BIG_CYLINDER_MOTOR
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET) //大气缸步进电机到达原点
	{
		delay_us(DELAY_US);
		if(GP_big_cyl_Sen==Sen_Block)
		{
			TIM_ITConfig(TIM3,TIM_IT_CC3,DISABLE);
			GP_motor_big_cyl.postion=0;
			GP_motor_big_cyl.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line7);
		printf("big cyl motor sen\r\n");
	}
	#else 
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET) //大气缸传感器
	{
		delay_us(DELAY_US);
		if(GP_cyl_pos_Sen==Sen_Block)
		{
			if(GP.task==GP_package)
			{
				GP.cyl_pos_flag=TRUE; //标识气缸到位需要动作
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	#endif

#ifdef GE_UP_SENSOR_BEFORE
	//GE_down_Sen
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET) //进料槽对射光电下
	{
		delay_us(DELAY_US);
		if(GE.task==GE_move_glass)
		{
					if(!flag)
					{
						if(GE_down_Sen==Sen_Block) //下端照射
						{
							TIM_Cmd(TIM1,DISABLE);
							GE_motor_struct.motion=Stop;
							printf("GE_down_Sen \r\n");
						}
					}else
					{
						flag=FALSE;
					}				
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	//GE_up_Sen
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET) ///进料槽对射光电上
	{
		delay_us(DELAY_US);
		if((GE.task==GE_move_front||GE.task==GE_move_glass)&&GC_ver_Sen==Sen_Block)
		{
			if(GE_up_Sen==Sen_Block) //上端照射
			{
				TIM_Cmd(TIM1,DISABLE);
				GE_motor_struct.motion=Stop;
				printf("GE_up_Sen\r\n");
				flag=TRUE;
			}
			
		}
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
	#endif
	
	#ifdef GE_UP_DOWN_SENSOR
		//GE_down_Sen
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET) //进料槽对射光电下
	{
		delay_us(DELAY_US);

		if(GE.task==GE_move_glass)
		{
				if(GE_down_Sen==Sen_Block&&GE_up_Sen!=Sen_Block) //只有下端照射
				{
						TIM_Cmd(TIM1,DISABLE);
						GE_motor_struct.motion=Stop;
						printf("GE_down_Sen \r\n");
				}		
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET) ///进料槽对射光电上
	{
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
	
	#endif

}
void EXTI15_10_IRQHandler(void)
{
	//GE_start_Sen
	if(EXTI_GetITStatus(EXTI_Line10)!=RESET) //进料槽移动到原点
	{
			delay_us(DELAY_US);
			if(GE_start_Sen==Sen_Block)//原点感应
			{
				TIM_Cmd(TIM1,DISABLE);
				GE_motor_struct.postion=0;
				GE_motor_struct.motion=Stop;
			}
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
	//GC_rot_Sen
	if(EXTI_GetITStatus(EXTI_Line11)!=RESET) //夹手旋转至原点
	{
		delay_us(DELAY_US+150);
		if(GC_rot_Sen!=Sen_Block) //夹手旋转停止
		{
			TIM_Cmd(TIM5,DISABLE);
			GC_rot_motor_struct.postion=0;
			GC_rot_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
	//GC_ver_Sen
	#ifdef BIG_CYLINDER_MOTOR
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET) //夹手运动到垂直原点
	{
		delay_us(DELAY_US);
		if(GC_ver_Sen==Sen_Block) //夹手垂直停止
		{
			TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
			GCV_motor_Break=GAS_DISABLE;
			GC_ver_motor_struct.postion=0;
			GC_ver_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line12);
		printf("GC_ver_sen\r\n");
	}
	#else
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET) //夹手运动到垂直原点
	{
		delay_us(DELAY_US);
		if(GC_ver_Sen==Sen_Block) //夹手垂直停止
		{
			TIM_Cmd(TIM3,DISABLE);
			GCV_motor_Break=GAS_DISABLE;
			GC_ver_motor_struct.postion=0;
			GC_ver_motor_struct.motion=Stop;
		}

		EXTI_ClearITPendingBit(EXTI_Line12);
	}
	#endif
	
	
	
	//GP_start_Sen
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET) //封片运动到原点
	{
		delay_us(DELAY_US);
		if(GP_start_Sen==Sen_Block)//封片设备停止
		{
			TIM_Cmd(TIM8,DISABLE);
			GP_motor_struct.postion=0;
			GP_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line13);
	}

	if(EXTI_GetITStatus(EXTI_Line14)!=RESET) //封片设备到达原点
	{
		delay_us(DELAY_US);
		if(GOV_box_Sen==Sen_Block) //上升沿触发
		{
			if(TaskThread_CheckStop()&&GO.task!=GO_Box_In)
			{
				TIM_Cmd(TIM6,DISABLE);
				GO.task=GO_Box_In;
				GO.sta=Ready;
				GO.subtask=0;
				TIM_Cmd(TIM6,ENABLE);
			}

		}else //下降沿触发
		{
			if(TaskThread_CheckStop()&&GO.task!=GO_Box_In)
			{
				TIM_Cmd(TIM6,DISABLE);
				GO.task=GO_Box_Out;
				GO.sta=Ready;
				GO.subtask=0;
				GO.Storage_full=TRUE;
				TIM_Cmd(TIM6,ENABLE);
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	
	
	
//	if(EXTI_GetITStatus(EXTI_Line15)!=RESET) //封片设备到达原点
//	{

//		EXTI_ClearITPendingBit(EXTI_Line15);
//	}



}

//		//GOH_mid_Sen
//	if(EXTI_GetITStatus(EXTI_Line0)!=RESET) //水平出料电机经过工作点
//	{
//		if(GO_hor_motor_struct.dir==Front) //向终点方向
//		{
//			delay_us(DELAY_US);
//			if(GOH_mid_Sen==Sen_Block) //到达工作点停止
//			{
//				TIM_Cmd(TIM2,DISABLE);
//				GO_hor_motor_struct.postion=GO.GOH_mid_pos;
//				GO_hor_motor_struct.motion=Stop;
//			}
//		}
//		EXTI_ClearITPendingBit(EXTI_Line0);
//		printf("exti 1\r\n");
//	}

//	printf("exit8 trigger\r\n");
//		if(GE_start_Sen==Sen_Block)
//		{
//	//		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
//			TIM_SetCompare1(TIM2,(TIM2->CNT+1000)%0xffff);
//			TIM_Cmd(TIM2,ENABLE);
//			
//			
////	TIM_SetAutoreload(TIM1,1000);//设定自动重装值	
////	TIM_SetCompare1(TIM1,500); //匹配值2等于重装值一半，是以占空比为50%	
////	TIM_Cmd(TIM1,ENABLE);

//			
//		}else
//		{
//			TIM_Cmd(TIM2, DISABLE);  //关闭TIM8				
//		}
//		EXTI_ClearITPendingBit(EXTI_Line8);


//		if(GE.task==GE_move_glass)
//		{
//			if(sensor_filter>=old_sensor_filter)
//			{
//				if(sensor_filter-old_sensor_filter>150)
//				{
//					if(!flag)
//					{
//						if(GE_down_Sen==Sen_Block) //只有下端照射
//						{
//							TIM_Cmd(TIM1,DISABLE);
//							GE_motor_struct.motion=Stop;
//							printf("GE_down_Sen %d\r\n",sensor_filter);
//						}
//					}else
//					{
//						flag=FALSE;
//					}
//				}
//			}else
//			{
//				if((sensor_filter+old_sensor_filter)%1000>150)
//				{
//					if(!flag)
//					{
//						if(GE_down_Sen==Sen_Block) //只有下端照射
//						{
//							TIM_Cmd(TIM1,DISABLE);
//							GE_motor_struct.motion=Stop;
//							printf("GE_down_Sen %d\r\n",sensor_filter);
//						}
//					}else
//					{
//						flag=FALSE;
//					}
//				}
//			}
//		}