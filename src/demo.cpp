#include "demo.hpp"
#include "camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "light.hpp"
#include "list.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

void processEvents(AppData *appData) {
	static unsigned long lastFrame = -1;
	Camera *cam = appData->cam;
	const unsigned long currentTime = SDL_GetTicks();
	const unsigned long deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;
	float cameraSpeed = 0.0025f * (float)deltaTime;
	if (appData->event.speed) {
		cameraSpeed *= 2.f;
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
		float xoffset = appData->event.mouseX;
		float yoffset = appData->event.mouseY;
		Camera_rotate(cam, xoffset, yoffset);
		appData->event.rotate = false;
	}
}

bool demoRender(AppData *appData) {
	glUseProgram(appData->program->id);

	glm::mat4 projectionMat = glm::identity<glm::mat4>();
	glm::mat4 modelMat = glm::identity<glm::mat4>();
	unsigned int modelLoc = Shader_uniformGet(appData->program, "model");
	unsigned int viewLoc = Shader_uniformGet(appData->program, "view");
	unsigned int projectionLoc =
	    Shader_uniformGet(appData->program, "projection");

	projectionMat =
	    glm::perspective(glm::radians(appData->cam->fov), appData->cam->aspect,
	                     appData->cam->nearZ, appData->cam->farZ);
	modelMat = glm::translate(modelMat, glm::vec3{0.f, 0.f, 0.f});
	modelMat = glm::scale(modelMat, glm::vec3{1.f, 1.f, 1.f});
	Shader_setMat4(modelLoc, modelMat);
	Shader_setMat4(viewLoc, appData->cam->lookAt);
	Shader_setMat4(projectionLoc, projectionMat);

	unsigned int viewPosLoc = Shader_uniformGet(appData->program, "viewPos");
	Shader_setVec3(viewPosLoc, appData->cam->pos);

	Model_draw(appData->model, appData->program);

	return true;
}

bool demoSetup(AppData *appData) {
	appData->cam = Camera_create();
	if (appData->cam == NULL) {
		SDL_Log("Error camera create");
		return false;
	}
	Camera_setFov(appData->cam, 75.0);
	Camera_setAspectFromViewport(appData->cam, appData->viewportWidth,
	                             appData->viewportHeight);
	Camera_setPosAndDirFromTarget(appData->cam, (glm::vec3){0., 0., 3.},
	                              (glm::vec3){0., 0., 0.});

	List shaderLoaderList = {};
	Shader_loaderAddShader(&shaderLoaderList, "modelVertex.glsl",
	                       GL_VERTEX_SHADER);
	Shader_loaderAddShader(&shaderLoaderList, "modelFragment.glsl",
	                       GL_FRAGMENT_SHADER);
	if (!Shader_init(&(appData->program), &shaderLoaderList)) {
		SDL_Log("Error shader_init");
		return false;
	}
	List_clear(&shaderLoaderList, &Shader_loaderDelete);

	if (!Texture_initBank(&appData->texBank)) {
		SDL_Log("Error Texture_initBank");
		return false;
	}

	// appData->model =
	// Model_create("ellie/ellie_animation.obj", &appData->texBank);
	// appData->model = Model_create("demo/terrain.fbx", &appData->texBank);
	appData->model = Model_create("backpack/backpack.obj", &appData->texBank);
	// appData->model =
	// Model_create("sylvanas/sylvanas.obj", &appData->texBank);

	if (!appData->model) {
		SDL_Log("Error Model create");
		return false;
	}

	appData->sceneLight.addAmbientLight(glm::vec3{0.f, 0.f, 0.f},
	                                    glm::vec3{1.f, 1.f, 1.f}, 0.15f);

	appData->sceneLight.addDirectionalLight(glm::vec3{0.f, 0.f, 0.f},
	                                        glm::vec3{1.f, 1.f, 1.f}, 1.f,
	                                        glm::vec3{0.5f, -0.5f, -0.5f});
	appData->sceneLight.addPointLight(glm::vec3{1.f, 0.5f, .5f},
	                                  glm::vec3{1.f, 1.f, 1.f}, 0.9f, 0.09f,
	                                  0.032f);
	appData->sceneLight.addPointLight(glm::vec3{0.f, 0.5f, .5f},
	                                  glm::vec3{1.f, 1.f, 1.f}, 0.9f, 0.09f,
	                                  0.032f);
	appData->sceneLight.addPointLight(glm::vec3{-1.f, 0.5f, .5f},
	                                  glm::vec3{1.f, 1.f, 1.f}, 0.9f, 0.09f,
	                                  0.032f);
	appData->sceneLight.load(appData->program);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	return true;
}

void demoDelete(AppData *appData) {
	List_clear(&appData->program->uniformList, &Shader_uniformDelete);
	List_clear(&appData->program->uniformBlockList, &Shader_uniformDelete);
	Camera_destroy(appData->cam);
	Texture_deleteBank(&appData->texBank);
	Model_delete(appData->model);
	Shader_free(&appData->program);
}
