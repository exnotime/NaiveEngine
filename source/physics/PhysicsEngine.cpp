#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine() {

}

PhysicsEngine::~PhysicsEngine() {

}

PhysicsEngine& PhysicsEngine::GetInstance() {
	static PhysicsEngine instance;
	return instance;
}

void PhysicsEngine::Init() {
	m_CollisionConfig = new btDefaultCollisionConfiguration();
	m_Dispatcher = new btCollisionDispatcher(m_CollisionConfig);
	m_PairCache = new btDbvtBroadphase();
	m_Solver = new btSequentialImpulseConstraintSolver;
	m_World = new btDiscreteDynamicsWorld(m_Dispatcher, m_PairCache, m_Solver, m_CollisionConfig);

	m_World->setGravity(btVector3(0, -9.2f, 0));
}

btRigidBody* PhysicsEngine::AddPhysicsObject(float mass, glm::vec3 pos, glm::vec3 size) {
	btBoxShape* object = new btBoxShape(btVector3(size.x,size.y,size.z));
	btDefaultMotionState* state = new btDefaultMotionState();
	btTransform transform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z));
	state->setWorldTransform(transform);
	btRigidBody* body = new btRigidBody(mass,state, object);
	m_World->addRigidBody(body);
	m_PhysicsObjects.push_back(object);
	return body;
}

void PhysicsEngine::Update(const float deltatime) {
	m_World->stepSimulation(deltatime, 10);
}

void PhysicsEngine::Shutdown() {
	for (int i = m_World->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject* obj = m_World->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		m_World->removeCollisionObject(obj);
		delete obj;
	}

	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		delete m_PhysicsObjects[i];
	}
	m_PhysicsObjects.clear();

	if (m_World) delete m_World;
	if (m_Solver) delete m_Solver;
	if (m_PairCache) delete m_PairCache;
	if (m_Dispatcher) delete m_Dispatcher;
	if (m_CollisionConfig) delete m_CollisionConfig;
}