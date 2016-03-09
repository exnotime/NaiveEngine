#include "EntityFactory.h"
#include "EntityManager.h"
#include "core/datasystem/ComponentManager.h"
#include "core/components/PlacementComponent.h"
#include "core/components/ModelComponent.h"
#include "core/components/RigidBodyComponent.h"
#include "core/components/CameraComponent.h"
#include <physics/PhysicsEngine.h>
#include <gfx/ShapeGenerator.h>

void SpawnShape(BASIC_SHAPE shape, glm::vec3 position, glm::vec3 size, float mass, glm::vec4 color) {
	Entity& entity = g_EntityManager.CreateEntity();
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = size;
	g_ComponentManager.CreateComponent(&pc, entity, PlacementComponent::Flag);
	//set up model component
	ModelComponent mc;
	mc.Color = color;
	mc.Model = gfx::g_ShapeGenerator.GenerateModel(shape);
	g_ComponentManager.CreateComponent(&mc, entity, ModelComponent::Flag);
	//set up rigid body
	RigidBodyComponent rbc;
	rbc.Body = g_PhysicsEngine.AddPhysicsShape(shape ,mass, pc.Position, pc.Scale);
	g_ComponentManager.CreateComponent(&rbc, entity, RigidBodyComponent::Flag);
}

void SpawnPlayer(glm::vec3 position, glm::vec3 size) {
	Entity& entity = g_EntityManager.CreateEntity();
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = size;
	g_ComponentManager.CreateComponent(&pc, entity, PlacementComponent::Flag);
	//set up rigid body
	RigidBodyComponent rbc;
	rbc.Body = g_PhysicsEngine.AddPhysicsShape(BASIC_SHAPE::CUBE, 10, pc.Position, pc.Scale); //0 in mass will make it and immovable object
	rbc.Body->setSleepingThresholds(0.0f, 0.0f);
	rbc.Body->setAngularFactor(0.0f);
	g_ComponentManager.CreateComponent(&rbc, entity, RigidBodyComponent::Flag);
	//set up camera
	CameraComponent cc;
	cc.Camera.SetPosition(pc.Position);
	g_ComponentManager.CreateComponent(&cc, entity, CameraComponent::Flag);
}

void SpawnBullet(glm::vec3 position, glm::vec3 orientation, float force) {
	Entity& entity = g_EntityManager.CreateEntity();
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = glm::vec3(0.1f,0.1f,0.2f);
	//calc rotation
	glm::vec3 axis = glm::cross(orientation, glm::vec3(0, 0, -1));
	float angle = glm::acos(glm::dot(orientation, glm::vec3(0, 0, -1)));
	pc.Orientation = glm::quat(angle, axis);
	g_ComponentManager.CreateComponent(&pc, entity, PlacementComponent::Flag);
	//set up rigid body
	RigidBodyComponent rbc;
	rbc.Body = g_PhysicsEngine.AddPhysicsShape(BASIC_SHAPE::CUBE, 0.001f, pc.Position, pc.Scale);
	glm::vec3 f = orientation * force;
	rbc.Body->applyForce(btVector3(f.x, f.y, f.z), btVector3(0,0,0));
	g_ComponentManager.CreateComponent(&rbc, entity, RigidBodyComponent::Flag);
	//set up model component
	ModelComponent mc;
	mc.Color = glm::vec4(1,0,0,1);
	mc.Model = gfx::g_ShapeGenerator.GenerateModel(BASIC_SHAPE::CUBE);
	g_ComponentManager.CreateComponent(&mc, entity, ModelComponent::Flag);
}