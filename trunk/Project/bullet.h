#ifndef BULLET_H
#define BULLET_H

#include "gameObject.h"

class Bullet : public GameObject {
	
private:
	D3DXVECTOR3 target;

public:
	Bullet::Bullet();
	void Bullet::setTarget(D3DXVECTOR3 _target);
	D3DXVECTOR3 Bullet::getTarget();
};

#endif