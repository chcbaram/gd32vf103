/*
 * button.c
 *
 *  Created on: 2021. 9. 1.
 *      Author: baram
 */


#include "button.h"
#include "cli.h"


#ifdef _USE_HW_BUTTON


typedef struct
{
  uint32_t      port;
  uint32_t      pin;
  FlagStatus    on_state;
} button_tbl_t;


button_tbl_t button_tbl[BUTTON_MAX_CH] =
    {
        {GPIOA, GPIO_PIN_0,  SET},
        {GPIOC, GPIO_PIN_13, SET},
    };


#ifdef _USE_HW_CLI
static void cliButton(cli_args_t *args);
#endif


bool buttonInit(void)
{
  bool ret = true;


  rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_GPIOC);


  for (int i=0; i<BUTTON_MAX_CH; i++)
  {
    gpio_init(button_tbl[i].port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, button_tbl[i].pin);
  }

#ifdef _USE_HW_CLI
  cliAdd("button", cliButton);
#endif

  return ret;
}

bool buttonGetPressed(uint8_t ch)
{
  bool ret = false;
  FlagStatus  pin_state;

  if (ch >= BUTTON_MAX_CH)
  {
    return false;
  }


  pin_state = gpio_input_bit_get(button_tbl[ch].port, button_tbl[ch].pin);

  if (pin_state == button_tbl[ch].on_state)
  {
    ret = true;
  }

  return ret;
}

void buttonObjCreate(button_obj_t *p_obj, uint8_t ch, uint32_t repeat_time)
{
  p_obj->ch = ch;
  p_obj->state = 0;
  p_obj->pre_time = millis();
  p_obj->repeat_time = repeat_time;
}

bool buttonObjGetClicked(button_obj_t *p_obj, uint32_t pressed_time)
{
  bool ret = false;


  switch(p_obj->state)
  {
    case 0:
      if (buttonGetPressed(p_obj->ch) == true)
      {
        p_obj->state = 1;
        p_obj->pre_time = millis();
      }
      break;

    case 1:
      if (buttonGetPressed(p_obj->ch) == true)
      {
        if (millis()-p_obj->pre_time >= pressed_time)
        {
          ret = true; // ¹öÆ° Å¬¸¯µÊ
          p_obj->state = 2;
          p_obj->pre_time = millis();
        }
      }
      else
      {
        p_obj->state = 0;
      }
      break;

    case 2:
      if (buttonGetPressed(p_obj->ch) == true)
      {
        if (p_obj->repeat_time > 0)
        {
          if (millis()-p_obj->pre_time >= p_obj->repeat_time)
          {
            ret = true; // ¹öÆ° Å¬¸¯µÊ
            p_obj->pre_time = millis();
          }
        }
      }
      else
      {
        p_obj->state = 0;
      }
      break;
  }

  return ret;
}


#ifdef _USE_HW_CLI

void cliButton(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "show"))
  {
    while(cliKeepLoop())
    {
      for (int i=0; i<BUTTON_MAX_CH; i++)
      {
        cliPrintf("%d", buttonGetPressed(i));
      }
      cliPrintf("\n");

      delay(100);
    }

    ret = true;
  }


  if (ret != true)
  {
    cliPrintf("button show\n");
  }
}
#endif



#endif
