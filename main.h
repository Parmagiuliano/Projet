#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"
#include "motors.h"
#include "sensors/proximity.h"
#include "sensors/imu.h"

#include <Drawing_test_function.h>
#include <process_image.h>

#include <Restart_Programm.h>
#include <Constants.h>

//constants for the differents parts of the project		//TO CHECK
//#define IMAGE_WIDTH				96
//#define WIDTH_SLOPE				2
//#define MIN_LINE_WIDTH			70
//#define ROTATION_THRESHOLD		10
//#define ROTATION_COEFF			2
//#define PXTOCM					1570.0f //experimental value
//#define GOAL_DISTANCE 			10.0f
//#define MAX_DISTANCE 			25.0f
//#define MAX_POINTS				81
//#define RESOLUTION				9
//#define SHIFT					5
//
//#define ERROR_THRESHOLD			0.1f	//[cm] because of the noise of the camera
//#define KP						800.0f
//#define KI 						3.5f	//must not be zero
//#define MAX_SUM_ERROR 			(MOTOR_SPEED_LIMIT/KI)
//#define IMAGE_BUFFER_SIZE		10
//
//#define SENSOR_X				2
//#define SENSOR_Y				0
//#define IR_OPTIMAL_DIST			400
//
//#define PI						3.14159
//
//#define NB_SAMPLES_OFFSET     200
//
//#define MOTOR_SPEED_LIMIT 				1100 	// [step/s]
//#define MOTOR_OPTIMAL_SPEED 			150 	// [step/s]
//#define MOTOR_NO_SPEED 					0 		// [step/s]
//#define DRAWING_CST_MIGHTY				7		//Step multiplier		//14
//#define STEP							168		//Step constant at MOTOR_OPTIMAL_SPEED

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);
static void serial_start(void);
void __stack_chk_fail(void);


#ifdef __cplusplus
}
#endif

#endif
