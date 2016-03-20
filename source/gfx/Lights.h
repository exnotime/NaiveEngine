#pragma once
#include <glm/glm.hpp>
namespace gfx {

struct Light {
	glm::vec4 Color;
	glm::vec3 Position;
	float Intensity;
	glm::vec3 Direction;
	float Length;
	glm::vec3 Attenuation;
	float Range;
};
}
