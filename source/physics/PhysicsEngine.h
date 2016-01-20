#pragma once
#include <Bullet/btBulletDynamicsCommon.h>
#include "PhysicsObject.h"
#include <vector>
#include <glm/glm.hpp>
#include <utility/BasicShapes.h>
#include "PhysicsLibraryDefine.h"

#define g_PhysicsEngine PhysicsEngine::GetInstance()
class PhysicsEngine {
public:
	~PhysicsEngine();
	PHYSICS_API static PhysicsEngine& GetInstance();
	PHYSICS_API void Init();
	PHYSICS_API btRigidBody* AddPhysicsObject(BASIC_SHAPE shape, float mass, glm::vec3 pos, glm::vec3 size);
	PHYSICS_API void Update(const float deltatime);
	PHYSICS_API void Shutdown();
private:
	PhysicsEngine();

	std::vector<btCollisionShape*> m_PhysicsObjects;

	btDefaultCollisionConfiguration* m_CollisionConfig = nullptr;
	btCollisionDispatcher* m_Dispatcher = nullptr;
	btBroadphaseInterface* m_PairCache = nullptr;
	btSequentialImpulseConstraintSolver* m_Solver = nullptr;
	btDiscreteDynamicsWorld* m_World = nullptr;
};