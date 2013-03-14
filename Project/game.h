#ifndef GAME_H
#define GAME_H

// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <ctime>
#include "bullet.h"
#include "enemy.h"
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

	// menu
	LPD3DXSPRITE menuBackground;
	LPDIRECT3DTEXTURE9 menuBackgroundTexture;
	LPD3DXFONT font;
	D3DCOLOR fontColor, fontColor2;
	D3DXVECTOR3 zero;
	RECT start;
	RECT quit;

	// levels
	D3DXVECTOR3 WTF______;
	double ___TEST;
	D3DXVECTOR3 testTarget;
	LPD3DXSPRITE gameSprites;
	LPDIRECT3DTEXTURE9 gameTexture;
	LPDIRECT3DTEXTURE9 laserTexture;
	LPDIRECT3DTEXTURE9 explosionTexture;
	LPDIRECT3DTEXTURE9 levelBackgroundTexture;
	LPDIRECT3DTEXTURE9 bulletTexture, greenLaserTexture, enemyTexture;
	D3DXVECTOR3 playerPos;
	D3DXVECTOR3 moves;
	D3DXVECTOR3 bgPos;
	D3DXVECTOR3 playerTarget;
	D3DXMATRIX spriteManip;
	D3DXMATRIX rotation, scaling;
	D3DXMATRIX translation1, translation2;
	D3DXVECTOR3 enemyPos;
	D3DXVECTOR3 rotateVector(D3DXVECTOR3 vec, double angle, size_t direction);
	float s;
	RECT bgTop, bgBottom, player, playerBox, laser, greenBullet, purpleBullet, redBall, kaguya, aimedShot, fairy, verticalShot, spreadShot, boss,
		explosionAnim, levelText, descText, greenLaser, smallGreenParticle, bigGreenParticle, topRight, largeGreen, blueBall, yellowStar;
	bool firing, exploding, focus, invincible, chaotic, spiral, spellcard1, spellcard2, clear;
	Bullet* playerBullets;
	Bullet* enemyBullets;
	Enemy* enemies;
	vector<Enemy> subunits;
	vector<Enemy> enemiesList;
	ParticleSystem particleHandler;
	GameObject playerObject;
	int leveltime, offset, curFrame, curRow, explosionTime, moveRate, fireDirection, hits;
	int cooldown;
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
	void drawPlayerBullets();
	void drawEnemyBullets();
	void rotatingFire(Enemy* i, int direction, double angle, int type);
	void fireSpiral(double t, Enemy* enemy_, int size);
	void moveSpiral(int i, float s_, float initS);
	void chaosSpiral(int i, float s_);
	void moveEnemies();
	void drawTitle();
	void drawEnemy(int i);
	void advance(int i);
	void waiting(int i);
	void bossPattern(int i, int interval);
	void resetMatrices();
	void rotateBullets(double angle, int i);
	RECT calcHitbox(RECT bounds);
	D3DXMATRIX Game::scale(D3DXMATRIX translation1, D3DXMATRIX translation2, int x, int y, D3DXMATRIX scaling, float xFactor, float yFactor);
};

#endif