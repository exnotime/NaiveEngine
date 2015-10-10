#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
struct PlacementComponent {
	PlacementComponent() : World(1), Position(0), Orientation(1.0f, 0 , 0 ,0), Scale(1)
	{
	}
	glm::mat4 World;
	glm::vec3 Position;
	glm::quat Orientation;
	glm::vec3 Scale;
};