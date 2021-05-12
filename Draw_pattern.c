/*
 * Draw_pattern.c
 *
 *  Created on: May 12, 2021
 *      Author: sjacq
 */
//static THD_WORKING_AREA(waThdSwitch_Selector, 128);
#include <process_image.h>
#include <main.h>
#include <Draw_pattern.h>
#include <motors.h>

static THD_WORKING_AREA(waThdDrawPattern, 128);
static THD_FUNCTION(ThdDrawPattern, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;


    while(1){
    	uint8_t *drawing;
    	uint8_t points [MAX_POINTS];
		uint8_t points_counter = 0, indice = 0;

    	if(get_draw_start ()){
    		drawing = get_points_location();
    		    	for(uint8_t i =0; i < (MAX_POINTS) ; i++){
    		    							points [i] = ((uint8_t)drawing [i]);
    		    							points_counter += points [i];
    		    						}
    		    	//loocking for an origin
    		    	for(uint8_t i =0; i < (MAX_POINTS) ; i++){
    		    				if(points[i]){
    		    					if(i/RESOLUTION >= i%RESOLUTION){	//Reach for the starting point of the pattern
    		    						//set speed diagonale
    		    						//wait
    		    						left_motor_set_speed((i%RESOLUTION)/(i/RESOLUTION)*MOTOR_OPTIMAL_SPEED); //X
    		    						right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);	//Y
    		    						chThdSleepMilliseconds(STEP*i/RESOLUTION);	//Move of Y pas
    		    					}
    		    					else{
    		    						//set speed diagonale
    		    						//wait
    		    						left_motor_set_speed(MOTOR_OPTIMAL_SPEED); //X
    		    						right_motor_set_speed((i/RESOLUTION)/(i%RESOLUTION)*(-MOTOR_OPTIMAL_SPEED));	//Y
    		    						chThdSleepMilliseconds(STEP*i%RESOLUTION);	//Move of Y pas
    		    					}

    		    	    							indice = i;
    		    	    							break;
    		    							}
    		    				else{
    		    					continue;
    		    					}
    		    	    	}	//End of the for loop

    		    	for(points_counter; points_counter >= 0; points_counter--){
    		    		if(points[indice+1]){//Test all the left-right-top-bottom points			//Right
    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(MOTOR_NO_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice + 1;

    		    		}else if(points[indice+RESOLUTION]){		//down
    		    			left_motor_set_speed(MOTOR_NO_SPEED);
    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice + RESOLUTION;

    		    		}else if(points[indice-1]){		//left
    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(MOTOR_NO_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice - 1;
    		    		}else if(points[indice-RESOLUTION]){		//up
    		    			left_motor_set_speed(MOTOR_NO_SPEED);
    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice-RESOLUTION;

    		    		}else if(points[indice-RESOLUTION+1]){		//up right
    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice-RESOLUTION+1;

    		    		}else if(indice+RESOLUTION+1){		//down right
    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice+RESOLUTION+1;

    		    		}else if(indice+RESOLUTION-1){		//down left
    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice+RESOLUTION-1;

    		    		}else{			//up left
    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice-RESOLUTION-1;

    		    		}
    		    		chThdSleepMilliseconds(STEP);
//    		    		left_motor_set_speed(MOTOR_NO_SPEED);
//    		    		right_motor_set_speed(MOTOR_NO_SPEED);

//    		    		points[indice] = 0;
    		    	} //end for loop
    		    	left_motor_set_speed(MOTOR_NO_SPEED);
    		    	right_motor_set_speed(MOTOR_NO_SPEED);




    			}
    			/*
    			 * Repérer les points de process image	-ok
    			 * Trouver le premier point de la chaine	-ok
    			 * Aller au centre du 1er point	-nop
    			 * Relier les points avec le stylo
    			 */

    		}
}


void draw_pattern_start(){
	chThdCreateStatic(waThdDrawPattern, sizeof(waThdDrawPattern), NORMALPRIO-1, ThdDrawPattern, NULL);
}


