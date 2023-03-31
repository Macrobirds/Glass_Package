#include "uart3_dataHandle.h"
#include "Globalconfig.h"
/* *************************************** 常量 *********************************** */


/* *************************************** 函数实现 *********************************** */
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
	screenUart_lastRecvIndex = Index;
	
//	uart1_SendString((char *)screenUart_RecvCompleteBuf, protocolLength);
	
	if(dataLength > 0)
	{
		data = mymalloc(SRAMIN, dataLength);	
		memset(data, 0, dataLength);
		memcpy(data, screenUart_RecvCompleteBuf + 6, dataLength);
	}
	// 判断协议类型
	switch(TYPE)
	{
		// 查询协议
		case Type_query:
		{
			switch(FC)
				{
					case Fc_state:
					{
						switch(Extra)
						{
							case Extra_state_online:
							{
								ack(Index, Type_queryAck, FC, Extra, 0);
								break;
							}
							case Extra_state_deviceState:
							{
								tmpBuf = mymalloc(SRAMIN, 13);
								memset(tmpBuf, 0, 13);

								screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 13);
								myfree(SRAMIN, tmpBuf);
								tmpBuf = 0;
								break;
							}
							case Extra_state_position:
							{
								tmpBuf = mymalloc(SRAMIN, 10);
								memset(tmpBuf, 0, 10);

								screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 10);
								myfree(SRAMIN, tmpBuf);
								tmpBuf = 0;
								break;
							}

						}
						break;
					}
					case Fc_param:
					{
						switch(Extra)
						{
							case Extra_param_motor:
								query_param_motor(Index);
								break;
							case Extra_param_deviceInfo:
							{
								tmpBuf = mymalloc(SRAMIN, 7);
								memset(tmpBuf, 0, 7);

								screenUart_ack_array(Index, Type_queryAck, FC, Extra, tmpBuf, 7);
								myfree(SRAMIN, tmpBuf);
								tmpBuf = 0;
								break;
							}
							case Extra_param_printf:
								query_param_printf(Index);
								break;
							case Extra_param_bak:
								query_param_bak(Index);
								break;
						}
						break;
					}
//					case Fc_error:
//					{
//						switch(Extra)
//						{
//							case Extra_error_tempSensor:
//								sendProtocol_2data(Index, Type_queryAck, FC,
//									Extra, ((tempSensor_errorNum >> 8) & 0xff), 
//									(tempSensor_errorNum& 0xff));
//								break;
//						}
//						break;
//					}
				}
			break;
		}
		// 查询返回
		case Type_queryAck:
		{
			switch(FC)
				{
					case Fc_data:
//						switch(Extra)
//						{
//							case Extra_data_1step:
//								queryAck_data_1step(data, dataLength);
//								break;
//						}
						break;
				}
			break;
		}


		
		// 设置协议
		case Type_set:
		{
			switch(FC)
				{
					case Fc_param:
					{
						switch(Extra)
						{
							case Extra_param_motor:
							{
								// 参数设置成功
								if(set_param_motor(data, dataLength))
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
								if(verifyCiphertextExistError())
								{
									if(dataLength != 4)
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
							case Extra_param_printf:
							{
								// 参数设置成功
								if(set_param_printf(data, dataLength))
								{
									ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
								}
								else
									ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
								break;
							}
							// 设置打印参数
							case Extra_param_bak:
							{
								// 参数设置成功
								if(set_param_bak(data, dataLength))
								{
									ack(Index, Type_setAck, FC, Extra, AckResult_ackSucceed);
								}
								else
									ack(Index, Type_setAck, FC, Extra, AckResult_ackFailed);
								break;
							}
//							case Extra_param_deviceType:
//							{
//								if(dataLength != 1)
//									break;
//								// 检查设备类型参数
//								if(!checkDeviceType_isExist(data[0]))
//									break;
//								DeviceType = data[0];
//								// 设置默认深孔板类型
//								set_carrierVersions_Default();
//								// 修改到该设备类型的默认参数
//								update_globalParam();
//								W25QXX_Write(&DeviceType, SpiFlashAddr_DeviceType, 1);
//								delay_ms(20);
//								mpd.carriersVersions = Carrier_Versions_Default;
//								W25QXX_Write((u8 *)&mpd,
//                     SpiFlashAddr_motorPositionData, sizeof(mpd));
//								ack(Index, Type_setAck, FC, Extra, DeviceType);
//								break;
//							}
						}
						break;
					}
					case Fc_data:
						break;
				}
			break;
		}
		// 控制协议
		case Type_control:
		{
			switch(FC)
				{
					case Fc_debug:
						control_debug(Index, Extra);
						break;
					case Fc_run:
						switch(Extra)
						{
							// 复位动作
							case Extra_run_motorReset:
							{
								if(dataLength != 1)
									break;
								// 是否需要应答标志位
								if(data[0] == OnOffState_on)
								{
									deviceState.waitAck = true;
									deviceState.waitAckIndex = Index;
								}
								else
									deviceState.waitAck = false;
								// 不做状态检测，强制复位
//								if(check_deviceCanControl())
//								{
//									stepperMotorStop(Motor_XZ);
									stepperMotorStop(Motor_X);
									stepperMotorStop(Motor_Y);
									deviceState.motorReset_runningState = RunningState_readyStart;	
									deviceState.motorReset_threadEnable = ON;
									ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
//								}
//								else
//									ack_2data(Index, Type_controlAck, FC, Extra,
//										AckResult_ackFailed, Reason_deviceBusy);
								break;
							}
							case Extra_run_pushinBox:
							{
								if(dataLength != 2)
									break;
								// 是否需要应答标志位
								if(data[0] == OnOffState_on)
								{
									deviceState.waitAck = true;
									deviceState.waitAckIndex = Index;
								}
								else
									deviceState.waitAck = false;
//								deviceState.targetSite = data[1];
//								if(deviceState.targetSite > BoxChannel)
//									deviceState.targetSite = 0;
								if(check_deviceCanControl())
								{
									deviceState.pushinBox_runningState = RunningState_readyStart;	
									deviceState.pushinBox_threadEnable = ON;
									ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
								}
								else
									ack_2data(Index, Type_controlAck, FC, Extra,
										AckResult_ackFailed, Reason_deviceBusy);
								break;
							}
							case Extra_run_pushOutBox:
							{
								if(dataLength != 1)
									break;
								// 是否需要应答标志位
								if(data[0] == OnOffState_on)
								{
									deviceState.waitAck = true;
									deviceState.waitAckIndex = Index;
								}
								else
									deviceState.waitAck = false;
								if(check_deviceCanControl())
								{
									deviceState.pushOutBox_runningState = RunningState_readyStart;	
									deviceState.pushOutBox_threadEnable = ON;
									ack(Index, Type_controlAck, FC, Extra, AckResult_ackSucceed);
								}
								else
									ack_2data(Index, Type_controlAck, FC, Extra,
										AckResult_ackFailed, Reason_deviceBusy);
								break;
							}
						}
						break;
				}
			break;
		}
	}
	// 清除data临时数组数据
	myfree(SRAMIN,data);
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
	if(dataLength > 0)
		memcpy(tmpBuf + 6, data, dataLength);
	setBCC(tmpBuf, tmpBufLength);
	tmpBuf[tmpBufLength - 1] = 0x23;
	screenUart_sendStr((const char *)tmpBuf, tmpBufLength);
	myfree(SRAMIN,tmpBuf);
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
	myfree(SRAMIN,tmpBuf);
	tmpBuf = 0;
}

// 查询-参数-电机参数 
void query_param_motor(u8 Index)
{
	float tmpFloat = 0;
	u8 * tmpBuf = 0;
	u8 tmpBufLength = 0x25;
	tmpBuf = mymalloc(SRAMIN, tmpBufLength);
	memset(tmpBuf, 0, tmpBufLength);
	memset(&mpd, 0, sizeof(mpd));
	W25QXX_Read((u8 *)&mpd,SpiFlashAddr_motorPositionData,sizeof(mpd));
	delay_ms(50);
	tmpBuf[0] = 0x3A;
	tmpBuf[1] = tmpBufLength;
	tmpBuf[2] = Index;
	tmpBuf[3] = Type_queryAck;
	tmpBuf[4] = Fc_param;
	tmpBuf[5] = Extra_param_motor;
	tmpBuf[6] = 0;//mpd.check_XZ6_enable & 0xff;
	
	tmpBuf[7] = 0;//(mpd.XZ_1d >> 8) & 0xff;
	tmpBuf[8] = 0;//mpd.XZ_1d & 0xff;
	tmpBuf[9] = (mpd.X_1mm >> 8) & 0xff;
	tmpBuf[10] = mpd.X_1mm & 0xff;
	tmpBuf[11] = (mpd.Y_1mm >> 8) & 0xff;
	tmpBuf[12] = mpd.Y_1mm & 0xff;
	
	tmpFloat = 0;//mpd.XZ_accStartFreq / (float)mpd.XZ_1d;
	tmpBuf[13] = 0;//(ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = mpd.X_accStartFreq / (float)mpd.X_1mm;
	tmpBuf[14] = (ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = mpd.Y_accStartFreq / (float)mpd.Y_1mm;
	tmpBuf[15] = (ROUND_TO_UINT16(tmpFloat)) & 0xff;

	tmpFloat = 0;//mpd.XZ_accMaxFreq / (float)mpd.XZ_1d;
	tmpBuf[16] = 0;//(ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = mpd.X_accMaxFreq / (float)mpd.X_1mm;
	tmpBuf[17] = (ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = mpd.Y_accMaxFreq / (float)mpd.Y_1mm;
	tmpBuf[18] = (ROUND_TO_UINT16(tmpFloat)) & 0xff;

	tmpFloat = 0;//mpd.XZ_site_1 / (float)mpd.XZ_1d * 10;
	tmpBuf[19] = 0;//(ROUND_TO_UINT16(tmpFloat) >> 8) & 0xff;
	tmpBuf[20] = 0;//(ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = 0;//mpd.XZ_site_2 / (float)mpd.XZ_1d * 10;
	tmpBuf[21] = 0;//(ROUND_TO_UINT16(tmpFloat) >> 8) & 0xff;
	tmpBuf[22] = 0;//(ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = 0;//mpd.XZ_site_3 / (float)mpd.XZ_1d * 10;
	tmpBuf[23] = 0;//(ROUND_TO_UINT16(tmpFloat) >> 8) & 0xff;
	tmpBuf[24] = 0;//(ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = 0;//mpd.XZ_site_4 / (float)mpd.XZ_1d * 10;
	tmpBuf[25] = 0;//(ROUND_TO_UINT16(tmpFloat) >> 8) & 0xff;
	tmpBuf[26] = 0;//(ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = 0;//mpd.XZ_site_5 / (float)mpd.XZ_1d * 10;
	tmpBuf[27] = 0;//(ROUND_TO_UINT16(tmpFloat) >> 8) & 0xff;
	tmpBuf[28] = 0;//(ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = 0;//mpd.XZ_site_6 / (float)mpd.XZ_1d * 10;
	tmpBuf[29] = 0;//(ROUND_TO_UINT16(tmpFloat) >> 8) & 0xff;
	tmpBuf[30] = 0;//(ROUND_TO_UINT16(tmpFloat)) & 0xff;
	
	tmpFloat = mpd.X_end / (float)mpd.X_1mm * 10;
	tmpBuf[31] = (ROUND_TO_UINT16(tmpFloat) >> 8) & 0xff;
	tmpBuf[32] = (ROUND_TO_UINT16(tmpFloat)) & 0xff;
	tmpFloat = mpd.Y_end / (float)mpd.Y_1mm * 10;
	tmpBuf[33] = (ROUND_TO_UINT16(tmpFloat) >> 8) & 0xff;
	tmpBuf[34] = (ROUND_TO_UINT16(tmpFloat)) & 0xff;
	
	setBCC(tmpBuf, tmpBufLength);
	tmpBuf[tmpBufLength - 1] = 0x23;
	screenUart_sendStr((const char *)tmpBuf, tmpBufLength);
	myfree(SRAMIN,tmpBuf);
	tmpBuf = 0;
}

// 查询返回-数据-单条步骤
bool queryAck_data_1step(u8* data, u8 dataLength)
{
//	u16 tmpU16_1 = 0;
//	if(dataLength != 38)
//		return false;
//	tmpU16_1 = ((data[0] << 8) & 0xff00) + (data[1] & 0xff);
//	if(deviceState.programIndex != tmpU16_1)
//		return false;
//	tmpU16_1 = ((data[2] << 8) & 0xff00) + (data[3] & 0xff);
//	if(deviceState.nowRunStepIndex != tmpU16_1)
//		return false;
//	stepInfo.indexNum = deviceState.nowRunStepIndex;
//	stepInfo.hole = ((data[4] << 8) & 0xff00) + (data[5] & 0xff);
//	stepInfo.volume = ((data[6] << 8) & 0xff00) + (data[7] & 0xff);
//	stepInfo.mixtureTime = ((data[8] << 8) & 0xff00) + (data[9] & 0xff);
//	stepInfo.mixtureSpeed = ((data[10] << 8) & 0xff00) + (data[11] & 0xff);
//	stepInfo.waitTime = ((data[12] << 8) & 0xff00) + (data[13] & 0xff);
//	stepInfo.waitPosition = ((data[14] << 8) & 0xff00) + (data[15] & 0xff);
//	stepInfo.suckTime = ((data[16] << 8) & 0xff00) + (data[17] & 0xff);
//	stepInfo.waterWait = ((data[18] << 8) & 0xff00) + (data[19] & 0xff);
//	stepInfo.bottomWait = ((data[20] << 8) & 0xff00) + (data[21] & 0xff);
//	stepInfo.waterHeight = ((data[22] << 8) & 0xff00) + (data[23] & 0xff);
//	stepInfo.heatHole = ((data[24] << 8) & 0xff00) + (data[25] & 0xff);
//	stepInfo.temp = ((data[26] << 8) & 0xff00) + (data[27] & 0xff);
//	
//	stepInfo.x_mixtureSpeed = ((data[28] << 8) & 0xff00) + (data[29] & 0xff);
//	stepInfo.x_suckTime = ((data[30] << 8) & 0xff00) + (data[31] & 0xff);
//	stepInfo.x_suckLeftRightWaitTime = ((data[32] << 8) & 0xff00) + (data[33] & 0xff);
//	stepInfo.x_suckPhaseCount = ((data[34] << 8) & 0xff00) + (data[35] & 0xff);
//	stepInfo.fanState = ((data[36] << 8) & 0xff00) + (data[37] & 0xff);
//	
//	if(deviceState.main_motionState == MotionState_executing
//				&& deviceState.main_motion == Motion_getStepInfo)
//		deviceState.main_motionState = MotionState_executeFinish;
	return true;
}


// 设置-参数-电机参数 
bool set_param_motor(u8* data, u8 dataLength)
{
	u16 tmp_speed = 0;
	if(dataLength != 29)
		return false;
//	mpd.check_XZ6_enable = data[0] & 0xff;
//	mpd.XZ_1d = ((data[1] << 8) & 0xff00) + (data[2] & 0xff);
	mpd.X_1mm = ((data[3] << 8) & 0xff00) + (data[4] & 0xff);
	mpd.Y_1mm = ((data[5] << 8) & 0xff00) + (data[6] & 0xff);
	
//	tmp_speed = data[7] & 0xff;
//	mpd.XZ_accStartFreq = mpd.XZ_1d * tmp_speed;
	tmp_speed = data[8] & 0xff;
	mpd.X_accStartFreq = mpd.X_1mm * tmp_speed;
	tmp_speed = data[9] & 0xff;
	mpd.Y_accStartFreq = mpd.Y_1mm * tmp_speed;
	
//	tmp_speed = data[10] & 0xff;
//	mpd.XZ_accMaxFreq = mpd.XZ_1d * tmp_speed;
	tmp_speed = data[11] & 0xff;
	mpd.X_accMaxFreq = mpd.X_1mm * tmp_speed;
	tmp_speed = data[12] & 0xff;
	mpd.Y_accMaxFreq = mpd.Y_1mm * tmp_speed;
	
//	mpd.XZ_site_1 = mpd.XZ_1d * (((data[13] << 8) & 0xff00) + (data[14] & 0xff))/10;
//	mpd.XZ_site_2 = mpd.XZ_1d * (((data[15] << 8) & 0xff00) + (data[16] & 0xff))/10;
//	mpd.XZ_site_3 = mpd.XZ_1d * (((data[17] << 8) & 0xff00) + (data[18] & 0xff))/10;
//	mpd.XZ_site_4 = mpd.XZ_1d * (((data[19] << 8) & 0xff00) + (data[20] & 0xff))/10;
//	mpd.XZ_site_5 = mpd.XZ_1d * (((data[21] << 8) & 0xff00) + (data[22] & 0xff))/10;
//	mpd.XZ_site_6 = mpd.XZ_1d * (((data[23] << 8) & 0xff00) + (data[24] & 0xff))/10;
	
	mpd.X_end = mpd.X_1mm * (((data[25] << 8) & 0xff00) + (data[26] & 0xff))/10;
	mpd.Y_end = mpd.Y_1mm * (((data[27] << 8) & 0xff00) + (data[28] & 0xff))/10;
	
//	mpd.XZ_defaultFreq = mpd.XZ_accMaxFreq / 2;
	mpd.X_defaultFreq = mpd.X_1mm * 30;
	mpd.Y_defaultFreq = mpd.Y_1mm * 30;
	
	W25QXX_Write((u8 *)&mpd,SpiFlashAddr_motorPositionData,sizeof(mpd));
	delay_ms(50);
	return true;
}


// 查询-参数-打印参数 
void query_param_printf(u8 Index)
{
	u8 * tmpBuf = 0;
	u8 tmpBufLength = 8 + FlashSIZE_printfParam;
	tmpBuf = mymalloc(SRAMIN, tmpBufLength);
	memset(tmpBuf, 0, tmpBufLength);
	W25QXX_Read((u8 *)&(tmpBuf[6]), SpiFlashAddr_printfParam, FlashSIZE_printfParam);
	delay_ms(50);
	tmpBuf[0] = 0x3A;
	tmpBuf[1] = tmpBufLength;
	tmpBuf[2] = Index;
	tmpBuf[3] = Type_queryAck;
	tmpBuf[4] = Fc_param;
	tmpBuf[5] = Extra_param_printf;
	setBCC(tmpBuf, tmpBufLength);
	tmpBuf[tmpBufLength - 1] = 0x23;
	screenUart_sendStr((const char *)tmpBuf, tmpBufLength);
	myfree(SRAMIN,tmpBuf);
	tmpBuf = 0;
}
// 设置-参数-打印参数 
bool set_param_printf(u8* data, u8 dataLength)
{
	if(dataLength != FlashSIZE_printfParam)
		return false;
	W25QXX_Write(data, SpiFlashAddr_printfParam, dataLength);
	delay_ms(50);
	return true;
}

// 控制-调试 
void control_debug(u8 Index, u8 Extra)
{
	switch(Extra)
	{
		case Extra_debug_reset:
//			stepperMotorStop(Motor_XZ);
			stepperMotorStop(Motor_X);
			stepperMotorStop(Motor_Y);
			deviceState.motorReset_runningState = RunningState_readyStart;
			deviceState.motorReset_threadEnable = ON;
			break;
//		case Extra_debug_xz_zero:
//			motorGo_acc(Motor_XZ, 0);
//			break;
//		case Extra_debug_xz_site_1:
//			motorGo_acc(Motor_XZ, mpd.XZ_site_1);
//			break;
//		case Extra_debug_xz_site_2:
//			motorGo_acc(Motor_XZ, mpd.XZ_site_2);
//			break;
//		case Extra_debug_xz_site_3:
//			motorGo_acc(Motor_XZ, mpd.XZ_site_3);
//			break;
//		case Extra_debug_xz_site_4:
//			motorGo_acc(Motor_XZ, mpd.XZ_site_4);
//			break;
//		case Extra_debug_xz_site_5:
//			motorGo_acc(Motor_XZ, mpd.XZ_site_5);
//			break;
//		case Extra_debug_xz_site_6:
//			motorGo_acc(Motor_XZ, mpd.XZ_site_6);
//			break;
		case Extra_debug_x_home:
			motorGo_acc(Motor_X, 0);
//			if(XH != XH_Blocking)
//			{motorGo_acc(Motor_X, 0);}
			break;
		case Extra_debug_x_end:
			motorGo_acc(Motor_X, mpd.X_end);
//			if(XE != XE_Blocking && XEG != XEG_ExistBox)
//			{motorGo_acc(Motor_X, mpd.X_end);}
			break;
		case Extra_debug_y_home:
			motorGo_acc(Motor_Y, 0);
//			if(YH != YH_Blocking)
//			{motorGo_acc(Motor_Y, 0);}
			break;
		case Extra_debug_y_end:
			motorGo_acc(Motor_Y, mpd.Y_end);
//			if(YE != YE_Blocking && (XH == XH_Blocking || XE == XE_Blocking))
//			{motorGo_acc(Motor_Y, mpd.Y_end);}
			break;
	}
}


// 查询-参数-打印参数 
void query_param_bak(u8 Index)
{
	u8 * tmpBuf = 0;
	u8 tmpBufLength = 8 + FlashSIZE_bakParam;
	tmpBuf = mymalloc(SRAMIN, tmpBufLength);
	memset(tmpBuf, 0, tmpBufLength);
	W25QXX_Read((u8 *)&(tmpBuf[6]), SpiFlashAddr_bakParam, FlashSIZE_bakParam);
	delay_ms(50);
	tmpBuf[0] = 0x3A;
	tmpBuf[1] = tmpBufLength;
	tmpBuf[2] = Index;
	tmpBuf[3] = Type_queryAck;
	tmpBuf[4] = Fc_param;
	tmpBuf[5] = Extra_param_bak;
	setBCC(tmpBuf, tmpBufLength);
	tmpBuf[tmpBufLength - 1] = 0x23;
	screenUart_sendStr((const char *)tmpBuf, tmpBufLength);
	myfree(SRAMIN,tmpBuf);
	tmpBuf = 0;
}
// 设置-参数-打印参数 
bool set_param_bak(u8* data, u8 dataLength)
{
	if(dataLength != FlashSIZE_bakParam)
		return false;
	W25QXX_Write(data, SpiFlashAddr_bakParam, dataLength);
	delay_ms(50);
	return true;
}







