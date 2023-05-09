#include "sys.h" 	
#include "delay.h"	
#include "usart.h"
#include "usmart.h"
#include "Globalconfig.h"


/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			4 //��ʼ��������ȼ�����Ϊ���
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
#define MAIN_STK_SIZE  					64
//�����ջ
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//������
void main_task(void *pdata);

//��ѹ��������
//�����������ȼ�
#define GAS_TASK_PRIO       			7 
//���������ջ��С
#define GAS_STK_SIZE  					64
//�����ջ
OS_STK GAS_TASK_STK[GAS_STK_SIZE];
//������
void gas_task(void *pdata);

//ͨѶ��������
//�����������ȼ�
#define UART_RECEIVE_TASK_PRIO       			10 
//���������ջ��С
#define UART_RECEIVE_STK_SIZE  					64
//�����ջ
OS_STK UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE];
//������
void uart_receivetask(void *pdata);


//ͨѶ��������
#define UART_SEND_TASK_PRIO       			9
//���������ջ��С
#define UART_SEND_STK_SIZE  					64
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
	uart2_init(115200);
	MYDMA_Config_Usart2(DMA1_Channel6,(u32)&USART2->DR,(u32)USART2_RX_BUF,USART2_REC_LEN);
	delay_ms(1000);
	postions_sensor_Init(); //λ�ô������ⲿ�жϼ��
	Gas_Init();  //����������ѹ��������ʼ��
	delay_ms(100);
	Motor_PWM_Init(); //���������ʼ��
	delay_ms(100);
	
	usmart_dev.init(SystemCoreClock/1000000); //��ʹ�ö�ʱ���ж� ����������ɨ��
	my_mem_init(SRAMIN); //��ʼ���ڴ��
	delay_ms(100);
	//��ʼ��ϵͳ���в��� 
	//���ڴ��ȡ

	//����ΪĬ�ϲ���
//	motor_parameter_Init(); //��ʼ���������
	
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	  	 
}
	  
//��ʼ����
void start_task(void *pdata)
{
	pdata = pdata; 
  OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);						   
 	OSTaskCreate(gas_task,(void *)0,(OS_STK*)&GAS_TASK_STK[GAS_STK_SIZE-1],GAS_TASK_PRIO);	 	
	OSTaskCreate(uart_receivetask,(void *)0,(OS_STK*)&UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE-1],UART_RECEIVE_TASK_PRIO);
 	OSTaskCreate(uart_sendtask,(void *)0,(OS_STK*)&UART_SEND_TASK_STK[UART_SEND_STK_SIZE-1],UART_SEND_TASK_PRIO);		
	TaskThread_Init(); //��ʼ����������ʱ�� �������
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}



void main_task(void *pdata)
{

	static u32 delay_20ms=0;
	//�ȴ���ѹ�ȶ�
	printf("main task\r\n");
	while(Gas_State==gas_pumping)
	{
		OSTimeDlyHMSM(0,0,0,200);
	}

//	//�����Զ���λ���� ������Ḵλ У׼��ʼλ��
//	Boot_ResetTaskThread();
//	//�ȴ���λ�������
//	while (!TaskThread_CheckIdle())
//	{
//		OSTimeDlyHMSM(0,0,0,200);
//	}
//	

//	// //���ϵͳ״̬ �ȴ�ϵͳ���ڿ�����״̬
//	while(TaskThread_IsReady()!=taskthread_ready)
//	{
//	 	OSTimeDlyHMSM(0,0,0,200);
//	}
//	
//	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)  
//	//�����豸����
//	TaskThread_State=taskthread_ready;
//	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)

	//��ʼϵͳ����
	while(1)
	{
		delay_20ms++;
		if(delay_20ms>=30) //1s���
		{
			//���ϵͳ�Ƿ���ڴ���
			if(TaskThread_State==taskthread_running)
			{
				//TaskThread_IsReady();
			}
			delay_20ms=0;
			//usmart ɨ��
			usmart_dev.scan(); 
			//���ڽ�������

			//����������
			//printf("main loop\r\n");
		}
		
		
		
		OSTimeDlyHMSM(0,0,0,20);
	}


}

void gas_task(void *pdata)
{
	printf("gas task\r\n");
	while (1)
	{
		if(TaskThread_State<taskthread_close)
		{
			Gas_State=Gas_pump_Func();

		}else //�ػ�����
		{
			if(TaskThread_CheckIdle()) //�ȴ�������к���з���
			{
				Gas_State=Gas_release_Func();
			}
			
		}


		OSTimeDlyHMSM(0,0,0,400);
	}
	
	


}

//���ڽ�������
void uart_receivetask(void *pdata)
{
	printf("uart receive task\r\n");
    while(1)
    {
    OSTimeDlyHMSM(0, 0, 0, 500);
//	  dmaRecv_makeProtocol_uart2();
	}
}

//���ڷ�������
void uart_sendtask(void *pdata)
{
	printf("uart send task\r\n");
	   while(1)
    {
    OSTimeDlyHMSM(0, 0, 0, 10);
//	  dmaRecv_makeProtocol_uart2();
	}

}



