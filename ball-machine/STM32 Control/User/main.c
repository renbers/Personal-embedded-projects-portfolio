#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "RP.h"
#include "Motor.h"
#include "Encoder.h"
#include "Push.h"
#include "uart.h"
#include "Intrp.h"

uint8_t KeyGet = 0, RPFlag = 0;
volatile uint8_t KeyPush = 0;

volatile float Target, Actual, Out;
volatile float Kp = 0.25f, Ki = 0.002f, Kd = 0.35f;
volatile float Error0, Error1;
static float ErrorIntegral;

#define PID_OUTPUT_LIMIT    85.0f
#define PID_INTEGRAL_LIMIT  12000.0f

static float Position_PID_Update(float target, float actual)
{
	float output;
	float candidate_integral;
	float candidate_output;
	
	Error1 = Error0;
	Error0 = target - actual;
	candidate_integral = ErrorIntegral + Error0;

	if (candidate_integral > PID_INTEGRAL_LIMIT) {candidate_integral = PID_INTEGRAL_LIMIT;}
	if (candidate_integral < -PID_INTEGRAL_LIMIT) {candidate_integral = -PID_INTEGRAL_LIMIT;}

	candidate_output = Kp * Error0 + Ki * candidate_integral + Kd * (Error0 - Error1);

	/* Saturated output only accepts integral action that helps leave saturation. */
	if ((candidate_output <= PID_OUTPUT_LIMIT && candidate_output >= -PID_OUTPUT_LIMIT) ||
		(candidate_output > PID_OUTPUT_LIMIT && Error0 < 0) ||
		(candidate_output < -PID_OUTPUT_LIMIT && Error0 > 0))
	{
		ErrorIntegral = candidate_integral;
	}

	output = Kp * Error0 + Ki * ErrorIntegral + Kd * (Error0 - Error1);
	
	if (output > PID_OUTPUT_LIMIT) {output = PID_OUTPUT_LIMIT;}
	if (output < -PID_OUTPUT_LIMIT) {output = -PID_OUTPUT_LIMIT;}
	if (output < 65 && output > 0) {output = 0;}
	if (output > -65 && output < 0) {output = 0;}
	
	return output;
}
int main(void)
{
	OLED_Init();								//PB8_SCL      PB9_SDA
	Key_Init();									//PB10
	Motor_Init();								//PB12  PB13   控制方向		PA0_TIM2_PWM	TIM2_CH1
	Motor2_Init();								//PA11  PA12   控制方向		PB6_TIM4_PWM	TIM1_CH1
	Encoder_Init();								//PA6   PA7    编码器输入					TIM3_CH1/CH2					
	Push_Init();								//PA4	PA5	   推杆上下方向
	RP_Init();									//PA2          电位器旋钮1
	GPIO_Interrupt_Init();						//PB0   PB1   PB3   PB4   PB5
	UART_Init();								//PA9_RX       PA10_TX   
	
	Timer_Init();
	
	OLED_Update();
	
	
	while (1)
	{
		KeyGet = Key_GetNum();
		
		if(KeyGet == 1){
			RPFlag = !RPFlag;}
		
		switch (KeyPush){
			case 1:Push_SetSpeed(1);break;
			case 2:Push_SetSpeed(2);break;
			case 3:Push_SetSpeed(3);break;
			case 4:KeyPush = 0;Push_SetSpeed(2);break;
		}
		
				
		if(RPFlag == 0){
		Target = RP_GetValue(1) / 4095.0 * 10000 - 5000;}

		
		OLED_Printf(0,16,OLED_8X16,"%f",Target);
		OLED_Printf(58,48,OLED_8X16,"Out: %d",Out);
		OLED_Printf(0,32,OLED_8X16,"A: %f",Actual);
		OLED_Printf(0,48,OLED_8X16,"high: %d",KeyPush);
		OLED_Printf(0,0,OLED_8X16,"%d",GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4));
		OLED_Printf(12,0,OLED_8X16,"%d",GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_5));
		
		OLED_Printf(24,0,OLED_8X16,"%d",GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4));
		OLED_Printf(36,0,OLED_8X16,"%d",GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5));
		OLED_Printf(48,0,OLED_8X16,"%d",EXTI_GetITStatus(EXTI_Line5));
		
		OLED_Update();	
		
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Key_Tick();
		
		Count ++;
		if (Count >= 40)
		{
			Count = 0;
			
			Actual += Encoder_Get();
			
			Out = Position_PID_Update(Target, Actual);
			
			Motor_SetPWM(Out);
			Motor2_SetPWM(Out);
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}



void USART1_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE) == SET)// 等待数据接收
	{
		USART_ReceiveData(USART1);
	}
}

// PB0中断	UP
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET){
        KeyPush = 1;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
 
// PB1中断	LEFT
void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET){
        
		Target = Blue_RP(4);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}
 
// PB3中断	OK
void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) != RESET){
		Target = Blue_RP(1);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}
 
// PB4中断	RIGHT
void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET){
        
		Target = Blue_RP(5);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}
 
// PB5中断（共享中断）	DOWM
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) != RESET){
        KeyPush = 3;
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}

// PB11中断	按键3
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line11) != RESET){
        KeyPush = 2;
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
}
