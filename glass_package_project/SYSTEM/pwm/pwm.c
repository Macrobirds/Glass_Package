#include "pwm.h"
#include "motor.h"

#define PWM_PRIORITY 1

// GE motor pwm
void TIM1_PWM_Init(u16 psc)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // 使能定时器1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GE_motor_struct.timerfeq = 72000000 / psc;

	// 设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形	GPIOA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		// TIM1_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIO

	// 设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形	GPIOA.8
	// 初始化TIM8
	TIM_TimeBaseStructure.TIM_Period = 1000;					// 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;				// 设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			// repeate counter must set 0
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);				// 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); // 使能指定的TIM1中断,允许更新中断
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);	   // 清中断标志位

	// 初始化TIM8 Channel1 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // 输出模式有八种，选着PWM1模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); // 使能TIM3在CCR2上的预装载寄存器
	TIM_ARRPreloadConfig(TIM1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;		  // TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY; // 先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_CtrlPWMOutputs(TIM1, ENABLE); // 开启定时器

	TIM_Cmd(TIM1, DISABLE);
}

// GO_ver_motor pwm
void TIM4_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GO_ver_motor_struct.timerfeq = 36000000 / psc;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);

	TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// GC_ver_motor pwm
void TIM3_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GC_ver_motor_struct.timerfeq = 36000000 / psc;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	#ifdef BIG_CYLINDER_MOTOR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	#endif

	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	
	
	
	#ifdef BIG_CYLINDER_MOTOR
	TIM_OC3Init(TIM3,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Disable);
	//开启定时器
	TIM_Cmd(TIM3,ENABLE);
	//需要手动开启中断
	TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
	TIM_ITConfig(TIM3,TIM_IT_CC3,DISABLE);
	#else
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	#endif

	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// GP_motor pwm
void TIM8_PWM_Init(u16 psc)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); // 使能定时器1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GP_motor_struct.timerfeq = 72000000 / psc;

	// 设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形	GPIOA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// 初始化TIM8
	TIM_TimeBaseStructure.TIM_Period = 1000;					// 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;				// 设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			// repeate counter must set 0
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);				// 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	//	TIM_UpdateRequestConfig(TIM8,TIM_UpdateSource_Regular);
	//	TIM_SelectOnePulseMode(TIM8,TIM_OPMode_Single); //单脉冲模式

	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE); // 使能指定的TIM1中断,允许更新中断
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);	   // 清中断标志位

	// 初始化TIM8 Channel1 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // 输出模式有八种，选着PWM1模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); // 使能TIM3在CCR2上的预装载寄存器
	TIM_ARRPreloadConfig(TIM8, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;		  // TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY; // 先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_CtrlPWMOutputs(TIM8, ENABLE); // 开启定时器

	TIM_Cmd(TIM8, DISABLE);
}

// GC_rot_motor pwm
void TIM5_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GC_rot_motor_struct.timerfeq = 36000000 / psc;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Disable);

	TIM_ITConfig(TIM5, TIM_IT_CC1, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// GO_hor_motor_pwm
// 引脚复用重定位 ，需要最后设置 ，否则复位失败
void TIM2_PWM_Init(u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // disable JTAG

	GO_hor_motor_struct.timerfeq = 36000000 / psc;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
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
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable); // 关闭预转载

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// 步进电机PWM初始化
void Motor_PWM_Init(void)
{
	TIM1_PWM_Init(APB2_PSC_FEQ);
	TIM4_PWM_Init(APB1_PSC_FEQ);
	TIM3_PWM_Init(APB1_PSC_FEQ);
	TIM8_PWM_Init(APB2_PSC_FEQ);
	TIM5_PWM_Init(APB1_PSC_FEQ);
	TIM2_PWM_Init(APB1_PSC_FEQ);
}

void TIM1_UP_IRQHandler(void) // TIM1中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update); // 清除更新中断标志位
		if (GE_motor_struct.dir == Front)			  // 计算当前位置
		{
			GE_motor_struct.postion++;
		}
		else
		{
			GE_motor_struct.postion--;
		}
		GE_motor_struct.step++; // 计算当前步数

		if (GE_motor_struct.planSetpNumber <= GE_motor_struct.step) // 到达指定步数,电机停止
		{
			TIM_Cmd(TIM1, DISABLE);
			GE_motor_struct.motion = Stop;
			return;
		}
		if (GE_motor_struct.max_pos <= GE_motor_struct.postion) // 到达轨道最大距离
		{
			TIM_Cmd(TIM1, DISABLE);
			GE_motor_struct.motion = Stop;
			return;
		}

		// 在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		//		if(GE_motor_struct.dir==Back&&GE_start_Sen==Sen_Block)
		//		{
		//			TIM_Cmd(TIM1,DISABLE);
		//			GE_motor_struct.postion=0;
		//			GE_motor_struct.motion=Stop;
		//			return;
		//		}
		/****/

		if (GE_motor_struct.motion == ConstMove) // 电机匀速运动
		{
			TIM_SetAutoreload(TIM1, GE_motor_struct.t_m);	// 设定自动重装值
			TIM_SetCompare1(TIM1, GE_motor_struct.t_m / 2); // 匹配值2等于重装值一半，是以占空比为50%
		}
		else if (GE_motor_struct.motion == AccMove)
		{
			// 加速运动
			if (GE_motor_struct.step <= GE_motor_struct.accStepNumber)
			{
				TIM_SetAutoreload(TIM1, GE_motor_struct.AccPeriodArray[GE_motor_struct.step]);	 // 设定自动重装值
				TIM_SetCompare1(TIM1, GE_motor_struct.AccPeriodArray[GE_motor_struct.step] / 2); // 匹配值2等于重装值一半，是以占空比为50%
			}																					 // 减速运动
			else if (GE_motor_struct.step >= GE_motor_struct.planSetpNumber - GE_motor_struct.accStepNumber)
			{
				TIM_SetAutoreload(TIM1, GE_motor_struct.AccPeriodArray[GE_motor_struct.planSetpNumber - GE_motor_struct.step]);	  // 设定自动重装值
				TIM_SetCompare1(TIM1, GE_motor_struct.AccPeriodArray[GE_motor_struct.planSetpNumber - GE_motor_struct.step] / 2); // 匹配值2等于重装值一半，是以占空比为50%
			}																													  // 匀速运动
			else
			{
				TIM_SetAutoreload(TIM1, GE_motor_struct.t_m);	// 设定自动重装值
				TIM_SetCompare1(TIM1, GE_motor_struct.t_m / 2); // 匹配值2等于重装值一半，是以占空比为50%
			}
		}
	}
}

void TIM2_IRQHandler(void) // TIM2 中断 GO_hor_motor
{
	static volatile u8 tim_cnt = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		tim_cnt++;
		tim_cnt %= 2;
		if (!tim_cnt) // 偶数步数
		{
			if (GO_hor_motor_struct.dir == Front) // 计算当前位置
			{
				GO_hor_motor_struct.postion++;
			}
			else
			{
				GO_hor_motor_struct.postion--;
			}
			GO_hor_motor_struct.step++;											// 计算当前步数
			if (GO_hor_motor_struct.planSetpNumber <= GO_hor_motor_struct.step) // 到达指定步数,电机停止
			{
				TIM_Cmd(TIM2, DISABLE);
				GO_hor_motor_struct.motion = Stop;
				return;
			}

			if (GO_hor_motor_struct.max_pos <= GO_hor_motor_struct.postion) // 到达轨道最大距离
			{
				TIM_Cmd(TIM2, DISABLE);
				GO_hor_motor_struct.motion = Stop;
				return;
			}
		}
		// 在不同任务下判断需要停止时电机方向和传感器状态
		/***/

		/****/

		if (GO_hor_motor_struct.motion == ConstMove) // 电机匀速运动
		{
			TIM_SetCompare1(TIM2, (TIM2->CNT + GO_hor_motor_struct.t_m) % 0xffff);
		}
		else if (GO_hor_motor_struct.motion == AccMove)
		{
			// 加速运动
			if (GO_hor_motor_struct.step <= GO_hor_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM2, (TIM2->CNT + GO_hor_motor_struct.AccPeriodArray[GO_hor_motor_struct.step]) % 0xffff);
			} // 减速运动
			else if (GO_hor_motor_struct.step > GO_hor_motor_struct.planSetpNumber - GO_hor_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM2, (TIM2->CNT + GO_hor_motor_struct.AccPeriodArray[GO_hor_motor_struct.planSetpNumber - GO_hor_motor_struct.step]) % 0xffff);
			} // 匀速运动
			else
			{
				TIM_SetCompare1(TIM2, (TIM2->CNT + GO_hor_motor_struct.t_m) % 0xffff);
			}
		}
	}
}

void TIM3_IRQHandler(void) // TIM3 中断 GC_ver_motor
{
	static volatile u8 tim_cnt1 = 0;
	#ifdef BIG_CYLINDER_MOTOR
	static volatile u8 tim_cnt2 = 0;
	#endif
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		tim_cnt1++;
		tim_cnt1 %= 2;
		if (!tim_cnt1)
		{
			if (GC_ver_motor_struct.dir == Front) // 计算当前位置
			{
				GC_ver_motor_struct.postion++;
			}
			else
			{
				GC_ver_motor_struct.postion--;
			}
			GC_ver_motor_struct.step++; // 计算当前步数

			if (GC_ver_motor_struct.planSetpNumber <= GC_ver_motor_struct.step) // 到达指定步数,电机停止
			{
				#ifdef BIG_CYLINDER_MOTOR
				TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
				#else
				TIM_Cmd(TIM3, DISABLE);
				#endif
				GCV_motor_Break = GAS_DISABLE;
				GC_ver_motor_struct.motion = Stop;
				return;
			}

			if (GC_ver_motor_struct.max_pos <= GC_ver_motor_struct.postion) // 到达轨道最大距离
			{
				#ifdef BIG_CYLINDER_MOTOR
				TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
				#else
				TIM_Cmd(TIM3, DISABLE);
				#endif
				GCV_motor_Break = GAS_DISABLE;
				GC_ver_motor_struct.motion = Stop;
				return;
			}
		}

		// 在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		//		if(GC_ver_motor_struct.planpostion==0&&GC_ver_Sen==Sen_Block)
		//		{
		//			TIM_Cmd(TIM3,DISABLE);
		//			GC_ver_motor_struct.motion=Stop;
		//			GC_ver_motor_struct.postion=0;
		//			return;

		//		}

		/****/

		if (GC_ver_motor_struct.motion == ConstMove) // 电机匀速运动
		{
			TIM_SetCompare1(TIM3, (TIM3->CNT + GC_ver_motor_struct.t_m) % 0xffff);
		}
		else if (GC_ver_motor_struct.motion == AccMove)
		{
			// 加速运动
			if (GC_ver_motor_struct.step <= GC_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM3, (TIM3->CNT + GC_ver_motor_struct.AccPeriodArray[GC_ver_motor_struct.step]) % 0xffff);
			} // 减速运动
			else if (GC_ver_motor_struct.step >= GC_ver_motor_struct.planSetpNumber - GC_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM3, (TIM3->CNT + GC_ver_motor_struct.AccPeriodArray[GC_ver_motor_struct.planSetpNumber - GC_ver_motor_struct.step]) % 0xffff);
			} // 匀速运动
			else
			{
				TIM_SetCompare1(TIM3, (TIM3->CNT + GC_ver_motor_struct.t_m) % 0xffff);
			}
		}
	}
	#ifdef BIG_CYLINDER_MOTOR
	if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
		tim_cnt2++;
		tim_cnt2 %= 2;
		if (!tim_cnt2)
		{
			if (GP_motor_big_cyl.dir == Front) // 计算当前位置
			{
				GP_motor_big_cyl.postion++;
			}
			else
			{
				GP_motor_big_cyl.postion--;
			}
			GP_motor_big_cyl.step++; // 计算当前步数

			if (GP_motor_big_cyl.planSetpNumber <= GP_motor_big_cyl.step) // 到达指定步数,电机停止
			{
				TIM_ITConfig(TIM3,TIM_IT_CC3,DISABLE);
				GP_motor_big_cyl.motion = Stop;
				return;
			}

			if (GP_motor_big_cyl.max_pos <= GP_motor_big_cyl.postion) // 到达轨道最大距离
			{
				TIM_ITConfig(TIM3,TIM_IT_CC3,DISABLE);
				GP_motor_big_cyl.motion = Stop;
				return;
			}
		}

		// 在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		if(GP.task==GP_sucker_down&&GP.subtask==1)
		{
			if(GP_sucker_Sen==Sen_Block)
			{
				TIM_ITConfig(TIM3,TIM_IT_CC3,DISABLE);
				GP_motor_big_cyl.motion = Stop;
			}
		}

		/****/

		if (GP_motor_big_cyl.motion == ConstMove) // 电机匀速运动
		{
			TIM_SetCompare3(TIM3, (TIM3->CNT + GP_motor_big_cyl.t_m) % 0xffff);
		}
		else if (GP_motor_big_cyl.motion == AccMove)
		{
			// 加速运动
			if (GP_motor_big_cyl.step <= GP_motor_big_cyl.accStepNumber)
			{
				TIM_SetCompare3(TIM3, (TIM3->CNT + GP_motor_big_cyl.AccPeriodArray[GP_motor_big_cyl.step]) % 0xffff);
			} // 减速运动
			else if (GP_motor_big_cyl.step >= GP_motor_big_cyl.planSetpNumber - GP_motor_big_cyl.accStepNumber)
			{
				TIM_SetCompare3(TIM3, (TIM3->CNT + GP_motor_big_cyl.AccPeriodArray[GP_motor_big_cyl.planSetpNumber - GP_motor_big_cyl.step]) % 0xffff);
			} // 匀速运动
			else
			{
				TIM_SetCompare3(TIM3, (TIM3->CNT + GP_motor_big_cyl.t_m) % 0xffff);
			}
		}
	}
	
	#endif
	
}

void TIM4_IRQHandler(void) // TIM4 中断 GO_ver_motor
{
	static volatile u8 tim_cnt;
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
		tim_cnt++;
		tim_cnt %= 2;
		if (!tim_cnt)
		{
			if (GO_ver_motor_struct.dir == Front) // 计算当前位置
			{
				GO_ver_motor_struct.postion++;
			}
			else
			{
				GO_ver_motor_struct.postion--;
			}
			GO_ver_motor_struct.step++; // 计算当前步数

			if (GO_ver_motor_struct.planSetpNumber <= GO_ver_motor_struct.step) // 到达指定步数,电机停止
			{
				TIM_Cmd(TIM4, DISABLE);
				GO_ver_motor_struct.motion = Stop;
				return;
			}
			if (GO_ver_motor_struct.max_pos <= GO_ver_motor_struct.postion) // 到达最大距离
			{
				TIM_Cmd(TIM4, DISABLE);
				GO_ver_motor_struct.motion = Stop;
				GO.Storage_full=TRUE;
				return;
			}
		}

		// 在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		if (GO.task == GO_Box_In && GO.subtask == 0) //存储盒进入到检测不到盒 或到原点
		{
			if (GOV_box_Sen != Sen_Block)
			{
				TIM_Cmd(TIM4, DISABLE);
				GO_ver_motor_struct.motion = Stop;
				return;
			}
		}

		/****/

		if (GO_ver_motor_struct.motion == ConstMove) // 电机匀速运动
		{
			TIM_SetCompare1(TIM4, (TIM4->CNT + GO_ver_motor_struct.t_m) % 0xffff);
		}
		else
		{
			// 加速运动
			if (GO_ver_motor_struct.step <= GO_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM4, (TIM4->CNT + GO_ver_motor_struct.AccPeriodArray[GO_ver_motor_struct.step]) % 0xffff);
			} // 减速运动
			else if (GO_ver_motor_struct.step >= GO_ver_motor_struct.planSetpNumber - GO_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM4, (TIM4->CNT + GO_ver_motor_struct.AccPeriodArray[GO_ver_motor_struct.planSetpNumber - GO_ver_motor_struct.step]) % 0xffff);
			} // 匀速运动
			else
			{
				TIM_SetCompare1(TIM4, (TIM4->CNT + GO_ver_motor_struct.t_m) % 0xffff);
			}
		}
	}
}

void TIM5_IRQHandler(void) // TIM5 中断 GC_rot_motor
{
	static volatile u8 tim_cnt = 0;
	if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
		tim_cnt++;
		tim_cnt %= 2;
		if (!tim_cnt)
		{
			if (GC_rot_motor_struct.dir == Front) // 计算当前位置
			{
				GC_rot_motor_struct.postion++;
			}
			else
			{
				GC_rot_motor_struct.postion--;
			}
			GC_rot_motor_struct.step++; // 计算当前步数

			if (GC_rot_motor_struct.planSetpNumber <= GC_rot_motor_struct.step) // 到达指定步数,电机停止
			{
				TIM_Cmd(TIM5, DISABLE);
				GC_rot_motor_struct.motion = Stop;
				return;
			}

			if(GC_rot_motor_struct.max_pos<= GC_rot_motor_struct.postion) //到达指定步数,电机停止
			{
				TIM_Cmd(TIM5,DISABLE);
				GC_rot_motor_struct.motion=Stop;
				return;
			}
		}

		// 在不同任务下判断需要停止时电机方向和传感器状态
		/***/
		//		if(GC_rot_motor_struct.dir==Back&&GC_rot_Sen==Sen_Block)
		//		{
		//			TIM_Cmd(TIM5,DISABLE);
		//			GC_rot_motor_struct.motion=Stop;
		//			GC_rot_motor_struct.postion=0;
		//			return;
		//		}

		/****/

		if (GC_rot_motor_struct.motion == ConstMove) // 电机匀速运动
		{
			TIM_SetCompare1(TIM5, (TIM5->CNT + GC_rot_motor_struct.t_m) % 0xffff);
		}
		else
		{
			// 加速运动
			if (GC_rot_motor_struct.step <= GC_rot_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM5, (TIM5->CNT + GC_rot_motor_struct.AccPeriodArray[GC_rot_motor_struct.step]) % 0xffff);
			} // 减速运动
			else if (GC_rot_motor_struct.step >= GC_rot_motor_struct.planSetpNumber - GC_rot_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM5, (TIM5->CNT + GC_rot_motor_struct.AccPeriodArray[GC_rot_motor_struct.planSetpNumber - GC_rot_motor_struct.step]) % 0xffff);
			} // 匀速运动
			else
			{
				TIM_SetCompare1(TIM5, (TIM5->CNT + GC_rot_motor_struct.t_m) % 0xffff);
			}
		}
	}
}

void TIM8_UP_IRQHandler(void) // TIM8中断 GP_motor
{
	if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM8, TIM_FLAG_Update); // 清除更新中断标志位
		if (GP_motor_struct.dir == Front)			  // 计算当前位置
		{
			GP_motor_struct.postion++;
		}
		else
		{
			GP_motor_struct.postion--;
		}
		GP_motor_struct.step++; // 计算当前步数

		if (GP_motor_struct.planSetpNumber <= GP_motor_struct.step) // 到达指定步数,电机停止
		{
			TIM_Cmd(TIM8, DISABLE);
			GP_motor_struct.motion = Stop;
			return;
		}

		if (GP_motor_struct.max_pos <= GP_motor_struct.postion) // 到达轨道最大距离
		{
			TIM_Cmd(TIM8, DISABLE);
			GP_motor_struct.motion = Stop;
			return;
		}

		//		//在不同任务下判断需要停止时电机方向和传感器状态
		//		/***/
		//		if(GP_motor_struct.dir==Back&&GP_start_Sen==Sen_Block)
		//		{
		//			TIM_Cmd(TIM8,DISABLE);
		//			GP_motor_struct.motion=Stop;
		//			GP_motor_struct.postion=0;
		//			return;

		//		}

		/****/

		if (GP_motor_struct.motion == ConstMove) // 电机匀速运动
		{
			TIM_SetAutoreload(TIM8, GP_motor_struct.t_m);	// 设定自动重装值
			TIM_SetCompare1(TIM8, GP_motor_struct.t_m / 2); // 匹配值2等于重装值一半，是以占空比为50%
		}
		else
		{
			// 加速运动
			if (GP_motor_struct.step <= GP_motor_struct.accStepNumber)
			{
				TIM_SetAutoreload(TIM8, GP_motor_struct.AccPeriodArray[GP_motor_struct.step]);	 // 设定自动重装值
				TIM_SetCompare1(TIM8, GP_motor_struct.AccPeriodArray[GP_motor_struct.step] / 2); // 匹配值2等于重装值一半，是以占空比为50%
			}																					 // 减速运动
			else if (GP_motor_struct.step >= GP_motor_struct.planSetpNumber - GP_motor_struct.accStepNumber)
			{
				TIM_SetAutoreload(TIM8, GP_motor_struct.AccPeriodArray[GP_motor_struct.planSetpNumber - GP_motor_struct.step]); // 设定自动重装值
				TIM_SetCompare1(TIM8, GP_motor_struct.AccPeriodArray[GP_motor_struct.planSetpNumber - GP_motor_struct.step]);	// 匹配值2等于重装值一半，是以占空比为50%
			}																													// 匀速运动
			else
			{
				TIM_SetAutoreload(TIM8, GP_motor_struct.t_m);	// 设定自动重装值
				TIM_SetCompare1(TIM8, GP_motor_struct.t_m / 2); // 匹配值2等于重装值一半，是以占空比为50%
			}
		}
	}
}

//			///////////////////////////////////
//			TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);//清除更新中断标志位
//			TIM_SetAutoreload(TIM1,1000);//设定自动重装值
//			TIM_SetCompare1(TIM1,500); //匹配值2等于重装值一半，是以占空比为50%
//			/////////////////////////////////

//	u16 capture;
//	if(TIM_GetITStatus(TIM5,TIM_IT_CC1)!=RESET)
//	{
//		TIM_ClearITPendingBit(TIM5,TIM_IT_CC1);
//		capture=TIM_GetCapture1(TIM5);
//		TIM_SetCompare1(TIM5,capture+1000);
//
//	}

//	u16 capture;
//	if(TIM_GetITStatus(TIM4,TIM_IT_CC1)!=RESET)
//	{
//		TIM_ClearITPendingBit(TIM4,TIM_IT_CC1);
//		capture=TIM_GetCapture1(TIM4);
//		TIM_SetCompare1(TIM4,capture+1000);
//
//	}

//	u16 capture=0;
//	if(TIM_GetITStatus(TIM3,TIM_IT_CC1)!=RESET)
//	{
//		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
//		capture=TIM_GetCapture1(TIM3);
//		TIM_SetCompare1(TIM3,capture+1000);
//	}

//		u16 capture_val=0;
//	 if(TIM_GetITStatus(TIM2,TIM_IT_CC1)!=RESET)
//	 {
//		TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
//		 capture_val=TIM_GetCapture1(TIM2);
//		 TIM_SetCompare1(TIM2,capture_val+1000);
//	 }
