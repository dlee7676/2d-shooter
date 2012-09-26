#ifndef ENEMY_H
#define ENEMY_H

#include "gameObject.h"
#include "bullet.h"

class Enemy : public GameObject {
	
private:
	int action, waitTime, cooldown;

public:
	Enemy::Enemy();
	void wait();
	void waitAt(int time, int pos);
	int getWaitTime();
	void setWaitTime(int time);
	int getCooldown();
	void setCooldown(int time);
	int getAction();
	void setAction(int command);
	void Enemy::aimFire(Bullet* enemyBullets, D3DXVECTOR3 playerPos, int size, int owner, RECT bounds);
};

#endif