#ifndef __MODEL__
#define __MODEL__

#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <assimp/cimport.h>
#include <assimp/scene.h>

#define MODEL_FOLDER "asset/model"

typedef struct {
	int meshesFreeSlot; // id of the first free slot
	int meshesLen;
	int materialsLen;
	const char *directory;
	TextureBank *texBank; // weak ptr, owned by appData
	Material *materials;
	Mesh *meshes;
} Model;

// recursive
bool Model_processMaterials(Model *model, const struct aiScene *scene);
bool Model_processNode(Model *model, struct aiNode *node,
                       const struct aiScene *scene);
bool Model_addMesh(Model *model, struct aiMesh *mesh,
                   const struct aiScene *scene);

const char *Model_getFilePath(Model *model, const char *filePath);

Model *Model_create(const char *path, TextureBank *texBank);
bool Model_draw(const Model *model, const ShaderProgram *shader);
void Model_delete(Model *model);

#endif
