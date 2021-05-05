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
//INCLUDES TO CHECK
<<<<<<< Updated upstream
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
#include <Drawing_test_function.h>
#include <Drawing_IMU_function.h>
#include <Mighty_logo_function.h>
=======
#include <stdio.h>		//OK
#include <stdlib.h>		//OK
#include <string.h>		//OK
#include <math.h>		//OK

#include <main.h>		//OK

#include <ch.h>			//OK
#include <hal.h>		//OK
#include "memory_protection.h"	//OK
#include <usbcfg.h>		//OK
#include <motors.h>		//OK
#include <camera/po8030.h>	//OK
#include <chprintf.h>	//OK
#include <selector.h>	//OK
#include <leds.h>		//OK
//#include "i2c_bus.h"			//NOT USEFULL?
#include "exti.h"		//OK

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
>>>>>>> Stashed changes

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

/** Finding the origin function @In detail
 *
 * By convention for this project, LEFT_MOTOR = X_MOTOR & RIGHT_MOTOR = Y_MOTOR
 *
 * Unit conversion: 1 step motor = 0.1382 mm with GearRadius R = 22mm.
 * On a square of 80 x 80 mm, we have approx 570 x 570 step of motor.
 * To avoid any damage of the hardware, the pen will move on a square of 70 x 70 mm, approximatively 500 x 500 step.
 *
 * Schematic of the hardware:
 *
 * 	<--X AXIS	70mm/approx 500px square
 * 	--------------------------------
 * 	-                     Origin  0-
 * 	-                              -
 * 	-                              -
 * 	-           TOP VIEW           -  YYY
 * 	-                              - YYOYY
 * 	-                              -  YYY
 * 	-     XXX                      -
 * 	-    XXOXX                     -Y AXIS
 * 	-     XXX                      -   ¦
 * 	--------------------------------   v
 *
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

/** Finding the origin function @brief
* ->Use IR sensors to find X-Y end courses
* ->Get back to the top right corner and set the position
*
* @param 	x,yTarget     Desired increase of the IR sensor response, to define the end course (percentage of the calibration value)
* 			x,yThreshold  Margin around both sides of the target (percentage)
*/

void FindTheOrigin(uint8_t xTarget, uint8_t xThreshold, uint8_t yTarget, uint8_t yThreshold)	//FLOAT INSTEAD OF INT?
	{
<<<<<<< Updated upstream
		//Variables declaration
		xTarget = 1.3;
		xThreshold = 0.1;
		yTarget = 1.6;
		yThreshold = 0.1;

=======
>>>>>>> Stashed changes
		//Starts the proximity measurement module
		proximity_start();
		//Runs the IR sensor calibration process
		calibrate_ir();
<<<<<<< Updated upstream
		//Returns the calibration value for the chosen sensors
		uint8_t sensor_x_calibration=get_calibrated_prox(SENSOR_X);
		uint8_t sensor_y_calibration=get_calibrated_prox(SENSOR_Y);
=======
>>>>>>> Stashed changes

		//Infinite loop, X axis
		while(1)
			{
				//Returns the last value measured by the X sensor, U106			//MUST RECHECK IF THE SENSORS NUMBERS ARE CORRECTS
				if(get_prox(SENSOR_X) < (xTarget - xThreshold)*sensor_x_calibration){
					left_motor_set_speed(MOTOR_OPTIMAL_SPEED); //CW rotation
				}else if(get_prox(SENSOR_X) > (xTarget + xThreshold)*sensor_x_calibration){
					left_motor_set_speed(-1*MOTOR_OPTIMAL_SPEED); //CCW rotation
				}else if(get_prox(SENSOR_X) > (xTarget - xThreshold)*sensor_x_calibration && get_prox(SENSOR_X) < (xTarget + xThreshold)*sensor_x_calibration){
					left_motor_set_speed(0); //Stop the rotation
<<<<<<< Updated upstream
				break;
=======
					break;
>>>>>>> Stashed changes
				}
			}

	chThdSleepMilliseconds(250);

<<<<<<< Updated upstream
	//Return the Xstage to the other limit for the Yoffset
	//Move Xmotor CCW, distance 70mm
	left_motor_get_pos();
	int32_t counter_value = 0;
	left_motor_set_pos(counter_value=-500);
=======
	/*
	 * Return the Xstage to the other limit for the Yoffset
	 * Move Xmotor CCW, distance 76mm ~=550 steps
	 */

	left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, -550);
>>>>>>> Stashed changes

	chThdSleepMilliseconds(250);

	//Infinite loop, Y axis
		while(1)
			{
<<<<<<< Updated upstream
				//Returns the last value measured by the Y sensor, (U108) ->U101	//MUST CHECK IF THE SENSORS NUMBERS ARE CORRECTS

				if(get_prox(SENSOR_Y) < (yTarget - yThreshold)*sensor_y_calibration){
					right_motor_set_speed(-1*MOTOR_OPTIMAL_SPEED); //CCW rotation
				}else if(get_prox(SENSOR_Y) > (yTarget + yThreshold)*sensor_y_calibration){
					right_motor_set_speed(MOTOR_OPTIMAL_SPEED); //CW rotation
				}else if(get_prox(SENSOR_Y) > (yTarget - yThreshold)*sensor_y_calibration && get_prox(SENSOR_Y) < (yTarget + yThreshold)*sensor_y_calibration){
=======
				//Returns the last value measured by the Y sensor ->U101
				if(get_prox(SENSOR_Y) < IR_OPTIMAL_DIST) {
					right_motor_set_speed(-MOTOR_OPTIMAL_SPEED); //CCW rotation
				}else if(get_prox(SENSOR_Y) > IR_OPTIMAL_DIST) {
>>>>>>> Stashed changes
					right_motor_set_speed(0); //Stop the rotation
				break;
				}
			}
<<<<<<< Updated upstream

	chThdSleepMilliseconds(250);

	//Return the Xstage to the X offset
	//Move Xmotor CW, distance 76mm
	left_motor_get_pos();
	left_motor_set_pos(counter_value=500); //(Normally, 500 step == 70mm)
	//Find_the_origin function completed
=======
	right_motor_set_pos(0);
	chThdSleepMilliseconds(250);

	/*
	 * Return the Xstage to the X offset
	 * Move Xmotor CW, distance 70mm for a margin of 6 mm
	 */

	left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, 500);
>>>>>>> Stashed changes

	chThdSleepMilliseconds(250);
	}

<<<<<<< Updated upstream
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

=======
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
	motors_init();
=======
    i2c_start();
    imu_start();
>>>>>>> Stashed changes

	FindTheOrigin(uint8_t xTarget, uint8_t xThreshold, uint8_t yTarget, uint8_t yThreshold);

	//Selector choice reading
	int selector = 0;
    selector = get_selector();

    switch (selector)
    {
    /*
     * 		case 0: Off position, nothing to be done
     */
    	case 1: //Draw a square spiral, starting from the external origin, dimensions 70mm x 70mm
    		Drawing_test_func();
			break;
			//WHEN THE FUNCTION IS FINISHED, ACTIVATION OF THE LED SEQUENCE

<<<<<<< Updated upstream
    	case 1: //Free to draw with IMU
    		Drawing_IMU(imu_msg_t *imu_values);
=======
    	case 2: //Free to draw with IMU
    		Drawing_IMU(&imu_values);
>>>>>>> Stashed changes
			break;
			//WHEN THE FUNCTION IS FINISHED, ACTIVATION OF THE LED SEQUENCE

<<<<<<< Updated upstream
    	case 2: //Registered drawing - Epfl Logo
    		Mighty_update(Mighty_sequence[sequence_pos]);
    		sequence_pos++;
    		sequence_pos %= 39;
    		Drawing_Mighty(uint8_t motor_speed_sign, uint16_t counter_value_motor);
=======
    	case 3: //Registered drawing - Epfl Logo
    		Drawing_Mighty();
>>>>>>> Stashed changes
			break;
			//WHEN THE FUNCTION IS FINISHED, ACTIVATION OF THE LED SEQUENCE

    	case 4: //Camera, pattern recognition
    		//get_Process_image
    		//get_PI_regulator
			break;

/*
 * 		case 8: Reserved for the epuck monitor (test mode)
 */

<<<<<<< Updated upstream
    	default: //LED pattern, inactive
    		LEDs_update(seq1[sequence_pos]);
    		sequence_pos++;
    		sequence_pos %= 8;
=======
    	default: //LEDs, inactive
    		set_body_led(1); // value (0=off 1=on higher=inverse)
    		set_front_led(1); //value (0=off 1=on higher=inverse)
>>>>>>> Stashed changes
    		break;
    }

    /* Infinite loop. */
    while (1) {
<<<<<<< Updated upstream
    	//waits 1 second
        chThdSleepMilliseconds(1000);
=======

    	//waits 0.1 second
        chThdSleepMilliseconds(100);
>>>>>>> Stashed changes
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
