#include "shader.h"
#include <SDL3/SDL_keycode.h>
#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>

#include "demo.h"
#include "gl_debug.h"


/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	AppData* appData = NULL;

	appData = calloc(1, sizeof(*appData));
	if (!appData) {
		printf("error malloc at startup\n");
		return SDL_APP_FAILURE;
	}
	*appstate = appData;

	appData->viewportWidth  = 800;
	appData->viewportHeight = 600;
	if (!SDL_SetAppMetadata("Gamomania", "0.0.1", "com.gamomamia.demo")) {
		SDL_LogWarn(0, "Couldn't set metadata: %s", SDL_GetError());
	}
	/* Create the opengl window */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	if ((appData->sdlWindow =
		 SDL_CreateWindow("Hello World", appData->viewportWidth, appData->viewportHeight,
						  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)) == NULL) {
		SDL_Log("Couldn't create window: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
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
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	// }

	if (!Shader_init(&(appData->program))) {
		SDL_Log("Error shader_init");
		return SDL_APP_FAILURE;
	}
	if (!loadVertices(appData)) {
		SDL_Log("Error loadVertices");
		return SDL_APP_FAILURE;
	}

	SDL_Log("App start");
	return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	AppData* appData       = (AppData*)appstate;
	static bool wireToggle = false;

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	} else if (event->type == SDL_EVENT_WINDOW_RESIZED) {
		int w, h;
		SDL_GetWindowSize(appData->sdlWindow, &w, &h);
		appData->viewportWidth  = w;
		appData->viewportHeight = h;
		glViewport(0, 0, appData->viewportWidth, appData->viewportHeight);
	} else if (event->type == SDL_EVENT_KEY_DOWN) {
		if (event->key.key == SDLK_T) {
			if (wireToggle) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			wireToggle = !wireToggle;
		} else if (event->key.key == SDLK_ESCAPE) {
			return SDL_APP_SUCCESS;
		}
	}
	return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate) {
	AppData* appData = (AppData*)appstate;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	renderTriangle(appData);
	SDL_GL_SwapWindow(appData->sdlWindow);

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	AppData* appData = (AppData*)appstate;

	unloadVertices(appData);
	Shader_free(&appData->program);
	SDL_GL_DestroyContext(appData->glContext);
	SDL_DestroyWindow(appData->sdlWindow);

	if (result == SDL_APP_SUCCESS) {
		SDL_Log("App quit");
	} else {
		SDL_Log("App quit error");
	}
}
