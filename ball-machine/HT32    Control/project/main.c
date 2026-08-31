#include "ht32.h"
#include "GPIO.h"
#include "Key.h"
#include "1msTimer.h"
#include "Motor1.h"
#include "USART0.h"
#include "Servo_GPTM1.h"
#include "Motor2.h"
#include "Protocol.h"

uint8_t Out;
int8_t KeyGet;
volatile int8_t KeyMotor = 0, KeyServo = 0;
volatile u8 ThrowLeftSpeed = 0, ThrowRightSpeed = 0;

int main(void)
{
	
	GPIO_Configuration();
	Motor2_PWM_Init();					//电机2	PC1——MCTM0	PA14 PA15
	Motor_PWM_Init();						//电机1	PB4——SCTM0	PD1  PD2
	Servo_PWM_Init();						//舵机PB3——SCTM1-2
	BFTM0_Configuration();
	Key_Init();									//PA6按键1   PA3按键2
	USART0_Configuration();			//PB0_RX   	 PB1_TX(串口与蓝牙交叉接入，此为接入）
	
	while (1)
	{
		Protocol_Process();
		
		KeyGet = Key_GetNum();
		
		if (KeyGet == 1)
		{
			if (KeyMotor >= 3)
			{
				KeyMotor = 0;
			}
			else
			{
				KeyMotor ++;
			}
		}

		switch (KeyMotor)
		{
			case 0:
				ThrowLeftSpeed = 0;
				ThrowRightSpeed = 0;
				break;
			case 1:
				ThrowLeftSpeed = 70;
				ThrowRightSpeed = 70;
				break;
			case 2:
				ThrowLeftSpeed = 85;
				ThrowRightSpeed = 85;
				break;
			case 3:
				ThrowLeftSpeed = 100;
				ThrowRightSpeed = 100;
				break;
			case 4:
				break;
			default:
				KeyMotor = 0;
				ThrowLeftSpeed = 0;
				ThrowRightSpeed = 0;
				break;
		}
			
		if (KeyGet == 2)
		{
			KeyServo ++;
		}
		if(KeyServo > 3){KeyServo = 0;}
			
		Out = ThrowLeftSpeed;
		Motor_SetPWM((int8_t)ThrowLeftSpeed);
		Motor2_SetPWM((int8_t)ThrowRightSpeed);
		
	}
}


void BFTM0_IRQHandler(void)
{
	static uint16_t Count;
	
	if (BFTM_GetFlagStatus(HT_BFTM0) == SET)
	{
		BFTM_ClearFlag(HT_BFTM0);
		Key_Tick();
		Servo_Mode_Choose(KeyServo);
	}
	
	Count ++;
	if(Count == 1000){
		GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_3);
		GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_10);
		GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_11);
		GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_12);
		GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_14);
		GPIO_ClearOutBits(HT_GPIOC,GPIO_PIN_15);
		Count = 0;
	}
}


