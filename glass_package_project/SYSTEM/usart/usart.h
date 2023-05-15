#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
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
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define USART2_REC_LEN  		250  	//�����������ֽ��� 200
#define MaxProtocolLength  		100		// ����2��3 ���ջ����С 
extern u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern u8 screenUart_RecvCompleteBuf[MaxProtocolLength];
extern u8 screenUart_RecvCompleteBufLength;
extern u8 screenUart_lastRecvIndex;
extern volatile unsigned char screenUart_RecvCompleteFlag;
extern RingBuffer * RingBuf_Send;

extern RingBuffer * RingBuf_Task;

//����봮���жϽ��գ��벻Ҫע�����º궨��
void dmaRecv_makeProtocol_uart2(void);
void uart_init(u32 bound);
void uart2_init(u32 bound);
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
void MYDMA_Config_Usart2(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void screenUart_sendByte(unsigned char data);
void screenUart_sendStr(const char Str[], u8 length);
void screenUart_sendStr_task(const char Str[],u8 length);
u8 checkBCC(u8 *data, u8 dataLength);

#endif


