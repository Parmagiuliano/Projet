/*
 * process_image.c
 *
 *  Created on: Apr 27, 2021
 *  Authors: Parma Giuliano & Jacquart Sylvain
 *
 */
#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <process_image.h>
#include <Restart_Programm.h>


//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

static bool draw_start = 0;

/* get_line_data function @brief
 * Called once to find the width and the center of the line,
 * then a second time to find the height of the picture and the center of the column.
 */

line_data get_line_data (uint8_t *buffer){
	line_data current_line;
	uint16_t i = 0, begin = 0, end = 0;
		uint8_t stop = 0, wrong_line = 0, line_not_found = 0;
		uint32_t mean = 0;
		current_line.width = current_line.position = 0;

		//mean
		for(uint16_t i = 0 ; i < IMAGE_WIDTH ; i++){
				mean += buffer[i];
			}
			mean /= IMAGE_WIDTH;

			do{

				wrong_line = 0;

				//search for a begin
				while(stop == 0 && i < (IMAGE_WIDTH-MIN_LINE_WIDTH))
				{
					//The slope must at least be WIDTH_SLOPE wide and is compared
				    //to the mean of the image.
					//sign changed
					 if(buffer[i] < mean/2   && buffer[i+WIDTH_SLOPE] > mean  )
				    {
				        begin = i;
				        stop = 1;
				    }
				    i++;
				}
				//If a begin was found, search for an end

				if (i < (IMAGE_WIDTH - WIDTH_SLOPE) && begin)
				{
				    stop = 0;

				    //changed signs
				    while(stop == 0 && i > WIDTH_SLOPE)
				    {
				    	if(buffer[i] < mean/2   && buffer[i-WIDTH_SLOPE] > mean )
				        {
				            end = i;
				            stop = 1;
				        }
				        i++;
				    }

				    //if an end was not found
				    if (i > IMAGE_WIDTH || !end)
				    {
				        line_not_found = 1;
				    }
				}
				else//if no begin was found
				{
				    line_not_found = 1;
				}

				//if a line too small has been detected, continues the search
				if(!line_not_found && (end-begin) < MIN_LINE_WIDTH){
					i = end;
					begin = 0;
					end = 0;
					stop = 0;
					wrong_line = 1;
				}
			}while(wrong_line);

			if(line_not_found){
					begin = 0;
					end = 0;

				}else{
					current_line.width  = (end - begin);
					current_line.position = (begin + end)/2; //gives the line position.
				}
					return current_line;
}


/* test_continuity function @brief
 * Test if on the pattern, a square doesn't have more than 4 squares adjoining, 2 on sides and 2 on diagonals
 */

bool test_continuity (uint8_t *points){
	uint8_t adjoining = 0, diagonal = 0;
	for(uint8_t i =0; i < (MAX_POINTS) ; i++){
		if(points [i]){
			adjoining = points [i-1] + points [i+1] + points [i-RESOLUTION] + points [i+RESOLUTION];
			diagonal = points [i-RESOLUTION + 1] + points [i+RESOLUTION +1] + points [i-RESOLUTION - 1] + points [i+RESOLUTION - 1];
			if (adjoining > 2 || diagonal > 2 || (adjoining + diagonal) <2 ){
				return false;
			}
			else{
				continue;
			}
		}
			else{
				continue;
			}
		}
	return true;
}

/* draw_pattern function @brief
 * Start by reaching the 1st square of the pattern,
 * Then for each point stocked on the string, check for the next square continuing the pattern.
 * Draw a line linking the center of each square of the pattern.
 */

void draw_pattern(uint8_t *points){
	uint8_t points_counter = 0, indice = 0, end = 0;
 uint8_t lower_speed = 0;
uint16_t sleep_time = 0;
	for(uint8_t i =0; i < (MAX_POINTS) ; i++){
	    		    							points_counter += points [i];
	    		    						}
	    		    	//Looking for the origin
	    		    	for(uint16_t i =0; i < (MAX_POINTS) ; i++){
	    		    				if(points[i]){
	    		    					if(i/RESOLUTION >= i%RESOLUTION){	//Reach for the starting point of the pattern
	    		    						//set speed diagonal			//i/RESOLUTION == line, i%RESOLUTION == column
	    		    						//wait
	    		    						lower_speed = ((i*MOTOR_OPTIMAL_SPEED)%RESOLUTION)/((i*MOTOR_OPTIMAL_SPEED)/RESOLUTION);
	    		    						sleep_time = STEP*i/RESOLUTION;
	    		    						left_motor_set_speed(lower_speed); //X
	    		    						right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);	//Y
	    		    						chThdSleepMilliseconds(sleep_time);	//Move of Y pas

	    		    					}
	    		    					else{
	    		    						//set speed diagonal
	    		    						//wait
	    		    						lower_speed = ((i*MOTOR_OPTIMAL_SPEED)/RESOLUTION)/((i*MOTOR_OPTIMAL_SPEED)%RESOLUTION);
	    		    						sleep_time = STEP*(i%RESOLUTION);
	    		    						left_motor_set_speed(MOTOR_OPTIMAL_SPEED); //X
	    		    						right_motor_set_speed(-lower_speed);	//Y
	    		    						chThdSleepMilliseconds(sleep_time);	//Move of Y pas

	    		    					}

	    		    					indice = end = i;
	    		    	    							break;
	    		    						}
	    		    				else{
	    		    					continue;
	    		    					}
	    		    	    	}	//End of the for loop

	    		    	for(uint8_t i = points_counter+1  ; i > 0; i--){
	    		    		if(points[indice+1]){//Test all the left-right-top-bottom points			//Right
	    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
	    		    			right_motor_set_speed(MOTOR_NO_SPEED);

	    		    			points[indice] = 0;
	    		    			indice = indice + 1;


	    		    		}else if(points[indice+RESOLUTION]){										//Down
	    		    			left_motor_set_speed(MOTOR_NO_SPEED);
	    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

	    		    			points[indice] = 0;
	    		    			indice = indice + RESOLUTION;

	    		    		}else if(points[indice-1]){													//Left
	    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
	    		    			right_motor_set_speed(-MOTOR_NO_SPEED);

	    		    			points[indice] = 0;
	    		    			indice = indice - 1;
	    		    		}else if(points[indice-RESOLUTION]){										//Up
	    		    			left_motor_set_speed(MOTOR_NO_SPEED);
	    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

	    		    			points[indice] = 0;
	    		    			indice = indice-RESOLUTION;

	    		    		}else if(points[indice-RESOLUTION+1]){										//Up right
	    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
	    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

	    		    			points[indice] = 0;
	    		    			indice = indice-RESOLUTION+1;

	    		    		}else if(points[indice+RESOLUTION+1]){										//Down right
	    		    			left_motor_set_speed(MOTOR_OPTIMAL_SPEED);
	    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

	    		    			points[indice] = 0;
	    		    			indice = indice+RESOLUTION+1;

	    		    		}else if(points[indice+RESOLUTION-1]){										//Down left
	    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
	    		    			right_motor_set_speed(-MOTOR_OPTIMAL_SPEED);

	    		    			points[indice] = 0;
	    		    			indice = indice+RESOLUTION-1;

	    		    		}else if(points[indice-RESOLUTION-1]){										//Up left
	    		    			left_motor_set_speed(-MOTOR_OPTIMAL_SPEED);
	    		    			right_motor_set_speed(MOTOR_OPTIMAL_SPEED);

	    		    			points[indice] = 0;
	    		    			indice = indice-RESOLUTION-1;

	    		    		}
	    		    		else {
	    		    			points[end] = 1;
		    		    		left_motor_set_speed(MOTOR_NO_SPEED);
		    		    		right_motor_set_speed(MOTOR_NO_SPEED);
		    		    		chThdSleepMilliseconds(STEP);
	    		    		}
	    		    		chThdSleepMilliseconds(STEP);




	//    		    		points[indice] = 0;
	    		    	} //end for loop
	    		    	left_motor_set_speed(MOTOR_NO_SPEED);
	    		    	right_motor_set_speed(MOTOR_NO_SPEED);


}

static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    //unsigned int i = 0;
    (void)arg;

	//Take 97 x 97 pixels from a square of 388x388 on the sensor with a SUBSAMPLING factor of 4 (9409 pixels in total)
	po8030_advanced_config(FORMAT_RGB565, 128, 10, 384, 384, SUBSAMPLING_X4, SUBSAMPLING_X4);
	dcmi_disable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
    	if(get_selector() == 4){
    		//    	chBSemWait(&process_image_start_sem);

    		        //starts a capture
    				dcmi_capture_start();
    				//waits for the capture to be done
    				wait_image_ready();
    				//signals an image has been captured
    				chBSemSignal(&image_ready_sem);
    	}else{
    		chThdSleepMilliseconds(200);
    	}

    }

}


static THD_WORKING_AREA(waProcessImage, 256);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    uint8_t led1= 0, led3 = 0, led5 = 0, led7 = 0, points_counter = 0, max = 0, min = 0;
	uint8_t *img_buff_ptr;
	uint8_t line[IMAGE_WIDTH] = {0};
	uint8_t points_buff[MAX_POINTS] = {0};
	uint16_t mean = 0;
	uint16_t indice_1 = 0, indice_2 = 0;

	line_data width , height;
	width.width = width.position = height.width = height.position = 0;

    while(1){


    	if(get_selector() == 4){


    	draw_start = 0;

    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565    
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the blue pixels
		for(uint16_t i =0; i < (2*IMAGE_WIDTH) ; i+= 2){
			//extracts last 5bits of the second byte
			//takes nothing from the first byte
			line[i/2] = (((uint8_t)img_buff_ptr[IMAGE_WIDTH*IMAGE_WIDTH+i+1]&0x1f));
		}

		//search for a line in the image and gets its width in pixels
		width = get_line_data(line);
		led1 = ( width.position || 0);
		palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
		//remplissage tableau pour lecture colonne
		//Extracts only the blue pixels
		if(width.position){
			for(uint16_t i =0; i < (2*IMAGE_WIDTH) ; i+= 2){
						//extracts last 5bits of the secind byte
						//takes nothing from the first byte
						// read the column centered on the width
						line[i/2] = (((uint8_t)img_buff_ptr[IMAGE_WIDTH*i+2*width.position+1]&0x1f));
			}
				//height of the square found
						height = get_line_data(line);
						led3 = (height.position || 0);
						}
		else{
			//height not founf
			led3 = 0;
		}
		palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
		palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
		if(height.position & width.position){
			// lecture points en frequence rouge
					for(uint8_t i =0; i < (2*(RESOLUTION+10)) ; i+= 2){
						for(uint8_t j =0; j < (2*RESOLUTION) ; j+= 2){
										for(uint8_t k = 0; k < (4) ; k+= 2){
											//lecture de 4 points dispos�s en carr�
											indice_1 = 2*(width.position-width.width/2+SHIFT) +2*(height.position-height.width/2+SHIFT)*IMAGE_WIDTH+
													i*(IMAGE_WIDTH * (height.width/RESOLUTION)) +j*(width.width/RESOLUTION)+ k*IMAGE_WIDTH;
											indice_2 = 2*(width.position-width.width/2+SHIFT) +2*(height.position-height.width/2+SHIFT)*IMAGE_WIDTH+
													i*(IMAGE_WIDTH* (height.width/RESOLUTION)) +j*(width.width/RESOLUTION)+ k*IMAGE_WIDTH+2;
											mean += (((uint8_t)img_buff_ptr[indice_1]&0xf8));

											mean += (((uint8_t)img_buff_ptr[indice_2]&0xf8));
										}
							//Calculate the mean of the 4 squares
							mean /= 4;
							points_buff[(i*RESOLUTION+j)/2] = mean;
							mean = 0;

						}
					}
					//Find a minimum and a maximum
					min = max = points_buff[0];
					for(uint8_t i =0; i < (MAX_POINTS) ; i++){
						if (points_buff[i] == 0 || points_buff[i] < min){
							min = points_buff[i];
						}
						else if (points_buff [i]> max){
							max = points_buff[i];
						}

					}
					//Do the mediane of values.
					mean = min +(max-min)/2;

			//Attributions of booleans values
					points_counter = 0;
					for(uint8_t i =0; i < (MAX_POINTS) ; i++){
						points_buff [i] = (points_buff [i] < mean);
					}

					if(test_continuity(points_buff)){
						led5 = 1;
                         draw_pattern(points_buff);
                         FindTheOrigin();
					}
					else{
						led7 = 0;
					}

					palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
					palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);

					chThdSleepMilliseconds(200);

			}

		else{
						led5 = 0;
						//led7 = 0;
						palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
			}
    }else{
    	chThdSleepMilliseconds(250);
    }

  }

}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}
