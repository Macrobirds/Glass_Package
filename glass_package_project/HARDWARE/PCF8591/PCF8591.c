#include "PCF8591.h" 
#include "delay.h"


void PCF8591_Init(void)
{
	IIC_Init();
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 PCF8591_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	
    IIC_Send_Byte(ReadAddr|0x01);   //PCF8591�ĵ�ַ��������

	IIC_Wait_Ack(); 			   
	 
    temp=IIC_Read_Byte(0);	        //����Ϊ1ʱ����Ack	��
    
    IIC_Stop();						//����һ��ֹͣ����
	
	return temp;
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void PCF8591_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{	
    IIC_Start();  
	
    IIC_Send_Byte(WriteAddr);	    //����д��ַ
	
	IIC_Wait_Ack();	   
    	  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	
    IIC_Wait_Ack();  		    	   
    
    IIC_Stop();						//����һ��ֹͣ���� 
	
	delay_ms(10);	 
}



u8 PCF8591_ADC_Input(unsigned char WriteAddr,unsigned char Channel)
{
    u8 temp=0;
    
    IIC_Start();              					 //��������
    
    IIC_Send_Byte(WriteAddr);            //����������ַ
   
    IIC_Wait_Ack();	 
    
    IIC_Send_Byte(Channel);              //���Ϳ����ֽ�
   
    IIC_Wait_Ack();
    
    IIC_Stop();                         //�������� 
    
    IIC_Start(); 
    
    IIC_Send_Byte(WriteAddr|0x01);      //���Ͷ������ĵ�ַ
   
    IIC_Wait_Ack();	 
    
    temp=IIC_Read_Byte(0);	            //����Ϊ1ʱ����Ack	��
    
    IIC_Stop();                         //�������� 
    
    return temp;
}


u8 PCF8591_DAC_Output(unsigned char WriteAddr,unsigned char c,  unsigned char Val)
{
    IIC_Start();                        //��������
    
    IIC_Send_Byte(WriteAddr);            //����������ַ
   
    IIC_Wait_Ack();	 
    
    IIC_Send_Byte(c);                   //���Ϳ����ֽ�
   
    IIC_Wait_Ack();	 
    
    IIC_Send_Byte(Val);                 //����DAC����ֵ  
   
    IIC_Wait_Ack();	 
    
    IIC_Stop();                         //��������
    
    return(1);
}

 
