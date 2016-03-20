#include "EntityFactory.h"
#include "EntityManager.h"
#include "core/datasystem/ComponentManager.h"
#include "core/components/PlacementComponent.h"
#include "core/components/ModelComponent.h"
#include "core/components/RigidBodyComponent.h"
#include "core/components/CameraComponent.h"
#include "core/components/LightComponent.h"
#include <physics/PhysicsEngine.h>
#include <gfx/ShapeGenerator.h>
#include <gfx/ModelBank.h>

void SpawnShape( BASIC_SHAPE shape,const glm::vec3& position, const glm::vec3& size, const float mass, const glm::vec4& color){
	Entity& entity = g_EntityManager.CreateEntity();
	entity.Tag = ENTITY_TYPE::SHAPE;
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
	rbc.PhysOBJ = g_PhysicsEngine.CreatePhysicsObject(shape ,mass, pc.Position, pc.Scale, entity.UID);
	g_ComponentManager.CreateComponent(&rbc, entity, RigidBodyComponent::Flag);
}

void SpawnPlayer(const glm::vec3& position,const glm::vec3& size){
	Entity& entity = g_EntityManager.CreateEntity();
	entity.Tag = ENTITY_TYPE::PLAYER;
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = size;
	g_ComponentManager.CreateComponent(&pc, entity, PlacementComponent::Flag);
	//set up rigid body
	RigidBodyComponent rbc;
	rbc.PhysOBJ = g_PhysicsEngine.CreatePhysicsObject(BASIC_SHAPE::CAPSULE, 75, pc.Position, pc.Scale, entity.UID);
	rbc.PhysOBJ->Body->setSleepingThresholds(0.0f, 0.0f);
	rbc.PhysOBJ->Body->setAngularFactor(0.0f);
	g_ComponentManager.CreateComponent(&rbc, entity, RigidBodyComponent::Flag);
	//set up camera
	CameraComponent cc;
	cc.Camera.SetPosition(pc.Position);
	g_ComponentManager.CreateComponent(&cc, entity, CameraComponent::Flag);
}

void SpawnBullet(const glm::vec3& position, const glm::vec3& orientation, const float force) {
	Entity& entity = g_EntityManager.CreateEntity();
	entity.Tag = ENTITY_TYPE::BULLET;
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = glm::vec3(0.5f);
	//calc rotation
	glm::vec3 rot = glm::normalize(orientation);
	glm::vec3 axis = glm::normalize(glm::cross(rot, glm::vec3(0, 0, -1)));
	float angle = glm::acos(glm::dot(rot, glm::vec3(0, 0, -1)));
	pc.Orientation = glm::rotate(pc.Orientation, angle, axis);
	g_ComponentManager.CreateComponent(&pc, entity, PlacementComponent::Flag);
	//set up rigid body
	RigidBodyComponent rbc;
	rbc.PhysOBJ = g_PhysicsEngine.CreatePhysicsObject(BASIC_SHAPE::SPHERE_SUBDIV, 10.0f, pc.Position, pc.Scale, entity.UID);
	glm::vec3 f = rot * force;
	rbc.PhysOBJ->Body->applyImpulse(btVector3(f.x, f.y, f.z), btVector3(0,0,0));
	g_ComponentManager.CreateComponent(&rbc, entity, RigidBodyComponent::Flag);
	//set up model component
	ModelComponent mc;
	mc.Color = glm::vec4(0.8f,0.7f,0.5f,1);
	mc.Model = gfx::g_ShapeGenerator.GenerateModel(BASIC_SHAPE::SPHERE_SUBDIV);
	g_ComponentManager.CreateComponent(&mc, entity, ModelComponent::Flag);
	//set up lighting
	LightComponent lc;
	lc.Light.Color = glm::vec4(1,1,1,1);
	lc.Light.Position = pc.Position;
	lc.Light.Range = 10.0f;
	lc.Light.Intensity = 1.0f;
	lc.Light.Attenuation = glm::vec3(1.0f, 0.1f, 0.1f);
	g_ComponentManager.CreateComponent(&lc, entity, LightComponent::Flag);
}

void SpawnLevelObject( const std::string& filename, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale ) {
	Entity& entity = g_EntityManager.CreateEntity();
	entity.Tag = ENTITY_TYPE::MODEL;
	//set up placement component
	PlacementComponent pc;
	pc.Position = position;
	pc.Scale = scale;
	pc.Orientation = orientation;
	g_ComponentManager.CreateComponent(&pc, entity, PlacementComponent::Flag);

	RigidBodyComponent rbc;
	rbc.PhysOBJ = g_PhysicsEngine.CreatePhysicsObjectModel(filename, 0, position, scale, orientation, entity.UID);
	g_ComponentManager.CreateComponent(&rbc, entity, RigidBodyComponent::Flag);

	ModelComponent mc;
	mc.Color = glm::vec4(1);
	mc.Model = gfx::g_ModelBank.LoadModel(filename.c_str());
	g_ComponentManager.CreateComponent(&mc, entity, ModelComponent::Flag);
}