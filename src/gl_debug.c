#include "gl_debug.h"
#include <SDL3/SDL_log.h>

bool _glCheckError(const char* file, const char* function, int line) {
	GLenum errorCode = GL_NO_ERROR;

	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		char buffer[32] = {};
		switch (errorCode) {
		case GL_INVALID_ENUM: strcat(buffer, "INVALID_ENUM"); break;
		case GL_INVALID_VALUE: strcat(buffer, "INVALID_VALUE"); break;
		case GL_INVALID_OPERATION: strcat(buffer, "INVALID_OPERATION"); break;
		case GL_STACK_OVERFLOW: strcat(buffer, "STACK_OVERFLOW"); break;
		case GL_STACK_UNDERFLOW: strcat(buffer, "STACK_UNDERFLOW"); break;
		case GL_OUT_OF_MEMORY: strcat(buffer, "OUT_OF_MEMORY"); break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			strcat(buffer, "INVALID_FRAMEBUFFER_OPERATION");
			break;
		default: strcat(buffer, "UNKNOWN"); break;
		}
		SDL_LogError(1, "OPENGL(%d):\t%s:%d: %s", errorCode, function, line, buffer);
	}

	return errorCode != GL_NO_ERROR;
}

void glDebugOutput(GLenum source,
				   GLenum type,
				   unsigned int id,
				   GLenum severity,
				   GLsizei length,
				   const char* message,
				   const void* userParam) {
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
		return;
	SDL_LogError(1, "OPENGL(%d):\t%s", id, message);

	// switch (source) {
	// case GL_DEBUG_SOURCE_API: std::cout << "Source: API"; break;
	// case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
	// 	std::cout << "Source: Window System";
	// 	break;
	// case GL_DEBUG_SOURCE_SHADER_COMPILER:
	// 	std::cout << "Source: Shader Compiler";
	// 	break;
	// case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "Source: Third Party"; break;
	// case GL_DEBUG_SOURCE_APPLICATION: std::cout << "Source: Application"; break;
	// case GL_DEBUG_SOURCE_OTHER: std::cout << "Source: Other"; break;
	// }
	// std::cout << std::endl;

	// switch (type) {
	// case GL_DEBUG_TYPE_ERROR: std::cout << "Type: Error"; break;
	// case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	// 	std::cout << "Type: Deprecated Behaviour";
	// 	break;
	// case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
	// 	std::cout << "Type: Undefined Behaviour";
	// 	break;
	// case GL_DEBUG_TYPE_PORTABILITY: std::cout << "Type: Portability"; break;
	// case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "Type: Performance"; break;
	// case GL_DEBUG_TYPE_MARKER: std::cout << "Type: Marker"; break;
	// case GL_DEBUG_TYPE_PUSH_GROUP: std::cout << "Type: Push Group"; break;
	// case GL_DEBUG_TYPE_POP_GROUP: std::cout << "Type: Pop Group"; break;
	// case GL_DEBUG_TYPE_OTHER: std::cout << "Type: Other"; break;
	// }
	// std::cout << std::endl;

	// switch (severity) {
	// case GL_DEBUG_SEVERITY_HIGH: std::cout << "Severity: high"; break;
	// case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "Severity: medium"; break;
	// case GL_DEBUG_SEVERITY_LOW: std::cout << "Severity: low"; break;
	// case GL_DEBUG_SEVERITY_NOTIFICATION:
	// 	std::cout << "Severity: notification";
	// 	break;
	// }
	// std::cout << std::endl;
	// std::cout << std::endl;
}
