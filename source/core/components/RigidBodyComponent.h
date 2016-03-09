#pragma once
#include <Bullet/btBulletDynamicsCommon.h>
struct RigidBodyComponent {
	btRigidBody* Body = nullptr;
	btCollisionObject* CollisionObject = nullptr;
	static unsigned int Flag;
};

