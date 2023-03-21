#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"



#define GE_DIR PBout(4)
#define GC_rot_DIR PAout(2)
#define GC_ver_DIR PAout(7)
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
	stop,
	move,
};

enum motor_direction
{
	front,
	back
};

typedef struct{
	enum motor_index name; //�������
	enum motor_state motion; //����˶�״̬
	enum motor_direction dir; //����˶�����
	u16 paluse_1mm; //�ƶ�1mm������
	u32 maxfeq; //�������Ƶ��
	u32 startfeq; //����Ƶ��
	u32 planSetpNumber; //�ƻ����в���
	u32 planpostion; //�ƻ�����λ��
	u32 accStepNumberl;
	u32 max_pos; //�����λ��
	u32 postion; //���λ��
	u32 step; //��������˶�����
	u8 * AccPeriodArray; //�Ӽ�������
	u8 FRONT; //��ǰ�˶�DR�ڵ�ƽ�ߵ� 
	float curvature; //�Ӽ����������ʣ�ֵԽС���ٶ�ԽС��
}motor_struct;
	
	
volatile motor_struct GE_motor_struct;
volatile motor_struct GC_rot_motor_struct;
volatile motor_struct GC_ver_motor_stcut;
volatile motor_struct GP_motor_struct;
volatile motor_struct GO_hor_motor_struct;
volatile motor_struct GO_ver_motor_struct;

#endif