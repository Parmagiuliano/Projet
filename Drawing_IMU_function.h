/*
 * Drawing_IMU_function.h
 *
 *  Created on: Apr 22, 2021
 *  Authors: Parma Giuliano & Jacquart Sylvain
 */
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"
#include "sensors/proximity.h"
#include "sensors/imu.h"

#ifndef DRAWING_IMU_FUNCTION_H_
#define DRAWING_IMU_FUNCTION_H_

static void serial_start(void);
static void timer11_start(void);
void Drawing_IMU(imu_msg_t *imu_values);
void IMU_drawing_variable_speed();


#endif /* DRAWING_IMU_FUNCTION_H_ */
