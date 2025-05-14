#ifndef __MESH__
#define __MESH__

#include "shader.h"
#include <cglm/cglm.h>

typedef unsigned int GLBufferId;

typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 texel;
	vec3 tan;
	vec3 bitan;
} Vertex;

typedef struct {
	GLBufferId VAO;
	GLBufferId VBO;
	GLBufferId EBO;
	int verticesLen;
	int indicesLen;
	int materialId;      // material in Model, <0 = no mat
	Material *materials; // weak ptr to parent model
	unsigned int *indices;
	Vertex *vertices;
} Mesh;

Mesh *Mesh_create(Vertex *vertices, int verticesLen, unsigned int *indices,
                  int indicesLen, int materialId);
bool Mesh_set(Mesh *mesh, Vertex *vertices, int verticesLen,
              unsigned int *indices, int indicesLen, int materialId);
bool Mesh_load(Mesh *mesh);
bool Mesh_draw(const Mesh *mesh, const ShaderProgram *shader);
void Mesh_delete(Mesh *mesh);

#endif
