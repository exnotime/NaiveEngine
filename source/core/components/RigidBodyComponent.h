#pragma once
#include <Bullet/btBulletDynamicsCommon.h>
#include "physics/PhysicsEngine.h"
struct RigidBodyComponent {
	PhysicsObject* PhysOBJ;
	static unsigned int Flag;
};

