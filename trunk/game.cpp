#include "game.h"

void Game::setHwnd(HWND _hwnd) {
	hwnd = _hwnd;
}

/* d3d initialization */

void Game::initd3d() {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;

	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDev);
	screen = 0;
	initMenuScreen();
}

void Game::gameloop() {
	render();
	handleInput();
}

/* controls */

void Game::handleInput() {
	switch (screen) {
		case 0: {
			if (GetAsyncKeyState(VK_DOWN)) {
				menuSelection++;
				Sleep(150);
			}
			if (GetAsyncKeyState(VK_UP)) {
				menuSelection--;
				Sleep(150);
			}
			if (GetAsyncKeyState(VK_RETURN)) {
				if (menuSelection == 0) {
					screen = 1;
					initLevel1();
				}
				if (menuSelection == 1)
					PostQuitMessage(0);
			}
			if (menuSelection < 0)
				menuSelection = 1;
			if (menuSelection > 1)
				menuSelection = 0;
			break;
		}

		case 1: {
			if (GetAsyncKeyState(VK_SHIFT))
				focus = true;
			else focus = false;
			if (focus)
				moveRate = 3;
			else moveRate = 6;
			if (GetAsyncKeyState(VK_DOWN) && !exploding) 
				playerPos.y += moveRate;
			if (GetAsyncKeyState(VK_UP) && !exploding) 
				playerPos.y -= moveRate;
			if (GetAsyncKeyState(VK_LEFT) && !exploding) 
				playerPos.x -= moveRate;
			if (GetAsyncKeyState(VK_RIGHT) && !exploding)
				playerPos.x += moveRate;
			/*if (GetAsyncKeyState('Z')) 
				playerPos.z -= 5;
			if (GetAsyncKeyState('A'))
				playerPos.z += 5;*/
			if (GetAsyncKeyState(VK_SPACE) && !exploding) {
				for (int i = 0; i < 100; i++) {
					if (!playerBullets[i].isActive()) {
						playerBullets[i].setActive(true);
						playerBullets[i].init(playerPos.x, playerPos.y, playerPos.z, laser);
						break;
					}
				}
			}
			if (GetAsyncKeyState(VK_ESCAPE)) {
				screen = 0;
				delete playerBullets;
				delete enemyBullets;
			}
			break;
		}
	}
}

void Game::render() {
	pDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	pDev->BeginScene();
	switch(screen) {
		case 0: {
			// draw menu screen
			menuBackground->Begin(D3DXSPRITE_ALPHABLEND);
			menuBackground->Draw(menuBackgroundTexture, NULL, NULL, &zero, 0xFFFFFFFF);
			menuBackground->End();
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

			// draw background
			scrollBackground();

			// draw player
			if (exploding) {
				explosionAnim.left = curFrame * 64;
				explosionAnim.top = curRow * 64;
				explosionAnim.right = explosionAnim.left + 64;
				explosionAnim.bottom = explosionAnim.top + 64;
				gameSprites->Draw(explosionTexture, &explosionAnim, NULL, &playerPos, 0xFFFFFFFF);
				explosionTime--;
				if (explosionTime <= 0) {
					curFrame++;
					explosionTime = 3;
				}
				if (curFrame > 4 && curRow > 4) {
					playerPos.x = SCREEN_WIDTH/2; 
					playerPos.y = SCREEN_HEIGHT*8/10;
					exploding = false;
					curFrame = 0;
					curRow = 0;
				}
				else if (curFrame > 4) {
					curFrame = 0;
					curRow++;
				}
			}
			else {
				gameSprites->Draw(gameTexture, &player, NULL, &playerPos, 0xFFFFFFFF);
				gameSprites->Draw(bulletTexture, &laser, NULL, &D3DXVECTOR3(playerBox.left + playerPos.x + 10, playerBox.top + playerPos.y + 10, 0), 0xFFFFFFFF);
			}

			// draw player bullets
			for (int i = 0; i < 100; i++) {
				if (playerBullets[i].isActive()) {
					if (playerBullets[i].isExploding()) {
						explosionAnim.left = 0;
						explosionAnim.top = 0;
						explosionAnim.right = 80;
						explosionAnim.bottom = 80;
						gameSprites->Draw(explosionTexture, &explosionAnim, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
						playerBullets[i].setAnimTime(playerBullets[i].getAnimTime() - 1);
						if (playerBullets[i].getAnimTime() <= 0) {
							playerBullets[i].setActive(false);
							playerBullets[i].setExploding(false);
							playerBullets[i].setPos(-100,-100,-100);
						}
					}
					else {
						gameSprites->Draw(bulletTexture, &laser, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
						playerBullets[i].move(0,-10,0); 
					}
					if (playerBullets[i].getPos(1) < 0)
						playerBullets[i].setActive(false);
				}
			}

			// draw enemy bullets
			drawEnemyBullets();
			// move enemies
			level1Script();
			gameSprites->End();
			leveltime++;
			break;
		}
	}
	pDev->EndScene();
	pDev->Present(NULL, NULL, NULL, NULL);
}

void Game::cleanup() {
	d3d->Release();
	pDev->Release();
	menuBackground->Release();
	menuBackgroundTexture->Release();
}

/*------------------*/

void Game::initMenuScreen() {
	// screen 0
	if (FAILED(D3DXCreateSprite(pDev, &menuBackground))) {
		MessageBox(hwnd, TEXT("Error Loading Sprite"), TEXT("Error"), MB_ICONERROR);
		return;
	} 
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("prismriver.jpg"), &menuBackgroundTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	D3DXCreateFont(pDev, 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Edwardian Script ITC"), &font ); 
	start.left=SCREEN_WIDTH*3/5;
	start.right=SCREEN_WIDTH*9/10;
	start.top=SCREEN_HEIGHT*3/5;
	start.bottom=start.top+45;
	quit.left=start.left;
	quit.right=start.right;
	quit.top=start.bottom+30;
	quit.bottom=quit.top+45;
	menuSelection = 0;
	zero.x = 0; zero.y = 0; zero.z = 0;
}

void Game::initLevel1() {
	leveltime = 0;
	if (FAILED(D3DXCreateSprite(pDev, &gameSprites))) {
		MessageBox(hwnd, TEXT("Error Loading Sprite"), TEXT("Error"), MB_ICONERROR);
		return;
	} 
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("ships.png"), &gameTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("laser.jpg"), &laserTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("forest.png"), &levelBackgroundTexture))) {
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
	playerPos.x = SCREEN_WIDTH/2; playerPos.y = SCREEN_HEIGHT*8/10; playerPos.z = 0;
	setRects();
	offset = 0;
	exploding = false;
	explosionTime = 3;
	curFrame = 0;
	curRow = 0;
	playerBullets = new Bullet[100];
	enemyBullets = new Bullet[1000];
	enemies = new Enemy[100];
}

void Game::setRects() {
	player.left=0;
	player.right=28;
	player.top=0;
	player.bottom=43;
	playerBox.left = 0;
	playerBox.right=player.right/2;
	playerBox.top = 0;
	playerBox.bottom=player.left/2;
	laser.left=127;
	laser.right=143;
	laser.top=0;
	laser.bottom=16;
	greenBullet.left = 159;
	greenBullet.right = 175;
	greenBullet.top = 22;
	greenBullet.bottom = 40;
	kaguya.left=108;
	kaguya.right=140;
	kaguya.top=5;
	kaguya.bottom=41;
	bucket.left=267; 
	bucket.top=8;
	bucket.right=294; 
	bucket.bottom=44;
	fairy.left=149; 
	fairy.top=2;
	fairy.right=201; 
	fairy.bottom=54;
}

void Game::scrollBackground() {
	offset++;
	if (offset >= 1000)
		offset = 0;
	D3DXVECTOR3 bgPos(0,0,0);
	bgTop.left = 0;
	bgTop.right = SCREEN_WIDTH;
	bgTop.top = 1000 - offset;
	bgTop.bottom = 1000;
	gameSprites->Draw(levelBackgroundTexture, &bgTop, NULL, &bgPos, 0xFFFFFFFF);
	bgBottom.left = 0;
	bgBottom.right = SCREEN_WIDTH;
	bgBottom.top = 0;
	bgBottom.bottom = SCREEN_HEIGHT - offset;
	bgPos.y = offset;
	gameSprites->Draw(levelBackgroundTexture, &bgBottom, NULL, &bgPos, 0xFFFFFFFF);
}

void Game::level1Script() {
// events
	if (leveltime == 150) {
		enemies[0].init(50,-20, 0, kaguya);
		enemies[1].init(100, -22, 0, kaguya);
		enemies[2].init(125, -30, 0, kaguya);
	}
	if (leveltime > 150 && leveltime < 800) {
		moveEnemies(0, 3, 100, 300, 101, 0, kaguya);
	}
	if (leveltime == 300) {
		enemies[3].init(650,-40, 0, bucket);
		enemies[4].init(670, -35, 0, bucket);
		enemies[5].init(700, -25, 0, bucket);
		enemies[6].init(725, -20, 0, bucket);
	}
	if (leveltime > 300) {
		moveEnemies(3, 7, 670, 225, 800, 300, bucket);
	}
	if (leveltime == 350) {
		enemies[7].init(200,-40, 0, bucket);
		enemies[8].init(300, -35, 0, bucket);
		enemies[9].init(400, -25, 0, bucket);
		enemies[10].init(500, -20, 0, bucket);
	}
	if (leveltime > 350) {
		moveEnemies(7, 11, 300, 400, -30, 400, bucket);
	}
	if (leveltime == 800) {
		enemies[1].init(350, -30, 0, fairy);
	}
	if (leveltime > 800) {
		moveEnemies(1, 2, 350, 100, 0, 0, fairy);
	}
}

void Game::drawEnemyBullets() {
	D3DXVECTOR3 moveRate;
	D3DXMATRIX rotation;
	D3DXMATRIX translation1, translation2;
	double angle; 
	int startX, startY;
	for (int i = 0; i < 1000; i++) {
		if (enemyBullets[i].isActive()) {
			if (enemyBullets[i].inBounds(playerBox, playerPos.x + 10, playerPos.y + 10) && !exploding) {
				exploding = true;
				enemyBullets[i].setActive(false);
			}
			angle = atan(moveRate.y/moveRate.x);
			D3DXMatrixTranslation(&translation1,-1*enemyBullets[i].getPos(0),-1*enemyBullets[i].getPos(1),0);
			if (enemyBullets[i].getPos(0) < playerPos.x)
				D3DXMatrixRotationZ(&rotation, angle + PI/4);
			else D3DXMatrixRotationZ(&rotation, angle - PI/4);
			D3DXMatrixTranslation(&translation2,enemyBullets[i].getPos(0),enemyBullets[i].getPos(1),0);
			D3DXMatrixMultiply(&spriteManip, &translation1, &rotation);
			D3DXMatrixMultiply(&spriteManip, &spriteManip, &translation2);
			gameSprites->SetTransform(&spriteManip);

			gameSprites->Draw(bulletTexture, &greenBullet, NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
			moveRate = enemyBullets[i].getTarget() - enemyBullets[i].getStartPos();
			D3DXVec3Normalize(&moveRate, &moveRate);
			enemyBullets[i].move(moveRate.x*3, moveRate.y*3, moveRate.z*3); 
			if (enemyBullets[i].getPos(1) > 620 || enemyBullets[i].getPos(1) < -10)
				enemyBullets[i].setActive(false);
			D3DXMatrixIdentity(&spriteManip);
			gameSprites->SetTransform(&spriteManip);
		}
	}
}

void Game::moveEnemies(int start, int end, int midX, int midY, int endX, int endY, RECT sprite) {
	moves.x = 0; moves.z = 0;
	for (int i = start; i < end; i++) {
		// draw enemies
		int action = enemies[i].getAction();
		if (enemies[i].getPos(1) < 0 && enemies[i].isActive()) 
			enemies[i].setAction(0);
		if (enemies[i].getPos(1) >= 0 && enemies[i].isActive()) {
			if (enemies[i].isExploding()) {
				explosionAnim.left = 0;
				explosionAnim.top = 0;
				explosionAnim.right = 80;
				explosionAnim.bottom = 80;
				gameSprites->Draw(explosionTexture, &explosionAnim, NULL, &enemies[i].getPos(), 0xFFFFFFFF);
				enemies[i].setAnimTime(enemies[i].getAnimTime() - 1);
				if (enemies[i].getAnimTime() <= 0) {
					enemies[i].setActive(false);
					enemies[i].setExploding(false);
					enemies[i].setPos(-100, -100, -100);
				}
			}
			else gameSprites->Draw(gameTexture, &sprite, NULL, &enemies[i].getPos(), 0xFFFFFFFF);
		}
		for (int j = 0; j < 100; j++) {
			if (enemies[i].inBounds(playerBullets[j]) && playerBullets[j].isActive() && enemies[i].getPos(1) > 0 && !enemies[i].isExploding()) {
				enemies[i].setExploding(true);
				playerBullets[j].setExploding(true);
			}
		}

		// move enemies
		switch(action) {
			case 0: {
				moves.y = 5;
				D3DXVec3CatmullRom(&enemyPos, &D3DXVECTOR3(enemies[i].getStartPos().x, enemies[i].getStartPos().y, 0),
											&D3DXVECTOR3(enemies[i].getStartPos().x, enemies[i].getStartPos().y, 0),
											&D3DXVECTOR3(midX + 25*(i-start), midY, 0),
											&D3DXVECTOR3(midX + 25*(i-start), midY, 0), enemies[i].getS());
				enemies[i].setS(enemies[i].getS() + 0.01);
				enemies[i].setPos(enemyPos.x, enemyPos.y, enemyPos.z);
				enemies[i].setWaitTime(0);
				if (enemies[i].getCooldown() <= 0) {
					enemies[i].aimFire(enemyBullets, playerPos, 1000, i, laser);
					enemies[i].setCooldown(1);
				}
				else enemies[i].setCooldown(enemies[i].getCooldown() - 1);
				if (enemies[i].getPos(1) >= midY - 2*i)
					enemies[i].setAction(1);
				break;
			}
			case 1: {
				enemies[i].wait();
				if (enemies[i].getWaitTime() >= 100)
					enemies[i].setAction(2);
				break;
			}
			case 2: {
				moves.y = -5;
				D3DXVec3CatmullRom(&enemyPos, &D3DXVECTOR3(endX, endY, 0),
											&D3DXVECTOR3(endX, endY, 0),
											&D3DXVECTOR3(midX + 25*(i-start), midY, 0),
											&D3DXVECTOR3(midX + 25*(i-start), midY, 0), enemies[i].getS());
				enemies[i].setS(enemies[i].getS() - 0.01);
				enemies[i].setPos(enemyPos.x, enemyPos.y, enemyPos.z);
				if (enemies[i].getPos(0) < 0 || enemies[i].getPos(1) < 0 || enemies[i].getPos(0) > SCREEN_WIDTH || enemies[i].getPos(1) > SCREEN_HEIGHT) {
					enemies[i].setActive(false);
				}
				break;
			}
		}
	}
}