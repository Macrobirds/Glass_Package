#include "position.h"

static void Exti_8_15_GPIO_Init(void) 
{ 
	// PE8-15
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|
		 GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//���ó��������� 
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
	//GPIO ��ʼ��
	Exti_0_7_GPIO_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	if(IoInput_0_Enable)
	{
		//GPIOD 0
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_Init(&EXTI_InitStructure);
	}
		if(IoInput_1_Enable)
	{
		//GPIOD 1
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line1;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_2_Enable)
	{
		//GPIOC 2
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line2;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_3_Enable)
	{
		//GPIOD 3
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource3);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line3;
		EXTI_Init(&EXTI_InitStructure);
	}
	
	if(IoInput_4_Enable)
	{
		//GPIOD 4
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource4);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line4;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_5_Enable)
	{
		//GPIOD 5
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource5);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_6_Enable)
	{
		//GPIOD 6
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource6);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line6;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_7_Enable)
	{
		//GPIOD 7
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line7;
		EXTI_Init(&EXTI_InitStructure);
	}
	
	
	
}
static void Exti_Init_8_15(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	//	GPIO��ʼ��
	Exti_8_15_GPIO_Init();	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

	//  �ж����Լ��жϳ�ʼ������
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	if(IoInput_15_Enable)
	{
		//GPIOE.15
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource15);
		//�����ش��� EXTI_Trigger_Rising_Falling; ˫���ش���
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_Line = EXTI_Line15;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_14_Enable)
	{
		//GPIOE.14
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource14);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line14;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_13_Enable)
	{
		//GPIOE.13
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource13);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line13;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_12_Enable)
	{
		//GPIOE.12
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource12);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line12;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_11_Enable)
	{
		//GPIOE.11
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource11);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line11;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_10_Enable)
	{
		//GPIOE.10
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource10);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line10;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_9_Enable)
	{
		//GPIOE.9
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource9);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
		EXTI_InitStructure.EXTI_Line = EXTI_Line9;
		EXTI_Init(&EXTI_InitStructure);
	}
	if(IoInput_8_Enable)
	{
		//GPIOE.8
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource8);
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
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
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_7_Enable
		|| IoInput_8_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
}

//λ�ô������ⲿ�жϳ�ʼ��
void postions_sensor_Init(void)
{
	 NVIC_InitTypeDef NVIC_InitStructure;
	Exti_Init_0_7();
	Exti_Init_8_15();
	//�жϿ���������
		if(IoInput_15_Enable
		|| IoInput_14_Enable
		|| IoInput_13_Enable
		|| IoInput_12_Enable
		|| IoInput_11_Enable
		|| IoInput_10_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
		if(IoInput_5_Enable
		||IoInput_6_Enable
		||IoInput_7_Enable
		|| IoInput_8_Enable
		||IoInput_9_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_0_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_1_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
	if(IoInput_2_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
		if(IoInput_3_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
		if(IoInput_4_Enable)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//�ж�����
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		NVIC_Init(&NVIC_InitStructure);
	}
}

void EXTI0_IRQHandler(void)
{
		//GOH_mid_Sen
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET) //ˮƽ���ϵ������������
	{
		if(GO_hor_motor_struct.dir==Front) //���յ㷽��
		{
			delay_us(50);
			if(GOH_mid_Sen==Sen_Block) //���﹤����ֹͣ
			{
				TIM_Cmd(TIM2,DISABLE);
				GO_hor_motor_struct.postion=GO.GOH_mid_pos;
				GO_hor_motor_struct.motion=Stop;
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
	if(EXTI_GetITStatus(EXTI_Line3)!=RESET) //��Ƭ�豸����ԭ��
	{
		delay_us(50);
		if(GOH_start_Sen==Sen_Block)//��Ƭ�豸ֹͣ
		{
			TIM_Cmd(TIM2,DISABLE);
			GO_hor_motor_struct.postion=0;
			GO_hor_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line3);
		printf("exti 3\r\n");
	}
}
void EXTI4_IRQHandler(void);


void EXTI9_5_IRQHandler(void)
{
	//GOH_end_Sen
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET) //ˮƽ���ϵ����յ�
	{
		delay_us(50);
		if(GOH_end_Sen==Sen_Block) 
		{
			TIM_Cmd(TIM2,DISABLE);
			GO_hor_motor_struct.postion=GO.GOH_end_pos;
			GO_hor_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	//GOV_start_Sen
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET) //��ֱ���ϲ۵���ԭ��
	{
		delay_us(50);
		if(GOV_start_Sen==Sen_Block)
		{
			TIM_Cmd(TIM4,DISABLE);
			GO_ver_motor_struct.postion=0;
			GO_ver_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	//GOV_galss_Sen
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET) //��⵽�洢�ۿ�
	{
		delay_us(50);
		if(GO.task>GO_Box_Out)
		{
			if(GOV_glass_Sen!=Sen_Block) //�½��ش���
			{
				TIM_Cmd(TIM4,DISABLE);
				GO_ver_motor_struct.motion=Stop;
			}
		}else if(GO.task==GO_Box_In&&GO.subtask==2)
		{
			if(GOV_glass_Sen!=Sen_Block) //�½��ش���
			{
				TIM_Cmd(TIM4,DISABLE);
				GO_ver_motor_struct.motion=Stop;
			}
		}
		printf("exit7 trigger\r\n");
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	//GE_down_Sen
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET) //���ϲ۶�������
	{
		delay_us(50);
		
		printf("exit8 trigger\r\n");
		if(GE.task>=GE_move_front&&GE.task<=GE_move_back)
		{
			if(GE_down_Sen==Sen_Block&&GE_up_Sen!=Sen_Block) //ֻ���¶�����
			{
				TIM_Cmd(TIM1,DISABLE);
				GE_motor_struct.motion=Stop;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	//GE_up_Sen
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET) ///���ϲ۶�������
	{
		delay_us(50);
		if(GE.task>=GE_move_front&&GE.task<=GE_move_back)
		{
			if(GE_up_Sen==Sen_Block) //ֻ���϶�����
			{
				TIM_Cmd(TIM1,DISABLE);
				GE_motor_struct.motion=Stop;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line9);
	}

}
void EXTI15_10_IRQHandler(void)
{
	printf("exti15-10\r\n");
	//GE_start_Sen
	if(EXTI_GetITStatus(EXTI_Line10)!=RESET) //���ϲ��ƶ���ԭ��
	{
			delay_us(50);
			if(GE_start_Sen==Sen_Block)//ԭ���Ӧ
			{
				TIM_Cmd(TIM1,DISABLE);
				GE_motor_struct.postion=0;
				GE_motor_struct.motion=Stop;
			}
			printf("GE_start_Sen\r\n");
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
	//GC_rot_Sen
	if(EXTI_GetITStatus(EXTI_Line11)!=RESET) //������ת��ԭ��
	{
		delay_us(50);
		if(GC_rot_Sen!=Sen_Block) //������תֹͣ
		{
			TIM_Cmd(TIM5,DISABLE);
			GC_rot_motor_struct.postion=0;
			GC_rot_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
		printf("GC_rot_Sen\r\n");
	}
	//GC_ver_Sen
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET) //�����˶�����ֱԭ��
	{
		delay_us(50);
		if(GC_ver_Sen==Sen_Block) //���ִ�ֱֹͣ
		{
			TIM_Cmd(TIM3,DISABLE);
			GCV_motor_Break=GAS_DISABLE;
			GC_ver_motor_struct.postion=0;
			GC_ver_motor_struct.motion=Stop;
		}

		EXTI_ClearITPendingBit(EXTI_Line12);
		printf("GC_ver_sen\r\n");
	}
	//GP_start_Sen
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET) //��Ƭ�˶���ԭ��
	{
		delay_us(50);
		if(GP_start_Sen==Sen_Block)//��Ƭ�豸ֹͣ
		{
			TIM_Cmd(TIM8,DISABLE);
			GP_motor_struct.postion=0;
			GP_motor_struct.motion=Stop;
		}
		EXTI_ClearITPendingBit(EXTI_Line13);
		printf("Exti 13\r\n");
	}

	
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET) //��Ƭ�豸����ԭ��
	{

		EXTI_ClearITPendingBit(EXTI_Line15);
		printf("exti 15\r\n");
	}



}

//	printf("exit8 trigger\r\n");
//		if(GE_start_Sen==Sen_Block)
//		{
//	//		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
//			TIM_SetCompare1(TIM2,(TIM2->CNT+1000)%0xffff);
//			TIM_Cmd(TIM2,ENABLE);
//			
//			
////	TIM_SetAutoreload(TIM1,1000);//�趨�Զ���װֵ	
////	TIM_SetCompare1(TIM1,500); //ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%	
////	TIM_Cmd(TIM1,ENABLE);

//			
//		}else
//		{
//			TIM_Cmd(TIM2, DISABLE);  //�ر�TIM8				
//		}
//		EXTI_ClearITPendingBit(EXTI_Line8);