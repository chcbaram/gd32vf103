/*
 * bsp.c
 *
 *  Created on: Jun 13, 2021
 *      Author: baram
 */

#include "hw_def.h"
#include "bsp.h"






bool bspInit(void)
{

  return true;
}

void delay(uint32_t time_ms)
{
  uint64_t start_mtime, delta_mtime;

  /* Don't start measuruing until we see an mtime tick */
  uint64_t tmp = get_timer_value();
  do {
      start_mtime = get_timer_value();
  } while (start_mtime == tmp);

  do {
      delta_mtime = get_timer_value() - start_mtime;
  }while(delta_mtime <(SystemCoreClock/4000.0 *time_ms ));
}

uint32_t millis(void)
{
  uint32_t ret;

  uint64_t start_mtime;

  /* Don't start measuruing until we see an mtime tick */
  uint64_t tmp = get_timer_value();
  do {
      start_mtime = get_timer_value();
  } while (start_mtime == tmp);


  ret = start_mtime / (SystemCoreClock/4000.0);

  return ret;
}


