#include "PCF8591.h" 
#include "delay.h"


void PCF8591_Init(void)
{
	IIC_Init();
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 PCF8591_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	
    IIC_Send_Byte(ReadAddr|0x01);   //PCF8591的地址，读控制

	IIC_Wait_Ack(); 			   
	 
    temp=IIC_Read_Byte(0);	        //参数为1时发送Ack	？
    
    IIC_Stop();						//产生一个停止条件
	
	return temp;
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void PCF8591_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{	
    IIC_Start();  
	
    IIC_Send_Byte(WriteAddr);	    //发送写地址
	
	IIC_Wait_Ack();	   
    	  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	
    IIC_Wait_Ack();  		    	   
    
    IIC_Stop();						//产生一个停止条件 
	
	delay_ms(10);	 
}



u8 PCF8591_ADC_Input(unsigned char WriteAddr,unsigned char Channel)
{
    u8 temp=0;
    
    IIC_Start();              					 //启动总线
    
    IIC_Send_Byte(WriteAddr);            //发送器件地址
   
    IIC_Wait_Ack();	 
    
    IIC_Send_Byte(Channel);              //发送控制字节
   
    IIC_Wait_Ack();
    
    IIC_Stop();                         //结束总线 
    
    IIC_Start(); 
    
    IIC_Send_Byte(WriteAddr|0x01);      //发送读操作的地址
   
    IIC_Wait_Ack();	 
    
    temp=IIC_Read_Byte(0);	            //参数为1时发送Ack	？
    
    IIC_Stop();                         //结束总线 
    
    return temp;
}


u8 PCF8591_DAC_Output(unsigned char WriteAddr,unsigned char c,  unsigned char Val)
{
    IIC_Start();                        //启动总线
    
    IIC_Send_Byte(WriteAddr);            //发送器件地址
   
    IIC_Wait_Ack();	 
    
    IIC_Send_Byte(c);                   //发送控制字节
   
    IIC_Wait_Ack();	 
    
    IIC_Send_Byte(Val);                 //发送DAC的数值  
   
    IIC_Wait_Ack();	 
    
    IIC_Stop();                         //结束总线
    
    return(1);
}

 
