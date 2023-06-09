#include "dac.h"

void DAC1_Init()
{
		GPIO_InitTypeDef GPIO_InitStructure;
	  DAC_InitTypeDef DAC_InitType;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	 //使能PA端口时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		//推挽输出 GPIO_Mode_Out_PP
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
	
		DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//不使用触发功能 TEN1=0
		DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
		DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
		DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1输出缓存关闭 BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);	 //初始化DAC通道1

		DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC1
  
    DAC_SetChannel1Data(DAC_Align_12b_R, 0); 
	
}

//设置通道1输出电压
//vol:0~3300,代表0~3.3V
void DAC_SetVolt(u16 vol)
{
	float temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12位右对齐数据格式设置DAC值
}