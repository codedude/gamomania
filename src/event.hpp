#ifndef __EVENT__H
#define __EVENT__H

#include <SDL3/SDL_events.h>

typedef enum : int { FORWARD = 0, RIGHT, LEFT, BACK, UP, DOWN } E_DIRECTION;

typedef struct {
	int direction : 6; // bitfield, indexes are E_DIRECTION
	bool speed;        // speed boost pushed ?
	float mouseX;
	float mouseY;
	bool rotate;
} Event_Movement;

#endif
