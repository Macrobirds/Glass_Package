#include "sys.h" 	
#include "delay.h"	
#include "usart.h"
#include "usmart.h"
#include "Globalconfig.h"

/*首次启动标志位*/
u8 firstStartFlag=0;
/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			4 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   

//主循环任务
//设置任务优先级
#define MAIN_TASK_PRIO       			9 
//设置任务堆栈大小
#define MAIN_STK_SIZE  					256
//任务堆栈
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//任务函数
void main_task(void *pdata);

//气压控制任务
//设置任务优先级
#define GAS_TASK_PRIO       			10
//设置任务堆栈大小
#define GAS_STK_SIZE  					64
//任务堆栈
OS_STK GAS_TASK_STK[GAS_STK_SIZE];
//任务函数
void gas_task(void *pdata);

//通讯接收任务
//设置任务优先级
#define UART_RECEIVE_TASK_PRIO       			7 
//设置任务堆栈大小
#define UART_RECEIVE_STK_SIZE  					128
//任务堆栈
OS_STK UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE];
//任务函数
void uart_receivetask(void *pdata);


//通讯发送任务
#define UART_SEND_TASK_PRIO       			8
//设置任务堆栈大小
#define UART_SEND_STK_SIZE  					128
//任务堆栈
OS_STK UART_SEND_TASK_STK[UART_SEND_STK_SIZE];
//任务函数
void uart_sendtask(void *pdata);


 int main(void)
 {	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();	    //延时函数初始化	  
	ALL_GPIO_Init();
	uart_init(115200);
	delay_ms(100);
	uart3_init(115200);
	delay_ms(100);
	MYDMA_Config_Usart3(DMA1_Channel3,(u32)&USART3->DR,(u32)USART3_RX_BUF,USART3_REC_LEN);	// 初始化串口3DMA接收
	delay_ms(1000);
	postions_sensor_Init(); //位置传感器外部中断检测
	Gas_Init();  //气阀气泵气压传感器初始化
	delay_ms(100);
	Motor_PWM_Init(); //步进电机初始化
	delay_ms(100);

	//不使用定时器中断 在主函数中扫描
	usmart_dev.init(SystemCoreClock/1000000); 
	my_mem_init(SRAMIN); //初始化内存池
	delay_ms(100);
	//初始化系统运行参数 
	getDeviceSnNumber();
	//W25QXX初始化,不能跟ADC Init粘在一起
	W25QXX_Init();

	W25QXX_Read(&firstStartFlag, SpiFlashAddr_initFlag, 1);
	delay_ms(20);
	//首次启动
	if(1)//firstStartFlag!=0x55)
	{
		//设置设备类型
		DeviceType=DeviceType_fpj;
		set_carrierVersions_Default();
		W25QXX_Write(&DeviceType,SpiFlashAddr_DeviceType,1);
		delay_ms(20);
		//读取默认参数
		Set_Global_Parameter_Default();
		/*将参数写入FLASH中*/
		//保存电机参数
		W25QXX_Write((u8 *)Global_Parm.MOT,SpiFlashAddr_motorData,sizeof(Motor_Data));
		delay_ms(50);
		//保存托盘夹手参数
		W25QXX_Write((u8 *)Global_Parm.GCS,SpiFlashAddr_clawsupportData,sizeof(Glass_ClawSupport__data));
		delay_ms(50);
		//保存封片参数
		W25QXX_Write((u8 *)Global_Parm.GP,SpiFlashAddr_packageData,sizeof(Glass_Package_Data));
		delay_ms(50);
		//保存存储器参数
		W25QXX_Write((u8 *)Global_Parm.GIO,SpiFlashAddr_inoutData,sizeof(Glass_In_Out_Date));
		delay_ms(50);
		//写入首次上电初始化标志位
		firstStartFlag=0x55;
		W25QXX_Write(&firstStartFlag,SpiFlashAddr_initFlag,1);
		delay_ms(20);
	}else
	{
		//读取设备类型
		W25QXX_Write(&DeviceType,SpiFlashAddr_DeviceType,1);
		delay_ms(20);
		//设备类型错误 设置为0
		/*************/
		/*************/
		/*从FLASH中读取参数*/
		//读取电机参数
		//保存电机参数
		W25QXX_Read((u8 *)Global_Parm.MOT,SpiFlashAddr_motorData,sizeof(Motor_Data));
		delay_ms(100);
		//保存托盘夹手参数
		W25QXX_Read((u8 *)Global_Parm.GCS,SpiFlashAddr_clawsupportData,sizeof(Glass_ClawSupport__data));
		delay_ms(100);
		//保存封片参数
		W25QXX_Read((u8 *)Global_Parm.GP,SpiFlashAddr_packageData,sizeof(Glass_Package_Data));
		delay_ms(100);
		//保存存储器参数
		W25QXX_Read((u8 *)Global_Parm.GIO,SpiFlashAddr_inoutData,sizeof(Glass_In_Out_Date));
		delay_ms(100);
	}

	
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	  	 
}
	  
//开始任务
void start_task(void *pdata)
{
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);						   
 	OSTaskCreate(gas_task,(void *)0,(OS_STK*)&GAS_TASK_STK[GAS_STK_SIZE-1],GAS_TASK_PRIO);	 	
	OSTaskCreate(uart_receivetask,(void *)0,(OS_STK*)&UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE-1],UART_RECEIVE_TASK_PRIO);
 	OSTaskCreate(uart_sendtask,(void *)0,(OS_STK*)&UART_SEND_TASK_STK[UART_SEND_STK_SIZE-1],UART_SEND_TASK_PRIO);		
	TaskThread_Init(); //初始化运行任务定时器 任务参数
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}



void main_task(void *pdata)
{

	static u32 delay_20ms=0;
	//等待气压稳定
	printf("main task\r\n");
	while(Gas_State==gas_pumping)
	{
		OSTimeDlyHMSM(0,0,0,200);
	}

	//开机自动复位任务 各电机轴复位 校准初始位置
	TaskThread_BootReset();
	//等待复位任务完成
	while (!TaskThread_CheckIdle())
	{
		OSTimeDlyHMSM(0,0,0,200);
	}
	printf("Idle success\r\n");

////	
//	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)  
//	//允许设备运行
//	TaskThread_State=taskthread_ready;
//	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)

	//开始系统运行
	while(1)
	{
		delay_20ms++;
		if(delay_20ms>=30) //1s间隔
		{
			delay_20ms=0;
			//usmart 扫描
			usmart_dev.scan(); 
			//屏幕串口协议解析任务
			if(screenUart_RecvCompleteFlag)
			{
				LED2=LED_ON;
				screenUart_protocolParse();
				LED2=!LED_ON;
				screenUart_RecvCompleteFlag = 0; // 协议已被处理
			}

			//盗版检测任务
			
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

		}else //关机放气
		{
			if(TaskThread_CheckIdle()) //等待任务空闲后进行放气
			{
				Gas_State=Gas_release_Func();
			}
		}
		OSTimeDlyHMSM(0,0,0,400);
	}
}

//串口接受任务
void uart_receivetask(void *pdata)
{
	printf("uart receive task\r\n");
    while(1)
    {
   	  OSTimeDlyHMSM(0, 0, 0, 10);
			dmaRecv_makeProtocol_uart3();
	}
}

//串口发送任务
void uart_sendtask(void *pdata)
{
	printf("uart send task\r\n");
	   while(1)
    {
    	OSTimeDlyHMSM(0, 0, 0, 20);
			Send_makeProtocol_uart3();

	}

}



