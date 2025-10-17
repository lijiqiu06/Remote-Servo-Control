#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_PowerWarningOn(void);
void LED_PowerWarningOff(void);
void LED_Blink(void);
#endif 
