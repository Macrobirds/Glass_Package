#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/10 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

   	   		   
//IO��������
#define SDA_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0X00080000;}
#define SDA_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0X00030000;}

//#define SDA_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0X00080000;}
//#define SDA_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0X00030000;}

//#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00008000;}
//#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00003000;}

//#define SDA_IN()  {GPIO_InitTypeDef GPIO_InitStructure;GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;GPIO_Init(GPIOC, &GPIO_InitStructure);}
//#define SDA_OUT() {GPIO_InitTypeDef GPIO_InitStructure;GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;GPIO_Init(GPIOC, &GPIO_InitStructure);}


//IO��������
#define IIC_SCL    PCout(1) //SCL
#define IIC_SDA    PAout(4) //SDA	 
#define READ_SDA   PAin(4)  //����SDA 
//#define IIC_SCL    PBout(10) //SCL
//#define IIC_SDA    PBout(11) //SDA	 
//#define READ_SDA   PBin(11)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�
u8 IIC_ReadOneByte(unsigned char WriteAddr,unsigned char Reg);
void IIC_WriteOneByte(u8 WriteAddr,u8 Reg,u8 DataToWrite);


void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif
















