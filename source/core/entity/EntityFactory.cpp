#include "EntityFactory.h"
#include "EntityManager.h"
#include "core/datasystem/ComponentManager.h"
#include "core/components/PlacementComponent.h"
#include "core/components/ModelComponent.h"
#include "core/components/RigidBodyComponent.h"
#include <gfx/ModelBank.h>
#include <physics/PhysicsEngine.h>

void SpawnCube(glm::vec3 position, glm::vec3 size, float mass, glm::vec4 color) {
	//set up 
	Entity& entity = g_EntityManager.CreateEntity();
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = size;
	g_ComponentManager.CreateComponent(&pc, entity, PlacementComponent::Flag);
	//set up model component
	ModelComponent mc;
	mc.Color = color;
	mc.Model = gfx::g_ModelBank.LoadModel("asset/model/cube.obj");
	g_ComponentManager.CreateComponent(&mc, entity, ModelComponent::Flag);
	//set up rigid body
	RigidBodyComponent rbc;
	rbc.Body = g_PhysicsEngine.AddPhysicsObject(mass, pc.Position, pc.Scale);
	g_ComponentManager.CreateComponent(&rbc, entity, RigidBodyComponent::Flag);
}