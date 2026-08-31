#include "ht32.h"

void Motor2_PWM_Init(void)							//PC1单通道输出PWM波形
{
  TM_TimeBaseInitTypeDef TM_TimeBaseInitStruct;
  TM_OutputInitTypeDef TM_OutputInitStruct;
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.MCTM0      = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  AFIO_GPxConfig(GPIO_PC, AFIO_PIN_1, AFIO_FUN_MCTM_GPTM);  

  TM_TimeBaseInitStruct.CounterReload = 100-1;
  TM_TimeBaseInitStruct.Prescaler = 120-1;
  TM_TimeBaseInitStruct.RepetitionCounter = 0;
  TM_TimeBaseInitStruct.CounterMode = TM_CNT_MODE_UP;
  TM_TimeBaseInitStruct.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
  TM_TimeBaseInit(HT_MCTM0, &TM_TimeBaseInitStruct);

  TM_OutputInitStruct.Channel = TM_CH_0;
  TM_OutputInitStruct.OutputMode = TM_OM_PWM1;
  TM_OutputInitStruct.Control = TM_CHCTL_ENABLE;
  TM_OutputInitStruct.ControlN = TM_CHCTL_ENABLE;
  TM_OutputInitStruct.Polarity = TM_CHP_NONINVERTED;
  TM_OutputInitStruct.PolarityN = TM_CHP_NONINVERTED;
  TM_OutputInitStruct.IdleState = MCTM_OIS_LOW;
  TM_OutputInitStruct.IdleStateN = MCTM_OIS_HIGH;
  TM_OutputInitStruct.Compare = 0;
  TM_OutputInit(HT_MCTM0, &TM_OutputInitStruct);

  MCTM_CHMOECmd(HT_MCTM0, ENABLE);
  TM_Cmd(HT_MCTM0, ENABLE);
}


void Motor2_SetPWM(int8_t PWM)
{
	if (PWM >= 0)
	{
		GPIO_SetOutBits(HT_GPIOA,GPIO_PIN_14);
		GPIO_ClearOutBits(HT_GPIOA,GPIO_PIN_15);
		TM_SetCaptureCompare0(HT_MCTM0,PWM);
	}
	else
	{
		GPIO_SetOutBits(HT_GPIOA,GPIO_PIN_15);
		GPIO_ClearOutBits(HT_GPIOA,GPIO_PIN_14);
		TM_SetCaptureCompare0(HT_MCTM0,-PWM);
	}
}
