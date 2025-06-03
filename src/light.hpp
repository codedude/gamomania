#ifndef __LIGHT_H
#define __LIGHT_H

#include "shader.hpp"
#include <glm/vec3.hpp>
#include <initializer_list>
#include <memory>

namespace dve {

inline constexpr int MAX_LIGHT = 16;

typedef struct {
	// attenuation
	float constant;
	float linear;
	float quadratic;
} PointLight;

typedef struct {
	glm::vec3 dir;
	// spot size
	float innerCutOff;
	float outerCutOff;
	// attenuation
	float constant;
	float linear;
	float quadratic;
} SpotLight;

typedef struct {
	glm::vec3 dir;
} DirectionalLight;

typedef struct {
	// no data yet
} AmbientLight;

enum class LightType { DIRECTIONAL, SPOT, POINT, AMBIENT };

typedef struct {
	glm::vec3 pos;
	glm::vec3 color; // color emitted
	float intensity;
	bool shadow;
	LightType type; // type of light
	union {
		DirectionalLight specDirectional;
		SpotLight specSpot;
		PointLight specPoint;
		AmbientLight specAmbient;
	};
} Light;

class SceneLight {
  private:
	std::vector<std::unique_ptr<Light>> lights;

  public:
	// constructor & destructor
	SceneLight() = default;
	~SceneLight() = default;
	// no copy
	SceneLight(const SceneLight &s) = delete;
	SceneLight &operator=(const SceneLight &s) = delete;
	// no move
	SceneLight(SceneLight &&s) = delete;
	SceneLight &operator=(SceneLight &&s) = delete;

	template <typename T, typename... Args>
	bool addLight(Args &&...pArgs) noexcept {
		auto p = std::make_unique<T>(std::forward<Args>(pArgs)...);
		lights.push_back(std::move(p));
		// emplace
	}

	bool addPointLight(glm::vec3 pos, glm::vec3 color, float intensity,
	                   float linear, float quadratic) noexcept;
	bool addSpotLight(glm::vec3 pos, glm::vec3 color, float intensity,
	                  glm::vec3 dir, float innerCutOff,
	                  float outerCutOff) noexcept;
	bool addDirectionalLight(glm::vec3 pos, glm::vec3 color, float intensity,
	                         glm::vec3 dir) noexcept;
	bool addAmbientLight(glm::vec3 pos, glm::vec3 color,
	                     float intensity) noexcept;
	bool load(ShaderProgram *shader) noexcept;
};
} // namespace dve

#endif
