#include "main.h"

uint8_t Turn_Flag;
uint8_t Turn_Count = 0;
uint16_t Turn_Dealy = 680;

uint32_t Line_Time = 0;

uint8_t Stander = 8;
uint8_t Line_Std = 18;
uint8_t Out_Std = 30;


void track(void)                        	//R2 R1 M L1 L2
{
	if(Line_Time == 0){
		track_turn();
	}

	if(Turn_Flag == 1){
		motor_target_set(0, 20);
		mspm0_delay_ms(Turn_Dealy);
		Turn_Flag = 0;
		motor_target_set(5,5);
		mspm0_delay_ms(50);

		Line_Time = system_time_10ms;
	}
	
	if(system_time_10ms - Line_Time > 300){
		track_turn();
	}

	if(system_time_10ms - Line_Time <= 300){
		track_line();
	}
}

void track_turn (void){
	if(L3 == 0 && L2 == 0 && L1 == 0){			//1 1 0 0 0 0 0
	Turn_Flag = 1;
	Turn_Count ++;
	if( Turn_Count == circle * 4 + 1){
		motor_target_set(0, 0);
		DL_TimerG_setCaptureCompareValue(Motor_Pid_INST, 0, DL_TIMER_CC_0_INDEX);
		DL_TimerG_setCaptureCompareValue(Motor_Pid_INST, 0, DL_TIMER_CC_1_INDEX);
		mspm0_delay_ms(3600000);
	}
	motor_target_set(0, 0);
	mspm0_delay_ms(50);
	}

	else if(R3 == 0 && R2 == 1)		//1 0 0 0 0 0 0
	{
		motor_target_set(Stander,Stander -1);
	}
	else if(L3 == 0 && L1 == 1)   	//0 0 0 0 0 x 1
	{
		motor_target_set(Stander -1,Stander);
	}

	else if(R2 == 0 && M  == 1)		//0 1 x 0 0 0 0
	{	
		motor_target_set(Stander,Stander -1);
	}
	else if(L2 == 0 && M  == 1)   	//0 0 0 0 x 1 0
	{
		motor_target_set(Stander -1,Stander);
	}

	else if(R1 == 0 && L1 == 1)		//0 0 1 x 0 0 0
	{
		motor_target_set(Stander,Stander -1); 
	}
	else if(L1 == 0 && R1 == 1)		//0 0 0 x 1 0 0
	{
		motor_target_set(Stander -1,Stander); 
	}
	else if(M  == 0 && L2 == 1 && R2 == 1)//0 0 x 1 x 0 0
	{
		motor_target_set(Stander,Stander); 
	}
}

void track_line (void){
	if(R3 == 0 && R2 == 1)		//1 0 0 0 0 0 0
	{
		motor_target_set(Line_Std,Line_Std -2);
	}
	else if(L3 == 0 && L1 == 1)   	//0 0 0 0 0 x 1
	{
		motor_target_set(Line_Std -2,Line_Std);
	}

	else if(R2 == 0 && M  == 1)		//0 1 x 0 0 0 0
	{	
		motor_target_set(Line_Std,Line_Std -2);
	}
	else if(L2 == 0 && M  == 1)   	//0 0 0 0 x 1 0
	{
		motor_target_set(Line_Std -2,Line_Std);
	}

	else if(R1 == 0 && L1 == 1)		//0 0 1 x 0 0 0
	{
		motor_target_set(Line_Std,Line_Std -1); 
	}
	else if(L1 == 0 && R1 == 1)		//0 0 0 x 1 0 0
	{
		motor_target_set(Line_Std -1,Line_Std); 
	}
	else if(M  == 0 && L2 == 1 && R2 == 1)//0 0 x 1 x 0 0
	{
		motor_target_set(Line_Std,Line_Std); 
	}
}


unsigned char digtal(unsigned char channel)		//获取对应路的电平  
{
	uint8_t value = 0;
	switch(channel) 
	{
		case 1:  
			if(DL_GPIO_readPins(GPIOB, GPIO_Gray_R3_PIN) == 0) value = 1;
			else value = 0;  
			break;	
		case 2: 
			if(DL_GPIO_readPins(GPIOB, GPIO_Gray_R2_PIN) == 0) value = 1;
			else value = 0;  
			break;  
		case 3: 
			if(DL_GPIO_readPins(GPIOB, GPIO_Gray_R1_PIN) ==  0 ) value = 1;
			else value = 0;  
			break;   
		case 4:  
			if(DL_GPIO_readPins(GPIOA, GPIO_Gray_M_PIN ) == 0) value = 1;
			else value = 0;  
			break;   
		case 5:
			if(DL_GPIO_readPins(GPIOB, GPIO_Gray_L1_PIN) == 0) value = 1;
			else value = 0;  
			break;
		case 6:
			if(DL_GPIO_readPins(GPIOB, GPIO_Gray_L2_PIN) == 0) value = 1;
			else value = 0;  
			break;
		case 7:
			if(DL_GPIO_readPins(GPIOB, GPIO_Gray_L3_PIN) == 0) value = 1;
			else value = 0;  
			break;
	}
	return value; 
}


void track2(void){
	if(L3 == 0 && L2 == 0 && L1 == 0){			//1 1 0 0 0 0 0
		Turn_Flag = 1;
		Turn_Count ++;
		/*if( Turn_Count == circle * 4 + 1){
			motor_target_set(0, 0);
			DL_TimerG_setCaptureCompareValue(Motor_Pid_INST, 0, DL_TIMER_CC_0_INDEX);
			DL_TimerG_setCaptureCompareValue(Motor_Pid_INST, 0, DL_TIMER_CC_1_INDEX);
			mspm0_delay_ms(3600000);
		}*/
		motorA_duty(20);
		motorB_duty(20);
		delay_cycles(10000000);;
	}

	OLED_ShowNum(65, 48, Turn_Count, 1,OLED_8X16);
	OLED_Update();

	if(Turn_Flag == 1){
		motorA_duty(0);
		motorB_duty(55);
		mspm0_delay_ms(Turn_Dealy);
		Turn_Flag = 0;
		motorA_duty(20);
		motorB_duty(20);
		mspm0_delay_ms(100);
	}
	
	else if(R3 == 0 && R2 == 1)		//1 0 0 0 0 0 0
	{
		motorA_duty(Out_Std);
		motorB_duty(Out_Std - 8);
	}
	else if(L3 == 0 && L1 == 1)   	//0 0 0 0 0 x 1
	{
		motorA_duty(Out_Std - 8);
		motorB_duty(Out_Std);
	}

	else if(R2 == 0 && M  == 1)		//0 1 x 0 0 0 0
	{	
		motorA_duty(Out_Std);
		motorB_duty(Out_Std - 8);
	}
	else if(L2 == 0 && M  == 1)   	//0 0 0 0 x 1 0
	{
		motorA_duty(Out_Std - 8);
		motorB_duty(Out_Std);
	}

	else if(R1 == 0 && L1 == 1)		//0 0 1 x 0 0 0
	{
		motorA_duty(Out_Std);
		motorB_duty(Out_Std - 3);
	}
	else if(L1 == 0 && R1 == 1)		//0 0 0 x 1 0 0
	{
		motorA_duty(Out_Std - 3);
		motorB_duty(Out_Std);
	}
	else if(M  == 0 && L2 == 1 && R2 == 1)//0 0 x 1 x 0 0
	{
		motorA_duty(Out_Std);
		motorB_duty(Out_Std); 
	}
}



		/*if(R2 == 0)        //1 0 0 0 0
		{
			r2.start_time= system_time_40ms; 
			r2.executing_flag = 1;
			motor_target_set(40,-10);
		}
		else if(L2 == 0)   //0 0 0 0 1
		{
			l2.start_time= system_time_40ms; 
			l2.executing_flag = 1;
			motor_target_set(-10,40);
		}
		else if(R1 == 0)   //0 1 0 0 0
		{
			r1.start_time= system_time_40ms; 
			r1.executing_flag = 1;
			motor_target_set(25,15);
		}
		else if(L1 == 0)   //0 0 0 1 0
		{
			l1.start_time= system_time_40ms; 
			l1.executing_flag = 1;
			motor_target_set(15,25);
		}
		else if(M  == 0)   //0 0 1 0 0
		{
			motor_target_set(10,10); 
		}*/