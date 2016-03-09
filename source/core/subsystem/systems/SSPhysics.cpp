#include "SSPhysics.h"
#include <physics/PhysicsEngine.h>
#include "../../Input/Input.h"
#include "../../entity/EntityManager.h"
#include "../../datasystem/ComponentManager.h"
#include "../../components/PlacementComponent.h"
#include "../../components/RigidBodyComponent.h"
#include "../../components/CollisionComponent.h"
SSPhysics::SSPhysics() {

}

SSPhysics::~SSPhysics() {
	
}

void SSPhysics::Startup() {
	g_PhysicsEngine.Init();
}

void SSPhysics::Update(const float deltaTime) {
	//update all the collision objects that should not be affected by the physics world/gravity/etc
	int flag = PlacementComponent::Flag | RigidBodyComponent::Flag;
	for (auto& entity : g_EntityManager.GetEntityList()) {
		if ((entity.ComponentBitfield & flag) == flag) {
			PlacementComponent* pc = (PlacementComponent*)g_ComponentManager.GetComponent(entity, PlacementComponent::Flag);
			RigidBodyComponent* rbc = (RigidBodyComponent*)g_ComponentManager.GetComponent(entity, RigidBodyComponent::Flag);
			if (rbc->CollisionObject) {
				btTransform transform;
				transform.setFromOpenGLMatrix(&pc->World[0][0]);
				rbc->CollisionObject->setWorldTransform(transform);
			}
		}
	}
	g_PhysicsEngine.Update(deltaTime);
	//get translation from the bullet rigid body
	flag = PlacementComponent::Flag | RigidBodyComponent::Flag;
	int i = 0;
	for (auto& entity : g_EntityManager.GetEntityList()) {
		if ((entity.ComponentBitfield & flag) == flag) {
			PlacementComponent* pc = (PlacementComponent*)g_ComponentManager.GetComponent(entity, PlacementComponent::Flag);
			RigidBodyComponent* rbc = (RigidBodyComponent*)g_ComponentManager.GetComponent(entity, RigidBodyComponent::Flag);
			//update collision components
			if (rbc->CollisionObject && rbc->CollisionObject->isActive()) {
				btTransform transform = rbc->CollisionObject->getWorldTransform();
				transform.getOpenGLMatrix(&pc->World[0][0]);
				pc->Orientation = glm::quat_cast(pc->World);
				pc->Position = glm::vec3(pc->World[3]);
			}
			if (rbc->Body && rbc->Body->isActive()) {
				btTransform transform;
				rbc->Body->getMotionState()->getWorldTransform(transform);
				transform.getOpenGLMatrix(&pc->World[0][0]);
				pc->Orientation = glm::quat_cast(pc->World);
				pc->Position = glm::vec3(pc->World[3]);
			}
			i++;
		}
	}
}

void SSPhysics::Shutdown() {
	g_PhysicsEngine.Shutdown();
}
