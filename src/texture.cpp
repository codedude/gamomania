#include "texture.hpp"
#include "alloc.hpp"
#include "file.hpp"
#include "gl_debug.hpp"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

TextureId Texture_createFromImg(const char *path, bool useSrgb) {
	TextureId textureId = 0;
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	unsigned char *data = NULL;
	GLenum formatIn = 0;
	GLenum formatOut = 0;

	stbi_set_flip_vertically_on_load(true); // TODO
	data = stbi_load(path, &width, &height, &nrChannels, 0);
	RET_IF_NULL(data, 0);

	if (nrChannels == 1) {
		formatIn = GL_RED;
		formatOut = GL_RED;
	} else if (nrChannels == 3) {
		formatIn = GL_RGB;
		formatOut = GL_SRGB;
	} else if (nrChannels == 4) {
		formatIn = GL_RGBA;
		formatOut = GL_SRGB_ALPHA;
	}

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

	glTexImage2D(GL_TEXTURE_2D, 0, useSrgb ? formatOut : formatIn, width,
	             height, 0, formatIn, GL_UNSIGNED_BYTE, data);
	if (glCheckError())
		return 0;
	glGenerateMipmap(GL_TEXTURE_2D);
	if (glCheckError())
		return 0;
	stbi_image_free(data);

	return textureId;
}

void Texture_delete(TextureId textureId) {
	if (textureId == TEXTURE_INVALID)
		return;
	glDeleteTextures(1, &textureId);
}

TextureId Texture_getTexInBank(TextureBank *texBank, const char *name) {
	unsigned int texHash = std::hash<const char *>{}(name);
	for (int i = 0; i < texBank->texturesLen; ++i) {
		if (texBank->textures[i].hash == texHash) {
			return texBank->textures[i].id;
		}
	}
	return TEXTURE_INVALID;
}

TextureId Texture_addTexBank(TextureBank *bank, const char *name,
                             bool useSrgb) {
	assert(bank != NULL);

	TextureId texId = Texture_getTexInBank(bank, name);
	if (texId != TEXTURE_INVALID)
		return texId;
	texId = Texture_createFromImg(name, useSrgb);
	if (texId == TEXTURE_INVALID) {
		SDL_Log("Texture_addTexBank: Cannot load texture %s", name);
		return false;
	}
	bank->textures[bank->freeSlot].id = texId;
	bank->textures[bank->freeSlot].hash = std::hash<const char *>{}(name);
	bank->freeSlot++;

	return texId;
}

bool Texture_initBank(TextureBank *bank) {
	assert(bank != NULL);

	const char *texWhitePath = NULL;

	bank->freeSlot = 0;
	bank->texturesLen = 16;
	bank->textures = ALLOC_ZERO(bank->texturesLen, Texture);
	CHECK_ALLOC(bank->textures, false);
	texWhitePath =
	    concatPath(PROJECT_PATH, TEXTURE_FOLDER, TEXUTRE_WHITE1X1_PATH);
	bank->texWhite = Texture_createFromImg(texWhitePath, false);
	CHECK_ALLOC(bank->texWhite, false);
	FREE(texWhitePath);

	return true;
}

void Texture_deleteBank(TextureBank *bank) {
	if (!bank)
		return;
	if (bank->texturesLen > 0) {
		for (int i = 0; i < bank->freeSlot; ++i) {
			Texture_delete(bank->textures[i].id);
		}
	}
	Texture_delete(bank->texWhite);
	FREE(bank->textures);
}
