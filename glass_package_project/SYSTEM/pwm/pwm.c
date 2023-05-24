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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // ʹ�ܶ�ʱ��1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GE_motor_struct.timerfeq = 72000000 / psc;

	// ���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��	GPIOA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		// TIM1_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // ��ʼ��GPIO

	// ���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��	GPIOA.8
	// ��ʼ��TIM8
	TIM_TimeBaseStructure.TIM_Period = 1000;					// ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;				// ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			// repeate counter must set 0
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);				// ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); // ʹ��ָ����TIM1�ж�,��������ж�
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);	   // ���жϱ�־λ

	// ��ʼ��TIM8 Channel1 PWMģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // ���ģʽ�а��֣�ѡ��PWM1ģʽ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); // ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;		  // TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY; // ��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_CtrlPWMOutputs(TIM1, ENABLE); // ������ʱ��

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
	//������ʱ��
	TIM_Cmd(TIM3,ENABLE);
	//��Ҫ�ֶ������ж�
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); // ʹ�ܶ�ʱ��1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GP_motor_struct.timerfeq = 72000000 / psc;

	// ���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��	GPIOA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// ��ʼ��TIM8
	TIM_TimeBaseStructure.TIM_Period = 1000;					// ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;				// ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			// repeate counter must set 0
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);				// ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	//	TIM_UpdateRequestConfig(TIM8,TIM_UpdateSource_Regular);
	//	TIM_SelectOnePulseMode(TIM8,TIM_OPMode_Single); //������ģʽ

	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE); // ʹ��ָ����TIM1�ж�,��������ж�
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);	   // ���жϱ�־λ

	// ��ʼ��TIM8 Channel1 PWMģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // ���ģʽ�а��֣�ѡ��PWM1ģʽ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable); // ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM8, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;		  // TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY; // ��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_CtrlPWMOutputs(TIM8, ENABLE); // ������ʱ��

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
// ���Ÿ����ض�λ ����Ҫ������� ������λʧ��
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
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable); // �ر�Ԥת��

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PWM_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// �������PWM��ʼ��
void Motor_PWM_Init(void)
{
	TIM1_PWM_Init(APB2_PSC_FEQ);
	TIM4_PWM_Init(APB1_PSC_FEQ);
	TIM3_PWM_Init(APB1_PSC_FEQ);
	TIM8_PWM_Init(APB2_PSC_FEQ);
	TIM5_PWM_Init(APB1_PSC_FEQ);
	TIM2_PWM_Init(APB1_PSC_FEQ);
}

void TIM1_UP_IRQHandler(void) // TIM1�ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update); // ��������жϱ�־λ
		if (GE_motor_struct.dir == Front)			  // ���㵱ǰλ��
		{
			GE_motor_struct.postion++;
		}
		else
		{
			GE_motor_struct.postion--;
		}
		GE_motor_struct.step++; // ���㵱ǰ����

		if (GE_motor_struct.planSetpNumber <= GE_motor_struct.step) // ����ָ������,���ֹͣ
		{
			TIM_Cmd(TIM1, DISABLE);
			GE_motor_struct.motion = Stop;
			return;
		}
		if (GE_motor_struct.max_pos <= GE_motor_struct.postion) // ������������
		{
			TIM_Cmd(TIM1, DISABLE);
			GE_motor_struct.motion = Stop;
			return;
		}

		// �ڲ�ͬ�������ж���Ҫֹͣʱ�������ʹ�����״̬
		/***/
		//		if(GE_motor_struct.dir==Back&&GE_start_Sen==Sen_Block)
		//		{
		//			TIM_Cmd(TIM1,DISABLE);
		//			GE_motor_struct.postion=0;
		//			GE_motor_struct.motion=Stop;
		//			return;
		//		}
		/****/

		if (GE_motor_struct.motion == ConstMove) // ��������˶�
		{
			TIM_SetAutoreload(TIM1, GE_motor_struct.t_m);	// �趨�Զ���װֵ
			TIM_SetCompare1(TIM1, GE_motor_struct.t_m / 2); // ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
		}
		else if (GE_motor_struct.motion == AccMove)
		{
			// �����˶�
			if (GE_motor_struct.step <= GE_motor_struct.accStepNumber)
			{
				TIM_SetAutoreload(TIM1, GE_motor_struct.AccPeriodArray[GE_motor_struct.step]);	 // �趨�Զ���װֵ
				TIM_SetCompare1(TIM1, GE_motor_struct.AccPeriodArray[GE_motor_struct.step] / 2); // ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
			}																					 // �����˶�
			else if (GE_motor_struct.step >= GE_motor_struct.planSetpNumber - GE_motor_struct.accStepNumber)
			{
				TIM_SetAutoreload(TIM1, GE_motor_struct.AccPeriodArray[GE_motor_struct.planSetpNumber - GE_motor_struct.step]);	  // �趨�Զ���װֵ
				TIM_SetCompare1(TIM1, GE_motor_struct.AccPeriodArray[GE_motor_struct.planSetpNumber - GE_motor_struct.step] / 2); // ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
			}																													  // �����˶�
			else
			{
				TIM_SetAutoreload(TIM1, GE_motor_struct.t_m);	// �趨�Զ���װֵ
				TIM_SetCompare1(TIM1, GE_motor_struct.t_m / 2); // ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
			}
		}
	}
}

void TIM2_IRQHandler(void) // TIM2 �ж� GO_hor_motor
{
	static volatile u8 tim_cnt = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		tim_cnt++;
		tim_cnt %= 2;
		if (!tim_cnt) // ż������
		{
			if (GO_hor_motor_struct.dir == Front) // ���㵱ǰλ��
			{
				GO_hor_motor_struct.postion++;
			}
			else
			{
				GO_hor_motor_struct.postion--;
			}
			GO_hor_motor_struct.step++;											// ���㵱ǰ����
			if (GO_hor_motor_struct.planSetpNumber <= GO_hor_motor_struct.step) // ����ָ������,���ֹͣ
			{
				TIM_Cmd(TIM2, DISABLE);
				GO_hor_motor_struct.motion = Stop;
				return;
			}

			if (GO_hor_motor_struct.max_pos <= GO_hor_motor_struct.postion) // ������������
			{
				TIM_Cmd(TIM2, DISABLE);
				GO_hor_motor_struct.motion = Stop;
				return;
			}
		}
		// �ڲ�ͬ�������ж���Ҫֹͣʱ�������ʹ�����״̬
		/***/

		/****/

		if (GO_hor_motor_struct.motion == ConstMove) // ��������˶�
		{
			TIM_SetCompare1(TIM2, (TIM2->CNT + GO_hor_motor_struct.t_m) % 0xffff);
		}
		else if (GO_hor_motor_struct.motion == AccMove)
		{
			// �����˶�
			if (GO_hor_motor_struct.step <= GO_hor_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM2, (TIM2->CNT + GO_hor_motor_struct.AccPeriodArray[GO_hor_motor_struct.step]) % 0xffff);
			} // �����˶�
			else if (GO_hor_motor_struct.step > GO_hor_motor_struct.planSetpNumber - GO_hor_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM2, (TIM2->CNT + GO_hor_motor_struct.AccPeriodArray[GO_hor_motor_struct.planSetpNumber - GO_hor_motor_struct.step]) % 0xffff);
			} // �����˶�
			else
			{
				TIM_SetCompare1(TIM2, (TIM2->CNT + GO_hor_motor_struct.t_m) % 0xffff);
			}
		}
	}
}

void TIM3_IRQHandler(void) // TIM3 �ж� GC_ver_motor
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
			if (GC_ver_motor_struct.dir == Front) // ���㵱ǰλ��
			{
				GC_ver_motor_struct.postion++;
			}
			else
			{
				GC_ver_motor_struct.postion--;
			}
			GC_ver_motor_struct.step++; // ���㵱ǰ����

			if (GC_ver_motor_struct.planSetpNumber <= GC_ver_motor_struct.step) // ����ָ������,���ֹͣ
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

			if (GC_ver_motor_struct.max_pos <= GC_ver_motor_struct.postion) // ������������
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

		// �ڲ�ͬ�������ж���Ҫֹͣʱ�������ʹ�����״̬
		/***/
		//		if(GC_ver_motor_struct.planpostion==0&&GC_ver_Sen==Sen_Block)
		//		{
		//			TIM_Cmd(TIM3,DISABLE);
		//			GC_ver_motor_struct.motion=Stop;
		//			GC_ver_motor_struct.postion=0;
		//			return;

		//		}

		/****/

		if (GC_ver_motor_struct.motion == ConstMove) // ��������˶�
		{
			TIM_SetCompare1(TIM3, (TIM3->CNT + GC_ver_motor_struct.t_m) % 0xffff);
		}
		else if (GC_ver_motor_struct.motion == AccMove)
		{
			// �����˶�
			if (GC_ver_motor_struct.step <= GC_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM3, (TIM3->CNT + GC_ver_motor_struct.AccPeriodArray[GC_ver_motor_struct.step]) % 0xffff);
			} // �����˶�
			else if (GC_ver_motor_struct.step >= GC_ver_motor_struct.planSetpNumber - GC_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM3, (TIM3->CNT + GC_ver_motor_struct.AccPeriodArray[GC_ver_motor_struct.planSetpNumber - GC_ver_motor_struct.step]) % 0xffff);
			} // �����˶�
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
			if (GP_motor_big_cyl.dir == Front) // ���㵱ǰλ��
			{
				GP_motor_big_cyl.postion++;
			}
			else
			{
				GP_motor_big_cyl.postion--;
			}
			GP_motor_big_cyl.step++; // ���㵱ǰ����

			if (GP_motor_big_cyl.planSetpNumber <= GP_motor_big_cyl.step) // ����ָ������,���ֹͣ
			{
				TIM_ITConfig(TIM3,TIM_IT_CC3,DISABLE);
				GP_motor_big_cyl.motion = Stop;
				return;
			}

			if (GP_motor_big_cyl.max_pos <= GP_motor_big_cyl.postion) // ������������
			{
				TIM_ITConfig(TIM3,TIM_IT_CC3,DISABLE);
				GP_motor_big_cyl.motion = Stop;
				return;
			}
		}

		// �ڲ�ͬ�������ж���Ҫֹͣʱ�������ʹ�����״̬
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

		if (GP_motor_big_cyl.motion == ConstMove) // ��������˶�
		{
			TIM_SetCompare3(TIM3, (TIM3->CNT + GP_motor_big_cyl.t_m) % 0xffff);
		}
		else if (GP_motor_big_cyl.motion == AccMove)
		{
			// �����˶�
			if (GP_motor_big_cyl.step <= GP_motor_big_cyl.accStepNumber)
			{
				TIM_SetCompare3(TIM3, (TIM3->CNT + GP_motor_big_cyl.AccPeriodArray[GP_motor_big_cyl.step]) % 0xffff);
			} // �����˶�
			else if (GP_motor_big_cyl.step >= GP_motor_big_cyl.planSetpNumber - GP_motor_big_cyl.accStepNumber)
			{
				TIM_SetCompare3(TIM3, (TIM3->CNT + GP_motor_big_cyl.AccPeriodArray[GP_motor_big_cyl.planSetpNumber - GP_motor_big_cyl.step]) % 0xffff);
			} // �����˶�
			else
			{
				TIM_SetCompare3(TIM3, (TIM3->CNT + GP_motor_big_cyl.t_m) % 0xffff);
			}
		}
	}
	
	#endif
	
}

void TIM4_IRQHandler(void) // TIM4 �ж� GO_ver_motor
{
	static volatile u8 tim_cnt;
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
		tim_cnt++;
		tim_cnt %= 2;
		if (!tim_cnt)
		{
			if (GO_ver_motor_struct.dir == Front) // ���㵱ǰλ��
			{
				GO_ver_motor_struct.postion++;
			}
			else
			{
				GO_ver_motor_struct.postion--;
			}
			GO_ver_motor_struct.step++; // ���㵱ǰ����

			if (GO_ver_motor_struct.planSetpNumber <= GO_ver_motor_struct.step) // ����ָ������,���ֹͣ
			{
				TIM_Cmd(TIM4, DISABLE);
				GO_ver_motor_struct.motion = Stop;
				return;
			}
			if (GO_ver_motor_struct.max_pos <= GO_ver_motor_struct.postion) // ����������
			{
				TIM_Cmd(TIM4, DISABLE);
				GO_ver_motor_struct.motion = Stop;
				GO.Storage_full=TRUE;
				return;
			}
		}

		// �ڲ�ͬ�������ж���Ҫֹͣʱ�������ʹ�����״̬
		/***/
		if (GO.task == GO_Box_In && GO.subtask == 0) //�洢�н��뵽��ⲻ���� ��ԭ��
		{
			if (GOV_box_Sen != Sen_Block)
			{
				TIM_Cmd(TIM4, DISABLE);
				GO_ver_motor_struct.motion = Stop;
				return;
			}
		}

		/****/

		if (GO_ver_motor_struct.motion == ConstMove) // ��������˶�
		{
			TIM_SetCompare1(TIM4, (TIM4->CNT + GO_ver_motor_struct.t_m) % 0xffff);
		}
		else
		{
			// �����˶�
			if (GO_ver_motor_struct.step <= GO_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM4, (TIM4->CNT + GO_ver_motor_struct.AccPeriodArray[GO_ver_motor_struct.step]) % 0xffff);
			} // �����˶�
			else if (GO_ver_motor_struct.step >= GO_ver_motor_struct.planSetpNumber - GO_ver_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM4, (TIM4->CNT + GO_ver_motor_struct.AccPeriodArray[GO_ver_motor_struct.planSetpNumber - GO_ver_motor_struct.step]) % 0xffff);
			} // �����˶�
			else
			{
				TIM_SetCompare1(TIM4, (TIM4->CNT + GO_ver_motor_struct.t_m) % 0xffff);
			}
		}
	}
}

void TIM5_IRQHandler(void) // TIM5 �ж� GC_rot_motor
{
	static volatile u8 tim_cnt = 0;
	if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
		tim_cnt++;
		tim_cnt %= 2;
		if (!tim_cnt)
		{
			if (GC_rot_motor_struct.dir == Front) // ���㵱ǰλ��
			{
				GC_rot_motor_struct.postion++;
			}
			else
			{
				GC_rot_motor_struct.postion--;
			}
			GC_rot_motor_struct.step++; // ���㵱ǰ����

			if (GC_rot_motor_struct.planSetpNumber <= GC_rot_motor_struct.step) // ����ָ������,���ֹͣ
			{
				TIM_Cmd(TIM5, DISABLE);
				GC_rot_motor_struct.motion = Stop;
				return;
			}

			if(GC_rot_motor_struct.max_pos<= GC_rot_motor_struct.postion) //����ָ������,���ֹͣ
			{
				TIM_Cmd(TIM5,DISABLE);
				GC_rot_motor_struct.motion=Stop;
				return;
			}
		}

		// �ڲ�ͬ�������ж���Ҫֹͣʱ�������ʹ�����״̬
		/***/
		//		if(GC_rot_motor_struct.dir==Back&&GC_rot_Sen==Sen_Block)
		//		{
		//			TIM_Cmd(TIM5,DISABLE);
		//			GC_rot_motor_struct.motion=Stop;
		//			GC_rot_motor_struct.postion=0;
		//			return;
		//		}

		/****/

		if (GC_rot_motor_struct.motion == ConstMove) // ��������˶�
		{
			TIM_SetCompare1(TIM5, (TIM5->CNT + GC_rot_motor_struct.t_m) % 0xffff);
		}
		else
		{
			// �����˶�
			if (GC_rot_motor_struct.step <= GC_rot_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM5, (TIM5->CNT + GC_rot_motor_struct.AccPeriodArray[GC_rot_motor_struct.step]) % 0xffff);
			} // �����˶�
			else if (GC_rot_motor_struct.step >= GC_rot_motor_struct.planSetpNumber - GC_rot_motor_struct.accStepNumber)
			{
				TIM_SetCompare1(TIM5, (TIM5->CNT + GC_rot_motor_struct.AccPeriodArray[GC_rot_motor_struct.planSetpNumber - GC_rot_motor_struct.step]) % 0xffff);
			} // �����˶�
			else
			{
				TIM_SetCompare1(TIM5, (TIM5->CNT + GC_rot_motor_struct.t_m) % 0xffff);
			}
		}
	}
}

void TIM8_UP_IRQHandler(void) // TIM8�ж� GP_motor
{
	if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM8, TIM_FLAG_Update); // ��������жϱ�־λ
		if (GP_motor_struct.dir == Front)			  // ���㵱ǰλ��
		{
			GP_motor_struct.postion++;
		}
		else
		{
			GP_motor_struct.postion--;
		}
		GP_motor_struct.step++; // ���㵱ǰ����

		if (GP_motor_struct.planSetpNumber <= GP_motor_struct.step) // ����ָ������,���ֹͣ
		{
			TIM_Cmd(TIM8, DISABLE);
			GP_motor_struct.motion = Stop;
			return;
		}

		if (GP_motor_struct.max_pos <= GP_motor_struct.postion) // ������������
		{
			TIM_Cmd(TIM8, DISABLE);
			GP_motor_struct.motion = Stop;
			return;
		}

		//		//�ڲ�ͬ�������ж���Ҫֹͣʱ�������ʹ�����״̬
		//		/***/
		//		if(GP_motor_struct.dir==Back&&GP_start_Sen==Sen_Block)
		//		{
		//			TIM_Cmd(TIM8,DISABLE);
		//			GP_motor_struct.motion=Stop;
		//			GP_motor_struct.postion=0;
		//			return;

		//		}

		/****/

		if (GP_motor_struct.motion == ConstMove) // ��������˶�
		{
			TIM_SetAutoreload(TIM8, GP_motor_struct.t_m);	// �趨�Զ���װֵ
			TIM_SetCompare1(TIM8, GP_motor_struct.t_m / 2); // ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
		}
		else
		{
			// �����˶�
			if (GP_motor_struct.step <= GP_motor_struct.accStepNumber)
			{
				TIM_SetAutoreload(TIM8, GP_motor_struct.AccPeriodArray[GP_motor_struct.step]);	 // �趨�Զ���װֵ
				TIM_SetCompare1(TIM8, GP_motor_struct.AccPeriodArray[GP_motor_struct.step] / 2); // ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
			}																					 // �����˶�
			else if (GP_motor_struct.step >= GP_motor_struct.planSetpNumber - GP_motor_struct.accStepNumber)
			{
				TIM_SetAutoreload(TIM8, GP_motor_struct.AccPeriodArray[GP_motor_struct.planSetpNumber - GP_motor_struct.step]); // �趨�Զ���װֵ
				TIM_SetCompare1(TIM8, GP_motor_struct.AccPeriodArray[GP_motor_struct.planSetpNumber - GP_motor_struct.step]);	// ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
			}																													// �����˶�
			else
			{
				TIM_SetAutoreload(TIM8, GP_motor_struct.t_m);	// �趨�Զ���װֵ
				TIM_SetCompare1(TIM8, GP_motor_struct.t_m / 2); // ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
			}
		}
	}
}

//			///////////////////////////////////
//			TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);//��������жϱ�־λ
//			TIM_SetAutoreload(TIM1,1000);//�趨�Զ���װֵ
//			TIM_SetCompare1(TIM1,500); //ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%
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
