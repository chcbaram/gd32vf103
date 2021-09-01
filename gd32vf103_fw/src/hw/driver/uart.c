/*
 * uart.c
 *
 *  Created on: 2021. 9. 1.
 *      Author: baram
 */


#include "uart.h"
#include "qbuffer.h"


#ifdef _USE_HW_UART



#define UART_RX_BUF_LENGTH      1024


typedef enum
{
  UART_HW_TYPE_UART,
  UART_HW_TYPE_USB,
  UART_HW_TYPE_BT_SPP,
} UartHwType_t;


typedef struct
{
  bool     is_open;
  uint32_t baud;
  UartHwType_t type;
  uint32_t h_uart;

  uint8_t  rx_buf[UART_RX_BUF_LENGTH];
  qbuffer_t qbuffer;

} uart_tbl_t;


static uart_tbl_t uart_tbl[UART_MAX_CH];





bool uartInit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open = false;
    uart_tbl[i].baud = 57600;
  }

  return true;
}

bool uartOpen(uint8_t ch, uint32_t baud)
{
  bool ret = false;
  uint32_t usart_periph;


  switch(ch)
  {
    case _DEF_UART1:
      uart_tbl[ch].type    = UART_HW_TYPE_UART;
      uart_tbl[ch].baud    = baud;
      uart_tbl[ch].h_uart  = USART0;

      qbufferCreate(&uart_tbl[ch].qbuffer, uart_tbl[ch].rx_buf, UART_RX_BUF_LENGTH);


      /* USART interrupt configuration */
      eclic_global_interrupt_enable();
      eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
      eclic_irq_enable(USART0_IRQn, 1, 0);

      /* configure COM0 */
      //gd_eval_com_init(EVAL_COM0);

       rcu_periph_clock_enable(RCU_GPIOA);
       rcu_periph_clock_enable(RCU_USART0);

       // TXD
       gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

       // RXD
       gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);


       usart_periph = uart_tbl[ch].h_uart;

       usart_deinit(usart_periph);
       usart_baudrate_set(usart_periph, baud);
       usart_word_length_set(usart_periph, USART_WL_8BIT);
       usart_stop_bit_set(usart_periph, USART_STB_1BIT);
       usart_parity_config(usart_periph, USART_PM_NONE);
       usart_hardware_flow_rts_config(usart_periph, USART_RTS_DISABLE);
       usart_hardware_flow_cts_config(usart_periph, USART_CTS_DISABLE);
       usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);
       usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);
       usart_enable(usart_periph);


      usart_interrupt_enable(usart_periph, USART_INT_RBNE);


      uart_tbl[ch].is_open = true;

      ret = true;
      break;
  }

  return ret;
}

bool uartClose(uint8_t ch)
{
  return true;
}

uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:
      ret = qbufferAvailable(&uart_tbl[ch].qbuffer);
      break;
  }

  return ret;
}

bool uartFlush(uint8_t ch)
{
  uint32_t pre_time;

  pre_time = millis();
  while(uartAvailable(ch))
  {
    if (millis()-pre_time >= 10)
    {
      break;
    }
    uartRead(ch);
  }

  return true;
}

uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:
      qbufferRead(&uart_tbl[ch].qbuffer, &ret, 1);
      break;
  }

  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  uint32_t pre_time;
  bool is_sent;


  switch(ch)
  {
    case _DEF_UART1:

      for (int i=0; i<length; i++)
      {
        usart_data_transmit(uart_tbl[ch].h_uart, p_data[i]);

        is_sent = false;
        pre_time = millis();
        while(millis()-pre_time < 10)
        {
          if (usart_flag_get(uart_tbl[ch].h_uart, USART_FLAG_TBE) == SET)
          {
            usart_flag_clear(uart_tbl[ch].h_uart, USART_FLAG_TBE);
            is_sent = true;
            break;
          }
        }
        if (is_sent == false)
        {
          break;
        }
      }

      if (is_sent == true)
      {
        ret = length;
      }
      break;
  }

  return ret;
}

uint32_t uartPrintf(uint8_t ch, const char *fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args);

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);


  return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
  uint32_t ret = 0;


  switch(ch)
  {
    case _DEF_UART1:
      ret = uart_tbl[ch].baud;
      break;
  }

  return ret;
}





void USART0_IRQHandler(void)
{
  uint8_t rx_data;
  uint8_t ch = _DEF_UART1;

  if(usart_interrupt_flag_get(uart_tbl[ch].h_uart, USART_INT_FLAG_RBNE) == SET)
  {
    rx_data = usart_data_receive(uart_tbl[ch].h_uart);
    qbufferWrite(&uart_tbl[ch].qbuffer, &rx_data, 1);
  }
}

#endif
