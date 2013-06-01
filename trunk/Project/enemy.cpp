#include "enemy.h"

Enemy::Enemy() : GameObject(), heading(0,0,0), action(0), waitTime(0) {

}

/* void init(float x, float y, float z, RECT bounds_, int type_, float midX, float midY, float endX, float endY, int life_, float speed_):
	Initializes an enemy with the normal GameObject parameters as well as movement coordinates, life, and aiming direction.
	Parameters:
	float x, y, z: the coordinates of the starting position of the enemy.
	RECT bounds_: the bounding box of the enemy.
	int type_: the kind of enemy this is. 
	float midX, midY: the first control point on a spline determining the enemy's movement. 
	float endX, endY: the end point of the spline (the final destination point).
	int life: the amount of damage the enemy can take.
	float speed: a higher value makes the enemy move faster. */

void Enemy::init(float x, float y, float z, RECT bounds_, int type_, float midX, float midY, float endX, float endY, float life_, float speed_) {
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

/* void wait(): increments the enemy's wait timer, which is used to control when it changes actions. */
void Enemy::wait() {
	waitTime++;
}

/* void moveTo(size_t dest): moves an enemy along a spline with two control points; essentially enemies
	will travel from a start point (generally offscreen) to a midpoint, where they may perform a different
	action, and then to an endpoint (offscreen again). 
	Parameters:
	size_t direction: determines the direction the enemy moves on the spline. 1 if the enemy is entering the screen, 0 if it is leaving. 
	float rate: how far to move along the spline at default speed (usually stays the same). */

void Enemy::moveTo(size_t direction, float rate) {
	D3DXVECTOR3 newPos;
	if (direction == 1) {
		D3DXVec3CatmullRom(&newPos, &this->getStartPos(), &this->getStartPos(), &mid, &mid, this->getS());
		setS(this->getS() + rate*this->getSpeed());
		setPos(newPos.x, newPos.y, newPos.z);
		setWaitTime(0);
	}
	if (direction == 0) {
		D3DXVec3CatmullRom(&newPos, &end, &end, &mid, &mid, this->getS());
		setS(this->getS() - rate*this->getSpeed());
		setPos(newPos.x, newPos.y, newPos.z);
		setWaitTime(0);
	}
}

// getter and setter for the direction that the targeting vector should rotate in
size_t Enemy::getRotateDirection() {
	return rotateDirection;
}
void Enemy::setRotateDirection(size_t direction) {
	rotateDirection = direction;
}

// getter and setter for enemy wait time
int Enemy::getWaitTime() {
	return waitTime;
}
void Enemy::setWaitTime(int time) {
	waitTime = time;
}

// getter and setter for cooldown time (how long it will be before the enemy can fire again)
int Enemy::getCooldown() {
	return cooldown;
}
void Enemy::setCooldown(int time) {
	cooldown = time;
}

// getter and setter for enemy action type. (0 = entering the screen, 1 = waiting, 2 = leaving the screen, 3 = boss actions) */
int Enemy::getAction() {
	return action;
}
void Enemy::setAction(int command) {
	action = command;
}

// getter and setter for enemy life total.
float Enemy::getLife() {
	return life;
}
void Enemy::setLife(float value) {
	life = value;
}

// getter and setter for current enemy movement direction (used for boss movement pattern) 
D3DXVECTOR3 Enemy::getHeading() {
	return heading;
}
void Enemy::setHeading(D3DXVECTOR3 _heading) {
	heading = _heading;
}

// getter and setter for the vector that determines where the enemy fires shots at.
D3DXVECTOR3 Enemy::getTargeting() {
	return targeting;
}
void Enemy::setTargeting(D3DXVECTOR3 _targeting) {
	targeting = _targeting;
}

// getters and setters for the control points of the enemy's movement.
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

/* void fire(vector<Bullet>* enemyBullets, D3DXVECTOR3 targetPos, D3DXVECTOR3 startPos, int size, RECT bounds, int type_, float speed_):
	Fires a bullet aimed at a given position.
	Parameters:
	vector<Bullet>* enemyBullets: the set of bullets to be used.
	D3DXVECTOR3 targetPos: the position vector of the target to fire at.
	D3DXVECTOR3 startPos: the position to initialize the bullet at.
	int size: the size of the bullet vector.
	RECT bounds: the bounding box of the bullet to be fired.
	int type_: the type of the bullet. 
		0 = flies straight
		1 = flies in a spiral
		2 = flies erratically
	float speed_: the speed of the bullet. */

void Enemy::fire(vector<Bullet>* enemyBullets, D3DXVECTOR3 targetPos, D3DXVECTOR3 startPos, int size, RECT bounds, int type_, float speed_) {
	Bullet next;
	enemyBullets->push_back(next);
	enemyBullets->back().init(startPos.x, startPos.y, startPos.z, bounds, type_, speed_);
	D3DXVECTOR3 target = D3DXVECTOR3(targetPos.x+10 - this->getPos(0), targetPos.y+5 - this->getPos(1), 0);
	enemyBullets->back().setTarget(targetPos);
}

/* D3DXVECTOR3 rotateAim(int direction, float angle): rotates the position vector the enemy is aiming at by a given angle
	and returns the rotated vector.
	Parameters:
	int direction: the direction to rotate; 1 for counterclockwise, 0 for clockwise.
	float angle: the angle to rotate by. */

D3DXVECTOR3 Enemy::rotateAim(int direction, float angle, float rate) {
	// apply a rotation matrix to the x and y components of the targeting vector
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
	newTarget.x *= rate;
	newTarget.y *= rate;
	return newTarget;
}

/* D3DXVECTOR3 fireSpiral(float t): returns a position vector at a given point in a spiral function; used for generating bullets in a spiral shape.
	Parameters:
	float t: how far along the spiral to find the position coordinates. */

D3DXVECTOR3 Enemy::fireSpiral(float t) {
	float x = t*cos(t);
	float y = t*sin(t);
	return D3DXVECTOR3(x,y,0);
}

/* D3DXVECTOR3 aim8Ways(int num): Returns a vector that is aimed in one of 8 directions depending on an input number.
	Used in a loop to fire bullets in the cardinal directions and the directions between them. 
	Parameters:
	int num: determines the direction to fire in. */

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

/* D3DXVECTOR3 aim3Ways(int num): Similar to the above function (aim8Ways) but returns a vector pointing
	in one of three directions (more widely spaced than in the 8 way function).
	Parameters:
	int num: determines the direction to fire in. */

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

/* void bossPattern(int interval, int time): causes an enemy to periodically move in a random direction.
	Used with bosses, as they stay on screen for long periods of time. 
	Parameters: 
	int interval: how often the enemy should change direction (higher takes longer).
	int time: the current level time.*/

void Enemy::bossPattern(int interval, int time) {
	D3DXVECTOR3 direction;
	// after every interval in time units, there is a 50% chance that the enemy will move in a new direction for the next interval.
    // Otherwise, it will stay still.
	if (time % interval == 0) {
		if (rand()%2 == 0) {
			direction = D3DXVECTOR3((float)(rand()%25-10), (float)(rand()%25-10), 0);
			D3DXVec3Normalize(&direction, &direction);
			this->setHeading(direction);
		}
		else this->setHeading(D3DXVECTOR3(0,0,0));
	}
	direction = this->getHeading();
	// move in the opposite direction if the enemy gets too close to the edges of the screen
	if (this->getPos(0) + direction.x*this->getSpeed() > 150 && this->getPos(0) + direction.x*this->getSpeed() < 600
		&& this->getPos(1) + direction.y*this->getSpeed() > 50 && this->getPos(1) + direction.y*this->getSpeed() < 200)
		this->move(direction.x*this->getSpeed(), direction.y*this->getSpeed(), direction.z*this->getSpeed());
	else this->setHeading(D3DXVECTOR3(-1*direction.x,-1*direction.y,0));
}