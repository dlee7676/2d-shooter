#ifndef GAME_H
#define GAME_H

// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "bullet.h"
#include "enemy.h"

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
	LPDIRECT3DTEXTURE9 levelBackgroundTexture;
	D3DXVECTOR3 playerPos;
	D3DXVECTOR3 moves;
	D3DXVECTOR3 bgPos;
	D3DXMATRIX spriteManip;
	RECT bgTop;
	RECT bgBottom;
	RECT player;
	RECT playerBox;
	RECT laser;
	RECT kaguya;
	RECT bucket;
	RECT fairy;
	bool firing;
	Bullet* playerBullets;
	Bullet* enemyBullets;
	Enemy* enemies;
	int leveltime, offset;

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
	void level1Script();
	void eventType1(int start, int end, int dest, RECT sprite);
};

#endif