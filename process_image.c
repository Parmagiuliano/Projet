#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <process_image.h>




//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);
static uint8_t points[MAX_POINTS] = {0};


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
					//the slope must at least be WIDTH_SLOPE wide and is compared
				    //to the mean of the image
					//sign changed
					 if(buffer[i] < mean/2   && buffer[i+WIDTH_SLOPE] > mean  )
				    {
				        begin = i;
				        stop = 1;
				    }
				    i++;
				}
				//if a begin was found, search for an end

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
bool test_continuity (uint8_t *points){
	uint8_t adjoining = 0, diagonal = 0;
	for(uint8_t i =0; i < (MAX_POINTS) ; i++){
		if(points [i]){
			//on teste la continuité de la trace et qu ça soit fermé
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


static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Take 97 x 97 pixels from a square of 388x388 on the sensor with a SUBSAMPLING factor of 4 (9409 pixels in total)
	po8030_advanced_config(FORMAT_RGB565, 128, 10, 384, 384, SUBSAMPLING_X4, SUBSAMPLING_X4);
	dcmi_disable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);
    }
}


static THD_WORKING_AREA(waProcessImage, 1024);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    uint8_t led1= 0, led3 = 0, led5 = 0, led7 = 0, points_counter = 0, max = 0, min = 0;
	uint8_t *img_buff_ptr;
	uint8_t line[IMAGE_WIDTH] = {0};
	uint16_t mean = 0;
	uint16_t indice_1 = 0, indice_2 = 0;
	line_data width , height;
	width.width = width.position = height.width = height.position = 0;

    while(1){
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
											//lecture de 4 points disposés en carré
											indice_1 = 2*(width.position-width.width/2+SHIFT) +2*(height.position-height.width/2+SHIFT)*IMAGE_WIDTH+
													i*(IMAGE_WIDTH * (height.width/RESOLUTION)) +j*(width.width/RESOLUTION)+ k*IMAGE_WIDTH;
											indice_2 = 2*(width.position-width.width/2+SHIFT) +2*(height.position-height.width/2+SHIFT)*IMAGE_WIDTH+
													i*(IMAGE_WIDTH* (height.width/RESOLUTION)) +j*(width.width/RESOLUTION)+ k*IMAGE_WIDTH+2;
											mean += (((uint8_t)img_buff_ptr[indice_1]&0xf8));

											mean += (((uint8_t)img_buff_ptr[indice_2]&0xf8));


										}
							//moyenne
							mean /= 4;
							points[(i*RESOLUTION+j)/2] = mean;
							mean = 0;

						}
					}
					//trouver maximal et minimal
					min = max = points[0];
					for(uint8_t i =0; i < (MAX_POINTS) ; i++){
						if (points[i] == 0 || points[i] < min){
							min = points[i];
						}
						else if (points [i]> max){
							max = points[i];
						}

					}
					//mediane des valeurs
					mean = min +(max-min)/2;

			//attributions valeurs buoleens
					points_counter = 0;
					for(uint8_t i =0; i < (MAX_POINTS) ; i++){
						points [i] = (points [i] < mean);
						points_counter += points [i];
					}
					//print values
//					for(uint8_t i =0; i < (RESOLUTION) ; i++){
//						for(uint8_t j =0; j < (RESOLUTION) ; j++){
//							chprintf((BaseSequentialStream *)&SD3, " %d ", points[i*RESOLUTION+j]);
//
//						}
//						chprintf((BaseSequentialStream *)&SD3, " \n ");
//					}


					if(test_continuity(points)){
						led5 = 1;
						;
					}
					else{
						led7 = 0;
					}
					if(points_counter == 20){
						led7 = 1;
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
						led7 = 0;
						palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
						palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);

			}

    }
}

uint8_t* get_points_location(void){
	return points;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}

