#ifndef __POWER_H
#define __POWER_H

#include "stm32f10x.h"

enum
{
    Low_Power = 0,
    Medium_Power,
    High_Power,
    Full_Power 
};
void Power_Init(void);
void Power_ReadVoltage(float *voltage);
void Power_ShowVoltage(void);
#endif
