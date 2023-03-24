#ifndef __GAS_H
#define __GAS_H
#include "sys.h"
#include "PCF8591.h"
#include "p6847.h"
#include "ITV0011.h"
#include "Globalconfig.h"

void Gas_Init(void);
void Gas_maintain_Func(void);
void Gas_release_Func(void);


#endif