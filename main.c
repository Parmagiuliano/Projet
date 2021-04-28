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
 * -Linear displacement of the target in front of the IR sensor -> graph of the response, influence of the luminosity.
 * -Trying the system in test mode, to see if the mechanic is working properly.
 * -Finding the minimal speed required to the motor to turn, threshold of the mechanical resistance (stuck).
 * -Define 3 speeds (minimum, common and maximum) for the IMU displacements, and the IMU values associated (thresholds).
 * -Set the distance required in X and Y for the find_the_origin function.
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

void panic_handler(const char *reason)
{
    (void)reason;

    palClearPad(GPIOD, GPIOD_LED1);
    palClearPad(GPIOD, GPIOD_LED3);
    palClearPad(GPIOD, GPIOD_LED5);
    palClearPad(GPIOD, GPIOD_LED7);
    palClearPad(GPIOD, GPIOD_LED_FRONT);
    palClearPad(GPIOB, GPIOB_LED_BODY);

    while (true) {

    }
}

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
//We'll move on a square of 76 x 76 mm, so 550 x 550 p (more secure).

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
	pi_regulator_start();
	process_image_start();

	/*
	 * Start of the new code
	 */

	//Finding the origin function
	    /*
	     * 	<-X AXIS	76mm/550px square
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
	     * Stop the rotation when the IR sensor (IR3, close to the selector) detect the stopping plate at a distance of 4 mm.
	     *
	     * Then rotate the Y motor to pull the printed stage to the Y origin
	     * Stop the rotation when the IR sensors (IR1 & IR8, close to the camera) detect the stopping plate at a distance of 4 mm.
	     *
	     * Define this position as the origin
	     */
	void find_the_origin(unsigned int xTarget = 4, xThreshold = 0.2, yTarget = 4, yThreshold = 0.2){//dimensions in mm, conversion?
		//Starts the proximity measurement module
		proximity_start();
		//Runs the IR sensor calibration process
		calibrate_ir();
		//Returns the calibration value for the chosen sensors
		int get_calibrated_prox(unsigned int sensor_number=6);
		int get_calibrated_prox(unsigned int sensor_number=8);

		//Infinite loop, X axis
			while(1)
				{
					//Returns the last value measured by the X sensor, U106
					//MUST CHECK IF THE SENSORS NUMBERS ARE CORRECTS
					int get_prox(unsigned int sensor_number = 6);
					if(get_prox < (xTarget - xThreshold)){
						void left_motor_set_speed(int speed=400); //CW rotation
					}else if(get_prox > (xTarget + xThreshold)){
						void left_motor_set_speed(int speed=-400); //CCW rotation
					}else if(get_prox > (xTarget - xThreshold) && get_prox < (xTarget + xThreshold)){
						void left_motor_set_speed(int speed=0); //Stop the rotation
						break;
					}
				}

		//waits 0.25 second
		chThdSleepMilliseconds(250);

		//Return the Xstage to the other limit for the Yoffset
		//Move Xmotor CCW, distance 76mm
		int32_t left_motor_get_pos(void);
		void left_motor_set_pos(int32_t counter_value=-550); //(Normally, 550 step == 76mm)

		//waits 0.25 second
		chThdSleepMilliseconds(250);

		//Infinite loop, Y axis
			while(1)
				{
				//Returns the last value measured by the Y sensor, (U101) ->U108
					//MUST CHECK IF THE SENSORS NUMBERS ARE CORRECTS
					int get_prox(unsigned int sensor_number = 8);
					if(get_prox < (yTarget - yThreshold)){
						void right_motor_set_speed(int speed=-400); //CCW rotation
					}else if(get_prox > (yTarget + yThreshold)){
						void right_motor_set_speed(int speed=400); //CW rotation
					}else if(get_prox > (yTarget - yThreshold) && get_prox < (yTarget + yThreshold)){
						void right_motor_set_speed(int speed=0); //Stop the rotation
						break;
					}
				}

		//waits 0.25 second
		chThdSleepMilliseconds(250);

		//Return the Xstage to the X offset
		//Move Xmotor CW, distance 76mm
		int32_t left_motor_get_pos(void);
		void left_motor_set_pos(int32_t counter_value=550); //(Normally, 550 step == 76mm)
		//Find_the_origin function completed

	}

	//Selector choice reading
	//int selector; -> Which type?
    //init_selector();	->Not usefull with ChibiOs it seems
    int8_t selector = get_selector();

    // LEDs sequences
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
    		//get_Drawing_IMU_function
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

    	default: //LED pattern, inactive
    		LEDs_update(seq1[sequence_pos]);
    		sequence_pos++;
    		sequence_pos %= 8;
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
