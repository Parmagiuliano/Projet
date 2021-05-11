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
#include "exti.h"

#include <Drawing_test_function.h>

/* Aimed pattern for the square spiral:
 *1 left_motor CW 55mm
 *2 rigth_motor CCW 55mm
 *3 left_motor CCW 51mm
 *4 right_motor CW 51mm
 *5 left_motor CW 47mm
 *6 right_motor CCW 47mm
 *7 left_motor CCW 43mm
 *8 right_motor CW 43mm
 * ... until
 *32 right_motor CW 4mm
 *
 *  --------------------X
 * 	¦
 * 	¦    -----------
 * 	¦    ¦         ¦
 * 	¦    v         ¦
 * 	----------------
 *		TOP VIEW
 */

const int Drawing_test_array[32][3] = {
			{0,	1,	41},
			{1,	-1,	41},
			{0,	-1,	39},
			{1,	1,	39},
			{0,	1,	36},
			{1,	-1,	36},
			{0,	-1,	33},
			{1,	1,	33},
			{0,	1,	30},
			{1,	-1,	30},
			{0,	-1,	27},
			{1,	1,	27},
			{0,	1,	24},
			{1,	-1,	24},
			{0,	-1,	21},
			{1,	1,	21},
			{0,	1,	18},
			{1,	-1,	18},
			{0,	-1,	15},
			{1,	1,	15},
			{0,	1,	12},
			{1,	-1,	12},
			{0,	-1,	9},
			{1,	1,	9},
			{0,	1,	6},
			{1,	-1,	6},
			{0,	-1,	3},
			{1,	1,	3},
};

/** Drawing_test function @brief
*   Draw a square spiral, starting from the external origin, dimensions 55mm x 5mm.
* 	Every change (X/Y motor, motor direction & lenght of lines) are controlled by the array
*/
void Drawing_test_func(void){

	chThdSleepMilliseconds(20000);		//Sleep to set the sheet under the pen

	//Variables declaration
	uint8_t numRows = 32;
	int8_t motor_speed_sign = 0;
	uint16_t counter_value_motor = 0;	//Init value for the 3th column

	for (uint8_t row = 0; row < numRows; row++)
	{
		counter_value_motor = Drawing_test_array[row][2];
		motor_speed_sign = Drawing_test_array[row][1];

		if(Drawing_test_array[row][0] == 0)
			   			 	 	 {
			   			 		 	 left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor*DRAWING_CST_TEST);
			   			 		 	 left_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
			   			 		     chThdSleepMilliseconds(100);
			   			 	 	 }
		else if(Drawing_test_array[row][0] == 1)
			   			 	 	 {
			   			 		 	 right_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor*DRAWING_CST_TEST);
			   			 		 	 right_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
			   			 		     chThdSleepMilliseconds(100);
			   			 	 	 }

		}
}

/*
 * As the square spiral has a repetitive pattern of motor/direction/number of step,
 * the function could be defined with a for loop and the modulo of the increment, as shown below.
 */


/** Drawing_test function @brief
*  Draw a square spiral, starting from the external origin, dimensions 70mm x 70mm.
*  Every change (X/Y motor, motor direction & lenght of lines) are controlled by the modulo of the increment for loop.
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
//	uint16_t counter_value_motor = 400; 	//400 steps ~= 55mm
//
//	for(i = 1; i < 33; i++)
// 	 {
//	 	 //Using the modulo 2 to choose which motor should run.
//	 	 if ((i % 2) == 0)
//	 	 {
//	 		 	 right_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor*DRAWING_CST);
//	 		 	 right_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
//	 		//return;
//	 	 }
//	 	 else if ((i % 2) != 0)
//	 	 {
//	 		 	 left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor*DRAWING_CST);
//	 		 	 left_motor_get_to_the_pos(MOTOR_NO_SPEED, 0);
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
//	 		 counter_value_motor = counter_value_motor - 2*DRAWING_CST; //38 steps ~= 5mm
//	 	 }
//	 	 else if (i % 3 != 1)
//	 	 {
//	 		 counter_value_motor = counter_value_motor;	//No changes
//	 	 }
//
//	chThdSleepMilliseconds(250);
// 	 }
//	//return;
//}



