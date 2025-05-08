#include "shader.h"
#include "alloc.h"
#include "file.h"
#include "gl_debug.h"
#include "list.h"
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_storage.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

bool Shader_shaderLoad(ShaderLoader *shader) {
	assert(shader != NULL);

	const char *shaderSource = NULL;
	const char *fullPath = NULL;
	int success = 0;
	char infoLog[512] = {};

	fullPath = concatPath(PROJECT_PATH, SHADER_FOLDER, shader->filePath);
	RET_IF_NULL(fullPath, InvalidShaderId);
	shaderSource = readFile(fullPath);
	FREE(fullPath);
	if (!shaderSource) {
		SDL_LogError(1, "Cannot read shader file %s", shader->filePath);
		return false;
	}
	shader->id = glCreateShader(shader->type);
	if (glCheckError())
		return false;
	glShaderSource(shader->id, 1, &shaderSource, NULL);
	SDL_free((void *)shaderSource);
	if (glCheckError())
		return false;
	glCompileShader(shader->id);
	glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader->id, 512, NULL, infoLog);
		SDL_LogError(1, "OPENGL(-):\t compiler shader = %s", infoLog);
		return false;
	}
	if (glCheckError())
		return false;

	return true;
}

ProgramId Shader_programLoad(List *loaderList) {
	assert(loaderList != NULL);
	assert(loaderList->head != NULL); // at least 1 elem

	ProgramId programId = 0;

	int success = 0;
	char infoLog[512] = {};
	ListItem *iter = loaderList->head;

	programId = glCreateProgram();
	if (glCheckError())
		return InvalidProgramId;
	while (iter) {
		RET_IF_FALSE(Shader_shaderLoad(iter->data), InvalidProgramId);
		glAttachShader(programId, ((ShaderLoader *)iter->data)->id);
		if (glCheckError())
			return InvalidProgramId;
		// Shader_loaderDelete(((ShaderLoader *)iter->data));
		iter = iter->next;
	}
	glLinkProgram(programId);
	if (glCheckError())
		return InvalidProgramId;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		SDL_LogError(1, "OPENGL(-):\t link program = %s", infoLog);
		return InvalidProgramId;
	}

	return programId;
}

UniformData *Shader_uniformCreate(const char *name, UniformLocation id) {
	UniformData *data = ALLOC(1, *data);
	CHECK_ALLOC(data, NULL);
	data->id = id;
	data->name = strdup(name);
	CHECK_ALLOC(data->name, NULL);
	return data;
}

bool Shader_uniformLoad(ShaderProgram *program) {
	GLuint *idUniforms = NULL;
	GLint *outUniforms = NULL;
	GLint nUniforms = -1;
	GLsizei length = -1;
	char nameUniform[SHADER_UNIFORM_MAX_LEN] = {};
	UniformLocation loc = -1;
	UniformData *data = NULL;

	glUseProgram(program->id);
	// UNIFORMS
	// get number of uniform in program
	glGetProgramiv(program->id, GL_ACTIVE_UNIFORMS, &nUniforms);
	idUniforms = ALLOC_ZERO(nUniforms, *idUniforms);
	CHECK_ALLOC(idUniforms, false);
	outUniforms = ALLOC_ZERO(nUniforms, *outUniforms);
	CHECK_ALLOC(outUniforms, false);
	for (int i = 0; i < nUniforms; ++i) {
		idUniforms[i] = i;
	}
	// dont add uniform block
	glGetActiveUniformsiv(program->id, nUniforms, idUniforms,
	                      GL_UNIFORM_BLOCK_INDEX, outUniforms);
	for (int i = 0; i < nUniforms; ++i) {
		if (outUniforms[i] != -1) {
			// SDL_Log("skip: %d", i);
			continue;
		}
		// get name of uniform, then Location (not always equals to index)
		glGetActiveUniformName(program->id, i, SHADER_UNIFORM_MAX_LEN, &length,
		                       nameUniform);
		loc = glGetUniformLocation(program->id, nameUniform);
		data = Shader_uniformCreate(nameUniform, loc);
		if (!List_add(&program->uniformList, data)) {
			return false;
		}
		// SDL_Log("uniform %s:%d:%d", nameUniform, i, loc);
	}
	FREE(idUniforms);
	FREE(outUniforms);

	// UNIFORMS BLOCK
	//  get uniforms blocks
	glGetProgramiv(program->id, GL_ACTIVE_UNIFORM_BLOCKS, &nUniforms);
	idUniforms = ALLOC_ZERO(nUniforms, *idUniforms);
	CHECK_ALLOC(idUniforms, false);
	for (int i = 0; i < nUniforms; ++i) {
		idUniforms[i] = i;
	}
	for (int i = 0; i < nUniforms; ++i) {
		glGetActiveUniformBlockName(program->id, i, 128, &length, nameUniform);
		loc = glGetUniformBlockIndex(program->id, nameUniform);
		data = Shader_uniformCreate(nameUniform, loc);
		if (!List_add(&program->uniformBlockList, data)) {
			return false;
		}
		// SDL_Log("block %s:%d:%d", nameUniform, i, loc);
	}
	FREE(idUniforms);

	return true;
}

static UniformLocation _Shader_uniformGet(List *list, const char *name) {
	UniformData *data = NULL;
	ListItem *iter = list->head;
	while (iter) {
		data = iter->data;
		if (SDL_strcmp(name, data->name) == 0) {
			return data->id;
		}
		iter = iter->next;
	}
	return InvalidLocation;
}

UniformLocation Shader_uniformGet(ShaderProgram *program, const char *name) {
	return _Shader_uniformGet(&program->uniformList, name);
}

UniformLocation Shader_uniformBlockGet(ShaderProgram *program,
                                       const char *name) {
	return _Shader_uniformGet(&program->uniformBlockList, name);
}

void Shader_uniformDelete(UniformData *data) {
	assert(data != NULL);
	FREE(data->name);
	FREE(data);
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

bool Shader_init(ShaderProgram **program, List *loaderList) {
	assert(program != NULL);

	ProgramId programId = 0;

	*program = ALLOC_ZERO(1, ShaderProgram);
	CHECK_ALLOC(program, false)
	programId = Shader_programLoad(loaderList);
	if (programId == 0) {
		free(*program);
		*program = NULL;
		return false;
	}
	(*program)->id = programId;
	Shader_uniformLoad(*program);

	return true;
}

void Shader_free(ShaderProgram **program) {
	if (program == NULL || *program == NULL) {
		return;
	}
	Shader_programUnload((*program)->id);
	FREE(*program);
	*program = NULL;
}

bool Shader_loaderAddShader(List *loaderList, const char *path, GLenum type) {
	ShaderLoader *item = ALLOC_ZERO(1, *item);
	CHECK_ALLOC(item, false);
	item->filePath = strdup(path);
	CHECK_ALLOC(item->filePath, false);
	item->type = type;
	List_add(loaderList, item);
	return true;
}

void Shader_loaderDelete(ShaderLoader *shaderLoader) {
	assert(shaderLoader != NULL);

	FREE(shaderLoader->filePath);
	Shader_shaderUnload(shaderLoader->id);
	FREE(shaderLoader);
}

inline void Shader_setInt(UniformLocation id, int v) { glUniform1i(id, v); }
inline void Shader_setFloat(UniformLocation id, float v) { glUniform1f(id, v); }
inline void Shader_setVec2(UniformLocation id, vec2 v) {
	glUniform2f(id, v[0], v[1]);
}
inline void Shader_setVec3(UniformLocation id, vec3 v) {
	glUniform3f(id, v[0], v[1], v[2]);
}
inline void Shader_setVec4(UniformLocation id, vec4 v) {
	glUniform4f(id, v[0], v[1], v[2], v[3]);
}
inline void _Shader_setMat4(UniformLocation id, int n, bool transpose, mat4 v) {
	glUniformMatrix4fv(id, n, transpose, (float *)v);
}
