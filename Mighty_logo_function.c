/*
 * Mighty_logo_function.c
 *
 *  Created on: Apr 22, 2021
 *      Author: sjacq
 */

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
* @brief Draw the new EPFL logo on a 70 x 20mm rectangle
* 		 The instructions are given in an 3 columns array
*
* @param @1	Chosen motor: 0=X, 1=Y
* 		 @2	Direction: 1=CW, -1=CCW
* 		 @3	Number of steps*drawing cst
*/
//const int Mighty_sequence_array[39][3] = {
//		{1,	-1,	16},
//		{0,	1,	8},
//		{1,	1,	2},
//		{0,	-1,	6},
//		{1,	1,	4},
//		{0,	1,	5},
//		{1,	-1,	2},
//		{0,	-1,	5},
//		{1,	1,	5},
//		{0,	1,	6},
//		{1,	1,	2},
//		{0,	-1,	8},
//		{0,	1,	12},
//		{1,	-1,	9},
//		{0,	1,	4},
//		{1,	1,	3},
//		{0,	-1,	4},
//		{1,	1,	2},
//		{0,	1,	6},
//		{1,	-1,	7},
//		{0,	-1,	9},
//		{1,	1,	11},
//		{0,	1,	13},
//		{1,	-1,	9}, //9
//		{0,	1,	6},
//		{1,	-1,	2},
//		{0,	-1,	8},
//		{1,	1,	5},
//		{0,	1,	8},
//		{1,	1,	1.6},//2
//		{0,	-1,	8},
//		{1,	1,	5},//5
//		{0,	1,	18},
//		{1,	-1,	2},
//		{0,	-1, 6},
//		{1,	-1,	9},
//		{0,	-1,	3},
//		{1,	1,	11},
//		{0,	1,	8},
//};

//void Drawing_Mighty(void){
//
//	chThdSleepMilliseconds(5000);		//Sleep to set the sheet under the pen
//
//	//Variables declaration
//	uint8_t numRows = sizeof(Mighty_sequence_array) / sizeof(Mighty_sequence_array[0]);
//	//uint8_t numRows = 38;
//	int8_t motor_speed_sign = 0;		//Init value for the 2nd column
//	uint8_t counter_value_motor = 0;	//Init value for the 3th column
//
//	for (uint8_t row = 0; row < numRows; row++)
//	{
//		counter_value_motor = Mighty_sequence_array[row][2];
//		motor_speed_sign = Mighty_sequence_array[row][1];
//
//		if(Mighty_sequence_array[row][0] == 0)
//			   			 	 	 {
//			   			 		 	 left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor*DRAWING_CST_MIGHTY);
//			   			 		 	 left_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
//			   			 		 	 chThdSleepMilliseconds(100);
//			   			 	 	 }
//		else if(Mighty_sequence_array[row][0] == 1)
//			   			 	 	 {
//			   			 		 	 right_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, motor_speed_sign*counter_value_motor*DRAWING_CST_MIGHTY);
//			   			 		 	 right_motor_get_to_the_pos(MOTOR_NO_SPEED, 50);
//			   			 		 	 chThdSleepMilliseconds(100);
//			   			 	 	 }
//
//		}
//	}
