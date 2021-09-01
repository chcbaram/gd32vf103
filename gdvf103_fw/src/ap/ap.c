/*
 * ap.c
 *
 *  Created on: Jun 13, 2021
 *      Author: baram
 */


#include "ap.h"




void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);
}

void apMain(void)
{
  uint32_t pre_time;
  button_obj_t btn_1;
  uint32_t press_cnt = 0;



  buttonObjCreate(&btn_1, 0, 0);


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();

      ledToggle(_DEF_LED1);
    }

    cliMain();

    if (buttonObjGetClicked(&btn_1, 50) == true)
    {
      logPrintf("press cnt : %d\n", press_cnt++);
    }
  }
}
