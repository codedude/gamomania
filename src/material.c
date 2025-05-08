#include "material.h"
#include "alloc.h"
#include "texture.h"
#include <SDL3/SDL_log.h>

static Material *_createMaterial(vec3 ambient, float shininess) {
	Material *mat = ALLOC_ZERO(1, *mat);
	CHECK_ALLOC(mat, NULL);
	mat->shininess = shininess;
	glm_vec3_copy(ambient, mat->ambient);
	return mat;
}

Material *Material_createRaw(vec3 diffuse, vec3 specular, vec3 ambient,
                             float shininess) {
	Material *mat = _createMaterial(ambient, shininess);
	RET_IF_NULL(mat, NULL);
	mat->type = MATERIAL_RAW;
	glm_vec3_copy(diffuse, mat->raw.diffuse);
	glm_vec3_copy(specular, mat->raw.specular);
	return mat;
}

Material *Material_createTexture(const char *diffuse, const char *specular,
                                 vec3 ambient, float shininess) {
	Material *mat = _createMaterial(ambient, shininess);
	RET_IF_NULL(mat, NULL);
	mat->type = MATERIAL_TEXTURE;
	mat->texture.diffuse = Texture_createFromImg(diffuse);
	if (!mat->texture.diffuse) {
		SDL_Log("Cannot load texture %s", diffuse);
		return NULL;
	}
	mat->texture.specular = Texture_createFromImg(specular);
	if (!mat->texture.specular) {
		SDL_Log("Cannot load texture %s", specular);
		return NULL;
	}
	return mat;
}

void Material_delete(Material *mat) {
	if (!mat)
		return;
	if (mat->type == MATERIAL_TEXTURE) {
		Texture_delete(mat->texture.diffuse);
		Texture_delete(mat->texture.specular);
	}
	FREE(mat);
}
