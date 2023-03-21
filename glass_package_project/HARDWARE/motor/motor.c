#include "motor.h"
#include "math.h"

// 步数，第几步，定时器时钟频率，启动频率，目标频率，曲线系数
unsigned short GetStepPeriodOnAcc(float len, u16 i, float pwm_clock, float fre_min, float fre_max,  float flexible)
{
  float deno ;
  float melo ;
	float fre	 ;
	unsigned short period;
  float delt = fre_max-fre_min;
	melo = flexible * (i-len/2) / (len/2);
	deno = 1.0 / (1 + expf(-melo)); 
	fre = delt * deno + fre_min;
	period = (unsigned short)(pwm_clock / fre); 
  return period;
}

// 步数，第几步，定时器时钟频率，启动频率，目标频率，曲线系数
unsigned short GetStepPeriodOnDec(float len, u16 i, float pwm_clock, float fre_max, float fre_min,  float flexible)
{
  float deno ;
  float melo ;
	float fre	 ;
	unsigned short period;
  float delt = fre_max-fre_min;
	melo = flexible * (i-len/2) / (len/2);
	deno = 1.0 / (1 + expf(-melo)); 
	fre = fre_max - delt * deno;
	period = (unsigned short)(pwm_clock / fre); 
  return period;
}

/*	获取上升曲线的周期数组。
*   计算上升曲线的频率值，转换为PWM的周期值，用在定时器定时中断期间，将周期值填充到PWM周期寄存器中.
*   len: 加速过程的步数，类型为浮点数，int可能会发生传输错误。
*   i: 获取在len步中的第i步频率。
*		pwm_clock: 定时器频率
*   fre_max: 最大频率	函数最大频率72000000，驱动器最大频率100000，参考程序最大频率40000
*   fre_min: 最小频率.  mind : 1000000/65535 = 15.259021 频率必须大于16.
*   flexible: 曲率，用于调整曲线. 通常值4-6，越大代表加速度越大，坡道越陡，越小越接近匀加速
*/
// 周期数组，步数，定时器时钟频率，启动频率，目标频率，曲线系数
void GetStepPeriodArrayOnAcc(unsigned short period[], float len,  float pwm_clock, float fre_min, float fre_max,  float flexible)
{
    int i=0;
    float deno ;
    float melo ;
		float fre	 ;
    float delt = fre_max-fre_min;
    for(i=0; i<len; i++)
    {
        melo = flexible * (i-len/2) / (len/2);
        deno = 1.0 / (1 + expf(-melo)); 
        fre = delt * deno + fre_min;
        period[i] = (unsigned short)(pwm_clock / fre); 
    }
}

// 周期数组，步数，定时器时钟频率，启动频率，目标频率，曲线系数
void GetStepPeriodArrayOnDec(unsigned short period[], float len,  float pwm_clock, float fre_max, float fre_min,  float flexible)
{
    int i=0;
    float deno ;
    float melo ;
		float fre	 ;
    float delt = fre_max-fre_min;
    for(; i<len; i++)
    {
        melo = flexible * (i-len/2) / (len/2);
        deno = 1.0 / (1 + expf(-melo)); 
        fre = fre_max - delt * deno ;
        period[i] = (unsigned short)(pwm_clock / fre); 
    }
}

void motorGo(u8 stepperMotorName, long planPosition);

// 匀速控制电机运行，参数：电机名，位置，频率
void stepperMotorRun_planPosition(u8 stepperMotorName, long planPosition, u32 freq);

void stepperMotorStop(u8 stepperMotorName);