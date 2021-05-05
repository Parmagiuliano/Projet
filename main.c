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

#include <main.h>

#include <ch.h>
#include <hal.h>
#include "memory_protection.h"
#include <usbcfg.h>
#include <motors.h>
#include <camera/po8030.h>
#include <chprintf.h>
#include <selector.h>
#include <leds.h>
#include "i2c_bus.h"
#include "exti.h"

#include <Drawing_test_function.h>
#include <Drawing_IMU_function.h>
#include <Mighty_logo_function.h>
#include <process_image.h>

//#include <messagebus.h>
//#include <sensors/imu.h>		//NOT USEFULL, used only in Drawing_IMU_function

//extern messagebus_t bus;		//ADDED LINE
static imu_msg_t imu_values;	//ADDED LINE, USEFULL

static void timer11_start(void){
    //General Purpose Timer configuration
    //timer 11 is a 16 bit timer so we can measure time
    //to about 65ms with a 1Mhz counter
    static const GPTConfig gpt11cfg = {
        1000000,        /* 1MHz timer clock in order to measure uS.*/
        NULL,           /* Timer callback.*/
        0,
        0
    };

    gptStart(&GPTD11, &gpt11cfg);
    //let the timer count to max value
    gptStartContinuous(&GPTD11, 0xFFFF);
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

/* Finding the origin function @In detail
 *
 * By convention for this project, LEFT_MOTOR = X_MOTOR & RIGHT_MOTOR = Y_MOTOR
 *
 * Unit conversion: 1 step motor = 0.1382 mm with GearRadius R = 22mm.
 * On a square of 80 x 80 mm, we have approx 570 x 570 step of motor.
 * To avoid any damage of the hardware, the pen will move on a square of 70 x 70 mm, approximatively 500 x 500 step.
 *
 * Schematic of the hardware:
 *
 * 	<--X AXIS  70mm/approx 500px square
 * 	------------------------------
 * 	-                   Origin  0-
 * 	-                            -
 * 	-                            -
 * 	-         TOP VIEW           -  YYY
 * 	-                            - YYOYY
 * 	-                            -  YYY
 * 	-   XXX                      -
 * 	-  XXOXX                     -Y AXIS
 * 	-   XXX                      -   ¦
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
* ->Get back to the top right corner and set the position
*
*/

static void FindTheOrigin()
	{
		//Starts the proximity measurement module
		proximity_start();
		//Runs the IR sensor calibration process
		calibrate_ir();

		/*
		 * Infinite loop, X axis
		 * Returns the last value measured by the X sensor ->U106
		 */
		while(1){
					if(get_prox(SENSOR_X) < IR_OPTIMAL_DIST){
						left_motor_set_speed(MOTOR_OPTIMAL_SPEED); 	//CW rotation
					}else if(get_prox(SENSOR_X) > IR_OPTIMAL_DIST){
						left_motor_set_speed(MOTOR_NO_SPEED); 		//Stop the rotation
						break;
					}
				 }

		chThdSleepMilliseconds(250);

		/*
		 * Return the Xstage to the other limit for the Yoffset
		 * Move Xmotor CCW, distance 76mm ~=550 steps
		 */
		left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, -550);
		chThdSleepMilliseconds(250);

		/*
		 * Infinite loop, Y axis
		 * Returns the last value measured by the Y sensor ->U101
		 */

		while(1)
			{
				if(get_prox(SENSOR_Y) < IR_OPTIMAL_DIST) {
					right_motor_set_speed(-MOTOR_OPTIMAL_SPEED); 	//CCW rotation
				}else if(get_prox(SENSOR_Y) > IR_OPTIMAL_DIST){
					right_motor_set_speed(MOTOR_NO_SPEED); 			//Stop the rotation
					break;
				}
			}

		chThdSleepMilliseconds(250);

		/*
		 * Return the Xstage to the X offset
		 * Move Xmotor CW, distance 70mm for a margin of 6 mm
		 */
		left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, 500);
		chThdSleepMilliseconds(250);
	}

int main(void)
{
    //start the system
    halInit();
    chSysInit();
    mpu_init();
	motors_init();

    //Some initializations
    serial_start();
    usb_start();
    dcmi_start();
	po8030_start();
    i2c_start();
    imu_start();

    //Finding the origin function
	FindTheOrigin();

	//Selector choice reading
	int selector = 0;
    selector = get_selector();

    switch (selector)
    {
    /*
     * 	case 0: Off position, nothing to be done
     */
    	case 1: //Draw a square spiral, starting from the external origin, dimensions 70mm x 70mm
    		Drawing_test_func();
			break;

    	case 2: //Free to draw with IMU
    		Drawing_IMU(&imu_values);
			break;

    	case 3: //Registered drawing - Epfl Logo
    		Drawing_Mighty();
			break;

    	case 4: //Camera, pattern recognition
    		//get_Process_image
    		//get_PI_regulator
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
