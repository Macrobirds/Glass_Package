#include "gpio.h"
#define Relay_1 	PEout(2)
#define Relay_2 	PEout(3)
#define Relay_3 	PEout(4)
#define Relay_4 	PEout(5)
#define Relay_ON 	1

//IO 输入输出初始化
void ALL_GPIO_Init(void)
{
LED_GPIO_Init();
RELAY_GPIO_Init();
Heater_Gpio_Init();
 Motor_Dir_GpioInit();
IoInput_GpioInit();
}

/************************************* Mini板IO口 **************************************/
void LED_GPIO_Init(void)
{
//    /**********************
//    1.执行端口重映射时,复用功能时钟得使能:RCC_APB2Periph_AFIO
//    2.&1.GPIO_Remap_SWJ_Disable: !< Full SWJ Disabled (JTAG-DP + SW-DP)
//    	此时PA13|PA14|PA15|PB3|PB4都可作为普通IO用了
//    	为了保存某些调试端口,GPIO_Remap_SWJ_Disable也可选择为下面两种模式：
//      &2.GPIO_Remap_SWJ_JTAGDisable: !< JTAG-DP Disabled and SW-DP Enabled
//    	此时PA15|PB3|PB4可作为普通IO用了
//      &3.GPIO_Remap_SWJ_NoJTRST: !< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
//    	此时只有PB4可作为普通IO用了
//    **********************/
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // 禁用Jtag，此时PA15|PB3|PB4可作为普通IO用了

		// PC4、5
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出 GPIO_Mode_Out_PP
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);			//输出高
}

void RELAY_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能PE端口时钟

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		Relay_1 = !Relay_ON;
		Relay_2 = !Relay_ON;
		Relay_3 = !Relay_ON;
		Relay_4 = !Relay_ON;
}

void Heater_Gpio_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
		uint16_t GPIO_Pin = 0;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
		GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin);
	
		GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, GPIO_Pin);
	
    GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_SetBits(GPIOE, GPIO_Pin);	
}

// 非PWM输出脚
void Motor_Dir_GpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t GPIO_Pin = 0;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIOA 1、7
	GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin); // 输出低
	// GPIOB 3、4、7
	GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin); // 输出低
	// GPIOC 7、8、9、13
	GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_Pin); // 输出低
	// GPIOE 1、6
	GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin); // 输出低
	
	// 共用引脚电机口
//	// GPIOB 8、9 (与CAN共用)
//	GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOB, GPIO_Pin); // 输出低
	
	// GPIOC 10-12 、D2 (与UART4、UART5共用)
	GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_Pin); // 输出低
	GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin); // 输出低
}


void IoInput_GpioInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE,ENABLE);
 // A 11、12
 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_12;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置成上拉输入 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 // B 5
 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置成上拉输入 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 // C 2
 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置成上拉输入 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
 // D 0-1、3-7
 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置成上拉输入 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 // E
 GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|
   GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置成上拉输入 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}









