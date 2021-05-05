/*
 * Drawing_IMU_function.c
 *
 *  Created on: Apr 22, 2021
 *  Authors: Parma Giuliano & Jacquart Sylvain
 */

//Includes to check
#include <ch.h>
#include <hal.h>
#include <main.h>
#include "usbcfg.h"
#include "chprintf.h"
//#include <messagebus.h>
#include "i2c_bus.h"
//#include "imu.h"
#include "exti.h"
#include <leds.h>

#include <Drawing_IMU_function.h>
#define NB_SAMPLES_OFFSET     200


//static void timer11_start(void){	//USEFULL?
//    //General Purpose Timer configuration
//    //timer 11 is a 16 bit timer so we can measure time
//    //to about 65ms with a 1Mhz counter
//    static const GPTConfig gpt11cfg = {
//        1000000,        /* 1MHz timer clock in order to measure uS.*/
//        NULL,           /* Timer callback.*/
//        0,
//        0
//    };
//}


//** Drawing_IMU function
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

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

static THD_WORKING_AREA(waThdFrontLed, 128);
static THD_FUNCTION(ThdFrontLed, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;

    while(1){
        time = chVTGetSystemTime();
        palTogglePad(GPIOD, GPIOD_LED_FRONT);
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

static THD_WORKING_AREA(waThdBodyLed, 128);
static THD_FUNCTION(ThdBodyLed, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        palTogglePad(GPIOB, GPIOB_LED_BODY);

        /*
        *   1st case :  pause the thread during 500ms
        */
        chThdSleepMilliseconds(500);

        /*
        *   2nd case :  make the thread work during the 500ms
        */

        // //about 500ms at 168MHz
        // for(uint32_t i = 0 ; i < 21000000 ; i++){
        //     __asm__ volatile ("nop");
        // }

        /*
        *   3rd case :  make the thread work during the 500ms
        *               and block the preemption
        */

        // chSysLock();
        // for(uint32_t i = 0 ; i < 21000000 ; i++){
        //     __asm__ volatile ("nop");
        // }
        // chSysUnlock();
    }
}

/* Drawing_IMU function
>>>>>>> Stashed changes
* @brief The user is controlling the pen by tilting the ePuck.
* 		 The IMU controls the function; the speed increases with the increase of the inclination.
* 		 -> Based on the code of the TP 1
*
* @param imu_values -> acceleration (m/s^2)
*/
static imu_msg_t imu_values;

void Drawing_IMU(imu_msg_t *imu_values){
    halInit();
    chSysInit();
    serial_start();
    timer11_start();
    i2c_start();
    imu_start();

	calibrate_acc();
	//get_acc_all();

	/** Inits the Inter Process Communication bus. */
	    messagebus_init(&bus, &bus_lock, &bus_condvar);

	    chThdCreateStatic(waThdFrontLed, sizeof(waThdFrontLed), NORMALPRIO, ThdFrontLed, NULL);
	    chThdCreateStatic(waThdBodyLed, sizeof(waThdBodyLed), NORMALPRIO, ThdBodyLed, NULL);


    //threshold value to not run the motors when the robot is too horizontal
    float threshold = 0.2;
    float ThresholdAngle = M_PI/10; //Should be contained between 0 and less than PI/2
    //create a pointer to the array for shorter name
    float *accel = imu_values->acceleration;
    //variable to measure the time some functions take
    //volatile to not be optimized out by the compiler if not used
    volatile uint16_t time = 0;

    int IMU_drawing_speed;

    //Variable speed of the motors, depending of the IMU accelerations.
    //The pen draws faster when the inclination increases.
    void IMU_drawing_variable_speed(){
    	uint32_t imu_max_axis_accel;
    	uint32_t global_max_accel;
    	if(fabs(accel[X_AXIS]) > fabs(accel[Y_AXIS]) && fabs(accel[X_AXIS]) > fabs(accel[Z_AXIS])){
    		imu_max_axis_accel=fabs(accel[X_AXIS]);
    	}else if(fabs(accel[Y_AXIS]) > fabs(accel[X_AXIS]) && fabs(accel[Y_AXIS]) > fabs(accel[Z_AXIS])){
    		imu_max_axis_accel=fabs(accel[Y_AXIS]);
    	}else if(fabs(accel[Z_AXIS]) > fabs(accel[X_AXIS]) && fabs(accel[Z_AXIS]) > fabs(accel[Y_AXIS])){
    		imu_max_axis_accel=fabs(accel[Z_AXIS]);

    	global_max_accel = 16; //Datasheet max scale: +-16g TO VERIFY
    	IMU_drawing_speed = (MOTOR_OPTIMAL_SPEED+imu_max_axis_accel/global_max_accel*(MOTOR_SPEED_LIMIT-MOTOR_OPTIMAL_SPEED));

    	}
    }

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
//}

//}




