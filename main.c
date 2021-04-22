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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
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

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
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

	//Selector choice reading
    init_selector();
    selector = get_selector();

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
