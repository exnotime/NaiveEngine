#include "PhysicsEngine.h"
#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes.h>
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

btRigidBody* PhysicsEngine::AddPhysicsObject(BASIC_SHAPE shape, float mass, glm::vec3 pos, glm::vec3 size) {
	btCollisionShape* physicsShape;
	par_shapes_mesh_s* mesh;
	switch (shape) {
	case BASIC_SHAPE::CUBE:
		physicsShape = new btBoxShape(btVector3(size.x, size.y, size.z));
		break;
	case BASIC_SHAPE::SPHERE_PARA:
		physicsShape = new btSphereShape(glm::max(glm::max(size.x,size.y),size.z));
		break;
	case BASIC_SHAPE::SPHERE_SUBDIV:
		physicsShape = new btSphereShape(glm::max(glm::max(size.x, size.y), size.z));
		break;
	case BASIC_SHAPE::OCTOHEDRON:
		mesh = par_shapes_create_octohedron();
		physicsShape = new btConvexHullShape(mesh->points, mesh->npoints, sizeof(float) * 3);
		par_shapes_free_mesh(mesh);
		break;
	case BASIC_SHAPE::TETRAHEDRON:
		mesh = par_shapes_create_tetrahedron();
		physicsShape = new btConvexHullShape(mesh->points, mesh->npoints, sizeof(float) * 3);
		par_shapes_free_mesh(mesh);
		break;
	case BASIC_SHAPE::DODECAHEDRON:
		mesh = par_shapes_create_dodecahedron();
		physicsShape = new btConvexHullShape(mesh->points, mesh->npoints, sizeof(float) * 3);
		par_shapes_free_mesh(mesh);
		break;
	case BASIC_SHAPE::ICOSAHEDRON:
		mesh = par_shapes_create_icosahedron();
		physicsShape = new btConvexHullShape(mesh->points, mesh->npoints, sizeof(float) * 3);
		par_shapes_free_mesh(mesh);
		break;
	case BASIC_SHAPE::DONUT:
		mesh = par_shapes_create_torus(8, 8, 0.5f);
		physicsShape = new btConvexHullShape(mesh->points, mesh->npoints, sizeof(float) * 3);
		par_shapes_free_mesh(mesh);
		break;
	default:
		printf("unrecognized shape in physicsengine\n");
		break;
	};

	btDefaultMotionState* state = new btDefaultMotionState();
	btTransform transform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z));
	state->setWorldTransform(transform);
	btVector3 inertia;
	physicsShape->calculateLocalInertia(mass, inertia);
	btRigidBody* body = new btRigidBody(mass,state, physicsShape, inertia);
	body->setRestitution(0.0f);
	m_World->addRigidBody(body);
	m_PhysicsObjects.push_back(physicsShape);
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