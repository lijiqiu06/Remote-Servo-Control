#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "NRF24L01.h"
#include "Timer.h"
#include "Power.h"
#include "LED.h"

uint8_t SendFlag;						   // 发送标志位
uint8_t SendSuccessCount, SendFailedCount; // 发送成功计次，发送失败计次

uint8_t ReceiveFlag;							 // 接收标志位
uint8_t ReceiveSuccessCount, ReceiveFailedCount; // 接收成功计次，接收失败计次

int main()
{
	/*初始化*/
	OLED_Init();
	Key_Init();
	NRF24L01_Init();
	Timer_Init();
	LED_Init();
	Power_Init();

	/*初始化测试数据，此处值为任意设定，便于观察实验现象*/
	NRF24L01_TxPacket[0] = 0x00;

	while (1)
	{

		float voltage;
		Power_ReadVoltage(&voltage);
		OLED_ShowString(0, 0, "V Value:", OLED_8X16);
		OLED_Printf(88, 0, OLED_8X16, "%.2f", voltage);
		OLED_Update();
		Power_ShowVoltage();
		if (Key_Check(1, KEY_CLICK)) // K1按下
		{

			NRF24L01_TxPacket[0] = 0x01;

			/*调用NRF24L01_Send函数，发送数据，同时返回发送标志位，方便用户了解发送状态*/
			/*发送标志位与发送状态的对应关系，可以转到此函数定义上方查看*/
			SendFlag = NRF24L01_Send();

			if (SendFlag == 1) // 发送标志位为1，表示发送成功
			{
				SendSuccessCount++; // 发送成功计次变量自增
			}
			else // 发送标志位不为1，即2/3/4，表示发送不成功
			{
				SendFailedCount++; // 发送失败计次变量自增
			}
			LED_Blink();
			OLED_Printf(0, 16, OLED_8X16, "%d", NRF24L01_TxPacket[0]);
			OLED_Update();
		}

		if (Key_Check(2, KEY_CLICK)) // K1按下
		{
			/*变换测试数据，便于观察实验现象*/
			/*实际项目中，可以将待发送的数据赋值给NRF24L01_TxPacket数组*/
			NRF24L01_TxPacket[0] = 0x02;

			/*调用NRF24L01_Send函数，发送数据，同时返回发送标志位，方便用户了解发送状态*/
			/*发送标志位与发送状态的对应关系，可以转到此函数定义上方查看*/
			SendFlag = NRF24L01_Send();

			if (SendFlag == 1) // 发送标志位为1，表示发送成功
			{
				SendSuccessCount++; // 发送成功计次变量自增
			}
			else // 发送标志位不为1，即2/3/4，表示发送不成功
			{
				SendFailedCount++; // 发送失败计次变量自增
			}
			LED_Blink();
			OLED_Printf(0, 16, OLED_8X16, "%d", NRF24L01_TxPacket[0]);
			OLED_Update();
		}

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
		}
		Delay_ms(100);
	}
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Key_Tick();

		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
