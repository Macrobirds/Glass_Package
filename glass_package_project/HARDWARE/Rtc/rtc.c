#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h" 		    
#include "Globalconfig.h"
//Mini STM32������
//RTCʵʱʱ�� ��������			 
//����ԭ��@ALIENTEK
//2010/6/6
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

#define LowSpeedCrystalSelectLSE  1 //�Ƿ�ʹ���ⲿ����
	   
_calendar_obj calendar;//ʱ�ӽṹ�� 
 
static void RTC_NVIC_Config(void)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

//��ʱ��2�жϷ������	 
void TIM7_IRQHandler(void)
{ 		    		  			    
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET)//����ж�
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //����жϱ�־λ 
		nowRtcTime++;
		LED1 = !LED1;
		
//		// ��ϵȴ�����
//		if(alarmEnable_mixture)
//			if(nowRtcTime >= alarmSeccount_mixture)
//				alarmEnable_mixture = OFF;
//			
//		// ���Ŷ����ȴ�����
//		if(alarmEnable_suckTopWait)
//		{
//			if(nowRtcTime >= alarmSeccount_suckTopWait)
//			{
//				if(deviceState.main_motion == Motion_suckTopWait && deviceState.main_motionState == MotionState_executing)
//				{
//					deviceState.main_motionState = MotionState_executeFinish;
//				}
//				alarmEnable_suckTopWait = 0;
//			}
//		}
//		
//		// ���ŵײ��ȴ�����
//		if(alarmEnable_suckBottomWait)
//		{
//			if(nowRtcTime >= alarmSeccount_suckBottomWait)
//			{
//				if(deviceState.main_motion == Motion_suckBottomWait && deviceState.main_motionState == MotionState_executing)
//				{
//					deviceState.main_motionState = MotionState_executeFinish;
//				}
//				alarmEnable_suckBottomWait = 0;
//			}
//		}
//		
//		// ���迪ʼʱ�ȴ�����
//		if(alarmEnable_stepWait)
//		{
//			if(nowRtcTime >= alarmSeccount_stepWait)
//			{
//				if(deviceState.main_motion == Motion_stepWait 
//					&& deviceState.main_motionState == MotionState_executing)
//				{
//					deviceState.main_motionState = MotionState_executeFinish;
//				}
//				alarmEnable_stepWait = OFF;
//			}
//		}
//		
//		// ������ɣ���������
//		if(hardwareState.completeSound
//			&& alarmEnable_completeSound)
//		{
//			BUZZER = !BUZZER_STATE;
//		}
		
	}				   
}
// ���ڿ��ƣ��Զ���װֵ��Ԥ��Ƶֵ
 void TIM7_Int_Init(u16 arr,u16 psc)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM5��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM7, TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM5�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //TIM5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM7, ENABLE);  //ʹ��TIMx		
}

//ʵʱʱ������
//��ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
//BKP->DR1���ڱ����Ƿ��һ�����õ�����
//����0:����
//����:�������

//u8 RTC_Init(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
//{
//	//����ǲ��ǵ�һ������ʱ��
//	u8 temp=0;
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
//	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  
////	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
////		{	 			
//		//BKP_DeInit();	//��λ�������� 	
//		RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
//		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//���ָ����RCC��־λ�������,�ȴ����پ������
//			{
//			temp++;
//			delay_ms(10);
//			}
//		if(temp>=250)
//		{
//			LED2 = LED_ON;
//			return 1;
//		}//��ʼ��ʱ��ʧ��,����������	    
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
//		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
//		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
//		RTC_WaitForSynchro();		//�ȴ�RTC�Ĵ���ͬ��  
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
//		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
//		RTC_EnterConfigMode();/// ��������	
//		RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
//		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
//		RTC_Set(syear,smon, sday, hour, min, sec);  //����ʱ��	
//		RTC_ExitConfigMode(); //�˳�����ģʽ  
////		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ�����д���û���������
////		}
////	else//ϵͳ������ʱ
////		{

////		RTC_WaitForSynchro();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
////		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//ʹ��RTC���ж�
////		//RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
////		//RTC_ITConfig(RTC_IT_ALR, ENABLE);
////		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
////		}
//	RTC_NVIC_Config();//RCT�жϷ�������		    				     
//	RTC_Get();//����ʱ��	
//	return 0; //ok

//}		 	
u8 RTC_Init(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	//����ǲ��ǵ�һ������ʱ��
	u8 temp=0;
//	u32 temp1;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);        //ʹ��PWR��BKP����ʱ��   
	PWR_BackupAccessCmd(ENABLE);        //ʹ�ܺ󱸼Ĵ�������  
//	temp1=BKP_ReadBackupRegister(BKP_DR1);
#if  LowSpeedCrystalSelectLSE   //�ⲿ�ֱ���ѡ��
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)                //��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
	{                                 
		BKP_DeInit();        //��λ��������         
		RCC_LSEConfig(RCC_LSE_ON);        //�����ⲿ���پ���(LSE),ʹ��������پ���
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)        //���ָ����RCC��־λ�������,�ȴ����پ������
		{
			temp++;
			delay_ms(10);
		}
		if(temp>=250)
		{
			LED2 = LED_ON;
			return 1;//��ʼ��ʱ��ʧ��,����������  
		}              
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                //����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);        //ʹ��RTCʱ��  
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();                //�ȴ�RTC�Ĵ���ͬ��  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);                //ʹ��RTC���ж�
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();/// ��������        
		RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_Set(syear,smon, sday, hour, min, sec);  //����ʱ��	      
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);        //��ָ���ĺ󱸼Ĵ�����д���û���������
	}
	else//ϵͳ������ʱ
	{
		RTC_WaitForSynchro();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);        //ʹ��RTC���ж�
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
	}                
#else                             //�ڲ����پ���ѡ��   40KHZ
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5040)                //��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
	{                                 
		BKP_DeInit();        //��λ��������         
		RCC_LSICmd(ENABLE);        //�����ⲿ���پ���(LSE),ʹ��������پ���
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET&&temp<250)        //���ָ����RCC��־λ�������,�ȴ����پ������
				{
				temp++;
				delay_ms(10);
				}
		if(temp>=250){
			LED2 = LED_ON;
			return 1;//��ʼ��ʱ��ʧ��,����������  
		}             
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);                //����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);        //ʹ��RTCʱ��  
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();                //�ȴ�RTC�Ĵ���ͬ��  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);                //ʹ��RTC���ж�
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();/// ��������        
		RTC_SetPrescaler(39999); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_Set(syear,smon, sday, hour, min, sec);  //����ʱ��	       
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5040);        //��ָ���ĺ󱸼Ĵ�����д���û���������
		}
		else//ϵͳ������ʱ
		{
		RCC_LSICmd(ENABLE);        //�����ⲿ���پ���(LSE),ʹ��������پ���
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET&&temp<250)        //���ָ����RCC��־λ�������,�ȴ����پ������
				{
				temp++;
				delay_ms(10);
				}
		if(temp>=250)
		{
			LED2 = LED_ON;
			return 1;//��ʼ��ʱ��ʧ��,����������  
		}          
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������        
		RTC_WaitForSynchro();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);        //ʹ��RTC���ж�
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();/// ��������        
		RTC_SetPrescaler(39999); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();        //�ȴ����һ�ζ�RTC�Ĵ�����д�������                        
	}                
#endif

	RTC_NVIC_Config();//RCT�жϷ�������                                                         
	RTC_Get();//����ʱ��        
	return 0; //ok

}
//RTCʱ���ж�
//ÿ�봥��һ��  
//extern u16 tcnt; 
void RTC_IRQHandler(void)
{		  
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//�����ж�
	{							
//		RTC_Get();//����ʱ��   
//		LED1 = !LED1;
//		
		RTC_ClearITPendingBit(RTC_IT_SEC);
		RTC_WaitForLastTask();	
 	}
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//�����ж�
	{	
		RTC_ClearITPendingBit(RTC_IT_ALR);		//�������ж�	
		RTC_WaitForLastTask();	
  } 
//	RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);		//�������ж�
//	RTC_WaitForLastTask();	
}
//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�	  
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//��������ݵ��������
	{
		if(Is_Leap_Year(t))seccount+=31622400;//�����������
		else seccount+=31536000;			  //ƽ���������
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //��ǰ���·ݵ����������
	{
		seccount+=(u32)mon_table[t]*86400;//�·����������
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������	   
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ���������� 
	seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;	 //����������
	seccount+=sec;//�������Ӽ���ȥ

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
	PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
	RTC_WaitForLastTask();
	RTC_EnterConfigMode();/// ��������	
	RTC_SetCounter(seccount);	//����RTC��������ֵ
	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_ExitConfigMode(); //�˳�����ģʽ  	
	return 0;	    
}



//��ʼ������		  
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//syear,smon,sday,hour,min,sec�����ӵ�������ʱ����   
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//��������ݵ��������
	{
		if(Is_Leap_Year(t))seccount+=31622400;//�����������
		else seccount+=31536000;			  //ƽ���������
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //��ǰ���·ݵ����������
	{
		seccount+=(u32)mon_table[t]*86400;//�·����������
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������	   
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ���������� 
	seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;	 //����������
	seccount+=sec;//�������Ӽ���ȥ 			    
//	//����ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
//	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  
//	//���������Ǳ����!
	RTC_SetAlarm(seccount);
	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	return 0;	    
}


//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
    timecount=RTC_GetCounter();	 
	nowRtcTime = timecount;
 	temp=timecount/86400;   //�õ�����(��������Ӧ��)
	if(daycnt!=temp)//����һ����
	{	  
		daycnt=temp;
		temp1=1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else {temp1++;break;}  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		calendar.w_year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;	//�õ��·�
		calendar.w_date=temp+1;  	//�õ����� 
	}
	temp=timecount%86400;     		//�õ�������   	   
	calendar.hour=temp/3600;     	//Сʱ
	calendar.min=(temp%3600)/60; 	//����	
	calendar.sec=(temp%3600)%60; 	//����
	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//��ȡ����   
	return 0;
}	 
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//������������������� 
//����ֵ�����ں�																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,�������100  
	if (yearH>19)yearL+=100;
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  
