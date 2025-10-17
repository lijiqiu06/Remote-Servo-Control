#include "Light.h"
#include "Servo.h"
#include "Delay.h"

void Light_Init(void)
{
    Servo_Init();
}

void Light_On(void)
{
    Servo_SetAngle(90); 
    Delay_ms(200);
    Servo_SetAngle(0);
    Delay_ms(200);
    Servo_SetAngle(90);
    
}

void Light_Off(void)
{
    Servo_SetAngle(90);
    Delay_ms(200);
    Servo_SetAngle(180);
    Delay_ms(200);
    Servo_SetAngle(90);
    
}
