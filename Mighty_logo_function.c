/*
 * Mighty_logo_function.c
 *
 *  Created on: Apr 22, 2021
 *      Author: sjacq
 */

//INCLUDES TO CHECK
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ch.h>
#include <hal.h>
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <chprintf.h>

#include <process_image.h>
#include <Mighty_logo_function.h>



/** Mighty_logo function
* @brief Draw the new EPFL logo on a 50x50cm square
* 		 The instructions are given in an 4 columns array
*
* @param
*/

/*
 * Drawing sequence:
 * @1	Chosen motor: 0=X, 1=Y
 * @2	Direction: 0=CW, 1=CCW
 * @3	Number of steps
 */

const int Mighty_sequence_array[39][3] = {
{1,	1,	217},
{0,	0,	74},
{1,	0,	19},
{0,	1,	55},
{1,	0,	41},
{0,	0,	48},
{1,	1,	19},
{0,	1,	48},
{1,	0,	43},
{0,	0,	53},
{1,	0,	19},
{0,	1,	77},
{0,	0,	118},
{1,	1,	86},
{0,	0,	38},
{1,	0,	24},
{0,	1,	38},
{1,	0,	19},
{0,	0,	60},
{1,	1,	50},
{0,	1,	82},
{1,	0,	106},
{0,	0,	120},
{1,	1,	84},
{0,	0,	53},
{1,	1,	19},
{0,	1,	77},
{1,	0,	43},
{0,	0,	72},
{1,	0,	19},
{0,	1,	72},
{1,	0,	19},
{0,	0,	168},
{1,	1,	19},
{0,	1,	53},
{1,	1,	86},
{0,	1,	24},
{1,	0,	106},
{0,	0,	77},
};

void Drawing_Mighty(void){

	//Variables declaration
	ptr[1][3] = Mighty_sequence_array[i];
	uint8_t numRows = 38;
	uint8_t numCols = 2;
	int *ptr;
	int8_t motor_speed_sign = 0;
	uint16_t counter_value_motor = 0;

	//counter_value_motor = 0; //Default value of the 3th column

	for (uint8_t row = 0; row < numRows; row++) // step through the rows in the array
	{
	    for (uint8_t col = 0; col < numCols; col++) // step through each element in the row
	    {
	    		 //Using the 2rd column value to define the sens of motors
	   			 	 if(ptr + 1 == 0)//2nd line value = 0
	   			 	 	 {
	   			 		 	 motor_speed_sign = 1;
	   			 	 	 }
	   			 	 else if(ptr + 1 == 1)//2rd line value = 1
	   			 	 	 {
	   			 		 	 motor_speed_sign = -1;
	   			 	 	 }
	   			 //Using the 1st column value to define which motor should run.
	   			 counter_value_motor = ptr + 2; //Default value of the 3th column
	   			 	 if (ptr == 0)//1st value = 0
	   			 	 	 {
	   			 		 	 left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor);
	   			 	 	 }
	   			 	 else if (ptr == 1)//1nd value = 1
	   			 	 	 {
	   			 		 	 right_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor);
	   			 	 	 }
	   			 chThdSleepMilliseconds(250);
	    }
	    return 0;
	}

//	//%%%%%%%%%%%%%%%%%%%%%%%%
//	int i;
//	for(i = 1; i < 40; i++){
//		 //Using the 2rd column to change the sens of motors
//			 	 if()//2rd line value = 0
//			 	 {
//			 		 motor_speed_sign = 1;
//			 	 }
//			 	 else if()//2rd line value = 1
//			 	 {
//			 		 motor_speed_sign = -1;
//			 	 }
//	counter_value_motor = 0; //Value of the 3th column
//
//	//Using the modulo 2 to choose which motor should run.
//		 	 if ()//1st value = 0
//		 	 {
//		 		 left_motor_set_pos(motor_speed_sign*counter_value_motor);
//		 	 }
//		 	 else if (i % 2 != 0)//1nd value = 1
//		 	 {
//		 		right_motor_set_pos(motor_speed_sign*counter_value_motor);
//		 	 }
//		 	chThdSleepMilliseconds(250);
//		 	return 0;
//	}
}


