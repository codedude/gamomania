#ifndef __MODEL__
#define __MODEL__

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "mesh.h"
#include "shader.h"

#define MODEL_FOLDER "asset/model"

typedef struct {
	int meshesFreeSlot; // id of the first free slot
	int meshesLen;
	const char *directory;
	Mesh *meshes;
} Model;

// recursive
bool Model_processNode(Model *model, struct aiNode *node,
                       const struct aiScene *scene);
bool Model_addMesh(Model *model, struct aiMesh *mesh,
                   const struct aiScene *scene);

Model *Model_create(const char *path);
bool Model_draw(const Model *model, const ShaderProgram *shader);
void Model_delete(Model *model);

#endif
