/* level.h
This class controls events related to a specific game level, such as creating enemies or signalling 
that the level is complete. */

#ifndef LEVEL_H
#define LEVEL_H

#include "enemy.h"
#include <vector>
#include <string>
#include <map>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define PI atan(1.0f)*4
#define MAX_BULLETS 1000

using namespace std;

class Level {
private:
	// pointer to a container for enemies to use in the level
	vector<Enemy>* pEnemies;
	// container for additional Enemy objects that act as additions to enemies in the main container; allows 
	// one enemy to effectively fire in multiple patterns at the same time
	vector<Enemy> subunits;
	// map that stores the various RECT structures used to hold the coordinates of sprites to be drawn; the string keys
	// are generally the names of the RECT structures
	map<string,RECT> boundaries;
	// the current time elapsed in the level; used for determining when events occur
	int leveltime;
	// whether the level is finished or not
	bool clear;
	// set of flags used to determine which attack types a boss has already used
	vector<bool> attackTypes;

public:
	Level();
	void init(vector<Enemy>* pEnemies_, size_t level);
	void level1Script();
	void incrementTime();
	void runLevel(int level_);
	int getTime();
	void setRects();
	bool isClear();
	map<string,RECT> getBoundaries();
	void boss1Actions(vector<Bullet>* enemyBullets, size_t i);
	void makeEnemy(float x, float y, float z, RECT bounds, int type, float midX, float midY, float endX, float endY, float life, float speed);
};

#endif