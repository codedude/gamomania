#ifndef __LIGHT_H
#define __LIGHT_H

#include <cglm/cglm.h>

typedef struct {
	// attenuation
	float constant;
	float linear;
	float quadratic;
} PointLight;

typedef struct {
	// spot size
	float innerCutOff;
	float outerCutOff;
} SpotLight;

typedef struct {
	// no data yet
} DirectionalLight;

typedef enum : int { LIGHT_DIRECTIONAL, LIGHT_SPOT, LIGHT_POINT } eLightType;

typedef struct {
	vec3 diffuse;    // color emitted
	vec3 direction;  // 0,0,0 if point
	vec3 ambient;    // 0,0,0 if no ambient
	vec3 specular;   // 0,0,0 if no specular
	eLightType type; // type of light
	union {
		DirectionalLight specDirectional;
		SpotLight specSpot;
		PointLight specPoint;
	};
} Light;

Light *Light_createPointLight(vec3 diffuse, vec3 specular, vec3 ambient,
                              float constant, float linear, float quadratic);
Light *Light_createSpotLight(vec3 direction, vec3 diffuse, vec3 specular,
                             float innerCutOff, float outerCutOff);
Light *Light_createDirectionnalLight(vec3 direction, vec3 diffuse,
                                     vec3 specular, vec3 ambient);
void Light_delete(Light *light);

#endif
