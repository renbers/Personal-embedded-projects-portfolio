#include "main.h"


void Interrupt_Init(void)
{
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(Vision_UART_INST_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_Encoder_INT_IRQN);
}

void SysTick_Handler(void)
{
    tick_ms++;
}

 
 void GROUP1_IRQHandler(void)
{
    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_LA_PIN)){
            Encoder_count1 ++;
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_LA_PIN);
    }


    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_RA_PIN)){
            Encoder_count2 ++;
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_RA_PIN);
    }

    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Key_PORT, GPIO_Key_K3_PIN)){
        Key_Num = 3;
        DL_GPIO_clearInterruptStatus(GPIO_Key_PORT, GPIO_Key_K3_PIN);
    }

    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Key_PORT, GPIO_Key_K4_PIN)){
        Key_Num = 4;
        DL_GPIO_clearInterruptStatus(GPIO_Key_PORT, GPIO_Key_K4_PIN);
    }

    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Key_PORT, GPIO_Key_K5_PIN)){
        Key_Num = 5;
        DL_GPIO_clearInterruptStatus(GPIO_Key_PORT, GPIO_Key_K5_PIN);
    }
}

void Vision_UART_INST_IRQHandler(void)
{
    uint8_t rx_data;

    switch (DL_UART_Main_getPendingInterrupt(Vision_UART_INST)) {
    case DL_UART_MAIN_IIDX_RX:
        rx_data = DL_UART_Main_receiveData(Vision_UART_INST);
        Vision_InputByte(rx_data);
        break;

    default:
        break;
    }
}
