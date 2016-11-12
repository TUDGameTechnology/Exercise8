#pragma once

#include "pch.h"

using namespace Kore;


// A plane is defined as the plane's normal and the distance of the plane to the origin
class PlaneCollider {
public:
	float d;
	vec3 normal;


};

// A sphere is defined by a radius and a center.
class SphereCollider {
public:
	vec3 center;
	float radius;

	/************************************************************************/
	/* Exercise P8.4*                                                       */
	/************************************************************************/
	/* Implement the collision functions below */

	// Return true iff there is an intersection with the other sphere
	bool IntersectsWith(const SphereCollider& other) {
		return false;
	}

	// Collision normal is the normal vector pointing towards the other sphere
	vec3 GetCollisionNormal(const SphereCollider& other) {
		return vec3();
	}

	// The penetration depth
	float PenetrationDepth(const SphereCollider& other) {
		return 0.0f;
	}


	bool IntersectsWith(const PlaneCollider& other) {
		return other.normal.dot(center) - other.d <= radius;
	}

	vec3 GetCollisionNormal(const PlaneCollider& other) {
		return other.normal;
	}

	float PenetrationDepth(const PlaneCollider &other) {
		return other.normal.dot(center) - other.d - radius;
	}


};
