#include "SSStartup.h"
#include "../../entity/EntityFactory.h"
#include "../../components/RigidBodyComponent.h"
#include "../../entity/EntityManager.h"
#include "../../datasystem/ComponentManager.h"
SSStartup::SSStartup(){

}

SSStartup::~SSStartup(){

}

void SSStartup::Startup() {
	//SpawnShape(BASIC_SHAPE::SPHERE_SUBDIV, glm::vec3(0, 0, -1), glm::vec3(1), 100, glm::vec4(0.6f, 0.3f, 0.2f, 1));
	//SpawnShape(BASIC_SHAPE::SPHERE_SUBDIV, glm::vec3(0, 0, 0), glm::vec3(1), 10, glm::vec4(0.6f, 0.3f, 0.2f, 1));
	//SpawnShape(BASIC_SHAPE::SPHERE_SUBDIV, glm::vec3(2, 1, 0), glm::vec3(2), 0, glm::vec4(0.6f, 0.3f, 0.2f, 1));

	//SpawnShape(BASIC_SHAPE::DODECAHEDRON, glm::vec3(0, 5, -1.3), glm::vec3(1), 8, glm::vec4(0.7f, 0.7f, 0.2f, 1));
	//SpawnShape(BASIC_SHAPE::DONUT, glm::vec3(0, 0, -1.4f), glm::vec3(1), 12, glm::vec4(1.0f,0.8f,0.0f, 1));
	//SpawnShape(BASIC_SHAPE::OCTOHEDRON, glm::vec3(0, 0, -4.0f), glm::vec3(1), 16, glm::vec4(0.2f, 0.4f,0.1f,1));
	//SpawnShape(BASIC_SHAPE::CUBE, glm::vec3(0, -3, 0), glm::vec3(3, 1, 3), 0, glm::vec4(0.5f,0.5f,0.5f,1.0f)); //no mass will make it static
	
	//SpawnShape(BASIC_SHAPE::CUBE, glm::vec3(3, 0, 0), glm::vec3(1, 3, 3), 0, glm::vec4(0.5f,0.5f,0.5f,1.0f));
	//SpawnShape(BASIC_SHAPE::CUBE, glm::vec3(0, 0, 3), glm::vec3(3, 3, 1), 0, glm::vec4(0.5f,0.5f,0.5f,1.0f));
	//SpawnShape(BASIC_SHAPE::CUBE, glm::vec3(-3, 0, 0), glm::vec3(1, 3, 3), 0, glm::vec4(0.5f,0.5f,0.5f,1.0f));
	//SpawnShape(BASIC_SHAPE::CUBE, glm::vec3(0, 0, -3), glm::vec3(3, 3, 1), 0, glm::vec4(0.5f,0.5f,0.5f,1.0f));
	SpawnLevelObject("asset/model/test/TestLevel.dae", glm::vec3(0), glm::quat(1,0,0,0), glm::vec3(10));
	SpawnPlayer(glm::vec3(0, 10, 0), glm::vec3(0.6f, 1.8f, 0.3f));
}

void SSStartup::Update(const float deltaTime) {

}

void SSStartup::Shutdown() {
}

