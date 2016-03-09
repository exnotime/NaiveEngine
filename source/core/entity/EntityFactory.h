#pragma once
#include <glm/glm.hpp>
#include <utility/BasicShapes.h>
void SpawnShape( BASIC_SHAPE shape, glm::vec3 position, glm::vec3 size, float mass, glm::vec4 color);
void SpawnPlayer(glm::vec3 position, glm::vec3 size);
void SpawnBullet(glm::vec3 position, glm::vec3 orientation, float force);