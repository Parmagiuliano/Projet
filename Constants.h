/*
 * Constants.h
 *
 *  Created on: May 14, 2021
 *      Author: sjacq
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define IMAGE_WIDTH				96
#define WIDTH_SLOPE				2
#define MIN_LINE_WIDTH			70
#define ROTATION_THRESHOLD		10
#define ROTATION_COEFF			2
#define PXTOCM					1570.0f //experimental value
#define GOAL_DISTANCE 			10.0f
#define MAX_DISTANCE 			25.0f
#define MAX_POINTS				81
#define RESOLUTION				9
#define SHIFT					5

#define ERROR_THRESHOLD			0.1f	//[cm] because of the noise of the camera
#define MAX_SUM_ERROR 			(MOTOR_SPEED_LIMIT/KI)
#define IMAGE_BUFFER_SIZE		10

#define SENSOR_X				2
#define SENSOR_Y				0
#define IR_OPTIMAL_DIST			400

#define PI						3.14159

#define NB_SAMPLES_OFFSET     200

//#define MOTOR_SPEED_LIMIT 				1100 	// [step/s]
#define MOTOR_OPTIMAL_SPEED 			150 	// [step/s]
#define MOTOR_NO_SPEED 					0 		// [step/s]
#define DRAWING_CST_MIGHTY				7		//Step multiplier		//14
#define STEP							400	//Step constant at MOTOR_OPTIMAL_SPEED



#endif /* CONSTANTS_H_ */
