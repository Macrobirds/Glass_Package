#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usmart.h"
#include "Globalconfig.h"

/*首次启动标志位*/
u8 firstStartFlag = 0;
/////////////////////////UCOSII任务设置///////////////////////////////////
// START 任务
// 设置任务优先级
#define START_TASK_PRIO 4 // 开始任务的优先级设置为最低
// 设置任务堆栈大小
#define START_STK_SIZE 64
// 任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
// 任务函数
void start_task(void *pdata);

// 主循环任务
// 设置任务优先级
#define MAIN_TASK_PRIO 8
// 设置任务堆栈大小
#define MAIN_STK_SIZE 256
// 任务堆栈
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
// 任务函数
void main_task(void *pdata);

// 气压控制任务
// 设置任务优先级
#define GAS_TASK_PRIO 10
// 设置任务堆栈大小
#define GAS_STK_SIZE 64
// 任务堆栈
OS_STK GAS_TASK_STK[GAS_STK_SIZE];
// 任务函数
void gas_task(void *pdata);

// 通讯接收任务
// 设置任务优先级
#define UART_RECEIVE_TASK_PRIO 9
// 设置任务堆栈大小
#define UART_RECEIVE_STK_SIZE 128
// 任务堆栈
OS_STK UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE];
// 任务函数
void uart_receivetask(void *pdata);

// 通讯发送任务
#define UART_SEND_TASK_PRIO 7
// 设置任务堆栈大小
#define UART_SEND_STK_SIZE 128
// 任务堆栈
OS_STK UART_SEND_TASK_STK[UART_SEND_STK_SIZE];
// 任务函数
void uart_sendtask(void *pdata);


int main(void)
{
	// 设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	// 延时函数初始化
	delay_init();		
	// GPIO 初始化							
	ALL_GPIO_Init();
	// debug 串口初始化								
	uart_init(115200);								
	delay_ms(100);
	// 屏幕串口初始化
	uart3_init(115200); 
	delay_ms(100);
	// 初始化串口3DMA接收
	MYDMA_Config_Usart3(DMA1_Channel3, (u32)&USART3->DR, (u32)USART3_RX_BUF, USART3_REC_LEN); 
	delay_ms(100);
	// 位置传感器外部中断检测
	postions_sensor_Init(); 
	// 气阀气泵气压传感器初始化
	Gas_Init();				
	delay_ms(100);
	// 步进电机初始化
	Motor_PWM_Init(); 
	delay_ms(100);
	//usmart初始化
	usmart_dev.init(SystemCoreClock / 1000000);// 不使用定时器中断 在主函数中扫描
	delay_ms(100);
	// 初始化内存池
	my_mem_init(SRAMIN); 
	// 获取设备序列号
	getDeviceSnNumber();
	// W25QXX初始化,不能跟ADC Init粘在一起
	W25QXX_Init();
	// 读取spi flash区首次启动标志位
	W25QXX_Read(&firstStartFlag, SpiFlashAddr_initFlag, 1);
	delay_ms(20);
	// 首次启动
	if (firstStartFlag != 0x55)//
	{
		// 设置设备类型
		DeviceType = DeviceType_fpj;
		set_carrierVersions_Default();
		W25QXX_Write(&DeviceType, SpiFlashAddr_DeviceType, 1);
		delay_ms(20);
		// 读取默认参数
		Set_Global_Parameter_Default();
		/*将参数写入FLASH中*/
		// 保存电机参数
		W25QXX_Write((u8 *)Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Motor_Data));
		delay_ms(50);
		// 保存托盘夹手参数
		W25QXX_Write((u8 *)Global_Parm.GCS, SpiFlashAddr_clawsupportData, sizeof(Glass_ClawSupport_data));
		delay_ms(50);
		// 保存封片参数
		W25QXX_Write((u8 *)Global_Parm.GP, SpiFlashAddr_packageData, sizeof(Glass_Package_Data));
		delay_ms(50);
		// 保存存储器参数
		W25QXX_Write((u8 *)Global_Parm.GIO, SpiFlashAddr_inoutData, sizeof(Glass_In_Out_Data));
		delay_ms(50);
		//保存警报参数
		W25QXX_Write((u8 *)Global_Parm.Warn, SpiFlashAddr_WarnData, sizeof(Warning_Data));
		delay_ms(50);
		// 写入首次上电初始化标志位
		firstStartFlag = 0x55;
		W25QXX_Write(&firstStartFlag, SpiFlashAddr_initFlag, 1);
		delay_ms(20);
	}
	else
	{
		// 读取设备类型
		W25QXX_Read(&DeviceType, SpiFlashAddr_DeviceType, 1);
		delay_ms(20);
		// 设备类型错误 设置为0
		/*************/
		if(!checkDeviceType_isExist(DeviceType))
		{
			DeviceType=DeviceType_none;
			W25QXX_Write(&DeviceType, SpiFlashAddr_DeviceType, 1);
		}
		/*************/
		/*从FLASH中读取参数*/
		// 读取电机参数
		W25QXX_Read((u8 *)Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Motor_Data));
		delay_ms(50);
		// 保存托盘夹手参数
		W25QXX_Read((u8 *)Global_Parm.GCS, SpiFlashAddr_clawsupportData, sizeof(Glass_ClawSupport_data));
		delay_ms(50);
		// 保存封片参数
		W25QXX_Read((u8 *)Global_Parm.GP, SpiFlashAddr_packageData, sizeof(Glass_Package_Data));
		delay_ms(50);
		// 保存存储器参数
		W25QXX_Read((u8 *)Global_Parm.GIO, SpiFlashAddr_inoutData, sizeof(Glass_In_Out_Data));
		delay_ms(50);
		//保存警报参数
		W25QXX_Read((u8 *)Global_Parm.Warn, SpiFlashAddr_WarnData, sizeof(Warning_Data));
		delay_ms(50);
	}

	OSInit();
	OSTaskCreate(start_task, (void *)0, (OS_STK *)&START_TASK_STK[START_STK_SIZE - 1], START_TASK_PRIO); // 创建起始任务
	OSStart();
}

// 开始任务
void start_task(void *pdata)
{
	pdata = pdata;
	OS_ENTER_CRITICAL(); // 进入临界区(无法被中断打断)
	//开启主任务
	OSTaskCreate(main_task, (void *)0, (OS_STK *)&MAIN_TASK_STK[MAIN_STK_SIZE - 1], MAIN_TASK_PRIO);
	//开启气路检测任务
	OSTaskCreate(gas_task, (void *)0, (OS_STK *)&GAS_TASK_STK[GAS_STK_SIZE - 1], GAS_TASK_PRIO);
	//开启串口协议接收任务
	OSTaskCreate(uart_receivetask, (void *)0, (OS_STK *)&UART_RECEIVE_TASK_STK[UART_RECEIVE_STK_SIZE - 1], UART_RECEIVE_TASK_PRIO);
	//开启串口协议发射任务
	OSTaskCreate(uart_sendtask, (void *)0, (OS_STK *)&UART_SEND_TASK_STK[UART_SEND_STK_SIZE - 1], UART_SEND_TASK_PRIO);
	// 初始化运行任务定时器 任务参数
	TaskThread_Init();				
	OSTaskSuspend(START_TASK_PRIO); // 挂起起始任务.
	OS_EXIT_CRITICAL();				// 退出临界区(可以被中断打断)
}

void main_task(void *pdata)
{
	static u32 delay_20ms = 0;
   static u32 checkPirate_delay_1sec = 0;
	 // 执行盗版检查时间（分钟）
    u8 checkPirate_time_minute = rand() % 100 + 30;
    // 欺骗破解者输入help自毁程序
//    printf("-- 系统初始化成功，输入《help+回车键》获得支持命令 --\r\n");
//	if(Enable_AutoOpen_FlashReadProtect
//	&&FLASH_GetReadOutProtectionStatus()==RESET)
//	{
//		open_FlashReadProtect();
//		//关闭电源 POWER = !POWER_ON;
//	}
	Buzzer=BUZZER_DISABLE;
	// 等待气压稳定
	while (Gas_State == gas_pumping)
	{
		OSTimeDlyHMSM(0, 0, 0, 200);
	}
	// 开机自动复位任务 各电机轴复位 校准初始位置
	TaskThread_BootReset();
	// 等待复位任务完成
	while (!TaskThread_CheckIdle())
	{
		OSTimeDlyHMSM(0, 0, 0, 200);
	}
	// 开始系统运行
	while (1)
	{
		delay_20ms++;
		if (delay_20ms >= 30) // 1s间隔
		{
			delay_20ms = 0;
			checkPirate_delay_1sec++;
			// usmart 扫描
			usmart_dev.scan();
			// 盗版检测任务
		}
		// 屏幕串口协议解析任务
		if (screenUart_RecvCompleteFlag)
		{
			LED2 = LED_ON;
			screenUart_protocolParse();
			LED2 = !LED_ON;
			screenUart_RecvCompleteFlag = 0; // 协议已被处理
		}
		if(Enable_Check_ProgramPirate
		&&checkPirate_delay_1sec>=60*checkPirate_time_minute)
		{
			checkPirate_delay_1sec=0;
			checkPirate_time_minute=rand()%100+30;
			//checkPirate();
		}

		OSTimeDlyHMSM(0, 0, 0, 20);
	}
}

void gas_task(void *pdata)
{
	while (1)
	{
		if (TaskThread_State < taskthread_close)
		{
			Gas_State = Gas_pump_Func();
		}
		else // 关机放气
		{
			if (TaskThread_CheckIdle()) // 等待任务空闲后进行放气
			{
				Gas_State = Gas_release_Func();
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 400);
	}
}

// 串口接受任务
void uart_receivetask(void *pdata)
{
	while (1)
	{
		OSTimeDlyHMSM(0, 0, 0, 10);
		dmaRecv_makeProtocol_uart3();
	}
}

// 串口发送任务
void uart_sendtask(void *pdata)
{
	while (1)
	{
		OSTimeDlyHMSM(0, 0, 0, 20);
		Send_makeProtocol_uart3();
	}
}
