/*
 * Draw_pattern.c
 *
 *  Created on: May 12, 2021
 *  Authors: Parma Giuliano & Jacquart Sylvain
 */
//static THD_WORKING_AREA(waThdSwitch_Selector, 128);
#include <process_image.h>
#include <main.h>
#include <Draw_pattern.h>
#include <motors.h>

static THD_WORKING_AREA(waThdDrawPattern, 1024);
static THD_FUNCTION(ThdDrawPattern, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    // Infinite loop
    while(1){
    	uint8_t *drawing;
    	uint8_t points [MAX_POINTS];
		uint8_t points_counter = 0, indice = 0;

		// Execute if the bool draw start is true
    	if(get_draw_start ()){
    		// Capture the points location on the picture
    		drawing = get_points_location();

    				// Scanning for each point of the 9x9 square array.
    		    	for(uint8_t i =0; i < (MAX_POINTS) ; i++){
    		    							points [i] = ((uint8_t)drawing [i]);
    		    							points_counter += points [i];
    		    						}

    		    	// Looking for the 1st cyan point on the picture -> Starting point of the drawing
    		    	for(uint8_t i =0; i < (MAX_POINTS) ; i++){
    		    				if(points[i]){
    		    					/*
    		    					 * The pen draw a straight line from the origin of the system to the 1st square.
    		    					 * i divided by RESOLUTION to find the line of the first square.
    		    					 * i modulo RESOLUTION to find the column.
    		    					 */

    		    					/* ASCII representation
    		    					 *
    		    					 *      ¦-- Column, i % RESOLUTION
									 *		v
									 *	. . . . . . . . .
									 *	. . O 1 . 1 1 . . <-- Line, i / RESOLUTION
									 *	. 1 . . 1 . . 1 .
									 *	. . 1 . . . 1 . .
									 *	. . . 1 . 1 . . .
									 *	. . . . 1 . . . .
									 *	. . . . . . . . .
									 *	O . . . . . . . .
									 *	^
									 *	¦--Origin
    		    					 */

    		    					// Set the speed ratio for the motors, depending if the line or the column takes more time to be reached.
    		    					if(i/RESOLUTION >= i%RESOLUTION){	//Takes more time to reach the line than the column.
    		    						left_motor_set_speed((i%RESOLUTION)/(i/RESOLUTION)*MOTOR_OPTIMAL_SPEED); 		//Xmotor -> slowest		//CW
    		    						right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);									//Ymotor -> Fastest		//CW

    		    						uint16_t DrawPatternSleepTime_Line = STEP*i/RESOLUTION;
    		    						chThdSleepMilliseconds(DrawPatternSleepTime_Line);								//Move of a number of step, fixed by the line position

									}else{								//Takes more time to reach the column than the line.
    		    						left_motor_set_speed(MOTOR_OPTIMAL_SPEED); 										//Xmotor -> Fastest		//CW
    		    						right_motor_set_speed((i/RESOLUTION)/(i%RESOLUTION)*(-MOTOR_OPTIMAL_SPEED));	//Ymotor -> slowest		//CCW

    		    						uint16_t DrawPatternSleepTime_Column = STEP*i%RESOLUTION;
    		    						chThdSleepMilliseconds(DrawPatternSleepTime_Column);							//Move of a number of step, fixed by the column position
    		    					}

    		    				indice = i;	// 'indice' represents the current drawn square
    		    				break;
    		    				}
    		    				else{
    		    					continue;
    		    				}
    		    	}	//End of the 'looking for the 1st square' loop

    		    	/*Description of the loop
    		    	 * For the number of cyan squares, look at the position of the folloing square in the pattern.
    		    	 * 8 possibilities (Right, Down, Left, Up, Up right, Down right, Down left, Up left).
    		    	 * Set the current position as the new centered position after every step.
    		    	 */

    		    	/*ASCII representation
    		    	 *
    		    	 *Up left	Up			Up right
    		    	 *Left		current pos	Right
					 *Down left	Down 		Down right
					 *
					 *			 ==
					 *
					 *i-res-1	i-res		i-res+1
					 *i-1		i 			i+1
					 *i+res-1	i+res		i+res+1
    		    	 */

    		    	for(points_counter; points_counter >= 0; points_counter--){
    		    		if(points[indice+1]){										//Right
    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(MOTOR_NO_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice + 1;

    		    		}else if(points[indice+RESOLUTION]){						//Down
    		    			left_motor_set_speed(MOTOR_NO_SPEED);
    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice + RESOLUTION;

    		    		}else if(points[indice-1]){									//Left
    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(MOTOR_NO_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice - 1;
    		    		}else if(points[indice-RESOLUTION]){						//Up
    		    			left_motor_set_speed(MOTOR_NO_SPEED);
    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice-RESOLUTION;

    		    		}else if(points[indice-RESOLUTION+1]){						//Up right
    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice-RESOLUTION+1;

    		    		}else if(indice+RESOLUTION+1){								//Down right
    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice+RESOLUTION+1;

    		    		}else if(indice+RESOLUTION-1){								//Down left
    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice+RESOLUTION-1;

    		    		}else{														//Up left
    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

    		    			points[indice] = 0;
    		    			indice = indice-RESOLUTION-1;

    		    		}
    		    		chThdSleepMilliseconds(STEP);
    		    		// Stop the motors after each iteration
    		    		left_motor_set_speed(MOTOR_NO_SPEED);
    		    		right_motor_set_speed(MOTOR_NO_SPEED);

//    		    		points[indice] = 0;
    		    	} //End of the 'drawing all the squares' loop

    		    	// Stop the motors after the loop
    		    	left_motor_set_speed(MOTOR_NO_SPEED);
    		    	right_motor_set_speed(MOTOR_NO_SPEED);




    			} // End of 'if(get_draw_start ())'
    		 } // End of 'while(1)'
} // End of the thread


void draw_pattern_start(){
	chThdCreateStatic(waThdDrawPattern, sizeof(waThdDrawPattern), NORMALPRIO-1, ThdDrawPattern, NULL);
}


