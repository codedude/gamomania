#ifndef __MATERIAL__H
#define __MATERIAL__H

#include "texture.hpp"
#include <glm/vec3.hpp>

static const char *MATERIAL_FOLDER = "asset/material";

typedef enum : int { MATERIAL_TYPE } eMaterialType;

typedef struct {
	char name[64];      // used for logs, editor and scene file
	glm::vec3 color;    // base color
	glm::vec3 specular; // specular color
	glm::vec3 emission; // emission color
	float reflectivity;
	float opacity;
	float refraction;
	float shininess;
	float emissionStrength;
	eMaterialType type; // not used
	// only one per type for now, maybe manage multiple tex later
	TextureId texWhite1x1;    // weak id, owned by TextureBank
	TextureId texDiffuseMap;  // weak id, owned by TextureBank
	TextureId texSpecularMap; // weak id, owned by TextureBank
	TextureId texBumpMap;     // weak id, owned by TextureBank
	TextureId texHeightMap;   // weak id, owned by TextureBank
} Material;

typedef struct {
	int freeSpot;
	int materialsLen;
	Material *materials;  // owner
	TextureBank *texBank; // weak ptr
} MaterialBank;           // per model

bool Material_init(Material *mat, const char *name, TextureId texWhite1x1);
bool Material_addTexture(Material *mat, TextureBank *texBank, const char *name,
                         eTextureType type);
void Material_delete(Material *mat);

void Material_setReflectivity(Material *mat, float v);
void Material_setOpacity(Material *mat, float v);
void Material_setRefraction(Material *mat, float v);
void Material_setShininess(Material *mat, float v);
void Material_setEmissionStrength(Material *mat, float v);
void Material_setColor(Material *mat, glm::vec3 v);
void Material_setSpecular(Material *mat, glm::vec3 v);
void Material_setEmission(Material *mat, glm::vec3 v);

#endif
