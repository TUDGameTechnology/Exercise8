#pragma once

#include "pch.h"

// A plane is defined as the plane's normal and the distance of the plane to the origin
class PlaneCollider {
public:
	float d;
	Kore::vec3 normal;
};

// A sphere is defined by a radius and a center.
class SphereCollider {
public:
	Kore::vec3 center;
	float radius;

	/************************************************************************/
	/* Exercise P8.4														*/
	/************************************************************************/
	/* Implement the collision functions below */

	// Return true iff there is an intersection with the other sphere
	bool IntersectsWith(const SphereCollider& other) {
		return false;
	}

	// Collision normal is the normal vector pointing towards the other sphere
	Kore::vec3 GetCollisionNormal(const SphereCollider& other) {
		return Kore::vec3();
	}

	// The penetration depth
	float PenetrationDepth(const SphereCollider& other) {
		return 0.0f;
	}


	bool IntersectsWith(const PlaneCollider& other) {
		return other.normal.dot(center) - other.d <= radius;
	}

	Kore::vec3 GetCollisionNormal(const PlaneCollider& other) {
		return other.normal;
	}

	float PenetrationDepth(const PlaneCollider &other) {
		return other.normal.dot(center) - other.d - radius;
	}
};
