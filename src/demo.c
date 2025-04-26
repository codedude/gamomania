#include "demo.h"
#include "shader.h"
#include <SDL3/SDL_timer.h>
#include <math.h>

void renderTriangle(AppData* appData) {
	float timeValue  = SDL_GetTicks() / 1000.0;
	float greenValue = (cos(timeValue) + sin(timeValue)) / 2.0f + 0.5f;
	int vertexColorLocation =
	glGetUniformLocation(appData->program->id, "timeValue");

	glUseProgram(appData->program->id);
	glUniform4f(vertexColorLocation, greenValue, greenValue, greenValue, 1.0f);
	glBindVertexArray(appData->VAO);
	// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}


bool loadVertices(AppData* appData) {
	float vertices[] = {
		// positions         // colors
		0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
		0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
	};

	// glGenBuffers(1, EBO);
	glGenBuffers(1, &appData->VBO);
	glGenVertexArrays(1, &appData->VAO);

	// 1. bind Vertex Array Object
	glBindVertexArray(appData->VAO);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, appData->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
						  (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	return true;
}

void unloadVertices(AppData* appData) {
	glDeleteVertexArrays(1, &appData->VAO);
	// glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &appData->VBO);
}
