#include "Globalconfig.h"
//////////////////////////////////////////////////////////////////////////////////
// ���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h" //ucos ʹ��
#endif
//////////////////////////////////////////////////////////////////////////////////
// ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// ALIENTEK STM32������
// ����1��ʼ��
// ����ԭ��@ALIENTEK
// ������̳:www.openedv.com
// �޸�����:2012/8/18
// �汾��V1.5
// ��Ȩ���У�����ؾ���
// Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
// All rights reserved
//********************************************************************************
// V1.3�޸�˵��
// ֧����Ӧ��ͬƵ���µĴ��ڲ���������.
// �����˶�printf��֧��
// �����˴��ڽ��������.
// ������printf��һ���ַ���ʧ��bug
// V1.4�޸�˵��
// 1,�޸Ĵ��ڳ�ʼ��IO��bug
// 2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
// 3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
// 4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
// V1.5�޸�˵��
// 1,�����˶�UCOSII��֧��
//////////////////////////////////////////////////////////////////////////////////

#define USART2_SEND_LEN 64
#define USART3_SEND_LEN 128
#define USART_SEND_LEN 32

DMA_InitTypeDef DMA_InitStructure;
u16 DMA1_MEM_LEN;				  // ����DMAÿ�����ݴ��͵ĳ���
u8 USART3_RX_BUF[USART3_REC_LEN]; // ���ջ���,���USART_REC_LEN���ֽ�.

u8 serial3_BufferLength = 0;
u8 serial3_Buffer[USART3_REC_LEN];

u8 screenUart_lastRecvIndex = 0;
volatile unsigned char screenUart_RecvCompleteFlag = 0;

// u8 USART2_RX_BUF[USART3_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

// u8 serial2_BufferLength = 0;
// u8 serial2_Buffer[USART2_REC_LEN];
//////////////////////////////////////////////////////////////////
// �������´���,֧��printf����,������Ҫѡ��use MicroLIB

#if 1
#pragma import(__use_no_semihosting)
// ��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
// ����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
// �ض���fputc����
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; // ѭ������,ֱ���������
	USART1->DR = (u8)ch;
	return ch;
}
#endif

/*ʹ��microLib�ķ���*/
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

#if EN_USART1_RX // ���ʹ���˽���
// ����1�жϷ������
// ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN]; // ���ջ���,���USART_REC_LEN���ֽ�.
// ����״̬
// bit15��	������ɱ�־
// bit14��	���յ�0x0d
// bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0; // ����״̬���

void uart_init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); // ʹ��USART1��GPIOAʱ��

	// USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			// ��ʼ��GPIOA.9

	// USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			  // PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  // ��ʼ��GPIOA.10

	// Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

	// USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;										// ���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ

	USART_Init(USART1, &USART_InitStructure);	   // ��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // �������ڽ����ж�
	USART_Cmd(USART1, ENABLE);					   // ʹ�ܴ���1
}

void USART1_IRQHandler(void) // ����1�жϷ������
{
	u8 Res;
#if SYSTEM_SUPPORT_OS // ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(USART1); // ��ȡ���յ�������

		if ((USART_RX_STA & 0x8000) == 0) // ����δ���
		{
			if (USART_RX_STA & 0x4000) // ���յ���0x0d
			{
				if (Res != 0x0a)
					USART_RX_STA = 0; // ���մ���,���¿�ʼ
				else
					USART_RX_STA |= 0x8000; // ���������
			}
			else // ��û�յ�0X0D
			{
				if (Res == 0x0d)
					USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0; // �������ݴ���,���¿�ʼ����
				}
			}
		}
	}
#if SYSTEM_SUPPORT_OS // ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();
#endif
}

// DMA���жϳ�ʼ��
void uart2_init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// USART2_TX A2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// USART2_RX A3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);	   // ��ʼ������2
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); // ʹ�ܴ���2��DMA����
}

void uart3_init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// ʹ��USART2��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);			// ��ʼ��GPIOB.10

	// USART3_RX	  GPIOB.11��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			  // PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);				  // ��ʼ��GPIOB.11

	// USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										// ���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ

	USART_Init(USART3, &USART_InitStructure);	   // ��ʼ������3
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); // ʹ�ܴ���3��DMA����

	//			//��ʼ�����ͻ�������
	//	RingBuf_Send=RingBuffer_Malloc(128);
	//	RingBuf_Task=RingBuffer_Malloc(128);
}

void MYDMA_Config_Usart2(DMA_Channel_TypeDef *DMA_CHx, u32 cpar, u32 cmar, u16 cndtr)
{
	NVIC_InitTypeDef NVIC_InitStructure;					 /* Configure one bit for preemption priority */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn; // DMA1ͨ��6���봮��2��Ӧ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ʹ��DMA����
	DMA_DeInit(DMA_CHx);							   // ��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA1_MEM_LEN = cndtr;
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;						// DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;							// DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						// ���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize = cndtr;								// DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// �����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// �ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			// ���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// ����������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					// DMAͨ�� xӵ�������ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							// DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);									// ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART2_Rx_DMA ����ʶ�ļĴ���

	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE); // ����DMA������ɺ�����ж�
	USART_Cmd(USART2, ENABLE);						// ʹ�ܴ���2
	DMA_Cmd(DMA_CHx, ENABLE);						// ʹ��USART2 TX DMA1 ��ָʾ��ͨ��
	DMA_ClearFlag(DMA1_FLAG_TC6);					// ���������ɱ�־λ�������������Ҫ����
	MYDMA_Enable(DMA1_Channel6);					// ��ʼһ��DMA���䣡
}

void MYDMA_Config_Usart3(DMA_Channel_TypeDef *DMA_CHx, u32 cpar, u32 cmar, u16 cndtr)
{
	NVIC_InitTypeDef NVIC_InitStructure;					 /* Configure one bit for preemption priority */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn; // DMA1ͨ��3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ʹ��DMA����
	DMA_DeInit(DMA_CHx);							   // ��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA1_MEM_LEN = cndtr;
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;						// DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;							// DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						// ���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize = cndtr;								// DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// �����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// �ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			// ���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// ����������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					// DMAͨ�� xӵ�������ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							// DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);									// ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART3_Rx_DMA ����ʶ�ļĴ���

	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE); // ����DMA������ɺ�����ж�
	USART_Cmd(USART3, ENABLE);						// ʹ�ܴ���3
	DMA_Cmd(DMA_CHx, ENABLE);						// ʹ��USART3 TX DMA1 ��ָʾ��ͨ��
	DMA_ClearFlag(DMA1_FLAG_TC3);					// ���������ɱ�־λ�������������Ҫ����
	MYDMA_Enable(DMA1_Channel3);					// ��ʼһ��DMA���䣡
}

void MYDMA_Enable(DMA_Channel_TypeDef *DMA_CHx)
{
	DMA_Cmd(DMA_CHx, DISABLE);					   // �ر�USART2 TX DMA1 ��ָʾ��ͨ��
	DMA_SetCurrDataCounter(DMA_CHx, DMA1_MEM_LEN); // �������û����С,ָ������0
	DMA_Cmd(DMA_CHx, ENABLE);					   // ʹ��USART2 TX DMA1 ��ָʾ��ͨ��
}

//// ����2 DMA���ջ��������ˣ���δ���ߣ������ж�
// void DMA1_Channel6_IRQHandler(void)
//{
//     if(DMA_GetFlagStatus(DMA1_FLAG_TC6))
//     {
//         DMA_ClearFlag(DMA1_FLAG_TC6); 							// ������������жϱ�־
//         memset(USART2_RX_BUF, 0, USART2_REC_LEN);   // ���DMA���ջ�����
//         MYDMA_Enable(DMA1_Channel6);								// ��ʼһ��DMA���䣡
////			DMA_ClearITPendingBit(DMA1_IT_GL6); // ���ȫ���жϱ�־
//    }
//}

// ����3 DMA���ջ��������ˣ���δ���ߣ������ж�
void DMA1_Channel3_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_TC3))
	{
		DMA_ClearFlag(DMA1_FLAG_TC3);			  // ������������жϱ�־
		memset(USART3_RX_BUF, 0, USART3_REC_LEN); // ���DMA���ջ�����
		MYDMA_Enable(DMA1_Channel3);			  // ��ʼһ��DMA���䣡
												  //			DMA_ClearITPendingBit(DMA1_IT_GL3); // ���ȫ���жϱ�־
	}
}

// DMA UART3 ���ڽ�������
void dmaRecv_makeProtocol_uart3(void)
{
	static u8 lastDmaDataLength = 0;
	u8 dmaDataLength = 0;
	u8 agreementHeadIndex = 0;
	u8 *p_chr = 0;
	/********************* ��DMA�������������ݵ����ڽ��ջ����� *************************/
	dmaDataLength = USART3_REC_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);
	// DMA��������,�����Ѿ�������ʱֹͣ
	if (dmaDataLength > 0 && lastDmaDataLength == dmaDataLength)
	{
		memcpy(serial3_Buffer + serial3_BufferLength, USART3_RX_BUF, dmaDataLength);
		memset(USART3_RX_BUF, 0, USART3_REC_LEN); // ���DMA���ջ�����
		MYDMA_Enable(DMA1_Channel6);			  // ��ʼһ��DMA���䣡
		serial3_BufferLength += dmaDataLength;
		dmaDataLength = 0;
		lastDmaDataLength = 0;
	}
	else
		lastDmaDataLength = dmaDataLength;
	/********************* ��DMA�������������ݵ����ڶ��л����� *************************/
	// ������ڽ��ջ������������ݣ��ҽ��ϴν�����ɵ�Э���Ѿ�������
	if (serial3_BufferLength >= 8 && !screenUart_RecvCompleteFlag)
	{
		agreementHeadIndex = 0;
		// ����Э��ͷ1
		while (agreementHeadIndex < serial3_BufferLength - 1)
		{
			if (serial3_Buffer[agreementHeadIndex] == 0x3A)
				break;
			agreementHeadIndex++;
		}
		// û���ҵ�Э��ͷ������ȫ������
		if (agreementHeadIndex >= serial3_BufferLength)
		{
			memset(serial3_Buffer, 0, USART3_REC_LEN);
			serial3_BufferLength = 0;
		}
		// �ҵ�Э��ͷ
		else
		{
			// ���Э��ͷ���ڵ�һλ��������������
			if (agreementHeadIndex != 0)
			{
				serial3_BufferLength = serial3_BufferLength - agreementHeadIndex;		  // ��ú���ʣ������ݳ���
				p_chr = mymalloc(SRAMIN, serial3_BufferLength);							  // �����ڴ��ź��������
				memcpy(p_chr, serial3_Buffer + agreementHeadIndex, serial3_BufferLength); // ����
				memset(serial3_Buffer, 0, USART3_REC_LEN);
				memcpy(serial3_Buffer, p_chr, serial3_BufferLength);
				myfree(SRAMIN, p_chr);
				p_chr = 0;
				agreementHeadIndex = 0;
			}

			// �����������8��֤��û������һ�����˳������½���Э��
			if (serial3_BufferLength < 8)
				return;
			// ���滹�������ݣ�Э��ͷ2�����ݳ���λ��
			else
			{
				// ������Խ���һ��������Э��
				if (serial3_Buffer[1] <= serial3_BufferLength)
				{
					// ���Э��β,У��λ
					if (serial3_Buffer[serial3_Buffer[1] - 1] == 0x23 && checkBCC(serial3_Buffer, serial3_Buffer[1]))
					{
						// ��������������ַ���
						memset(screenUart_RecvCompleteBuf, 0, MaxProtocolLength);
						screenUart_RecvCompleteBufLength = serial3_Buffer[1];
						memcpy(screenUart_RecvCompleteBuf, serial3_Buffer, screenUart_RecvCompleteBufLength);
						screenUart_RecvCompleteFlag = 1;
					}
					// ʣ��������������
					serial3_BufferLength = serial3_BufferLength - screenUart_RecvCompleteBufLength;			// ��ú���ʣ������ݳ���
					p_chr = mymalloc(SRAMIN, serial3_BufferLength);											// �����ڴ��ź��������
					memcpy(p_chr, serial3_Buffer + screenUart_RecvCompleteBufLength, serial3_BufferLength); // ����
					memset(serial3_Buffer, 0, USART3_REC_LEN);
					memcpy(serial3_Buffer, p_chr, serial3_BufferLength);
					myfree(SRAMIN, p_chr);
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
	while ((USART3->SR & 0X40) == 0)
		; // ѭ������,ֱ���������
	USART3->DR = (u8)data;
}

// ���뷢�ͻ��������еȴ����ڷ���
void screenUart_sendStr(const char Str[], u8 length)
{
	RingBuffer_In(&RingBuf_Send, Str, length);
}


void screenUart_sendStr_task(const char Str[], u8 length)
{
	RingBuffer_In(&RingBuf_Task, Str, length);
}

u8 checkBCC(u8 *data, u8 dataLength)
{
	u8 i = 0, tmpU8 = 0;
	for (i = 0; i < dataLength - 2; i++)
		tmpU8 = tmpU8 ^ data[i];
	if (tmpU8 == data[dataLength - 2])
		return 1;
	else
		return 0;
}

void Send_makeProtocol_uart3(void)
{
	u8 send_len = 0;
	u8 n = 0;
	u8 *tmpbuf = NULL;
	// ����Э��ջ�ظ�����
	if (!RingBuffer_IsEmpty(&RingBuf_Send)) // ���ͻ������鲻Ϊ��
	{
		tmpbuf = mymalloc(SRAMIN, USART_SEND_LEN);
		memset(tmpbuf, 0, USART_SEND_LEN);
		// �ӻ���������ȡ������
		send_len = RingBuffer_Out(&RingBuf_Send, tmpbuf, USART_SEND_LEN);
		// �Ӵ����з�������
		for (n = 0; n < send_len; n++)
		{
			screenUart_sendByte(tmpbuf[n]);
		}
		myfree(SRAMIN, tmpbuf);
		tmpbuf = NULL;
	}
	// �������������еĻظ�����
	if (!RingBuffer_IsEmpty(&RingBuf_Task)) // ���ͻ������鲻Ϊ��
	{
		tmpbuf = mymalloc(SRAMIN, USART_SEND_LEN);
		memset(tmpbuf, 0, USART_SEND_LEN);

		// �ӻ���������ȡ������
		send_len = RingBuffer_Out(&RingBuf_Task, tmpbuf, USART_SEND_LEN);
		// �Ӵ����з�������
		for (n = 0; n < send_len; n++)
		{
			screenUart_sendByte(tmpbuf[n]);
		}
		myfree(SRAMIN, tmpbuf);
		tmpbuf = NULL;
	}
}

#endif
