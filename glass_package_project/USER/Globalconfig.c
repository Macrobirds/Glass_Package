#include "Globalconfig.h"

OS_CPU_SR cpu_sr = 0;
struct Global_Parameter_struct Global_Parm={0};
enum sensor_index sensor_error_idx=0;
volatile u32 nowRtcTime = 0;
u32 deviceSnNumber = 0;
u8 DeviceType = DeviceType_Default;
static u8 Carrier_Versions_Default = 0;
u8 debug_flag=FALSE;

void Set_Global_Parameter_Default(void)
{

}

void setBCC(u8 *data, u8 dataLength)
{
    u8 i = 0, bcc = 0;
    for(i = 0; i < dataLength - 2; i++)
        bcc = bcc ^ data[i];
    data[dataLength - 2] = bcc;
}

void set_carrierVersions_Default(void)
{
	Carrier_Versions_Default = 0;
	// 判断设备类型
	switch(DeviceType)
	{
		case DeviceType_dbj_zw_bmh:
			Carrier_Versions_Default = BMH;	
			//mpd.carriersVersions = BMH;
			break;
		case DeviceType_dbj_zw_bp:
			Carrier_Versions_Default = BP;	
			//mpd.carriersVersions = BP;
			break;
		// 未设置机型，使用默认参数
		case DeviceType_none:
		default:
			break;
	}
}