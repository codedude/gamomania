#include "shader.h"
#include "file.h"
#include "gl_debug.h"
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_storage.h>
#include <assert.h>
#include <stdlib.h>

ShaderId Shader_shaderLoad(const char* path, GLenum type) {
	const char* shaderSource = NULL;
	const char* fullPath     = NULL;
	ShaderId shaderId        = 0;
	int success              = 0;
	char infoLog[512]        = {};

	fullPath = concatPath(PROJECT_PATH, SHADER_FOLDER, path, NULL);
	if (!fullPath) {
		return 0;
	}
	shaderSource = readFile(fullPath);
	free((void*)fullPath);
	if (!shaderSource) {
		SDL_LogError(1, "Cannot read shader file %s", path);
		return 0;
	}
	shaderId = glCreateShader(type);
	if (glCheckError())
		return 0;
	glShaderSource(shaderId, 1, &shaderSource, NULL);
	SDL_free((void*)shaderSource);
	if (glCheckError())
		return 0;
	glCompileShader(shaderId);
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		SDL_LogError(1, "OPENGL(-):\t compiler shader = %s", infoLog);
		return 0;
	}
	if (glCheckError())
		return 0;

	return shaderId;
}

ProgramId Shader_programLoad(const char* shaderPath, const char* fragmentPath) {
	ProgramId programId = 0;
	ShaderId vertexId   = 0;
	ShaderId fragmentId = 0;
	int success         = 0;
	char infoLog[512]   = {};

	vertexId = Shader_shaderLoad(shaderPath, GL_VERTEX_SHADER);
	if (!vertexId)
		return 0;
	fragmentId = Shader_shaderLoad(fragmentPath, GL_FRAGMENT_SHADER);
	if (!fragmentId) {
		Shader_shaderUnload(vertexId);
		return 0;
	}
	programId = glCreateProgram();
	if (glCheckError())
		return 0;
	glAttachShader(programId, vertexId);
	if (glCheckError())
		return 0;
	glAttachShader(programId, fragmentId);
	if (glCheckError())
		return 0;
	glLinkProgram(programId);
	if (glCheckError())
		return 0;
	Shader_shaderUnload(vertexId);
	Shader_shaderUnload(fragmentId);
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		SDL_LogError(1, "OPENGL(-):\t link program = %s", infoLog);
	}
	ShaderProgram* program = malloc(sizeof(ShaderProgram));
	if (!program) {
		Shader_programUnload(programId);
		SDL_LogError(1, "malloc error");
		return 0;
	}

	return programId;
}

void Shader_shaderUnload(ShaderId shader) {
	if (shader == 0)
		return;
	glDeleteShader(shader);
	glCheckError();
}

void Shader_programUnload(ProgramId program) {
	if (program == 0)
		return;
	glDeleteProgram(program);
	glCheckError();
}


bool Shader_init(ShaderProgram** program) {
	assert(program != NULL);

	ProgramId programId = 0;

	*program = calloc(1, sizeof(ShaderProgram));
	if (!(*program)) {
		return false;
	}
	programId = Shader_programLoad("vertex.glsl", "fragment.glsl");
	if (programId == 0) {
		free(*program);
		*program = NULL;
		return false;
	}
	(*program)->id = programId;

	return true;
}

void Shader_free(ShaderProgram** program) {
	if (program == NULL || *program == NULL) {
		return;
	}
	Shader_programUnload((*program)->id);

	*program = NULL;
}
