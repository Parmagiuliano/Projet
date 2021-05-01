/*
 * Drawing_test_function.c
 *
 *  Created on: Apr 27, 2021
 *      Author: sjacq
 *
 *  Draw a square spiral, starting from the external origin, dimensions 70mm x 70mm
 *  Cf excel file associated
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

//  int i;
//
//  for (i = 1; i < 11; ++i)
//  {
//    /*
//     *
//     */
//  }
/*
 * left_motor CW 70
 * rigth_motor CCW 70
 * left_motor CCW 70
 * right_motor CW 65
 * left_motor CW 65
 * right_motor CCW 65
 * left_motor CCW 60
 * right_motor CW 60
 * left_motor CW 60
 * right_motor CCW 55
 * left_motor CCW 55
 * right_motor CW 55
 * left_motor CW 50
 * right_motor CCW 50
 *
 */

int mod3 = 3;
int motor_drawing_test_func;

if (number % 2 == 0) numberIsEven 	//right motor
if (number % 2 != 0) numberIsOdd 	//left motor

//Set the origin for the drawing test func
left_motor_get_pos;
right_motor_get_pos;



for($i=1; $i<40; $i++)
{
    if($i % $x == 0)
    {
        // display image
    }
}

int i;

 for (i = 1; i < 11; ++i)
 {
   printf("%d ", i);
 }

right_motor_set_speed

