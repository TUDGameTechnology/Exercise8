#include "pch.h"

#include "PhysicsWorld.h"
#include "PhysicsObject.h"

using namespace Kore;

PhysicsWorld::PhysicsWorld() {
	physicsObjects = new PhysicsObject*[100];
		for (int i = 0; i < 100; i++) {
			physicsObjects[i] = nullptr;
		}

		plane.normal = vec3(0, 1, 0);
		plane.d = 1;
}

void PhysicsWorld::Update(float deltaT) {
	PhysicsObject** currentP = &physicsObjects[0];
		while (*currentP != nullptr) {

		// Apply gravity (= constant accceleration, so we multiply with the mass and divide in the integration step.
		// The alternative would be to add gravity during the integration as a constant.

		(*currentP)->ApplyForceToCenter(vec3(0, (*currentP)->Mass * -9.81f, 0.0f));
		
		// Integrate the equations of motion
		(*currentP)->Integrate(deltaT);
		++currentP;
	}

		currentP = &physicsObjects[0];
		while (*currentP != nullptr) {
			// Check for collisions with the plane
			(*currentP)->HandleCollision(plane, deltaT);

			++currentP;
		}

		currentP = &physicsObjects[0];
		while (*currentP != nullptr) {
			
			// Check for collisions with the other objects
			PhysicsObject** currentCollision = currentP + 1;
			while (*currentCollision != nullptr) {

				(*currentP)->HandleCollision(*currentCollision, deltaT);
				++currentCollision;
			}
			++currentP;
		}
}


void PhysicsWorld::AddObject(PhysicsObject* po) {
	PhysicsObject** current = &physicsObjects[0];
	while (*current != nullptr) {
		++current;
	} 
	*current = po;
}
