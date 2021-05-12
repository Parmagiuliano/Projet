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
#include <stdint.h>
#include <stdbool.h>
#include <main.h>


#include <ch.h>
#include <hal.h>
#include <usbcfg.h>
#include <chprintf.h>
#include "memory_protection.h"
#include "i2c_bus.h"
#include "exti.h"
#include <msgbus/messagebus.h>

#include <motors.h>
#include <camera/po8030.h>
#include <selector.h>
#include <leds.h>
#include <sensors/proximity.h>
#include <sensors/imu.h>

/*
 * Home-made includes to add
 */
//#include <Drawing_test_function.c>
#include <Drawing_test_function.h>
//#include <Drawing_IMU_function.c>
#include <Drawing_IMU_function.h>
//#include <Mighty_logo_function.c>		//To remove?
#include <Mighty_logo_function.h>
//#include <process_image.c>			//Just commented
#include <process_image.h>
#include <Draw_pattern.h>

/*
 * Definition of functions
 */
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

    sdStart(&SD3, &ser_cfg); // UART3. Connected to the second com port of the programmer
}

/* Finding the origin function @In detail
 *
 * By convention for this project, LEFT_MOTOR = X_MOTOR & RIGHT_MOTOR = Y_MOTOR
 *
 * Unit conversion: 1 step motor = 0.1382 mm with GearRadius R = 22mm.
 * On a square of 70 x 70 mm, we have approx 500 x 500 step of motor.
 * To avoid any damage to the hardware, the pen will draw on smaller squares (typ 65 x 65 mm).
 *
 * Schematic of the hardware:
 *
 * 	<--X AXIS  70mm/approx 500px square
 * 	------------------------------
 * 	-                   Origin  0-
 * 	-                            -
 * 	-                            -
 * 	-         TOP VIEW           -  OOO
 * 	-                            - OOYOO Y motor
 * 	-                            -  OOO
 * 	-   OOO                      -
 * 	-  OOXOO X motor             -Y AXIS
 * 	-   OOO                      -   ¦
 * 	------------------------------   v
 *
 * Rotate the X motor CW to push the wooden stage to the X origin.
 * Stop the rotation when the IR sensor (IR3, close to the selector),
 * detect the stopping plate when the intensity perceived by the sensor move from 0 to 30%.
 *
 * Rotate the Y motor to pull the grey printed stage to the Y origin.
 * Stop the rotation when the IR sensors (IR1 , close to the camera) detect the stopping plate,
 * detect the stopping plate when the intensity perceived by the sensor move from 0 to 50%.
 *
 * Define this position as the origin
 */

/* Finding the origin function @brief
* ->Use IR sensors to find X-Y end courses
* ->Get back to the top right corner and set the position as (0,0)
*/

void FindTheOrigin(void)
	{
	//** Drawing_IMU function		//JUST ADDED
	messagebus_t bus;
	MUTEX_DECL(bus_lock);
	CONDVAR_DECL(bus_condvar);


		messagebus_init(&bus, &bus_lock, &bus_condvar);
		//Starts the proximity measurement module
		proximity_start();
		//Runs the IR sensor calibration process
		calibrate_ir();

		//chThdCreateStatic(waThdFindTheXOrigin, sizeof(waThdFindTheXOrigin), NORMALPRIO, ThdFindTheXOrigin, NULL);

		/*
		 * Infinite loop, X axis
		 * Returns the last value measured by the X sensor ->U106
		 */
		while(1){
					left_motor_set_speed(-MOTOR_OPTIMAL_SPEED); 	//CCW rotation
					if(get_prox(SENSOR_X) > IR_OPTIMAL_DIST)
						{
							left_motor_set_speed(MOTOR_NO_SPEED);
							break;
						}
				}

		/*
		 * Return the Xstage to the other limit for the Yoffset
		 * Move Xmotor CW, distance 65mm ~=470 steps
		 */
		left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, 480);	//470
		chThdSleepMilliseconds(250);

		//chThdCreateStatic(waThdFindTheYOrigin, sizeof(waThdFindTheYOrigin), NORMALPRIO, ThdFindTheYOrigin, NULL);

		/*
		 * Infinite loop, Y axis
		 * Returns the last value measured by the Y sensor ->U101
		 */
		while(1){
					right_motor_set_speed(MOTOR_OPTIMAL_SPEED); 	//CW rotation
					if(get_prox(SENSOR_Y) > IR_OPTIMAL_DIST)
						{
							right_motor_set_speed(-MOTOR_NO_SPEED);
							break;
						}
				}

		/*
		 * Return the Xstage to the X offset
		 * Move Xmotor CW, distance 55mm
		 */
		left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, -450);	//-400
		chThdSleepMilliseconds(250);
}

/*
 * Definition of threads
 */

//Calling the Drawing_IMU function
static THD_WORKING_AREA(waThdDrawing_IMU, 128);
static THD_FUNCTION(ThdDrawing_IMU, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    /** Inits the Inter Process Communication bus. */
    			//** Drawing_IMU function		//JUST ADDED
    			messagebus_t bus;
    			MUTEX_DECL(bus_lock);
    			CONDVAR_DECL(bus_condvar);

        	    messagebus_init(&bus, &bus_lock, &bus_condvar);
        	    imu_start();

        	    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
        	    imu_msg_t imu_values;

        	    //wait 2 sec to be sure the e-puck is in a stable position
        	    chThdSleepMilliseconds(2000);
        	    imu_compute_offset(imu_topic, NB_SAMPLES_OFFSET);

    while(1){
		//wait for new measures to be published
    		        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));
    		        Drawing_IMU(&imu_values);
    		        chThdSleepMilliseconds(100);
    }
}

//Looking for a selector change while the plotter is working
static THD_WORKING_AREA(waThdSwitch_Selector, 128);
static THD_FUNCTION(ThdSwitch_Selector, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    uint8_t last_selector_position = get_selector();
    uint8_t definitive_choice_selector = 0;

    while(1){
    	uint8_t current_selector_position = get_selector();

    	if (current_selector_position != last_selector_position){
    		/*
    		 * Stop the current drawing
    		 * Wait for X seconds, recheck if the selector pos changed again.
    		 * If not, procede to a new iteration of the FindTheOrigin function,
    		 * then start the the function chosen by the selecter.
    		 */

    		chThdSleepMilliseconds(2000);
    		definitive_choice_selector = get_selector();
    		while (definitive_choice_selector =! current_selector_position){
    			definitive_choice_selector = current_selector_position;
    			chThdSleepMilliseconds(2000);
    		}

//    		if (uint8_t chosen_selector_position == current_selector_position){	//The new position has been reached
//
//    		}else if(chosen_selector_position =! current_selector_position){ //We're still choosing the position



    	}

    }
}

//Tracking the pen position during the Drawing_IMU func, to avoid hardware damages
static THD_WORKING_AREA(waThdIMU_Pen_Tracker, 128);
static THD_FUNCTION(ThdIMU_Pen_Tracker, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
	int16_t pos_motor_limit_min = -10;
	int16_t pos_motor_limit_max = 500;

	//Set the origin for the drawing_IMU_func
	left_motor_set_pos(0);
	right_motor_set_pos(0);

    while(1){
    	//track in real time the position of the pen in the board
		int x_pos_motor = left_motor_get_pos();
		int y_pos_motor = right_motor_get_pos();

		//Size of the border to fit
		if(x_pos_motor > pos_motor_limit_max ){
				left_motor_set_speed(MOTOR_NO_SPEED); //X motor -> Stop
				chThdSleepMilliseconds(500);
				left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, -10);

		}else if(x_pos_motor < pos_motor_limit_min){
            	left_motor_set_speed(MOTOR_NO_SPEED); //X motor -> Stop
            	chThdSleepMilliseconds(500);
            	left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, 10);

		}else if(y_pos_motor > pos_motor_limit_max){
				right_motor_set_speed(MOTOR_NO_SPEED); //Y motor -> Stop
				chThdSleepMilliseconds(500);
				right_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, -10);

		}else if(y_pos_motor < pos_motor_limit_min){
				right_motor_set_speed(MOTOR_NO_SPEED); //Y motor -> Stop
				chThdSleepMilliseconds(500);
				right_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, 10);
		}
    }
}

int main(void)
{
    //Initialize the system
    halInit();
    chSysInit();
    mpu_init();
	motors_init();

    //Start the peripherals / communication
    serial_start();
    usb_start();
    dcmi_start();
	po8030_start();
    i2c_start();

    //Finding the origin function
	FindTheOrigin();

	//Selector choice reading
	int8_t selector = 0;
    selector = get_selector();

    switch (selector)
    {
    /*
     * 	case 0: Off position, nothing to be done
     */
    	case 1: //Draw a square spiral, starting from the external origin
    		Drawing_test_func();
			break;

    	case 2: //Free to draw with IMU, the code is based on TP3
    	    chThdCreateStatic(waThdDrawing_IMU, sizeof(waThdDrawing_IMU), NORMALPRIO+1, ThdDrawing_IMU, NULL);
    	    chThdCreateStatic(waThdIMU_Pen_Tracker, sizeof(waThdIMU_Pen_Tracker), NORMALPRIO, ThdIMU_Pen_Tracker, NULL);
    	    break;

    	case 3: //Registered drawing - Epfl Logo
    		Drawing_Mighty();
			break;

    	case 4: //Camera, pattern recognition and drawing
    		process_image_start();
    		draw_pattern_start();
			break;
	/*
	 * 	case 8: Reserved for the epuck monitor (test mode)
	 */

    	default: //LEDs, inactive
    		set_body_led(1); // value (0=off 1=on higher=inverse)
    		set_front_led(1); //value (0=off 1=on higher=inverse)
    		break;
    }

    /* Infinite loop. */
    while (1) 	{
    	//Check for selector change
    	chThdCreateStatic(waThdSwitch_Selector, sizeof(waThdSwitch_Selector), NORMALPRIO, ThdSwitch_Selector, NULL);
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
