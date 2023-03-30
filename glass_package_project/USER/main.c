#include "sys.h" 	
#include "delay.h"	
#include "usart.h"
#include "usmart.h"
#include "Globalconfig.h"
#include "includes.h"

/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
////LED0任务
////设置任务优先级
//#define LED0_TASK_PRIO       			7 
////设置任务堆栈大小
//#define LED0_STK_SIZE  		    		64
////任务堆栈	
//OS_STK LED0_TASK_STK[LED0_STK_SIZE];
////任务函数
//void led0_task(void *pdata);


//主循环任务
//设置任务优先级
#define MAIN_TASK_PRIO       			8 
//设置任务堆栈大小
#define MAIN_STK_SIZE  					128
//任务堆栈
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//任务函数
void main_task(void *pdata);

//气压控制任务
//设置任务优先级
#define GAS_TASK_PRIO       			7 
//设置任务堆栈大小
#define GAS_STK_SIZE  					128
//任务堆栈
OS_STK GAS_TASK_STK[GAS_STK_SIZE];
//任务函数
void gas_task(void *pdata);

//通讯接收任务
//设置任务优先级
#define UART_RECEIVE_TASK_PRIO       			5 
//设置任务堆栈大小
#define UART_RECEIVE_STK_SIZE  					128
//任务堆栈
OS_STK UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE];
//任务函数
void uart_receivetask(void *pdata);


//通讯发送任务
#define UART_SEND_TASK_PRIO       			6 
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
	usmart_dev.init(SystemCoreClock/1000000); //不使用定时器中断 在主函数中扫描
	postions_sensor_Init(); //位置传感器外部中断检测
	Gas_Init();  //气阀气泵气压传感器初始化
	Motor_PWM_Init(); //步进电机初始化
	my_mem_init(SRAMIN); //初始化内存池
	delay_ms(100);
	//初始化系统运行参数 
	//从内存读取

	//设置为默认参数
	motor_parameter_Init(); //初始化电机参数
	TaskThread_Init(); //初始化运行任务定时器 任务参数
	OSInit();   
 	OSTaskCreate(gas_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	  	 
}
	  
//开始任务
void start_task(void *pdata)
{
   OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);						   
 	OSTaskCreate(gas_task,(void *)0,(OS_STK*)&GAS_TASK_STK[GAS_STK_SIZE-1],GAS_TASK_PRIO);	 	
	OSTaskCreate(uart_receivetask,(void *)0,(OS_STK*)&UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE-1],UART_RECEIVE_TASK_PRIO);
 	OSTaskCreate(uart_sendtask,(void *)0,(OS_STK*)&UART_SEND_TASK_STK[UART_SEND_STK_SIZE-1],UART_SEND_TASK_PRIO);		
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}



void main_task(void *pdata)
{
	static u32 delay_20ms=0;
	//等待气压稳定
	while(Gas_pump_Func(Global_Parm.GP.spray_pressure)!=gas_pumped)
	{
		OSTimeDlyHMSM(0,0,0,200);
	}
	//允许设备运行
	TaskThread_State=TRUE;
	//各电机轴复位 校准初始位置
	Boot_ResetTaskThread();
	//等待校准任务完成
	while(GE.task==GE_none&&GC.task==GC_none
		&&GP.task==GP_none&&GO.task==GO_none)
	{
		OSTimeDlyHMSM(0,0,0,200);
	}
	//检查系统是否处于可运行状态
	TaskThread_IsReady();
	
	//开始系统运行
	while(1)
	{
		delay_20ms++;
		if(delay_20ms>=30) //1s间隔
		{
			delay_20ms=0;
			//usmart 扫描
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



