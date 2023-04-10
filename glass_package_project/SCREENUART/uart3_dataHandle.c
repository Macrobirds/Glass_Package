#include "uart3_dataHandle.h"
#include "Globalconfig.h"
/* *************************************** 常量 *********************************** */

/* *************************************** 函数实现 *********************************** */

// 大端数据转换未小端数据 返回下一目标指针
static u8 *mymemcpy1(void *dst, void *src, size_t size)
{
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
	uint8_t size_cnt = size;
	uint8_t *src_ptr = (uint8_t *)src;
	uint8_t *dst_ptr = (uint8_t *)dst;
	src_ptr += size_cnt;
	while (size_cnt)
	{
		size_cnt--;
		src_ptr--;
		*dst_ptr = *src_ptr;
		dst_ptr++;
	}
	dst_ptr++;
	return dst_ptr;
}

// 大端数据转换未小端数据 返回下一源指针
static u8 *mymemcpy2(void *src, void *dst, size_t size)
{

	uint8_t size_cnt = size;
	uint8_t *src_ptr = (uint8_t *)src;
	uint8_t *dst_ptr = (uint8_t *)dst;
	src_ptr += size_cnt;
	while (size_cnt)
	{
		size_cnt--;
		src_ptr--;
		*dst_ptr = *src_ptr;
		dst_ptr++;
	}
	src_ptr++;
	return src_ptr;
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
		tempbuf[2] = (1 << 0);
	}
	if (GP_small_cyl_Sen == Sen_Block)
	{
		tempbuf[2] = (1 << 1);
	}
	if (GP_big_cyl_Sen == Sen_Block)
	{
		tempbuf[2] = (1 << 2);
	}
	if (GP_sucker_Sen == Sen_Block)
	{
		tempbuf[2] = (1 << 3);
	}
	if (GP_spray_Sen == Sen_Block)
	{
		tempbuf[2] = (1 << 4);
	}
	if (GOH_start_Sen == Sen_Block)
	{
		tempbuf[3] = (1 << 0);
	}
	if (GOH_mid_Sen == Sen_Block)
	{
		tempbuf[3] = (1 << 1);
	}
	if (GOH_end_Sen == Sen_Block)
	{
		tempbuf[3] = (1 << 2);
	}
	if (GOV_start_Sen == Sen_Block)
	{
		tempbuf[3] = (1 << 3);
	}
	if (GOV_glass_Sen == Sen_Block)
	{
		tempbuf[3] = (1 << 4);
	}
	if (GOV_box_Sen == Sen_Block)
	{
		tempbuf[3] = (1 << 5);
	}
	screenUart_ack_array(Index, Type_queryAck, Fc_state, Extra_state_sen, tempbuf, 4);

	myfree(SRAMIN, tempbuf);
	tempbuf = 0;
}

static void query_state_gas(u8 Index)
{
	u8 tmpBuf = 0;
	// 主气泵阀
	if (Gas_State == gas_pumping)
	{
		tmpBuf |= (1 << 0);
	}
	// 吸盘气泵

	// 夹手阀
	if (GC_claw_Sen == Sen_Block)
	{
		tmpBuf |= (1 << 2);
	}
	if (GP_small_cyl_Sen == Sen_Block)
	{
		tmpBuf |= (1 << 3);
	}
	if (GP_big_cyl_Sen == Sen_Block)
	{
		tmpBuf |= (1 << 4);
	}
	// 吸盘1
	// 吸盘2
	// 滴胶阀
	ack(Index, Type_queryAck, Fc_state, Extra_state_gas, tmpBuf);
}

static void query_param(u8 Index, u8 datalen, u8 extra, void (*cpy_fun)(u8 *))
{
	u8 *tmpBuf = 0;
	u8 buflen = datalen;
	tmpBuf = mymalloc(SRAMIN, buflen);
	memset(tmpBuf, 0, buflen);
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
	screenUart_sendStr(tmpBuf, buflen);
	myfree(SRAMIN, tmpBuf);
	tmpBuf = 0;
}

static void query_param_calw(const u8 *bufptr)
{

	u8 *tempptr = bufptr;
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCR_hor_pos, sizeof(Global_Parm.GCS.GCR_hor_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCR_ver_pos, sizeof(Global_Parm.GCS.GCR_ver_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCR_max_pos, sizeof(Global_Parm.GCS.GCR_max_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCV_down_pos, sizeof(Global_Parm.GCS.GCV_down_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCV_max_pos, sizeof(Global_Parm.GCS.GCV_max_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GOH_mid_pos, sizeof(Global_Parm.GCS.GOH_mid_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GOH_end_pos, sizeof(Global_Parm.GCS.GOH_end_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GOH_max_pos, sizeof(Global_Parm.GCS.GOH_max_pos));
	tempptr = 0;
}

static void query_param_package(const u8 *bufptr)
{
	u8 *tempptr = bufptr;

	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.delay_before, sizeof(Global_Parm.GP.delay_before));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.delay_after, sizeof(Global_Parm.GP.delay_after));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.sucker_delay, sizeof(Global_Parm.GP.sucker_delay));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.max_pos, sizeof(Global_Parm.GP.max_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.sucker_pos, sizeof(Global_Parm.GP.sucker_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_pos, sizeof(Global_Parm.GP.spray_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_len, sizeof(Global_Parm.GP.spray_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_pressure, sizeof(Global_Parm.GP.spray_pressure));

	tempptr = 0;
}

static void query_param_outin(const u8 *bufptr)
{
	u8 *tempptr = bufptr;
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.delay_before, sizeof(Global_Parm.GP.delay_before));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.delay_after, sizeof(Global_Parm.GP.delay_after));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.sucker_delay, sizeof(Global_Parm.GP.sucker_delay));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.max_pos, sizeof(Global_Parm.GP.max_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.sucker_pos, sizeof(Global_Parm.GP.sucker_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_pos, sizeof(Global_Parm.GP.spray_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_len, sizeof(Global_Parm.GP.spray_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_pressure, sizeof(Global_Parm.GP.spray_pressure));
	tempptr = 0;
}

//// 设置-参数-电机参数
u8 set_param_motor(u8 *data, u8 dataLength)
{
	u8 *tmpptr = data;
	if (dataLength != 32)
	{
		tmpptr = 0;
		return 0;
	}

	// 写入电机参数到全局变量
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GE_1mm, sizeof(Global_Parm.MOT.GE_1mm));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GE_max_speed, sizeof(Global_Parm.MOT.GE_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GE_min_speed, sizeof(Global_Parm.MOT.GE_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCV_1mm, sizeof(Global_Parm.MOT.GCV_1mm));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCV_max_speed, sizeof(Global_Parm.MOT.GCV_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCV_min_speed, sizeof(Global_Parm.MOT.GCV_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCR_max_speed, sizeof(Global_Parm.MOT.GCR_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCR_min_speed, sizeof(Global_Parm.MOT.GCR_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GP_1mm, sizeof(Global_Parm.MOT.GP_1mm));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GP_max_speed, sizeof(Global_Parm.MOT.GP_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GP_min_speed, sizeof(Global_Parm.MOT.GP_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOH_1mm, sizeof(Global_Parm.MOT.GOH_1mm));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOH_max_speed, sizeof(Global_Parm.MOT.GOH_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOH_min_speed, sizeof(Global_Parm.MOT.GOH_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOV_1mm, sizeof(Global_Parm.MOT.GOV_1mm));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOV_max_speed, sizeof(Global_Parm.MOT.GOV_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOV_min_speed, sizeof(Global_Parm.MOT.GOV_min_speed));
	tmpptr = 0;
	// 配置电机参数
	motor_parameter_Init();
	W25QXX_Write((u8 *)&Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Global_Parm.MOT));
	delay_ms(50);
	return 1;
}

static u8 set_param_claw(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 34)
	{
		tempptr = 0;
		return 0;
	}
	// 写入夹手参数
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCR_hor_pos, sizeof(Global_Parm.GCS.GCR_hor_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCR_ver_pos, sizeof(Global_Parm.GCS.GCR_ver_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCR_max_pos, sizeof(Global_Parm.GCS.GCR_max_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCV_down_pos, sizeof(Global_Parm.GCS.GCV_down_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCV_max_pos, sizeof(Global_Parm.GCS.GCV_max_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GOH_mid_pos, sizeof(Global_Parm.GCS.GOH_mid_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GOH_end_pos, sizeof(Global_Parm.GCS.GOH_end_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GOH_max_pos, sizeof(Global_Parm.GCS.GOH_max_pos));
	tempptr = 0;
	// 更新任务参数
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GCS, SpiFlashAddr_clawsupportData, sizeof(Global_Parm.GCS));
	delay_ms(50);
	return 1;
}

static u8 set_param_package(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 33)
	{
		tempptr = 0;
		return 0;
	}
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.delay_before, sizeof(Global_Parm.GP.delay_before));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.delay_after, sizeof(Global_Parm.GP.delay_after));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.sucker_delay, sizeof(Global_Parm.GP.sucker_delay));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.max_pos, sizeof(Global_Parm.GP.max_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.sucker_pos, sizeof(Global_Parm.GP.sucker_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_pos, sizeof(Global_Parm.GP.spray_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_len, sizeof(Global_Parm.GP.spray_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_pressure, sizeof(Global_Parm.GP.spray_pressure));
	tempptr = 0;
	// 更新任务参数
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GP, SpiFlashAddr_packageData, sizeof(Global_Parm.GP));
	delay_ms(50);
	return 1;
}

static u8 set_param_outin(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 36)
	{
		tempptr = 0;
		return 0;
	}
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.delay_before, sizeof(Global_Parm.GP.delay_before));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.delay_after, sizeof(Global_Parm.GP.delay_after));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.sucker_delay, sizeof(Global_Parm.GP.sucker_delay));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.max_pos, sizeof(Global_Parm.GP.max_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.sucker_pos, sizeof(Global_Parm.GP.sucker_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_pos, sizeof(Global_Parm.GP.spray_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_len, sizeof(Global_Parm.GP.spray_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_pressure, sizeof(Global_Parm.GP.spray_pressure));

	tempptr = 0;
	// 更新任务参数
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GIO, SpiFlashAddr_inoutData, sizeof(Global_Parm.GIO));
	delay_ms(50);
	return 1;
}

void set_state_gas(u8 data)
{
	if (data & (1 << 0))
	{
		Main_Pump = GAS_ENABLE;
	}
	if (data & (1 << 1))
	{
		GP_sucker_Pump = GAS_ENABLE;
	}
	if (data & (1 << 2))
	{
		GC_claw_Cyl = GAS_ENABLE;
	}
	if (data & (1 << 3))
	{
		GP_small_Cyl = GAS_ENABLE;
	}
	if (data & (1 << 4))
	{
		GP_big_Cyl = GAS_ENABLE;
	}
	if (data & (1 << 5))
	{
		GP_sucker1_Cyl = GAS_ENABLE;
	}
	if (data & (1 << 6))
	{
		GP_sucker2_Cyl = GAS_ENABLE;
	}
	if (data & (1 << 7))
	{
		GP_spray_Cyl = GAS_ENABLE;
	}
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
	u8 *data = 0;
	u8 *tmpBuf = 0;
	u8 *tempptr = 0;
	screenUart_lastRecvIndex = Index;

	//	uart1_SendString((char *)screenUart_RecvCompleteBuf, protocolLength);

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
			{ // 在线状态
			case Extra_state_online:
			{
				ack(Index, Type_queryAck, FC, Extra, 0);
				break;
			} // 设备状态
			case Extra_state_deviceState:
			{
				tmpBuf = mymalloc(SRAMIN, 8);
				memset(tmpBuf, 0, 8);
				tmpBuf[0] = GE.task;
				tmpBuf[1] = GE.sta;
				tmpBuf[2] = GC.task;
				tmpBuf[3] = GC.sta;
				tmpBuf[4] = GP.task;
				tmpBuf[5] = GP.sta;
				tmpBuf[6] = GO.task;
				tmpBuf[7] = GO.sta;
				screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 8);
				myfree(SRAMIN, tmpBuf);
				tmpBuf = 0;
				break;
			} // 设备位置
			case Extra_state_position:
			{
				tmpBuf = mymalloc(SRAMIN, 24);
				memset(tmpBuf, 0, 24);
				tempptr = tmpBuf;
				tempptr = mymemcpy1(tempptr, &GE.motor->postion, sizeof(GE.motor->postion));
				tempptr = mymemcpy1(tempptr, &GC.motor_v->postion, sizeof(GC.motor_v->postion));
				tempptr = mymemcpy1(tempptr, &GC.motor_r->postion, sizeof(GC.motor_r->postion));
				tempptr = mymemcpy1(tempptr, &GP.motor->postion, sizeof(GP.motor->postion));
				tempptr = mymemcpy1(tempptr, &GO.motor_h->postion, sizeof(GO.motor_h->postion));
				tempptr = mymemcpy1(tempptr, &GO.motor_v->postion, sizeof(GO.motor_v->postion));
				screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 24);
				myfree(SRAMIN, tmpBuf);
				tmpBuf = 0;
				tempptr = 0;
				break;
			}
			case Extra_state_gas:
			{
				query_state_gas(Index);
				break;
			}
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
			case Extra_param_motor:
				query_param_motor(Index);
				break;
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
			case Extra_param_serialNumber:
				// query_param_printf(Index);
				break;
			case Extra_param_claw:
				query_param(Index, 34, Extra_param_claw, (void *)query_param_calw);
				break;
			case Extra_param_package:
				query_param(Index, 33, Extra_param_package, (void *)query_param_package);
				break;
			case Extra_param_outin:
				query_param(Index, 34, Extra_param_outin, (void *)query_param_outin);
				break;
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
			case Extra_param_motor:
			{
				// 参数设置成功
				if (set_param_motor(data, dataLength))
				{
					//									// 根据深孔板型号更新全局硬件参数
					//									update_globalParam();
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
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
				if (set_param_package(data, dataLength))
				{
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				break;

			// 设置进出槽参数
			case Extra_param_outin:
				if (set_param_outin(data, dataLength))
				{
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				break;
			}
			break;
		}
		// 直接控制状态
		case Fc_state:
		// 设置气阀打开状态
		case Extra_state_gas:
			TaskThread_State = taskthread_debug; // 进入debug模式
			set_state_gas(data[0]);

			break;
		}
		break;
	}
	// 控制协议
	case Type_control:
	{
		switch (FC)
		{
		case Fc_debug:
			if(TaskThread_State==taskthread_running) return; //系统正在运行时无法进入debug模式
			TaskThread_State = taskthread_debug; // 进入debug模式
			switch (Extra)
			{
			case Extra_debug_reset:
			//
				break;
			//调整到指定测试任务程序
			case Extra_debug_GE:
				if(data[0]<=GE_finish)
				{
					GE.task=data[0];
					GE.sta=Ready;
				}
				break;
			case Extra_debug_GC:
				if(data[0]<=GC_finish)
				{
					GC.task=data[0];
					GC.sta=Ready;
				}
				break;
			case Extra_debug_GP:
				if(data[0]<=GP_finish)
				{
					GP.task=data[0];
					GP.sta=Ready;
				}
				break;
			case Extra_debug_GO:
				if(data[0]<=GO_finish)
				{
					GO.task=data[0];
					GO.sta=Ready;
				}
				break;
			}
			break;
		case Fc_run:
			switch (Extra)
			{
			// 复位动作
			case Extra_run_Reset:
			{
				if (dataLength != 1)
					break;
				// 是否需要应答标志位
				if (data[0] == OnOffState_on)
				{
				}
				else
					//									deviceState.waitAck = false;
					// 不做状态检测，强制复位
					//								if(check_deviceCanControl())
					//								{
					//									stepperMotorStop(Motor_XZ);

					//								}
					//								else
					//									ack_2data(Index, Type_controlAck, FC, Extra,
					//										AckResult_ackFailed, Reason_deviceBusy);
					break;
			}
			// 进槽动作
			case Extra_run_GEIn:
			{
				if (dataLength != 2)
					break;
				// 是否需要应答标志位
				//								if(data[0] == OnOffState_on)
				//								{
				//									deviceState.waitAck = true;
				//									deviceState.waitAckIndex = Index;
				//								}
				//								else
				//									deviceState.waitAck = false;
				////								deviceState.targetSite = data[1];
				////								if(deviceState.targetSite > BoxChannel)
				////									deviceState.targetSite = 0;
				//								if(check_deviceCanControl())
				//								{
				//									deviceState.pushinBox_runningState = RunningState_readyStart;
				//									deviceState.pushinBox_threadEnable = ON;
				//									ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
				//								}
				//								else
				//									ack_2data(Index, Type_controlAck, FC, Extra,
				//										AckResult_ackFailed, Reason_deviceBusy);
				break;
			}
			// 出槽动作
			case Extra_run_GEOut:
			{
				//								if(dataLength != 1)
				//									break;
				//								// 是否需要应答标志位
				//								if(data[0] == OnOffState_on)
				//								{
				//									deviceState.waitAck = true;
				//									deviceState.waitAckIndex = Index;
				//								}
				//								else
				//									deviceState.waitAck = false;
				//								if(check_deviceCanControl())
				//								{
				//									deviceState.pushOutBox_runningState = RunningState_readyStart;
				//									deviceState.pushOutBox_threadEnable = ON;
				//									ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
				//								}
				//								else
				//									ack_2data(Index, Type_controlAck, FC, Extra,
				//										AckResult_ackFailed, Reason_deviceBusy);
				break;
			}
			// 进缸动作
			case Extra_run_GOIn:
				break;
			// 出缸动作
			case Extra_run_GOOut:
				break;
			// 开始运行
			case Extra_run_Start:
				break;
			// 暂停运行
			case Extra_run_Pause:
				break;
			// 关机运行
			case Extra_run_Close:
				break;
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

// 查询-参数-电机参数
void query_param_motor(u8 Index)
{
	u8 *tempbuf = 0;
	u8 *tmpptr = 0;
	u8 buflen = 42;
	tempbuf = mymalloc(SRAMIN, buflen);
	tmpptr = tempbuf;
	memset(tempbuf, 0, buflen);
	// 清空数据 从Flash 中重新读取？
	memset(&Global_Parm.MOT, 0, sizeof(Global_Parm.MOT));
	W25QXX_Read((u8 *)&Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Global_Parm.MOT));
	delay_ms(50);
	tempbuf[0] = 0x3a;
	tempbuf[1] = 54;
	tempbuf[2] = Index;
	tempbuf[3] = Type_queryAck;
	tempbuf[4] = Fc_param;
	tempbuf[5] = Extra_param_motor;

	// 读取电机参数
	tmpptr = &tempbuf[6];
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GE_1mm, sizeof(Global_Parm.MOT.GE_1mm));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GE_max_speed, sizeof(Global_Parm.MOT.GE_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GE_min_speed, sizeof(Global_Parm.MOT.GE_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCV_1mm, sizeof(Global_Parm.MOT.GCV_1mm));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCV_max_speed, sizeof(Global_Parm.MOT.GCV_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCV_min_speed, sizeof(Global_Parm.MOT.GCV_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCR_max_speed, sizeof(Global_Parm.MOT.GCR_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCR_min_speed, sizeof(Global_Parm.MOT.GCR_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GP_1mm, sizeof(Global_Parm.MOT.GP_1mm));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GP_max_speed, sizeof(Global_Parm.MOT.GP_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GP_min_speed, sizeof(Global_Parm.MOT.GP_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOH_1mm, sizeof(Global_Parm.MOT.GOH_1mm));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOH_max_speed, sizeof(Global_Parm.MOT.GOH_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOH_min_speed, sizeof(Global_Parm.MOT.GOH_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOV_1mm, sizeof(Global_Parm.MOT.GOV_1mm));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOV_max_speed, sizeof(Global_Parm.MOT.GOV_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOV_min_speed, sizeof(Global_Parm.MOT.GOV_min_speed));
	setBCC(tempbuf, buflen);
	tempbuf[buflen - 1] = 0x23;

	screenUart_sendStr((const char *)tempbuf, buflen);
	myfree(SRAMIN, tempbuf);
	tempbuf = 0;
	tmpptr = 0;

}

// 查询返回-数据-单条步骤
bool queryAck_data_1step(u8 *data, u8 dataLength)
{

	return true;
}






