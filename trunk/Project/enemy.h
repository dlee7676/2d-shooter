/* class Enemy
Contains information specific to game enemies and functions for controlling these values as well as
enemy actions. */

#ifndef ENEMY_H
#define ENEMY_H

#include "gameObject.h"
#include "bullet.h"
#include <vector>

using namespace std;

class Enemy : public GameObject {
	
private:
	/* the value of the action variable determines what the enemy is doing.
		0 = entering the screen
		1 = waiting
		2 = leaving the screen
		3 = move in a boss enemy pattern */
	int action;
	// time that the enemy has been waiting at a position and time that the enemy has to wait to fire
	int waitTime, cooldown;
	// value used to store the direction the enemy should rotate the position vector it fires at in certain situations
	size_t rotateDirection;
	// the life total of the enemy
	float life;
	// control points on a spline that determine where the enemy moves
	D3DXVECTOR3 mid, end;
	// the direction the enemy moves in (used for straight line movements) and the direction the enemy fires bullets in
	D3DXVECTOR3 heading, targeting;


public:
	Enemy::Enemy();
	void init(float x, float y, float z, RECT bounds_, int type_, float midX, float midY, float endX, float endY, float life_, float speed_);
	void wait();
	void waitAt(int time, float pos);
	void moveTo(size_t dest, float rate);
	size_t getRotateDirection();
	void setRotateDirection(size_t direction);
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
	D3DXVECTOR3 rotateAim(int direction, float angle, float rate);
	D3DXVECTOR3 fireSpiral(float t);
	D3DXVECTOR3 aim8Ways(int num);
	D3DXVECTOR3 aim3Ways(int num);
	void bossPattern(int interval, int time);
};

#endif