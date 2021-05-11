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
 * Home-made functions
 */
#include <Drawing_test_function.c>
#include <Drawing_test_function.h>
#include <Drawing_IMU_function.c>
#include <Drawing_IMU_function.h>
#include <Mighty_logo_function.c>
#include <Mighty_logo_function.h>
//#include <process_image.h>

void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

//static THD_WORKING_AREA(waThdFrontLed, 128);
//static THD_FUNCTION(ThdFrontLed, arg) {
//
//    chRegSetThreadName(__FUNCTION__);
//    (void)arg;
//
//    systime_t time;
//
//    while(1){
//        time = chVTGetSystemTime();
//        palTogglePad(GPIOD, GPIOD_LED_FRONT);
//        chThdSleepUntilWindowed(time, time + MS2ST(10));
//    }
//}
//
//chThdCreateStatic(waThdFrontLed, sizeof(waThdFrontLed), NORMALPRIO, ThdFrontLed, NULL);

//static THD_WORKING_AREA(waThdFindTheXOrigin, 128);
//static THD_FUNCTION(ThdFindTheXOrigin, arg) {
//
//    chRegSetThreadName(__FUNCTION__);
//    (void)arg;
//
//    //systime_t time;
//
//    /*
//    * Infinite loop, X axis
//    * Returns the last value measured by the X sensor ->U106
//    */
//    while(1){
//    	left_motor_set_speed(-MOTOR_OPTIMAL_SPEED); 	//CCW rotation
//    		if(get_prox(SENSOR_X) > IR_OPTIMAL_DIST)
//    			{
//    				left_motor_set_speed(MOTOR_NO_SPEED);
//    				break;
//    			}
//    		}
//    /*
//     * Return the Xstage to the other limit for the Yoffset
//     * Move Xmotor CW, distance 65mm ~=470 steps
//     */
//	left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, 25*DRAWING_CST);
//	chThdSleepMilliseconds(250);
//}

//static THD_WORKING_AREA(waThdFindTheYOrigin, 128);
//static THD_FUNCTION(ThdFindTheYOrigin, arg) {
//
//    chRegSetThreadName(__FUNCTION__);
//    (void)arg;
//
//    //systime_t time;
//
//    /*
//    		 * Infinite loop, Y axis
//    		 * Returns the last value measured by the Y sensor ->U101
//    		 */
//
//    while(1){
//    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED); 	//CW rotation
//    			if(get_prox(SENSOR_Y) > IR_OPTIMAL_DIST)
//    				{
//    					right_motor_set_speed(-MOTOR_NO_SPEED);
//    					break;
//    				}
//    		}
//	/*
//	 * Return the Xstage to the X offset
//	 * Move Xmotor CW, distance 55mm
//	 */
//	left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, -19*DRAWING_CST_TEST);
//
//}


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
*
*/

void FindTheOrigin(void)
	{
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
		left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, 48*DRAWING_CST_TEST);	//470
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
		left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, -38*DRAWING_CST_TEST);	//-400
		chThdSleepMilliseconds(250);
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
    	case 1: //Draw a square spiral, starting from the external origin, dimensions 70mm x 70mm
    		Drawing_test_func();
			break;

    	case 2: //Free to draw with IMU
    			//It comes from TP3

    	    /** Inits the Inter Process Communication bus. */
    	    messagebus_init(&bus, &bus_lock, &bus_condvar);
    	    imu_start();

//    	    chThdCreateStatic(waThdFrontLed, sizeof(waThdFrontLed), NORMALPRIO, ThdFrontLed, NULL); //JUST ADDED
//    	    chThdCreateStatic(waThdBodyLed, sizeof(waThdBodyLed), NORMALPRIO, ThdBodyLed, NULL);


    	    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
    	    imu_msg_t imu_values;

    	    //wait 2 sec to be sure the e-puck is in a stable position
    	    chThdSleepMilliseconds(2000);
    	    imu_compute_offset(imu_topic, NB_SAMPLES_OFFSET);
    		while(1){
    		        //wait for new measures to be published
    		        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));

//    		        chprintf((BaseSequentialStream *)&SD3, "%Ax=%-7d Ay=%-7d Az=%-7d Gx=%-7d Gy=%-7d Gz=%-7d\r\n",
//    		                imu_values.acc_raw[X_AXIS], imu_values.acc_raw[Y_AXIS], imu_values.acc_raw[Z_AXIS],
//    		                imu_values.gyro_raw[X_AXIS], imu_values.gyro_raw[Y_AXIS], imu_values.gyro_raw[Z_AXIS]);
//
//    		        //prints raw values with offset correction
//    		        chprintf((BaseSequentialStream *)&SD3, "%Ax=%-7d Ay=%-7d Az=%-7d Gx=%-7d Gy=%-7d Gz=%-7d\r\n",
//    		                imu_values.acc_raw[X_AXIS]-imu_values.acc_offset[X_AXIS],
//    		                imu_values.acc_raw[Y_AXIS]-imu_values.acc_offset[Y_AXIS],
//    		                imu_values.acc_raw[Z_AXIS]-imu_values.acc_offset[Z_AXIS],
//    		                imu_values.gyro_raw[X_AXIS]-imu_values.gyro_offset[X_AXIS],
//    		                imu_values.gyro_raw[Y_AXIS]-imu_values.gyro_offset[Y_AXIS],
//    		                imu_values.gyro_raw[Z_AXIS]-imu_values.gyro_offset[Z_AXIS]);
//
//    		        //prints values in readable units
//    		        chprintf((BaseSequentialStream *)&SD3, "%Ax=%.2f Ay=%.2f Az=%.2f Gx=%.2f Gy=%.2f Gz=%.2f (%x)\r\n\n",
//    		                imu_values.acceleration[X_AXIS], imu_values.acceleration[Y_AXIS], imu_values.acceleration[Z_AXIS],
//    		                imu_values.gyro_rate[X_AXIS], imu_values.gyro_rate[Y_AXIS], imu_values.gyro_rate[Z_AXIS],
//    		                imu_values.status);

    		        Drawing_IMU(&imu_values);
    		        chThdSleepMilliseconds(50);	//100
    		}
			break;

    	case 3: //Registered drawing - Epfl Logo
    		Drawing_Mighty();
			break;

    	case 4: //Camera, pattern recognition
//    		process_image_start();
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
