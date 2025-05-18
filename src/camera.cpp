#include "camera.hpp"
#include "alloc.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <glm/glm.hpp>

void printVec3(const char *t, glm::vec3 v) {
	SDL_Log("%s => v={%.2f, %.2f, %.2f}", t, v[0], v[1], v[2]);
}

// lookAt to a dir, not a target
static void _updateLook(Camera *cam) {
	cam->lookAt = glm::lookAt(cam->pos, cam->pos + cam->forward, cam->up);
}

Camera *Camera_create() {
	Camera *cam = NULL;

	cam = ALLOC_ZERO(1, Camera);
	if (!cam)
		return NULL;

	cam->fov = 75.0;
	cam->aspect = 800. / 600.;
	cam->nearZ = 0.1;
	cam->farZ = 128.0;
	cam->eulerAngle = glm::vec3{0., -90., 0.}; // default no rot
	cam->pos = glm::vec3{0., 0., 3.};          // default pos at {0,0,3}
	cam->up = glm::vec3{0., 1., 0.}; // init at {0,1,0} to get right vector
	// target the center of the scene {0,0,0}
	// dir is reverse cause ogl is RH, to get right an up axis
	cam->forward = glm::normalize(glm::vec3{0., 0., 0.} - cam->pos);
	cam->right = glm::cross(cam->up, cam->forward);
	cam->up = glm::cross(cam->forward, cam->right);

	_updateLook(cam);

	return cam;
}

void Camera_destroy(Camera *cam) {
	if (cam)
		FREE(cam);
}

void Camera_setPos(Camera *cam, glm::vec3 pos) {
	cam->pos = pos;
	_updateLook(cam);
}

void Camera_setDir(Camera *cam, glm::vec3 dir) {
	cam->forward = dir;
	_updateLook(cam);
}

void Camera_setDirFromTarget(Camera *cam, glm::vec3 target) {
	cam->forward = glm::normalize(target - cam->pos);
	_updateLook(cam);
}

void Camera_setPosAndDirFromTarget(Camera *cam, glm::vec3 pos,
                                   glm::vec3 target) {
	cam->pos = pos;
	cam->forward = glm::normalize(target - pos);
	_updateLook(cam);
}

void Camera_setPosAndDir(Camera *cam, glm::vec3 pos, glm::vec3 dir) {
	cam->pos = pos;
	cam->forward = dir;
	_updateLook(cam);
}

void Camera_setFov(Camera *cam, float fov) { cam->fov = fov; }

void Camera_setAspect(Camera *cam, float aspect) { cam->aspect = aspect; }

void Camera_setAspectFromViewport(Camera *cam, int width, int height) {
	cam->aspect = (float)width / (float)height;
}

void Camera_setZ(Camera *cam, float nearZ, float farZ) {
	cam->nearZ = nearZ;
	cam->farZ = farZ;
}

void Camera_moveForward(Camera *cam, float step) {
	cam->pos += cam->forward * step;
	_updateLook(cam);
}

void Camera_moveBackward(Camera *cam, float step) {
	cam->pos -= cam->forward * step;
	_updateLook(cam);
}

void Camera_moveRight(Camera *cam, float step) {
	cam->pos += cam->right * step;
	_updateLook(cam);
}

void Camera_moveLeft(Camera *cam, float step) {
	cam->pos -= cam->right * step;
	_updateLook(cam);
}

void Camera_moveUp(Camera *cam, float step) {
	cam->pos += cam->up * step;
	_updateLook(cam);
}

void Camera_moveDown(Camera *cam, float step) {
	cam->pos -= cam->up * step;
	_updateLook(cam);
}

void Camera_rotate(Camera *cam, float yawStep, float pitchStep) {
	cam->eulerAngle.y += yawStep;
	cam->eulerAngle.x -= pitchStep;

	if (cam->eulerAngle.x > 75.f)
		cam->eulerAngle.x = 75.f;
	if (cam->eulerAngle.x < -75.f)
		cam->eulerAngle.x = -75.f;
	cam->eulerAngle.y = fmodf(cam->eulerAngle.y, 360.f);

	glm::vec3 tmp;
	tmp.x = cos(glm::radians(cam->eulerAngle.y)) *
	        cos(glm::radians(cam->eulerAngle.x));
	tmp.y = sin(glm::radians(cam->eulerAngle.x));
	tmp.z = sin(glm::radians(cam->eulerAngle.y)) *
	        cos(glm::radians(cam->eulerAngle.x));
	cam->forward = tmp;
	// update right
	cam->right = glm::normalize(glm::cross(cam->forward, cam->up));
	_updateLook(cam);
}
