#include "bullet.h"

Bullet::Bullet() : GameObject(){

}

// getter and setter for the position of the bullet's target (where it is aimed at)
D3DXVECTOR3 Bullet::getTarget() {
	return target;
}
void Bullet::setTarget(D3DXVECTOR3 _target) {
	target = _target;
}

/* void moveSpiral(size_t direction, float rate, float s_, float initS): moves a bullet along a spiral path.
	Parameters:
	size_t direction: 0 or 1, determines which way the bullet should spiral. 
	float rate: Factor to modify the speed the bullet travels (normally will not change).
	float s_: The relative amount to move the bullet along the path.
	float initS: how far along the spiral the bullet should begin its path at. */

void Bullet::moveSpiral(size_t direction, float rate, float s_, float initS) {
	float x, y, curS;
	curS = this->getS();
	if (curS == 0) 
		this->setS(initS);
	if (direction == 0) {
		x = curS*cos(curS);
		y = curS*sin(curS);
	}
	else {
		x = -curS*cos(curS);
		y = -curS*sin(curS);
	}
	this->setPos(rate*x+this->getPos(0), rate*y+this->getPos(1), 0);
	this->setS(curS+s_);
}

/* void randomSpiral(float rate, float s_): Moves a bullet in an erratic path based on a spiral function.
	Parameters:
	float rate: Factor to modify the speed the bullet travels (normally will not change).
	float s_: The relative amount to move the bullet along the path.
	float initS: how far along the spiral the bullet should begin its path at. */

void Bullet::randomSpiral(float rate, float s_, float initS) {
	float x, y, curS;
	curS = this->getS();
	if (curS == 0) 
		this->setS(initS);
	if (rand()%2 == 0) {
		x = curS*cos(curS);
		y = curS*sin(curS);
	}
	else {
		x = -curS*cos(curS);
		y = -curS*sin(curS);
	}
	this->setPos(rate*x+this->getPos(0), rate*y+this->getPos(1), 0);
	this->setS(this->getS()+s_);
}