#include "GPIO.h"

//-----------------------------------------------------------------------------
void GPIO_Configuration(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	CKCUClock.Bit.PA    = 1;
	CKCUClock.Bit.PB    = 1;
	CKCUClock.Bit.PC    = 1;
	CKCUClock.Bit.PD    = 1;
	CKCUClock.Bit.AFIO  = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	GPIO_DirectionConfig(HT_GPIOD,GPIO_PIN_1,GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOD,GPIO_PIN_2,GPIO_DIR_OUT);
	
	GPIO_DirectionConfig(HT_GPIOA,GPIO_PIN_14,GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOA,GPIO_PIN_15,GPIO_DIR_OUT);
	
	GPIO_DirectionConfig(HT_GPIOD,GPIO_PIN_3,GPIO_DIR_OUT);GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_3);
	GPIO_DirectionConfig(HT_GPIOC,GPIO_PIN_10,GPIO_DIR_OUT);GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_10);
	GPIO_DirectionConfig(HT_GPIOC,GPIO_PIN_11,GPIO_DIR_OUT);GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_11);
	GPIO_DirectionConfig(HT_GPIOC,GPIO_PIN_12,GPIO_DIR_OUT);GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_12);
	GPIO_DirectionConfig(HT_GPIOC,GPIO_PIN_14,GPIO_DIR_OUT);GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_14);
	GPIO_DirectionConfig(HT_GPIOC,GPIO_PIN_15,GPIO_DIR_OUT);GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_15);
}

