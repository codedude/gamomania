#include "light.h"
#include "alloc.h"

static Light *_createLight(vec3 direction, vec3 diffuse, vec3 specular,
                           vec3 ambient) {
	Light *light = calloc(1, sizeof(*light));
	CHECK_ALLOC(light, NULL)
	glm_vec3_copy(direction, light->direction);
	glm_vec3_copy(diffuse, light->diffuse);
	glm_vec3_copy(specular, light->specular);
	glm_vec3_copy(ambient, light->ambient);
	return light;
}

Light *Light_createPointLight(vec3 diffuse, vec3 specular, vec3 ambient,
                              float constant, float linear, float quadratic) {
	Light *light =
	    _createLight((vec3){0.f, 0.f, 0.f}, diffuse, specular, ambient);
	RET_IF_NULL(light, NULL);
	light->type = LIGHT_POINT;
	light->specPoint = (PointLight){
	    .constant = constant, .linear = linear, .quadratic = quadratic};
	return light;
}

Light *Light_createSpotLight(vec3 direction, vec3 diffuse, vec3 specular,
                             float innerCutOff, float outerCutOff) {
	Light *light =
	    _createLight(direction, diffuse, specular, (vec3){0.f, 0.f, 0.f});
	RET_IF_NULL(light, NULL);
	light->type = LIGHT_SPOT;
	light->specSpot =
	    (SpotLight){.innerCutOff = innerCutOff, .outerCutOff = outerCutOff};
	return light;
}

Light *Light_createDirectionnalLight(vec3 direction, vec3 diffuse,
                                     vec3 specular, vec3 ambient) {
	Light *light = _createLight(direction, diffuse, specular, ambient);
	RET_IF_NULL(light, NULL);
	light->type = LIGHT_DIRECTIONAL;
	light->specDirectional = (DirectionalLight){};
	return light;
}

void Light_delete(Light *light) { free(light); }
