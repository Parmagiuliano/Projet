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
#include <motors.h>
#include "usbcfg.h"
#include "chprintf.h"
#include "i2c_bus.h"
#include "imu.h"
#include "exti.h"

/* Aimed pattern for the square spiral:
 *1 left_motor CW 70
 *2 rigth_motor CCW 70
 *3 left_motor CCW 70
 *4 right_motor CW 65
 *5 left_motor CW 65
 *6 right_motor CCW 65
 *7 left_motor CCW 60
 *8 right_motor CW 60
 *9 left_motor CW 60
 * ... until
 *40 right_motor CW 5
 */


//Set the motor speed for the function -> usefull?
//right_motor_set_speed(int speed = MOTOR_OPTIMAL_SPEED);
//left_motor_set_speed(int speed = MOTOR_OPTIMAL_SPEED);

void drawing_test_func(uint8_t motor_speed_sign, uint16_t counter_value_motor)
{
	//Set the origin for the drawing_test_func
	left_motor_get_pos;
	right_motor_get_pos;

	int i;
	for(i = 1; i < 41; i++)
 	 {
	 	 //Using the modulo 4 to change the sens of motors in the for loop
	 	 if(i % 4 == 0 | i % 4 == 1)
	 	 {
	 		 motor_speed_sign = 1;
	 	 }
	 	 else if (i % 4 == 2 | i % 4 == 3)
	 	 {
	 		 motor_speed_sign = -1;
	 	 }

	 	 //Using the modulo 3 to decrease the length of lines over time.
	 	 counter_value_motor = 507; //507 steps ~= 70cm
	 	 if(i % 3 == 1 && i != 1)
	 	 {
	 		 counter_value_motor = counter_value_motor - 36; //36 steps ~= 5cm
	 	 }
//	 	 else if (i % 3 != 1)
//	 	 {
//	 		 counter_value_motor = counter_value_motor;
//	 	 }

	 	 //Using the modulo 2 to choose which motor should run.
	 	 if (i % 2 == 0)
	 	 {
	 		 right_motor_set_pos(motor_speed_sign*counter_value_motor);
	 	 }
	 	 else if (i % 2 != 0)
	 	 {
	 		 left_motor_set_pos(motor_speed_sign*counter_value_motor);
	 	 }

	chThdSleepMilliseconds(250);
	return 0;
 	 }
}



