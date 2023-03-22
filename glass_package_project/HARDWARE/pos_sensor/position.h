#ifndef __POSITION_H
#define __POSITION_H
#include "stm32f10x_conf.h"
#include "sys.h"

#define IoInput_0_Enable					0
#define IoInput_1_Enable					0
#define IoInput_2_Enable					0
#define IoInput_3_Enable					0
#define IoInput_4_Enable					0
#define IoInput_5_Enable					1
#define IoInput_6_Enable					1
#define IoInput_7_Enable					1
#define IoInput_8_Enable					1
#define IoInput_9_Enable					1
#define IoInput_10_Enable					1
#define IoInput_11_Enable					1
#define IoInput_12_Enable					1
#define IoInput_13_Enable					1
#define IoInput_14_Enable					1
#define IoInput_15_Enable					1

#define GE_start_Sen PEin(8) // read the ·âÆ¬×ÊÔ´.md 
#define GE_up_Sen PEin(9)
#define GE_down_Sen PEin(10)
#define GC_rot_Sen PEin(11)
#define GC_ver_Sen PEin(12)
#define GP_start_Sen PEin(13)
#define GOH_start_Sen PEin(14)
#define GOH_mid_Sen PEin(15)
#define GOH_end_Sen PDin(5)
#define GOV_start_Sen PDin(6)
#define GOV_glass_Sen PDin(7)

#define Sen_Block 1



void postions_sensor_Init(void);

#endif