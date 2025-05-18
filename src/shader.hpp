#ifndef __SHARE_H
#define __SHARE_H

#include "list.hpp"
#include "material.hpp"
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stddef.h>

#define SHADER_UNIFORM_MAX_LEN 128

// From project root
static const char *SHADER_FOLDER = "src/shader/";

typedef GLuint ProgramId;
typedef GLuint ShaderId;
typedef GLint UniformLocation;

static const ProgramId InvalidProgramId = 0;
static const ShaderId InvalidShaderId = 0;
static const UniformLocation InvalidLocation = -1;

// TODO: real hash map
//  simple list for now
typedef struct {
	char *name;
	UniformLocation id; // location for uniform, index for uniform block
} UniformData;

typedef struct {
	GLenum type;    // vertex, fragment... shader
	ShaderId id;    // ogl id gotten after compile
	char *filePath; // file to load
} ShaderLoader;

typedef struct {
	ProgramId id;
	List uniformList;
	List uniformBlockList;
	Material *container;
} ShaderProgram;

bool Shader_loaderAddShader(List *loaderList, const char *path, GLenum type);
void Shader_loaderDelete(ShaderLoader *shaderLoader);

bool Shader_init(ShaderProgram **program, List *loaderList);
void Shader_free(ShaderProgram **program);

// use struct only for loading, then delete ShaderLoader info
void Shader_uniformDelete(UniformData *data);
UniformData *Shader_uniformCreate(const char *name, UniformLocation id);
bool Shader_uniformLoad(ShaderProgram *program);
UniformLocation Shader_uniformGet(const ShaderProgram *program,
                                  const char *name);
UniformLocation Shader_uniformBlockGet(const ShaderProgram *program,
                                       const char *name);

bool Shader_shaderLoad(ShaderLoader *shader);
void Shader_shaderUnload(ShaderId shader);
ProgramId Shader_programLoad(List *loaderList);
void Shader_programUnload(ProgramId program);

// ignore return
void Shader_setInt(UniformLocation id, int v);
void Shader_setFloat(UniformLocation id, float v);
// float by default
void Shader_setVec2(UniformLocation id, glm::vec2 v);
void Shader_setVec3(UniformLocation id, glm::vec3 v);
void Shader_setVec4(UniformLocation id, glm::vec4 v);
void _Shader_setMat4(UniformLocation id, int n, bool transpose, glm::mat4 v);
#define Shader_setMat4(id, v) _Shader_setMat4((id), 1, false, (v))
#define Shader_setMat4n(id, v, n) _Shader_setMat4((id), (n), false, (v))
#define Shader_setMat4t(id, v) _Shader_setMat4((id), 1, true, (v))
#define Shader_setMat4nt(id, v, n) _Shader_setMat4((id), (n), true, (v))

#endif
