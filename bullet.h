#ifndef BULLET_H
#define BULLET_H

#include "gameObject.h"

class Bullet : public GameObject {
	
private:
	int owner;
	D3DXVECTOR3 startPos;
	D3DXVECTOR3 target;
	bool pointed;

public:
	Bullet::Bullet();
	void setOwner(int _owner);
	int getOwner();
	void setPointed(bool _pointed);
	bool isPointed();
	void setStartPos(D3DXVECTOR3 _startPos);
	D3DXVECTOR3 getStartPos();
	void Bullet::setTarget(D3DXVECTOR3 _target);
	D3DXVECTOR3 Bullet::getTarget();
};

#endif