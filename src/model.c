#include "model.h"
#include "alloc.h"
#include "assimp/mesh.h"
#include "file.h"
#include "mesh.h"
#include <SDL3/SDL_log.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string.h>

// "asset/model/backpack/backpack.obj"
Model *Model_create(const char *path) {
	Model *model = ALLOC_ZERO(1, *model);
	CHECK_ALLOC(model, NULL);

	const char *fullPath = concatPath(PROJECT_PATH, MODEL_FOLDER, path);
	char *pathDirPos = strrchr(fullPath, '/');
	if (!pathDirPos) {
		SDL_Log("Model_create cant find dir path in %s", fullPath);
		return NULL;
	}
	model->directory = strndup(fullPath, pathDirPos - fullPath);

	const struct aiScene *scene = aiImportFile(
	    fullPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
	                  aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
	    !scene->mRootNode) {
		SDL_LogError(1, "Model_create::Assimp: %s", aiGetErrorString());
		return NULL;
	}
	FREE(fullPath);
	model->meshesLen = scene->mNumMeshes;
	model->meshes = ALLOC_ZERO(model->meshesLen, *model->meshes);
	CHECK_ALLOC(model->meshes, NULL);

	Model_processNode(model, scene->mRootNode, scene);
	aiReleaseImport(scene);

	return model;
}

bool Model_draw(const Model *model, const ShaderProgram *shader) {
	for (int i = 0; i < model->meshesLen; ++i) {
		Mesh_draw(&model->meshes[i], shader);
	}
	return true;
}

void Model_delete(Model *model) {
	RET_IF_NULL(model, );
	for (int i = 0; i < model->meshesLen; ++i) {
		Mesh_delete(&model->meshes[i]);
	}
	FREE(model->directory);
	FREE(model->meshes);
	FREE(model);
}

bool Model_processNode(Model *model, struct aiNode *node,
                       const struct aiScene *scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		Model_addMesh(model, mesh, scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		Model_processNode(model, node->mChildren[i], scene);
	}
	return true;
}

bool Model_addMesh(Model *model, struct aiMesh *mesh,
                   const struct aiScene *scene) {
	Vertex *vertices = NULL;
	unsigned int *indices = NULL;
	Material *materials = NULL;
	int verticesLen = 0;
	int indicesLen = 0;
	int materialsLen = 0;

	// vertices
	verticesLen = mesh->mNumVertices;
	vertices = ALLOC_ZERO(verticesLen, *vertices);
	CHECK_ALLOC(vertices, false);
	for (unsigned int i = 0; i < verticesLen; ++i) {
		vertices[i].pos[0] = mesh->mVertices[i].x;
		vertices[i].pos[1] = mesh->mVertices[i].y;
		vertices[i].pos[2] = mesh->mVertices[i].z;
		if (mesh->mNormals) {
			vertices[i].normal[0] = mesh->mNormals[i].x;
			vertices[i].normal[1] = mesh->mNormals[i].y;
			vertices[i].normal[2] = mesh->mNormals[i].z;
		}
		// any texutres ? TODO can have up to 8 textures
		if (mesh->mTextureCoords[0]) {
			vertices[i].texel[0] = mesh->mTextureCoords[0][i].x;
			vertices[i].texel[1] = mesh->mTextureCoords[0][i].y;
		} else {
			vertices[i].texel[0] = 0.f;
			vertices[i].texel[1] = 0.f;
		}
	}

	// indices
	// first count indices len to allocate
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		indicesLen += mesh->mFaces[i].mNumIndices;
	}
	indices = ALLOC(indicesLen, *indices);
	CHECK_ALLOC(indices, false);
	unsigned int indicePos = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		struct aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices[indicePos++] = face.mIndices[j];
		}
	}
	// materials
	// if (mesh->mMaterialIndex >= 0) {
	// }

	Mesh_set(&model->meshes[model->meshesFreeSlot++], vertices, verticesLen,
	         indices, indicesLen, materials, materialsLen);
	return true;
}
