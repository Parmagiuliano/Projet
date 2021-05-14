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
#include <Drawing_test_function.h>
#include <Drawing_IMU_function.h>
#include <process_image.h>
#include <Restart_Programm.h>

//Related to IMU, must be defined right after the includes
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

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

	/** Inits the Inter Process Communication bus. */
	messagebus_init(&bus, &bus_lock, &bus_condvar);
	/** The IMU must start after the messagebus init. */
	imu_start();

	//Starts the proximity measurement module
	proximity_start();
	//Runs the IR sensor calibration process
	calibrate_ir();

   // Finding the origin function
//	FindTheOrigin();

    process_image_start();
    Drawing_functions_start();
    Drawing_IMU_start();
    Restart_Programm_start();


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
