#ifndef ENEMY_H
#define ENEMY_H

#include "gameObject.h"
#include "bullet.h"

class Enemy : public GameObject {
	
private:
	int action, waitTime, cooldown;
	float life;
	D3DXVECTOR3 mid, end, heading, targeting;

public:
	Enemy::Enemy();
	void init(int x, int y, int z, RECT bounds_, int type_, int midX, int midY, int endX, int endY, int life_, float speed_);
	void wait();
	void waitAt(int time, int pos);
	void moveTo(int dest);
	int getWaitTime();
	void setWaitTime(int time);
	int getCooldown();
	void setCooldown(int time);
	int getAction();
	void setAction(int command);
	float getLife();
	void setLife(float value);
	D3DXVECTOR3 getHeading();
	void setHeading(D3DXVECTOR3 _heading);
	D3DXVECTOR3 getTargeting();
	void setTargeting(D3DXVECTOR3 _targeting);
	D3DXVECTOR3 getMid();
	void setMid(D3DXVECTOR3 _mid);
	D3DXVECTOR3 getEnd();
	void setEnd(D3DXVECTOR3 _end);
	void Enemy::aimFire(Bullet* enemyBullets, D3DXVECTOR3 playerPos, D3DXVECTOR3 startPos, int size, int owner, RECT bounds, RECT init_, int type_);
};

#endif