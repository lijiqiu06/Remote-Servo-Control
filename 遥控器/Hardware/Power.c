#include "Power.h"
#include "AD.h"
#include "LED.h"
#define AVG_WINDOW_SIZE 20
static uint16_t ad_buf[AVG_WINDOW_SIZE] = {0}; // 存储历史AD值
static uint8_t buf_index = 0;                  // 缓冲区索引

void Power_Init(void)
{
    AD_Init();
    LED_Init();
	LED_On();
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



void Power_ShowVoltage(void)
{
    float voltage;
    uint8_t voltage_level;
    
    Power_ReadVoltage(&voltage);
    if (voltage < 3.8f)
    {
        LED_PowerWarningOn();
        
    }
    else
    {
        LED_PowerWarningOff();
        
    }
    
}