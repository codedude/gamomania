#include "light.h"
#include "alloc.h"
#include <SDL3/SDL_log.h>

bool Light_load(SceneLight *scene, ShaderProgram *shader) {
	int nPointLight = 0;
	int nSpotLight = 0;
	int nDirLight = 0;
	int nAmbientLight = 0;

#define BUFF_SIZE 128
	char buff[BUFF_SIZE] = {};

	for (int i = 0; i < scene->freeSpot; ++i) {
		Light *light = scene->lights[i];
		if (light->type == LIGHT_DIRECTIONAL) {
			snprintf(buff, BUFF_SIZE, "directionalLights[%d].pos", nDirLight);
			Shader_setVec3(Shader_uniformGet(shader, buff), light->pos);
			snprintf(buff, BUFF_SIZE, "directionalLights[%d].color", nDirLight);
			Shader_setVec3(Shader_uniformGet(shader, buff), light->color);
			snprintf(buff, BUFF_SIZE, "directionalLights[%d].intensity",
			         nDirLight);
			Shader_setFloat(Shader_uniformGet(shader, buff), light->intensity);
			snprintf(buff, BUFF_SIZE, "directionalLights[%d].dir", nDirLight);
			Shader_setVec3(Shader_uniformGet(shader, buff),
			               light->specDirectional.dir);
			++nDirLight;
		} else if (light->type == LIGHT_POINT) {
			snprintf(buff, BUFF_SIZE, "pointLights[%d].pos", nPointLight);
			Shader_setVec3(Shader_uniformGet(shader, buff), light->pos);
			snprintf(buff, BUFF_SIZE, "pointLights[%d].color", nPointLight);
			Shader_setVec3(Shader_uniformGet(shader, buff), light->color);
			snprintf(buff, BUFF_SIZE, "pointLights[%d].intensity", nPointLight);
			Shader_setFloat(Shader_uniformGet(shader, buff), light->intensity);
			snprintf(buff, BUFF_SIZE, "pointLights[%d].constant", nPointLight);
			Shader_setFloat(Shader_uniformGet(shader, buff),
			                light->specPoint.constant);
			snprintf(buff, BUFF_SIZE, "pointLights[%d].linear", nPointLight);
			Shader_setFloat(Shader_uniformGet(shader, buff),
			                light->specPoint.linear);
			snprintf(buff, BUFF_SIZE, "pointLights[%d].quadratic", nPointLight);
			Shader_setFloat(Shader_uniformGet(shader, buff),
			                light->specPoint.quadratic);
			++nPointLight;
		} else if (light->type == LIGHT_AMBIENT) {
			snprintf(buff, BUFF_SIZE, "ambientLights[%d].color", nAmbientLight);
			Shader_setVec3(Shader_uniformGet(shader, buff), light->color);
			snprintf(buff, BUFF_SIZE, "ambientLights[%d].intensity",
			         nAmbientLight);
			Shader_setFloat(Shader_uniformGet(shader, buff), light->intensity);
			++nAmbientLight;
		} else if (light->type == LIGHT_SPOT) {
			++nSpotLight;
		}
	}
	Shader_setInt(Shader_uniformGet(shader, "pointLightsLen"), nPointLight);
	Shader_setInt(Shader_uniformGet(shader, "spotLightsLen"), nSpotLight);
	Shader_setInt(Shader_uniformGet(shader, "directionalLightsLen"), nDirLight);
	Shader_setInt(Shader_uniformGet(shader, "ambientLightsLen"), nAmbientLight);

	return true;
}

bool Light_init(SceneLight *scene) {
	scene->lightsLen = LIGHT_MAX;
	scene->lights = ALLOC_ZERO(scene->lightsLen, *scene->lights);
	CHECK_ALLOC(scene->lights, false);
	return true;
}

void Light_delete(SceneLight *scene) {
	for (int i = 0; i < scene->freeSpot; ++i) {
		Light_deleteLight(scene->lights[i]);
	}
	if (scene->lightsLen > 0)
		FREE(scene->lights);
}

static bool _addLightToScene(SceneLight *scene, Light *light) {
	if (scene->freeSpot == scene->lightsLen) {
		SDL_Log("Max number of light reached");
		return false;
	}
	scene->lights[scene->freeSpot++] = light;
	return true;
}

static Light *_createLight(vec3 pos, vec3 color, float intensity,
                           eLightType type) {
	Light *light = calloc(1, sizeof(*light));
	CHECK_ALLOC(light, NULL)
	glm_vec3_copy(pos, light->pos);
	glm_vec3_copy(color, light->color);
	light->intensity = intensity;
	light->type = type;
	return light;
}

bool Light_addPointLight(SceneLight *scene, vec3 pos, vec3 color,
                         float intensity, float linear, float quadratic) {
	Light *light = _createLight(pos, color, intensity, LIGHT_POINT);
	RET_IF_NULL(light, NULL);
	light->specPoint =
	    (PointLight){.constant = 1.0, .linear = linear, .quadratic = quadratic};
	return _addLightToScene(scene, light);
}

bool Light_addSpotLight(SceneLight *scene, vec3 pos, vec3 color,
                        float intensity, vec3 direction, float innerCutOff,
                        float outerCutOff) {
	Light *light = _createLight(pos, color, intensity, LIGHT_SPOT);
	RET_IF_NULL(light, NULL);
	light->specSpot =
	    (SpotLight){.innerCutOff = innerCutOff, .outerCutOff = outerCutOff};
	return _addLightToScene(scene, light);
}

bool Light_addDirectionalLight(SceneLight *scene, vec3 pos, vec3 color,
                               float intensity, vec3 dir) {
	Light *light = _createLight(pos, color, intensity, LIGHT_DIRECTIONAL);
	RET_IF_NULL(light, NULL);
	glm_vec3_copy(dir, light->specDirectional.dir);
	return _addLightToScene(scene, light);
}

bool Light_addAmbientLight(SceneLight *scene, vec3 pos, vec3 color,
                           float intensity) {
	Light *light = _createLight(pos, color, intensity, LIGHT_AMBIENT);
	RET_IF_NULL(light, NULL);
	return _addLightToScene(scene, light);
}

void Light_deleteLight(Light *light) { FREE(light); }
