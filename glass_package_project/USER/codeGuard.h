#ifndef __CODEGUARD_H
#define __CODEGUARD_H	 
#include "sys.h"

void writeCiphertext(u8 * ciphertext);

void readCiphertext(u8 * ciphertext);

// 打开Flash读保护（会不能再少写程序）
void open_FlashReadProtect(void);

// 关闭Flash读保护（若打开了读保护再调用，会自毁程序）
void close_FlashReadProtect(void);

// 检查SPI Flash中的密文对不对得上(若为真则为盗版)
u8 verifyCiphertextExistError(void);
			    
void setSerialNumber(u32 serialNumber);
#endif
