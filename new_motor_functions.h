/*
 * new_motor_functions.h
 *
 *  Created on: May 14, 2021
 *      Author: sjacq
 */

#ifndef NEW_MOTOR_FUNCTIONS_H_
#define NEW_MOTOR_FUNCTIONS_H_

#include <stdint.h>
#include <hal.h>
#include <process_image.h>
#include <motors.h>
#include <main.h>
#include <Constants.h>

/**
 * @brief 	NEW: Set the speed of the left motor, to reach a certain target value
 *
 * @param 	speed     		speed desired in step/s
 * 			target_value	the number of steps desired
 */
void left_motor_get_to_the_pos(int speed, int16_t target_value);

/**
 * @brief 	NEW: Set the speed of the right motor, to reach a certain target value
 *
 * @param 	speed     		speed desired in step/s
 * 			target_value	the number of steps desired
 */
void right_motor_get_to_the_pos(int speed, int16_t target_value);


#endif /* NEW_MOTOR_FUNCTIONS_H_ */
