#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H
#include <stdbool.h>
#include <stdint.h>
typedef struct {
	uint16_t position;
	uint16_t width;
} line_data;

void process_image_start(void);

#endif /* PROCESS_IMAGE_H */
