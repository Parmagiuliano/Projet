/*
 * 	main.c
 *
 *  Plotter multi axis
 *  E-puck project
 *  MT - EPFL - 2021
 *
 *  Authors: Parma Giuliano & Jacquart Sylvain
 *  Created : 14 april 2021
 */

/*
 * TO DO LIST:
 *
 * 		->Stick the target to the sensor, in X the luminosity (of sensor IR3/IR2 in epuck monitor) can improve from 0 to approx 30%
 * 										  in Y the luminosity (of sensor IR1/IR0 in epuck monitor) can improve from 0 to 50-100%
 * -Define 3 speeds (minimum, common and maximum) for the IMU displacements, and the IMU values associated (thresholds). -> 250/350/450 rpm
 * -Set the distance required in X and Y for the find_the_origin function. -> Stick to the sensor
 *
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

#include <pi_regulator.h>
#include <process_image.h>

#include <Drawing_IMU_function.h>

void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

/*
 * Start of the new code
 */

//Unit conversion: 1 step motor = 0.1382 mm with GearRadius R = 22mm.
//On a square of 80 x 80 mm, we have approx 570 x 570 step of motor.
//We'll move on a square of 70 x 70 mm, so approx 500 x 500 p (more secure).

int main(void)
{
    //start the system
    halInit();
    chSysInit();
    mpu_init();

    //start the serial communication
    serial_start();
    //start the USB communication
    usb_start();
    //starts the camera
    dcmi_start();
	po8030_start();
	//inits the motors
	motors_init();

	//stars the threads for the pi regulator and the processing of the image
	//pi_regulator_start();
	//process_image_start();

	/*
	 * Start of the new code
	 */

	//Finding the origin function
	    /*
	     * 	<-X AXIS	70mm/approx 500px square
	     * 	-----------------------------------
	     * 	-                        Origin  0-
	     * 	-                                 -
	     * 	-                                 -
	     * 	-                                 -
	     * 	-                                 -
	     * 	-                                 -
	     * 	-                                 -
	     * 	-                                 -	Y AXIS
	     * 	-                                 -	  ¦
	     * 	-----------------------------------   v
	     *
	     * BY CONVENTION FOR THIS PROJECT, LEFT_MOTOR = X_MOTOR & RIGHT_MOTOR = Y_MOTOR
	     *
	     * Rotate the X motor CW to push the wooden stage to the X origin.
	     * Stop the rotation when the IR sensor (IR3, close to the selector) detect the stopping plate when the intensity perceived by the sensor move from 0  to 30%.
	     *
	     * Then rotate the Y motor to pull the printed stage to the Y origin
	     * Stop the rotation when the IR sensors (IR1 & IR8, close to the camera) detect the stopping plate when the intensity perceived by the sensor move from 0  to 50%.
	     *
	     * Define this position as the origin
	     */

	//left_motor_set_speed(MOTOR_OPTIMAL_SPEED); //CW rotation, test

	void FindTheOrigin(uint8_t xTarget, uint8_t xThreshold, uint8_t yTarget, uint8_t yThreshold)
		{
			//Variables declaration
			xTarget = 1.3;
			xThreshold = 0.1;
			yTarget = 1.6;
			yThreshold = 0.1;
			//uint8_t sensor_x = SENSOR_X;
			//uint8_t sensor_y = SENSOR_Y;

			//Starts the proximity measurement module
			proximity_start();
			//Runs the IR sensor calibration process
			calibrate_ir();
			//Returns the calibration value for the chosen sensors
		//	uint8_t sensor_number = SENSOR_X;

			uint8_t sensor_x_calibration=get_calibrated_prox(SENSOR_X);
			uint8_t sensor_y_calibration=get_calibrated_prox(SENSOR_Y);

//			get_calibrated_prox(sensor_x);
//			get_calibrated_prox(sensor_y);

			//Infinite loop, X axis
			while(1)
				{
					//Returns the last value measured by the X sensor, U106
					//MUST CHECK IF THE SENSORS NUMBERS ARE CORRECTS
					//get_prox(sensor_x);
					if(get_prox(SENSOR_X) < (xTarget - xThreshold)*sensor_x_calibration){
						left_motor_set_speed(MOTOR_OPTIMAL_SPEED); //CW rotation
					}else if(get_prox(SENSOR_X) > (xTarget + xThreshold)*sensor_x_calibration){
						left_motor_set_speed(-1*MOTOR_OPTIMAL_SPEED); //CCW rotation
					}else if(get_prox > (xTarget - xThreshold)*sensor_x_calibration && get_prox < (xTarget + xThreshold)*sensor_x_calibration){
						left_motor_set_speed(0); //Stop the rotation
					break;
					}
				}

		//waits 0.25 second
		chThdSleepMilliseconds(250);

		//Return the Xstage to the other limit for the Yoffset
		//Move Xmotor CCW, distance 70mm
		left_motor_get_pos();
		int32_t counter_value = 0;
		left_motor_set_pos(counter_value=-500); //(Normally, -500 step == -70mm)

		//waits 0.25 second
		chThdSleepMilliseconds(250);

		//Infinite loop, Y axis
			while(1)
				{
				//Returns the last value measured by the Y sensor, (U108) ->U101
					//MUST CHECK IF THE SENSORS NUMBERS ARE CORRECTS
					if(get_prox(SENSOR_Y) < (yTarget - yThreshold)*sensor_y_calibration){
						right_motor_set_speed(-1*MOTOR_OPTIMAL_SPEED); //CCW rotation
					}else if(get_prox(SENSOR_Y) > (yTarget + yThreshold)*sensor_y_calibration){
						right_motor_set_speed(MOTOR_OPTIMAL_SPEED); //CW rotation
					}else if(get_prox(SENSOR_Y) > (yTarget - yThreshold)*sensor_y_calibration && get_prox(SENSOR_Y) < (yTarget + yThreshold)*sensor_y_calibration){
						right_motor_set_speed(0); //Stop the rotation
						break;
					}
				}

		//waits 0.25 second
		chThdSleepMilliseconds(250);

		//Return the Xstage to the X offset
		//Move Xmotor CW, distance 76mm
		left_motor_get_pos();
		left_motor_set_pos(counter_value=500); //(Normally, 500 step == 70mm)
		//Find_the_origin function completed

	}

	//Selector choice reading
	//int selector; -> Which type?
    //init_selector();	->Not usefull with ChibiOs it seems
    int8_t selector = get_selector();

    // LEDs sequences
    int sequence_pos = 0;
    static const uint8_t seq1[8][4] = {
        {0, 0, 0, 1},	// ON1
    	{0, 0, 0, 0},	// OFF1
    	{0, 0, 1, 0},	// ON3
    	{0, 0, 0, 0},	// OFF3
        {0, 1, 0, 0},	// ON5
    	{0, 0, 0, 0},	// OFF5
    	{1, 0, 0, 0},	// ON7
    	{0, 0, 0, 0},	// OFF7
    };

    switch (selector) //Need to put this in a thread, or let in the main?
    {
    	case 0: // Test function - Draw X and Y lines
    		//get_Drawing_test_function

    		/*
    		 * Once at the origin, start
    		 */
			break;

    	case 1: //Free to use, IMU		->To define: what about Z axis movement? Add a button which move up/down the pen when pressed?
    		show_gravity();
			break;

    	case 2: //Camera, pattern recognition
    		//get_Process_image
    		//get_PI_regulator
			break;

    	case 3: //Registered drawing - Epfl Logo
    		//get_Mighty_logo_function
			break;

//    	case 4: //Registered drawing - Random
//    		//get_Random_draw_function_1
//			break;
//
//    	case 5: //Registered drawing - Random
//    		//get_Random_draw_function_2
//			break;
//    	case 8: //Reserved for the epuck monitor (test mode)
//    		//get_Random_draw_function_2
//			break;

    	default: //LED pattern, inactive
//    		LEDs_update(seq1[sequence_pos]);
//    		sequence_pos++;
//    		sequence_pos %= 8;
    		break;
    }

    /* Infinite loop. */
    while (1) {
    	//waits 1 second
        chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
