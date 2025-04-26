#ifndef __SHARE_H
#define __SHARE_H

#include <glad/glad.h>
#include <stddef.h>

// From project root
static const char* SHADER_FOLDER = "shader/";

typedef GLuint ProgramId;
typedef GLuint ShaderId;

typedef struct {
	ProgramId id;
} ShaderProgram;

bool Shader_init(ShaderProgram** program);
void Shader_free(ShaderProgram** program);

ShaderId Shader_shaderLoad(const char* path, GLenum type);
ProgramId Shader_programLoad(const char* shaderPath, const char* fragmentPath);
void Shader_shaderUnload(ShaderId shader);
void Shader_programUnload(ProgramId program);

#endif
