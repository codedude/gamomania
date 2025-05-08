#ifndef __SHARE_H
#define __SHARE_H

#include "cglm/cglm.h"
#include "list.h"
#include "material.h"
#include "texture.h"
#include <glad/glad.h>
#include <stddef.h>

// From project root
static const char *SHADER_FOLDER = "src/shader/";

typedef GLuint ProgramId;
typedef GLuint ShaderId;
typedef unsigned int UniformLocation;

static const ProgramId InvalidProgramId = 0;
static const ShaderId InvalidShaderId = 0;

typedef struct {
	GLenum type;
	ShaderId id;
	const char *filePath;
} ShaderLoader;

typedef struct {
	ProgramId id;
	Material *container;
} ShaderProgram;

bool Shader_loaderAddShader(List *loaderList, const char *path, GLenum type);
void Shader_loaderDelete(ShaderLoader *shaderLoader);

bool Shader_init(ShaderProgram **program, List *loaderList);
void Shader_free(ShaderProgram **program);

// use struct only for loading, then delete ShaderLoader info
bool Shader_shaderLoad(ShaderLoader *shader);
void Shader_shaderUnload(ShaderId shader);
ProgramId Shader_programLoad(List *loaderList);
void Shader_programUnload(ProgramId program);

// ignore return
void Shader_setInt(UniformLocation id, int v);
void Shader_setFloat(UniformLocation id, float v);
// float by default
void Shader_setVec2(UniformLocation id, vec2 v);
void Shader_setVec3(UniformLocation id, vec3 v);
void Shader_setVec4(UniformLocation id, vec4 v);
void _Shader_setMat4(UniformLocation id, int n, bool transpose, mat4 v);
#define Shader_setMat4(id, v) _Shader_setMat4((id), 1, false, (v))
#define Shader_setMat4n(id, v, n) _Shader_setMat4((id), (n), false, (v))
#define Shader_setMat4t(id, v) _Shader_setMat4((id), 1, true, (v))
#define Shader_setMat4nt(id, v, n) _Shader_setMat4((id), (n), true, (v))

#endif
