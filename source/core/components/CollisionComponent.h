#pragma once
#include <Bullet/btBulletCollisionCommon.h>
struct CollisionComponent {
	btCollisionObject* CollisionObject;
	static unsigned int Flag;
};