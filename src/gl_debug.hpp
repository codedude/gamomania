#ifndef __GL_DEBUG__H
#define __GL_DEBUG__H

#include "shader.hpp"
#include <SDL3/SDL_log.h>

bool _glCheckError(const char *file, const char *function, int line);
void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                   GLsizei length, const char *message, const void *userParam);

#define glCheckError() _glCheckError(__FILE__, __FUNCTION__, __LINE__)

#endif
