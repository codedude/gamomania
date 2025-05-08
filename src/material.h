#ifndef __MATERIAL__H
#define __MATERIAL__H

#include "texture.h"
#include <cglm/cglm.h>

typedef struct {
	TextureId diffuse;
	TextureId specular;
} MaterialTexture;

typedef struct {
	vec3 diffuse;
	vec3 specular;
} MaterialRaw;

typedef enum : int { MATERIAL_RAW, MATERIAL_TEXTURE } eMaterialType;

typedef struct {
	vec3 ambient;
	float shininess;
	eMaterialType type;
	union {
		MaterialRaw raw;
		MaterialTexture texture;
	};
} Material;

Material *Material_createRaw(vec3 diffuse, vec3 specular, vec3 ambient,
                             float shininess);
Material *Material_createTexture(const char *diffuse, const char *specular,
                                 vec3 ambient, float shininess);
void Material_delete(Material *material);

#endif
