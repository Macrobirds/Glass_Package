#include "Globalconfig.h"

OS_CPU_SR cpu_sr = 0;

Motor_Data MOT_Parm = {0};
Glass_ClawSupport__data GCS_Parm = {0};
Glass_Package_Data GP_Parm = {0};
Glass_In_Out_Data GIO_Parm = {0};

u32 sensor_error_idx = 0;
volatile u32 nowRtcTime = 0;
u32 deviceSnNumber = 0;
u8 DeviceType = DeviceType_Default;
static u8 Carrier_Versions_Default = 0;
u8 debug_flag = FALSE;

struct Global_Parameter_struct Global_Parm = {
	.MOT = &MOT_Parm,
	.GCS = &GCS_Parm,
	.GP = &GP_Parm,
	.GIO = &GIO_Parm,
};

void Set_Global_Parameter_Default(void)
{
	// set GE task parm
	Global_Parm.GIO->GE_box_len = GE.GE_box_len * SCALE / GE_motor_struct.pulse_1mm;
	Global_Parm.GIO->GE_box_speed = GE.GE_box_speed / GE_motor_struct.pulse_1mm;
	// set GC task parm
	Global_Parm.GCS->GCR_hor_pos = GC.GCR_hor_pos;
	Global_Parm.GCS->GCR_ver_pos = GC.GCR_ver_pos;
	Global_Parm.GCS->GCV_down_pos = GC.GCV_down_pos * SCALE / GC_ver_motor_struct.pulse_1mm;
	Global_Parm.GCS->GCV_glass_len = GC.GCV_glass_len * SCALE / GC_ver_motor_struct.pulse_1mm;
	Global_Parm.GCS->GCV_package_pos = GC.GCV_pacakge_pos;

	// set GP parm
	Global_Parm.GP->delay_after = GP.delay_after;
	Global_Parm.GP->delay_before = GP.delay_before;
	Global_Parm.GP->sucker_pos = GP.sucker_pos * SCALE / GP_motor_struct.pulse_1mm;
	Global_Parm.GP->spray_pos = GP.spray_pos * SCALE / GP_motor_struct.pulse_1mm;
	Global_Parm.GP->spray_len = GP.spray_len * SCALE / GP_motor_struct.pulse_1mm;
	Global_Parm.GP->spray_speed = GP.spray_speed / GP_motor_struct.pulse_1mm;
	Global_Parm.GP->spray_pressure = GP.spray_pressure;
	// set GO parm
	Global_Parm.GCS->GOH_end_pos = GO.GOH_end_pos * SCALE / GO_hor_motor_struct.pulse_1mm;
	Global_Parm.GCS->GOH_mid_pos = GO.GOH_mid_pos * SCALE / GO_hor_motor_struct.pulse_1mm;
	Global_Parm.GIO->GOV_box_dis = GO.GOV_box_dis * SCALE / GO_ver_motor_struct.pulse_1mm;
	Global_Parm.GIO->GOV_slot_dis = GO.GOV_slot_dis * SCALE / GO_ver_motor_struct.pulse_1mm;
	Global_Parm.GIO->GOV_box_len = GO.GOV_box_len * SCALE / GO_ver_motor_struct.pulse_1mm;
	Global_Parm.GIO->GOV_adjust = GO.GOV_adjust;
	Global_Parm.GIO->GOV_adjust_start = GO.GOV_adjust_start;

	// set GE motor parm
	Global_Parm.MOT->GE_speed = GE_motor_struct.defaultfeq / GE_motor_struct.pulse_1mm;
	Global_Parm.MOT->GE_dir = GE_motor_struct.FRONT;

	// set GCV motor parm

	Global_Parm.MOT->GCV_speed = GC_ver_motor_struct.defaultfeq / GC_ver_motor_struct.pulse_1mm;
	Global_Parm.MOT->GC_ver_dir = GC_ver_motor_struct.FRONT;

	// set GCR motor parm
	Global_Parm.MOT->GCR_speed = GC_rot_motor_struct.defaultfeq;
	Global_Parm.MOT->GC_rot_dir = GC_rot_motor_struct.FRONT;

	// set GP motor parm
	Global_Parm.MOT->GP_speed = GP_motor_struct.defaultfeq / GP_motor_struct.pulse_1mm;
	Global_Parm.MOT->GP_dir = GP_motor_struct.FRONT;

	// set GOH motor parm
	Global_Parm.MOT->GOH_speed = GO_hor_motor_struct.defaultfeq / GO_hor_motor_struct.pulse_1mm;
	Global_Parm.MOT->GOH_dir = GO_hor_motor_struct.FRONT;

	// set GOV motor parm
	Global_Parm.MOT->GOV_speed = GO_ver_motor_struct.defaultfeq / GO_ver_motor_struct.pulse_1mm;
	Global_Parm.MOT->GOV_dir= GO_ver_motor_struct.FRONT;
}

void setBCC(u8 *data, u8 dataLength)
{
	u8 i = 0, bcc = 0;
	for (i = 0; i < dataLength - 2; i++)
		bcc = bcc ^ data[i];
	data[dataLength - 2] = bcc;
}

void set_carrierVersions_Default(void)
{
	Carrier_Versions_Default = 0;
	// 判断设备类型
	switch (DeviceType)
	{
	case DeviceType_dbj_zw_bmh:
		Carrier_Versions_Default = BMH;
		// mpd.carriersVersions = BMH;
		break;
	case DeviceType_dbj_zw_bp:
		Carrier_Versions_Default = BP;
		// mpd.carriersVersions = BP;
		break;
	// 未设置机型，使用默认参数
	case DeviceType_fpj:
		Carrier_Versions_Default = FPJ;
		break;
	case DeviceType_none:
	default:
		break;
	}
}

// 获取设备序列号
void getDeviceSnNumber(void)
{
	u32 ChipUniqueID[3];
	// 地址从小到大,先放低字节，再放高字节：小端模式
	// 地址从小到大,先放高字节，再放低字节：大端模式
	ChipUniqueID[2] = *(__IO u32 *)(0X1FFFF7E8); // 低字节
	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); //
	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
	deviceSnNumber = (long)((ChipUniqueID[0] ^ ChipUniqueID[1] ^ ChipUniqueID[2]) & 0xffffff);
}

// void checkPirate(void)
//{
//	// 如果检验失败，程序是盗版，破坏程序
//	if (verifyCiphertextExistError())
//	{
//		// 如果读保护已经开启，关闭读保护，格式化芯片
//		if (FLASH_GetReadOutProtectionStatus() == SET)
//			close_FlashReadProtect();
//		// 读保护未开启，先打开，等下开机再格式化
//		else
//		{
//			open_FlashReadProtect();
//			POWER = !POWER_ON;
//		}
//	}
// }