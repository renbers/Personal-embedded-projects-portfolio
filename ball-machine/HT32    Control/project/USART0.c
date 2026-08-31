#include "USART0.h"

__ALIGN4 _USART0_STRUCT rxd_scomm0;
__ALIGN4 _USART0_STRUCT txd_scomm0;

/*
 * USART0 is used as the Bluetooth/control input.
 * RX interrupt only stores bytes into rxd_scomm0; protocol parsing runs in main loop.
 */
void USART0_Configuration(void)
{
	USART_InitTypeDef USART_InitStruct;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.USART0   = 1;
	CKCUClock.Bit.AFIO     = 1;
	CKCUClock.Bit.PB      = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);

	USART_DeInit(HT_USART0);  //复位串口3

  GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_1, GPIO_PR_UP);  //Turn on UxART Rx internal pull up resistor to prevent unknow state

  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_0, AFIO_FUN_USART_UART);  // Config AFIO mode
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_1, AFIO_FUN_USART_UART);  // Config AFIO mode

  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStruct.USART_StopBits = USART_STOPBITS_1;
  USART_InitStruct.USART_Parity = USART_PARITY_NO;
  USART_InitStruct.USART_Mode = USART_MODE_NORMAL;
  USART_Init(HT_USART0, &USART_InitStruct);
  USART_RxCmd(HT_USART0, ENABLE);
  USART_TxCmd(HT_USART0, ENABLE);

  USART_IntConfig(HT_USART0, USART_INT_RXDR, ENABLE);

  NVIC_EnableIRQ(USART0_IRQn);
  USART0_init_buffer();
}

//-----------------------------------------------------------------------------
void USART0_init_buffer (void)
{
  rxd_scomm0.read_pt = 0;
  rxd_scomm0.write_pt = 0;
  rxd_scomm0.cnt = 0;

  txd_scomm0.read_pt = 0;
  txd_scomm0.write_pt = 0;
  txd_scomm0.cnt = 0;
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void USART0_tx_data(u8 *pt, u8 len)
{
  /* Blocking TX is acceptable for short ACK/NACK frames at 9600 bps. */
  while(len--)
  {
    while (USART_GetFlagStatus(HT_USART0, USART_FLAG_TXDE) == RESET)
    {
    }
    USART_SendData(HT_USART0, *pt++);
  }

  while (USART_GetFlagStatus(HT_USART0, USART_FLAG_TXC) == RESET)
  {
  }
}

//-----------------------------------------------------------------------------
u8 USART0_read_byte(u8 *data)
{
  /* Called by Protocol_Process() to drain bytes received by USART0_IRQHandler(). */
  if (rxd_scomm0.cnt == 0)
  {
    return 0;
  }

  *data = rxd_scomm0.buffer[rxd_scomm0.read_pt];
  rxd_scomm0.read_pt = (rxd_scomm0.read_pt + 1) % USART0_BUF_SIZE;

  NVIC_DisableIRQ(USART0_IRQn);
  rxd_scomm0.cnt--;
  NVIC_EnableIRQ(USART0_IRQn);

  return 1;
}

//-----------------------------------------------------------------------------
void USART0_test(void)
{
  u8 i,test_array[8];
  for(i=0; i<8; i++)
  {
    test_array[i] = i;
  }
  USART0_tx_data(test_array, 8);
}

//-----------------------------------------------------------------------------
void USART0_IRQHandler(void)
{
  u8 data;

  if (USART_GetIntStatus(HT_USART0, USART_INT_RXDR) != RESET)
  {
    USART_ClearFlag(HT_USART0, USART_INT_RXDR);
    data = (u8)USART_ReceiveData(HT_USART0);

    /* Keep ISR short: only push the byte into the RX ring buffer. */
    if (rxd_scomm0.cnt < USART0_BUF_SIZE)
    {
      rxd_scomm0.buffer[rxd_scomm0.write_pt] = data;
      rxd_scomm0.write_pt = (rxd_scomm0.write_pt + 1) % USART0_BUF_SIZE;
      rxd_scomm0.cnt++;
    }
  }
}




