#include "Globalconfig.h"


// ��Flash���������᲻������д����
void open_FlashReadProtect(void) 
{
	if(FLASH_GetReadOutProtectionStatus() != SET)
	{ 
		FLASH_Unlock();		// ����  
		FLASH_ReadOutProtection(ENABLE);
		FLASH_Lock();		// ���� 
	} 
}

// �ر�Flash�������������˶������ٵ��ã����Իٳ���
void close_FlashReadProtect(void) 
{ 
	if(FLASH_GetReadOutProtectionStatus() != RESET)
	{
		FLASH_Unlock(); 	// ������FALSHҲ�����ö����� 
		FLASH_ReadOutProtection(DISABLE);
		FLASH_Lock();		// ���� 
	} 
}

// ���SPI Flash�е����ĶԲ��Ե���(��Ϊ����Ϊ����)
u8 verifyCiphertextExistError(void)
{
	// ��Ϊ�����ó����ڷ�����ֱ���ҵ������ַ�����������ַ�����������
	// ��ʵ��ַ = ��SerialNumberRomAddr - Developer_QQ + Program_Versions�� * 40��ÿλ��ַ+4��3��u32
	volatile u32 chipIdAddr = 13421721 + Developer_QQ - Program_Versions;//409798596
	u8 verifyReturn = 0;
	u8 i = 0;
	u32 *chipId;
	static u8 ciphertext_chipEncrypt[4] = {0,0,0,0};
	static u8 ciphertext_inRom[4] = {0,0,0,0};
	// δ���оƬID���������
	if(ciphertext_chipEncrypt[0] == 0 && ciphertext_chipEncrypt[1] == 0
		&& ciphertext_chipEncrypt[2] == 0 && ciphertext_chipEncrypt[3] == 0)
	{
		chipId = mymalloc(SRAMIN,12);		// ���봢��оƬID�ĵ�ַ
		memset(chipId,0,12);				// ��մ���оƬID���ڴ�
		chipIdAddr = (chipIdAddr - Developer_QQ + Program_Versions) * 40;	// ת��Ϊ�����ʵоƬID��ַ
		chipId[0] = *(vu32*)(chipIdAddr);	// ��ȡоƬ���кŵ��ֽ�
		chipIdAddr += 4;
		chipId[1] = *(vu32*)(chipIdAddr);
		chipIdAddr += 4;
		chipId[2] = *(vu32*)(chipIdAddr);	// ��ȡоƬ���кŸ��ֽ�
		// ���ļ����㷨,��ü��ܺ����ֵ
		chipIdAddr = (((chipId[0] ^ chipId[1] ^ chipId[2])&0xffffff \
			^ Developer_QQ) / Program_Versions ) & 0xffffffff;
		for(i = 0; i < 4; i++)				// ת��Ϊ��˴���ģʽ���������
			ciphertext_chipEncrypt[i] = ((u8 *)(&chipIdAddr))[3 - i];
		myfree(SRAMIN,chipId);
	}
	// δ���Rom�е�����
	if(ciphertext_inRom[0] == 0 && ciphertext_inRom[1] == 0
		&& ciphertext_inRom[2] == 0 && ciphertext_inRom[3] == 0)
		readCiphertext(ciphertext_inRom);	// ��ȡ������Flash Rom�е�����
	
	// �������������ͬ��У��У��ͨ�������0
	verifyReturn = (ciphertext_chipEncrypt[0] - ciphertext_inRom[0]) + \
					(ciphertext_chipEncrypt[1] - ciphertext_inRom[1]) + \
					(ciphertext_chipEncrypt[2] - ciphertext_inRom[2]) + \
					(ciphertext_chipEncrypt[3] - ciphertext_inRom[3]) ;
	return verifyReturn;
}

// 
/**---------------------------------------------------------------------------------
  * @brief  ������д���ڲ�Flash ROM����ַRomFlashAddr_codeGuard������4λ
  * @param  ����������ʼ��ַ
  --------------------------------------------------------------------------------*/
void writeCiphertext(u8* ciphertext)
{	
	/* ���ڲ�RC���� */
	RCC_HSICmd(ENABLE);	
	/*��FLASH�ɲ���������*/
	FLASH_Unlock();
	/*��flash������־λȫ��*/
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	// ������ҳ
	FLASH_ErasePage(RomFlashAddr_codeGuard);
	/*flash Ϊһ���ֽڴ洢��16λ���ݱ����ַ��2*/
	FLASH_ProgramHalfWord(RomFlashAddr_codeGuard,*((u16 *)(ciphertext))); 
	FLASH_ProgramHalfWord((RomFlashAddr_codeGuard + 2),*((u16 *)(ciphertext + 2))); 
	/* �������� */
	FLASH_Lock(); 
	/* �ر��ڲ�RC���� */
	RCC_HSICmd(DISABLE);
}

/**---------------------------------------------------------------------------------
  * @brief  ���ڲ�Flash ROM�������ģ���ַRomFlashAddr_codeGuard������4λ
  * @param  memory_data�������ŵ�ַ��n �������
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
