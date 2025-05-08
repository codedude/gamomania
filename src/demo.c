#include "demo.h"
#include "alloc.h"
#include "camera.h"
#include "cglm/util.h"
#include "list.h"
#include "material.h"
#include "shader.h"
#include "texture.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <cglm/cglm.h>

void processEvents(AppData *appData) {
	static unsigned long lastFrame = -1;
	static float lastMouseX = -1.f;
	static float lastMouseY = -1.f;
	Camera *cam = appData->cam;
	const unsigned long currentTime = SDL_GetTicks();
	const unsigned long deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;
	float cameraSpeed = 0.005f * (float)deltaTime;
	if (appData->event.speed) {
		cameraSpeed *= 1.5f;
	}

	if (appData->event.direction & (1 << FORWARD)) {
		Camera_moveForward(appData->cam, cameraSpeed);
	}
	if (appData->event.direction & (1 << BACK)) {
		Camera_moveBackward(appData->cam, cameraSpeed);
	}
	if (appData->event.direction & (1 << RIGHT)) {
		Camera_moveRight(appData->cam, cameraSpeed);
	}
	if (appData->event.direction & (1 << LEFT)) {
		Camera_moveLeft(appData->cam, cameraSpeed);
	}
	if (appData->event.direction & (1 << UP)) {
		Camera_moveUp(appData->cam, cameraSpeed);
	}
	if (appData->event.direction & (1 << DOWN)) {
		Camera_moveDown(appData->cam, cameraSpeed);
	}

	if (appData->event.rotate) {
		// if (lastMouseX < 0.f) {
		// 	lastMouseX = appData->event.mouseX;
		// 	lastMouseY = appData->event.mouseY;
		// }
		// float xoffset = appData->event.mouseX - lastMouseX; // to right
		// float yoffset = lastMouseY - appData->event.mouseY; // to up
		// lastMouseX = appData->event.mouseX;
		// lastMouseY = appData->event.mouseY;
		float sensitivity = 0.1f;
		float xoffset = appData->event.mouseX;
		float yoffset = appData->event.mouseY;
		Camera_rotate(cam, xoffset, yoffset);

		appData->event.rotate = false;
	}
}

void demoRender(AppData *appData) {
	vec3 cubePositions[] = {{0.0f, 0.0f, 0.0f},    {2.0f, 5.0f, -15.0f},
	                        {-1.5f, -2.2f, -2.5f}, {-3.8f, -2.0f, -12.3f},
	                        {2.4f, -0.4f, -3.5f},  {-1.7f, 3.0f, -7.5f},
	                        {1.3f, -2.0f, -2.5f},  {1.5f, 2.0f, -2.5f},
	                        {1.5f, 0.2f, -1.5f},   {-1.3f, 1.0f, -1.5f}};

	glUseProgram(appData->program->id);

	mat4 projectionMat = GLM_MAT4_IDENTITY;
	unsigned int modelLoc = Shader_uniformGet(appData->program, "model");
	unsigned int viewLoc = Shader_uniformGet(appData->program, "view");
	unsigned int projectionLoc =
	    Shader_uniformGet(appData->program, "projection");

	glm_perspective(glm_rad(appData->cam->fov), appData->cam->aspect,
	                appData->cam->nearZ, appData->cam->farZ, projectionMat);

	Shader_setMat4(viewLoc, appData->cam->lookAt);
	Shader_setMat4(projectionLoc, projectionMat);

	unsigned int viewPosLoc = Shader_uniformGet(appData->program, "viewPos");
	Shader_setVec3(viewPosLoc, appData->cam->pos);

	const float radius = 10.0f;
	float camX = sin(SDL_GetTicks() / 1000.) * radius;
	float camZ = cos(SDL_GetTicks() / 1000.) * radius;
	// Camera_setPos(appData->cam, (vec3){ camX, 0., camZ });
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, appData->program->container->texture.diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, appData->program->container->texture.specular);
	glBindVertexArray(appData->VAO);
	// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	for (int i = 0; i < 10; ++i) {
		mat4 modelMat = GLM_MAT4_IDENTITY;
		float angle = 16.66 * (i + 1);
		glm_translate(modelMat, cubePositions[i]);
		// glm_rotate(modelMat, SDL_GetTicks() / 1000.0 * glm_rad(angle),
		//            (vec3){1.0, 0.3, 0.5});
		Shader_setMat4(modelLoc, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glUseProgram(appData->lightProgram->id);
	unsigned int lightModelLoc =
	    glGetUniformLocation(appData->lightProgram->id, "model");
	unsigned int lightViewLoc =
	    glGetUniformLocation(appData->lightProgram->id, "view");
	unsigned int lightProjectionLoc =
	    glGetUniformLocation(appData->lightProgram->id, "projection");
	Shader_setMat4(lightViewLoc, appData->cam->lookAt);
	Shader_setMat4(lightProjectionLoc, projectionMat);

	mat4 lightModelMat = GLM_MAT4_IDENTITY;
	glm_translate(lightModelMat, (vec3){0.f, 1.f, -2.f});
	glm_scale(lightModelMat, (vec3){0.2f, 0.2f, 0.2f});
	Shader_setMat4(lightModelLoc, lightModelMat);
	glBindVertexArray(appData->lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

bool loadVertices(AppData *appData) {
	float vertices[] = {
	    // positions          // normals           // texture coords
	    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
	    -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
	    0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
	    1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
	    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

	    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
	    0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
	    0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	    1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
	    -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
	    0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
	    0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
	    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

	    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
	    -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
	    0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
	    0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
	    -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
	    -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
	    1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
	    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

	    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
	    -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
	    1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	    1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};
	// float vertices[] = {
	// 	// positions          // texture coords
	// 	0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
	// 	0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
	// 	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
	// 	-0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
	// };
	// unsigned int indices[] = {
	//     0, 1, 3, // first triangle
	//     1, 2, 3  // second triangle
	// };

	glEnable(GL_DEPTH_TEST);

	glGenBuffers(1, &appData->EBO);
	glGenBuffers(1, &appData->VBO);
	glGenVertexArrays(1, &appData->VAO);

	// 1. bind Vertex Array Object
	glBindVertexArray(appData->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, appData->VBO);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, appData->EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	//              GL_STATIC_DRAW);

	// 3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	                      (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	                      (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	                      (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &appData->lightVAO);
	glBindVertexArray(appData->lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, appData->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
	                      (void *)0);
	glEnableVertexAttribArray(0);

	Material *container =
	    Material_createTexture("container2.png", "container2_specular.png",
	                           (vec3){0.f, 0.f, 0.f}, 64.0f);
	appData->program->container = container;
	CHECK_ALLOC(container, false);

	glUseProgram(appData->program->id);
	// spot light
	// glUniform3f(Shader_uniformGet(appData->program, "light.pos"),
	//             appData->cam->pos[0], appData->cam->pos[1],
	//             appData->cam->pos[2]);
	// glUniform3f(Shader_uniformGet(appData->program,
	// "light.direction"),
	//             appData->cam->forward[0], appData->cam->forward[1],
	//             appData->cam->forward[2]);
	// Shader_setFloat(Shader_uniformGet(appData->program, "light.cutOff"),
	//             SDL_cosf(glm_rad(16.66f)));
	// Shader_setFloat(Shader_uniformGet(appData->program,
	// "light.outerCutOff"),
	//             SDL_cosf(glm_rad(20.0f)));

	// point light
	Shader_setVec3(Shader_uniformGet(appData->program, "light.pos"),
	               (vec3){0.f, 0.33f, -2.f});
	Shader_setFloat(Shader_uniformGet(appData->program, "light.constant"),
	                1.0f);
	Shader_setFloat(Shader_uniformGet(appData->program, "light.linear"), 0.09f);
	Shader_setFloat(Shader_uniformGet(appData->program, "light.quadratic"),
	                0.032f);

	// direction light
	// glUniform3f(Shader_uniformGet(appData->program,
	// "light.direction"),
	//             -0.2f, -1.0f, -0.3f);

	Shader_setVec3(Shader_uniformGet(appData->program, "light.ambient"),
	               (vec3){0.2f, 0.2f, 0.2f});
	Shader_setVec3(Shader_uniformGet(appData->program, "light.diffuse"),
	               (vec3){0.5f, 0.5f, 0.5f});
	Shader_setVec3(Shader_uniformGet(appData->program, "light.specular"),
	               (vec3){1.0f, 1.0f, 1.0f});

	// material
	Shader_setInt(Shader_uniformGet(appData->program, "material.diffuse"), 0);
	Shader_setInt(Shader_uniformGet(appData->program, "material.specular"), 1);
	Shader_setFloat(Shader_uniformGet(appData->program, "material.shininess"),
	                64.f);

	Camera_setPosAndDir(appData->cam, (vec3){0., 0., 3.}, (vec3){0., 0., -1.});

	return true;
}

void unloadVertices(AppData *appData) {
	glDeleteVertexArrays(1, &appData->VAO);
	glDeleteVertexArrays(1, &appData->lightVAO);
	glDeleteBuffers(1, &appData->EBO);
	glDeleteBuffers(1, &appData->VBO);
	Material_delete(appData->program->container);

	List_clear(&appData->program->uniformList, &Shader_uniformDelete);
	List_clear(&appData->program->uniformBlockList, &Shader_uniformDelete);
	List_clear(&appData->lightProgram->uniformList, &Shader_uniformDelete);
	List_clear(&appData->lightProgram->uniformBlockList, &Shader_uniformDelete);
}
