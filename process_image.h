#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H
typedef struct {
	uint16_t position;
	uint16_t width;
} line_data;

#define MAX_POINTS		10	//Must be corrected
#define RESOLUTION		10	//Same

#define SMALL_SQUARE_SIDE	60 //px
#define BIG_SQUARE_SIDE		9*SMALL_SQUARE_SIDE //px





uint16_t get_line_position(void);


#endif /* PROCESS_IMAGE_H */
