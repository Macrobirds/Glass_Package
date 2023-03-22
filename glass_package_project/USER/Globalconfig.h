#ifndef __GLOBALCONFIG_H
#define __GLOBALCONFIG_H
#include "sys.h"


typedef struct{
	u16 GE_1mm;
	u32 GE_max_speed;
	u32 GE_min_speed;
	u16 GCV_1mm;
	u32 GCV_max_speed;
	u32 GCV_min_speed;
	u16 GCR_max_speed;
	u16 GCR_min_speed;
	u16 GP_1mm;
	u32 GP_max_speed;
	u32 GP_min_speed;
	u16 GOH_1mm;
	u32 GOH_max_speed;
	u32 GOH_min_speed;
	u16 GOV_1mm;
	u32 GOV_max_speed;
	u32 GOV_min_speed;
}Motor_Data; 


typedef struct{
	u16 GCR_hor_pos;
	u16 GCR_ver_pos;
	u16 GCR_max_pos;
	u32 GCV_down_pos;
	u32 GCV_max_pos;
	u32 GOH_mid_pos;
	u32 GOH_end_pos;
	u32 GOH_max_pos;
}Glass_ClawSupport__data;

typedef struct{
	u16 delay_before;
	u16 delay_after;
	u16 sucker_delay;
	u32 max_pos;
	u32 sucker_pos;
	u32 spray_pos;
	u32 spray_len;
	u16 spray_speed;
	u8 spray_pressure;
}Glass_Package_Data;

typedef struct{
	u32 GOV_box_dis;
	u32 GOV_slot_dis;
	u32 GOV_box_len;
	u32 GOV_max_pos;
	u32 GE_box_len;
	u32 GE_box_dis;
	u32 GE_max_pos;
}Glass_In_Out_Date;


struct Global_Parameter_struct{
	Motor_Data MOT;
	Glass_ClawSupport__data GCS;
	Glass_Package_Data GP;
	Glass_In_Out_Date GIO;
};

struct Global_Parameter_struct Global_Parm; 

void Set_Global_Parameter_Default(void);





#endif