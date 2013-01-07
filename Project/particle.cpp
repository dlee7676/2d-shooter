#include "particle.h"

Particle::Particle() : GameObject() {

}

void Particle::init(D3DXVECTOR3 pos_, int type_, RECT bounds_, float speed_, D3DXVECTOR3 heading_, int lifespan_) {
	this->GameObject::init(pos_.x,pos_.y,pos_.z, bounds_, type_, speed_);
	heading = heading_;
	lifespan = lifespan_;
	time = 0;
}

D3DXVECTOR3 Particle::getHeading() {
	return heading;
}

int Particle::getLifespan() {
	return lifespan;
}

void Particle::setLifespan(int lifespan_) {
	lifespan = lifespan_;
}

int Particle::getTime() {
	return time;
}

void Particle::setTime(int time_) {
	time = time_;
}