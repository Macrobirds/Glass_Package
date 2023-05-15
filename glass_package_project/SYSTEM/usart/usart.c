#include "Globalconfig.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

 
DMA_InitTypeDef DMA_InitStructure;
u16 DMA1_MEM_LEN;//����DMAÿ�����ݴ��͵ĳ���
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

u8 serial2_BufferLength = 0;
u8 serial2_Buffer[USART2_REC_LEN];

u8 screenUart_RecvCompleteBuf[MaxProtocolLength] = {0};
u8 screenUart_RecvCompleteBufLength = 0;
u8 screenUart_lastRecvIndex = 0;
volatile unsigned char screenUart_RecvCompleteFlag = 0;

RingBuffer * RingBuf_Send=0;
RingBuffer * RingBuf_Task=0;
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	 

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
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
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 



}

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}


// DMA���жϳ�ʼ��
void uart2_init(u32 bound)
{
	//GPIO�˿�����
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
	
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); //ʹ�ܴ���2��DMA����

	//��ʼ�����ͻ�������
	RingBuf_Send=RingBuffer_Malloc(512);
}

void MYDMA_Config_Usart2(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
    NVIC_InitTypeDef NVIC_InitStructure;     /* Configure one bit for preemption priority */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;	// DMA1ͨ��6���봮��2��Ӧ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    //ʹ��DMA����
    DMA_DeInit(DMA_CHx); //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
    DMA1_MEM_LEN=cndtr;
    DMA_InitStructure.DMA_PeripheralBaseAddr = cpar; //DMA�������ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = cmar; //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
    DMA_InitStructure.DMA_BufferSize = cndtr; //DMAͨ����DMA����Ĵ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ�������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ�Ĵ�������
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���ݿ��Ϊ8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //����������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ�
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA_CHx, &DMA_InitStructure); //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART2_Rx_DMA ����ʶ�ļĴ���

    DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);  //����DMA������ɺ�����ж�
    USART_Cmd(USART2, ENABLE); //ʹ�ܴ���2
    DMA_Cmd(DMA_CHx, ENABLE); //ʹ��USART2 TX DMA1 ��ָʾ��ͨ��
    DMA_ClearFlag(DMA1_FLAG_TC6);// ���������ɱ�־λ�������������Ҫ����
    MYDMA_Enable(DMA1_Channel6);//��ʼһ��DMA���䣡
}


void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{
    DMA_Cmd(DMA_CHx, DISABLE ); //�ر�USART3 TX DMA1 ��ָʾ��ͨ��
    DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//�������û����С,ָ������0
    DMA_Cmd(DMA_CHx, ENABLE); //ʹ��USART3 TX DMA1 ��ָʾ��ͨ��
}

// ����2 DMA���ջ��������ˣ���δ���ߣ������ж�
void DMA1_Channel6_IRQHandler(void)
{
    if(DMA_GetFlagStatus(DMA1_FLAG_TC6))
    {
        DMA_ClearFlag(DMA1_FLAG_TC6); 							// ������������жϱ�־
        memset(USART2_RX_BUF, 0, USART2_REC_LEN);   // ���DMA���ջ�����
        MYDMA_Enable(DMA1_Channel6);								// ��ʼһ��DMA���䣡
//			DMA_ClearITPendingBit(DMA1_IT_GL6); // ���ȫ���жϱ�־
    }
}

//DMA UART2 ���ڽ�������
void dmaRecv_makeProtocol_uart2(void)
{
	static u8 lastDmaDataLength = 0;
	u8 dmaDataLength = 0;
	u8 agreementHeadIndex = 0;
	u8 * p_chr = 0;
	/********************* ��DMA�������������ݵ����ڽ��ջ����� *************************/
	dmaDataLength = USART2_REC_LEN - DMA_GetCurrDataCounter(DMA1_Channel6);
	// DMA��������,�����Ѿ�������ʱֹͣ
	if(dmaDataLength > 0 && lastDmaDataLength == dmaDataLength)
	{
			memcpy(serial2_Buffer + serial2_BufferLength, USART2_RX_BUF, dmaDataLength);
			memset(USART2_RX_BUF, 0, USART2_REC_LEN);    	// ���DMA���ջ�����
			MYDMA_Enable(DMA1_Channel6);					// ��ʼһ��DMA���䣡
			serial2_BufferLength += dmaDataLength;
			dmaDataLength = 0;
			lastDmaDataLength = 0;
	}
	else
			lastDmaDataLength = dmaDataLength;
	/********************* ��DMA�������������ݵ����ڶ��л����� *************************/
	// ������ڽ��ջ������������ݣ��ҽ��ϴν�����ɵ�Э���Ѿ�������
	if(serial2_BufferLength >= 8 && !screenUart_RecvCompleteFlag)
	{
			agreementHeadIndex = 0;
			// ����Э��ͷ1
			while(agreementHeadIndex < serial2_BufferLength - 1)
			{
					if(serial2_Buffer[agreementHeadIndex] == 0x3A)
							break;
					agreementHeadIndex ++;
			}
			// û���ҵ�Э��ͷ������ȫ������
			if(agreementHeadIndex >= serial2_BufferLength)
			{
					memset(serial2_Buffer, 0, USART2_REC_LEN);
					serial2_BufferLength = 0;
			}
			// �ҵ�Э��ͷ
			else
			{
					// ���Э��ͷ���ڵ�һλ��������������
					if(agreementHeadIndex != 0)
					{
							serial2_BufferLength = serial2_BufferLength - agreementHeadIndex;		// ��ú���ʣ������ݳ���
							p_chr = mymalloc(SRAMIN, serial2_BufferLength);											// �����ڴ��ź��������
							memcpy(p_chr, serial2_Buffer + agreementHeadIndex, serial2_BufferLength);	// ����
							memset(serial2_Buffer, 0, USART2_REC_LEN);
							memcpy(serial2_Buffer, p_chr, serial2_BufferLength);
							myfree(SRAMIN,p_chr);
							p_chr = 0;
							agreementHeadIndex = 0;
					}

					// �����������8��֤��û������һ�����˳������½���Э��
					if(serial2_BufferLength < 8)
							return;
					// ���滹�������ݣ�Э��ͷ2�����ݳ���λ��
					else
					{
							// ������Խ���һ��������Э��
							if(serial2_Buffer[1] <= serial2_BufferLength)
							{
									// ���Э��β,У��λ
									if(serial2_Buffer[serial2_Buffer[1] - 1] == 0x23
													&& checkBCC(serial2_Buffer, serial2_Buffer[1]))
									{
											// ��������������ַ���
											memset(screenUart_RecvCompleteBuf, 0, MaxProtocolLength);
											screenUart_RecvCompleteBufLength = serial2_Buffer[1];
											memcpy(screenUart_RecvCompleteBuf, serial2_Buffer, screenUart_RecvCompleteBufLength);
											screenUart_RecvCompleteFlag = 1;
									}
									// ʣ��������������
									serial2_BufferLength = serial2_BufferLength - screenUart_RecvCompleteBufLength;		// ��ú���ʣ������ݳ���
									p_chr = mymalloc(SRAMIN, serial2_BufferLength);											// �����ڴ��ź��������
									memcpy(p_chr, serial2_Buffer + screenUart_RecvCompleteBufLength, serial2_BufferLength);	// ����
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
    while((USART2->SR&0X40)==0);//ѭ������,ֱ���������
    USART2->DR = (u8) data;
}

//���뷢�ͻ��������еȴ����ڷ���
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

