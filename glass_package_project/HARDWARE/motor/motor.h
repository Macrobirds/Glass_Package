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
#define GP_CYL_DIR PCout(9)

enum motor_index{
	GE_motor,
	GC_rot_motor,
	GC_ver_motor,
	GP_motor,
	GO_hor_motor,
	GO_ver_motor,
	GP_big_cyl_motor,
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

typedef volatile struct{
	enum motor_index name; //�������
	enum motor_state motion; //����˶�״̬
	enum motor_direction dir; //����˶�����
	u16 pulse_1mm; //�ƶ�1mm������
	u16 t_m; //�����˶�ʱ��ʱ������ֵ
	u32 maxfeq; //�������Ƶ��
	u32 startfeq; //��С����Ƶ��
	u32 defaultfeq; //�����˶�Ƶ��
	u32 planSetpNumber; //�ƻ����в���
	u32 planpostion; //�ƻ�����λ��
	u32 accStepNumber;
	int32_t max_pos; //�����λ��
	int32_t postion; //���λ��
	u32 step; //��������˶�����
	u32 timerfeq; //��ʱ��Ƶ��
	u16 * AccPeriodArray; //�Ӽ�������
	u8 FRONT; //��ǰ�˶�DR�ڵ�ƽ�ߵ� 
	float curvature; //�Ӽ����������ʣ�ֵԽС���ٶ�ԽС��
	TIM_TypeDef * TIM;
}motor_struct;
	

void motor_parameter_Init(void);
u8 motorGo_acc(motor_struct * motor, long planPosition);
void motorGo(motor_struct * motor, long planPosition,u32 freq);
void stepperMotorStop(motor_struct * motor);

//�������
void motorGO_Debug(motor_struct * motor,u32 pulse,u32 freq);
void motorAccGO_Debug(motor_struct * motor,u32 pulse);
void motor_Set_Direction(motor_struct * motor);

#endif

//extern volatile int32_t GE_motor_struct_postion;
//extern volatile u32 GE_motor_struct_step;
//extern volatile int32_t GC_rot_motor_struct_postion;
//extern volatile u32 GC_rot_motor_struct_step;
//extern volatile int32_t GC_ver_motor_struct_postion;
//extern volatile u32 GC_ver_motor_struct_step;
//extern volatile int32_t GP_motor_struct_postion;
//extern volatile u32 GP_motor_struct_step;
//extern volatile int32_t GO_hor_motor_struct_postion;
//extern volatile u32 GO_hor_motor_struct_step;
//extern volatile int32_t GO_ver_motor_struct_postion;
//extern volatile u32 GO_ver_motor_struct_step;