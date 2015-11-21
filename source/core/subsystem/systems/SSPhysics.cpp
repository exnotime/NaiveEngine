#include "SSPhysics.h"
#include <physics/PhysicsEngine.h>
#include "../../Input/Input.h"
#include "../../entity/EntityManager.h"
#include "../../datasystem/ComponentManager.h"
#include "../../components/PlacementComponent.h"
#include "../../components/RigidBodyComponent.h"
SSPhysics::SSPhysics() {

}

SSPhysics::~SSPhysics() {

}

void SSPhysics::Startup() {
	g_PhysicsEngine.Init();
}

void SSPhysics::Update(const float deltaTime) {
	g_PhysicsEngine.Update(deltaTime);
	//get translation from the bullet rigid body

	int flag = PlacementComponent::Flag | RigidBodyComponent::Flag;

	for (auto& entity : g_EntityManager.GetEntityList()) {
		if (entity.ComponentBitfield & flag) {
			PlacementComponent* pc = g_ComponentManager.GetComponent<PlacementComponent>(entity);
			RigidBodyComponent* rbc = g_ComponentManager.GetComponent<RigidBodyComponent>(entity);

			if (rbc->Body->isActive()) {
				btTransform transform;
				rbc->Body->getMotionState()->getWorldTransform(transform);
				transform.getOpenGLMatrix(&pc->World[0][0]);
				pc->Orientation = glm::quat_cast(pc->World);
				pc->Position = glm::vec3(pc->World[3]);
			}
		}
	}
}

void SSPhysics::Shutdown() {
}
