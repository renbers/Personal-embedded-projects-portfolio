#include "stm32f10x.h"                  // Device header


void UART_Init(void)
{
    //步骤一：
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
    //步骤二：
    //（TX）
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    //(RX)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
 
    //配置USART参数：
	USART_InitTypeDef UART_InitStructure;
    UART_InitStructure.USART_BaudRate = 9600;  // 设置波特率为9600
    UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //禁用硬件流控制
    UART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // 同时支持发送和接收
    UART_InitStructure.USART_Parity = USART_Parity_No;  // 不使用奇偶校验
    UART_InitStructure.USART_StopBits = USART_StopBits_1;  // 设置停止位为1位
    UART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 设置数据位长度为8位
    USART_Init(USART1, &UART_InitStructure);  // 使用上述配置初始化USART1
 
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//函数用于配置 USART 的中断使能状态
 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStructure);
 
    USART_Cmd(USART1,ENABLE);//使能USART1
 
}
