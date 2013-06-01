/*game.h
The core class of the game.  Runs the game loop and controls the use of all other classes related to the game.  
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

class Game {

private:
	// the type of screen being shown. Currently 0 = menu, 1 = gameplay.
	int screen;
	// pointers to the structures used for Direct3D
	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 pDev;
	// stores the window handle generated in WinMain
	HWND hwnd;
	LPD3DXSPRITE gameSprites;
	// pointer to the font structure used for drawing text
	LPD3DXFONT font;
	// color values for text
	D3DCOLOR fontColor, fontColor2;

	/* Used in the menu: */
	// the current selection in the menu
	int menuSelection;
	// the background texture for the menu
	LPDIRECT3DTEXTURE9 menuBackgroundTexture;
	// coordinates for placing the menu options
	RECT start, quit;

	/* Used in the gameplay screen: */
	// pointers to various textures
	LPDIRECT3DTEXTURE9 gameTexture, explosionTexture, levelBackgroundTexture, bulletTexture, enemyTexture;
	// position of the scrolling background
	D3DXVECTOR3 bgPos;
	// used to hold the position of the approximate center of the player sprite (where enemies should aim at)
	D3DXVECTOR3 playerTarget;
	// matrices for scaling and rotating graphics
	D3DXMATRIX spriteManip, rotation, scaling, translation1, translation2;
	// coordinates on the screen to show text fields in
	RECT bgTop, bgBottom, levelText, subText1, topRight;
	// coordinates for where to draw from the explosions sprite sheet; changes to show different frames of animation
	RECT drawExplosion;
	// container for coordinates of sections of the sprite sheet to draw
	map<string,RECT> drawBoundaries;
	// do not handle player hits when this is true
	bool invincible;
	// the game elements
	vector<Bullet> playerBullets, enemyBullets;
	vector<Enemy> enemiesList, subunits;
	ParticleSystem particleHandler;
	GameObject playerObject;
	// handles the level events
	Level curLevel;
	int curAlpha, curFrame, curRow;
	// game data
	int hits, score, level;
	// position of the scrolling background
	float offset;

public:
	void setHwnd(HWND _hwnd);
	void initd3d();
	void gameloop();
	void render();
	void handleInput();
	void updatePositions();
	void cleanup();

	void initMenuScreen();
	void initLevel(size_t level);
	void loadTextures(size_t level);
	void scrollBackground();
	void sceneryParticles();
	void drawPlayer();
	void drawPlayerBullets();
	void movePlayerBullets();
	void drawEnemyBullets();
	void moveEnemyBullets();
	void rotateBullets(float angle, size_t i);
	void drawEnemies();
	void moveEnemies();
	void drawGameText();
	void drawTitle(size_t in, size_t out, size_t end);
	void drawTextAndNumber(LPCWSTR text, int num, RECT pos, D3DCOLOR fontColor);
	void checkEnemyHits(); 
	void advance(size_t i);
	void waiting(size_t i);
	void clearEnemiesList();
	void drawExplosions(GameObject* exploding, float xFactor, float yFactor);

	D3DXMATRIX scale(D3DXMATRIX translation1, D3DXMATRIX translation2, float x, float y, D3DXMATRIX scaling, float xFactor, float yFactor);
	D3DXVECTOR3 rotateVector(D3DXVECTOR3 vec, float angle, size_t direction);
	void resetMatrices();
};

#endif