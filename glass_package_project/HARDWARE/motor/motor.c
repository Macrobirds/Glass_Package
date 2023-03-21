#include "motor.h"
#include "math.h"

// �������ڼ�������ʱ��ʱ��Ƶ�ʣ�����Ƶ�ʣ�Ŀ��Ƶ�ʣ�����ϵ��
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

// �������ڼ�������ʱ��ʱ��Ƶ�ʣ�����Ƶ�ʣ�Ŀ��Ƶ�ʣ�����ϵ��
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

/*	��ȡ�������ߵ��������顣
*   �����������ߵ�Ƶ��ֵ��ת��ΪPWM������ֵ�����ڶ�ʱ����ʱ�ж��ڼ䣬������ֵ��䵽PWM���ڼĴ�����.
*   len: ���ٹ��̵Ĳ���������Ϊ��������int���ܻᷢ���������
*   i: ��ȡ��len���еĵ�i��Ƶ�ʡ�
*		pwm_clock: ��ʱ��Ƶ��
*   fre_max: ���Ƶ��	�������Ƶ��72000000�����������Ƶ��100000���ο��������Ƶ��40000
*   fre_min: ��СƵ��.  mind : 1000000/65535 = 15.259021 Ƶ�ʱ������16.
*   flexible: ���ʣ����ڵ�������. ͨ��ֵ4-6��Խ�������ٶ�Խ���µ�Խ����ԽСԽ�ӽ��ȼ���
*/
// �������飬��������ʱ��ʱ��Ƶ�ʣ�����Ƶ�ʣ�Ŀ��Ƶ�ʣ�����ϵ��
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

// �������飬��������ʱ��ʱ��Ƶ�ʣ�����Ƶ�ʣ�Ŀ��Ƶ�ʣ�����ϵ��
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

// ���ٿ��Ƶ�����У��������������λ�ã�Ƶ��
void stepperMotorRun_planPosition(u8 stepperMotorName, long planPosition, u32 freq);

void stepperMotorStop(u8 stepperMotorName);