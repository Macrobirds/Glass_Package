#include "sys.h" 	
#include "delay.h"	
#include "usart.h"
#include "usmart.h"
#include "Globalconfig.h"

/*�״�������־λ*/
u8 firstStartFlag=0;
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
 			   

//��ѭ������
//�����������ȼ�
#define MAIN_TASK_PRIO       			9 
//���������ջ��С
#define MAIN_STK_SIZE  					256
//�����ջ
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//������
void main_task(void *pdata);

//��ѹ��������
//�����������ȼ�
#define GAS_TASK_PRIO       			10
//���������ջ��С
#define GAS_STK_SIZE  					64
//�����ջ
OS_STK GAS_TASK_STK[GAS_STK_SIZE];
//������
void gas_task(void *pdata);

//ͨѶ��������
//�����������ȼ�
#define UART_RECEIVE_TASK_PRIO       			7 
//���������ջ��С
#define UART_RECEIVE_STK_SIZE  					128
//�����ջ
OS_STK UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE];
//������
void uart_receivetask(void *pdata);


//ͨѶ��������
#define UART_SEND_TASK_PRIO       			8
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
	delay_ms(100);
	uart3_init(115200);
	delay_ms(100);
	MYDMA_Config_Usart3(DMA1_Channel3,(u32)&USART3->DR,(u32)USART3_RX_BUF,USART3_REC_LEN);	// ��ʼ������3DMA����
	delay_ms(1000);
	postions_sensor_Init(); //λ�ô������ⲿ�жϼ��
	Gas_Init();  //����������ѹ��������ʼ��
	delay_ms(100);
	Motor_PWM_Init(); //���������ʼ��
	delay_ms(100);

	//��ʹ�ö�ʱ���ж� ����������ɨ��
	usmart_dev.init(SystemCoreClock/1000000); 
	my_mem_init(SRAMIN); //��ʼ���ڴ��
	delay_ms(100);
	//��ʼ��ϵͳ���в��� 
	getDeviceSnNumber();
	//W25QXX��ʼ��,���ܸ�ADC Initճ��һ��
	W25QXX_Init();

	W25QXX_Read(&firstStartFlag, SpiFlashAddr_initFlag, 1);
	delay_ms(20);
	//�״�����
	if(1)//firstStartFlag!=0x55)
	{
		//�����豸����
		DeviceType=DeviceType_fpj;
		set_carrierVersions_Default();
		W25QXX_Write(&DeviceType,SpiFlashAddr_DeviceType,1);
		delay_ms(20);
		//��ȡĬ�ϲ���
		Set_Global_Parameter_Default();
		/*������д��FLASH��*/
		//����������
		W25QXX_Write((u8 *)Global_Parm.MOT,SpiFlashAddr_motorData,sizeof(Motor_Data));
		delay_ms(50);
		//�������̼��ֲ���
		W25QXX_Write((u8 *)Global_Parm.GCS,SpiFlashAddr_clawsupportData,sizeof(Glass_ClawSupport__data));
		delay_ms(50);
		//�����Ƭ����
		W25QXX_Write((u8 *)Global_Parm.GP,SpiFlashAddr_packageData,sizeof(Glass_Package_Data));
		delay_ms(50);
		//����洢������
		W25QXX_Write((u8 *)Global_Parm.GIO,SpiFlashAddr_inoutData,sizeof(Glass_In_Out_Date));
		delay_ms(50);
		//д���״��ϵ��ʼ����־λ
		firstStartFlag=0x55;
		W25QXX_Write(&firstStartFlag,SpiFlashAddr_initFlag,1);
		delay_ms(20);
	}else
	{
		//��ȡ�豸����
		W25QXX_Write(&DeviceType,SpiFlashAddr_DeviceType,1);
		delay_ms(20);
		//�豸���ʹ��� ����Ϊ0
		/*************/
		/*************/
		/*��FLASH�ж�ȡ����*/
		//��ȡ�������
		//����������
		W25QXX_Read((u8 *)Global_Parm.MOT,SpiFlashAddr_motorData,sizeof(Motor_Data));
		delay_ms(100);
		//�������̼��ֲ���
		W25QXX_Read((u8 *)Global_Parm.GCS,SpiFlashAddr_clawsupportData,sizeof(Glass_ClawSupport__data));
		delay_ms(100);
		//�����Ƭ����
		W25QXX_Read((u8 *)Global_Parm.GP,SpiFlashAddr_packageData,sizeof(Glass_Package_Data));
		delay_ms(100);
		//����洢������
		W25QXX_Read((u8 *)Global_Parm.GIO,SpiFlashAddr_inoutData,sizeof(Glass_In_Out_Date));
		delay_ms(100);
	}

	
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

	//�����Զ���λ���� ������Ḵλ У׼��ʼλ��
	TaskThread_BootReset();
	//�ȴ���λ�������
	while (!TaskThread_CheckIdle())
	{
		OSTimeDlyHMSM(0,0,0,200);
	}
	printf("Idle success\r\n");

////	
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
			delay_20ms=0;
			//usmart ɨ��
			usmart_dev.scan(); 
			//��Ļ����Э���������
			if(screenUart_RecvCompleteFlag)
			{
				LED2=LED_ON;
				screenUart_protocolParse();
				LED2=!LED_ON;
				screenUart_RecvCompleteFlag = 0; // Э���ѱ�����
			}

			//����������
			
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
   	  OSTimeDlyHMSM(0, 0, 0, 10);
			dmaRecv_makeProtocol_uart3();
	}
}

//���ڷ�������
void uart_sendtask(void *pdata)
{
	printf("uart send task\r\n");
	   while(1)
    {
    	OSTimeDlyHMSM(0, 0, 0, 20);
			Send_makeProtocol_uart3();

	}

}



