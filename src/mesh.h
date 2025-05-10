#ifndef __MESH__
#define __MESH__

#include "material.h"
#include "shader.h"
#include <cglm/cglm.h>

typedef unsigned int GLBufferId;

typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 texel;
} Vertex;

typedef struct {
	GLBufferId VAO;
	GLBufferId VBO;
	GLBufferId EBO;
	int verticesLen;
	int indicesLen;
	int materialsLen;
	Material *materials;
	unsigned int *indices;
	Vertex *vertices;
} Mesh;

Mesh *Mesh_create(Vertex *vertices, int verticesLen, unsigned int *indices,
                  int indicesLen, Material *materials, int materialsLen);
bool Mesh_set(Mesh *mesh, Vertex *vertices, int verticesLen,
              unsigned int *indices, int indicesLen, Material *materials,
              int materialsLen);
bool Mesh_load(Mesh *mesh);
bool Mesh_draw(const Mesh *mesh, const ShaderProgram *shader);
void Mesh_delete(Mesh *mesh);

#endif
