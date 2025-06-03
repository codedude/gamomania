#include "light.hpp"
#include <SDL3/SDL_log.h>
#include <cstdio>
#include <glm/vec3.hpp>
#include <memory>

namespace dve {

bool SceneLight::addPointLight(glm::vec3 pos, glm::vec3 color, float intensity,
                               float linear, float quadratic) noexcept {
	auto light =
	    std::make_unique<Light>(pos, color, intensity, true, LightType::POINT);
	light->specPoint.constant = 1.0f;
	light->specPoint.linear = linear;
	light->specPoint.quadratic = quadratic;
	lights.push_back(std::move(light));
	return true;
}

bool SceneLight::addSpotLight(glm::vec3 pos, glm::vec3 color, float intensity,
                              glm::vec3 dir, float innerCutOff,
                              float outerCutOff) noexcept {
	auto light =
	    std::make_unique<Light>(pos, color, intensity, true, LightType::SPOT);
	light->specSpot.innerCutOff = innerCutOff;
	light->specSpot.outerCutOff = outerCutOff;
	lights.push_back(std::move(light));
	return true;
}

bool SceneLight::addDirectionalLight(glm::vec3 pos, glm::vec3 color,
                                     float intensity, glm::vec3 dir) noexcept {
	auto light = std::make_unique<Light>(pos, color, intensity, true,
	                                     LightType::DIRECTIONAL);
	light->specDirectional.dir = dir;
	lights.push_back(std::move(light));
	return true;
}

bool SceneLight::addAmbientLight(glm::vec3 pos, glm::vec3 color,
                                 float intensity) noexcept {
	auto light = std::make_unique<Light>(pos, color, intensity, true,
	                                     LightType::AMBIENT);
	lights.push_back(std::move(light));
	return true;
}

bool SceneLight::load(ShaderProgram *shader) noexcept {
	int nPointLight = 0;
	int nSpotLight = 0;
	int nDirLight = 0;
	int nAmbientLight = 0;

#define BUFF_SIZE 128
	char buff[BUFF_SIZE] = {};

	for (const auto &light : lights) {
		if (light->type == LightType::DIRECTIONAL) {
			std::snprintf(buff, BUFF_SIZE, "directionalLights[%d].pos",
			              nDirLight);
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
		} else if (light->type == LightType::POINT) {
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
		} else if (light->type == LightType::AMBIENT) {
			snprintf(buff, BUFF_SIZE, "ambientLights[%d].color", nAmbientLight);
			Shader_setVec3(Shader_uniformGet(shader, buff), light->color);
			snprintf(buff, BUFF_SIZE, "ambientLights[%d].intensity",
			         nAmbientLight);
			Shader_setFloat(Shader_uniformGet(shader, buff), light->intensity);
			++nAmbientLight;
		} else if (light->type == LightType::SPOT) {
			++nSpotLight;
		}
	}
	Shader_setInt(Shader_uniformGet(shader, "pointLightsLen"), nPointLight);
	Shader_setInt(Shader_uniformGet(shader, "spotLightsLen"), nSpotLight);
	Shader_setInt(Shader_uniformGet(shader, "directionalLightsLen"), nDirLight);
	Shader_setInt(Shader_uniformGet(shader, "ambientLightsLen"), nAmbientLight);

	return true;
}

} // namespace dve
