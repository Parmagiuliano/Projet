/*
 * Drawing_test_function.c
 *
 *  Created on: Apr 27, 2021
 *  Authors: Parma Giuliano & Jacquart Sylvain
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ch.h>
#include <hal.h>
#include "memory_protection.h"
#include <main.h>
#include <motors.h>
#include "usbcfg.h"
#include "chprintf.h"
#include "i2c_bus.h"
//#include "imu.h"		//NOT USEFULL
#include "exti.h"

#include <Drawing_test_function.h>

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
 *
 *  --------------------X
 * 	¦
 * 	¦    -----------
 * 	¦    ¦         ¦
 * 	¦    v         ¦
 * 	----------------
 *
 */

const int Drawing_test_array[20][3] = {
//		{0,	1,	433},
//		{1,	-1,	433},
//		{0,	-1,	397},
//		{1,	1,	397},
//		{0,	1,	361},
//		{1,	-1,	361},
//		{0,	-1,	325},
//		{1,	1,	325},
		{0,	1,	289},
		{1,	-1,	289},
		{0,	-1,	253},
		{1,	1,	253},
		{0,	1,	217},
		{1,	-1,	217},
		{0,	-1,	180},
		{1,	1,	180},
		{0,	1,	144},
		{1,	-1,	144},
		{0,	-1,	108},
		{1,	1,	108},
		{0,	1,	72},
		{1,	-1,	72},
		{0,	-1,	36},
		{1,	1,	36},
//		{0,	1,	180},
//		{1,	-1,	180},
//		{0,	-1,	144},
//		{1,	1,	144},
//		{0,	1,	144},
//		{1,	-1,	108},
//		{0,	-1,	108},
//		{1,	1,	108},
//		{0,	1,	72},
//		{1,	-1,	72},
//		{0,	-1,	72},
//		{1,	1,	36},
//		{0,	1,	36},
//		{1,	-1,	36},
};

void Drawing_test_func(void){

	chThdSleepMilliseconds(15000);

	//Variables declaration
	uint8_t numRows = 20;
	int8_t motor_speed_sign = 0;
	uint16_t counter_value_motor = 0;	//Init value for the 3th column

	for (uint8_t row = 0; row < numRows; row++)
	{
		counter_value_motor = Drawing_test_array[row][2];
		motor_speed_sign = Drawing_test_array[row][1];

		if(Drawing_test_array[row][0] == 0)
			   			 	 	 {
			   			 		 	 left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor);
			   			 		 	 left_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
			   			 		 //chThdSleepMilliseconds(250);
			   			 	 	 }
		else if(Drawing_test_array[row][0] == 1)
			   			 	 	 {
			   			 		 	 right_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor);
			   			 		 	 right_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
			   			 		 //chThdSleepMilliseconds(250);
			   			 	 	 }

		}
	}


/** Drawing_test function
* @brief Draw a square spiral, starting from the external origin, dimensions 70mm x 70mm.
* 		 Every change (X/Y motor, motor direction & lenght of lines) are controlled by the modulo of the increment for loop.
*
* @param motor_speed_sign		+-1, based on the modulo of the for loop increment
* 		 counter_value_motor	value set at 507 ~=70cm, decrease by step of 36 ~=5cm
*/

//void Drawing_test_func(void)
//{
//	//Set the origin for the drawing_test_func
//	left_motor_set_pos(0);
//	right_motor_set_pos(0);
//
//	//Variables declaration
//
//	uint8_t i;
//	int8_t motor_speed_sign = 1;
//	uint16_t counter_value_motor = 507; 	//507 steps ~= 70cm
//
//	for(i = 1; i < 41; i++)
// 	 {
//	 	 //Using the modulo 2 to choose which motor should run.
//	 	 if ((i % 2) == 0)
//	 	 {
//	 		 	 right_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor);
//	 		 	 right_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
//	 		//return;
//	 	 }
//	 	 else if ((i % 2) != 0)
//	 	 {
//	 		 	 left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor);
//	 		 	 left_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
//	 		//return;
//	 	 }
//
//	 	 //Using the modulo 4 to change the sens of motors in the for loop
//	 	 if((i % 4 == 0) || (i % 4 == 1))
//	 	 {
//	 		 motor_speed_sign = 1;
//	 	 }
//	 	 else if ((i % 4 == 2) || (i % 4 == 3))
//	 	 {
//	 		 motor_speed_sign = -1;
//	 	 }
//
//	 	 //Using the modulo 3 to decrease the length of lines over time.
//	 	 if((i % 3 == 1) && (i != 1))
//	 	 {
//	 		 counter_value_motor = counter_value_motor - 36; //36 steps ~= 5cm
//	 	 }
////	 	 else if (i % 3 != 1)
////	 	 {
////	 		 counter_value_motor = counter_value_motor;	//No changes
////	 	 }
//
//	chThdSleepMilliseconds(250);
// 	 }
//	//return;
//}



