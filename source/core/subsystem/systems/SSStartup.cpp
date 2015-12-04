#include "SSStartup.h"
#include "../../entity/EntityFactory.h"
SSStartup::SSStartup(){

}

SSStartup::~SSStartup(){

}

void SSStartup::Startup() {
	SpawnCube(glm::vec3(0, 0, 0), glm::vec3(1), 1, glm::vec4(0, 0, 1, 1));

	SpawnCube(glm::vec3(0, -50, -1), glm::vec3(20), 0, glm::vec4(1,0,0,1)); //no mass will make it static
}

void SSStartup::Update(const float deltaTime) {
}

void SSStartup::Shutdown() {

}

