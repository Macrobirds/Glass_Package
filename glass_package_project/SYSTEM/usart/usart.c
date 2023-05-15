#include "Globalconfig.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

 
DMA_InitTypeDef DMA_InitStructure;
u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

u8 serial2_BufferLength = 0;
u8 serial2_Buffer[USART2_REC_LEN];

u8 screenUart_RecvCompleteBuf[MaxProtocolLength] = {0};
u8 screenUart_RecvCompleteBufLength = 0;
u8 screenUart_lastRecvIndex = 0;
volatile unsigned char screenUart_RecvCompleteFlag = 0;

RingBuffer * RingBuf_Send=0;
RingBuffer * RingBuf_Task=0;
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	 

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 



}

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}


// DMA不中断初始化
void uart2_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 

	//USART2_TX A2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART2_RX A3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	USART_InitStructure.USART_BaudRate = bound;                
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     
	USART_InitStructure.USART_Parity = USART_Parity_No;        
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      
	
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); //使能串口2的DMA接收

	//初始化发送环形数组
	RingBuf_Send=RingBuffer_Malloc(512);
}

void MYDMA_Config_Usart2(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
    NVIC_InitTypeDef NVIC_InitStructure;     /* Configure one bit for preemption priority */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;	// DMA1通道6（与串口2对应）
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    //使能DMA传输
    DMA_DeInit(DMA_CHx); //将DMA的通道1寄存器重设为缺省值
    DMA1_MEM_LEN=cndtr;
    DMA_InitStructure.DMA_PeripheralBaseAddr = cpar; //DMA外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = cmar; //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //数据传输方向，从内存读取发送到外设
    DMA_InitStructure.DMA_BufferSize = cndtr; //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //工作在正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA_CHx, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道USART2_Rx_DMA 所标识的寄存器

    DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);  //配置DMA发送完成后产生中断
    USART_Cmd(USART2, ENABLE); //使能串口2
    DMA_Cmd(DMA_CHx, ENABLE); //使能USART2 TX DMA1 所指示的通道
    DMA_ClearFlag(DMA1_FLAG_TC6);// 清除传输完成标志位，避免产生不必要干扰
    MYDMA_Enable(DMA1_Channel6);//开始一次DMA传输！
}


void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{
    DMA_Cmd(DMA_CHx, DISABLE ); //关闭USART3 TX DMA1 所指示的通道
    DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//从新设置缓冲大小,指向数组0
    DMA_Cmd(DMA_CHx, ENABLE); //使能USART3 TX DMA1 所指示的通道
}

// 串口2 DMA接收缓冲区满了，仍未拿走，触发中断
void DMA1_Channel6_IRQHandler(void)
{
    if(DMA_GetFlagStatus(DMA1_FLAG_TC6))
    {
        DMA_ClearFlag(DMA1_FLAG_TC6); 							// 清除缓冲区满中断标志
        memset(USART2_RX_BUF, 0, USART2_REC_LEN);   // 清空DMA接收缓冲区
        MYDMA_Enable(DMA1_Channel6);								// 开始一次DMA传输！
//			DMA_ClearITPendingBit(DMA1_IT_GL6); // 清除全部中断标志
    }
}

//DMA UART2 串口接受任务
void dmaRecv_makeProtocol_uart2(void)
{
	static u8 lastDmaDataLength = 0;
	u8 dmaDataLength = 0;
	u8 agreementHeadIndex = 0;
	u8 * p_chr = 0;
	/********************* 从DMA缓冲区复制数据到串口接收缓冲区 *************************/
	dmaDataLength = USART2_REC_LEN - DMA_GetCurrDataCounter(DMA1_Channel6);
	// DMA存在数据,而且已经接收暂时停止
	if(dmaDataLength > 0 && lastDmaDataLength == dmaDataLength)
	{
			memcpy(serial2_Buffer + serial2_BufferLength, USART2_RX_BUF, dmaDataLength);
			memset(USART2_RX_BUF, 0, USART2_REC_LEN);    	// 清空DMA接收缓冲区
			MYDMA_Enable(DMA1_Channel6);					// 开始一次DMA传输！
			serial2_BufferLength += dmaDataLength;
			dmaDataLength = 0;
			lastDmaDataLength = 0;
	}
	else
			lastDmaDataLength = dmaDataLength;
	/********************* 从DMA缓冲区复制数据到串口队列缓冲区 *************************/
	// 如果串口接收缓冲区存在数据，且接上次接收完成的协议已经被处理
	if(serial2_BufferLength >= 8 && !screenUart_RecvCompleteFlag)
	{
			agreementHeadIndex = 0;
			// 查找协议头1
			while(agreementHeadIndex < serial2_BufferLength - 1)
			{
					if(serial2_Buffer[agreementHeadIndex] == 0x3A)
							break;
					agreementHeadIndex ++;
			}
			// 没有找到协议头，丢掉全部数据
			if(agreementHeadIndex >= serial2_BufferLength)
			{
					memset(serial2_Buffer, 0, USART2_REC_LEN);
					serial2_BufferLength = 0;
			}
			// 找到协议头
			else
			{
					// 如果协议头不在第一位，数据整体左移
					if(agreementHeadIndex != 0)
					{
							serial2_BufferLength = serial2_BufferLength - agreementHeadIndex;		// 获得后面剩余的数据长度
							p_chr = mymalloc(SRAMIN, serial2_BufferLength);											// 申请内存存放后面的数据
							memcpy(p_chr, serial2_Buffer + agreementHeadIndex, serial2_BufferLength);	// 复制
							memset(serial2_Buffer, 0, USART2_REC_LEN);
							memcpy(serial2_Buffer, p_chr, serial2_BufferLength);
							myfree(SRAMIN,p_chr);
							p_chr = 0;
							agreementHeadIndex = 0;
					}

					// 如果数据少于8，证明没接收完一条，退出，重新解析协议
					if(serial2_BufferLength < 8)
							return;
					// 后面还是有数据（协议头2、数据长度位）
					else
					{
							// 如果可以接收一条完整的协议
							if(serial2_Buffer[1] <= serial2_BufferLength)
							{
									// 检查协议尾,校验位
									if(serial2_Buffer[serial2_Buffer[1] - 1] == 0x23
													&& checkBCC(serial2_Buffer, serial2_Buffer[1]))
									{
											// 拷贝到接收完成字符串
											memset(screenUart_RecvCompleteBuf, 0, MaxProtocolLength);
											screenUart_RecvCompleteBufLength = serial2_Buffer[1];
											memcpy(screenUart_RecvCompleteBuf, serial2_Buffer, screenUart_RecvCompleteBufLength);
											screenUart_RecvCompleteFlag = 1;
									}
									// 剩余数据整体左移
									serial2_BufferLength = serial2_BufferLength - screenUart_RecvCompleteBufLength;		// 获得后面剩余的数据长度
									p_chr = mymalloc(SRAMIN, serial2_BufferLength);											// 申请内存存放后面的数据
									memcpy(p_chr, serial2_Buffer + screenUart_RecvCompleteBufLength, serial2_BufferLength);	// 复制
									memset(serial2_Buffer, 0, USART2_REC_LEN);
									memcpy(serial2_Buffer, p_chr, serial2_BufferLength);
									myfree(SRAMIN,p_chr);
									p_chr = 0;
							}
							else
									return;
					}
			}
	}
}

void screenUart_sendByte(unsigned char data)
{
    while((USART2->SR&0X40)==0);//循环发送,直到发送完毕
    USART2->DR = (u8) data;
}

//放入发送环形数组中等待串口发送
void screenUart_sendStr(const char Str[], u8 length)
{
	RingBuffer_In(RingBuf_Send,Str,length);
}

//    unsigned char n = 0;
//    for(; n < length; n++)
//        screenUart_sendByte(Str[n]);

void screenUart_sendStr_task(const char Str[],u8 length)
{
	RingBuffer_In(RingBuf_Task,Str,length);
}


u8 checkBCC(u8 *data, u8 dataLength)
{
    u8 i = 0, tmpU8 = 0;
    for(i = 0; i < dataLength - 2; i++)
        tmpU8 = tmpU8 ^ data[i];
    if(tmpU8 == data[dataLength - 2])
        return 1;
    else
        return 0;
}

#endif	

