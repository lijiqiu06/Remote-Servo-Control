#include "Power.h"
#include "AD.h"

#define AVG_WINDOW_SIZE 20
static uint16_t ad_buf[AVG_WINDOW_SIZE] = {0}; // 存储历史AD值
static uint8_t buf_index = 0;                  // 缓冲区索引

void Power_Init(void)
{
    AD_Init();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
}

void Power_ReadVoltage(float* voltage)
{
    uint16_t ad_sum = 0;
    uint8_t i;

    // 读取新的AD值，存入缓冲区（覆盖 oldest 数据）
    ad_buf[buf_index] = AD_getValue();
    buf_index = (buf_index + 1) % AVG_WINDOW_SIZE;

    // 计算缓冲区所有值的平均值
    for (i = 0; i < AVG_WINDOW_SIZE; i++)
    {
        ad_sum += ad_buf[i];
    }
    uint16_t ad_avg = ad_sum / AVG_WINDOW_SIZE;
    // 根据传感器的特性进行换算

    *voltage = (ad_avg / 4095.0f) * 3.3f * (2.0f); // 分压比为2:1
    
}

uint8_t Power_GetVoltageLevel(float voltage)
{
    if (voltage < 3.825f)
    {
        return Low_Power;
    }
    else if (voltage < 4.0f)
    {
        return Medium_Power;
    }
    else if (voltage < 4.125f)
    {
        return High_Power;
    }
    else
    {
        return Full_Power;
    }
}

void Power_ShowVoltage(void)
{
    float voltage;
    uint8_t voltage_level;
    
    Power_ReadVoltage(&voltage);
    voltage_level = Power_GetVoltageLevel(voltage);
    switch (voltage_level)
    {
    case Low_Power:
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);
        GPIO_ResetBits(GPIOB, GPIO_Pin_10);
        GPIO_SetBits(GPIOA, GPIO_Pin_7); 
        break;
    case Medium_Power:
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        GPIO_ResetBits(GPIOB, GPIO_Pin_10);
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
        break;
    case High_Power:
        GPIO_ResetBits(GPIOB, GPIO_Pin_10);
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
        break;
    case Full_Power:
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
        GPIO_SetBits(GPIOB, GPIO_Pin_10);
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
        break;
    default:
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);
        GPIO_ResetBits(GPIOB, GPIO_Pin_10);
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);
        break;
    }
}