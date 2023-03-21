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
	enum motor_index name; //电机名称
	enum motor_state motion; //电机运动状态
	enum motor_direction direction; //电机运动方向
	u16 paluse_1mm; //移动1mm脉冲数
	u32 maxfeq; //最大运行频率
	u32 startfeq; //启动频率
	u32 max_pos; //最大轨道位置
	u32 postion; //电机位置
	u32 step; //电机单次运动步数
	u8 * AccPeriodArray; //加减速数组
}motor;
	
	


#endif