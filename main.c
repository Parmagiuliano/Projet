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

#include <process_image.h>
//#include <Drawing_test_function.h>
//#include <Drawing_IMU_function.h>
//#include <Mighty_logo_function.h>

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

/** Finding the origin function @In detail
 *
 * By convention for this project, LEFT_MOTOR = X_MOTOR & RIGHT_MOTOR = Y_MOTOR
 *
 * Unit conversion: 1 step motor = 0.1382 mm with GearRadius R = 22mm.
 * At the speed of 300 rpm, with 1 wheel turn =1'000 steps, it takes 0.2 milliseconds by step.
 *
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

static void FindTheOrigin(void)
	{
		//Variables declaration
//		uint8_t xTarget = 130;				//%
//		uint8_t xThreshold = 10;			//%
//		uint8_t yTarget = 150;				//%
//		uint8_t yThreshold = 10;			//%
//		uint8_t ir_sampling_number = 10;
//		uint16_t sensor_x_calibration_avg;
//		uint16_t sensor_y_calibration_avg;

		//Starts the proximity measurement module
		proximity_start();
		//Runs the IR sensor calibration process
		calibrate_ir();

//		//Set a reference value for the sensors by average
//		int32_t sumx = 0;
//		int32_t sumy = 0;
//		for(uint8_t j = 0; j < ir_sampling_number; j++){
//			int i = get_calibrated_prox(SENSOR_X); 			//from the epuck guide, get_calibrated_prox_value=0...4'095
//			sumx += i;
//		}
//		sensor_x_calibration_avg = sumx / ir_sampling_number;
//
//		for(uint8_t j = 0; j < ir_sampling_number; j++){
//			int i=get_calibrated_prox(SENSOR_Y);
//			sumy += i;
//		}
//		sensor_y_calibration_avg = sumy / ir_sampling_number;

		//Infinite loop, X axis
		while(1)
			{
				//Returns the last value measured by the X sensor -> U106
				if(get_prox(SENSOR_X) < IR_OPTIMAL_DIST){
					left_motor_set_speed(MOTOR_OPTIMAL_SPEED); //CW rotation
				}else if(get_prox(SENSOR_X) > IR_OPTIMAL_DIST){
					left_motor_set_speed(0); //Stop the rotation
				}

//				if(get_prox(SENSOR_X) < (xTarget/100 - xThreshold/100)*sensor_x_calibration_avg){
//					left_motor_set_speed(MOTOR_OPTIMAL_SPEED); //CW rotation
//				}else if(get_prox(SENSOR_X) > (xTarget/100 + xThreshold/100)*sensor_x_calibration_avg){
//					left_motor_set_speed(-1*MOTOR_OPTIMAL_SPEED); //CCW rotation
//				}else if(get_prox(SENSOR_X) > (xTarget/100 - xThreshold/100)*sensor_x_calibration_avg && get_prox(SENSOR_X) < (xTarget/100 + xThreshold/100)*sensor_x_calibration_avg){
//					left_motor_set_speed(0); //Stop the rotation
//				break;
//				}
			}
	left_motor_set_pos(0);
	chThdSleepMilliseconds(250);

	//Return the Xstage to the other limit for the Yoffset
	//Move Xmotor CCW, distance 76mm ~=550 steps
	left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, -550);

	chThdSleepMilliseconds(250);

	//Infinite loop, Y axis
		while(1)
			{
				//Returns the last value measured by the Y sensor, (U108) ->U101	//MUST CHECK IF THE SENSORS NUMBERS ARE CORRECTS

				if(get_prox(SENSOR_Y) < IR_OPTIMAL_DIST) {
					right_motor_set_speed(-MOTOR_OPTIMAL_SPEED); //CCW rotation
				}else if(get_prox(SENSOR_Y) > IR_OPTIMAL_DIST) {
					right_motor_set_speed(0); //Stop the rotation
					break;
				}

//				if(get_prox(SENSOR_Y) < (yTarget/100 - yThreshold/100)*sensor_y_calibration_avg){
//					right_motor_set_speed(-1*MOTOR_OPTIMAL_SPEED); //CCW rotation
//				}else if(get_prox(SENSOR_Y) > (yTarget/100 + yThreshold/100)*sensor_y_calibration_avg){
//					right_motor_set_speed(MOTOR_OPTIMAL_SPEED); //CW rotation
//				}else if(get_prox(SENSOR_Y) > (yTarget/100 - yThreshold/100)*sensor_y_calibration_avg && get_prox(SENSOR_Y) < (yTarget/100 + yThreshold/100)*sensor_y_calibration_avg){
//					right_motor_set_speed(0); //Stop the rotation
//				break;
//				}
			}

	right_motor_set_pos(0);
	chThdSleepMilliseconds(250);

	//Return the Xstage to the X offset
	//Move Xmotor CW, distance 70mm for a margin of 6 mm
	left_motor_get_to_the_pos(MOTOR_OPTIMAL_SPEED, 500);

	chThdSleepMilliseconds(250);
	}

/* Drawing test function @In detail
 * Aimed pattern for the square spiral:
 *1 left_motor CW 70
 *2 rigth_motor CCW 70
 *3 left_motor CCW 70
 *4 right_motor CW 65
 *5 left_motor CW 65
 *6 right_motor CCW 65
 *7 left_motor CCW 60
 *8 right_motor CW 60
 *9 left_motor CW 60
 * ... until
 *40 right_motor CW 5
 *
 *  --------------------X
 * 	¦
 * 	¦    -----------
 * 	¦    ¦         ¦
 * 	¦    v         ¦
 * 	----------------
 */

/* Drawing_test function @brief
*  		 Draw a square spiral, starting from the external origin, dimensions 70mm x 70mm.
* 		 Every change (X/Y motor, motor direction & lenght of lines) are controlled by the modulo of the increment for loop.
*
* @param motor_speed_sign		+-1, based on the modulo of the for loop increment
* 		 counter_value_motor	value set at 507 ~=70cm, decrease by step of 36 ~=5cm
*/

static void Drawing_test_func(void)
{
	int8_t motor_speed_sign = 1;
	uint16_t counter_value_motor = 507; //507 steps ~= 70cm

	//Set the origin for the drawing_test_func
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	uint8_t i;
	for(i = 1; i < 41; i++)
 	 {
	 	 //Using the modulo 4 to change the sens of motors in the for loop
	 	 if(i % 4 == 0 || i % 4 == 1)
	 	 {
	 		 motor_speed_sign = 1;
	 	 }
	 	 else if (i % 4 == 2 && i % 4 == 3)
	 	 {
	 		 motor_speed_sign = -1;
	 	 }

	 	 //Using the modulo 3 to decrease the length of lines over time.
	 	 if(i % 3 == 1 && i != 1)
	 	 {
	 		 counter_value_motor = counter_value_motor - 36; //36 steps ~= 5cm
	 	 }
//	 	 else if (i % 3 != 1)
//	 	 {
//	 		 counter_value_motor = counter_value_motor;	//No changes
//	 	 }

	 	 //Using the modulo 2 to choose which motor should run.
	 	 if (i % 2 == 0)
	 	 {
	 		 right_motor_get_to_the_pos(motor_speed_sign*MOTOR_OPTIMAL_SPEED, counter_value_motor);
	 	 }
	 	 else if (i % 2 != 0)
	 	 {
	 		 left_motor_get_to_the_pos(motor_speed_sign*MOTOR_OPTIMAL_SPEED, counter_value_motor);
	 	 }

	chThdSleepMilliseconds(250);
	//return 0;
 	 }
}

/* Drawing_IMU function
* @brief The user is controlling the pen by tilting the ePuck.
* 		 The IMU controls the function; the speed increases with the increase of the inclination.
* 		 -> Based on the code of the TP 1
*
* @param imu_values -> acceleration (m/s^2)
*/

void Drawing_IMU(imu_msg_t *imu_values){
	imu_start();
	calibrate_acc();
	//get_acc_all();

    //threshold value to not run the motors when the robot is too horizontal
    float threshold = 0.2;
    float ThresholdAngle = M_PI/10; //Should be contained between 0 and less than PI/2
    //create a pointer to the array for shorter name
    float *accel = imu_values->acceleration;
    //variable to measure the time some functions take
    //volatile to not be optimized out by the compiler if not used
    volatile uint16_t time = 0;

    //Variable speed of the motors, depending of the IMU accelerations.
    //The pen draws faster when the inclination increases.
    //void IMU_drawing_variable_speed(){
    	uint32_t imu_max_axis_accel = 0;
    	uint32_t global_max_accel = 16;		//Datasheet max scale: +-16g TO VERIFY
    	if(fabs(accel[X_AXIS]) > fabs(accel[Y_AXIS]) && fabs(accel[X_AXIS]) > fabs(accel[Z_AXIS])){
    		imu_max_axis_accel=fabs(accel[X_AXIS]);
    	}else if(fabs(accel[Y_AXIS]) > fabs(accel[X_AXIS]) && fabs(accel[Y_AXIS]) > fabs(accel[Z_AXIS])){
    		imu_max_axis_accel=fabs(accel[Y_AXIS]);
    	}else if(fabs(accel[Z_AXIS]) > fabs(accel[X_AXIS]) && fabs(accel[Z_AXIS]) > fabs(accel[Y_AXIS])){
    		imu_max_axis_accel=fabs(accel[Z_AXIS]);
    	}
    	//int IMU_drawing_speed = (MOTOR_OPTIMAL_SPEED+imu_max_axis_accel/global_max_accel*(MOTOR_SPEED_LIMIT-MOTOR_OPTIMAL_SPEED));
    	int IMU_drawing_speed = MOTOR_OPTIMAL_SPEED;
    //}

    /*
    * Quadrants & directions:
    *
    *	       BACK
    *	        @1
    *	       ####
    * @8	#    0   #	  @2
    *	  #            #
    *@7  #-PI/2 TOP PI/2#	@3
    *    #     VIEW     #
    *     #            #
    * @6    # -PI|PI #	  @4
    * 	       ####
    *      		@5
    *         FRONT
    */

    if(fabs(accel[X_AXIS]) > threshold || fabs(accel[Y_AXIS]) > threshold){

        chSysLock();
        //reset the timer counter
        GPTD11.tim->CNT = 0;
        //clock wise angle in rad with 0 being the back of the e-puck2 (Y axis of the IMU)
        float angle = atan2(accel[X_AXIS], accel[Y_AXIS]);
        //by reading time with the debugger, we can know the computational time of atan2 function
        time = GPTD11.tim->CNT;
        chSysUnlock();

        //rotates the angle by 45 degrees (simpler to compare with PI and PI/2 than with 5*PI/4)
        angle += M_PI/4;

        //if the angle is greater than PI, then it has shifted on the -PI side of the quadrant
        //so we correct it
        //Note : CW == Clockwise, CCW == Counter Clockwise
        if(angle > M_PI){
            angle = -2 * M_PI + angle;
        }

        //Select the motor(s) to move depending of the angle value
        if(angle > -ThresholdAngle && angle < ThresholdAngle){										//@1
        	//X motor -> Static
        	right_motor_set_speed(IMU_drawing_speed);//Y motor -> CW direction

        }else if(angle >= ThresholdAngle && angle < (M_PI/2 - ThresholdAngle)){						//@2
        	left_motor_set_speed(IMU_drawing_speed);//X motor -> CW direction
        	right_motor_set_speed(IMU_drawing_speed);//Y motor -> CW direction

        }else if(angle >= (M_PI/2 - ThresholdAngle) && angle < (M_PI/2 + ThresholdAngle)){			//@3
        	left_motor_set_speed(IMU_drawing_speed);//X motor -> CW direction
        	//Y motor -> Static

        }else if(angle >= (M_PI/2 + ThresholdAngle) && angle < (M_PI - ThresholdAngle)){			//@4
        	left_motor_set_speed(IMU_drawing_speed);//X motor -> CW direction
        	right_motor_set_speed(-IMU_drawing_speed);//Y motor -> CCW direction

        }else if(angle >= (M_PI - ThresholdAngle) && angle < (-M_PI + ThresholdAngle)){				//@5
        	//X motor -> Static
        	right_motor_set_speed(-IMU_drawing_speed);//Y motor -> CCW direction

        }else if(angle >= (-M_PI + ThresholdAngle) && angle < (-M_PI/2 - ThresholdAngle)){			//@6
        	left_motor_set_speed(-IMU_drawing_speed);//X motor -> CCW direction
        	right_motor_set_speed(-IMU_drawing_speed);//Y motor -> CCW direction

        }else if(angle >= (-M_PI/2 - ThresholdAngle) && angle < (-M_PI/2 + ThresholdAngle)){		//@7
        	left_motor_set_speed(-IMU_drawing_speed);//X motor -> CCW direction
        	//Y motor -> Static

        }else if(angle >= (-M_PI/2 + ThresholdAngle) && angle < -ThresholdAngle){					//@8
        	left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);//X motor -> CCW direction
        	right_motor_set_speed(MOTOR_OPTIMAL_SPEED);//Y motor -> CW direction
        }

        /*
         * Eventually, track the position to avoid any break of the system.
         * If the IMU value would bring the pen out of the 70x70 square (+-margin), stop the IMU leading instruction.
         */

    	}

    }



int main(void)
{

    //start the system
    halInit();
    chSysInit();
    mpu_init();

    //Some initializations
    serial_start();
    usb_start();
    dcmi_start();
    timer11_start();
	po8030_start();
	motors_init();
    i2c_start();
    imu_start();

	//Set the pen to the origin before the draw
	FindTheOrigin();

	//Selector choice reading
	int selector;
    selector = get_selector();

    //unsigned int value = 1;
    switch (selector)
    {
    	case 0: //Draw a square spiral, starting from the external origin, dimensions 70mm x 70mm
    		Drawing_test_func();
			break;

    	case 1: //Free to draw with IMU
//    	    /** Inits the Inter Process Communication bus. */
//    	    messagebus_init(&bus, &bus_lock, &bus_condvar);
//
//    	    chThdCreateStatic(waThdFrontLed, sizeof(waThdFrontLed), NORMALPRIO, ThdFrontLed, NULL);
//    	    chThdCreateStatic(waThdBodyLed, sizeof(waThdBodyLed), NORMALPRIO, ThdBodyLed, NULL);
//
//    	    //to change the priority of the thread invoking the function. The main function in this case
//    	    //chThdSetPriority(NORMALPRIO+2);
//
//    	    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
//    	    imu_msg_t imu_values;
//
//    	    //wait 2 sec to be sure the e-puck is in a stable position
//    	    chThdSleepMilliseconds(2000);
//    	    imu_compute_offset(imu_topic, NB_SAMPLES_OFFSET);

    		Drawing_IMU(&imu_values);
    		//imu_msg_t *imu_values
			break;

    	case 2: //Registered drawing - Epfl Logo
//    		Mighty_update(Mighty_sequence[sequence_pos]);
//    		sequence_pos++;
//    		sequence_pos %= 39;
    		Drawing_Mighty();
			break;

    	case 3: //Camera, pattern recognition		//SHOULD BE MADE IN 2 SELECTOR FUNC; READ THEN DRAW
    		//get_Process_image
    		//get_PI_regulator
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

    	default: //LEDs, inactive

    		set_body_led(1); // value (0=off 1=on higher=inverse)
    		set_front_led(1); //value (0=off 1=on higher=inverse)
    		break;
    }

    /* Infinite loop. */
    while (1) {

//        //wait for new measures to be published
//        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));
//        //prints raw values
//        chprintf((BaseSequentialStream *)&SD3, "%Ax=%-7d Ay=%-7d Az=%-7d Gx=%-7d Gy=%-7d Gz=%-7d\r\n",
//                imu_values.acc_raw[X_AXIS], imu_values.acc_raw[Y_AXIS], imu_values.acc_raw[Z_AXIS],
//                imu_values.gyro_raw[X_AXIS], imu_values.gyro_raw[Y_AXIS], imu_values.gyro_raw[Z_AXIS]);
//
//        //prints raw values with offset correction
//        chprintf((BaseSequentialStream *)&SD3, "%Ax=%-7d Ay=%-7d Az=%-7d Gx=%-7d Gy=%-7d Gz=%-7d\r\n",
//                imu_values.acc_raw[X_AXIS]-imu_values.acc_offset[X_AXIS],
//                imu_values.acc_raw[Y_AXIS]-imu_values.acc_offset[Y_AXIS],
//                imu_values.acc_raw[Z_AXIS]-imu_values.acc_offset[Z_AXIS],
//                imu_values.gyro_raw[X_AXIS]-imu_values.gyro_offset[X_AXIS],
//                imu_values.gyro_raw[Y_AXIS]-imu_values.gyro_offset[Y_AXIS],
//                imu_values.gyro_raw[Z_AXIS]-imu_values.gyro_offset[Z_AXIS]);
//
//        //prints values in readable units
//        chprintf((BaseSequentialStream *)&SD3, "%Ax=%.2f Ay=%.2f Az=%.2f Gx=%.2f Gy=%.2f Gz=%.2f (%x)\r\n\n",
//                imu_values.acceleration[X_AXIS], imu_values.acceleration[Y_AXIS], imu_values.acceleration[Z_AXIS],
//                imu_values.gyro_rate[X_AXIS], imu_values.gyro_rate[Y_AXIS], imu_values.gyro_rate[Z_AXIS],
//                imu_values.status);
    	//waits 0.1 second
        chThdSleepMilliseconds(100);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
