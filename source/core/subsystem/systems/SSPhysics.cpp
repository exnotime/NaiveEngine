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
		if ((entity.ComponentBitfield & flag) == flag) {
			PlacementComponent* pc = (PlacementComponent*)g_ComponentManager.GetComponent(entity, PlacementComponent::Flag);
			RigidBodyComponent* rbc = (RigidBodyComponent*)g_ComponentManager.GetComponent(entity, RigidBodyComponent::Flag);
			if (rbc->PhysOBJ->Body) {
				if(rbc->PhysOBJ->Body->isActive()){
					btTransform transform;
					rbc->PhysOBJ->Body->getMotionState()->getWorldTransform(transform);
					transform.getOpenGLMatrix(&pc->World[0][0]);
					pc->Orientation = glm::quat_cast(pc->World);
					pc->Position = glm::vec3(pc->World[3]);
				}
			}
		}
	}
}

void SSPhysics::Shutdown() {
	g_PhysicsEngine.Shutdown();
}
