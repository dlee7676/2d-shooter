#ifndef LEVEL_H
#define LEVEL_H

//#include "game.h"
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
	vector<Enemy>* pEnemies;
	vector<Enemy> subunits;
	map<string,RECT> boundaries;
	int leveltime;
	bool clear;
	vector<bool> spellcards;

public:
	Level();
	void init(vector<Enemy>* pEnemies_);
	void level1Script();
	void incrementTime();
	int getTime();
	void setRects();
	bool isClear();
	map<string,RECT> getBoundaries();
	void boss1Actions(Bullet* enemyBullets, int i);
	void makeEnemy(int x, int y, int z, RECT bounds, int type, int midX, int midY, int endX, int endY, int life, int speed);
};

#endif