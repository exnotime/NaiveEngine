#include "PhysicsEngine.h"
#define PAR_SHAPES_IMPLEMENTATION
//for creating and loading shapes
#include <par_shapes.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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
	m_Solver = new btSequentialImpulseConstraintSolver();
	m_World = new btDiscreteDynamicsWorld(m_Dispatcher, m_PairCache, m_Solver, m_CollisionConfig);

	m_World->setGravity(btVector3(0, -9.2f, 0));
}

PhysicsObject* PhysicsEngine::CreatePhysicsObject(BASIC_SHAPE shape, float mass, glm::vec3 pos, glm::vec3 size, unsigned int UID) {
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
		case BASIC_SHAPE::CAPSULE:
			physicsShape = new btCapsuleShape(size.x,size.y); //size.x is radius and size.y is length/height of capsule
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
	PhysicsObject* physObj = new PhysicsObject();
	btDefaultMotionState* state = new btDefaultMotionState();
	btTransform transform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z));
	state->setWorldTransform(transform);
	btVector3 inertia;
	physicsShape->calculateLocalInertia(mass, inertia);
	btRigidBody* body = new btRigidBody(mass,state, physicsShape, inertia);
	body->setRestitution(0.0f);
	m_World->addRigidBody(body);
	physObj->Body = body;
	physObj->EntityUID = UID;
	m_PhysicsObjects.push_back(physObj);
	return physObj;
}

PhysicsObject* PhysicsEngine::CreatePhysicsObjectModel( const std::string& filename, float mass, glm::vec3 pos, glm::vec3 size, glm::quat orentation, unsigned int UID ) {

	Assimp::Importer loader;
	const aiScene* scene = loader.ReadFile( filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace );
	if( !scene ) {
		printf("Error: Could not load physics model %s\n", filename.c_str());
		return nullptr;
	}
	btTriangleMesh* btMesh = new btTriangleMesh(true, false);
	for( int i = 0; i < scene->mNumMeshes; ++i ) {
		const aiMesh* mesh = scene->mMeshes[i];
		btVector3 vert;
		//foreach vertice
		for( int j = 0; j < mesh->mNumVertices; ++j ) {
			vert = btVector3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			btMesh->findOrAddVertex(vert,false); //no need to remove identical vertices since assimp has already done so.
		}
		//foreach triangle
		for( int j = 0; j < mesh->mNumFaces; ++j ) {
			btMesh->addTriangleIndices(mesh->mFaces[j].mIndices[0],mesh->mFaces[j].mIndices[1],mesh->mFaces[j].mIndices[2]);
		}
	}

	PhysicsObject* physObj = new PhysicsObject();
	btBvhTriangleMeshShape* physicsShape = new btBvhTriangleMeshShape(btMesh, true);
	btScaledBvhTriangleMeshShape* scaledShape = new btScaledBvhTriangleMeshShape(physicsShape, btVector3(size.x,size.y,size.z));
	scaledShape->setMargin(0.01f);
	btDefaultMotionState* state = new btDefaultMotionState();
	btTransform transform(btQuaternion(orentation.x,orentation.y,orentation.z,orentation.w), btVector3(pos.x, pos.y, pos.z));
	state->setWorldTransform(transform);
	
	btRigidBody* body = new btRigidBody(mass, state, scaledShape);
	body->setRestitution(0.0f);
	body->setFriction(1.0f);

	m_World->addRigidBody(body);
	physObj->Body = body;
	physObj->EntityUID = UID;
	m_PhysicsObjects.push_back(physObj);
	return physObj;
}

void PhysicsEngine::RemovePhysicsObjectFromEntity(unsigned int UID) {
	PhysicsObject* physObj = nullptr;
	int i;
	for (i = 0; i < m_PhysicsObjects.size(); i++) {
		if (m_PhysicsObjects[i]->EntityUID == UID) {
			physObj = m_PhysicsObjects[i];
			break;
		}
	}
	if (!physObj)
		return;
	if (physObj->Body && physObj->Body->getMotionState() && physObj->Body->getCollisionShape()) {
		delete physObj->Body->getMotionState();
		delete physObj->Body->getCollisionShape();
	}
	m_World->removeRigidBody(physObj->Body);
	delete physObj->Body;
	physObj->Body = nullptr;
	physObj->EntityUID = 0;
	delete physObj;
	m_PhysicsObjects.erase(m_PhysicsObjects.begin() + i);
}

void PhysicsEngine::Update(const float deltatime) {
	m_World->stepSimulation(deltatime, 10);
}

void PhysicsEngine::Shutdown() {
	for (int i = m_World->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject* obj = m_World->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState() && body->getCollisionShape()) {
			delete body->getMotionState();
			delete body->getCollisionShape();
		}
		m_World->removeCollisionObject(obj);
		delete body;
	}
	for (auto& obj : m_PhysicsObjects) {
		if(obj) {
			delete obj;
		}
	}
	m_PhysicsObjects.clear();

	if (m_World) delete m_World;
	if (m_Solver) delete m_Solver;
	if (m_PairCache) delete m_PairCache;
	if (m_Dispatcher) delete m_Dispatcher;
	if (m_CollisionConfig) delete m_CollisionConfig;
}