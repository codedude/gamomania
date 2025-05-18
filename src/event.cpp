#include "event.hpp"
#include "demo.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <glad/glad.h>

bool Event_process(AppData *appData, SDL_Event *event) {
	static bool wireToggle = false;
	const bool *kbState = SDL_GetKeyboardState(NULL);

	if (event->type == SDL_EVENT_KEY_UP) {
		if (event->key.key == SDLK_T) {
			if (wireToggle) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			wireToggle = !wireToggle;
		} else if (event->key.key == SDLK_ESCAPE) {
			return false;
		}
	} else if (event->type == SDL_EVENT_MOUSE_MOTION) {
		appData->event.mouseX = event->motion.xrel;
		appData->event.mouseY = event->motion.yrel;
		appData->event.rotate = true;
	}

#define SET_BIT(v, b) (v |= (1 << (b)))
#define UNSET_BIT(v, b) (v &= ~(1 << (b)))

	if (kbState[SDL_SCANCODE_W]) {
		SET_BIT(appData->event.direction, FORWARD);
	} else {
		UNSET_BIT(appData->event.direction, FORWARD);
	}
	if (kbState[SDL_SCANCODE_S]) {
		SET_BIT(appData->event.direction, BACK);
	} else {
		UNSET_BIT(appData->event.direction, BACK);
	}
	if (kbState[SDL_SCANCODE_D]) {
		SET_BIT(appData->event.direction, RIGHT);
	} else {
		UNSET_BIT(appData->event.direction, RIGHT);
	}
	if (kbState[SDL_SCANCODE_A]) {
		SET_BIT(appData->event.direction, LEFT);
	} else {
		UNSET_BIT(appData->event.direction, LEFT);
	}
	if (kbState[SDL_SCANCODE_E]) {
		SET_BIT(appData->event.direction, UP);
	} else {
		UNSET_BIT(appData->event.direction, UP);
	}
	if (kbState[SDL_SCANCODE_Q]) {
		SET_BIT(appData->event.direction, DOWN);
	} else {
		UNSET_BIT(appData->event.direction, DOWN);
	}
	appData->event.speed = kbState[SDL_SCANCODE_LSHIFT];

	return true;
}
