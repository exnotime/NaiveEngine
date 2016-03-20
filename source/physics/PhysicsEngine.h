#pragma once
#include <Bullet/btBulletDynamicsCommon.h>
#include "PhysicsObject.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility/BasicShapes.h>
#include "PhysicsLibraryDefine.h"
#define g_PhysicsEngine PhysicsEngine::GetInstance()
class PhysicsEngine {
public:
	~PhysicsEngine();
	PHYSICS_API static PhysicsEngine& GetInstance();
	PHYSICS_API void Init();
	PHYSICS_API PhysicsObject* CreatePhysicsObject(BASIC_SHAPE shape, float mass, glm::vec3 pos, glm::vec3 size, unsigned int UID);
	PHYSICS_API PhysicsObject* CreatePhysicsObjectModel(const std::string& filename, float mass, glm::vec3 pos, glm::vec3 size, glm::quat orentation, unsigned int UID);
	PHYSICS_API void RemovePhysicsObjectFromEntity(unsigned int UID);
	PHYSICS_API void Update(const float deltatime);
	PHYSICS_API void Shutdown();
private:
	PhysicsEngine();

	std::vector<PhysicsObject*> m_PhysicsObjects;

	btDefaultCollisionConfiguration* m_CollisionConfig = nullptr;
	btCollisionDispatcher* m_Dispatcher = nullptr;
	btBroadphaseInterface* m_PairCache = nullptr;
	btConstraintSolver* m_Solver = nullptr;
	btDiscreteDynamicsWorld* m_World = nullptr;
};