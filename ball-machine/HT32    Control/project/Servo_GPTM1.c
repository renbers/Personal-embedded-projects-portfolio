#include "Servo_GPTM1.h"


uint8_t Angle = 70;

void Servo_SetAngle(uint8_t AngleValue)
{
	if (AngleValue > 180)
	{
		AngleValue = 180;
	}
	Angle = AngleValue;
}

void Servo_PWM_Init(void)					//定时器1初始化
{
	TM_TimeBaseInitTypeDef TM_TimeBaseInitStruct;
	TM_OutputInitTypeDef TM_OutputInitStruct;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	CKCUClock.Bit.SCTM1      = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);

    AFIO_GPxConfig(GPIO_PB, AFIO_PIN_3, AFIO_FUN_SCTM);  //Config AFIO mode、
  
	TM_TimeBaseInitStruct.CounterReload = 1000-1;
	TM_TimeBaseInitStruct.Prescaler = 960-1;
	TM_TimeBaseInitStruct.RepetitionCounter = 0;
	TM_TimeBaseInitStruct.CounterMode = TM_CNT_MODE_UP;
	TM_TimeBaseInitStruct.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
	TM_TimeBaseInit(HT_SCTM1, &TM_TimeBaseInitStruct);

	TM_OutputInitStruct.Channel = TM_CH_0;
	TM_OutputInitStruct.OutputMode = TM_OM_PWM1;
	TM_OutputInitStruct.Control = TM_CHCTL_ENABLE;
	TM_OutputInitStruct.ControlN = TM_CHCTL_DISABLE;
	TM_OutputInitStruct.Polarity = TM_CHP_NONINVERTED;
	TM_OutputInitStruct.PolarityN = TM_CHP_NONINVERTED;
	TM_OutputInitStruct.IdleState = MCTM_OIS_LOW;
	TM_OutputInitStruct.IdleStateN = MCTM_OIS_HIGH;
	TM_OutputInitStruct.Compare = 0;
	TM_OutputInit(HT_SCTM1, &TM_OutputInitStruct);

	TM_Cmd(HT_SCTM1, ENABLE);
}

uint16_t Count ;

void Servo_Mode_Choose(uint8_t KeyServo)
{
	TM_SetCaptureCompare0(HT_SCTM1,0/180 * 100 + 25);
	
	if(KeyServo == 0){
		TM_SetCaptureCompare0(HT_SCTM1,0/180 * 100 + 25);
	}
	
	if(KeyServo == 1){
		Count++;
		if(Count <=2000){
			TM_SetCaptureCompare0(HT_SCTM1,0/180 * 100 + 25);}
		
		if(Count >= 2000){
			TM_SetCaptureCompare0(HT_SCTM1,Angle * 100 /180 + 25);}
		
		if(Count >= 2100){
			TM_SetCaptureCompare0(HT_SCTM1,0/180 * 100 + 25);
			Count  = 0;}
	}
	
	if(KeyServo == 2){
		Count++;
		if(Count <=1500){
			TM_SetCaptureCompare0(HT_SCTM1,0/180 * 100 + 25);}
		
		if(Count >= 1500){
			TM_SetCaptureCompare0(HT_SCTM1,Angle * 100 /180 + 25);}
		
		if(Count >= 1600){
			TM_SetCaptureCompare0(HT_SCTM1,0/180 * 100 + 25);
			Count  = 0;}
	}
	
	if(KeyServo == 3){
		Count++;
		if(Count <=1000){
			TM_SetCaptureCompare0(HT_SCTM1,0/180 * 100 + 25);}
		
		if(Count >= 1000){
			TM_SetCaptureCompare0(HT_SCTM1,Angle * 100 /180 + 25);}
		
		if(Count >= 1100){
			TM_SetCaptureCompare0(HT_SCTM1,0/180 * 100 + 25);
			Count  = 0;}
	}
}

