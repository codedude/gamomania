#include "alloc.hpp"
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

// TODO: improve and count length first to alloc
const char *_concatPath(const char *base, ...) {
	assert(base != NULL);

	char *buffer = NULL;
	char *arg = NULL;
	const size_t maxPathLen = 1024;
	va_list args;

	buffer = (char *)ALLOC_ZERO(maxPathLen, char);
	CHECK_ALLOC(buffer, NULL);
	SDL_strlcat(buffer, base, maxPathLen);
	va_start(args, base);
	for (arg = va_arg(args, char *); arg; arg = va_arg(args, char *)) {
		SDL_strlcat(buffer, "/", maxPathLen);
		SDL_strlcat(buffer, arg, maxPathLen);
	}
	va_end(args);

	return buffer;
}

const char *readFile(const char *path) {
	assert(path != NULL);

	const char *data = NULL;
	size_t size = -1;

	data = (const char *)SDL_LoadFile(path, &size);
	if (!data || size < 0) {
		SDL_LogError(0, "SDL_LoadFile: %s", SDL_GetError());
		return NULL;
	}
	return data;
}
