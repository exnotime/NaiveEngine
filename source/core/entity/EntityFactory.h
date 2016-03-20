#pragma once
#include <glm/glm.hpp>
#include <utility/BasicShapes.h>
#include <string>
#include <glm/gtc/quaternion.hpp>
enum ENTITY_TYPE : unsigned int {
	PLAYER,
	MODEL,
	SHAPE,
	BULLET
};

void SpawnShape( BASIC_SHAPE shape,const glm::vec3& position, const glm::vec3& size, const float mass, const glm::vec4& color);
void SpawnPlayer(const glm::vec3& position,const glm::vec3& size);
void SpawnBullet(const glm::vec3& position, const glm::vec3& orientation, const float force);
void SpawnLevelObject(const std::string& filename, const glm::vec3& position, const glm::quat& orientation , const glm::vec3& scale );