#ifndef BULLET_H
#define BULLET_H

#include "gameObject.h"

class Bullet : public GameObject {
	
private:
	D3DXVECTOR3 target;

public:
	Bullet::Bullet();
	void setTarget(D3DXVECTOR3 _target);
	D3DXVECTOR3 getTarget();
	void moveSpiral(int i, float rate, float s_, float initS);
	void randomSpiral(int i, float rate, float s_);
};

#endif