#include "material.hpp"
#include "texture.hpp"
#include <SDL3/SDL_log.h>
#include <string.h>

bool Material_init(Material *mat, const char *name, TextureId texWhite1x1) {
	memset(mat, 0, sizeof(*mat));
	strncpy(mat->name, name, sizeof(mat->name) - 1);
	mat->opacity = 1.0f;
	mat->texWhite1x1 = texWhite1x1;
	return true;
}

bool Material_addTexture(Material *mat, TextureBank *texBank, const char *name,
                         eTextureType type) {
	TextureId tex = Texture_addTexBank(texBank, name, type == TEXTURE_DIFFUSE);
	if (!tex) {
		SDL_Log("Material_textureAdd: Cannot load texture %s", name);
		return false;
	}
	switch (type) {
	case TEXTURE_DIFFUSE:
		mat->texDiffuseMap = tex;
		break;
	case TEXTURE_SPECULAR:
		mat->texSpecularMap = tex;
		break;
	case TEXTURE_BUMP:
		mat->texBumpMap = tex;
		break;
	case TEXTURE_HEIGHT:
		mat->texHeightMap = tex;
		break;
	default: // must never happens
		assert(false);
		break;
	}
	return true;
}

void Material_delete(Material *mat) {
	if (!mat)
		return;
}

void Material_setReflectivity(Material *mat, float v) { mat->reflectivity = v; }
void Material_setOpacity(Material *mat, float v) { mat->opacity = v; }
void Material_setRefraction(Material *mat, float v) { mat->refraction = v; }
void Material_setShininess(Material *mat, float v) { mat->shininess = v; }
void Material_setEmissionStrength(Material *mat, float v) {
	mat->emissionStrength = v;
}
void Material_setColor(Material *mat, glm::vec3 v) { mat->color = v; }
void Material_setSpecular(Material *mat, glm::vec3 v) { mat->specular = v; }
void Material_setEmission(Material *mat, glm::vec3 v) { mat->emission = v; }
