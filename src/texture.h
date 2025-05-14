#ifndef __TEXTURE__H
#define __TEXTURE__H

static const char *TEXTURE_FOLDER = "asset/texture";
static const char *TEXUTRE_WHITE1X1_PATH = "white1x1.jpg";

typedef unsigned int TextureId;

#define TEXTURE_INVALID 0

typedef enum {
	TEXTURE_DIFFUSE,
	TEXTURE_BUMP,
	TEXTURE_SPECULAR,
	TEXTURE_HEIGHT,
} eTextureType;

typedef struct {
	unsigned int hash; // hash of the full path to check if already loaded
	eTextureType type;
	TextureId id;
} Texture;

typedef struct {
	int freeSlot;
	int texturesLen;
	TextureId texWhite; // special 1x1 white texture shared by all shaders
	Texture *textures;
} TextureBank;

TextureId Texture_createFromImg(const char *path, bool useSrgb);
void Texture_delete(TextureId textureId);

bool Texture_initBank(TextureBank *bank);
TextureId Texture_getTexInBank(TextureBank *texBank, const char *name);
TextureId Texture_addTexBank(TextureBank *bank, const char *name, bool useSrgb);
void Texture_deleteBank(TextureBank *bank);

#endif
