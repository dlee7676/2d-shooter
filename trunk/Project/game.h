/*game.h
The core class of the game.  Controls the use of all other classes related to the game.  
Handles drawing of game objects and game control. */

#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <map>
#include <ctime>
#include <string>
#include "bullet.h"
#include "enemy.h"
#include "level.h"
#include "particleSystem.h"

using namespace std;

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define PI atan(1.0f)*4
#define MAX_BULLETS 1000

class Game {

private:
	int screen, menuSelection;

	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 pDev;
	HWND hwnd;
	LPD3DXSPRITE gameSprites;

	// menu
	LPDIRECT3DTEXTURE9 menuBackgroundTexture;
	LPD3DXFONT font;
	D3DCOLOR fontColor, fontColor2;
	RECT start, quit;

	// levels
	LPDIRECT3DTEXTURE9 gameTexture, laserTexture, explosionTexture, levelBackgroundTexture, bulletTexture, greenLaserTexture, enemyTexture;

	D3DXVECTOR3 bgPos, playerTarget;
	D3DXMATRIX spriteManip, rotation, scaling, translation1, translation2;
	RECT bgTop, bgBottom, levelText, subText1, drawExplosion;

	map<string,RECT> drawBoundaries;
	bool focus, invincible, spellcard1, spellcard2, clear;
	Bullet* playerBullets;
	Bullet* enemyBullets;

	vector<Enemy> enemiesList, subunits;
	ParticleSystem particleHandler;
	GameObject playerObject;
	Level curLevel;
	
	int leveltime, offset, curFrame, curRow, moveRate, fireDirection, hits;
	int curAlpha;
	double currentT;

public:
	void setHwnd(HWND _hwnd);
	void initd3d();
	void gameloop();
	void render();
	void handleInput();
	void cleanup();

	void initMenuScreen();
	void initLevel1();
	void setRects();
	void scrollBackground();
	void sceneryParticles();
	void level1Script();
	void makeEnemy(int x, int y, int z, RECT bounds, int type, int midX, int midY, int endX, int endY, int life, int speed);
	void drawPlayer();
	void drawPlayerBullets();
	void drawEnemyBullets();
	void moveEnemies();
	void drawTitle();
	void drawClearText();
	void drawEnemy();
	void checkEnemyHits(); 
	void advance(int i);
	void waiting(int i);
	void bossPattern(int i, int interval);
	void boss1Actions(int i);
	void resetMatrices();
	void rotateBullets(double angle, int i);
	void refreshEnemies();
	D3DXMATRIX scale(D3DXMATRIX translation1, D3DXMATRIX translation2, int x, int y, D3DXMATRIX scaling, float xFactor, float yFactor);
	D3DXVECTOR3 rotateVector(D3DXVECTOR3 vec, double angle, size_t direction);
};

#endif