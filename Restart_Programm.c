/*
 * Restart_Programm.c
 *
 *  Created on: May 13, 2021
 *      Author: sjacq
 */

#include <Restart_Programm.h>
#include <selector.h>
#include "sensors/proximity.h"
#include "motors.h"
#include <main.h>


static int8_t last_selector = 0;

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
//	messagebus_t bus;
//	MUTEX_DECL(bus_lock);
//	CONDVAR_DECL(bus_condvar);
//
//
//		messagebus_init(&bus, &bus_lock, &bus_condvar);
//		//Starts the proximity measurement module
//		proximity_start();
//		//Runs the IR sensor calibration process
//		calibrate_ir();

		//chThdCreateStatic(waThdFindTheXOrigin, sizeof(waThdFindTheXOrigin), NORMALPRIO, ThdFindTheXOrigin, NULL);

		/*
		 * Infinite loop, X axis
		 * Returns the last value measured by the X sensor ->U106
		 */
		while(1){
					left_motor_set_speed(-MOTOR_OPTIMAL_SPEED*2); 	//CCW rotation
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
		left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED*2, 480);	//480
		chThdSleepMilliseconds(250);

		//chThdCreateStatic(waThdFindTheYOrigin, sizeof(waThdFindTheYOrigin), NORMALPRIO, ThdFindTheYOrigin, NULL);

		/*
		 * Infinite loop, Y axis
		 * Returns the last value measured by the Y sensor ->U101
		 */
		while(1){
					right_motor_set_speed(MOTOR_OPTIMAL_SPEED*2); 	//CW rotation
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
		left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED*2, -450);	//-450
		chThdSleepMilliseconds(250);
}


static THD_WORKING_AREA(waThdRestart_Programm, 256);
static THD_FUNCTION(ThdRestart_Programm, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    last_selector = get_selector();
    uint8_t current_selector_position = 0;

    FindTheOrigin();

    while(1){
    	current_selector_position = get_selector();
    	if(current_selector_position =! last_selector){
    	FindTheOrigin();

    	}

    }
}

void Restart_Programm_start(void){
	chThdCreateStatic(waThdRestart_Programm, sizeof(waThdRestart_Programm), NORMALPRIO, ThdRestart_Programm, NULL);
}



