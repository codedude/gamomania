#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include "alloc.hpp"
#include "camera.hpp"
#include "demo.hpp"
#include "gl_debug.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
	AppData *appData = NULL;

	appData = ALLOC_ZERO(1, AppData);
	CHECK_ALLOC(appData, SDL_APP_FAILURE);
	*appstate = appData;

	appData->viewportWidth = 800;
	appData->viewportHeight = 600;

	if (!SDL_SetAppMetadata("Gamomania", "0.0.1", "com.gamomamia.demo")) {
		SDL_LogWarn(0, "Couldn't set metadata: %s", SDL_GetError());
	}
	/* Create the opengl window */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 4);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	if ((appData->sdlWindow = SDL_CreateWindow(
	         "Gamomania", appData->viewportWidth, appData->viewportHeight,
	         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
	             SDL_WINDOW_MOUSE_RELATIVE_MODE)) == NULL) {
		SDL_Log("Couldn't create window: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_WARP_MOTION, "1",
	                        SDL_HINT_OVERRIDE);
	SDL_SetWindowRelativeMouseMode(appData->sdlWindow, true);
	appData->glContext = SDL_GL_CreateContext(appData->sdlWindow);
	if (!appData->glContext) {
		SDL_Log("Couldn't create openGL context: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	if (!gladLoadGL()) {
		SDL_Log("Couldn't init opengl");
		return SDL_APP_FAILURE;
	}
	SDL_Log("OpenGL version: %s\n", glGetString(GL_VERSION));

	// int displayCount          = 0;
	// SDL_DisplayID* displaysId = SDL_GetDisplays(&displayCount);
	// if (displayCount == 0 || displaysId == NULL) {
	// 	SDL_LogError(0, "Couldn't get displays: %s", SDL_GetError());
	// 	return SDL_APP_FAILURE;
	// }

	glViewport(0, 0, appData->viewportWidth, appData->viewportHeight);

// int flags;
// glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
// if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
	                      GL_TRUE);
#endif
	// }

	if (!demoSetup(appData)) {
		return SDL_APP_FAILURE;
	}

	SDL_Log("App start");
	return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
	AppData *appData = (AppData *)appstate;
	int width = -1;
	int height = -1;

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	} else if (event->type == SDL_EVENT_WINDOW_RESIZED) {
		SDL_GetWindowSize(appData->sdlWindow, &width, &height);
		appData->viewportWidth = width;
		appData->viewportHeight = height;
		Camera_setAspectFromViewport(appData->cam, width, height);
		glViewport(0, 0, appData->viewportWidth, appData->viewportHeight);
	}

	if (!Event_process(appData, event)) {
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
	AppData *appData = (AppData *)appstate;

	processEvents(appData);

	glDisable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.2f, 0.3f, 0.3f, .5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	demoRender(appData);
	glEnable(GL_FRAMEBUFFER_SRGB);
	SDL_GL_SwapWindow(appData->sdlWindow);

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
	AppData *appData = (AppData *)appstate;

	demoDelete(appData);
	SDL_GL_DestroyContext(appData->glContext);
	SDL_DestroyWindow(appData->sdlWindow);
	FREE(appData);
	if (result == SDL_APP_SUCCESS) {
		SDL_Log("App quit success");
	} else {
		SDL_Log("App quit error");
	}
}
