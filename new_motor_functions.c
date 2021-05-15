/*
 * new_motor_functions.c
 *
 *  Created on: May 14, 2021
 *      Author: sjacq
 */
#include <ch.h>
#include <hal.h>
#include "motors.h"
#include "leds.h"
#include <math.h>
#include <new_motor_functions.h>
#include <Constants.h>

//#define MOTOR_TIMER_FREQ 100000 // [Hz]
//#define THRESV 650 // This is the speed under which the power save feature is active.
//
//static const uint8_t step_halt[4] = {0, 0, 0, 0};
////table of the differents steps of to control the motors
////it corresponds to microsteps.
////in the reality two microsteps correspond to 1 step of the motor.
//static const uint8_t step_table[8][4] = {
//    {1, 0, 1, 0},
//	{0, 0, 1, 0},
//    {0, 1, 1, 0},
//	{0, 1, 0, 0},
//    {0, 1, 0, 1},
//	{0, 0, 0, 1},
//    {1, 0, 0, 1},
//	{1, 0, 0, 0},
//};

//struct stepper_motor_s {
//    enum {
//        HALT=0,
//        FORWARD=1,
//        BACKWARD=2
//    } direction;
//    uint8_t step_index;
//    int32_t count;  //in microsteps
//    void (*update)(const uint8_t *out);
//    void (*enable_power_save)(void);
//    void (*disable_power_save)(void);
//    PWMDriver *timer;
//};
//
//struct stepper_motor_s right_motor;
//struct stepper_motor_s left_motor;


void left_motor_get_to_the_pos(int speed, int16_t target_value){
	//Single step time: 200us
				if(target_value > 0){
					left_motor_set_speed(speed);
				}else if(target_value < 0){
					left_motor_set_speed(-speed);
				}
				chThdSleepMilliseconds(7*abs(target_value));			//Approx. pour MOTOR_OPTIMAL_SPEED=300
				left_motor_set_speed(MOTOR_NO_SPEED);
}

void right_motor_get_to_the_pos(int speed, int16_t target_value){
	//Single step time: 200us
				if(target_value > 0){
					right_motor_set_speed(speed);
				}else if(target_value < 0){
					right_motor_set_speed(-speed);
				}
				//chThdSleepMilliseconds((1/speed)*abs(target_value));
				chThdSleepMilliseconds(7*abs(target_value));			//Approx. pour MOTOR_OPTIMAL_SPEED=300
				right_motor_set_speed(MOTOR_NO_SPEED);
	    	  }
