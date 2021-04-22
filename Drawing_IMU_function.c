/*
 * Drawing_IMU_function.c
 *
 *  Created on: Apr 22, 2021
 *      Author: sjacq
 */
#define NB_SAMPLES_OFFSET     200

//messagebus_t bus;
//MUTEX_DECL(bus_lock);
//CONDVAR_DECL(bus_condvar);

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

imu_start();
get_acc_offset();


void show_gravity(imu_msg_t *imu_values){

    //we create variables for the led in order to turn them off at each loop and to
    //select which one to turn on
    uint8_t led1 = 0, led3 = 0, led5 = 0, led7 = 0;
    //threshold value to not use the leds when the robot is too horizontal
    float threshold = 0.2;
    float ThresholdAngle = M_PI/10; //Should be contained between 0 and less than PI/2
    //create a pointer to the array for shorter name
    float *accel = imu_values->acceleration;
    //variable to measure the time some functions take
    //volatile to not be optimized out by the compiler if not used
    volatile uint16_t time = 0;

    /*
    * Quadrant:
    *
    *       BACK
    *       ####
    *    #    0   #
    *  #            #
    * #-PI/2 TOP PI/2#
    * #      VIEW    #
    *  #            #
    *    # -PI|PI #
    *       ####
    *       FRONT
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
        if(angle > -ThresholdAngle && angle < ThresholdAngle){
        	//X motor -> Static
        	//Y motor -> CW direction
        }else if(angle >= ThresholdAngle && angle < (M_PI/2 - ThresholdAngle)){
        	//X motor -> CW direction
        	//Y motor -> CW direction
        }else if(angle >= (M_PI/2 - ThresholdAngle) && angle < (M_PI/2 + ThresholdAngle)){
        	//X motor -> CW direction
        	//Y motor -> Static
        }else if(angle >= (M_PI/2 + ThresholdAngle) && angle < (M_PI - ThresholdAngle)){
        	//X motor -> CW direction
        	//Y motor -> CCW direction
        }else if(angle >= (M_PI - ThresholdAngle) && angle < (-M_PI + ThresholdAngle)){
        	//X motor -> Static
        	//Y motor -> CCW direction
        }else if(angle >= (-M_PI + ThresholdAngle) && angle < (-M_PI/2 - ThresholdAngle)){
        	//X motor -> CCW direction
        	//Y motor -> CCW direction
        }else if(angle >= (-M_PI/2 - ThresholdAngle) && angle < (-M_PI/2 + ThresholdAngle)){
        	//X motor -> CCW direction
        	//Y motor -> Static
        }else if(angle >= (-M_PI/2 + ThresholdAngle) && angle < -ThresholdAngle){
        	//X motor -> CCW direction
        	//Y motor -> CW direction
        }




        if(angle >= 0 && angle < M_PI/2){
        	led5 = 1;
        }else if(angle >= M_PI/2 && angle < M_PI){
            led7 = 1;
        }else if(angle >= -M_PI && angle < -M_PI/2){
            led1 = 1;
        }else if(angle >= -M_PI/2 && angle < 0){
            led3 = 1;
        }
    }
}



