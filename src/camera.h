#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "cglm/cglm.h"

typedef struct {
	mat4 lookAt;     // cache, updated each time pos or target are modified
	vec3 eulerAngle; // cache, pith/yaw/roll
	versor quat;     // quaternion used for rotation
	vec3 pos;        // in world space
	vec3 forward;    // normalized, ogl RH is z-
	vec3 up;         // normalized, ogl RH is y+
	vec3 right;      // normalized, ogl RH is x+
	float yaw;       // -360;360
	float pitch;     // -360;360
	float fov;
	float aspect; // aspect ratio
	float nearZ;
	float farZ;
} Camera;

// print vec
void printVec3(const char *t, vec3 v);

// Camera life cycle
Camera *Camera_create();
void Camera_destroy(Camera *cam);

// Settings
void Camera_setFov(Camera *cam, float fov);
void Camera_setAspect(Camera *cam, float aspect);
void Camera_setAspectFromViewport(Camera *cam, int width, int height);
void Camera_setZ(Camera *cam, float nearZ, float farZ);
void Camera_setPos(Camera *cam, vec3 pos);
void Camera_setDir(Camera *cam, vec3 dir);
void Camera_setDirFromTarget(Camera *cam, vec3 target);
void Camera_setPosAndDir(Camera *cam, vec3 pos, vec3 dir);
void Camera_setPosAndDirFromTarget(Camera *cam, vec3 pos, vec3 target);

// Movements
void Camera_moveForward(Camera *cam, float step);
void Camera_moveBackward(Camera *cam, float step);
void Camera_moveRight(Camera *cam, float step);
void Camera_moveLeft(Camera *cam, float step);
void Camera_moveUp(Camera *cam, float step);
void Camera_moveDown(Camera *cam, float step);

// Rotations
void Camera_rotate(Camera *cam, float yawStep, float pitchStep);
void Camera_rotateX(Camera *cam, float step);
void Camera_rotateY(Camera *cam, float step);
void Camera_rotateZ(Camera *cam, float step);

#endif
