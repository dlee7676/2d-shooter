#include "particle.h"

Particle::Particle() : GameObject() {

}

/* void init(D3DXVECTOR3 pos_, int type_, RECT bounds_, float speed_, D3DXVECTOR3 heading_, float lifespan_): initializes a particle at a given position with
	a bounding box, speed, direction to travel in, and lifespan.
	Parameters:
	D3DXVECTOR3 pos_: the position to initialize the particle at.
	int type_: the type of particle; 0 = small, 1 = large.
	RECT bounds_: the bounding box of the particle.
	float speed_: the speed of the particle.
	D3DXVECTOR3 heading_: the direction the particle will travel in.
	float lifespan_: the length of time the particle will exist for. */

void Particle::init(D3DXVECTOR3 pos_, int type_, RECT bounds_, float speed_, D3DXVECTOR3 heading_, float lifespan_) {
	this->GameObject::init(pos_.x,pos_.y,pos_.z, bounds_, type_, speed_);
	heading = heading_;
	lifespan = lifespan_;
	time = 0;
}

// returns the direction the particle is traveling in
D3DXVECTOR3 Particle::getHeading() {
	return heading;
}

// getter and setter for the particle's lifespan
float Particle::getLifespan() {
	return lifespan;
}
void Particle::setLifespan(float lifespan_) {
	lifespan = lifespan_;
}

// getter and setter for the time the particle has existed for
int Particle::getTime() {
	return time;
}
void Particle::setTime(int time_) {
	time = time_;
}