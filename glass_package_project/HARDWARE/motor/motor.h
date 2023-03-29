#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"
#include "stm32f10x_conf.h"



#define GE_DIR PBout(4)
#define GCR_DIR PAout(1)
#define GCV_DIR PAout(7)
#define GP_DIR PCout(7)
#define GOH_DIR PBout(3)
#define GOV_DIR PBout(7)

enum motor_index{
	GE_motor,
	GC_rot_motor,
	GC_ver_motor,
	GP_motor,
	GO_hor_motor,
	GO_ver_motor,
};

enum motor_state{
	Stop,
	AccMove,
	ConstMove,
};

enum motor_direction
{
	Front,
	Back,
};

typedef struct{
	enum motor_index name; //�������
	volatile enum motor_state motion; //����˶�״̬
	volatile enum motor_direction dir; //����˶�����
	u16 pulse_1mm; //�ƶ�1mm������
	u16 t_m; //�����˶�ʱ��ʱ������ֵ
	u32 maxfeq; //�������Ƶ��
	u32 startfeq; //��С����Ƶ��
	u32 defaultfeq; //�����˶�Ƶ��
	u32 planSetpNumber; //�ƻ����в���
	u32 planpostion; //�ƻ�����λ��
	u32 accStepNumber;
	u32 max_pos; //�����λ��
	volatile int32_t postion; //���λ��
	volatile u32 step; //��������˶�����
	u16 timerfeq; //��ʱ��Ƶ��
	u16 * AccPeriodArray; //�Ӽ�������
	u8 FRONT; //��ǰ�˶�DR�ڵ�ƽ�ߵ� 
	float curvature; //�Ӽ����������ʣ�ֵԽС���ٶ�ԽС��
	TIM_TypeDef * TIM;
}motor_struct;
	
	
extern motor_struct GE_motor_struct;
extern motor_struct GC_rot_motor_struct;
extern motor_struct GC_ver_motor_struct;
extern motor_struct GP_motor_struct;
extern motor_struct GO_hor_motor_struct;
extern motor_struct GO_ver_motor_struct;

void motor_parameter_Init(void);
u8 motorGo_acc(motor_struct * motor, long planPosition);
void motorGo(motor_struct * motor, long planPosition,u32 freq);
void stepperMotorStop(motor_struct * motor);

#endif