#include "particleSystem.h"

ParticleSystem::ParticleSystem() {

}

void ParticleSystem::emit(D3DXVECTOR3 pos, int type_, float speed_, D3DXVECTOR3 direction) {
	Particle next;
	RECT bounds;
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = 0;
	bounds.bottom = 0;
	int life = rand()%200+150;
	next.init(pos, type_, bounds, speed_, direction, life);
	particleList.push_back(next);
}

void ParticleSystem::update() {
	D3DXVECTOR3 current;
	float speed;
	for (int i = 0; i < particleList.size(); i++) {
		current = particleList[i].getHeading();
		speed = particleList[i].getSpeed();
		particleList[i].move(current.x*speed*0.75, current.y*speed*0.75, 0);
		particleList[i].setLifespan(particleList[i].getLifespan()-1);
		particleList[i].setTime(particleList[i].getTime()+1);
		if (particleList[i].getLifespan() <= 0) {
			particleList[i].setActive(false);
			if (!particleList[0].isActive())
				particleList.erase(particleList.begin());
		}
	}
}

vector<Particle> ParticleSystem::getParticles() {
	return particleList;
}