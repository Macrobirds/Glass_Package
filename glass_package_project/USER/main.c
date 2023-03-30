#include "sys.h" 	
#include "delay.h"	
#include "usart.h"
#include "usmart.h"
#include "Globalconfig.h"
#include "includes.h"

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
////LED0����
////�����������ȼ�
//#define LED0_TASK_PRIO       			7 
////���������ջ��С
//#define LED0_STK_SIZE  		    		64
////�����ջ	
//OS_STK LED0_TASK_STK[LED0_STK_SIZE];
////������
//void led0_task(void *pdata);


//��ѭ������
//�����������ȼ�
#define MAIN_TASK_PRIO       			8 
//���������ջ��С
#define MAIN_STK_SIZE  					128
//�����ջ
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//������
void main_task(void *pdata);

//��ѹ��������
//�����������ȼ�
#define GAS_TASK_PRIO       			7 
//���������ջ��С
#define GAS_STK_SIZE  					128
//�����ջ
OS_STK GAS_TASK_STK[GAS_STK_SIZE];
//������
void gas_task(void *pdata);

//ͨѶ��������
//�����������ȼ�
#define UART_RECEIVE_TASK_PRIO       			5 
//���������ջ��С
#define UART_RECEIVE_STK_SIZE  					128
//�����ջ
OS_STK UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE];
//������
void uart_receivetask(void *pdata);


//ͨѶ��������
#define UART_SEND_TASK_PRIO       			6 
//���������ջ��С
#define UART_SEND_STK_SIZE  					128
//�����ջ
OS_STK UART_SEND_TASK_STK[UART_SEND_STK_SIZE];
//������
void uart_sendtask(void *pdata);


 int main(void)
 {	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    //��ʱ������ʼ��	  
	ALL_GPIO_Init();
	uart_init(115200);
	usmart_dev.init(SystemCoreClock/1000000); //��ʹ�ö�ʱ���ж� ����������ɨ��
	postions_sensor_Init(); //λ�ô������ⲿ�жϼ��
	Gas_Init();  //����������ѹ��������ʼ��
	Motor_PWM_Init(); //���������ʼ��
	my_mem_init(SRAMIN); //��ʼ���ڴ��
	delay_ms(100);
	//��ʼ��ϵͳ���в��� 
	//���ڴ��ȡ

	//����ΪĬ�ϲ���
	motor_parameter_Init(); //��ʼ���������
	TaskThread_Init(); //��ʼ����������ʱ�� �������
	OSInit();   
 	OSTaskCreate(gas_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	  	 
}
	  
//��ʼ����
void start_task(void *pdata)
{
   OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);						   
 	OSTaskCreate(gas_task,(void *)0,(OS_STK*)&GAS_TASK_STK[GAS_STK_SIZE-1],GAS_TASK_PRIO);	 	
	OSTaskCreate(uart_receivetask,(void *)0,(OS_STK*)&UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE-1],UART_RECEIVE_TASK_PRIO);
 	OSTaskCreate(uart_sendtask,(void *)0,(OS_STK*)&UART_SEND_TASK_STK[UART_SEND_STK_SIZE-1],UART_SEND_TASK_PRIO);		
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}



void main_task(void *pdata)
{
	static u32 delay_20ms=0;
	//�ȴ���ѹ�ȶ�
	while(Gas_pump_Func(Global_Parm.GP.spray_pressure)!=gas_pumped)
	{
		OSTimeDlyHMSM(0,0,0,200);
	}
	//�����豸����
	TaskThread_State=TRUE;
	//������Ḵλ У׼��ʼλ��
	Boot_ResetTaskThread();
	//�ȴ�У׼�������
	while(GE.task==GE_none&&GC.task==GC_none
		&&GP.task==GP_none&&GO.task==GO_none)
	{
		OSTimeDlyHMSM(0,0,0,200);
	}
	//���ϵͳ�Ƿ��ڿ�����״̬
	TaskThread_IsReady();
	
	//��ʼϵͳ����
	while(1)
	{
		delay_20ms++;
		if(delay_20ms>=30) //1s���
		{
			delay_20ms=0;
			//usmart ɨ��
			usmart_dev.scan(); 
		}
		
		
		
		OSTimeDlyHMSM(0,0,0,20);
	}


}

void gas_task(void *pdata)
{
	


}

void uart_receivetask(void *pdata)
{

}

void uart_sendtask(void *pdata)
{

}



