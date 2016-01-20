#include "SSStartup.h"
#include "../../entity/EntityFactory.h"
SSStartup::SSStartup(){

}

SSStartup::~SSStartup(){

}

void SSStartup::Startup() {
	SpawnShape(BASIC_SHAPE::SPHERE_SUBDIV, glm::vec3(0, 0, -1), glm::vec3(1), 4, glm::vec4(0.3f,0.5f,0.8f,1));
	SpawnShape(BASIC_SHAPE::DODECAHEDRON, glm::vec3(0, 5, -1.3), glm::vec3(1), 8, glm::vec4(0.3f, 0.5f, 0.8f, 1));
	SpawnShape(BASIC_SHAPE::DONUT, glm::vec3(0, 0, -1.4f), glm::vec3(1), 12, glm::vec4(0.3f, 0.5f, 0.8f, 1));
	SpawnShape(BASIC_SHAPE::OCTOHEDRON, glm::vec3(0, 0, -4.0f), glm::vec3(1), 16, glm::vec4(0.9f, 0.5f, 0.8f, 1));
	SpawnShape(BASIC_SHAPE::CUBE, glm::vec3(0, -50, -1), glm::vec3(50, 10, 50), 0, glm::vec4(0.5f,0.5f,0.5f,1.0f)); //no mass will make it static
}

void SSStartup::Update(const float deltaTime) {

}

void SSStartup::Shutdown() {

}

