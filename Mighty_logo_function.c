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
 * @1	Increment number
 * @2	Chosen motor: 0=X, 1=Y
 * @3	Direction: 0=CW, 1=CCW
 * @4	Number of steps
 */

static const uint8_t Mighty_sequence[39][4] = {
{1,		1,	1,	217},
{2,		0,	0,	74},
{3,		1,	0,	19},
{4,		0,	1,	55},
{5,		1,	0,	41},
{6,		0,	0,	48},
{7,		1,	1,	19},
{8,		0,	1,	48},
{9,		1,	0,	43},
{10,	0,	0,	53},
{11,	1,	0,	19},
{12,	0,	1,	77},
{13,	0,	0,	118},
{14,	1,	1,	86},
{15,	0,	0,	38},
{16,	1,	0,	24},
{17,	0,	1,	38},
{18,	1,	0,	19},
{19,	0,	0,	60},
{20,	1,	1,	50},
{21,	0,	1,	82},
{22,	1,	0,	106},
{23,	0,	0,	120},
{24,	1,	1,	84},
{25,	0,	0,	53},
{26,	1,	1,	19},
{27,	0,	1,	77},
{28,	1,	0,	43},
{29,	0,	0,	72},
{30,	1,	0,	19},
{31,	0,	1,	72},
{32,	1,	0,	19},
{33,	0,	0,	168},
{34,	1,	1,	19},
{35,	0,	1,	53},
{36,	1,	1,	86},
{37,	0,	1,	24},
{38,	1,	0,	106},
{39,	0,	0,	77},
};

void Drawing_Mighty(uint8_t motor_speed_sign, uint16_t counter_value_motor){
	int i;
	for(i = 1; i < 40; i++){
		 //Using the 3rd column to change the sens of motors
			 	 if()//3rd line value = 0
			 	 {
			 		 motor_speed_sign = 1;
			 	 }
			 	 else if()//3rd line value = 1
			 	 {
			 		 motor_speed_sign = -1;
			 	 }
	counter_value_motor = 0; //Value of the 4th column

	//Using the modulo 2 to choose which motor should run.
		 	 if ()//2nd value = 0
		 	 {
		 		 left_motor_set_pos(motor_speed_sign*counter_value_motor);
		 	 }
		 	 else if (i % 2 != 0)//2nd value = 1
		 	 {
		 		right_motor_set_pos(motor_speed_sign*counter_value_motor);
		 	 }
		 	chThdSleepMilliseconds(250);
		 	return 0;
	}
}


