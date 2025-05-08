#include "camera.h"
#include "cglm/euler.h"
#include "cglm/handed/euler_to_quat_rh.h"
#include "cglm/quat.h"
#include "cglm/util.h"
#include "cglm/vec3.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

void printVec3(const char *t, vec3 v) {
	SDL_Log("%s => v={%.2f, %.2f, %.2f}", t, v[0], v[1], v[2]);
}

// lookAt to a target, not a dir
static inline void _updateLookAt(Camera *cam, vec3 target) {
	vec3 dir;
	// glm_vec3_sub(target, cam->pos, dir);
	// glm_vec3_normalize_to(dir, cam->forward);
	glm_quat_look(cam->pos, cam->quat, cam->lookAt);
	// glm_look(cam->pos, cam->forward, cam->up, cam->lookAt);
}

// lookAt to a dir, not a target
static inline void _updateLook(Camera *cam) {
	glm_quat_look(cam->pos, cam->quat, cam->lookAt);
	// glm_look(cam->pos, cam->forward, cam->up, cam->lookAt);
}

Camera *Camera_create() {
	Camera *cam = NULL;

	cam = calloc(1, sizeof(*cam));
	if (!cam)
		return NULL;

	cam->fov = 45.0;
	cam->aspect = 800. / 600.;
	cam->nearZ = 0.1;
	cam->farZ = 128.0;
	cam->yaw = -90.f;
	cam->pitch = 0.f;
	glm_vec3_zero(cam->pos); // default pos at {0,0,3}
	cam->pos[2] = 3.;
	glm_vec3_zero(cam->up); // init at {0,1,0} to get right vector
	cam->up[1] = 1.;
	// target the center of the scene {0,0,0}
	glm_vec3_sub(cam->pos, (vec3){0., 0., 0.}, cam->forward);
	// dir is reverse cause ogl is RH, to get right an up axis
	glm_vec3_normalize(cam->forward);
	glm_vec3_crossn(cam->up, cam->forward, cam->right);
	glm_vec3_crossn(cam->forward, cam->right, cam->up);
	cam->forward[2] *= -1.0; // set direction back to z-

	cam->eulerAngle[0] = 0.f;
	cam->eulerAngle[1] = 0.f;
	cam->eulerAngle[2] = 0.f;
	versor quatX, quatY, quatZ, quatTmp;
	glm_quat(quatX, glm_rad(cam->eulerAngle[0]), 1.f, 0.f, 0.f);
	glm_quat(quatY, glm_rad(cam->eulerAngle[1]), 0.f, 1.f, 0.f);
	glm_quat(quatZ, glm_rad(cam->eulerAngle[2]), 0.f, 0.f, 1.f);
	glm_quat_mul(quatZ, quatY, quatTmp);
	glm_quat_mul(quatTmp, quatX, cam->quat);

	_updateLook(cam);

	return cam;
}

void Camera_destroy(Camera *cam) {
	if (cam)
		free(cam);
}

void inline Camera_setPos(Camera *cam, vec3 pos) {
	glm_vec3_dup(pos, cam->pos);
	_updateLook(cam);
}

void inline Camera_setDir(Camera *cam, vec3 dir) {
	glm_vec3_dup(dir, cam->forward);
	_updateLook(cam);
}

void inline Camera_setDirFromTarget(Camera *cam, vec3 target) {
	_updateLookAt(cam, target);
}

void inline Camera_setPosAndDirFromTarget(Camera *cam, vec3 pos, vec3 target) {
	glm_vec3_dup(pos, cam->pos);
	_updateLookAt(cam, target);
}

void inline Camera_setPosAndDir(Camera *cam, vec3 pos, vec3 dir) {
	glm_vec3_dup(pos, cam->pos);
	glm_vec3_dup(dir, cam->forward);
	_updateLook(cam);
}

void inline Camera_setFov(Camera *cam, float fov) { cam->fov = fov; }

void inline Camera_setAspect(Camera *cam, float aspect) {
	cam->aspect = aspect;
}

void Camera_setAspectFromViewport(Camera *cam, int width, int height) {
	cam->aspect = (float)width / (float)height;
}

void inline Camera_setZ(Camera *cam, float nearZ, float farZ) {
	cam->nearZ = nearZ;
	cam->farZ = farZ;
}

void inline Camera_moveForward(Camera *cam, float step) {
	vec3 qvec, tmp, dir;
	qvec[0] = cam->quat[0];
	qvec[1] = cam->quat[1];
	qvec[2] = cam->quat[2];
	glm_vec3_scale(cam->forward, cam->quat[3], dir);
	glm_vec3_cross(qvec, cam->forward, tmp);
	glm_vec3_add(tmp, cam->forward, tmp);
	glm_vec3_cross(qvec, tmp, tmp);
	glm_vec3_scale(tmp, 2.0, tmp);
	glm_vec3_add(tmp, cam->forward, dir);

	// v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
	glm_vec3_muladds(dir, step, cam->pos);
	_updateLook(cam);
}

void inline Camera_moveBackward(Camera *cam, float step) {
	glm_vec3_mulsubs(cam->forward, step, cam->pos);
	_updateLook(cam);
}

void inline Camera_moveRight(Camera *cam, float step) {
	glm_vec3_muladds(cam->right, step, cam->pos);
	_updateLook(cam);
}

void inline Camera_moveLeft(Camera *cam, float step) {
	glm_vec3_mulsubs(cam->right, step, cam->pos);
	_updateLook(cam);
}

void inline Camera_moveUp(Camera *cam, float step) {
	glm_vec3_muladds(cam->up, step, cam->pos);
	_updateLook(cam);
}

void inline Camera_moveDown(Camera *cam, float step) {
	glm_vec3_mulsubs(cam->up, step, cam->pos);
	_updateLook(cam);
}

void inline Camera_rotate(Camera *cam, float yawStep, float pitchStep) {
	// cam->yaw += yawStep;
	// cam->pitch += pitchStep;
	// SDL_Log("cam: %.2f, %.2f", cam->eulerAngle[0], cam->eulerAngle[1]);

	// if (cam->pitch > 89.99f)
	// 	cam->pitch = 89.99f;
	// if (cam->pitch < -89.99f)
	// 	cam->pitch = -89.99f;
	// if (cam->yaw > 89.99f)
	// 	cam->yaw = 89.99f;
	// if (cam->yaw < -89.99f)
	// 	cam->yaw = -89.99f;

	// vec3 direction;
	// direction[0] = cos(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
	// direction[1] = sin(glm_rad(cam->pitch));
	// direction[2] = sin(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
	// glm_normalize_to(direction, cam->forward);
	cam->eulerAngle[0] -= pitchStep;
	cam->eulerAngle[1] -= yawStep;
	cam->eulerAngle[1] = SDL_fmodf(cam->eulerAngle[1], 360.f);
	if (cam->eulerAngle[0] > 89.99f)
		cam->eulerAngle[0] = 89.99f;
	if (cam->eulerAngle[0] < -89.99f)
		cam->eulerAngle[0] = -89.99f;
	versor quatX, quatY, quatZ, quatTmp;
	glm_quat(quatX, glm_rad(cam->eulerAngle[0]), 1.f, 0.f, 0.f);
	glm_quat(quatY, glm_rad(cam->eulerAngle[1]), 0.f, 1.f, 0.f);
	glm_quat(quatZ, glm_rad(cam->eulerAngle[2]), 0.f, 0.f, 1.f);
	// z * y * x important, if not rotation is absolute not relative
	glm_quat_mul(quatZ, quatY, quatTmp);
	glm_quat_mul(quatTmp, quatX, cam->quat);
	_updateLook(cam);
}
