#include "bullet.h"

Bullet::Bullet() : GameObject(){

}

void Bullet::setTarget(D3DXVECTOR3 _target) {
	target = _target;
}

D3DXVECTOR3 Bullet::getTarget() {
	return target;
}

void Bullet::moveSpiral(int i, float rate, float s_, float initS) {
	double x, y, curS;
	curS = this->getS();
	if (curS == 0) 
		this->setS(initS);
	if (i%2 == 0) {
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

void Bullet::randomSpiral(int i, float s_) {
	double x, y, curS;
	curS = this->getS();
	if (rand()%2 == 0) {
		x = curS*cos(curS);
		y = curS*sin(curS);
	}
	else {
		x = -curS*cos(curS);
		y = -curS*sin(curS);
	}
	this->setPos(x+this->getPos(0), y+this->getPos(1), 0);
	this->setS(this->getS()+s_);
}