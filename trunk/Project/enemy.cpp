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
	targeting = D3DXVECTOR3(0,-1,0);
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

int Enemy::getRotateDirection() {
	return rotateDirection;
}
	
void Enemy::setRotateDirection(int direction) {
	rotateDirection = direction;
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

void Enemy::fire(vector<Bullet>* enemyBullets, D3DXVECTOR3 targetPos, D3DXVECTOR3 startPos, int size, RECT bounds, int type_, float speed_) {
	Bullet next;
	enemyBullets->push_back(next);
	enemyBullets->back().init(startPos.x, startPos.y, startPos.z, bounds, type_, speed_);
	D3DXVECTOR3 target = D3DXVECTOR3(targetPos.x+10 - this->getPos(0), targetPos.y+5 - this->getPos(1), 0);
	enemyBullets->back().setTarget(targetPos);
//enemyBullets[i].setSpeed(2);
}

D3DXVECTOR3 Enemy::rotateAim(int direction, double angle) {
	if (direction == 1) {
		targeting.x = targeting.x*cos(angle)-targeting.y*sin(angle);
		targeting.y = targeting.x*sin(angle)+targeting.y*cos(angle);
	}
	else {
		targeting.x = targeting.x*cos(angle)+targeting.y*sin(angle);
		targeting.y = targeting.y*cos(angle)-targeting.x*sin(angle);
	}
	targeting.z = 0;

	D3DXVECTOR3 newTarget = targeting;
	if (abs(targeting.x) < 0.001)
		setTargeting(D3DXVECTOR3(0,1,0));
	newTarget.x *= 3;
	newTarget.y *= 3;
	return newTarget;
}


D3DXVECTOR3 Enemy::fireSpiral(double t) {
	double x = t*cos(t);
	double y = t*sin(t);
	return D3DXVECTOR3(x,y,0);
}

D3DXVECTOR3 Enemy::aim8Ways(int num) {
	D3DXVECTOR3 shot;
	if (num == 0 || num == 4)
		shot.x = 0;
	if (num == 1 || num == 2 || num == 3)
		shot.x = 1;
	if (num == 5 || num == 6 || num == 7)
		shot.x = -1;
	if (num == 0 || num == 1 || num == 7)
		shot.y = 1;
	if (num == 2 || num == 6)
		shot.y = 0;
	if (num == 3 || num == 4 || num == 5)
		shot.y = -1;
	shot.z = 0;
	return shot;
}

D3DXVECTOR3 Enemy::aim3Ways(int num) {
	D3DXVECTOR3 shot;
	if (num == 0) {
		shot.x = -1*sqrt(3.0f);
		shot.y = 1;
	}
	if (num == 1) {
		shot.x = 0;
		shot.y = 1;
	}
	if (num == 2) {
		shot.x = sqrt(3.0f);
		shot.y = 1;
	}		
	shot.z = 0;
	return shot;
}

void Enemy::bossPattern(int interval, int time) {
	D3DXVECTOR3 direction;
	if (time % interval == 0) {
		if (rand()%2 == 0) {
			direction = D3DXVECTOR3(rand()%25-10, rand()%25-10, 0);
			D3DXVec3Normalize(&direction, &direction);
			this->setHeading(direction);
		}
		else this->setHeading(D3DXVECTOR3(0,0,0));
	}
	direction = this->getHeading();
	if (this->getPos(0) + direction.x*2 > 150 && this->getPos(0) + direction.x*2 < 600
		&& this->getPos(1) + direction.y*2 > 50 && this->getPos(1) + direction.y*2 < 200)
		this->move(direction.x*this->getSpeed(), direction.y*this->getSpeed(), direction.z*this->getSpeed());
	else this->setHeading(D3DXVECTOR3(-1*direction.x,-1*direction.y,0));
}