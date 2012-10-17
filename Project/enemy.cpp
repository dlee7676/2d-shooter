#include "enemy.h"

Enemy::Enemy() : GameObject(), action(0), waitTime(0) {

}

void Enemy::wait() {
	waitTime++;
}

void Enemy::waitAt(int time, int pos) {
	if (this->getPos(1) >= pos)
		if (this->getWaitTime() < time) 
			waitTime++;
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

void Enemy::aimFire(Bullet* enemyBullets, D3DXVECTOR3 playerPos, int size, int owner, RECT bounds, int type_) {
	for (int i=0; i < size; i++) {
		if (!enemyBullets[i].isActive()) {
			enemyBullets[i].setActive(true);
			enemyBullets[i].setBounds(bounds);
			enemyBullets[i].setOwner(owner);
			enemyBullets[i].setType(type_);
			enemyBullets[i].setStartPos(this->getPos());
			enemyBullets[i].setPos(this->getPos(0), this->getPos(1), this->getPos(2));
			enemyBullets[i].setTarget(playerPos);
			break;
		}
	}
}
