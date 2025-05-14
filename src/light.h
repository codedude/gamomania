#ifndef __LIGHT_H
#define __LIGHT_H

#include "shader.h"
#include <cglm/cglm.h>

#define LIGHT_MAX 16

typedef struct {
	// attenuation
	float constant;
	float linear;
	float quadratic;
} PointLight;

typedef struct {
	vec3 dir;
	// spot size
	float innerCutOff;
	float outerCutOff;
	// attenuation
	float constant;
	float linear;
	float quadratic;
} SpotLight;

typedef struct {
	vec3 dir;
} DirectionalLight;

typedef struct {
	// no data yet
} AmbientLight;

typedef struct {
	// no data yet
} AreaLight;

typedef enum : int {
	LIGHT_DIRECTIONAL,
	LIGHT_SPOT,
	LIGHT_POINT,
	// LIGHT_AREA,
	LIGHT_AMBIENT
} eLightType;

typedef struct {
	vec3 pos;
	vec3 color; // color emitted
	float intensity;
	eLightType type; // type of light
	union {
		DirectionalLight specDirectional;
		SpotLight specSpot;
		PointLight specPoint;
		AreaLight specArea;
		AmbientLight specAmbient;
	};
} Light;

typedef struct {
	int freeSpot; // todo proper array
	int lightsLen;
	Light **lights;
} SceneLight;

bool Light_init(SceneLight *scene);
bool Light_load(SceneLight *scene, ShaderProgram *shader);
void Light_delete(SceneLight *scene);

bool Light_addPointLight(SceneLight *scene, vec3 pos, vec3 color,
                         float intensity, float linear, float quadratic);
bool Light_addSpotLight(SceneLight *scene, vec3 pos, vec3 color,
                        float intensity, vec3 dir, float innerCutOff,
                        float outerCutOff);
bool Light_addDirectionalLight(SceneLight *scene, vec3 pos, vec3 color,
                               float intensity, vec3 dir);
bool Light_addAmbientLight(SceneLight *scene, vec3 pos, vec3 color,
                           float intensity);
void Light_deleteLight(Light *light);

#endif
