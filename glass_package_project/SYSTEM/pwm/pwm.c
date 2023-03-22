#include "pwm.h"
#include "motor.h"



//GE motor pwm
void TIM1_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	GE_motor_struct.timerfeq=72000000/psc;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM1,TIM_IT_CC1,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//GO_ver_motor pwm
void TIM4_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
	GO_ver_motor_struct.timerfeq=72000000/psc;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

//GC_ver_motor pwm
void TIM3_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	GC_ver_motor_struct.timerfeq=72000000/psc;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//GP_motor pwm
void TIM8_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
	
	GP_motor_struct.timerfeq=72000000/psc;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM8,TIM_IT_CC1,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//GC_rot_motor pwm
void TIM5_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	GC_rot_motor_struct.timerfeq=72000000/psc;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_CC1,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//GO_ver_motor_pwm
//引脚复用重定位 ，需要最后设置 ，否则复位失败
void TIM2_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE); //disable JTAG
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2,ENABLE); //TIM2 Remap CH1->PA15
	
	GO_hor_motor_struct.timerfeq=72000000/psc;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Motor_PWM_Init(void)
{
	TIM1_PWM_Init(PSC_FEQ);
	TIM4_PWM_Init(PSC_FEQ);
	TIM3_PWM_Init(PSC_FEQ);
	TIM8_PWM_Init(PSC_FEQ);
	TIM5_PWM_Init(PSC_FEQ);
	TIM2_PWM_Init(PSC_FEQ);
}



void TIM1_CC_IRQHandler(void) //TIM1中断
{
	if(TIM_GetITStatus(TIM1,TIM_IT_CC1)!=RESET)
	{
		if(GE_motor_struct.dir==Front) //计算当前位置
		{
			GE_motor_struct.postion++;
		}else
		{
			GE_motor_struct.postion--;
		}
		GE_motor_struct.step++;//计算当前步数
		
		if(GE_motor_struct.planSetpNumber==GO_hor_motor_struct.step) //到达指定步数,电机停止
		{
			TIM_Cmd(TIM1,DISABLE);
			GE_motor_struct.motion=Stop;
			return;
		}
		//在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		
		
		
		/****/
		
		
	
		if(GE_motor_struct.motion==ConstMove)//电机匀速运动
		{
			TIM_SetCompare1(TIM1,(TIM1->CNT+GE_motor_struct.t_m)%0xffff);
		}else
		{
			//加速运动
			if(GE_motor_struct.step<=GE_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM1,(TIM1->CNT+GE_motor_struct.AccPeriodArray[GE_motor_struct.step])%0xffff);
			}//减速运动
			else if(GE_motor_struct.step>=GE_motor_struct.planSetpNumber-GE_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM1,(TIM1->CNT+GE_motor_struct.AccPeriodArray[GE_motor_struct.planSetpNumber-GE_motor_struct.step])%0xffff);
			}//匀速运动
			else
			{
				TIM_SetCompare1(TIM1,(TIM1->CNT+GE_motor_struct.t_m)%0xffff);
			}
			
		}
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_CC1);
	}
}

void TIM2_IRQHandler(void) //TIM2 中断 GO_hor_motor
{
	if(TIM_GetITStatus(TIM2,TIM_IT_CC1)!=RESET)
	{
		if(GO_hor_motor_struct.dir==Front) //计算当前位置
		{
			GO_hor_motor_struct.postion++;
		}else
		{
			GO_hor_motor_struct.postion--;
		}
		GO_hor_motor_struct.step++;//计算当前步数
		
		if(GO_hor_motor_struct.planSetpNumber==GO_hor_motor_struct.step) //到达指定步数,电机停止
		{
			TIM_Cmd(TIM2,DISABLE);
			GO_hor_motor_struct.motion=Stop;
			return;
		}
		//在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		
		/****/
		
		
	
		if(GO_hor_motor_struct.motion==ConstMove)//电机匀速运动
		{
			TIM_SetCompare1(TIM2,(TIM2->CNT+GO_hor_motor_struct.t_m)%0xffff);
		}else
		{
			//加速运动
			if(GO_hor_motor_struct.step<=GO_hor_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM2,(TIM2->CNT+GO_hor_motor_struct.AccPeriodArray[GO_hor_motor_struct.step])%0xffff);
			}//减速运动
			else if(GO_hor_motor_struct.step>=GO_hor_motor_struct.planSetpNumber-GO_hor_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM2,(TIM2->CNT+GO_hor_motor_struct.AccPeriodArray[GO_hor_motor_struct.planSetpNumber-GO_hor_motor_struct.step])%0xffff);
			}//匀速运动
			else
			{
				TIM_SetCompare1(TIM2,(TIM2->CNT+GO_hor_motor_struct.t_m)%0xffff);
			}
			
		}
		
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
	}
}

void TIM3_IRQHandler(void) //TIM3 中断
{
	if(TIM_GetITStatus(TIM3,TIM_IT_CC1)!=RESET)
	{
		if(GC_ver_motor_struct.dir==Front) //计算当前位置
		{
			GC_ver_motor_struct.postion++;
		}else
		{
			GC_ver_motor_struct.postion--;
		}
		GC_ver_motor_struct.step++;//计算当前步数
		
		if(GC_ver_motor_struct.planSetpNumber==GC_ver_motor_struct.step) //到达指定步数,电机停止
		{
			TIM_Cmd(TIM3,DISABLE);
			GC_ver_motor_struct.motion=Stop;
			return;
		}
		//在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		
		
		
		/****/
		
		
	
		if(GC_ver_motor_struct.motion==ConstMove)//电机匀速运动
		{
			TIM_SetCompare1(TIM3,(TIM3->CNT+GC_ver_motor_struct.t_m)%0xffff);
		}else
		{
			//加速运动
			if(GC_ver_motor_struct.step<=GC_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM3,(TIM3->CNT+GC_ver_motor_struct.AccPeriodArray[GC_ver_motor_struct.step])%0xffff);
			}//减速运动
			else if(GC_ver_motor_struct.step>=GC_ver_motor_struct.planSetpNumber-GC_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM3,(TIM1->CNT+GC_ver_motor_struct.AccPeriodArray[GC_ver_motor_struct.planSetpNumber-GC_ver_motor_struct.step])%0xffff);
			}//匀速运动
			else
			{
				TIM_SetCompare1(TIM3,(TIM3->CNT+GC_ver_motor_struct.t_m)%0xffff);
			}
			
		}
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
	}
}

void TIM4_IRQHandler(void) //TIM4 中断
{
	if(TIM_GetITStatus(TIM4,TIM_IT_CC1)!=RESET)
	{
		if(GO_ver_motor_struct.dir==Front) //计算当前位置
		{
			GO_ver_motor_struct.postion++;
		}else
		{
			GO_ver_motor_struct.postion--;
		}
		GO_ver_motor_struct.step++;//计算当前步数
		
		if(GO_ver_motor_struct.planSetpNumber==GO_ver_motor_struct.step) //到达指定步数,电机停止
		{
			TIM_Cmd(TIM4,DISABLE);
			GO_ver_motor_struct.motion=Stop;
			return;
		}
		//在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		
		
		
		/****/
		
		
	
		if(GO_ver_motor_struct.motion==ConstMove)//电机匀速运动
		{
			TIM_SetCompare1(TIM4,(TIM4->CNT+GO_ver_motor_struct.t_m)%0xffff);
		}else
		{
			//加速运动
			if(GO_ver_motor_struct.step<=GO_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM4,(TIM4->CNT+GO_ver_motor_struct.AccPeriodArray[GO_ver_motor_struct.step])%0xffff);
			}//减速运动
			else if(GO_ver_motor_struct.step>=GO_ver_motor_struct.planSetpNumber-GO_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM4,(TIM4->CNT+GO_ver_motor_struct.AccPeriodArray[GO_ver_motor_struct.planSetpNumber-GO_ver_motor_struct.step])%0xffff);
			}//匀速运动
			else
			{
				TIM_SetCompare1(TIM4,(TIM4->CNT+GO_ver_motor_struct.t_m)%0xffff);
			}
			
		}
		
		TIM_ClearITPendingBit(TIM4,TIM_IT_CC1);
	}
}

void TIM5_IRQHandler(void) //TIM5 中断
{
if(TIM_GetITStatus(TIM5,TIM_IT_CC1)!=RESET)
	{
		if(GC_rot_motor_struct.dir==Front) //计算当前位置
		{
			GC_rot_motor_struct.postion++;
		}else
		{
			GC_rot_motor_struct.postion--;
		}
		GC_rot_motor_struct.step++;//计算当前步数
		
		if(GC_rot_motor_struct.planSetpNumber==GC_rot_motor_struct.step) //到达指定步数,电机停止
		{
			TIM_Cmd(TIM5,DISABLE);
			GC_rot_motor_struct.motion=Stop;
			return;
		}
		//在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		
		
		
		/****/
		
		
	
		if(GC_rot_motor_struct.motion==ConstMove)//电机匀速运动
		{
			TIM_SetCompare1(TIM5,(TIM5->CNT+GC_rot_motor_struct.t_m)%0xffff);
		}else
		{
			//加速运动
			if(GC_rot_motor_struct.step<=GC_rot_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM5,(TIM5->CNT+GC_rot_motor_struct.AccPeriodArray[GC_rot_motor_struct.step])%0xffff);
			}//减速运动
			else if(GC_rot_motor_struct.step>=GC_rot_motor_struct.planSetpNumber-GC_rot_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM5,(TIM4->CNT+GC_rot_motor_struct.AccPeriodArray[GC_rot_motor_struct.planSetpNumber-GC_rot_motor_struct.step])%0xffff);
			}//匀速运动
			else
			{
				TIM_SetCompare1(TIM5,(TIM5->CNT+GC_rot_motor_struct.t_m)%0xffff);
			}
			
		}
		
		TIM_ClearITPendingBit(TIM5,TIM_IT_CC1);
	}
}

void TIM8_CC_IRQHandler(void) //TIM8中断
{
if(TIM_GetITStatus(TIM8,TIM_IT_CC1)!=RESET)
	{
		if(GP_motor_struct.dir==Front) //计算当前位置
		{
			GP_motor_struct.postion++;
		}else
		{
			GP_motor_struct.postion--;
		}
		GP_motor_struct.step++;//计算当前步数
		
		if(GP_motor_struct.planSetpNumber==GP_motor_struct.step) //到达指定步数,电机停止
		{
			TIM_Cmd(TIM8,DISABLE);
			GP_motor_struct.motion=Stop;
			return;
		}
		//在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		
		
		
		/****/
		
		
	
		if(GP_motor_struct.motion==ConstMove)//电机匀速运动
		{
			TIM_SetCompare1(TIM8,(TIM8->CNT+GC_rot_motor_struct.t_m)%0xffff);
		}else
		{
			//加速运动
			if(GP_motor_struct.step<=GP_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM8,(TIM8->CNT+GP_motor_struct.AccPeriodArray[GP_motor_struct.step])%0xffff);
			}//减速运动
			else if(GP_motor_struct.step>=GP_motor_struct.planSetpNumber-GP_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM8,(TIM8->CNT+GP_motor_struct.AccPeriodArray[GP_motor_struct.planSetpNumber-GP_motor_struct.step])%0xffff);
			}//匀速运动
			else
			{
				TIM_SetCompare1(TIM8,(TIM8->CNT+GP_motor_struct.t_m)%0xffff);
			}
			
		}
		
		TIM_ClearITPendingBit(TIM5,TIM_IT_CC1);
	}
}