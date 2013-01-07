#include "particleSystem.h"

ParticleSystem::ParticleSystem() {

}

void ParticleSystem::emit(D3DXVECTOR3 pos, int type_) {
	Particle next;
	RECT bounds;
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = 0;
	bounds.bottom = 0;
	D3DXVECTOR3 random = randomHeading();
	int life = rand()%100+100;
	next.init(pos, type_, bounds, 2, random, life);
	particleList.push_back(next);
}

D3DXVECTOR3 ParticleSystem::randomHeading() {
	D3DXVECTOR3 direction;
	direction = D3DXVECTOR3(rand()%1-2, rand()%10+1, 0);
	D3DXVec3Normalize(&direction, &direction);
	return direction;
}

void ParticleSystem::update() {
	D3DXVECTOR3 current;
	float speed;
	for (int i = 0; i < particleList.size(); i++) {
		current = particleList[i].getHeading();
		speed = particleList[i].getSpeed();
		particleList[i].move(current.x*speed, current.y*speed, current.z*speed);
		particleList[i].setLifespan(particleList[i].getLifespan()-1);
		particleList[i].setTime(particleList[i].getTime()+1);
		if (particleList[i].getLifespan() <= 0) {
			particleList[i].setActive(false);
			particleList.erase(particleList.begin());
		}
	}
}

vector<Particle> ParticleSystem::getParticles() {
	return particleList;
}