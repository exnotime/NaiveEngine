#pragma once
#include <glm/glm.hpp>

namespace gfx_dx {
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec3 Tangent;
	};
}