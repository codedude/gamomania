#ifndef __TEXTURE__H
#define __TEXTURE__H

static const char *TEXTURE_FOLDER = "asset/texture";

typedef unsigned int TextureId;

TextureId Texture_createFromImg(const char *path);
void Texture_delete(TextureId textureId);

#endif
