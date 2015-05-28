#include "game.h"

/* void setHwnd(): used to bring the window handle from the main function into the Game object for initializing Direct3D */
void Game::setHwnd(HWND _hwnd) {
	hwnd = _hwnd;
}

/* void initGame(): initializes a Direct3D object */
void Game::initGame() {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	// sets the presentation parameters
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDev);
	
	screen = 0;
	level = 1;
	levelText.top = 250;
	levelText.bottom = 300;
	subText1.top = 310;
	subText1.bottom = 360;
	topRight.top = 5;
	topRight.bottom = 65;
	topRight.left = 550;
	topRight.right = 800;
	initMenuScreen();
}

/* void gameloop(): function run repeatedly in the message loop, executes drawing code and responds to game controls */
void Game::gameloop() {
	render();
	handleInput();
	updatePositions();
}

/* void handleInput(): contains responses to key presses of the game controls */
void Game::handleInput() {
	// Esc returns to the menu screen from gameplay
	if (GetAsyncKeyState(VK_ESCAPE) && screen != 0) {
		screen = 0;
		level = 1;
		levelBackgroundTexture->Release();
		playerBullets.clear();
		enemyBullets.clear();
		enemiesList.clear();
		subunits.clear();
		initMenuScreen();
	}

	// controls for the menu screen
	switch (screen) {
		case 0: {
			// move between options
			if (GetAsyncKeyState(VK_DOWN)) {
				menuSelection++;
				Sleep(150);
			}
			if (GetAsyncKeyState(VK_UP)) {
				menuSelection--;
				Sleep(150);
			}
			// select an option
			if (GetAsyncKeyState(VK_RETURN)) {
				if (menuSelection == 0) {
					screen = 1;
					initLevel(1);
					menuBackgroundTexture->Release();
				}
				if (menuSelection == 1)
					PostQuitMessage(0);
			}
			// loop the menu selections
			if (menuSelection < 0)
				menuSelection = 1;
			if (menuSelection > 1)
				menuSelection = 0;
			break;
		}

		case 1: {
			// direction keys move the player
			if (GetAsyncKeyState(VK_DOWN) && (!playerObject.isExploding()) && playerObject.getPos(1) + playerObject.getSpeed() + 32 < SCREEN_HEIGHT) 
				playerObject.setPos(1, (playerObject.getPos(1) + playerObject.getSpeed()));
			if (GetAsyncKeyState(VK_UP) && (!playerObject.isExploding()) && playerObject.getPos(1) - playerObject.getSpeed() > 0) 
				playerObject.setPos(1, (playerObject.getPos(1) - playerObject.getSpeed()));
			if (GetAsyncKeyState(VK_LEFT) && (!playerObject.isExploding()) && playerObject.getPos(0) - playerObject.getSpeed() > 0) 
				playerObject.setPos(0, (playerObject.getPos(0) - playerObject.getSpeed()));
			if (GetAsyncKeyState(VK_RIGHT) && (!playerObject.isExploding()) && playerObject.getPos(0) + playerObject.getSpeed() + 22 < SCREEN_WIDTH)
				playerObject.setPos(0, (playerObject.getPos(0) + playerObject.getSpeed()));

			// holding Shift causes the player to move at half speed for better precision
			if (GetAsyncKeyState(VK_SHIFT))
				playerObject.setSpeed(3);
			else playerObject.setSpeed(6);

			// spacebar fires a spread of 6 bullets with the middle being more powerful
			if (GetAsyncKeyState(VK_SPACE) && (!playerObject.isExploding()) && curLevel.getTime()%2 == 0) {
				for (int num = 0; num < 6; num++) {
					// initialize bullets in front of the player
					Bullet next;
					playerBullets.push_back(next);
 					playerBullets.back().init(playerObject.getPos(0)-20+num*10, playerObject.getPos(1), 0, drawBoundaries["laser"], num, 1);
				}
				// duplicate the middle bullets to increase the damage dealt by the middle of the spread
				for (int num = 0; num < 4; num++) {
					Bullet next;
					playerBullets.push_back(next);
 					playerBullets.back().init(playerObject.getPos(0)-10+num*10, playerObject.getPos(1), 0, drawBoundaries["laser"], 3, 1);
				}
			}

			// enter goes to the next stage if the level is over
			if (GetAsyncKeyState(VK_RETURN) && curLevel.isClear()) {
				level++;
				initLevel(level);
			}
			break;
		}
	}
}

/* void render(): determines the positions of game objects and draws them on the screen. */
void Game::render() {
	pDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	pDev->BeginScene();
	switch(screen) {
		case 0: {
			// draw menu screen
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			gameSprites->Draw(menuBackgroundTexture, NULL, NULL, &D3DXVECTOR3(0,0,0), 0xFFFFFFFF);
			gameSprites->End();
			if (menuSelection == 0)
				fontColor = D3DCOLOR_ARGB(255,240,100,100); 
			else fontColor = D3DCOLOR_ARGB(255,240,240,240); 
			font->DrawText(NULL, TEXT("Start Game"), -1, &start, 0, fontColor);
			if (menuSelection == 1)
				fontColor = D3DCOLOR_ARGB(255,240,100,100); 
			else fontColor = D3DCOLOR_ARGB(255,240,240,240); 
			font->DrawText(NULL, TEXT("Quit"), -1, &quit, 0, fontColor);
			break;
		}
		case 1: {
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			// draw the game elements
			if (level < 2)
				scrollBackground();	
			drawPlayer();
			sceneryParticles();
			drawPlayerBullets();
			drawEnemyBullets();
			drawEnemies();
			gameSprites->End();
			drawGameText();
			break;
		}
	}
	pDev->EndScene();
	pDev->Present(NULL, NULL, NULL, NULL);
}

/* void updatePositions(): moves the game objects and adds new enemies when needed */
void Game::updatePositions() {
	// add any new enemies
	curLevel.runLevel(level);
	// move enemies
	moveEnemies();
	movePlayerBullets();
	moveEnemyBullets();
	// handle enemy collisions with player bullets
	checkEnemyHits();
	curLevel.incrementTime();
}


/* void cleanup(): frees the memory associated with the game's objects. */
void Game::cleanup() {
	d3d->Release();
	pDev->Release();
	gameSprites->Release();
	font->Release();
}

/* D3DXVECTOR3 rotateVector(D3DXVECTOR3 vec, double angle, size_t direction): applies a rotation matrix to the x and y coordinates of a vector and returns the result.  
	Parameters: 
	D3DXVECTOR3 vec - the vector to be rotated
	double angle - the angle in radians to rotate the vector
	size_t direction - 1 for counterclockwise, 0 for clockwise */

D3DXVECTOR3 Game::rotateVector(D3DXVECTOR3 vec, float angle, size_t direction) {
	if (direction == 1) {
		vec.x = vec.x*cos(angle)-vec.y*sin(angle);
		vec.y = vec.x*sin(angle)+vec.y*cos(angle);
	}
	else {
		vec.x = vec.x*cos(angle)+vec.y*sin(angle);
		vec.y = vec.y*cos(angle)-vec.x*sin(angle);
	}
	vec.z = 0;
	return vec;
}

/* D3DXMATRIX scale(D3DXMATRIX translation1, D3DXMATRIX translation2, int x, int y, D3DXMATRIX scaling, float xFactor, float yFactor):
Returns a matrix that will apply a scaling transformation that maintains the scaled object's position by translating it to the origin while scaling.
	Parameters:
	D3DMATRIX translation1, translation2: translation matrices for moving the object to the origin and back.
	int x, y: x and y coordinates of the object to be scaled.
	D3DMATRIX scaling: the matrix that contains the scaling transformation.
	float xFactor, yFactor: the factors to scale the x and y coordinates by. */

D3DXMATRIX Game::scale(D3DXMATRIX translation1, D3DXMATRIX translation2, float x, float y, D3DXMATRIX scaling, float xFactor, float yFactor) {
	D3DXMATRIX resultMatrix;
	// do a translation to the origin
	D3DXMatrixTranslation(&translation1, -1*x, -1*y, 0);
	// apply the scaling matrix
	D3DXMatrixScaling(&scaling, xFactor, yFactor, 1);
	// translate back to the original position
	D3DXMatrixTranslation(&translation2, x, y, 0);
	D3DXMatrixMultiply(&resultMatrix, &translation1, &scaling);
	D3DXMatrixMultiply(&resultMatrix, &resultMatrix, &translation2);
	return resultMatrix;
}

/* void rotateBullets(double angle, size_t i): Sets up a rotation matrix that will turn a bullet to face its target.
	Parameters: 
	double angle: the angle in radians to rotate the bullet.
	size_t i: The index of the bullet to be rotated. */

void Game::rotateBullets(float angle, size_t i) {
	D3DXMatrixTranslation(&translation1,-1*enemyBullets[i].getPos(0),-1*enemyBullets[i].getPos(1),0);
	if (enemyBullets[i].getStartPos().x < enemyBullets[i].getTarget().x)
		D3DXMatrixRotationZ(&rotation, angle + PI/4);
	else D3DXMatrixRotationZ(&rotation, angle - PI/4);
	D3DXMatrixTranslation(&translation2,enemyBullets[i].getPos(0),enemyBullets[i].getPos(1),0);
	D3DXMatrixMultiply(&spriteManip, &translation1, &scaling);
	D3DXMatrixMultiply(&spriteManip, &spriteManip, &rotation);
	D3DXMatrixMultiply(&spriteManip, &spriteManip, &translation2);
}


/* void resetMatrices(): returns all transformation matrices to identity matrices. */
void Game::resetMatrices() {
	D3DXMatrixIdentity(&spriteManip);
	D3DXMatrixIdentity(&rotation);
	D3DXMatrixIdentity(&scaling);
	D3DXMatrixIdentity(&translation1);
	D3DXMatrixIdentity(&translation2);
}

/* void drawGameText(): draws text displays in gameplay. */
void Game::drawGameText() {
	drawTitle(60,160,240); 
	drawTextAndNumber(TEXT("Score: "), score, topRight, fontColor2);
	if (curLevel.isClear()) {
		font->DrawText(NULL, TEXT("Stage Clear!"), -1, &levelText, 0, fontColor);
		drawTextAndNumber(TEXT("Hits taken: "), hits, subText1, fontColor);
	}
}

/* void drawTitle(): causes the stage title to fade in and out,
	Parameters:
	size_t in: the time value to stop fading in text at
	size_t out: the time value to begin fading out text
	size_t end: the time value to finish fading out */

void Game::drawTitle(size_t in, size_t out, size_t end) {
	// show title at the beginning of the stage
	if (curLevel.getTime() >= 0 && curLevel.getTime() < 240) {
		levelText.left = SCREEN_WIDTH/2-75;
		levelText.right = SCREEN_WIDTH/2+200;
		subText1.left = SCREEN_WIDTH/2-75;
		subText1.right = SCREEN_WIDTH/2+200;
		// fade in
		for (size_t i = 0; i < in; i++) {
			if (curLevel.getTime() == i)  {
				curAlpha += 3;
				fontColor = D3DCOLOR_ARGB(curAlpha,200,200,255); 
			}
		}
		// fade out after some time
		for (size_t i = out; i < end; i++) {
			if (curLevel.getTime() == i && curAlpha-3 >= 0)  {
				curAlpha -= 3;
				fontColor = D3DCOLOR_ARGB(curAlpha,200,200,255); 
			}
		}
		drawTextAndNumber(TEXT("Stage "), level, levelText, fontColor);
		if (level == 2)
			font->DrawText(NULL, TEXT("In progress"), -1, &subText1, 0, fontColor);
	}
	else fontColor = fontColor = D3DCOLOR_ARGB(255,200,200,255);
}

/* void drawTextAndNumber(LPCWSTR text, int num, RECT pos, D3DCOLOR fontColor): draws text with a variable number after it.
	Parameters:
	LPCWSTR text: the text before the number.
	int num: the number value.
	RECT pos: where to draw the text.
	D3DCOLOR fontColor: the colour of the text. */

void Game::drawTextAndNumber(LPCWSTR text, int num, RECT pos, D3DCOLOR fontColor) {
	wchar_t buffer[16];
	wsprintf(buffer, TEXT("%d"), num);
	wstring s(text);
	s += wstring(buffer); 
	font->DrawText(NULL, s.c_str(), -1, &pos, 0, fontColor);
}

/* void initMenuScreen(): initializes values relevant to the menu screen. */
void Game::initMenuScreen() {
	// screen 0
	if (FAILED(D3DXCreateSprite(pDev, &gameSprites))) {
		MessageBox(hwnd, TEXT("Error Loading Sprite"), TEXT("Error"), MB_ICONERROR);
		return;
	} 
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("menu.jpg"), &menuBackgroundTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	D3DXCreateFont(pDev, 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Edwardian Script ITC"), &font); 
	// set values as they should be at the start of the game
	start.left=SCREEN_WIDTH*3/5;
	start.right=SCREEN_WIDTH*9/10;
	start.top=SCREEN_HEIGHT*3/5;
	start.bottom=start.top+45;
	quit.left=start.left;
	quit.right=start.right;
	quit.top=start.bottom+30;
	quit.bottom=quit.top+45;
	menuSelection = 0;
	score = 0;
}

/* void initLevel(size_t level_): sets values as they should be at the start of each level.
	Parameters:
	size_t level: the stage to initialize. */

void Game::initLevel(size_t level_) {
	loadTextures(level_);
	D3DXCreateFont(pDev, 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Franklin Gothic Demi"), &font); 
	// seed a random number for various events
	srand((unsigned int)(time(0)));
	offset = 0;
	fontColor2 = D3DCOLOR_ARGB(255,240,240,240);
	curFrame = 0;
	curRow = 0;
	curAlpha = 50;
	hits = 0;
	playerObject.init(SCREEN_WIDTH/2, SCREEN_HEIGHT*8/10, 0, drawBoundaries["playerBox"], 0, 6);
	curLevel.init(&enemiesList, level_);
	drawBoundaries = curLevel.getBoundaries();
}

/* void loadTextures(size_t level_): loads the textures needed for a level. 
	Parameters:
	size_t level: the current stage. */

void Game::loadTextures(size_t level_) {
	switch(level_) {
		case 1: {
			if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("enemySprites.png"), &enemyTexture))) {
				MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
				return;
			}
			if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("explosionSpriteSheet.png"), &explosionTexture))) {
				MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
				return;
			}
			if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("bulletSprites.png"), &bulletTexture))) {
				MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
				return;
			}
			if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("forest.png"), &levelBackgroundTexture))) {
				MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
				return;
			}
			break;
		}
		case 2: {
			if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("menu.jpg"), &levelBackgroundTexture))) {
				MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
				return;
			}
		}
	}
}

/* void scrollBackground(): displays a scrolling background image. */
void Game::scrollBackground() {
	offset++;
	if (offset >= 1000)
		offset = 0;
	D3DXVECTOR3 bgPos(0,0,0);
	bgTop.left = 0;
	bgTop.right = SCREEN_WIDTH;
	bgTop.top = (LONG)(1000 - offset);
	bgTop.bottom = 1000;
	gameSprites->Draw(levelBackgroundTexture, &bgTop, NULL, &bgPos, 0xFFFFFFFF);
	bgBottom.left = 0;
	bgBottom.right = SCREEN_WIDTH;
	bgBottom.top = 0;
	bgBottom.bottom = (LONG)(SCREEN_HEIGHT - offset);
	bgPos.y = offset;
	gameSprites->Draw(levelBackgroundTexture, &bgBottom, NULL, &bgPos, 0xFFFFFFFF);
}

/* void sceneryParticles(): causes scenery particles to appear, move, and disappear. */
void Game::sceneryParticles() {
	// every 50 time units, there is a 50% chance of emitting a particle
	if (curLevel.getTime()%50 == 0) {
		for (int i = 0; i < 10; i++) {
			if (rand()%2 == 0)
				// randomize the position, size, lifespan, speed, and heading of the particle
				particleHandler.emit(D3DXVECTOR3((float)(rand()%800), (float)(rand()%600), 0), rand()%1, (float)(rand()%200+150), (float)(rand()%2+1)*0.75f, 
					D3DXVECTOR3((float)(rand()%2-1)*0.3f, (float)(rand()%2+2)*0.3f, 0));
		}
	}
	vector<Particle> display = particleHandler.getParticles();
	// make particles fade in and out according to their lifespan
	for (size_t i = 0; i < display.size(); i++) {
		if (display[i].isActive()) {
			D3DCOLOR currentColor;
			if (display[i].getTime() < display[i].getLifespan()) {
				currentColor = D3DCOLOR_ARGB(display[i].getTime()*4, 255, 255, 255);
				if (display[i].getTime()*4 > 255)
					currentColor = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
			else {
				currentColor = D3DCOLOR_ARGB((int)(display[i].getLifespan())*4, 255, 255, 255);
				if (display[i].getLifespan()*4 > 255)
					currentColor = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
			if (display[i].getType() == 0)
				gameSprites->Draw(enemyTexture, &drawBoundaries["smallGreenParticle"], NULL, &display[i].getPos(), currentColor);
			else gameSprites->Draw(enemyTexture, &drawBoundaries["bigGreenParticle"], NULL, &display[i].getPos(), currentColor);
		}
	}
	particleHandler.update();
}

/* void drawPlayer(): draws the player's sprite.  Draws an explosion if hit. */
void Game::drawPlayer() {
	// draw the explosion animation
	if (playerObject.isExploding()) {
		// set a RECT to the appropriate part of the explosion sprite sheet (a 4x4 grid)
		drawExplosions(&playerObject, 1, 1);
		// change the image after a given interval
		if (playerObject.getAnimTime() <= 0) {
			curFrame++;
			playerObject.setAnimTime(6);
		}
		// stop the explosion animation and make the player invincible after all the frames are shown
		if (curFrame > 4 && curRow > 4) {
			playerObject.setExploding(false);
			curFrame = 0;
			curRow = 0;
			playerObject.setAnimTime(100);
			invincible = true;
		}
		// move to the next row of images after 4 are shown
		else if (curFrame > 4) {
			curFrame = 0;
			if (curRow < 4)
				curRow = 4;
			else curRow++;
		}
	}
	// draws the player sprite and causes it to flicker when invincible after being hit
	else {
		if ((invincible && curLevel.getTime()%2 == 0) || !invincible) 
			gameSprites->Draw(enemyTexture, &drawBoundaries["player"], NULL, &playerObject.getPos(), 0xFFFFFFFF);
	}
	// makes the player vulnerable again a short time after being hit
	if (playerObject.getAnimTime() > 5) {
		playerObject.setAnimTime(playerObject.getAnimTime()-1);
		if (playerObject.getAnimTime() <= 5)
			invincible = false;
	}
}

/* void drawPlayerBullets(): draws the sprites for bullets fired by the player. */
void Game::drawPlayerBullets() {
	for (size_t i = 0; i < playerBullets.size(); i++) {
		if (playerBullets[i].isActive()) {
			// draw an explosion if a player bullet hits an enemy
			if (playerBullets[i].isExploding()) {
				drawExplosions(&playerBullets[i], 0.5, 0.5);
				if (playerBullets[i].getAnimTime() <= 0) {
					playerBullets[i].setExploding(false);
					playerBullets[i].setActive(false);
				}
			}
			else {
				spriteManip = scale(translation1, translation2, playerBullets[i].getPos(0), playerBullets[i].getPos(1), scaling, 0.7f, 0.9f);
				gameSprites->SetTransform(&spriteManip);
				gameSprites->Draw(bulletTexture, &drawBoundaries["laser"], NULL, &playerBullets[i].getPos(), 0xB1FFFFFF);
			}
			resetMatrices();
			gameSprites->SetTransform(&spriteManip);
			// set bullets to inactive when they pass the top of the screen
			if (playerBullets[i].getPos(1) < 0) {
 				playerBullets[i].setActive(false);
			}
		}
	}
}

/* void movePlayerBullets(): moves the player's bullets toward their destinations. Removes inactive bullets. */
void Game::movePlayerBullets() {
	for (size_t i = 0; i < playerBullets.size(); i++) {
		if (playerBullets[i].isActive() && !playerBullets[i].isExploding()) {
			// move the bullets at a narrower angle if the player is focused
			if (playerObject.getSpeed() == 3) {
				if (playerBullets[i].getType() < 2)
					playerBullets[i].move(-1,-15,0); 
				else if (playerBullets[i].getType() < 4)
					playerBullets[i].move(0,-15,0); 
				else if (playerBullets[i].getType() < 6)
					playerBullets[i].move(1,-15,0); 
			}
			else {
				if (playerBullets[i].getType() < 2)
					playerBullets[i].move(-2,-15,0); 
				else if (playerBullets[i].getType() < 4)
					playerBullets[i].move(0,-15,0); 
				else if (playerBullets[i].getType() < 6)
					playerBullets[i].move(2,-15,0);
			}
		}
		// remove the first member of the vector if it is inactive to keep the size low
		if (!playerBullets[0].isActive())
  			playerBullets.erase(playerBullets.begin());
	}
}

/* void drawEnemyBullets(): draws the sprites for enemy bullets. */
void Game::drawEnemyBullets() {
	D3DXVECTOR3 moveRate;
	float angle; 
	for (size_t i = 0; i < enemyBullets.size(); i++) {
		if (enemyBullets[i].isActive()) {
			// handle enemy bullet collisions with the player; trigger player explosion animation
			if (enemyBullets[i].inBounds(playerObject, 11, 13) && (!playerObject.isExploding()) && !invincible) {
				playerObject.setExploding(true);
				hits++;
				enemyBullets[i].setPos(-100,-100,-100);
			}
			if (curLevel.isClear())
				enemyBullets[i].setActive(false);
			// rotate bullet sprites to face toward the position they are aimed at
			moveRate = enemyBullets[i].getTarget() - enemyBullets[i].getStartPos();
			D3DXVec3Normalize(&moveRate, &moveRate);
			angle = atan(moveRate.y/moveRate.x);
			if (enemyBullets[i].getType() == 0)
			rotateBullets(angle, i);
			gameSprites->SetTransform(&spriteManip);
			// draw bullets 
			gameSprites->Draw(bulletTexture, &enemyBullets[i].getDrawingBounds(), NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
			resetMatrices();
			gameSprites->SetTransform(&spriteManip);
		}
	}
}

/* void moveEnemyBullets(): moves enemy bullets in the directions they are heading. */
void Game::moveEnemyBullets() {
	D3DXVECTOR3 moveRate;
	for (size_t i=0; i < enemyBullets.size(); i++) {
		// calculate a vector pointing from the bullet to the target and move along it
		moveRate = enemyBullets[i].getTarget() - enemyBullets[i].getStartPos();
		D3DXVec3Normalize(&moveRate, &moveRate);
		enemyBullets[i].move(0.75*moveRate.x*enemyBullets[i].getSpeed(), 0.75*moveRate.y*enemyBullets[i].getSpeed(), 0); 
		// apply special movement types to certain bullet types
		if (enemyBullets[i].getType() == 3)
			enemyBullets[i].randomSpiral(0.5f, 0.01f, 10.0f);
		if (enemyBullets[i].getType() == 2)
			enemyBullets[i].moveSpiral(i%2, 0.1f, 0.05f, 20.0f);
		if (enemyBullets[i].getPos(1) > 620 || enemyBullets[i].getPos(1) < 0 || enemyBullets[i].getPos(0) < 0 || enemyBullets[i].getPos(0) > SCREEN_WIDTH) {
			enemyBullets[i].setActive(false);
		}
		if (!enemyBullets[0].isActive())
			enemyBullets.erase(enemyBullets.begin());
	}
}

/* void drawExplosions(GameObject* exploding, float xFactor, float yFactor): Draws explosion graphics.
	Parameters:
	GameObject* exploding: pointer to the game object that is exploding.
	float xFactor, yFactor: scaling factors for the explosion image. */

void Game::drawExplosions(GameObject* exploding, float xFactor, float yFactor) {
	// select the appropriate area of the explosion sprite sheet depending on the animation frame to be shown
	drawExplosion.left = curFrame*100;
	drawExplosion.top = curRow*100+5*curRow;
	drawExplosion.right = drawExplosion.left+100;
	drawExplosion.bottom = drawExplosion.top+100-5*curRow;
	// scale the explosion image by the given factors and draw it
	spriteManip = scale(translation1, translation2, exploding->getPos(0), exploding->getPos(1), scaling, xFactor, yFactor);
	gameSprites->SetTransform(&spriteManip);
	D3DXVECTOR3 drawPos = D3DXVECTOR3(exploding->getPos(0)-5, exploding->getPos(1)-5, 0);
	gameSprites->Draw(explosionTexture, &drawExplosion, NULL, &drawPos, 0xFFFFFFFF);
	// count down the animation time of the object (changes frames when the time is 0)
	exploding->setAnimTime(exploding->getAnimTime() - 1);
}

/* void drawEnemies(): draws the enemy sprites. */
void Game::drawEnemies() {
	for (size_t i = 0; i < enemiesList.size(); i++) {
		if (enemiesList[i].isActive()) {
			// remove enemies when the level is cleared
			if (curLevel.isClear())
				enemiesList[i].setActive(false);
			// show explosions for dead enemies
			if (enemiesList[i].isExploding()) {
				drawExplosions(&enemiesList[i], 1, 1);
				if (enemiesList[i].getAnimTime() <= 0) {
					enemiesList[i].setActive(false);
					enemiesList[i].setExploding(false);
				}
			}
			else {
				// scale the boss sprite to a larger size
				if (enemiesList[i].getType() == -1) {
					spriteManip = scale(translation1, translation2, enemiesList[i].getPos(0), enemiesList[i].getPos(1), scaling, 1.3f, 1.3f);
					gameSprites->SetTransform(&spriteManip);
				}
				gameSprites->Draw(enemyTexture, &enemiesList[i].getDrawingBounds(), NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
			}
			resetMatrices();
			gameSprites->SetTransform(&spriteManip);
		}
	}
}

/* void checkEnemyHits(): handles collisions between enemies and player bullets. */
void Game::checkEnemyHits() {
	for (size_t i = 0; i < enemiesList.size(); i++) {
		if (enemiesList[i].getPos(1) >= 0) { 
			for (size_t j = 0; j < playerBullets.size(); j++) {
				// if an enemy is hit by a player bullet, destroy it if its life total is 0 and add to score
				if (enemiesList[i].inBounds(playerBullets[j], 0, 0) && playerBullets[j].isActive() && !playerBullets[j].isExploding() 
					&& enemiesList[i].getPos(1) > 0 && !enemiesList[i].isExploding()) {
					if (enemiesList[i].getLife() <= 0) {
						enemiesList[i].setExploding(true);
						score += abs(enemiesList[i].getType()*1000);
					}
					// otherwise decrease the enemy's life total and increment the score
					else {
						enemiesList[i].setLife(enemiesList[i].getLife()-2);
						score += 1;
					}
					playerBullets[j].setExploding(true);
				}
			}
		}
	}
}

/* void moveEnemies(): moves the enemies toward their destinations. */
void Game::moveEnemies() {
	playerTarget = D3DXVECTOR3(playerObject.getPos(0)+14, playerObject.getPos(1)+21, 0);
	//D3DXVECTOR3 shot = playerObject.getPos();
	for (size_t i = 0; i < enemiesList.size(); i++) {
		int action = enemiesList[i].getAction();
		if (enemiesList[i].isActive()) {
			// move enemies according to their action variable
			switch(action) {
				case 0: {
					advance(i);
					break;
				}
				case 1: {
					waiting(i);
					break;
				}
				case 2: {
					// exiting the screen
					if (enemiesList[i].getCooldown() <= 0) {
						enemiesList[i].setCooldown(30);	
					}
					else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
					enemiesList[i].moveTo(0, 0.001f);
					// set the enemy to be inactive when it leaves the screen
					if (enemiesList[i].getPos(0) < -10 || enemiesList[i].getPos(1) < -10 || enemiesList[i].getPos(0) > SCREEN_WIDTH || enemiesList[i].getPos(1) > SCREEN_HEIGHT) {
   						enemiesList[i].setActive(false);
					}
					break;
				}
				case 3: {
					// actions used for bosses
					enemiesList[i].setSpeed(2);
					enemiesList[i].bossPattern(100, curLevel.getTime());
					curLevel.boss1Actions(&enemyBullets, i);
				}
			}
		}
		else 
			if (!enemiesList[0].isActive())
				enemiesList.erase(enemiesList.begin());
	}
}

/* void advance(size_t i): handle actions taken by certain enemy types while moving. 
	Parameters:
	size_t i: the index of the enemy to handle. */

void Game::advance(size_t i) {
	playerTarget = D3DXVECTOR3(playerObject.getPos(0)+14, playerObject.getPos(1)+21, 0);
	D3DXVECTOR3 shot = playerObject.getPos();
	// move the enemy along a given spline
	enemiesList[i].moveTo(1, 0.001f);
	// fire bullets according to the type of enemy when the cooldown timer is 0
	if (enemiesList[i].getCooldown() <= 0) {
		if (enemiesList[i].getType() == 0)
			enemiesList[i].fire(&enemyBullets, playerTarget, enemiesList[i].getPos(), enemyBullets.size(), drawBoundaries["greenBullet"], 0, 3);
		if (enemiesList[i].getType() == 1)
			enemiesList[i].fire(&enemyBullets, playerTarget, enemiesList[i].getPos(), enemyBullets.size(), drawBoundaries["purpleBullet"], 0, 3);
		// fire a spread of bullets
		if (enemiesList[i].getType() == 3) {
			for (int j = 0; j < 5; j++) {
				enemiesList[i].fire(&enemyBullets, shot, enemiesList[i].getPos(), enemyBullets.size(), drawBoundaries["purpleBullet"], 0, 3);
				shot = rotateVector(shot, PI/6, 1);
				enemiesList[i].setCooldown(50);
			}
		}
		// fire bullets upward and downward
		if (enemiesList[i].getType() == 4) {
			if (enemiesList[i].getCooldown() <= 0) {
				for (int j = 0; j < 2; j++) {
					shot.x = 0;
					if (j == 0)
						shot.y = -1;
					if (j == 1)
						shot.y = 1;
					enemiesList[i].fire(&enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1)+10, shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1)+10, 0), enemyBullets.size(), drawBoundaries["redBall"], 1, 3);
					enemiesList[i].setCooldown(10);
				}
			}
			//else enemiesList[i].setCooldown(enemiesList[i].getCooldown()-1);
		}
		if (enemiesList[i].getType() < 3)
			enemiesList[i].setCooldown(20);
	}
	else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
	// change actions when the enemy approaches the middle control point of its spline
	if (enemiesList[i].getS() >= 0.95)
		if (enemiesList[i].getType() == -1)
			enemiesList[i].setAction(3);
	else enemiesList[i].setAction(1);
}

/* void waiting(size_t i): handle actions taken by certain enemy types when they stop moving. 
	Parameters:
	size_t i: the index of the enemy to handle. */

void Game::waiting(size_t i) {
	D3DXVECTOR3 shot = playerObject.getPos();
	shot.z = 0;
	// use different types of firing patterns while stationary, depending on enemy type
	if (enemiesList[i].getType() == 1) {
		if (enemiesList[i].getCooldown() <= 0) {
			for (int j = 0; j < 8; j++) {
				shot = enemiesList[i].aim8Ways(j);
				enemiesList[i].fire(&enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1)+10, shot.z), 
					D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1)+10, 0), enemyBullets.size(), drawBoundaries["redBall"], 1, 3);
				enemiesList[i].setCooldown(30);
			}
		}
		else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
		// the enemy will wait until its wait timer runs out and then change actions again
		enemiesList[i].wait();
		if (enemiesList[i].getWaitTime() >= 200)
			enemiesList[i].setAction(2);
	}
	// fire 3 ways
	else if (enemiesList[i].getType() == 2) {
		if (enemiesList[i].getCooldown() <= 0) {
			for (int j = 0; j < 3; j++) {	
				shot = enemiesList[i].aim3Ways(j);
				enemiesList[i].fire(&enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1), shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1), 0), enemyBullets.size(), drawBoundaries["largeGreen"], 1, 2);
			}
			enemiesList[i].setCooldown(50);	
		}
		else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);		
		enemiesList[i].wait();
		if (enemiesList[i].getWaitTime() >= 300)
			enemiesList[i].setAction(2);
	}
	else enemiesList[i].setAction(2);
}
