#include "bullet.h"

Bullet::Bullet() : GameObject(){

}

void Bullet::setTarget(D3DXVECTOR3 _target) {
	target = _target;
}

D3DXVECTOR3 Bullet::getTarget() {
	return target;
}