#include "gameObject.h"

GameObject::GameObject() :pos(0,0,0), active(false), exploding(false) {

}

void GameObject::init(int x, int y, int z, RECT bounds_, int type_, float speed_) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
	startPos.x = x;
	startPos.y = y;
	startPos.z = z;
	s = 0.0f;
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = bounds_.right - bounds_.left;
	bounds.bottom = bounds_.bottom - bounds_.top;
	active = true;
	exploding = false;
	animTime = 10;
	type = type_;
	speed = speed_;
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
		if (pos.y < dest) 
			this->move(rate.x,rate.y,rate.z);
	if (dest < 0)
		if (pos.y > dest) 
			this->move(rate.x,rate.y,rate.z);
}

D3DXVECTOR3 GameObject::getStartPos() {
	return startPos;
}

void GameObject::setStartPos(D3DXVECTOR3 _startPos) {
	startPos = _startPos;
}

float GameObject::getS() {
	return s;
}

void GameObject::setS(float s_) {
	s = s_;
}

void GameObject::setBounds(RECT bounds_) {
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = bounds_.right - bounds_.left;
	bounds.bottom = bounds_.bottom - bounds_.top;
}

RECT GameObject::getBounds() {
	return bounds;
}
	
bool GameObject::inBounds(GameObject test) {
	RECT testRect = test.getBounds();
	if (testRect.right + test.getPos(0) < bounds.left + pos.x || testRect.bottom + test.getPos(1) < bounds.top + pos.y || 
		testRect.left + test.getPos(0) > bounds.right + pos.x || testRect.top + test.getPos(1) > bounds.bottom + pos.y)
		return false;
	else return true;
}

bool GameObject::inBounds(RECT test, int x, int y) {
	if (test.right + x < bounds.left + pos.x || test.bottom + y < bounds.top + pos.y || 
		test.left + x > bounds.right + pos.x || test.top + y > bounds.bottom + pos.y)
		return false;
	else return true;
}

bool GameObject::isExploding() {
	return exploding;
}

void GameObject::setExploding(bool exploding_) {
	exploding = exploding_;
}

int GameObject::getAnimTime() {
	return animTime;
}

void GameObject::setAnimTime(int time) {
	animTime = time;
}

int GameObject::getType() {
	return type;
}

void GameObject::setType(int type_) {
	type = type_;
}

float GameObject::getSpeed() {
	return speed;
}
