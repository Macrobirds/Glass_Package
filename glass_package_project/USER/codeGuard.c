#include "Globalconfig.h"


// 打开Flash读保护（会不能再少写程序）
void open_FlashReadProtect(void) 
{
	if(FLASH_GetReadOutProtectionStatus() != SET)
	{ 
		FLASH_Unlock();		// 解锁  
		FLASH_ReadOutProtection(ENABLE);
		FLASH_Lock();		// 上锁 
	} 
}

// 关闭Flash读保护（若打开了读保护再调用，会自毁程序）
void close_FlashReadProtect(void) 
{ 
	if(FLASH_GetReadOutProtectionStatus() != RESET)
	{
		FLASH_Unlock(); 	// 不解锁FALSH也可设置读保护 
		FLASH_ReadOutProtection(DISABLE);
		FLASH_Lock();		// 上锁 
	} 
}

// 检查SPI Flash中的密文对不对得上(若为真则为盗版)
u8 verifyCiphertextExistError(void)
{
	// 因为不想让程序在反汇编后直接找到这个地址，所以这个地址是运算出来的
	// 真实地址 = （SerialNumberRomAddr - Developer_QQ + Program_Versions） * 40；每位地址+4共3个u32
	volatile u32 chipIdAddr = 13421721 + Developer_QQ - Program_Versions;//409798596
	u8 verifyReturn = 0;
	u8 i = 0;
	u32 *chipId;
	static u8 ciphertext_chipEncrypt[4] = {0,0,0,0};
	static u8 ciphertext_inRom[4] = {0,0,0,0};
	// 未获得芯片ID翻译的密文
	if(ciphertext_chipEncrypt[0] == 0 && ciphertext_chipEncrypt[1] == 0
		&& ciphertext_chipEncrypt[2] == 0 && ciphertext_chipEncrypt[3] == 0)
	{
		chipId = mymalloc(SRAMIN,12);		// 申请储存芯片ID的地址
		memset(chipId,0,12);				// 清空储存芯片ID的内存
		chipIdAddr = (chipIdAddr - Developer_QQ + Program_Versions) * 40;	// 转换为获得真实芯片ID地址
		chipId[0] = *(vu32*)(chipIdAddr);	// 获取芯片序列号低字节
		chipIdAddr += 4;
		chipId[1] = *(vu32*)(chipIdAddr);
		chipIdAddr += 4;
		chipId[2] = *(vu32*)(chipIdAddr);	// 获取芯片序列号高字节
		// 密文加密算法,获得加密后的数值
		chipIdAddr = (((chipId[0] ^ chipId[1] ^ chipId[2])&0xffffff \
			^ Developer_QQ) / Program_Versions ) & 0xffffffff;
		for(i = 0; i < 4; i++)				// 转换为大端储存模式，获得密文
			ciphertext_chipEncrypt[i] = ((u8 *)(&chipIdAddr))[3 - i];
		myfree(SRAMIN,chipId);
	}
	// 未获得Rom中的密文
	if(ciphertext_inRom[0] == 0 && ciphertext_inRom[1] == 0
		&& ciphertext_inRom[2] == 0 && ciphertext_inRom[3] == 0)
		readCiphertext(ciphertext_inRom);	// 读取储存在Flash Rom中的密文
	
	// 如果两段密文相同，校验校验通过，输出0
	verifyReturn = (ciphertext_chipEncrypt[0] - ciphertext_inRom[0]) + \
					(ciphertext_chipEncrypt[1] - ciphertext_inRom[1]) + \
					(ciphertext_chipEncrypt[2] - ciphertext_inRom[2]) + \
					(ciphertext_chipEncrypt[3] - ciphertext_inRom[3]) ;
	return verifyReturn;
}

// 
/**---------------------------------------------------------------------------------
  * @brief  把密文写入内部Flash ROM，地址RomFlashAddr_codeGuard，长度4位
  * @param  输入数据起始地址
  --------------------------------------------------------------------------------*/
void writeCiphertext(u8* ciphertext)
{	
	/* 打开内部RC振荡器 */
	RCC_HSICmd(ENABLE);	
	/*打开FLASH可擦除控制器*/
	FLASH_Unlock();
	/*将flash三个标志位全清*/
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	// 擦除整页
	FLASH_ErasePage(RomFlashAddr_codeGuard);
	/*flash 为一个字节存储，16位数据必须地址加2*/
	FLASH_ProgramHalfWord(RomFlashAddr_codeGuard,*((u16 *)(ciphertext))); 
	FLASH_ProgramHalfWord((RomFlashAddr_codeGuard + 2),*((u16 *)(ciphertext + 2))); 
	/* 重新上锁 */
	FLASH_Lock(); 
	/* 关闭内部RC振荡器 */
	RCC_HSICmd(DISABLE);
}

/**---------------------------------------------------------------------------------
  * @brief  从内部Flash ROM读出密文，地址RomFlashAddr_codeGuard，长度4位
  * @param  memory_data：输出存放地址，n 输出个数
  --------------------------------------------------------------------------------*/
void readCiphertext(u8 * ciphertext)
{	
	u8 i = 0;
	for(i = 0; i < 4; i++)
		ciphertext[i] = *((u8 *)(RomFlashAddr_codeGuard + i));
}


void setSerialNumber(u32 serialNumber)
{
	u8 tmpbuf[4];
	tmpbuf[0] = ((u8 *)(&serialNumber))[3];
	tmpbuf[1] = ((u8 *)(&serialNumber))[2];
	tmpbuf[2] = ((u8 *)(&serialNumber))[1];
	tmpbuf[3] = ((u8 *)(&serialNumber))[0];
	writeCiphertext(tmpbuf);
}
