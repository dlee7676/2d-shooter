#include "gameObject.h"

GameObject::GameObject() :pos(0,0,0), active(false) {

}

bool GameObject::isActive() {
	if (active)
		return true;
	else return false;
}

void GameObject::setActive(bool setting) {
	active = setting;
}

D3DXVECTOR3 GameObject::getPos() {
	return pos;
}

int GameObject::getPos(int coord) {
	switch(coord) {
		case 0:
			return pos.x;
		case 1:
			return pos.y;
		case 2:
			return pos.z;	
		default:
			return pos.x;
	}
}

void GameObject::setPos(int x, int y, int z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

void GameObject::move(int x, int y, int z) {
	pos.x += x;
	pos.y += y;
	pos.z += z;
}

void GameObject::moveTo(int dest, D3DXVECTOR3 rate) {
	if (dest > 0)
		if (this->getPos(1) < dest) 
			this->move(rate.x,rate.y,rate.z);
	if (dest < 0)
		if (this->getPos(1) > dest) 
			this->move(rate.x,rate.y,rate.z);
}
