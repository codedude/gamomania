#ifndef __DEMO_H
#define __DEMO_H

#include "shader.h"
#include <SDL3/SDL_video.h>
#include <glad/glad.h>

typedef struct {
	ShaderProgram* program;
	SDL_Window* sdlWindow;
	SDL_GLContext glContext;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	int viewportWidth;
	int viewportHeight;
} AppData;

void renderTriangle(AppData* appData);
bool loadVertices(AppData* appData);
void unloadVertices(AppData* appData);

#endif
