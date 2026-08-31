#include "ht32.h"


void Motor_PWM_Init(void)									//PB4单通道输出PWM波形
{
	TM_TimeBaseInitTypeDef TM_TimeBaseInitStruct;
	TM_OutputInitTypeDef TM_OutputInitStruct;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	CKCUClock.Bit.SCTM0      = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);

	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_4, AFIO_FUN_SCTM);  //Config AFIO mode、
  
	TM_TimeBaseInitStruct.CounterReload = 100-1;
	TM_TimeBaseInitStruct.Prescaler = 120-1;
	TM_TimeBaseInitStruct.RepetitionCounter = 0;
	TM_TimeBaseInitStruct.CounterMode = TM_CNT_MODE_UP;
	TM_TimeBaseInitStruct.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
	TM_TimeBaseInit(HT_SCTM0, &TM_TimeBaseInitStruct);

	TM_OutputInitStruct.Channel = TM_CH_0;
	TM_OutputInitStruct.OutputMode = TM_OM_PWM1;
	TM_OutputInitStruct.Control = TM_CHCTL_ENABLE;
	TM_OutputInitStruct.ControlN = TM_CHCTL_DISABLE;//TM_CHCTL_ENABLE;
	TM_OutputInitStruct.Polarity = TM_CHP_NONINVERTED;
	TM_OutputInitStruct.PolarityN = TM_CHP_NONINVERTED;
	TM_OutputInitStruct.IdleState = MCTM_OIS_LOW;
	TM_OutputInitStruct.IdleStateN = MCTM_OIS_HIGH;
	TM_OutputInitStruct.Compare = 0;
	TM_OutputInit(HT_SCTM0, &TM_OutputInitStruct);

	TM_Cmd(HT_SCTM0, ENABLE);
}


void Motor_SetPWM(int8_t PWM)
{
	if (PWM >= 0)
	{
		GPIO_SetOutBits(HT_GPIOD,GPIO_PIN_1);
		GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_2);
		TM_SetCaptureCompare0(HT_SCTM0,PWM);
	}
	else
	{
		GPIO_SetOutBits(HT_GPIOD,GPIO_PIN_2);
		GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_1);
		TM_SetCaptureCompare0(HT_SCTM0,-PWM);
	}
}
