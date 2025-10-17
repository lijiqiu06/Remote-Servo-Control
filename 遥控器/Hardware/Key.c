#include "stm32f10x.h"                  // Device header
#include "Key.h"

/*全局变量，用于存储按键键码*/
/*数组0位置未使用，1~4位置分别存储按键K1、K2、K3、K4的事件标志位*/
uint8_t Key_Code[5];

/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
		
}

/**
  * 函    数：按键读取引脚电平
  * 参    数：n 指定哪个按键，范围：1~2
  * 返 回 值：按键当前的电平，按键按下为0，未按下为1
  */
uint8_t Key_ReadPin(uint8_t n)
{

	
	if (n == 1)
	{
		return !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
	}
	if (n == 2)
	{
		return !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
	}
	
	return 1;	//正常情况下不会执行到这句，除非n的值非法，此时默认返回1，表示按键未按下
}

/**
  * 函    数：按键检查事件
  * 参    数：n 指定哪个按键，范围：1~4
  * 参    数：Event 指定检查上面事件，范围：KEY_CLICK，KEY_DOUBLE，KEY_LONG
  * 返 回 值：指定按键的指定事件是否发生，发生为1，未发生为0
  */
uint8_t Key_Check(uint8_t n, uint8_t Event)
{
	if (Key_Code[n] & Event)	//使用掩码检查全局变量表示的按键键码
	{
		Key_Code[n] &= ~Event;	//清除此事件对应的标志位
		return 1;				//返回1，表示事件发生
	}
	return 0;		//if不成立，返回0，表示事件未发生
}

/**
  * 函    数：按键清除所有事件标志位
  * 参    数：无
  * 返 回 值：无
  */
void Key_Clear(void)
{
	uint8_t i;
	for (i = 1; i < 5; i ++)
	{
		Key_Code[i] = 0;		//清除按键K1、K2、K3、K4所有的事件标志位
	}
}

/**
  * 函    数：用于驱动按键模块运行的自定义按键定时中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数必须在主程序中每隔1ms自动执行一次
  */
void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t PrevState[5], CurrState[5];
	static uint8_t S[5];
	static uint8_t KeyCount[5];
	uint8_t i;
	
	Count ++;
	if (Count >= 20)	//如果计次20次，则if成立，即if每隔20ms进一次
	{
		Count = 0;
		
		for (i = 1; i < 5; i ++)				//遍历按键K1、K2、K3、K4
		{
			PrevState[i] = CurrState[i];		//获取上次状态
			CurrState[i] = Key_ReadPin(i);		//获取本次状态
			
			/*使用状态机，判断对应的按键事件有没有发生*/
			switch (S[i])
			{
				case 0:		//状态0
					if (PrevState[i] == 1 && CurrState[i] == 0)		//产生下降沿
					{
						S[i] = 1;			//转入状态1
						KeyCount[i] = 0;	//计数计时变量清零
					}
				break;
				case 1:		//状态1
					KeyCount[i] ++;			//开始计数计时
					if (KeyCount[i] >= 50)	//计时50*20=1000ms
					{
						S[i] = 0;			//回到状态0
						Key_Code[i] |= KEY_LONG;		//置长按标志位
					}
					else if (PrevState[i] == 0 && CurrState[i] == 1)		//产生上升沿
					{
						S[i] = 2;			//转入状态2
						KeyCount[i] = 0;	//计数计时变量清零
					}
				break;
				case 2:  // 松开后：等待是否有第二次按下（双击窗口）
					KeyCount[i]++;
					// 若超过300ms（15*20ms）未再次按下，判定为单击
					if (KeyCount[i] >= 15) {
						Key_Code[i] |= KEY_CLICK;  // 置单击标志
						S[i] = 0;  // 回到空闲
					}
					// 若在300ms内检测到第二次按下（下降沿），进入双击判定
					else if (PrevState[i] == 1 && CurrState[i] == 0) {
						S[i] = 3;  // 等待第二次松开
						KeyCount[i] = 0;
					}
					break;
				
			   case 3:  // 等待第二次松开：完成双击
					// 检测到第二次松开（上升沿），判定为双击
					if (PrevState[i] == 0 && CurrState[i] == 1) {
						Key_Code[i] |= KEY_DOUBLE;  // 置双击标志
						S[i] = 0;  // 回到空闲
					}
					break;
			}
		}
	}
}
