#ifndef __DEMO_H
#define __DEMO_H

#include <SDL3/SDL_video.h>
#include <glad/glad.h>

#include "camera.h"
#include "event.h"
#include "shader.h"

typedef struct {
	SDL_Window *sdlWindow;
	SDL_GLContext glContext;
	int viewportWidth;
	int viewportHeight;
	Camera *cam;
	ShaderProgram *program;
	ShaderProgram *lightProgram;
	Event_Movement event;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int lightVAO;
	unsigned int EBO;
} AppData;

bool Event_process(AppData *appData, SDL_Event *event);

void processEvents(AppData *appData);
void demoRender(AppData *appData);
bool loadVertices(AppData *appData);
void unloadVertices(AppData *appData);

#endif
