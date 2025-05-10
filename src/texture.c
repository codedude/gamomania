#include "texture.h"
#include "alloc.h"
#include "file.h"
#include "gl_debug.h"
#include "shader.h"
#include <SDL3/SDL_stdinc.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

TextureId Texture_createFromImg(const char *path) {
	TextureId textureId = 0;
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	unsigned char *data = NULL;
	const char *texturePath = NULL;
	GLenum format = 0;

	texturePath = concatPath(PROJECT_PATH, TEXTURE_FOLDER, path);
	RET_IF_NULL(texturePath, 0);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	RET_IF_NULL(data, 0);
	FREE(texturePath);

	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glGenTextures(1, &textureId);
	if (glCheckError())
		return 0;
	glBindTexture(GL_TEXTURE_2D, textureId);
	if (glCheckError())
		return 0;
	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
	             GL_UNSIGNED_BYTE, data);
	if (glCheckError())
		return 0;
	glGenerateMipmap(GL_TEXTURE_2D);
	if (glCheckError())
		return 0;
	stbi_image_free(data);

	return textureId;
}

void Texture_delete(TextureId textureId) {
	if (textureId == 0)
		return;
	glDeleteTextures(1, &textureId);
}
