#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "OLED.h"
void Servo_Init(void)
{
	PWM_Init();
	
}

void Servo_SetAngle(float angle)
{
	uint16_t ret=angle/180.0*2000+500;
	PWM_SetCompare1(ret);
	
	
}

