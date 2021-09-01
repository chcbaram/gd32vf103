/*
 * led.c
 *
 *  Created on: 2021. 6. 14.
 *      Author: baram
 */


#include "led.h"


#ifdef _USE_HW_LED

typedef struct
{
  uint32_t  port;
  uint32_t  pin;
  uint8_t   on_state;
  uint8_t   off_state;
} led_tbl_t;



static const led_tbl_t led_tbl[LED_MAX_CH] =
    {
        {GPIOB, GPIO_PIN_5, 0, 1},
        {GPIOB, GPIO_PIN_0, 0, 1},
        {GPIOB, GPIO_PIN_1, 0, 1},
    };





bool ledInit(void)
{
  rcu_periph_clock_enable(RCU_GPIOB);


  for (int i=0; i<LED_MAX_CH; i++)
  {
    gpio_init(led_tbl[i].port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, led_tbl[i].pin);

    ledOff(i);
  }

  return true;
}

void ledOn(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;


  if (led_tbl[ch].on_state > 0)
  {
    gpio_bit_set(led_tbl[ch].port, led_tbl[ch].pin);
  }
  else
  {
    gpio_bit_reset(led_tbl[ch].port, led_tbl[ch].pin);
  }
}

void ledOff(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  if (led_tbl[ch].off_state > 0)
  {
    gpio_bit_set(led_tbl[ch].port, led_tbl[ch].pin);
  }
  else
  {
    gpio_bit_reset(led_tbl[ch].port, led_tbl[ch].pin);
  }
}

void ledToggle(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  if (gpio_input_bit_get(led_tbl[ch].port, led_tbl[ch].pin) == RESET)
  {
    gpio_bit_set(led_tbl[ch].port, led_tbl[ch].pin);
  }
  else
  {
    gpio_bit_reset(led_tbl[ch].port, led_tbl[ch].pin);
  }
}


#endif
