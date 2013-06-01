/* class Particle
Contains information for describing the state of randomly emitted particles, such as their direction of travel, 
elapsed time since creation, and time to live.  Functions access and modify these values. */

#ifndef PARTICLE_H
#define PARTICLE_H

#include "gameObject.h"

class Particle : public GameObject {

private:
	// the direction vector the particle is moving in 
	D3DXVECTOR3 heading;
	// how long the particle lasts for
	float lifespan;
	// the time that the particle has existed for
	int time;

public:
	Particle();
	void init(D3DXVECTOR3 pos_, int type_, RECT bounds_, float speed_, D3DXVECTOR3 heading_, float lifespan_);
	D3DXVECTOR3 getHeading();
	float getLifespan();
	void setLifespan(float lifespan_);
	int getTime();
	void setTime(int time_);
};

#endif