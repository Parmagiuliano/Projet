#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H
#include <stdbool.h>
#include <stdint.h>
typedef struct {
	uint16_t position;
	uint16_t width;
} line_data;

//bool test_continuity (uint8_t *points);
bool get_draw_start(void);
uint8_t* get_points_location(void);
uint16_t get_line_position(void);


#endif /* PROCESS_IMAGE_H */
