#pragma once
#include <glm/glm.hpp>
#include <physics/PhysicsObject.h>
struct CollisionComponent {
	bool Hit;
	glm::vec3 CollisionPoint;
	unsigned int EntityUID;
	PhysicsObject* PhysObject;
	static unsigned int Flag;
};