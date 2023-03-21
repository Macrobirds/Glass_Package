#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//IIC 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/10 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

   	   		   
//IO方向设置
#define SDA_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0X00080000;}
#define SDA_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0X00030000;}

//#define SDA_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0X00080000;}
//#define SDA_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0X00030000;}

//#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00008000;}
//#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00003000;}

//#define SDA_IN()  {GPIO_InitTypeDef GPIO_InitStructure;GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;GPIO_Init(GPIOC, &GPIO_InitStructure);}
//#define SDA_OUT() {GPIO_InitTypeDef GPIO_InitStructure;GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;GPIO_Init(GPIOC, &GPIO_InitStructure);}


//IO操作函数
#define IIC_SCL    PCout(1) //SCL
#define IIC_SDA    PAout(4) //SDA	 
#define READ_SDA   PAin(4)  //输入SDA 
//#define IIC_SCL    PBout(10) //SCL
//#define IIC_SDA    PBout(11) //SDA	 
//#define READ_SDA   PBin(11)  //输入SDA 

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号
u8 IIC_ReadOneByte(unsigned char WriteAddr,unsigned char Reg);
void IIC_WriteOneByte(u8 WriteAddr,u8 Reg,u8 DataToWrite);


void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif
















