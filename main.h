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


//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE		640
#define WIDTH_SLOPE				5
#define MIN_LINE_WIDTH			40
#define ROTATION_THRESHOLD		10
#define ROTATION_COEFF			2 
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE 			10.0f
#define MAX_DISTANCE 			25.0f
//#define ERROR_THRESHOLD			0.1f	//[cm] because of the noise of the camera
//#define KP						800.0f
//#define KI 						3.5f	//must not be zero
//#define MAX_SUM_ERROR 			(MOTOR_SPEED_LIMIT/KI)
#define SENSOR_X				3
#define SENSOR_Y				1

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);
static void serial_start(void);
void FindTheOrigin(uint8_t xTarget, uint8_t xThreshold, uint8_t yTarget, uint8_t yThreshold);

#ifdef __cplusplus
}
#endif

#endif
