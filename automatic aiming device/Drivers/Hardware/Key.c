#include "ti_msp_dl_config.h"
#include "Key.h"

								
uint8_t Key_Num = 0;	


uint8_t Key_GetNum(void)										
{										
	uint8_t Temp;
	if (Key_Num)
	{
		Temp = Key_Num;
		Key_Num = 0;
        DL_GPIO_setPins(GPIO_Key_PORT,GPIO_Key_K1_PIN);
		DL_GPIO_clearPins(GPIO_Key_PORT,GPIO_Key_K2_PIN);
		DL_GPIO_clearPins(GPIO_Key_PORT,GPIO_Key_K3_PIN);
		return Temp;
	}
	return 0;										
}


uint8_t Key_GetState(void){					
	if (DL_GPIO_readPins(GPIO_Key_PORT,GPIO_Key_K1_PIN) == 0){return 1;}
	if (DL_GPIO_readPins(GPIO_Key_PORT,GPIO_Key_K2_PIN) == 1){return 2;}
	else{return 0;}
}

void Key_Tick(void)									
{
	static uint8_t Count;
	static uint8_t CurrState, PrevState;
	
	Count ++;
	
	if (Count >= 20)
	{
		Count = 0;
		
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if (CurrState == 0 && PrevState != 0)
		{
			Key_Num = PrevState;
		}
	}
}
