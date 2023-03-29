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
	enum motor_index name; //电机名称
	volatile enum motor_state motion; //电机运动状态
	volatile enum motor_direction dir; //电机运动方向
	u16 pulse_1mm; //移动1mm脉冲数
	u16 t_m; //匀速运动时定时器计数值
	u32 maxfeq; //最大运行频率
	u32 startfeq; //最小启动频率
	u32 defaultfeq; //匀速运动频率
	u32 planSetpNumber; //计划运行步数
	u32 planpostion; //计划运行位置
	u32 accStepNumber;
	u32 max_pos; //最大轨道位置
	volatile int32_t postion; //电机位置
	volatile u32 step; //电机单次运动步数
	u16 timerfeq; //定时器频率
	u16 * AccPeriodArray; //加减速数组
	u8 FRONT; //向前运动DR口电平高低 
	float curvature; //加减速曲线曲率（值越小加速度越小）
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