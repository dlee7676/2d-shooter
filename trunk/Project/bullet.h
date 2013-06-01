/* class Bullet:
Stores the position a fired bullet is aimed at, and contains functions for manipulating the position of 
a bullet. */

#ifndef BULLET_H
#define BULLET_H

#include "gameObject.h"

class Bullet : public GameObject {
	
private:
	// the position that the bullet is aimed at
	D3DXVECTOR3 target;

public:
	Bullet::Bullet();
	void setTarget(D3DXVECTOR3 _target);
	D3DXVECTOR3 getTarget();
	void moveSpiral(size_t direction, float rate, float s_, float initS);
	void randomSpiral(float rate, float s_, float initS);
};

#endif