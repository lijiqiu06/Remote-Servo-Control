#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "NRF24L01.h"
#include "Timer.h"
#include "Light.h"
#include "Power.h"

uint8_t SendFlag;						   // 发送标志位
uint8_t SendSuccessCount, SendFailedCount; // 发送成功计次，发送失败计次

uint8_t ReceiveFlag;							 // 接收标志位
uint8_t ReceiveSuccessCount, ReceiveFailedCount; // 接收成功计次，接收失败计次

int main()
{
	/*初始化*/
	OLED_Init();
	NRF24L01_Init();
	Timer_Init();
	Light_Init();
	Power_Init();

	OLED_ShowString(0, 32, "R:000-000-0", OLED_8X16); // 格式为：R:接收成功计次-接收失败计次-接收标志位
	OLED_Update();

	while (1)
	{

		/*主循环内循环执行NRF24L01_Receive函数，接收数据，同时返回接收标志位，方便用户了解接收状态*/
		/*接收标志位与接收状态的对应关系，可以转到此函数定义上方查看*/
		ReceiveFlag = NRF24L01_Receive();

		if (ReceiveFlag) // 接收标志位不为0，表示收到了一个数据包
		{
			if (ReceiveFlag == 1) // 接收标志位为1，表示接收成功
			{
				ReceiveSuccessCount++; // 接收成功计次变量自增
			}
			else // 接收标志位不为0也不为1，即2/3，表示此次接收产生了错误，错误接收的数据不应该使用
			{
				ReceiveFailedCount++; // 接收失败计次变量自增
			}

			switch (NRF24L01_RxPacket[0])
			{
			case 0x01:
				Light_On();
				break;
			case 0x02:
				Light_Off();
				break;
			default:
				break;
			}

			OLED_Update();
		}

		float voltage;
		Power_ReadVoltage(&voltage);
		OLED_ShowString(0, 0, "V Value:", OLED_8X16);
		OLED_Printf(88, 0, OLED_8X16, "%.2f", voltage);
		Power_ShowVoltage();
		OLED_Printf(0, 16, OLED_8X16, "%d %d %d %d", GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0), GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1), GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10), GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7));
		OLED_Update();

		// Delay_ms(100);
	}
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{

		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
