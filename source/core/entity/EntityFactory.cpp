#include "EntityFactory.h"
#include "EntityManager.h"
#include "core/datasystem/ComponentManager.h"
#include "core/components/PlacementComponent.h"
#include "core/components/ModelComponent.h"
#include "core/components/RigidBodyComponent.h"
#include <gfx/ModelBank.h>
#include <physics/PhysicsEngine.h>
#include <glm/gtx/transform.hpp>

void SpawnCube(glm::vec3 position, glm::vec3 size, float mass, glm::vec4 color) {
	//set up 
	Entity& entity = g_EntityManager.CreateEntity();
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = size;	
	g_ComponentManager.CreateComponent<PlacementComponent>(pc, entity, GET_TYPE_ID(PlacementComponent));
	//set up model component
	ModelComponent mc;
	mc.Color = color;
	mc.Model = gfx::g_ModelBank.LoadModel("asset/model/cube.obj");
	g_ComponentManager.CreateComponent<ModelComponent>(mc, entity, GET_TYPE_ID(ModelComponent));
	//set up rigid body
	RigidBodyComponent rbc;
	rbc.Body = g_PhysicsEngine.AddPhysicsObject(mass, pc.Position, pc.Scale);
	g_ComponentManager.CreateComponent<RigidBodyComponent>(rbc, entity, GET_TYPE_ID(RigidBodyComponent));
}

void SpawnGround() {
	Entity& entity = g_EntityManager.CreateEntity();
	//set up placement component
	PlacementComponent pc;
	pc.Position = glm::vec3(0);
	pc.Scale = glm::vec3(100.0f);
	pc.World = glm::translate(pc.Position) * glm::scale(pc.Scale);
	g_ComponentManager.CreateComponent<PlacementComponent>(pc, entity, GET_TYPE_ID(PlacementComponent));
	//set up model component
	ModelComponent mc;
	mc.Color = glm::vec4(1);
	mc.Model = gfx::g_ModelBank.LoadModel("asset/model/Ground/Ground.dae");
	g_ComponentManager.CreateComponent<ModelComponent>(mc, entity, GET_TYPE_ID(ModelComponent));
}

void SpawnTree(glm::vec3 position) {
	Entity& entity = g_EntityManager.CreateEntity();
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = glm::vec3(1.0f);
	pc.World = glm::translate(pc.Position) * glm::scale(pc.Scale);
	g_ComponentManager.CreateComponent<PlacementComponent>(pc, entity, GET_TYPE_ID(PlacementComponent));
	//set up model component
	ModelComponent mc;
	mc.Color = glm::vec4(1);
	mc.Model = gfx::g_ModelBank.LoadModel("asset/model/Tree/Tree.dae");
	g_ComponentManager.CreateComponent<ModelComponent>(mc, entity, GET_TYPE_ID(ModelComponent));
}