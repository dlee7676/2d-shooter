/* particleSystem.h
This class contains functions that allow a set of randomly moving particles to be generated. */

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include "particle.h"

using namespace std;

class ParticleSystem {

private:
	vector<Particle> particleList;

public:
	ParticleSystem();
	void emit(D3DXVECTOR3 pos, int type_, int life_, float speed_, D3DXVECTOR3 direction);
	void update();
	vector<Particle> getParticles();
};

#endif