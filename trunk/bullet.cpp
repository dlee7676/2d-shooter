#include "bullet.h"

Bullet::Bullet() : GameObject(), pointed(false) {

}

void Bullet::setOwner(int _owner) {
	owner = _owner;
}

int Bullet::getOwner() {
	return owner;
}

void Bullet::setTarget(D3DXVECTOR3 _target) {
	target = _target;
}

D3DXVECTOR3 Bullet::getTarget() {
	return target;
}