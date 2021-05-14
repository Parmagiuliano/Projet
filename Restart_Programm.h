/*
 * Restart_Programm.h
 *
 *  Created on: May 13, 2021
 *      Author: sjacq
 */

#ifndef RESTART_PROGRAMM_H_
#define RESTART_PROGRAMM_H_

#include <selector.h>
#include "sensors/proximity.h"
#include "motors.h"
#include <chbsem.h>
#include <main.h>

#include <stdbool.h>


    static BSEMAPHORE_DECL(process_image_start_sem, TRUE);
    //chBSemWait(&process_image_start_sem);
    //chBSemSignal(&process_image_start_sem);

    static BSEMAPHORE_DECL(Drawing_functions_start_sem, TRUE);
    //chBSemWait(&Drawing_functions_start_sem);
    //chBSemSignal(&Drawing_functions_start_sem);

    static BSEMAPHORE_DECL(Drawing_IMU_start_sem, TRUE);
    //chBSemWait(&Drawing_IMU_start_sem);
    //chBSemSignal(&Drawing_IMU_start_sem);

void FindTheOrigin(void);
void Restart_Programm_start(void);



#endif /* RESTART_PROGRAMM_H_ */
