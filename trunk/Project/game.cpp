#include "game.h"

void Game::setHwnd(HWND _hwnd) {
	hwnd = _hwnd;
}

/* d3d initialization */

void Game::initd3d() {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
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
			if (GetAsyncKeyState(VK_DOWN) && !exploding && playerPos.y + moveRate + 32 < SCREEN_HEIGHT) 
				playerPos.y += moveRate;
			if (GetAsyncKeyState(VK_UP) && !exploding && playerPos.y - moveRate > 0) 
				playerPos.y -= moveRate;
			if (GetAsyncKeyState(VK_LEFT) && !exploding && playerPos.x - moveRate > 0) 
				playerPos.x -= moveRate;
			if (GetAsyncKeyState(VK_RIGHT) && !exploding && playerPos.x + moveRate + 22 < SCREEN_WIDTH)
				playerPos.x += moveRate;
			/*if (GetAsyncKeyState('Z')) 
				playerPos.z -= 5;
			if (GetAsyncKeyState('A'))
				playerPos.z += 5;*/
			if (GetAsyncKeyState(VK_SPACE) && !exploding) {
				for (int num = 0; num < 4; num++) {
					for (int i = 0; i < 1000; i++) {
						//if (cooldown <= 0) {
							if (!playerBullets[i].isActive()) {
								playerBullets[i].setActive(true);
								playerBullets[i].init(playerPos.x-10+num*10, playerPos.y, playerPos.z, laser, 0, 1);
								break;
							}
							//cooldown = 1;
						//}
						//else cooldown--;
					}
				}
			}
			if (GetAsyncKeyState(VK_ESCAPE)) {
				screen = 0;
				delete playerBullets;
				delete enemyBullets;
				enemyTexture->Release();
				enemiesList.clear();
				initMenuScreen();
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
				explosionAnim.left = curFrame*100;
				explosionAnim.top = curRow*100+5*curRow;
				explosionAnim.right = explosionAnim.left+100;
				explosionAnim.bottom = explosionAnim.top+100-5*curRow;
				gameSprites->Draw(explosionTexture, &explosionAnim, NULL, &playerPos, 0xFFFFFFFF);
				explosionTime--;
				if (explosionTime <= 0) {
					curFrame++;
					explosionTime = 5;
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
					if (curRow < 4)
						curRow = 4;
					else curRow++;
				}
			}
			else {
				gameSprites->Draw(gameTexture, &player, NULL, &playerPos, 0xFFFFFFFF);
				gameSprites->Draw(bulletTexture, &laser, NULL, &D3DXVECTOR3(playerBox.left + playerPos.x + 7, playerBox.top + playerPos.y + 14, 0), 0xFFFFFFFF);
			}

			// draw player bullets
			drawPlayerBullets();

			// draw enemy bullets
			drawEnemyBullets();
			// move enemies
			level1Script();
			moveEnemies();
			gameSprites->End();
			drawTitle();
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
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Edwardian Script ITC"), &font); 
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
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("enemySprites.png"), &enemyTexture))) {
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
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("greenLaserRay.png"), &greenLaserTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	D3DXCreateFont(pDev, 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Franklin Gothic Demi"), &font); 
	playerPos.x = SCREEN_WIDTH/2; playerPos.y = SCREEN_HEIGHT*8/10; playerPos.z = 0;
	setRects();
	srand(time(0));
	offset = 0;
	exploding = false;
	explosionTime = 3;
	curFrame = 0;
	curRow = 0;
	cooldown = 0;
	curAlpha = 50;
	playerBullets = new Bullet[1000];
	enemyBullets = new Bullet[1000];
	enemies = new Enemy[100];
	vector<Enemy> enemiesList; 
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

void Game::drawPlayerBullets() {
	for (int i = 0; i < 1000; i++) {
		if (playerBullets[i].isActive()) {
			if (playerBullets[i].isExploding()) {
				explosionAnim.left = 0;
				explosionAnim.top = 0;
				explosionAnim.right = 80;
				explosionAnim.bottom = 80;
				spriteManip = scale(translation1, translation2, playerBullets[i].getPos(0), playerBullets[i].getPos(1), scaling, 0.5, 0.5);
				gameSprites->SetTransform(&spriteManip);
				gameSprites->Draw(explosionTexture, &explosionAnim, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
				playerBullets[i].setAnimTime(playerBullets[i].getAnimTime() - 1);
				if (playerBullets[i].getAnimTime() <= 0) {
					playerBullets[i].setActive(false);
					playerBullets[i].setExploding(false);
					playerBullets[i].setPos(-100,-100,-100);
				}
			}
			else {
				spriteManip = scale(translation1, translation2, playerBullets[i].getPos(0), playerBullets[i].getPos(1), scaling, 0.75, 0.9);
				gameSprites->SetTransform(&spriteManip);
				gameSprites->Draw(bulletTexture, &laser, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
				playerBullets[i].move(0,-15,0); 
			}
			resetMatrices();
			gameSprites->SetTransform(&spriteManip);
			if (playerBullets[i].getPos(1) < 0)
				playerBullets[i].setActive(false);
		}
	}
}

void Game::drawEnemyBullets() {
	D3DXVECTOR3 moveRate;
	double angle; 
	int startX, startY, bulletType;
	for (int i = 0; i < 1000; i++) {
		if (enemyBullets[i].isActive()) {
			if (enemyBullets[i].getType() == 3) {
				if (enemyBullets[i].inBounds(playerBox, playerPos.x + 45, playerPos.y + 21) && !exploding) {
					exploding = true;
					enemyBullets[i].setActive(false);
				}
			}
			else if (enemyBullets[i].inBounds(playerBox, playerPos.x + 14, playerPos.y + 21) && !exploding) {
				exploding = true;
				enemyBullets[i].setActive(false);
			}

			moveRate = enemyBullets[i].getTarget() - enemyBullets[i].getStartPos();
			D3DXVec3Normalize(&moveRate, &moveRate);
			angle = atan(moveRate.y/moveRate.x);

			rotateBullets(angle, i);

			gameSprites->SetTransform(&spriteManip);
			if (enemyBullets[i].getType() == 3)
				gameSprites->Draw(greenLaserTexture, NULL, NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
			else gameSprites->Draw(bulletTexture, &enemyBullets[i].getInitialBounds(), NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
			enemyBullets[i].move(moveRate.x*3, moveRate.y*3, moveRate.z*3); 
			if (enemyBullets[i].getPos(1) > 620 || enemyBullets[i].getPos(1) < -10 || enemyBullets[i].getPos(0) < 0 || enemyBullets[i].getPos(0) > SCREEN_WIDTH)
				enemyBullets[i].setActive(false);
			resetMatrices();
			gameSprites->SetTransform(&spriteManip);
		}
	}
}

void Game::moveEnemies() {
	moves.x = 0; moves.z = 0;
	for (int i = 0; i < enemiesList.size(); i++) {
		int action = enemiesList[i].getAction();
		if (enemiesList[i].isActive()) {
			drawEnemy(i);
			// move enemies
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
					moves.y = -5;
					if (enemiesList[i].getCooldown() <= 0) {
						enemiesList[i].aimFire(enemyBullets, playerPos, enemiesList[i].getPos(), 1000, i, calcHitbox(greenBullet), greenBullet, 0);
						enemiesList[i].setCooldown(30);	
					}
					else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
					enemiesList[i].moveTo(0);
					if (enemiesList[i].getPos(0) < 0 || enemiesList[i].getPos(1) < 0 || enemiesList[i].getPos(0) > SCREEN_WIDTH || enemiesList[i].getPos(1) > SCREEN_HEIGHT) {
   						enemiesList[i].setActive(false);
					}
					break;
				}
				case 3: {
					bossPattern(i, 100);
				}
			}
		}
	}
}

void Game::drawEnemy(int i) {
	// draw enemies
	if (enemiesList[i].isExploding()) {
		explosionAnim.left = 0;
		explosionAnim.top = 0;
		explosionAnim.right = 80;
		explosionAnim.bottom = 80;
		gameSprites->Draw(explosionTexture, &explosionAnim, NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
		enemiesList[i].setAnimTime(enemiesList[i].getAnimTime() - 1);
		if (enemiesList[i].getAnimTime() <= 0) {
			enemiesList[i].setActive(false);
			enemiesList[i].setExploding(false);
			enemiesList[i].setPos(-100, -100, -100);
		}
	}
	else {
		//gameSprites->Draw(enemyTexture, &enemiesList[i].getInitialBounds(), NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
	}
	if (enemiesList[i].getPos(1) >= 0) { 
		for (int j = 0; j < 100; j++) {
			if (enemiesList[i].inBounds(playerBullets[j]) && playerBullets[j].isActive() && enemiesList[i].getPos(1) > 0 && !enemiesList[i].isExploding()) {
				if (enemiesList[i].getLife() <= 0) {
					enemiesList[i].setExploding(true);
				}
				else enemiesList[i].setLife(enemiesList[i].getLife()-0.25f);
				playerBullets[j].setExploding(true);
			}
		}
	}
}

void Game::advance(int i) {
	moves.y = 5;
	enemiesList[i].moveTo(1);
	if (enemiesList[i].getCooldown() <= 0) {
		if (enemiesList[i].getType() == 0)
			enemiesList[i].aimFire(enemyBullets, playerPos, enemiesList[i].getPos(), 1000, i, calcHitbox(greenBullet), greenBullet, 0);
		if (enemiesList[i].getType() == 1)
			enemiesList[i].aimFire(enemyBullets, playerPos, enemiesList[i].getPos(), 1000, i, calcHitbox(purpleBullet), purpleBullet, 1);
		if (enemiesList[i].getType() == 3) {
			D3DXVECTOR3 shot = playerPos;
			for (int j = 0; j < 10; j++) {
				enemiesList[i].aimFire(enemyBullets, shot, enemiesList[i].getPos(), 1000, i, calcHitbox(purpleBullet), purpleBullet, 1);
				shot = rotateVector(shot, PI/12, 1);
				enemiesList[i].setCooldown(50);
			}
		}
		if (enemiesList[i].getType() != 3)
			enemiesList[i].setCooldown(20);
	}
	else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
	if (enemiesList[i].getS() >= 0.95)
		if (enemiesList[i].getType() == -1)
			enemiesList[i].setAction(3);
	else enemiesList[i].setAction(1);
}

void Game::waiting(int i) {
	D3DXVECTOR3 shot = playerPos;
	shot.z = 0;
	if (enemiesList[i].getType() == 1) {
		for (int j = 0; j < 8; j++) {
			if (j == 0 || j == 4)
				shot.x = 0;
			if (j == 1 || j == 2 || j == 3)
				shot.x = 1;
			if (j == 5 || j == 6 || j == 7)
				shot.x = -1;
			if (j == 0 || j == 1 || j == 7)
				shot.y = 1;
			if (j == 2 || j == 6)
				shot.y = 0;
			if (j == 3 || j == 4 || j == 5)
				shot.y = -1;
			if (enemiesList[i].getCooldown() <= 0) {
				enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1)+10, shot.z), 
					D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1)+10, 0), 1000, i, redBall, redBall, 2);
				enemiesList[i].setCooldown(20);
			}
			else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
			//break;
		}
		enemiesList[i].wait();
		if (enemiesList[i].getWaitTime() >= 100)
			enemiesList[i].setAction(2);
	}
	else if (enemiesList[i].getType() == 2) {
		D3DXVECTOR3 corner1, corner2;
		if (enemiesList[i].getCooldown() <= 0) {
			for (int j = 0; j < 3; j++) {	
				if (j == 0) {
					corner1 = D3DXVECTOR3(-1*greenLaser.right,greenLaser.top,0);
					corner2 = D3DXVECTOR3(-1*greenLaser.left,greenLaser.bottom,0);
					greenLaser.left = rotateVector(corner1, PI/6, 1).x;
					greenLaser.top = rotateVector(corner1, PI/6, 1).y;
					greenLaser.right = rotateVector(corner2, PI/6, 1).x;
					greenLaser.bottom = rotateVector(corner2, PI/6, 1).y;
					shot.x = -1*sqrt(3.0f);
					shot.y = 1;
					enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)-70, shot.y+enemiesList[i].getPos(1)+60, shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)-70, enemiesList[i].getPos(1)+60, 0), 1000, i, greenLaser, greenLaser, 3);
				}
				if (j == 1) {
					shot.x = 0;
					shot.y = 1;
					corner1 = D3DXVECTOR3(greenLaser.left,greenLaser.top,0);
					corner2 = D3DXVECTOR3(greenLaser.right,greenLaser.bottom,0);
					/*greenLaser.left = rotateVector(corner1, 1.65, 1).x;
					greenLaser.top = rotateVector(corner1, 1.65, 1).y;
					greenLaser.right = rotateVector(corner2, 1.65, 1).x;
					greenLaser.bottom = rotateVector(corner2, 1.65, 1).y;*/
					greenLaser.left = 0;
					greenLaser.top = 0;
					greenLaser.right = 10;
					greenLaser.bottom = 110;
					enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+50, shot.y+enemiesList[i].getPos(1), shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+50, enemiesList[i].getPos(1), 0), 1000, i, greenLaser, greenLaser, 3);
				}
				if (j == 2) {
					corner1 = D3DXVECTOR3(greenLaser.left,greenLaser.top,0);
					corner2 = D3DXVECTOR3(greenLaser.right,greenLaser.bottom,0);
					greenLaser.left = rotateVector(corner1, PI/6, 1).x;
					greenLaser.top = rotateVector(corner1, PI/6, 1).y;
					greenLaser.right = rotateVector(corner2, PI/6, 1).x;
					greenLaser.bottom = rotateVector(corner2, PI/6, 1).y;
					shot.x = sqrt(3.0f);
					shot.y = 1;
					enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0), shot.y+enemiesList[i].getPos(1), shot.z), 
						enemiesList[i].getPos(), 1000, i, greenLaser, greenLaser, 3);
				}
				//shot = rotateVector(shot, PI/4, 0);	
				greenLaser.left = 10;
				greenLaser.top = 8;
				greenLaser.right = 100;
				greenLaser.bottom = 24;
				enemiesList[i].setCooldown(85);	
			}
		}
		else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);		
		enemiesList[i].wait();
		if (enemiesList[i].getWaitTime() >= 300)
			enemiesList[i].setAction(2);
	}
	else enemiesList[i].setAction(2);
}

void Game::bossPattern(int i, int interval) {
	D3DXVECTOR3 direction;
	if (leveltime % interval == 0) {
		//int random = rand();
		if (rand()%2 == 0) {
			direction = D3DXVECTOR3(rand()%25-10, rand()%25-10, 0);
			D3DXVec3Normalize(&direction, &direction);
			enemiesList[i].setHeading(direction);
		}
		else enemiesList[i].setHeading(D3DXVECTOR3(0,0,0));
	}
	direction = enemiesList[i].getHeading();
	if (enemiesList[i].getPos(0) + direction.x*2 > 150 && enemiesList[i].getPos(0) + direction.x*2 < 600
		&& enemiesList[i].getPos(1) + direction.y*2 > 50 && enemiesList[i].getPos(1) + direction.y*2 < 200)
		enemiesList[i].move(direction.x*2, direction.y*2, direction.z*2);
	else enemiesList[i].setHeading(D3DXVECTOR3(-1*direction.x,-1*direction.y,0));
	if (enemiesList[i].getLife() < 10000);
	else if (enemiesList[i].getLife() < 30000);
	else if (enemiesList[i].getLife() <= 50000)
		rotatingFire(i, 0);
}

void Game::rotatingFire(int i, int direction) {
	if (enemiesList[i].getTargeting().x > 15000 || enemiesList[i].getTargeting().x < -15000) {
		if (rand()%5 == 0) {
			if (direction == 0)
				direction = 1;
			else direction = 0;
		}
	}
	D3DXVECTOR3 newTarget = rotateVector(enemiesList[i].getTargeting(), PI/12, direction);
	enemiesList[i].setTargeting(newTarget);
	newTarget.x *= 3;
	newTarget.y *= 3;
	enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(newTarget.x+enemiesList[i].getPos(0)+10, newTarget.y+enemiesList[i].getPos(1)+10,0), 
		D3DXVECTOR3(enemiesList[i].getPos(0)+10,enemiesList[i].getPos(1)+10,0), 1000, i, calcHitbox(greenBullet), greenBullet, 1);
}

/*void Game::fireSpiral(double t, int size) {
	int x = t*cos(t);
	int y = t*sin(t);
	for (int i=0; i < size; i++) {
		if (!enemyBullets[i].isActive()) {
			enemyBullets[i].init(x, y, 0, bounds, init_, type_, 1);
			D3DXVECTOR3 target = D3DXVECTOR3(targetPos.x - this->getPos(0), targetPos.y - this->getPos(1), 0);
			enemyBullets[i].setTarget(targetPos);
			break;
		}
	}
}*/

void Game::makeEnemy(int x, int y, int z, RECT bounds, int type, int midX, int midY, int endX, int endY, int life, int speed) {
	Enemy next;
	enemiesList.push_back(next);
	enemiesList.back().init(x, y, z, bounds, type, midX, midY, endX, endY, life, speed);
}

RECT Game::calcHitbox(RECT bounds) {
	RECT hitbox;
	hitbox.left = 0;
	hitbox.right = (bounds.right - bounds.left)/4;
	hitbox.bottom = (bounds.bottom - bounds.top)/4;
	hitbox.top = 0;
	return hitbox;
}

D3DXVECTOR3 Game::rotateVector(D3DXVECTOR3 vec, double angle, size_t direction) {
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

D3DXMATRIX Game::scale(D3DXMATRIX translation1, D3DXMATRIX translation2, int x, int y, D3DXMATRIX scaling, float xFactor, float yFactor) {
	D3DXMATRIX resultMatrix;
	D3DXMatrixTranslation(&translation1, -1*x, -1*y, 0);
	D3DXMatrixScaling(&scaling, xFactor, yFactor, 1);
	D3DXMatrixTranslation(&translation2, x, y, 0);
	D3DXMatrixMultiply(&resultMatrix, &translation1, &scaling);
	D3DXMatrixMultiply(&resultMatrix, &resultMatrix, &translation2);
	return resultMatrix;
}

void Game::rotateBullets(double angle, int i) {
	D3DXMatrixTranslation(&translation1,-1*enemyBullets[i].getPos(0),-1*enemyBullets[i].getPos(1),0);
	if (enemyBullets[i].getType() == 3) {
		D3DXMatrixRotationZ(&rotation, angle);
		D3DXMatrixScaling(&scaling, 0.3, 0.3, 1);
	}
	else if (enemyBullets[i].getStartPos().x < enemyBullets[i].getTarget().x)
		D3DXMatrixRotationZ(&rotation, angle + PI/4);
	else D3DXMatrixRotationZ(&rotation, angle - PI/4);
	D3DXMatrixTranslation(&translation2,enemyBullets[i].getPos(0),enemyBullets[i].getPos(1),0);
	D3DXMatrixMultiply(&spriteManip, &translation1, &scaling);
	D3DXMatrixMultiply(&spriteManip, &spriteManip, &rotation);
	D3DXMatrixMultiply(&spriteManip, &spriteManip, &translation2);
}

void Game::resetMatrices() {
	D3DXMatrixIdentity(&spriteManip);
	D3DXMatrixIdentity(&rotation);
	D3DXMatrixIdentity(&scaling);
	D3DXMatrixIdentity(&translation1);
	D3DXMatrixIdentity(&translation2);
}

void Game::drawTitle() {
	if (leveltime >= 0 && leveltime < 240) {
		//fontColor = D3DCOLOR_ARGB(255,240,255,120); 
		levelText.left = SCREEN_WIDTH/2-100;
		levelText.right = SCREEN_WIDTH/2+200;
		descText.left = SCREEN_WIDTH/2;
		descText.right = SCREEN_WIDTH/2+200;
		for (int i = 0; i < 60; i++) {
			if (leveltime == i)  {
				curAlpha += 3;
				fontColor = D3DCOLOR_ARGB(curAlpha,200,200,255); 
				/*levelText.left = 7*i;
				levelText.right = 7*i+200;
				descText.left = SCREEN_WIDTH-5*i-200;
				descText.right = SCREEN_WIDTH-5*i;*/
			}
		}
		for (int i = 160; i < 240; i++) {
			if (leveltime == i && curAlpha-3 >= 0)  {
				curAlpha -= 3;
				fontColor = D3DCOLOR_ARGB(curAlpha,200,200,255); 
				/*levelText.left = 7*(i-100);
				levelText.right = 7*(i-100)+200;
				descText.left = SCREEN_WIDTH-5*(i-100)-200;
				descText.right = SCREEN_WIDTH-5*(i-100);*/
			}
		}
		font->DrawText(NULL, TEXT("- Stage 1 -"), -1, &levelText, 0, fontColor);
		//font->DrawText(NULL, TEXT("Introduction"), -1, &descText, 0, fontColor);
	}
}

void Game::setRects() {
	player.left=0;
	player.right=28;
	player.top=0;
	player.bottom=43;
	playerBox.left = 0;
	playerBox.right=player.right/20;
	playerBox.top = 0;
	playerBox.bottom=player.bottom/20;
	kaguya.left=454;
	kaguya.top=1360;
	kaguya.right=500;
	kaguya.bottom=1420;
	aimedShot.left=170; 
	aimedShot.top=670;
	aimedShot.right=230; 
	aimedShot.bottom=740;
	fairy.left=172; 
	fairy.top=310;
	fairy.right=219; 
	fairy.bottom=371;
	spreadShot.left=454;
	spreadShot.top=380;
	spreadShot.right=500;
	spreadShot.bottom=435;
	boss.left=445;
	boss.top=991;
	boss.right=502;
	boss.bottom=1050;
	laser.left=127;
	laser.right=143;
	laser.top=0;
	laser.bottom=16;
	greenBullet.left = 159;
	greenBullet.right = 175;
	greenBullet.top = 22;
	greenBullet.bottom = 40;
	redBall.left = 30; 
	redBall.top = 40;
	redBall.right = 48; 
	redBall.bottom = 62;
	purpleBullet.left = 62; 
	purpleBullet.top = 20;
	purpleBullet.right = 78; 
	purpleBullet.bottom = 38;
	greenLaser.left = 0;
	greenLaser.top = 0;
	greenLaser.right = 110;
	greenLaser.bottom = 32;
	/*greenLaser.left = 0;
	greenLaser.top = 0;
	greenLaser.right = 110;
	greenLaser.bottom = 32;*/
	levelText.top = 250;
	levelText.bottom = 300;
	descText.top = 310;
	descText.bottom = 360;
}

void Game::level1Script() {
// events
	for (int i = 0; i < enemiesList.size(); i++) {
		if (enemiesList[i].isActive())
			break;
		if (i == enemiesList.size()-1) {
			enemiesList.clear();
		}
	}

	if (leveltime == 250) {
		makeEnemy(150, -25, 0, aimedShot, 0, 50, 215, -30, 300, 50, 2);
		makeEnemy(175, -35, 0, aimedShot, 0, 60, 225, -30, 300, 50, 2);
		makeEnemy(200, -25, 0, aimedShot, 0, 70, 220, -30, 300, 50, 2);
		makeEnemy(225, -20, 0, aimedShot, 0, 80, 225, -30, 300, 50, 2);
		makeEnemy(225, -20, 0, kaguya, 1, 150, 200, -30, 300, 250, 2);
		makeEnemy(250, -40, 0, aimedShot, 0, 95, 218, -30, 300, 50, 2);
		makeEnemy(275, -35, 0, aimedShot, 0, 100, 225, -30, 300, 50, 2);
		makeEnemy(300, -25, 0, aimedShot, 0, 110, 220, -30, 300, 50, 2);
		makeEnemy(325, -20, 0, aimedShot, 0, 120, 225, -30, 300, 50, 2);
	}

	if (leveltime == 450) {
		makeEnemy(550,-40, 0, aimedShot, 0, 550, 215, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(575, -35, 0, aimedShot, 0, 560, 225, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(600, -25, 0, aimedShot, 0,  570, 220, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(625, -20, 0, aimedShot, 0, 580, 225, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(625, -20, 0, kaguya, 1, 500, 200, SCREEN_WIDTH, 300, 250, 2);
		makeEnemy(650, -40, 0, aimedShot, 0, 600, 218, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(670, -35, 0, aimedShot, 0, 610, 225, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(700, -25, 0, aimedShot, 0,  620, 220, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(725, -20, 0, aimedShot, 0, 630, 225, SCREEN_WIDTH, 300, 50, 2);
	}

	if (leveltime == 700) {
		makeEnemy(200,-40, 0, aimedShot, 0, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(300, -35, 0, aimedShot, 0, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(400, -25, 0, aimedShot, 0, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(500, -20, 0, aimedShot, 0, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(100,-40, 0, aimedShot, 0, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(600, -35, 0, aimedShot, 0, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(700, -25, 0, aimedShot, 0, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(800, -20, 0, aimedShot, 0, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(205,-40, 0, aimedShot, 0, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(305, -35, 0, aimedShot, 0, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(405, -25, 0, aimedShot, 0, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(505, -20, 0, aimedShot, 0, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(105,-40, 0, aimedShot, 0, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(605, -35, 0, aimedShot, 0, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(705, -25, 0, aimedShot, 0, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(805, -20, 0, aimedShot, 0, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(SCREEN_WIDTH/2 - 200, -20, 0, kaguya, 1, SCREEN_WIDTH/2 - 200, 250, SCREEN_WIDTH/2 - 200, -20, 250, 2);
		makeEnemy(SCREEN_WIDTH/2 - 100, -20, 0, kaguya, 1, SCREEN_WIDTH/2 - 100, 250, SCREEN_WIDTH/2 - 100, -20, 250, 2);
		makeEnemy(SCREEN_WIDTH/2 + 100, -20, 0, kaguya, 1, SCREEN_WIDTH/2 + 100, 250, SCREEN_WIDTH/2 + 100, -20, 250, 2);
	}

	if (leveltime == 1000) {makeEnemy(550,-40, 0, aimedShot, 0, 600, 215, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(575, -35, 0, aimedShot, 0, 610, 225, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(600, -25, 0, aimedShot, 0,  620, 220, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(625, -20, 0, aimedShot, 0, 630, 225, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(650, -40, 0, aimedShot, 0, 640, 218, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(670, -35, 0, aimedShot, 0, 650, 225, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(700, -25, 0, aimedShot, 0,  660, 220, SCREEN_WIDTH, 300, 50, 2);
		makeEnemy(725, -20, 0, aimedShot, 0, 670, 225, SCREEN_WIDTH, 300, 50, 2);
	}

	if (leveltime >= 1020 && leveltime < 1100) {
		for (int i = 1020; i < 1100; i+=10) {
			if (leveltime == i) {
				makeEnemy(0, 200, 0, spreadShot, 3, 1000, 250, -100, 200, 25, 1);
			}
		}
	}

	if (leveltime == 1250) {
		makeEnemy(150, -25, 0, aimedShot, 0, 50, 215, -30, 300, 50, 2);
 		makeEnemy(175, -35, 0, aimedShot, 0, 60, 225, -30, 300, 50, 2);
		makeEnemy(200, -25, 0, aimedShot, 0, 70, 220, -30, 300, 50, 2);
		makeEnemy(225, -20, 0, aimedShot, 0, 80, 225, -30, 300, 50, 2);
		makeEnemy(250, -40, 0, aimedShot, 0, 95, 218, -30, 300, 50, 2);
		makeEnemy(275, -35, 0, aimedShot, 0, 100, 225, -30, 300, 50, 2);
		makeEnemy(300, -25, 0, aimedShot, 0, 110, 220, -30, 300, 50, 2);
		makeEnemy(325, -20, 0, aimedShot, 0, 120, 225, -30, 300, 50, 2);
	}

	if (leveltime >= 1260 && leveltime < 1340) {
		for (int i = 1260; i < 1340; i+=10) {
			if (leveltime == i) {
				makeEnemy(900, 200, 0, spreadShot, 3, -100, 250, 1000, 200, 25, 1);
			}
		}
	}

	if (leveltime >= 1500 && leveltime < 1550) {
		for (int i = 1500; i < 1550; i+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, aimedShot, 0, 100, 250, -10, 300, 25, 2);
				makeEnemy(600, -10, 0, aimedShot, 0, 600, 250, 1000, 300, 25, 2);
			}
		}
	}

	if (leveltime == 1550) {
		makeEnemy(300, -30, 0, fairy, 2, 300, 230, SCREEN_WIDTH, 0, 500, 2);
	}

	if (leveltime >= 1700 && leveltime < 1750) {
		for (int i = 1700; i < 1750; i+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, spreadShot, 3, 100, 250, -10, 300, 25, 2);
				makeEnemy(600, -10, 0, spreadShot, 3, 600, 250, 1000, 300, 25, 2);
			}
		}
	}

	if (leveltime == 1750) {
		makeEnemy(140, -30, 0, fairy, 2, 200, 230, 0, 0, 500, 2);
		makeEnemy(300, -30, 0, fairy, 2, 300, 230, SCREEN_WIDTH, 0, 500, 2);
		makeEnemy(500, -30, 0, fairy, 2, 450, 230, SCREEN_WIDTH, 0, 500, 2);
	}

	if (leveltime == 1950) {
		makeEnemy(240, -30, 0, fairy, 2, 200, 130, 0, 0, 500, 2);
		makeEnemy(300, -30, 0, fairy, 2, 300, 130, SCREEN_WIDTH, 0, 500, 2);
		makeEnemy(400, -30, 0, fairy, 2, 450, 130, SCREEN_WIDTH, 0, 500, 2);
	}

	if (leveltime >= 2050 && leveltime < 2100) {
		for (int i = 2050; i < 2100; i+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, aimedShot, 0, 100, 250, -10, 300, 25, 2);
				makeEnemy(600, -10, 0, aimedShot, 0, 600, 250, 1000, 300, 25, 2);
			}
		}
	}

	if (leveltime == 2150) {
		makeEnemy(140, -30, 0, fairy, 2, 200, 170, 0, 0, 500, 2);
		makeEnemy(300, -30, 0, fairy, 2, 300, 170, SCREEN_WIDTH, 0, 500, 2);
		makeEnemy(500, -30, 0, fairy, 2, 450, 170, SCREEN_WIDTH, 0, 500, 2);
	}

	if (leveltime == 2350) {
		makeEnemy(240, -30, 0, fairy, 2, 200, 190, 0, 0, 500, 2);
		makeEnemy(320, -30, 0, fairy, 2, 300, 190, SCREEN_WIDTH, 0, 500, 2);
		makeEnemy(400, -30, 0, fairy, 2, 450, 190, SCREEN_WIDTH, 0, 500, 2);
		makeEnemy(700, -30, 0, fairy, 2, 450, 190, SCREEN_WIDTH, 0, 500, 2);
	}

	if (leveltime >= 2300 && leveltime < 2400) {
		for (int i = 2300; i < 2400; i+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, spreadShot, 3, 100, 250, -10, 300, 25, 2);
				makeEnemy(600, -10, 0, spreadShot, 3, 600, 250, 1000, 300, 25, 2);
			}
		}
	}

	if (leveltime >= 2600 && leveltime < 2650) {
		for (int i = 2600, j = 0; i < 2650; i+=10, j+=10) {
			if (leveltime == i) {
				makeEnemy(600, -10, 0, fairy, 2, 600, 15*j, 600, -10, 500, 2);
				makeEnemy(100, -10, 0, spreadShot, 3, 100, 250, -10, 300, 25, 2);
			}
		}
	}


	if (leveltime >= 2900 && leveltime < 2950) {
		for (int i = 2900, j = 0; i < 2950; i+=10, j+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, fairy, 2, 100, 15*j, 100, -10, 500, 2);
				makeEnemy(700, -10, 0, spreadShot, 3, 700, 250, 1000, 300, 25, 2);
			}
		}
	}

	if (leveltime == 3300) {
		makeEnemy(200,-40, 0, aimedShot, 0, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(300, -35, 0, aimedShot, 0, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(400, -25, 0, aimedShot, 0, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(500, -20, 0, aimedShot, 0, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(100,-40, 0, aimedShot, 0, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(600, -35, 0, aimedShot, 0, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(700, -25, 0, aimedShot, 0, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(800, -20, 0, aimedShot, 0, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(205,-40, 0, aimedShot, 0, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(305, -35, 0, aimedShot, 0, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(405, -25, 0, aimedShot, 0, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(505, -20, 0, aimedShot, 0, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(105,-40, 0, aimedShot, 0, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(605, -35, 0, aimedShot, 0, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(705, -25, 0, aimedShot, 0, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(805, -20, 0, aimedShot, 0, 305, 300, 1000, 500, 50, 1.5);

		makeEnemy(200,-40, 0, spreadShot, 3, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(300, -35, 0, spreadShot, 3, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(400, -25, 0, spreadShot, 3, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(500, -20, 0, spreadShot, 3, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(100,-40, 0, spreadShot, 3, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(600, -35, 0, spreadShot, 3, 300, 300, -30, 500, 50, 1.5);
		makeEnemy(700, -25, 0, spreadShot, 3, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(800, -20, 0, spreadShot, 3, 300, 300, 1000, 500, 50, 1.5);
		makeEnemy(205,-40, 0, spreadShot, 3, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(305, -35, 0, spreadShot, 3, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(405, -25, 0, spreadShot, 3, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(505, -20, 0, spreadShot, 3, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(105,-40, 0, spreadShot, 3, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(605, -35, 0, spreadShot, 3, 305, 300, -30, 500, 50, 1.5);
		makeEnemy(705, -25, 0, spreadShot, 3, 305, 300, 1000, 500, 50, 1.5);
		makeEnemy(805, -20, 0, spreadShot, 3, 305, 300, 1000, 500, 50, 1.5);

		makeEnemy(SCREEN_WIDTH/2 - 200, -20, 0, kaguya, 1, SCREEN_WIDTH/2 - 200, 250, SCREEN_WIDTH/2 - 200, -20, 250, 2);
		makeEnemy(SCREEN_WIDTH/2 - 100, -20, 0, kaguya, 1, SCREEN_WIDTH/2 - 100, 250, SCREEN_WIDTH/2 - 100, -20, 250, 2);
		makeEnemy(SCREEN_WIDTH/2 + 100, -20, 0, kaguya, 1, SCREEN_WIDTH/2 + 100, 250, SCREEN_WIDTH/2 + 100, -20, 250, 2);
		makeEnemy(SCREEN_WIDTH/2 - 300, -20, 0, kaguya, 1, SCREEN_WIDTH/2 - 300, 250, SCREEN_WIDTH/2 - 200, -20, 250, 2);
		makeEnemy(SCREEN_WIDTH/2 - 150, -20, 0, kaguya, 1, SCREEN_WIDTH/2 - 150, 250, SCREEN_WIDTH/2 - 100, -20, 250, 2);
		makeEnemy(SCREEN_WIDTH/2 + 200, -20, 0, kaguya, 1, SCREEN_WIDTH/2 + 200, 250, SCREEN_WIDTH/2 + 100, -20, 250, 2);
	}

	if (leveltime == 200) {
		makeEnemy(0, -10, 0, boss, -1, 350, 200, 0, 0, 50000, 2);
		makeEnemy(100, -10, 0, fairy, 2, 300, 200, 100, -10, 500, 2);
	}

	/*if (leveltime >= 200 && leveltime < 240) {
		for (int i = 200, j = 0; i < 240; i+=10, j+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, fairy, 2, 300, 15*j, 100, -10, 500, 2);
			}
		}
	}*/
}