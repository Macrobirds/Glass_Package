#include "Globalconfig.h"

/* *************************************** ���� *********************************** */

/* *************************************** ����ʵ�� *********************************** */

// �������ת��δС������ ������һĿ��ָ��
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

// �������ת��δС������ ������һԴָ��
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

// ���ش���������
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
	//	if (GOH_mid_Sen == Sen_Block)
	//	{
	//		tempbuf[3] = (1 << 1);
	//	}
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

// ��ѯ����ϵͳ״̬
static void query_state_gas(u8 Index)
{
	u8 *tmpBuf = mymalloc(SRAMIN, 6);
	u32 pressures = 0;
	memset(tmpBuf, 0, 6);
	pressures = P6847_adc_GetPressure();
	mymemcpy1(&(tmpBuf[0]), &pressures, sizeof(pressures));
	tmpBuf[4] = ITV0011_IIC_GetPressure();
	// �����÷�
	if (Gas_State == gas_pumping)
	{
		tmpBuf[5] |= (1 << 0);
	}
	// ��������
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))
	{
		tmpBuf[5] |= (1 << 1);
	}
	// ���ַ�
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
	// ����1
	if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14))
	{
		tmpBuf[5] |= (1 << 5);
	}
	// ����2
	if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15))
	{
		tmpBuf[5] |= (1 << 6);
	}
	// �ν���
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))
	{
		tmpBuf[5] |= (1 << 7);
	}
	screenUart_ack_array(Index, Type_queryAck, Fc_state, Extra_state_gas, tmpBuf, 6);
	myfree(SRAMIN, tmpBuf);
	tmpBuf = 0;
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
	// ���¶�ȡ��������
	switch (extra)
	{
	case Extra_param_motor:
		memset(&Global_Parm.MOT, 0, sizeof(Global_Parm.MOT));
		W25QXX_Read((u8 *)&Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Global_Parm.MOT));
		break;
	case Extra_param_claw:
		memset(&Global_Parm.GCS, 0, sizeof(Global_Parm.GCS));
		W25QXX_Read((u8 *)&Global_Parm.GCS, SpiFlashAddr_clawsupportData, sizeof(Global_Parm.GCS));
		break;
	case Extra_param_package:
		memset(&Global_Parm.GP, 0, sizeof(Global_Parm.GP));
		W25QXX_Read((u8 *)&Global_Parm.GP, SpiFlashAddr_packageData, sizeof(Global_Parm.GP));
		break;
	case Extra_param_outin:
		memset(&Global_Parm.GIO, 0, sizeof(Global_Parm.GIO));
		W25QXX_Read((u8 *)&Global_Parm.GIO, SpiFlashAddr_inoutData, sizeof(Global_Parm.GIO));
		break;
	default:
		break;
	}
	delay_ms(50);
	// ��������
	cpy_fun(&tmpBuf[6]);

	setBCC(tmpBuf, buflen);
	tmpBuf[buflen - 1] = 0x23;
	screenUart_sendStr(tmpBuf, buflen);
	myfree(SRAMIN, tmpBuf);
	tmpBuf = 0;
}

// ��ѯ-����-�������
static void query_param_motor(u8 *bufptr)
{
	u8 *tmpptr = bufptr;
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GE_max_speed, sizeof(Global_Parm.MOT.GE_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GE_min_speed, sizeof(Global_Parm.MOT.GE_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCV_max_speed, sizeof(Global_Parm.MOT.GCV_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCV_min_speed, sizeof(Global_Parm.MOT.GCV_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCR_max_speed, sizeof(Global_Parm.MOT.GCR_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GCR_min_speed, sizeof(Global_Parm.MOT.GCR_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GP_max_speed, sizeof(Global_Parm.MOT.GP_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GP_min_speed, sizeof(Global_Parm.MOT.GP_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOH_max_speed, sizeof(Global_Parm.MOT.GOH_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOH_min_speed, sizeof(Global_Parm.MOT.GOH_min_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOV_max_speed, sizeof(Global_Parm.MOT.GOV_max_speed));
	tmpptr = mymemcpy1(tmpptr, &Global_Parm.MOT.GOV_min_speed, sizeof(Global_Parm.MOT.GOV_min_speed));
	tmpptr = 0;
}

static void query_param_calw(u8 *bufptr)
{

	u8 *tempptr = bufptr;
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCR_hor_pos, sizeof(Global_Parm.GCS.GCR_hor_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCR_ver_pos, sizeof(Global_Parm.GCS.GCR_ver_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCV_down_pos, sizeof(Global_Parm.GCS.GCV_down_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GCV_glass_len, sizeof(Global_Parm.GCS.GCV_glass_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GOH_mid_pos, sizeof(Global_Parm.GCS.GOH_mid_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GCS.GOH_end_pos, sizeof(Global_Parm.GCS.GOH_end_pos));
	tempptr = 0;
}

static void query_param_package(u8 *bufptr)
{
	u8 *tempptr = bufptr;
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.delay_before, sizeof(Global_Parm.GP.delay_before));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.delay_after, sizeof(Global_Parm.GP.delay_after));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.sucker_pos, sizeof(Global_Parm.GP.sucker_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_pos, sizeof(Global_Parm.GP.spray_pos));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_len, sizeof(Global_Parm.GP.spray_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_speed, sizeof(Global_Parm.GP.spray_speed));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GP.spray_pressure, sizeof(Global_Parm.GP.spray_pressure));
	tempptr = 0;
}

static void query_param_outin(u8 *bufptr)
{
	u8 *tempptr = bufptr;
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO.GOV_box_dis, sizeof(Global_Parm.GIO.GOV_box_dis));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO.GOV_slot_dis, sizeof(Global_Parm.GIO.GOV_slot_dis));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO.GOV_box_len, sizeof(Global_Parm.GIO.GOV_box_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO.GOV_adjust, sizeof(Global_Parm.GIO.GOV_adjust));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO.GOV_adjust_start, sizeof(Global_Parm.GIO.GOV_adjust_start));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO.GE_box_len, sizeof(Global_Parm.GIO.GE_box_len));
	tempptr = mymemcpy1(tempptr, &Global_Parm.GIO.GE_box_speed, sizeof(Global_Parm.GIO.GE_box_speed));
	tempptr = 0;
}

//// ����-����-�������
u8 set_param_motor(u8 *data, u8 dataLength)
{
	u8 *tmpptr = data;
	if (dataLength != 14)
	{
		tmpptr = 0;
		return 0;
	}
	if (!TaskThread_CheckIdle()) // ����״̬��Ϊ����״̬
	{
		tmpptr = 0;
		return 0;
	}
	// д����������ȫ�ֱ���
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GE_max_speed, sizeof(Global_Parm.MOT.GE_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GE_min_speed, sizeof(Global_Parm.MOT.GE_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCV_max_speed, sizeof(Global_Parm.MOT.GCV_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCV_min_speed, sizeof(Global_Parm.MOT.GCV_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCR_max_speed, sizeof(Global_Parm.MOT.GCR_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GCR_min_speed, sizeof(Global_Parm.MOT.GCR_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GP_max_speed, sizeof(Global_Parm.MOT.GP_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GP_min_speed, sizeof(Global_Parm.MOT.GP_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOH_max_speed, sizeof(Global_Parm.MOT.GOH_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOH_min_speed, sizeof(Global_Parm.MOT.GOH_min_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOV_max_speed, sizeof(Global_Parm.MOT.GOV_max_speed));
	tmpptr = mymemcpy2(tmpptr, &Global_Parm.MOT.GOV_min_speed, sizeof(Global_Parm.MOT.GOV_min_speed));
	tmpptr = 0;
	// ���õ������
	motor_parameter_Init();
	W25QXX_Write((u8 *)&Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Global_Parm.MOT));
	delay_ms(50);
	return 1;
}

static u8 set_param_claw(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 20)
	{
		tempptr = 0;
		return 0;
	}
	if (!TaskThread_CheckIdle()) // ����״̬��Ϊ����״̬
	{
		tempptr = 0;
		return 0;
	}
	// д����ֲ���
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCR_hor_pos, sizeof(Global_Parm.GCS.GCR_hor_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCR_ver_pos, sizeof(Global_Parm.GCS.GCR_ver_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCV_down_pos, sizeof(Global_Parm.GCS.GCV_down_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GCV_glass_len, sizeof(Global_Parm.GCS.GCV_glass_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GOH_mid_pos, sizeof(Global_Parm.GCS.GOH_mid_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GCS.GOH_end_pos, sizeof(Global_Parm.GCS.GOH_end_pos));
	tempptr = 0;
	// �����������
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GCS, SpiFlashAddr_clawsupportData, sizeof(Global_Parm.GCS));
	delay_ms(50);
	return 1;
}

static u8 set_param_package(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 18)
	{
		tempptr = 0;
		return 0;
	}
	if (!TaskThread_CheckIdle()) // ����״̬��Ϊ����״̬
	{
		tempptr = 0;
		return 0;
	}
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.delay_before, sizeof(Global_Parm.GP.delay_before));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.delay_after, sizeof(Global_Parm.GP.delay_after));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.sucker_pos, sizeof(Global_Parm.GP.sucker_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_pos, sizeof(Global_Parm.GP.spray_pos));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_len, sizeof(Global_Parm.GP.spray_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_speed, sizeof(Global_Parm.GP.spray_speed));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GP.spray_pressure, sizeof(Global_Parm.GP.spray_pressure));
	tempptr = 0;
	// �����������
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GP, SpiFlashAddr_packageData, sizeof(Global_Parm.GP));
	delay_ms(50);
	return 1;
}

static u8 set_param_outin(u8 *data, u8 datalen)
{
	u8 *tempptr = data;
	if (datalen != 24)
	{
		tempptr = 0;
		return 0;
	}
	if (!TaskThread_CheckIdle()) // ����״̬��Ϊ����״̬
	{
		tempptr = 0;
		return 0;
	}
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO.GOV_box_dis, sizeof(Global_Parm.GIO.GOV_box_dis));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO.GOV_slot_dis, sizeof(Global_Parm.GIO.GOV_slot_dis));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO.GOV_box_len, sizeof(Global_Parm.GIO.GOV_box_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO.GOV_adjust, sizeof(Global_Parm.GIO.GOV_adjust));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO.GOV_adjust_start, sizeof(Global_Parm.GIO.GOV_adjust_start));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO.GE_box_len, sizeof(Global_Parm.GIO.GE_box_len));
	tempptr = mymemcpy2(tempptr, &Global_Parm.GIO.GE_box_speed, sizeof(Global_Parm.GIO.GE_box_speed));

	tempptr = 0;
	// �����������
	TaskThread_Parm_Init();
	W25QXX_Write((u8 *)&Global_Parm.GIO, SpiFlashAddr_inoutData, sizeof(Global_Parm.GIO));
	delay_ms(50);
	return 1;
}

static u8 set_state_gas(u8 data)
{
	if (TaskThread_CheckIdle())
	{
		return 0;
	}
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
#ifdef BIG_CYLINDER
		GP_big_Cyl = GAS_ENABLE;
#endif
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
	TaskThread_State = taskthread_debug; // ����debugģʽ
	return 1;
}

// ���յ������ݽ���Э�����
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
	// �ж�Э������
	switch (TYPE)
	{
	// ��ѯЭ��
	case Type_query:
	{
		switch (FC)
		{
		case Fc_state:
		{
			switch (Extra)
			{ // ����״̬
			case Extra_state_online:
			{
				ack(Index, Type_queryAck, FC, Extra, 0);
				break;
			} // �豸״̬
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
			} // �豸λ��
			case Extra_state_position:
			{
				tmpBuf = mymalloc(SRAMIN, 24);
				memset(tmpBuf, 0, 24);
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
				query_param(Index, 22, Extra_param_motor, query_param_motor);
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
				query_param(Index, 28, Extra_param_claw, query_param_calw);
				break;
			case Extra_param_package:
				query_param(Index, 27, Extra_param_package, query_param_package);
				break;
			case Extra_param_outin:
				query_param(Index, 32, Extra_param_outin, query_param_outin);
				break;
			}
			break;
		}
		}
		break;
	}
	// ����Э��
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

				if (set_param_motor(data, dataLength))
				{
					// ����������óɹ�
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
				// ����δ��Ȩ
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
			// ���ô�ӡ����
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
			// ���ü������̲���
			case Extra_param_claw:
			{
				// �������óɹ�
				if (set_param_claw(data, dataLength))
				{
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				break;
			}
			// ���÷�Ƭ����
			case Extra_param_package:
				if (set_param_package(data, dataLength))
				{
					ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
				}
				else
					ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
				break;

			// ���ý����۲���
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
		// ����������״̬
		case Extra_state_gas:
			if (set_state_gas(data[0]))
			{
				ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
			}
			else
				ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
			break;
		}
		break;
	}
	// ����Э��
	case Type_control:
	{
		switch (FC)
		{
		case Fc_debug:
			if (TaskThread_State == taskthread_running)
				return;							 // ϵͳ��������ʱ�޷�����debugģʽ
			TaskThread_State = taskthread_debug; // ����debugģʽ
			switch (Extra)
			{
			// ������ָ�������������
			case Extra_debug_GE:
				if (data[0] <= GE_finish && data[0] >= GE_start)
				{
					GE.task = data[0];
					GE.sta = Ready;
				}
				break;
			case Extra_debug_GC:
				if (data[0] <= GC_finish && data[0] >= GC_start)
				{
					GC.task = data[0];
					GC.sta = Ready;
				}
				break;
			case Extra_debug_GP:
				if (data[0] <= GP_finish && data[0] >= GP_start)
				{
					GP.task = data[0];
					GP.sta = Ready;
				}
				break;
			case Extra_debug_GO:
				if (data[0] <= GO_finish && data[0] >= GO_start)
				{
					GO.task = data[0];
					GO.sta = Ready;
				}
				break;
			}
			break;
		case Fc_run:
			switch (Extra)
			{
			// ��λ����
			case Extra_run_Reset:
			{
				if (dataLength != 1)
				{
					if (data[0])
					{
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
					}

					break;
				}
				// ��Ҫϵͳ����
				if (TaskThread_State != taskthread_error)
				{
					if (TaskThread_CheckIdle())
					{
						// ���п�����λ
						Boot_ResetTaskThread();
						if (data[0])
							ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
					}
					else
					{
						if (data[0])
							ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
					}
				}
				// �Ƿ���ҪӦ���־λ
				if (data[0] == OnOffState_on)
				{
					GE.WaitAck = 1;
					GE.Index = Index;
				}
				break;
			}
			// ���۶���
			case Extra_run_GEInOut:
			{
				if (dataLength != 1)
				{
					if (data[0])
					{
						ack(Index, Type_controlAck, FC, Extra, AckResult_ackFailed);
					}

					break;
				}
				// ϵͳ��������״̬����Ҫֹͣϵͳ
	

				TaskThread_State = taskthread_boost;
				// ������������
				GE.sta = Ready;
				GE.task = GE_Box_In;
				// �Ƿ���ҪӦ���־λ
				if (data[0] == OnOffState_on)
				{
					GE.Index = Index;
					GE.WaitAck = 1;
				}
				break;
			}
			// ���۶���
			case Extra_run_GEOut:
			{
				if (dataLength != 1)
					break;

				if (TaskThread_State == taskthread_running)
				{
					Pause_TaskThread();
					delay_ms(1000);
				}
				TaskThread_State = taskthread_boost;
				// ������������
				GE.sta = Ready;
				GE.task = GE_BOx_Out;
				// �Ƿ���ҪӦ���־λ
				if (data[0] == OnOffState_on)
				{
					GE.Index = Index;
					GE.WaitAck = 1;
				}
				break;
			}
			// ���׶���
			case Extra_run_GOIn:
				if (dataLength != 1)
					break;

				if (TaskThread_State == taskthread_running)
				{
					Pause_TaskThread();
					delay_ms(1000);
				}
				TaskThread_State = taskthread_boost;
				// ������������
				GO.sta = Ready;
				GO.task = GO_Box_In;
				// �Ƿ���ҪӦ���־λ
				if (data[0] == OnOffState_on)
				{
					GO.Index = Index;
					GO.WaitAck = 1;
				}
				break;
			// ���׶���
			case Extra_run_GOOut:
				if (dataLength != 1)
					break;

				if (TaskThread_State == taskthread_running)
				{
					Pause_TaskThread();
					delay_ms(1000);
				}
				TaskThread_State = taskthread_boost;
				// ������������
				GO.sta = Ready;
				GO.task = GO_Box_Out;
				// �Ƿ���ҪӦ���־λ
				if (data[0] == OnOffState_on)
				{
					GO.Index = Index;
					GO.WaitAck = 1;
				}
				break;
			// ��ʼ����
			case Extra_run_Start:
				if (dataLength != 1)
					break;
				// �������׼��״̬
				if (TaskThread_IsReady() != taskthread_ready) //
				{
				}

				if (TaskThread_State == taskthread_pause)
				{

					Resume_TaskThread();
				}
				break;
			// ��ͣ����
			case Extra_run_Pause:
				break;
			// �ػ�����
			case Extra_run_Close:
				break;
			}
			break;
		}
		break;
	}
	}
	// ���data��ʱ��������
	myfree(SRAMIN, data);
	data = 0;
}
// ͨ��Ӧ��-Ӧ��1λ����
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

// �����߳��е���Ӧ��-Ӧ��1λ����
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

// ͨ��Ӧ��-Ӧ��0λ����
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

// ͨ��Ӧ��-Ӧ��2λ����
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
// ͨ��Ӧ��-Ӧ��3λ����
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

// ͨ��Ӧ��-Ӧ���λ����
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

// �����߳��е���Ӧ��-Ӧ���λ����
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


// ͨ������ 2data
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

// // ��ѯ-����-�������
// void query_param_motor(u8 Index)
// {
// 	u8 *tempbuf = 0;
// 	u8 *tmpptr = 0;
// 	u8 buflen = 42;
// 	tempbuf = mymalloc(SRAMIN, buflen);
// 	tmpptr = tempbuf;
// 	memset(tempbuf, 0, buflen);
// 	// ������� ��Flash �����¶�ȡ��
// 	memset(&Global_Parm.MOT, 0, sizeof(Global_Parm.MOT));
// 	W25QXX_Read((u8 *)&Global_Parm.MOT, SpiFlashAddr_motorData, sizeof(Global_Parm.MOT));
// 	delay_ms(50);
// 	tempbuf[0] = 0x3a;
// 	tempbuf[1] = 54;
// 	tempbuf[2] = Index;
// 	tempbuf[3] = Type_queryAck;
// 	tempbuf[4] = Fc_param;
// 	tempbuf[5] = Extra_param_motor;

// 	// ��ȡ�������
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

// ��ѯ����-����-��������
bool queryAck_data_1step(u8 *data, u8 dataLength)
{

	return true;
}
