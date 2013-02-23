#include "enemy.h"

Enemy::Enemy() : GameObject(), heading(0,0,0), action(0), waitTime(0) {

}

void Enemy::init(int x, int y, int z, RECT bounds_, int type_, int midX, int midY, int endX, int endY, int life_, float speed_) {
	this->GameObject::init(x,y,z,bounds_,type_, speed_);
	mid.x = midX;
	mid.y = midY;
	mid.z = 0;
	end.x = endX;
	end.y = endY;
	end.z = 0;
	life = life_;
	targeting = D3DXVECTOR3(0,1,0);
}

void Enemy::wait() {
	waitTime++;
}

void Enemy::waitAt(int time, int pos) {
	if (this->getPos(1) >= pos)
		if (this->getWaitTime() < time) 
			waitTime++;
}

void Enemy::moveTo(int dest) {
	D3DXVECTOR3 newPos;
	if (dest == 1) {
		D3DXVec3CatmullRom(&newPos, &this->getStartPos(), &this->getStartPos(), &mid, &mid, this->getS());
		setS(this->getS() + 0.001*this->getSpeed());
		setPos(newPos.x, newPos.y, newPos.z);
		setWaitTime(0);
	}
	if (dest == 0) {
		D3DXVec3CatmullRom(&newPos, &end, &end, &mid, &mid, this->getS());
		setS(this->getS() - 0.001*this->getSpeed());
		setPos(newPos.x, newPos.y, newPos.z);
		setWaitTime(0);
	}
}

int Enemy::getWaitTime() {
	return waitTime;
}

void Enemy::setWaitTime(int time) {
	waitTime = time;
}

int Enemy::getCooldown() {
	return cooldown;
}

void Enemy::setCooldown(int time) {
	cooldown = time;
}

int Enemy::getAction() {
	return action;
}

void Enemy::setAction(int command) {
	action = command;
}

float Enemy::getLife() {
	return life;
}

void Enemy::setLife(float value) {
	life = value;
}

D3DXVECTOR3 Enemy::getHeading() {
	return heading;
}

void Enemy::setHeading(D3DXVECTOR3 _heading) {
	heading = _heading;
}

D3DXVECTOR3 Enemy::getTargeting() {
	return targeting;
}

void Enemy::setTargeting(D3DXVECTOR3 _targeting) {
	targeting = _targeting;
}

D3DXVECTOR3 Enemy::getMid() {
	return mid;
}

void Enemy::setMid(D3DXVECTOR3 _mid) {
	mid = _mid;
}

D3DXVECTOR3 Enemy::getEnd() {
	return end;
}

void Enemy::setEnd(D3DXVECTOR3 _end) {
	end = _end;
}

void Enemy::aimFire(Bullet* enemyBullets, D3DXVECTOR3 targetPos, D3DXVECTOR3 startPos, int size, int owner, RECT bounds, RECT init_, int type_, float speed_) {
	for (int i=0; i < size; i++) {
		if (!enemyBullets[i].isActive()) {
			enemyBullets[i].init(startPos.x, startPos.y, startPos.z, bounds, init_, type_, speed_);
			D3DXVECTOR3 target = D3DXVECTOR3(targetPos.x - this->getPos(0), targetPos.y - this->getPos(1), 0);
			enemyBullets[i].setTarget(targetPos);
			break;
		}
	}
}
