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

//constants for the differents parts of the project		//TO CHECK
#define IMAGE_BUFFER_SIZE		640
#define WIDTH_SLOPE				5
#define MIN_LINE_WIDTH			40
#define ROTATION_THRESHOLD		10
#define ROTATION_COEFF			2 
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE 			10.0f
#define MAX_DISTANCE 			25.0f

#define SENSOR_X				2
#define SENSOR_Y				0
#define IR_OPTIMAL_DIST			400

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

//functions in the main.c

void SendUint8ToComputer(uint8_t* data, uint16_t size);
static void serial_start(void);
static void FindTheOrigin(void);
static void Drawing_test_func(void);
void Drawing_IMU(imu_msg_t *imu_values);
void Drawing_Mighty();



#ifdef __cplusplus
}
#endif

#endif
