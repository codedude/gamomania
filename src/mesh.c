#include "mesh.h"
#include "alloc.h"
#include "gl_debug.h"
#include "material.h"
#include "shader.h"

Mesh *Mesh_create(Vertex *vertices, int verticesLen, unsigned int *indices,
                  int indicesLen, Material *materials, int materialsLen) {
	Mesh *mesh = ALLOC_ZERO(1, *mesh);
	CHECK_ALLOC(mesh, NULL);
	mesh->verticesLen = verticesLen;
	mesh->indicesLen = indicesLen;
	mesh->materialsLen = materialsLen;
	mesh->vertices = vertices;
	mesh->indices = indices;
	mesh->materials = materials;
	Mesh_load(mesh);
	return mesh;
}

bool Mesh_set(Mesh *mesh, Vertex *vertices, int verticesLen,
              unsigned int *indices, int indicesLen, Material *materials,
              int materialsLen) {
	mesh->verticesLen = verticesLen;
	mesh->indicesLen = indicesLen;
	mesh->materialsLen = materialsLen;
	mesh->vertices = vertices;
	mesh->indices = indices;
	mesh->materials = materials;
	Mesh_load(mesh);
	return true;
}

bool Mesh_load(Mesh *mesh) {
	assert(mesh != NULL);

	glGenVertexArrays(1, &mesh->VAO);
	glGenBuffers(1, &mesh->VBO);
	glGenBuffers(1, &mesh->EBO);

	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

	glBufferData(GL_ARRAY_BUFFER, mesh->verticesLen * sizeof(mesh->vertices[0]),
	             &mesh->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	             mesh->indicesLen * sizeof(mesh->indices[0]), &mesh->indices[0],
	             GL_STATIC_DRAW);

	// vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh->vertices[0]),
	                      (void *)0);
	// normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh->vertices[0]),
	                      (void *)offsetof(Vertex, normal));
	// texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh->vertices[0]),
	                      (void *)offsetof(Vertex, texel));

	glBindVertexArray(0);

	return glCheckError();
}

void Mesh_delete(Mesh *mesh) {
	RET_IF_NULL(mesh, );
	FREE(mesh->vertices);
	FREE(mesh->indices);
	for (int i = 0; i < mesh->materialsLen; ++i) {
		Material_delete(&mesh->materials[i]);
	}
	FREE(mesh->materials);
	glDeleteVertexArrays(1, &mesh->VAO);
	glDeleteBuffers(1, &mesh->EBO);
	glDeleteBuffers(1, &mesh->VBO);
}

bool Mesh_draw(const Mesh *mesh, const ShaderProgram *shader) {
	// todo manage texture
	glBindVertexArray(mesh->VAO);
	glDrawElements(GL_TRIANGLES, mesh->indicesLen, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	return glCheckError();
}
