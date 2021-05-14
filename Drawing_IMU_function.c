/*
 * Drawing_IMU_function.c
 *
 *  Created on: Apr 22, 2021
 *  Authors: Parma Giuliano & Jacquart Sylvain
 */
#include <ch.h>
#include <hal.h>
#include <main.h>
#include "usbcfg.h"
#include "chprintf.h"
#include "i2c_bus.h"
#include "exti.h"
#include <leds.h>

#include <sensors/imu.h>
#include <Drawing_IMU_function.h>
#include <Restart_Programm.h>


//** Drawing_IMU function			//Already define in the main
//messagebus_t bus;
//MUTEX_DECL(bus_lock);
//CONDVAR_DECL(bus_condvar);

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

static void timer11_start(void){		//JUST COMMENTED
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

/*
 * Mandatory to define the global imu_values, to avoid the "unspecified variable" error.
 */
static imu_msg_t imu_values;

/* Drawing_IMU function
* @brief The user is controlling the pen by tilting the ePuck.
* 		 The IMU controls the function; the speed increases with the increase of the inclination.
* 		 -> Based on the code of the TP 1
*
* @param imu_values -> acceleration (m/s^2)
*/
static THD_WORKING_AREA(waThdDrawing_IMU, 1024);
static THD_FUNCTION(ThdDrawing_IMU, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    /** Inits the Inter Process Communication bus. */
    			//** Drawing_IMU function		//JUST ADDED
//    			messagebus_t bus;
//    			MUTEX_DECL(bus_lock);
//    			CONDVAR_DECL(bus_condvar);

//        	    messagebus_init(&bus, &bus_lock, &bus_condvar);
//        	    imu_start();

        	    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
        	    imu_msg_t imu_values;

        	    //wait 2 sec to be sure the e-puck is in a stable position
        	    chThdSleepMilliseconds(2000);
        	    imu_compute_offset(imu_topic, NB_SAMPLES_OFFSET);

    while(1){
    	chBSemWait(&Drawing_IMU_start_sem);
//    	if(get_selector() == 2){
		//wait for new measures to be published
    		        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));
    		        Drawing_IMU(&imu_values);
    		        chThdSleepMilliseconds(100);
//    	}else{

    	}
    chThdSleepMilliseconds(250);
    }
//}

void Drawing_IMU(imu_msg_t *imu_values){

	/** Inits the Inter Process Communication bus. */
	 //   messagebus_init(&bus, &bus_lock, &bus_condvar);

    //threshold value to not run the motors when the robot is too horizontal
    float threshold = 0.4;
    float ThresholdAngle = PI/10;//Should be contained between 0 and less than PI/2			//M_PI before
    //create a pointer to the array for shorter name
    float *accel = imu_values->acceleration;
    //variable to measure the time some functions take
    //volatile to not be optimized out by the compiler if not used
    volatile uint16_t time = 0;

//    int IMU_drawing_speed;
    int IMU_drawing_speed=MOTOR_OPTIMAL_SPEED*2;

    //Variable speed of the motors, depending of the IMU accelerations.
    //The pen draws faster when the inclination increases.
//    void IMU_drawing_variable_speed(){
//    	uint32_t imu_max_axis_accel;
//    	uint32_t global_max_accel;
//    	if(fabs(accel[X_AXIS]) > fabs(accel[Y_AXIS]) && fabs(accel[X_AXIS]) > fabs(accel[Z_AXIS])){
//    		imu_max_axis_accel=fabs(accel[X_AXIS]);
//    	}else if(fabs(accel[Y_AXIS]) > fabs(accel[X_AXIS]) && fabs(accel[Y_AXIS]) > fabs(accel[Z_AXIS])){
//    		imu_max_axis_accel=fabs(accel[Y_AXIS]);
//    	}else if(fabs(accel[Z_AXIS]) > fabs(accel[X_AXIS]) && fabs(accel[Z_AXIS]) > fabs(accel[Y_AXIS])){
//    		imu_max_axis_accel=fabs(accel[Z_AXIS]);
//
//    	global_max_accel = 16; //Datasheet max scale: +-16g TO VERIFY
//    	IMU_drawing_speed = (MOTOR_OPTIMAL_SPEED+imu_max_axis_accel/global_max_accel*(MOTOR_SPEED_LIMIT-MOTOR_OPTIMAL_SPEED));
//
// 	}
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
        angle += PI/4;			//M_PI

        //if the angle is greater than PI, then it has shifted on the -PI side of the quadrant
        //so we correct it
        //Note : CW == Clockwise, CCW == Counter Clockwise
        if(angle > PI){ //M_PI
            angle = -2 * PI + angle; //M_PI
        }

        //Select the motor(s) to move depending of the angle value
        if(angle > -ThresholdAngle && angle < ThresholdAngle){										//@1
        	left_motor_set_speed(MOTOR_NO_SPEED); //X motor -> Static
        	right_motor_set_speed(IMU_drawing_speed);//Y motor -> CW direction

        }else if(angle >= ThresholdAngle && angle < (PI/2 - ThresholdAngle)){		//M_PI				//@2
        	left_motor_set_speed(-IMU_drawing_speed);//X motor -> CCW direction
        	right_motor_set_speed(IMU_drawing_speed);//Y motor -> CW direction

        }else if(angle >= (PI/2 - ThresholdAngle) && angle < (PI/2 + ThresholdAngle)){			//@3
        	left_motor_set_speed(-IMU_drawing_speed);//X motor -> CCW direction
        	right_motor_set_speed(MOTOR_NO_SPEED); ////Y motor -> Static

        }else if(angle >= (PI/2 + ThresholdAngle) && angle < (PI - ThresholdAngle)){			//@4
        	left_motor_set_speed(-IMU_drawing_speed);//X motor -> CCW direction
        	right_motor_set_speed(-IMU_drawing_speed);//Y motor -> CCW direction

        }else if(angle >= (PI - ThresholdAngle) && angle < (-PI + ThresholdAngle)){				//@5
        	left_motor_set_speed(MOTOR_NO_SPEED); //X motor -> Static
        	right_motor_set_speed(-IMU_drawing_speed);//Y motor -> CCW direction

        }else if(angle >= (-PI + ThresholdAngle) && angle < (-PI/2 - ThresholdAngle)){			//@6
        	left_motor_set_speed(IMU_drawing_speed);//X motor -> CW direction
        	right_motor_set_speed(-IMU_drawing_speed);//Y motor -> CCW direction

        }else if(angle >= (-PI/2 - ThresholdAngle) && angle < (-PI/2 + ThresholdAngle)){		//@7
        	left_motor_set_speed(IMU_drawing_speed);//X motor -> CW direction
        	right_motor_set_speed(MOTOR_NO_SPEED); ////Y motor -> Static

        }else if(angle >= (-PI/2 + ThresholdAngle) && angle < -ThresholdAngle){					//@8
        	left_motor_set_speed(MOTOR_OPTIMAL_SPEED);//X motor -> CW direction
        	right_motor_set_speed(MOTOR_OPTIMAL_SPEED);//Y motor -> CW direction
        }else if(fabs(accel[X_AXIS]) < threshold || fabs(accel[Y_AXIS]) < threshold){				//Stop
        	left_motor_set_speed(MOTOR_NO_SPEED); ////Y motor -> Static
        	right_motor_set_speed(MOTOR_NO_SPEED); ////Y motor -> Static
        }
    }
}

void Drawing_IMU_start(void){
	chThdCreateStatic(waThdDrawing_IMU, sizeof(waThdDrawing_IMU), NORMALPRIO+1, ThdDrawing_IMU, NULL);
}
