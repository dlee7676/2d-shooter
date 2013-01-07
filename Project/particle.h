#ifndef PARTICLE_H
#define PARTICLE_H

#include "gameObject.h"

class Particle : public GameObject {

private:
	D3DXVECTOR3 heading;
	int lifespan, time;

public:
	Particle();
	void init(D3DXVECTOR3 pos_, int type_, RECT bounds_, float speed_, D3DXVECTOR3 heading_, int lifespan_);
	D3DXVECTOR3 getHeading();
	int getLifespan();
	void setLifespan(int lifespan_);
	int getTime();
	void setTime(int time_);
};

#endif