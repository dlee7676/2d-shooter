#include "particleSystem.h"

ParticleSystem::ParticleSystem() {

}

/* void emit(D3DXVECTOR3 pos, int type_, float life_, float speed_, D3DXVECTOR3 direction):
	Initializes a particle and adds it to a vector of particles handled by this object.
	Parameters:
	D3DXVECTOR3 pos: the starting position of the particle.
	int type_: the type of the particle.
	float life_: the time the particle will exist for.
	float speed_: the speed of the particle.
	D3DXVECTOR3 direction: the direction the particle will travel in. */

void ParticleSystem::emit(D3DXVECTOR3 pos, int type_, float life_, float speed_, D3DXVECTOR3 direction) {
	Particle next;
	RECT bounds;
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = 0;
	bounds.bottom = 0;
	float life = life_;
	next.init(pos, type_, bounds, speed_, direction, life);
	particleList.push_back(next);
}

/* void update(): Moves the particles in their assigned directions and increments their time values.  Removes particles when
	their time values exceed their lifespan. */
void ParticleSystem::update() {
	D3DXVECTOR3 current;
	float speed;
	for (size_t i = 0; i < particleList.size(); i++) {
		current = particleList[i].getHeading();
		speed = particleList[i].getSpeed();
		particleList[i].move(current.x*speed, current.y*speed, 0);
		particleList[i].setLifespan(particleList[i].getLifespan()-1);
		particleList[i].setTime(particleList[i].getTime()+1);
		if (particleList[i].getLifespan() <= 0) {
			particleList[i].setActive(false);
			if (!particleList[0].isActive())
				particleList.erase(particleList.begin());
		}
	}
}

// getter for the vector of particles associated with the object
vector<Particle> ParticleSystem::getParticles() {
	return particleList;
}