#include "pch.h"

#include <Kore/System.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/Math/Random.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Log.h>
#include "ObjLoader.h"

#include "Collision.h"
#include "PhysicsWorld.h"
#include "PhysicsObject.h"
#include "Memory.h"
#include "ShaderProgram.h"

using namespace Kore;

namespace {

// A simple particle implementation
class Particle {
public:
	Graphics4::VertexBuffer* vb;
	Graphics4::IndexBuffer* ib;

	mat4 M;
	
	// The current position
	vec3 position;
	
	// The current velocity
	vec3 velocity;

	// The remaining time to live
	float timeToLive;

	// The total time time to live
	float totalTimeToLive;

	// Is the particle dead (= ready to be re-spawned?)
	bool dead;

	void init(const Graphics4::VertexStructure& structure) {
		vb = new Graphics4::VertexBuffer(4, structure,0);
		float* vertices = vb->lock();
		SetVertex(vertices, 0, -1, -1, 0, 0, 0);
		SetVertex(vertices, 1, -1, 1, 0, 0, 1);
		SetVertex(vertices, 2, 1, 1, 0, 1, 1); 
		SetVertex(vertices, 3, 1, -1, 0, 1, 0); 
		vb->unlock();

		// Set index buffer
		ib = new Graphics4::IndexBuffer(6);
		int* indices = ib->lock();
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;
		ib->unlock();

		dead = true;
	}


	void Emit(vec3 pos, vec3 velocity, float timeToLive) {
		position = pos;
		this->velocity = velocity;
		dead = false;
		this->timeToLive = timeToLive;
		totalTimeToLive = timeToLive;
	}

	Particle() {
	}


	void SetVertex(float* vertices, int index, float x, float y, float z, float u, float v) {
		vertices[index* 8 + 0] = x;
		vertices[index*8 + 1] = y;
		vertices[index*8 + 2] = z;
		vertices[index*8 + 3] = u;
		vertices[index*8 + 4] = v;
		vertices[index*8 + 5] = 0.0f;
		vertices[index*8 + 6] = 0.0f;
		vertices[index*8 + 7] = -1.0f;
	}

	void render() {
		Graphics4::setVertexBuffer(*vb);
		Graphics4::setIndexBuffer(*ib);
		Graphics4::drawIndexedVertices();
	}

	void Integrate(float deltaTime) {
		timeToLive -= deltaTime;

		if (timeToLive < 0.0f) {
			dead = true;
		}
		
		// Note: We are using no forces or gravity at the moment.

		position += velocity * deltaTime;

		// Build the matrix
		M = mat4::Translation(position.x(), position.y(), position.z()) * mat4::Scale(0.2f, 0.2f, 0.2f);
	}
};


class ParticleSystem {
private:
	ShaderProgram* shaderProgram;
	
	Graphics4::Texture* particleImage;
	
public:

	// The center of the particle system
	vec3 position;

	// The minimum coordinates of the emitter box
	vec3 emitMin;

	// The maximal coordinates of the emitter box
	vec3 emitMax;
	
	// The list of particles
	Particle* particles;

	// The number of particles
	int numParticles;

	// The spawn rate
	float spawnRate;
	
	// When should the next particle be spawned?
	float nextSpawn;

	ParticleSystem(int maxParticles, const Graphics4::VertexStructure& structure, ShaderProgram* shaderProgram) : numParticles(maxParticles), shaderProgram(shaderProgram) {
		particles = new Particle[maxParticles];
		for (int i = 0; i < maxParticles; i++) {
			particles[i].init(structure);
		}
		spawnRate = 0.05f;
		nextSpawn = spawnRate;

		setPosition(vec3(0.5f, 1.3f, 0.5f));
		
		particleImage = new Graphics4::Texture("SuperParticle.png", true);
	}
	
	void setPosition(const Kore::vec3& inPosition, float distance = 0.1f)
	{
		position = inPosition;
		
		emitMin = position - vec3(distance, distance, distance);
		emitMax = position + vec3(distance, distance, distance);
	}

	
	void update(float deltaTime) {
		// Do we need to spawn a particle?
		nextSpawn -= deltaTime;
		bool spawnParticle = false;
		if (nextSpawn < 0) {
			spawnParticle = true;
			nextSpawn = spawnRate;
		}
		
		for (int i = 0; i < numParticles; i++) {
			
			if (particles[i].dead) {
				if (spawnParticle) {
					EmitParticle(i);
					spawnParticle = false;
				}
			}

			particles[i].Integrate(deltaTime);
		}
	}

	void render(SceneParameters& parameters) {
		/************************************************************************/
		/* Exercise P8.1														*/
		/************************************************************************/
		/* Change the matrix V in such a way that the billboards are oriented towards the camera */


		/************************************************************************/
		/* Exercise P8.2														*/
		/************************************************************************/
		/* Animate using at least one new control parameter */		

		for (int i = 0; i < numParticles; i++) {
			// Skip dead particles
			if (particles[i].dead) continue;
			
			shaderProgram->Set(parameters, particles[i].M * parameters.V, particleImage);
			
			particles[i].render();
		}
	}

	float getRandom(float minValue, float maxValue) {
		int randMax = 1000000;
		int randInt = Random::get(0, randMax);
		float r =  (float) randInt / (float) randMax;
		return minValue + r * (maxValue - minValue);
	}

	void EmitParticle(int index) {
		// Calculate a random position inside the box
		float x = getRandom(emitMin.x(), emitMax.x());
		float y = getRandom(emitMin.y(), emitMax.y());
		float z = getRandom(emitMin.z(), emitMax.z());

		vec3 pos;
		pos.set(x, y, z);

		vec3 velocity(0, 0.3f, 0);

		particles[index].Emit(pos, velocity, 3.0f);
	}
};

	const int width = 512;
	const int height = 512;
	
	float angle = 0.0f;

	// null terminated array of MeshObject pointers
	MeshObject* objects[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	// null terminated array of PhysicsObject pointers
	PhysicsObject* physicsObjects[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	// The view projection matrix aka the camera
	mat4 P;
	mat4 V;
	mat4 PV;

	vec3 cameraPosition;

	MeshObject* sphere;
	PhysicsObject* po;

	PhysicsWorld physics;

	ParticleSystem* particleSystem;
	
	SceneParameters parameters;

	double startTime;
	double lastTime;

	void update() {
		double t = System::time() - startTime;
		double deltaT = t - lastTime;
		// Kore::log(Info, "%f\n", deltaT);
		lastTime = t;
		
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag | Graphics4::ClearDepthFlag, 0xff9999FF, 1000.0f);

		angle += 0.3f * (float) deltaT;

		float x = 0 + 3 * Kore::cos(angle);
		float z = 0 + 3 * Kore::sin(angle);
		
		cameraPosition.set(x, 2, z);

		P = mat4::Perspective(20.0f, (float)width / (float)height, 0.1f, 100.0f);
		V = mat4::lookAt(vec3(x, 2, z), vec3(0, 2, 0), vec3(0, 1, 0));
		PV = P * V;

		parameters.PV = PV;
		parameters.V = V;
		
		// Iterate the MeshObjects and render them
		MeshObject** current = &objects[0];
		while (*current != nullptr) {
			(*current)->render(parameters);
			++current;
		} 

		// Update the physics and render the meshes
		physics.Update((float) deltaT);

		PhysicsObject** currentP = &physics.physicsObjects[0];
		while (*currentP != nullptr) {
			(*currentP)->UpdateMatrix();
			(*currentP)->Mesh->render(parameters);
			++currentP;
		}
		
		particleSystem->update((float) deltaT);
		particleSystem->render(parameters);

		Graphics4::end();
		Graphics4::swapBuffers();
	}

	void SpawnSphere(vec3 Position, vec3 Velocity) {
		PhysicsObject* po = new PhysicsObject();
		po->SetPosition(Position);
		po->Velocity = Velocity;
		po->Collider.radius = 0.2f;

		po->Mass = 5;
		po->Mesh = sphere;
			
		po->ApplyImpulse(Velocity);
		physics.AddObject(po);
	}

	void keyDown(KeyCode code) {
		if (code == KeySpace) {
			
			// The impulse should carry the object forward
			// Use the inverse of the view matrix
			vec4 impulse(0, 0.4f, 2.0f, 0);
			mat4 VI = V.Invert();
			impulse = VI * impulse;
			
			vec3 impulse3(impulse.x(), impulse.y(), impulse.z());

			
			SpawnSphere(cameraPosition + impulse3 *0.2f, impulse3);
		}
	}

	void keyUp(KeyCode code) {
		if (code == KeyLeft) {
			// ...
		}
	}

	void mouseMove(int windowId, int x, int y, int movementX, int movementY) {

	}
	
	void mousePress(int windowId, int button, int x, int y) {

	}

	void mouseRelease(int windowId, int button, int x, int y) {
		
	}

	void init() {
		Memory::init();
		
		// This defines the structure of your Vertex Buffer
		Graphics4::VertexStructure structure;
		structure.add("pos", Graphics4::Float3VertexData);
		structure.add("tex", Graphics4::Float2VertexData);
		structure.add("nor", Graphics4::Float3VertexData);

		// Set up shader
		ShaderProgram* shader = new ShaderProgram("shader.vert", "shader.frag", structure, true);
		ShaderProgram* shaderParticle = new ShaderProgram("shader.vert", "shader.frag", structure, false);
		
		objects[0] = new MeshObject("Base.obj", "Level/basicTiles6x6.png", structure, shader);
		objects[0]->M = mat4::Translation(0.0f, 1.0f, 0.0f);
		
		sphere = new MeshObject("ball_at_origin.obj", "Level/unshaded.png", structure, shader);
		
		SpawnSphere(vec3(0, 2, 0), vec3(0, 0, 0));
		
		particleSystem = new ParticleSystem(100, structure, shaderParticle);
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("Exercise 8", width, height);

	Kore::Random::init(42);

	init();

	Kore::System::setCallback(update);

	startTime = (float) System::time();

	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Move = mouseMove;
	Mouse::the()->Press = mousePress;
	Mouse::the()->Release = mouseRelease;

	Kore::System::start();

	return 0;
}
