#include "1msTimer.h"

void BFTM0_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};					//开启BFTM0外设时钟并使能
  CKCUClock.Bit.BFTM0 = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  NVIC_EnableIRQ(BFTM0_IRQn);   										//BFTM0定时器的NVIC中断使能
  BFTM_IntConfig(HT_BFTM0, ENABLE);

  BFTM_SetCompare(HT_BFTM0, 48000 - 1);									//设置比较值（此处即自动重载值）
  BFTM_SetCounter(HT_BFTM0, 0);											//初始化计数器为0
  BFTM_EnaCmd(HT_BFTM0, ENABLE);
}
