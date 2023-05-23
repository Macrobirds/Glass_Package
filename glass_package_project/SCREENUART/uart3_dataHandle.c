#include "Globalconfig.h"

static u8 ringbuf_send_buf[128]={0};
static u8 ringbuf_task_buf[128]={0};

RingBuffer  RingBuf_Send={
	.size=128,
	.in=0,
	.out=0,
	.buffer=ringbuf_send_buf,
};

RingBuffer  RingBuf_Task={
	.size=128,
	.in=0,
	.out=0,
	.buffer=ringbuf_task_buf,
};


u8 screenUart_RecvCompleteBuf[MaxProtocolLength] = {0};
u8 screenUart_RecvCompleteBufLength = 0;
/* *************************************** 常量 *********************************** */

/* *************************************** 函数实现 *********************************** */

// 大端数据转换未小端数据 返回下一目标指针
static u8 *mymemcpy1(void *dst, void *src, size_t size)
{

	switch (size)
	{
	case sizeof(uint8_t):
		*(uint8_t *)dst = *(uint8_t *)src;
		break;
	case sizeof(uint16_t):
		*(uint16_t *)dst = *(uint16_t *)src;
		*(uint16_t *)dst = __REV16(*(uint16_t *)dst);
		break;
	case sizeof(uint32_t):
		*(uint32_t *)dst = *(uint32_t *)src;
		*(uint32_t *)dst = __REV(*(uint32_t *)dst);
		break;
	default:
		break;
	}
	return (uint8_t *)dst + size;
}

// 大端数据转换未小端数据 返回下一源指针
static u8 *mymemcpy2(void *src, void *dst, size_t size)
{
	switch (size)
	{
	case sizeof(uint8_t):
		*(uint8_t *)dst = *(uint8_t *)src;
		break;
	case sizeof(uint16_t):
		*(uint16_t *)dst = *(uint16_t *)src;
		*(uint16_t *)dst = __REV16(*(uint16_t *)dst);
		break;
	case sizeof(uint32_t):
		*(uint32_t *)dst = *(uint32_t *)src;
		*(uint32_t *)dst = __REV(*(uint32_t *)dst);
		break;
	default:
		break;
	}
	return (uint8_t *)src + size;
}

// 返回传感器数据
static void query_state_sensor(u8 Index)
{
	u8 *tempbuf = 0;
	tempbuf = mymalloc(SRAMIN, 4);
	memset(tempbuf, 0, 4);
	if (GE_start_Sen == Sen_Block)
	{
		tempbuf[0] |= (1 << 0);
	}
	if (GE_up_Sen == Sen_Block)
	{
		tempbuf[0] |= (1 << 1);
	}
	if (GE_down_Sen == Sen_Block)
	{
		tempbuf[0] |= (1 << 2);
	}
	if (GC_claw_Sen == Sen_Block)
	{
		tempbuf[1] |= (1 << 0);
	}
	if (GC_ver_Sen == Sen_Block)
	{
		tempbuf[1] |= (1 << 1);
	}
	if (GC_rot_Sen == Sen_Block)
	{
		tempbuf[1] |= (1 << 2);
	}
	if (GP_start_Sen == Sen_Block)
	{
		tempbuf[2] |= (1 << 0);
	}
	if (GP_small_cyl_Sen == Sen_Block)
	{
		tempbuf[2] |= (1 << 1);
	}
	if (GP_big_cyl_Sen == Sen_Block)
	{
		tempbuf[2] |= (1 << 2);
	}
	if (GP_sucker_Sen == Sen_Block)
	{
		tempbuf[2] |= (1 << 3);
	}
	if (GP_spray_Sen == Sen_Block)
	{
		tempbuf[2] |= (1 << 4);
	}
	if (GOH_start_Sen == Sen_Block)
	{
		tempbuf[3] |= (1 << 0);
	}
	//	if (GOH_mid_Sen == Sen_Block)
	//	{
	//		tempbuf[3] = (1 << 1);
	//	}
	if (GOH_end_Sen == Sen_Block)
	{
		tempbuf[3] |= (1 << 2);
	}
	if (GOV_start_Sen == Sen_Block)
	{
		tempbuf[3] |= (1 << 3);
	}
	if (GOV_glass_Sen == Sen_Block)
	{
		tempbuf[3] |= (1 << 4);
	}
	if (GOV_box_Sen == Sen_Block)
	{
		tempbuf[3] |= (1 << 5);
	}
	screenUart_ack_array(Index, Type_queryAck, Fc_state, Extra_state_sen, tempbuf, 4);

	myfree(SRAMIN, tempbuf);
	tempbuf = 0;
}

// 查询气动系统状态
static void query_state_gas(u8 Index)
{
	u8 *tmpBuf = mymalloc(SRAMIN, 6);
	u32 pressures = 0;
	memset(tmpBuf, 0, 6);
	pressures = P6847_adc_GetPressure();
	mymemcpy1(&(tmpBuf[0]), &pressures, sizeof(pressures));
	tmpBuf[4] = ITV0011_IIC_GetPressure();
	// 主气泵阀
	if (Gas_State == gas_pumping)
	{
		tmpBuf[5] |= (1 << 0);
	}
	// 吸盘气泵
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))
	{
		tmpBuf[5] |= (1 << 1);
	}
	// 夹手阀
	if (GC_claw_Sen == Sen_Block)
	{
		tmpBuf[5] |= (1 << 2);
	}
	if (GP_small_cyl_Sen == Sen_Block)
	{
		tmpBuf[5] |= (1 << 3);
	}
	if (GP_big_cyl_Sen == Sen_Block)
	{
		tmpBuf[5] |= (1 << 4);
	}
	// 吸盘1
	if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14))
	{
		tmpBuf[5] |= (1 << 5);
	}
	// 吸盘2
	if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15))
	{
		tmpBuf[5] |= (1 << 6);
	}
	// 滴胶阀
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))
	{
		tmpBuf[5] |= (1 << 7);
	}
	screenUart_ack_array(Index, Type_queryAck, Fc_state, Extra_state_gas, tmpBuf, 6);
	myfree(SRAMIN, tmpBuf);
	tmpBuf = 0;
}
// 查询参数
static void query_param(u8 Index, u8 datalen, u8 extra, void (*cpy_fun)(u8 *))
{
	u8 i=0;
	u8 *tmpBuf = 0;
	u8 buflen = datalen;
	tmpBuf = mymalloc(SRAMIN, buflen+1);
	memset(tmpBuf, 0, buflen+1);

	// 重新读取参数配置
	switch (extra)
	{
	case Extra_param_motor:
		memset(Global_Parm.MOT, 0, sizeof(Motor_Data));
		W25QXX_Read((u8 *)Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Motor_Data));
		break;
	case Extra_param_claw:
		memset(Global_Parm.GCS, 0, sizeof(Glass_ClawSupport__data));
		W25QXX_Read((u8 *)Global_Parm.GCS, SpiFlashAddr_clawsupportData, sizeof(Glass_ClawSupport__data));
		break;
	case Extra_param_package:
		memset(Global_Parm.GP, 0, sizeof(Glass_Package_Data));
		W25QXX_Read((u8 *)Global_Parm.GP, SpiFlashAddr_packageData, sizeof(Glass_Package_Data));
		break;
	case Extra_param_outin:
		memset(Global_Parm.GIO, 0, sizeof(Glass_In_Out_Date));
		W25QXX_Read((u8 *)Global_Parm.GIO, SpiFlashAddr_inoutData, sizeof(Glass_In_Out_Date));
		break;
	default:
		break;
	}
	delay_ms(50);
	tmpBuf[0] = 0x3a;
	tmpBuf[1] = buflen;
	tmpBuf[2] = Index;
	tmpBuf[3] = Type_queryAck;
	tmpBuf[4] = Fc_param;
	tmpBuf[5] = extra;
	// 拷贝参数
	cpy_fun(&tmpBuf[6]);
	setBCC(tmpBuf, buflen);
	tmpBuf[buflen - 1] = 0x23;
	printf("\r\nmotor ans");
	for(i=0;i<buflen;i++)
	{
		printf("%x ",tmpBuf[i]);
	}
	screenUart_sendStr((const char *)tmpBuf, buflen);
	myfree(SRAMIN, tmpBuf);
	tmpBuf = 0;
}

// 查询-参数-电机参数
static void query_param_motor(u8 *bufptr)
{
	u8 *tmpptr = bufptr;
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GE_max_speed, sizeof(Global_Parm.MOT->GE_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GE_min_speed, sizeof(Global_Parm.MOT->GE_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GCV_max_speed, sizeof(Global_Parm.MOT->GCV_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GCV_min_speed, sizeof(Global_Parm.MOT->GCV_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GCR_max_speed, sizeof(Global_Parm.MOT->GCR_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GCR_min_speed, sizeof(Global_Parm.MOT->GCR_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GP_max_speed, sizeof(Global_Parm.MOT->GP_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GP_min_speed, sizeof(Global_Parm.MOT->GP_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GOH_max_speed, sizeof(Global_Parm.MOT->GOH_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GOH_min_speed, sizeof(Global_Parm.MOT->GOH_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GOV_max_speed, sizeof(Global_Parm.MOT->GOV_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GOV_min_speed, sizeof(Global_Parm.MOT->GOV_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GOV_dir, sizeof(Global_Parm.MOT->GOV_dir));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GOH_dir, sizeof(Global_Parm.MOT->GOH_dir));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GP_dir, sizeof(Global_Parm.MOT->GP_dir));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GC_rot_dir, sizeof(Global_Parm.MOT->GC_rot_dir));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GC_ver_dir, sizeof(Global_Parm.MOT->GC_ver_dir));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT->GE_dir, sizeof(Global_Parm.MOT->GE_dir));
	tmpptr = 0;
}
// 查询-参数-夹手参数
static void query_param_calw(u8 *bufptr)
{

	u8 *tempptr = bufptr;
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS->GCR_hor_pos, sizeof(Global_Parm.GCS->GCR_hor_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS->GCR_ver_pos, sizeof(Global_Parm.GCS->GCR_ver_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS->GCV_down_pos, sizeof(Global_Parm.GCS->GCV_down_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS->GCV_glass_len, sizeof(Global_Parm.GCS->GCV_glass_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS->GOH_mid_pos, sizeof(Global_Parm.GCS->GOH_mid_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS->GOH_end_pos, sizeof(Global_Parm.GCS->GOH_end_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS->GCV_package_pos, sizeof(Global_Parm.GCS->GCV_package_pos));
	tempptr = 0;
}
// 查询-参数-封片参数
static void query_param_package(u8 *bufptr)
{
	u8 *tempptr = bufptr;
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP->delay_before, sizeof(Global_Parm.GP->delay_before));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP->delay_after, sizeof(Global_Parm.GP->delay_after));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP->sucker_pos, sizeof(Global_Parm.GP->sucker_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP->spray_pos, sizeof(Global_Parm.GP->spray_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP->spray_len, sizeof(Global_Parm.GP->spray_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP->spray_speed, sizeof(Global_Parm.GP->spray_speed));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP->spray_pressure, sizeof(Global_Parm.GP->spray_pressure));
	tempptr = 0;
}
// 查询-参数-存储器参数
static void query_param_outin(u8 *bufptr)
{
	u8 *tempptr = bufptr;
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO->GOV_box_dis, sizeof(Global_Parm.GIO->GOV_box_dis));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO->GOV_slot_dis, sizeof(Global_Parm.GIO->GOV_slot_dis));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO->GOV_box_len, sizeof(Global_Parm.GIO->GOV_box_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO->GOV_adjust, sizeof(Global_Parm.GIO->GOV_adjust));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO->GOV_adjust_start, sizeof(Global_Parm.GIO->GOV_adjust_start));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO->GE_box_len, sizeof(Global_Parm.GIO->GE_box_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO->GE_box_speed, sizeof(Global_Parm.GIO->GE_box_speed));
	tempptr = 0;
}

// 设置-参数-电机参数
u8 set_param_motor(u8 *data, u8 dataLength)
{
	u8 *tmpptr = data;
	if (dataLength != 15)
	{
		tmpptr = 0;
		return 0;
	}
	if (!TaskThread_CheckIdle()) // 运行状态不为空闲状态
	{
		tmpptr = 0;
		return 0;
	}
	// 写入电机参数到全局变量
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GE_max_speed, sizeof(Global_Parm.MOT->GE_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GE_min_speed, sizeof(Global_Parm.MOT->GE_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GCV_max_speed, sizeof(Global_Parm.MOT->GCV_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GCV_min_speed, sizeof(Global_Parm.MOT->GCV_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GCR_max_speed, sizeof(Global_Parm.MOT->GCR_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GCR_min_speed, sizeof(Global_Parm.MOT->GCR_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GP_max_speed, sizeof(Global_Parm.MOT->GP_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GP_min_speed, sizeof(Global_Parm.MOT->GP_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GOH_max_speed, sizeof(Global_Parm.MOT->GOH_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GOH_min_speed, sizeof(Global_Parm.MOT->GOH_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GOV_max_speed, sizeof(Global_Parm.MOT->GOV_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GOV_min_speed, sizeof(Global_Parm.MOT->GOV_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GOV_dir, sizeof(Global_Parm.MOT->GOV_dir));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GOH_dir, sizeof(Global_Parm.MOT->GOH_dir));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GP_dir, sizeof(Global_Parm.MOT->GP_dir));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GC_rot_dir, sizeof(Global_Parm.MOT->GC_rot_dir));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GC_ver_dir, sizeof(Global_Parm.MOT->GC_ver_dir));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT->GE_dir, sizeof(Global_Parm.MOT->GE_dir));
	tmpptr = 0;
	// 配置电机参数
	motor_parameter_Init();
	W25QXX_Write((u8 *)&Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Global_Parm.MOT));
	delay_ms(50);
	return 1;
}
// 设置-参数-夹手参数
static u8 set_param_claw(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 24)
	{
		tempptr = 0;
		return 0;
	}
	if (!TaskThread_CheckIdle()) // 运行状态不为空闲状态
	{
		tempptr = 0;
		return 0;
	}
	// 写入夹手参数
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS->GCR_hor_pos, sizeof(Global_Parm.GCS->GCR_hor_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS->GCR_ver_pos, sizeof(Global_Parm.GCS->GCR_ver_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS->GCV_down_pos, sizeof(Global_Parm.GCS->GCV_down_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS->GCV_glass_len, sizeof(Global_Parm.GCS->GCV_glass_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS->GOH_mid_pos, sizeof(Global_Parm.GCS->GOH_mid_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS->GOH_end_pos, sizeof(Global_Parm.GCS->GOH_end_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS->GCV_package_pos, sizeof(Global_Parm.GCS->GCV_package_pos));
	tempptr = 0;
	// 更新任务参数
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GCS, SpiFlashAddr_clawsupportData, sizeof(Global_Parm.GCS));
	delay_ms(50);
	return 1;
}
// 设置-参数-封片参数
static u8 set_param_package(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 18)
	{
		tempptr = 0;
		return 0;
	}
	if (!TaskThread_CheckIdle()) // 运行状态不为空闲状态
	{
		tempptr = 0;
		return 0;
	}
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP->delay_before, sizeof(Global_Parm.GP->delay_before));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP->delay_after, sizeof(Global_Parm.GP->delay_after));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP->sucker_pos, sizeof(Global_Parm.GP->sucker_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP->spray_pos, sizeof(Global_Parm.GP->spray_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP->spray_len, sizeof(Global_Parm.GP->spray_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP->spray_speed, sizeof(Global_Parm.GP->spray_speed));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP->spray_pressure, sizeof(Global_Parm.GP->spray_pressure));
	tempptr = 0;
	// 更新任务参数
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GP, SpiFlashAddr_packageData, sizeof(Global_Parm.GP));
	delay_ms(50);
	return 1;
}
// 设置-参数-存储器参数
static u8 set_param_outin(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 24)
	{
		tempptr = 0;
		return 0;
	}
	if (!TaskThread_CheckIdle()) // 运行状态不为空闲状态
	{
		tempptr = 0;
		return 0;
	}
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO->GOV_box_dis, sizeof(Global_Parm.GIO->GOV_box_dis));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO->GOV_slot_dis, sizeof(Global_Parm.GIO->GOV_slot_dis));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO->GOV_box_len, sizeof(Global_Parm.GIO->GOV_box_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO->GOV_adjust, sizeof(Global_Parm.GIO->GOV_adjust));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO->GOV_adjust_start, sizeof(Global_Parm.GIO->GOV_adjust_start));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO->GE_box_len, sizeof(Global_Parm.GIO->GE_box_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO->GE_box_speed, sizeof(Global_Parm.GIO->GE_box_speed));

	tempptr = 0;
	// 更新任务参数
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GIO, SpiFlashAddr_inoutData, sizeof(Global_Parm.GIO));
	delay_ms(50);
	return 1;
}
// debug 模式下配置IO口
static u8 set_state_IO(u8 *data)
{
	if (TaskThread_CheckIdle())
	{
		return 0;
	}
	if (data[0] & (1 << 0))
	{
		Main_Pump = GAS_ENABLE;
	}
	else
		Main_Pump = GAS_DISABLE;

	if (data[0] & (1 << 1))
	{
		Main_in_Cyl = GAS_ENABLE;
	}
	else
		Main_in_Cyl = GAS_DISABLE;

	if (data[0] & (1 << 2))
	{
		Main_out_Cyl = GAS_ENABLE;
	}
	else
		Main_out_Cyl = GAS_DISABLE;

	if (data[0] & (1 << 3))
	{
		GP_sucker1_Cyl = GAS_ENABLE;
	}
	else
		GP_sucker1_Cyl = GAS_DISABLE;

	if (data[0] & (1 << 4))
	{

		GP_sucker2_Cyl = GAS_ENABLE;
	}
	else
		GP_sucker2_Cyl = GAS_DISABLE;

	if (data[0] & (1 << 5))
	{
		GP_sucker_Pump = GAS_ENABLE;
	}
	else
		GP_sucker_Pump = GAS_DISABLE;

#ifdef BIG_CYLINDER

	if (data[0] & (1 << 6))
	{
		GP_big_Cyl = GAS_ENABLE;
	}
	else
		GP_big_Cyl = GAS_DISABLE;

#endif
	if (data[0] & (1 << 7))
	{
		GP_small_Cyl = GAS_ENABLE;
	}
	else
		GP_small_Cyl = GAS_DISABLE;

	if (data[1] & (1 << 0))
	{
		GC_claw_Cyl = GAS_ENABLE;
	}
	else
		GC_claw_Cyl = GAS_DISABLE;

	if (data[1] & (1 << 1))
	{
		GP_spray_Cyl = GAS_ENABLE;
	}
	else
		GP_spray_Cyl = GAS_DISABLE;

	if (data[1] & (1 << 2))
	{
		GP_ITV1100 = ITV1100_ENABLE;
	}
	else
		GP_ITV1100 = ITV1100_DISABLE;

	if (data[1] & (1 << 3))
	{
		GCV_motor_Break = GAS_ENABLE;
	}
	else
		GCV_motor_Break = GAS_DISABLE;

	TaskThread_State = taskthread_debug; // 进入debug模式
	return 1;
}

// 接收到的数据进行协议解析
void screenUart_protocolParse(void)
{
	u8 protocolLength = screenUart_RecvCompleteBuf[1];
	u8 Index = screenUart_RecvCompleteBuf[2];
	u8 TYPE = screenUart_RecvCompleteBuf[3];
	u8 FC = screenUart_RecvCompleteBuf[4];
	u8 Extra = screenUart_RecvCompleteBuf[5];
	u8 dataLength = protocolLength - 8;
	u8 *data = NULL;
	u8 *tmpBuf = NULL;
	u8 *tempptr = NULL;
	motor_struct *motor_ptr = NULL;
	screenUart_lastRecvIndex = Index;
	printf("type:%d,fc:%d,extra:%d,datalength:%d",TYPE,FC,Extra,dataLength);

	if (dataLength > 0)
	{
		data = mymalloc(SRAMIN, dataLength);
		memset(data, 0, dataLength);
		memcpy(data, screenUart_RecvCompleteBuf + 6, dataLength);
	}

	// 判断协议类型
	switch (TYPE)
	{
	// 查询协议
	case Type_query:
	{
		switch (FC)
		{
		case Fc_state:
		{
			switch (Extra)
			{
			// 在线状态
			case Extra_state_online:
			{
				ack(Index, Type_queryAck, FC, Extra, 0);
				break;
			}
			// 设备状态
			case Extra_state_deviceState:
			{
				tmpBuf = mymalloc(SRAMIN, 9);
				memset(tmpBuf, 0, 9);
				tmpBuf[0] = TaskThread_State;
				tmpBuf[1] = GE.task;
				tmpBuf[2] = GE.sta;
				tmpBuf[3] = GC.task;
				tmpBuf[4] = GC.sta;
				tmpBuf[5] = GP.task;
				tmpBuf[6] = GP.sta;
				tmpBuf[7] = GO.task;
				tmpBuf[8] = GO.sta;
				screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 9);
				myfree(SRAMIN, tmpBuf);
				tmpBuf = 0;
				break;
			}
			// 设备位置
			case Extra_state_position:
			{
				tmpBuf = mymalloc(SRAMIN, 25);
				memset(tmpBuf, 0, 25);
				tempptr = tmpBuf;
				tempptr = mymemcpy1(tempptr, &(GE.motor->postion), sizeof(GE.motor->postion));
				tempptr = mymemcpy1(tempptr, &(GC.motor_v->postion), sizeof(GC.motor_v->postion));
				tempptr = mymemcpy1(tempptr, &(GC.motor_r->postion), sizeof(GC.motor_r->postion));
				tempptr = mymemcpy1(tempptr, &(GP.motor->postion), sizeof(GP.motor->postion));
				tempptr = mymemcpy1(tempptr, &(GO.motor_h->postion), sizeof(GO.motor_h->postion));
				tempptr = mymemcpy1(tempptr, &(GO.motor_v->postion), sizeof(GO.motor_v->postion));
				tempptr = 0;
				screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 24);
				myfree(SRAMIN, tmpBuf);
				tmpBuf = 0;
				break;
			}
			// 气路状态
			case Extra_state_gas:
			{
				query_state_gas(Index);
				break;
			}
			// 传感器状态
			case Extra_state_sen:
			{
				query_state_sensor(Index);
				break;
			}
			}
			break;
		}
		case Fc_param:
		{
			switch (Extra)
			{
			// 电机参数
			case Extra_param_motor:
			{
				printf("query motor\r\n");
				query_param(Index, 28, Extra, query_param_motor);
				break;
			}
			// 设备信息
			case Extra_param_deviceInfo:
			{
				tmpBuf = mymalloc(SRAMIN, 7);
				memset(tmpBuf, 0, 7);
				tmpBuf[0] = Program_Versions;
				tmpBuf[1] = verifyCiphertextExistError();
				tmpBuf[2] = (deviceSnNumber >> 16) & 0xff;
				tmpBuf[3] = (deviceSnNumber >> 8) & 0xff;
				tmpBuf[4] = deviceSnNumber & 0xff;
				tmpBuf[5] = DeviceType;
				// tmpBuf[6] = mpd.carriersVersions;
				tmpBuf[6] = 0;
				screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 7);
				myfree(SRAMIN, tmpBuf);
				tmpBuf = 0;
				break;
			}
			// 授权码
			case Extra_param_serialNumber:
				// query_param_printf(Index);
				break;
			// 夹手托盘参数
			case Extra_param_claw:
			{
				query_param(Index, 32, Extra_param_claw, query_param_calw);
				break;
			}
			// 封片参数
			case Extra_param_package:
			{
				query_param(Index, 27, Extra_param_package, query_param_package);
				break;
			}
			// 存储器参数
			case Extra_param_outin:
			{
				query_param(Index, 32, Extra_param_outin, query_param_outin);
				break;
			}
			}
			break;
		}
		}
		break;
	}
	// 设置协议
	case Type_set:
	{
		switch (FC)
		{
		case Fc_param:
		{
			switch (Extra)
			{
			// 电机参数
			case Extra_param_motor:
			{

				if (set_param_motor(data, dataLength))
				{
					// 电机参数设置成功
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
				{
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				}
				break;
			}
			case Extra_param_serialNumber:
			{
				// 程序未授权
				if (verifyCiphertextExistError())
				{
					if (dataLength != 4)
						break;
					writeCiphertext(data);
					delay_ms(10);
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				break;
			}
			// 设置打印参数
			case Extra_param_deviceInfo:
			{
				tmpBuf = mymalloc(SRAMIN, 7);
				memset(tmpBuf, 0, 7);
				tmpBuf[0] = Program_Versions;
				tmpBuf[1] = verifyCiphertextExistError();
				tmpBuf[2] = (deviceSnNumber >> 16) & 0xff;
				tmpBuf[3] = (deviceSnNumber >> 8) & 0xff;
				tmpBuf[4] = deviceSnNumber & 0xff;
				tmpBuf[5] = DeviceType;
				// tmpBuf[6] = mpd.carriersVersions;
				screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 7);
				myfree(SRAMIN, tmpBuf);
				tmpBuf = 0;
				break;
			}
			// 设置夹手托盘参数
			case Extra_param_claw:
			{
				// 参数设置成功
				if (set_param_claw(data, dataLength))
				{
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				break;
			}
			// 设置封片参数
			case Extra_param_package:
			{
				if (set_param_package(data, dataLength))
				{
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				break;
			}

			// 设置进出槽参数
			case Extra_param_outin:
			{
				if (set_param_outin(data, dataLength))
				{
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				break;
			}
			}

			break;
		}
		}
	}
	// 控制协议
	case Type_control:
	{
		switch (FC)
		{
		case Fc_debug:
		{
			// 系统正在运行时无法进入debug模式
			if (TaskThread_State == taskthread_running)
				break;
			TaskThread_State = taskthread_debug; // 进入debug模式
			switch (Extra)
			{
			case Extra_debug_motor:
			{
				if (dataLength != 10)
				{
					break;
				}
				switch (data[0])
				{
				case GE_motor:
					motor_ptr = &GE_motor_struct;
					break;
				case GC_ver_motor:
					motor_ptr = &GC_ver_motor_struct;
					break;
				case GC_rot_motor:
					motor_ptr = &GC_rot_motor_struct;
					break;
				case GP_motor:
					motor_ptr = &GP_motor_struct;
					break;
				case GO_hor_motor:
					motor_ptr = &GO_hor_motor_struct;
					break;
				case GO_ver_motor:
					motor_ptr = &GO_ver_motor_struct;
					break;
				default:
					break;
				}
				if (data[1])
				{
					motor_ptr->dir = Front;
				}
				else
				{
					motor_ptr->dir = Back;
				}
				motor_Set_Direction(motor_ptr);
				motorGO_Debug(motor_ptr, *((uint32_t *)(&data[2])), *((uint32_t *)(&data[6])));
				motor_ptr = NULL;
				break;
			}

			// 调整到指定测试任务程序
			case Extra_debug_GE:
			{
				if (data[0] < GE_finish && data[0] >= GE_start)
				{
					GE.task = data[0];
					GE.sta = Ready;
				}
				break;
			}

			case Extra_debug_GC:
			{
				if (data[0] < GC_finish && data[0] >= GC_start)
				{
					GC.task = data[0];
					GC.sta = Ready;
				}
				break;
			}

			case Extra_debug_GP:
			{
				if (data[0] < GP_finish && data[0] >= GP_start)
				{
					GP.task = data[0];
					GP.sta = Ready;
				}
				break;
			}

			case Extra_debug_GO:
			{
				if (data[0] < GO_finish && data[0] >= GO_start)
				{
					GO.task = data[0];
					GO.sta = Ready;
				}
				break;
			}

			case Extra_debug_IO:
			{
				if (dataLength != 2)
				{
					break;
				}
				set_state_IO(data);
				break;
			}
			}
			break;
		}

		case Fc_run:
			switch (Extra)
			{
			// 复位动作
			case Extra_run_Reset:
			{
				if (dataLength != 1)
				{
					break;
				}
				// 进行开机复位
				if (TaskThread_BootReset())
				{
					if (data[0] == OnOffState_on)
					{
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
					}
					// 是否需要应答标志位
					if (data[0] == OnOffState_on)
					{
						GE.WaitAck = 1;
						GE.Index = Index;
					}
				}
				else
				{
					if (data[0] == OnOffState_on)
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
				}
				break;
			}
			// 紧急停止
			case Extra_run_Emergency:
			{
				TaskThread_Emergency();
				break;
			}

				// 进出缸动作
			case Extra_run_GEInOut:
			{
				if (dataLength != 1)
				{
					break;
				}

				if (TaskThread_CheckStop())
				{
					// 进行进缸操作
					if (GE.task == GE_BOx_Out && GE.sta == Finish)
					{
						TaskThread_GEIn();
					}
					else // 进行出缸操作
					{
						TaskThread_GEOut();
					}
					if (data[0] == OnOffState_on)
					{
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
						// 是否需要应答标志位
						GE.WaitAck = 1;
						GE.Index = Index;
					}
				}
				else
				{
					if (data[0] == OnOffState_on)
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
				}

				break;
			}
			// 进出槽动作
			case Extra_run_GOInOut:
			{
				if (dataLength != 1)
				{
					break;
				}
				if (TaskThread_CheckStop())
				{
					// 进行进缸操作
					if (GO.task == GO_Box_Out && GO.sta == Finish)
					{
						TaskThread_GOIn();
					}
					else // 进行出缸操作
					{
						TaskThread_GOOut();
					}
					if (data[0] == OnOffState_on)
					{
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
						// 是否需要应答标志位
						GO.WaitAck = 1;
						GO.Index = Index;
					}
				}
				else
				{
					if (data[0] == OnOffState_on)
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
				}
				break;
			}
			// 开始/暂停 运行
			case Extra_run_Start:
			{
				if (dataLength != 1)
					break;
				if (TaskThread_State == taskthread_boost)
				{
					if (TaskThread_Start())
					{
						if (data[0] == OnOffState_on)
						{
							ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
						}
					}
					else
					{
						if (data[0] == OnOffState_on)
							ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
					}
				}
				else if (TaskThread_State == taskthread_running)
				{
					TaskThread_Pause();
					if (data[0] == OnOffState_on)
					{
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
					}
				}
				else if (TaskThread_State == taskthread_pause)
				{
					if (TaskThread_CheckIdle())
					{
						TaskThread_Resume();
						if (data[0] == OnOffState_on)
						{
							ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
						}
					}
					else
					{
						if (data[0] == OnOffState_on)
						{
							ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
						}
					}
				}
				else if (TaskThread_State == taskthread_error)
				{
					if (TaskThread_Resume_Error())
					{
						if (data[0] == OnOffState_on)
						{
							ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
						}
					}
					else
					{
						if (data[0] == OnOffState_on)
						{
							ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
						}
					}
				}
				break;
			}

			// 关机运行
			case Extra_run_Close:
			{
				if (TaskThread_Close())
				{
					if (data[0] == OnOffState_on)
					{
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
					}
				}
				else
				{
					if (data[0] == OnOffState_on)
					{
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
					}
				}
				break;
			}
			}
			break;
		}
		break;
	}
	}
	// 清除data临时数组数据
	myfree(SRAMIN, data);
	data = 0;

}
// 通用应答-应答1位数据
void ack(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 ackByte)
{
	u8 tmpBuf[9] = {0x3A, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	tmpBuf[6] = ackByte;
	setBCC(tmpBuf, sizeof(tmpBuf));
	screenUart_sendStr((const char *)tmpBuf, sizeof(tmpBuf));
}

// 任务线程中调用应答-应答1位数据
void ack_task(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 ackByte)
{
	u8 tmpBuf[9] = {0x3A, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	tmpBuf[6] = ackByte;
	setBCC(tmpBuf, sizeof(tmpBuf));
	screenUart_sendStr_task((const char *)tmpBuf, sizeof(tmpBuf));
}

// 通用应答-应答0位数据
void ack_0data(u8 Index, u8 TYPE, u8 FC, u8 Extra)
{
	u8 tmpBuf[8] = {0x3A, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	setBCC(tmpBuf, sizeof(tmpBuf));
	screenUart_sendStr((const char *)tmpBuf, sizeof(tmpBuf));
}

void ack_0data_task(u8 Index, u8 TYPE, u8 FC, u8 Extra)
{
	u8 tmpBuf[8] = {0x3A, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	setBCC(tmpBuf, sizeof(tmpBuf));
	screenUart_sendStr_task((const char *)tmpBuf, sizeof(tmpBuf));
}

// 通用应答-应答2位数据
void ack_2data(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 ackByte, u8 ackByte_2)
{
	u8 tmpBuf[10] = {0x3A, 0x0A, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x23};
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	tmpBuf[6] = ackByte;
	tmpBuf[7] = ackByte_2;
	setBCC(tmpBuf, sizeof(tmpBuf));
	screenUart_sendStr((const char *)tmpBuf, sizeof(tmpBuf));
}
// 通用应答-应答3位数据
void ack_3data(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 ackByte, u8 ackByte_2, u8 ackByte_3)
{
	u8 tmpBuf[11] = {0x3A, 0x0B, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	tmpBuf[6] = ackByte;
	tmpBuf[7] = ackByte_2;
	tmpBuf[8] = ackByte_3;
	setBCC(tmpBuf, sizeof(tmpBuf));
	screenUart_sendStr((const char *)tmpBuf, sizeof(tmpBuf));
}

// 通用应答-应答多位数组
void screenUart_ack_array(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 *data, u8 dataLength)
{
	u8 *tmpBuf = 0;
	u8 tmpBufLength = dataLength + 8;
	tmpBuf = mymalloc(SRAMIN, tmpBufLength);
	tmpBuf[0] = 0x3A;
	tmpBuf[1] = tmpBufLength;
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	if (dataLength > 0)
		memcpy(tmpBuf + 6, data, dataLength);
	setBCC(tmpBuf, tmpBufLength);
	tmpBuf[tmpBufLength - 1] = 0x23;
	screenUart_sendStr((const char *)tmpBuf, tmpBufLength);
	myfree(SRAMIN, tmpBuf);
	tmpBuf = 0;
}

// 任务线程中调用应答-应答多位数组
void screenUart_ack_array_task(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 *data, u8 dataLength)
{
	u8 *tmpBuf = 0;
	u8 tmpBufLength = dataLength + 8;
	tmpBuf = mymalloc(SRAMIN, tmpBufLength);
	tmpBuf[0] = 0x3A;
	tmpBuf[1] = tmpBufLength;
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	if (dataLength > 0)
		memcpy(tmpBuf + 6, data, dataLength);
	setBCC(tmpBuf, tmpBufLength);
	tmpBuf[tmpBufLength - 1] = 0x23;
	screenUart_sendStr_task((const char *)tmpBuf, tmpBufLength);
	myfree(SRAMIN, tmpBuf);
	tmpBuf = 0;
}

// 通用请求 2data
void sendProtocol_2data(u8 Index, u8 TYPE, u8 FC, u8 Extra, u8 data1, u8 data2)
{
	u8 *tmpBuf = 0;
	u8 tmpBufLength = 8 + 2;
	tmpBuf = mymalloc(SRAMIN, tmpBufLength);
	tmpBuf[0] = 0x3A;
	tmpBuf[1] = tmpBufLength;
	tmpBuf[2] = Index;
	tmpBuf[3] = TYPE;
	tmpBuf[4] = FC;
	tmpBuf[5] = Extra;
	tmpBuf[6] = data1;
	tmpBuf[7] = data2;
	setBCC(tmpBuf, tmpBufLength);
	tmpBuf[tmpBufLength - 1] = 0x23;
	screenUart_sendStr((const char *)tmpBuf, tmpBufLength);
	myfree(SRAMIN, tmpBuf);
	tmpBuf = 0;
}

// // 查询-参数-电机参数
// void query_param_motor(u8 Index)
// {
// 	u8 *tempbuf = 0;
// 	u8 *tmpptr = 0;
// 	u8 buflen = 42;
// 	tempbuf = mymalloc(SRAMIN, buflen);
// 	tmpptr = tempbuf;
// 	memset(tempbuf, 0, buflen);
// 	// 清空数据 从Flash 中重新读取？
// 	memset(&Global_Parm.MOT, 0, sizeof(Global_Parm.MOT));
// 	W25QXX_Read((u8 *)&Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Global_Parm.MOT));
// 	delay_ms(50);
// 	tempbuf[0] = 0x3a;
// 	tempbuf[1] = 54;
// 	tempbuf[2] = Index;
// 	tempbuf[3] = Type_queryAck;
// 	tempbuf[4] = Fc_param;
// 	tempbuf[5] = Extra_param_motor;

// 	// 读取电机参数
// 	tmpptr = &tempbuf[6];
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GE_1mm, sizeof(Global_Parm.MOT.GE_1mm));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GE_max_speed, sizeof(Global_Parm.MOT.GE_max_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GE_min_speed, sizeof(Global_Parm.MOT.GE_min_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCV_1mm, sizeof(Global_Parm.MOT.GCV_1mm));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCV_max_speed, sizeof(Global_Parm.MOT.GCV_max_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCV_min_speed, sizeof(Global_Parm.MOT.GCV_min_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCR_max_speed, sizeof(Global_Parm.MOT.GCR_max_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCR_min_speed, sizeof(Global_Parm.MOT.GCR_min_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GP_1mm, sizeof(Global_Parm.MOT.GP_1mm));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GP_max_speed, sizeof(Global_Parm.MOT.GP_max_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GP_min_speed, sizeof(Global_Parm.MOT.GP_min_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOH_1mm, sizeof(Global_Parm.MOT.GOH_1mm));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOH_max_speed, sizeof(Global_Parm.MOT.GOH_max_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOH_min_speed, sizeof(Global_Parm.MOT.GOH_min_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOV_1mm, sizeof(Global_Parm.MOT.GOV_1mm));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOV_max_speed, sizeof(Global_Parm.MOT.GOV_max_speed));
// 	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOV_min_speed, sizeof(Global_Parm.MOT.GOV_min_speed));
// 	setBCC(tempbuf, buflen);
// 	tempbuf[buflen - 1] = 0x23;

// 	screenUart_sendStr((const char *)tempbuf, buflen);
// 	myfree(SRAMIN, tempbuf);
// 	tempbuf = 0;
// 	tmpptr = 0;
// }

// uint8_t temp_buff[4]={0};
// uint8_t *src_ptr=(uint8_t *)src;
// uint8_t temp_cnt=0;
// uint8_t size_cnt=size;
// src_ptr+=size_cnt;
// while(size_cnt)
// {
//     size_cnt--;
//     src_ptr--;
//     temp_buff[temp_cnt]=*src_ptr;
//     temp_cnt++;
// }
// memcpy(dst,temp_buff,size);

// uint8_t size_cnt = size;
// uint8_t *src_ptr = (uint8_t *)src;
// uint8_t *dst_ptr = (uint8_t *)dst;
// src_ptr += size_cnt;
// while (size_cnt)
// {
// 	size_cnt--;
// 	src_ptr--;
// 	*dst_ptr = *src_ptr;
// 	dst_ptr++;
// }
// dst_ptr++;
// return dst_ptr;

// uint8_t size_cnt = size;
// uint8_t *src_ptr = (uint8_t *)src;
// uint8_t *dst_ptr = (uint8_t *)dst;
// src_ptr += size_cnt;
// while (size_cnt)
// {
// 	size_cnt--;
// 	src_ptr--;
// 	*dst_ptr = *src_ptr;
// 	dst_ptr++;
// }
// src_ptr++;
// return src_ptr;

// 查询返回-数据-单条步骤
bool queryAck_data_1step(u8 *data, u8 dataLength)
{

	return true;
}
