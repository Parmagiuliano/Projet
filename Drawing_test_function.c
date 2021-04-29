/*
 * Drawing_test_function.c
 *
 *  Created on: Apr 27, 2021
 *      Author: sjacq
 */

//Includes to check
#include <ch.h>
#include <hal.h>
#include <main.h>
#include "usbcfg.h"
#include "chprintf.h"
#include "i2c_bus.h"
#include "imu.h"
#include "exti.h"

  int i;

  for (i = 1; i < 11; ++i)
  {
    printf("%d ", i);
  }

int32_t left_motor_get_pos(void);
int32_t right_motor_get_pos(void);

