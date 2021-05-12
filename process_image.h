#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H
typedef struct {
	uint16_t position;
	uint16_t width;
} line_data;

uint8_t* get_points_location(void);
uint16_t get_line_position(void);


#endif /* PROCESS_IMAGE_H */
