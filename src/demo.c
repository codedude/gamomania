#include "demo.h"
#include "camera.h"
#include "light.h"
#include "list.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <cglm/cglm.h>

void processEvents(AppData *appData) {
	static unsigned long lastFrame = -1;
	Camera *cam = appData->cam;
	const unsigned long currentTime = SDL_GetTicks();
	const unsigned long deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;
	float cameraSpeed = 0.001f * (float)deltaTime;
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

	mat4 projectionMat = GLM_MAT4_IDENTITY;
	mat4 modelMat = GLM_MAT4_IDENTITY;
	unsigned int modelLoc = Shader_uniformGet(appData->program, "model");
	unsigned int viewLoc = Shader_uniformGet(appData->program, "view");
	unsigned int projectionLoc =
	    Shader_uniformGet(appData->program, "projection");

	glm_perspective(glm_rad(appData->cam->fov), appData->cam->aspect,
	                appData->cam->nearZ, appData->cam->farZ, projectionMat);
	glm_translate(modelMat, (vec3){0.f, 0.f, 0.f});
	glm_scale(modelMat, (vec3){1.f, 1.f, 1.f});
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
	Camera_setPosAndDir(appData->cam, (vec3){0., 0., 3.}, (vec3){0., 0., -1.});

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
	appData->model = Model_create("backpack/backpack.obj", &appData->texBank);
	// appData->model = Model_create("sylvanas/sylvanas.obj",
	// &appData->texBank);

	if (!appData->model) {
		SDL_Log("Error Model create");
		return false;
	}

	if (!Light_init(&appData->sceneLight)) {
		SDL_Log("Error Light_init");
		return false;
	}

	Light_addAmbientLight(&appData->sceneLight, (vec3){0.f, 0.f, 0.f},
	                      (vec3){1.f, 1.f, 1.f}, 0.15f);

	Light_addDirectionalLight(&appData->sceneLight, (vec3){0.f, 0.f, 0.f},
	                          (vec3){1.f, 1.f, 1.f}, 1.f,
	                          (vec3){0.5f, -0.5f, -0.5f});
	Light_addPointLight(&appData->sceneLight, (vec3){1.f, 0.5f, .5f},
	                    (vec3){1.f, 1.f, 1.f}, 0.9f, 0.09f, 0.032f);
	Light_addPointLight(&appData->sceneLight, (vec3){0.f, 0.5f, .5f},
	                    (vec3){1.f, 1.f, 1.f}, 0.9f, 0.09f, 0.032f);
	Light_addPointLight(&appData->sceneLight, (vec3){-1.f, 0.5f, .5f},
	                    (vec3){1.f, 1.f, 1.f}, 0.9f, 0.09f, 0.032f);
	Light_load(&appData->sceneLight, appData->program);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	return true;
}

void demoDelete(AppData *appData) {
	List_clear(&appData->program->uniformList, &Shader_uniformDelete);
	List_clear(&appData->program->uniformBlockList, &Shader_uniformDelete);
	Light_delete(&appData->sceneLight);
	Camera_destroy(appData->cam);
	Texture_deleteBank(&appData->texBank);
	Model_delete(appData->model);
	Shader_free(&appData->program);
}
