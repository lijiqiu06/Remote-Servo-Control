#include "LED.h"
#include "Delay.h"
void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14;				
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
    GPIO_Init(GPIOB, &GPIO_InitStructure);					
    
    LED_PowerWarningOff();  
    												
}

void LED_PowerWarningOn(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_14);							
}

void LED_PowerWarningOff(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);						
}

void LED_On(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);							
}

void LED_Off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);							
}

void LED_Blink(void)
{
    LED_Off();
    Delay_ms(100);
    LED_On();
    
}