#ifndef GAME_H
#define GAME_H

// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include "bullet.h"
#include "enemy.h"

using namespace std;

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define PI atan(1.0f)*4

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
	D3DCOLOR fontColor;
	D3DXVECTOR3 zero;
	RECT start;
	RECT quit;

	// levels
	LPD3DXSPRITE gameSprites;
	LPDIRECT3DTEXTURE9 gameTexture;
	LPDIRECT3DTEXTURE9 laserTexture;
	LPDIRECT3DTEXTURE9 explosionTexture;
	LPDIRECT3DTEXTURE9 levelBackgroundTexture;
	LPDIRECT3DTEXTURE9 bulletTexture, greenLaserTexture, enemyTexture;
	D3DXVECTOR3 playerPos;
	D3DXVECTOR3 moves;
	D3DXVECTOR3 bgPos;
	D3DXMATRIX spriteManip;
	D3DXMATRIX rotation, scaling;
	D3DXMATRIX translation1, translation2;
	D3DXVECTOR3 enemyPos;
	D3DXVECTOR3 rotateVector(D3DXVECTOR3 vec, double angle, size_t direction);
	float s;
	RECT bgTop, bgBottom, player, playerBox, laser, greenBullet, purpleBullet, redBall, kaguya, aimedShot, fairy, spreadShot, explosionAnim, levelText, descText;
	bool firing, exploding, focus;
	Bullet* playerBullets;
	Bullet* enemyBullets;
	Enemy* enemies;
	vector<Enemy> enemiesList;
	GameObject playerObject;
	int leveltime, offset, curFrame, curRow, explosionTime, moveRate;
	int cooldown;

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
	void level1Script();
	void makeEnemy(int x, int y, int z, RECT bounds, int type, int midX, int midY, int endX, int endY, int life, int speed);
	void drawPlayerBullets();
	void drawEnemyBullets();
	void moveEnemies();
	void drawTitle();
	RECT calcHitbox(RECT bounds);
};

#endif