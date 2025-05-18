#include "model.hpp"
#include "alloc.hpp"
#include "file.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <SDL3/SDL_log.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <string.h>

// "asset/model/backpack/backpack.obj"
Model *Model_create(const char *path, TextureBank *texBank) {
	Model *model = ALLOC_ZERO(1, Model);
	CHECK_ALLOC(model, NULL);

	const char *fullPath = concatPath(PROJECT_PATH, MODEL_FOLDER, path);
	const char *pathDirPos = strrchr(fullPath, '/');
	if (!pathDirPos) {
		SDL_Log("Model_create cant find dir path in %s", fullPath);
		return NULL;
	}
	model->directory = strndup(fullPath, pathDirPos - fullPath);

	const struct aiScene *scene = aiImportFile(
	    fullPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
	                  aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
	                  aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
	    !scene->mRootNode) {
		SDL_LogError(1, "Model_create::Assimp: %s", aiGetErrorString());
		return NULL;
	}
	FREE(fullPath);
	model->meshesLen = scene->mNumMeshes;
	model->meshes = ALLOC_ZERO(model->meshesLen, Mesh);
	CHECK_ALLOC(model->meshes, NULL);

	// first to populate materials array to meshes
	model->texBank = texBank;
	if (!Model_processMaterials(model, scene)) {
		return NULL;
	}
	if (!Model_processNode(model, scene->mRootNode, scene)) {
		return NULL;
	}
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
	for (int i = 0; i < model->materialsLen; ++i) {
		Material_delete(&model->materials[i]);
	}
	FREE(model->materials);
	FREE(model);
}

bool Model_processMaterials(Model *model, const struct aiScene *scene) {
	model->materialsLen = scene->mNumMaterials;
	model->materials = ALLOC_ZERO(model->materialsLen, Material);
	CHECK_ALLOC(model->materials, false);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
		Material *localMat = &model->materials[i];
		const struct aiMaterial *newMat = scene->mMaterials[i];
		struct aiString matName = {};
		aiGetMaterialString(newMat, AI_MATKEY_NAME, &matName);
		Material_init(localMat, matName.data, model->texBank->texWhite);
		aiColor4D buff4d = {};
		float buffFloat = 0.f;
		struct aiString buffString = {};
		for (unsigned int j = 0; j < newMat->mNumProperties; ++j) {
			if (AI_SUCCESS ==
			    aiGetMaterialColor(newMat, AI_MATKEY_COLOR_DIFFUSE, &buff4d)) {
				localMat->color[0] = buff4d.r;
				localMat->color[1] = buff4d.g;
				localMat->color[2] = buff4d.b;
			}
			// if (AI_SUCCESS ==
			//     aiGetMaterialColor(newMat, AI_MATKEY_COLOR_AMBIENT, &buff4d))
			//     {
			// 	localMat->ambient[0] = buff4d.r;
			// 	localMat->ambient[1] = buff4d.g;
			// 	localMat->ambient[2] = buff4d.b;
			// }
			if (AI_SUCCESS ==
			    aiGetMaterialColor(newMat, AI_MATKEY_COLOR_SPECULAR, &buff4d)) {
				localMat->specular[0] = buff4d.r;
				localMat->specular[1] = buff4d.g;
				localMat->specular[2] = buff4d.b;
			}
			if (AI_SUCCESS ==
			    aiGetMaterialFloat(newMat, AI_MATKEY_SHININESS, &buffFloat)) {
				localMat->shininess = buffFloat;
			}

			unsigned int texCount;
			enum aiTextureMapping texMapping;
			unsigned int uvIndex;
			ai_real blend;
			enum aiTextureOp texOp;
			enum aiTextureMapMode texMap;
			unsigned int flags;

			// TODO : identical for loops
			// TODO : only 1 texture now
			texCount = aiGetMaterialTextureCount(newMat, aiTextureType_DIFFUSE);
			if (texCount > 0) {
				for (auto t = 0; t < texCount; ++t) {
					if (AI_SUCCESS ==
					    aiGetMaterialTexture(newMat, aiTextureType_DIFFUSE, t,
					                         &buffString, &texMapping, &uvIndex,
					                         &blend, &texOp, &texMap, &flags)) {
						const char *texPath =
						    Model_getFilePath(model, buffString.data);
						if (!Material_addTexture(localMat, model->texBank,
						                         texPath, TEXTURE_DIFFUSE)) {
							SDL_LogError(1, "Cannot load texture %s", texPath);
							return false;
						}
						FREE(texPath);
					}
				}
			}
			texCount =
			    aiGetMaterialTextureCount(newMat, aiTextureType_SPECULAR);
			if (texCount > 0) {
				for (auto t = 0; t < texCount; ++t) {
					if (AI_SUCCESS ==
					    aiGetMaterialTexture(newMat, aiTextureType_SPECULAR, t,
					                         &buffString, &texMapping, &uvIndex,
					                         &blend, &texOp, &texMap, &flags)) {
						const char *texPath =
						    Model_getFilePath(model, buffString.data);

						if (!Material_addTexture(localMat, model->texBank,
						                         texPath, TEXTURE_SPECULAR)) {
							SDL_LogError(1,
							             "Model proces: Cannot load texture %s",
							             model->directory);
							return false;
						}
						FREE(texPath);
					}
				}
			}
		}
	}

	return true;
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
	int verticesLen = 0;
	int indicesLen = 0;

	// vertices
	verticesLen = mesh->mNumVertices;
	vertices = ALLOC_ZERO(verticesLen, Vertex);
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
			// todo, support up to 8 texture channel
			vertices[i].texel[0] = mesh->mTextureCoords[0][i].x;
			vertices[i].texel[1] = mesh->mTextureCoords[0][i].y;
			vertices[i].tan[0] = mesh->mTangents[i].x;
			vertices[i].tan[1] = mesh->mTangents[i].y;
			vertices[i].tan[2] = mesh->mTangents[i].z;
			vertices[i].bitan[0] = mesh->mBitangents[i].x;
			vertices[i].bitan[1] = mesh->mBitangents[i].y;
			vertices[i].bitan[2] = mesh->mBitangents[i].z;
		} // already zero alloc
	}

	// indices
	// first count indices len to allocate
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		indicesLen += mesh->mFaces[i].mNumIndices;
	}
	indices = ALLOC_ZERO(indicesLen, unsigned int);
	CHECK_ALLOC(indices, false);
	unsigned int indicePos = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		struct aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices[indicePos++] = face.mIndices[j];
		}
	}
	Mesh_set(&model->meshes[model->meshesFreeSlot], vertices, verticesLen,
	         indices, indicesLen, mesh->mMaterialIndex);
	model->meshes[model->meshesFreeSlot].materials = model->materials;
	++model->meshesFreeSlot;

	return true;
}

const char *Model_getFilePath(Model *model, const char *filePath) {
	return concatPath(model->directory, filePath);
}
