#pragma once

#include "pch.h"

#include <Kore/Graphics1/Image.h>
#include <Kore/Graphics5/Graphics.h>

#include "Memory.h"
#include "Collision.h"
#include "MeshObject.h"

// A physically simulated object
class PhysicsObject {
vec3 Position;
public:
	float Mass;
	vec3 Velocity;
	int id;

	static int currentID;

	void SetPosition(vec3 pos) {
		Position = pos;
		Collider.center = pos;
	}

	vec3 GetPosition() {
		return Position;
	}
	
	// Force accumulator
	vec3 Accumulator;
	
	SphereCollider Collider;
	
	MeshObject* Mesh;

	PhysicsObject();

	// Do the integration step for the equations of motion
	void Integrate(float deltaT);

	// Apply a force that acts along the center of mass
	void ApplyForceToCenter(vec3 force);
	
	// Apply an impulse
	void ApplyImpulse(vec3 impulse);

	// Handle the collision with the plane (includes testing for intersection)
	void HandleCollision(const PlaneCollider& collider, float deltaT);

	// Handle the collision with another sphere (includes testing for intersection)
	void HandleCollision(PhysicsObject* other, float deltaT);

	// Update the matrix of the mesh
	void UpdateMatrix();

};
