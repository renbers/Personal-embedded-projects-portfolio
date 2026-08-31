#include "ht32.h"
#include "Key.h"

#define LOOP_CYCLES_PER_MS 48000								//48MHz对应1ms的周期数

volatile uint8_t Key_Num = 0;	

void Key_Init(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};	     	//创建结构体并将所有位初始化为0
	CKCUClock.Bit.PA    = 1;									//使能GPIOA的时钟
	CKCUClock.Bit.AFIO  = 1;									//使能AFIO的时钟
	CKCU_PeripClockConfig(CKCUClock, ENABLE);					//开启时钟
	
	//按键1
	GPIO_InputConfig(HT_GPIOA,GPIO_PIN_6, ENABLE);				//设置PA6引脚为输入模式
	GPIO_PullResistorConfig(HT_GPIOA,GPIO_PIN_6, GPIO_PR_UP);	//设置PA6引脚为上拉输入模式

	//按键2
	GPIO_InputConfig(HT_GPIOA,GPIO_PIN_3, ENABLE);				//设置PA3引脚为输入模式
	GPIO_PullResistorConfig(HT_GPIOA,GPIO_PIN_3, GPIO_PR_UP);
}

//获取按键
uint8_t Key_GetNum(void)										
{										
	uint8_t Temp;
	if (Key_Num)
	{
		Temp = Key_Num;
		Key_Num = 0;
		return Temp;
	}
	return 0;										//返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
}



uint8_t Key_GetState(void)							//获取按键现阶段状态（引脚电平）
{
	if (GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_6) == 0)
	{
		return 1;
	}
	
	if (GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_3) == 0)
	{
		return 2;
	}
	
	return 0;
}

void Key_Tick(void)									//每隔20ms获取按键状态并更新数据
{
	static uint8_t Count;
	static uint8_t CurrState, PrevState;
	
	Count ++;
	
	if (Count >= 20)
	{
		Count = 0;
		
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if (CurrState == 0 && PrevState != 0)		//当现在为0，刚刚为1时执行，即按键松开
		{
			Key_Num = PrevState;
		}
	}
}
