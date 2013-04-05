#ifndef ENEMY_H
#define ENEMY_H

#include "gameObject.h"
#include "bullet.h"
#include <vector>

using namespace std;

class Enemy : public GameObject {
	
private:
	int action, waitTime, cooldown, rotateDirection;
	float life;
	D3DXVECTOR3 mid, end, heading, targeting;

public:
	Enemy::Enemy();
	void init(int x, int y, int z, RECT bounds_, int type_, int midX, int midY, int endX, int endY, int life_, float speed_);
	void wait();
	void waitAt(int time, int pos);
	void moveTo(int dest);
	int getRotateDirection();
	void setRotateDirection(int direction);
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
	void fire(vector<Bullet>* enemyBullets, D3DXVECTOR3 playerPos, D3DXVECTOR3 startPos, int size, RECT bounds, int type_, float speed_);
	D3DXVECTOR3 rotateAim(int direction, double angle);
	D3DXVECTOR3 fireSpiral(double t);
	D3DXVECTOR3 aim8Ways(int num);
	D3DXVECTOR3 aim3Ways(int num);
	void bossPattern(int interval, int time);
	//void moveEnemies();
};

#endif