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

			/*for (int i = 0; i < MAX_BULLETS; i++) {
				if (!enemyBullets[i].isActive())
					break;
				if (i >= MAX_BULLETS-1) {
					delete enemyBullets;
					enemyBullets = new Bullet[MAX_BULLETS];
				}
			}*/


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
					explosionTime = 6;
				}
				if (curFrame > 4 && curRow > 4) {
					//playerPos.x = SCREEN_WIDTH/2; 
					//playerPos.y = SCREEN_HEIGHT*8/10;
					exploding = false;
					curFrame = 0;
					curRow = 0;
					explosionTime = 100;
					invincible = true;
				}
				else if (curFrame > 4) {
					curFrame = 0;
					if (curRow < 4)
						curRow = 4;
					else curRow++;
				}
			}
			else {
				if ((invincible && leveltime%2 == 0) || !invincible) 
					gameSprites->Draw(enemyTexture, &player, NULL, &playerPos, 0xFFFFFFFF);
				//gameSprites->Draw(bulletTexture, &laser, NULL, &D3DXVECTOR3(playerBox.left + playerPos.x + 7, playerBox.top + playerPos.y + 14, 0), 0xFFFFFFFF);
			}
			if (explosionTime > 5) {
				explosionTime--;
				if (explosionTime <= 5)
					invincible = false;
			}
			

			sceneryParticles();

			// draw player bullets
			drawPlayerBullets();

			// draw enemy bullets
			drawEnemyBullets();
			// move enemies
			level1Script();
			moveEnemies();
			gameSprites->End();
			drawTitle();
			wchar_t buffer[16];
			_itow_s(hits, buffer, 16);
			wstring s(TEXT("Hits taken: "));
			s += wstring(buffer);
			font->DrawText(NULL, s.c_str(), -1, &topRight, 0, fontColor2); 
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
	fontColor2 = D3DCOLOR_ARGB(255,240,240,240);
	exploding = false;
	invincible = false;
	spiral = false;
	chaotic = false;
	spellcard1 = false;
	spellcard2 = false;
	explosionTime = 3;
	curFrame = 0;
	curRow = 0;
	cooldown = 0;
	curAlpha = 50;
	currentT = 0;
	hits = 0;
	playerBullets = new Bullet[1000];
	enemyBullets = new Bullet[MAX_BULLETS];
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

void Game::sceneryParticles() {
	if (leveltime%50 == 0) {
		for (int i = 0; i < 10; i++) {
			if (rand()%2 == 0)
				particleHandler.emit(D3DXVECTOR3(rand()%800, rand()%600, 0), rand()%1);
		}
	}
	vector<Particle> display = particleHandler.getParticles();
	for (int i = 0; i < display.size(); i++) {
		if (display[i].isActive()) {
			D3DCOLOR currentColor;
			if (display[i].getTime() < display[i].getLifespan()) {
				currentColor = D3DCOLOR_ARGB(display[i].getTime()*4, 255, 255, 255);
				if (display[i].getTime()*4 > 255)
					currentColor = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
			else {
				currentColor = D3DCOLOR_ARGB(display[i].getLifespan()*4, 255, 255, 255);
				if (display[i].getLifespan()*4 > 255)
					currentColor = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
			if (display[i].getType() == 0)
				gameSprites->Draw(enemyTexture, &smallGreenParticle, NULL, &display[i].getPos(), currentColor);
			else gameSprites->Draw(enemyTexture, &bigGreenParticle, NULL, &display[i].getPos(), currentColor);
		}
	}
	particleHandler.update();
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
				spriteManip = scale(translation1, translation2, playerBullets[i].getPos(0), playerBullets[i].getPos(1), scaling, 0.7, 0.9);
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
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (enemyBullets[i].isActive()) {
			/*if (enemyBullets[i].getType() == 3) {
				if (enemyBullets[i].inBounds(playerBox, playerPos.x + 45, playerPos.y + 21) && !exploding && !invincible) {
					exploding = true;
					hits++;
					enemyBullets[i].setActive(false);
				}
			}*/
			if (enemyBullets[i].inBounds(playerBox, playerPos.x + 14, playerPos.y + 21) && !exploding && !invincible) {
				exploding = true;
				hits++;
				enemyBullets[i].setActive(false);
			}

			moveRate = enemyBullets[i].getTarget() - enemyBullets[i].getStartPos();
			D3DXVec3Normalize(&moveRate, &moveRate);
			angle = atan(moveRate.y/moveRate.x);

			if (enemyBullets[i].getType() < 3)
			rotateBullets(angle, i);

			gameSprites->SetTransform(&spriteManip);
			//if (enemyBullets[i].getType() == 3)
				//gameSprites->Draw(greenLaserTexture, NULL, NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
			gameSprites->Draw(bulletTexture, &enemyBullets[i].getInitialBounds(), NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
			enemyBullets[i].move(moveRate.x*enemyBullets[i].getSpeed(), moveRate.y*enemyBullets[i].getSpeed(), 0); 
			if (chaotic)
				chaosSpiral(i);
			else if (spiral)
				moveSpiral(i, 10.0f);
			if (enemyBullets[i].getPos(1) > 620 || enemyBullets[i].getPos(1) < 0 || enemyBullets[i].getPos(0) < 0 || enemyBullets[i].getPos(0) > SCREEN_WIDTH)
				enemyBullets[i].setActive(false);
			resetMatrices();
			gameSprites->SetTransform(&spriteManip);
		}
	}
}

void Game::moveEnemies() {
	moves.x = 0; moves.z = 0;
	playerTarget = D3DXVECTOR3(playerPos.x+14, playerPos.y+21, 0);
	D3DXVECTOR3 shot = playerPos;
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
						//enemiesList[i].aimFire(enemyBullets, playerTarget, enemiesList[i].getPos(), MAX_BULLETS, i, calcHitbox(greenBullet), greenBullet, 0, 3);
						enemiesList[i].setCooldown(30);	
					}
					else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
					enemiesList[i].moveTo(0);
					if (enemiesList[i].getPos(0) < -10 || enemiesList[i].getPos(1) < -10 || enemiesList[i].getPos(0) > SCREEN_WIDTH || enemiesList[i].getPos(1) > SCREEN_HEIGHT) {
   						enemiesList[i].setActive(false);
					}
					break;
				}
				case 3: {
					bossPattern(i, 100);
				}
			}
			if (enemiesList[i].getType() == 4) {
				if (enemiesList[i].getCooldown() <= 0) {
					for (int j = 0; j < 2; j++) {
						shot.x = 0;
						if (j == 0)
							shot.y = -1;
						if (j == 1)
							shot.y = 1;
						enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1)+10, shot.z), 
							D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1)+10, 0), MAX_BULLETS, i, redBall, redBall, 2, 3);
						enemiesList[i].setCooldown(6);
					}
				}
				else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
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
		gameSprites->Draw(enemyTexture, &enemiesList[i].getInitialBounds(), NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
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
	playerTarget = D3DXVECTOR3(playerPos.x+14, playerPos.y+21, 0);
	D3DXVECTOR3 shot = playerPos;
	enemiesList[i].moveTo(1);
	if (enemiesList[i].getCooldown() <= 0) {
		if (enemiesList[i].getType() == 0)
			enemiesList[i].aimFire(enemyBullets, playerTarget, enemiesList[i].getPos(), MAX_BULLETS, i, calcHitbox(greenBullet), greenBullet, 0, 3);
		if (enemiesList[i].getType() == 1)
			enemiesList[i].aimFire(enemyBullets, playerTarget, enemiesList[i].getPos(), MAX_BULLETS, i, calcHitbox(purpleBullet), purpleBullet, 1, 3);
		if (enemiesList[i].getType() == 3) {
			for (int j = 0; j < 10; j++) {
				enemiesList[i].aimFire(enemyBullets, shot, enemiesList[i].getPos(), MAX_BULLETS, i, calcHitbox(purpleBullet), purpleBullet, 1, 3);
				shot = rotateVector(shot, PI/12, 1);
				enemiesList[i].setCooldown(50);
			}
		}

		/*if (enemiesList[i].getType() == 4) {
			if (enemiesList[i].getCooldown() <= 0) {
				for (int j = 0; j < 2; j++) {
					shot.x = 0;
					if (j == 0)
						shot.y = -1;
					if (j == 1)
						shot.y = 1;
					enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1)+10, shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1)+10, 0), MAX_BULLETS, i, redBall, redBall, 2, 3);
					enemiesList[i].setCooldown(10);
				}
			}
			else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
		}*/
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
		if (enemiesList[i].getCooldown() <= 0) {
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
			
				enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1)+10, shot.z), 
					D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1)+10, 0), MAX_BULLETS, i, redBall, redBall, 2, 3);
				enemiesList[i].setCooldown(30);
			}
				//break;
		}
		else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
		enemiesList[i].wait();
		if (enemiesList[i].getWaitTime() >= 200)
			enemiesList[i].setAction(2);
	}
	else if (enemiesList[i].getType() == 2) {
		D3DXVECTOR3 corner1, corner2;
		if (enemiesList[i].getCooldown() <= 0) {
			for (int j = 0; j < 3; j++) {	
				if (j == 0) {
					shot.x = -1*sqrt(3.0f);
					shot.y = 1;
					enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1), shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1), 0), MAX_BULLETS, i, largeGreen, largeGreen, 3, 3);
				}
				if (j == 1) {
					shot.x = 0;
					shot.y = 1;
					enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1), shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1), 0), MAX_BULLETS, i, largeGreen, largeGreen, 3, 3);
				}
				if (j == 2) {
					shot.x = sqrt(3.0f);
					shot.y = 1;
					enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0), shot.y+enemiesList[i].getPos(1), shot.z), 
						enemiesList[i].getPos(), MAX_BULLETS, i, largeGreen, largeGreen, 3, 3);
				}
				//shot = rotateVector(shot, PI/4, 0);	
				greenLaser.left = 10;
				greenLaser.top = 8;
				greenLaser.right = 100;
				greenLaser.bottom = 24;
				enemiesList[i].setCooldown(40);	
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
	if (fireDirection != 0 && fireDirection != 1)
		fireDirection = 0;
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
	if (enemiesList[i].getLife() < 1000) {
		chaotic = true;
	}
	if (enemiesList[i].getLife() < 8000) {
		spiral = true;
		if (enemiesList[i].getCooldown() <= 0) {
			rotatingFire(&enemiesList[i], fireDirection, PI/12, 3);
			currentT+=0.5;
			enemiesList[i].setCooldown(1);
		}
		if (currentT > 100) {
			enemiesList[i].setCooldown(5);
			currentT = 0;
		}
		enemiesList[i].setCooldown(enemiesList[i].getCooldown()-1);
	}
	else if (enemiesList[i].getLife() < 9000) {
		if (enemiesList[i].getCooldown() <= 0) {
			fireSpiral(currentT, &enemiesList[i], 8);
			currentT+=0.5;
			enemiesList[i].setCooldown(10);
		}
		if (currentT > 100) {
			enemiesList[i].setCooldown(150);
			currentT = 50;
		}
		enemiesList[i].setCooldown(enemiesList[i].getCooldown()-1);
	}
	else if (enemiesList[i].getLife() <= 13000 && !spellcard2) {
		spellcard2 = true;
		enemiesList[i].setTargeting(D3DXVECTOR3(0,1,0));
		Enemy newStream;
		newStream.init(enemiesList[i].getPos().x,enemiesList[i].getPos().y,0,boss,-1,0,0,0,0,1,1);
		newStream.setTargeting(D3DXVECTOR3(0,-1,0));
		subunits.push_back(newStream);
	}
	else if (enemiesList[i].getLife() <= 17000 && !spellcard1) {
		spellcard1 = true;
		enemiesList[i].setTargeting(D3DXVECTOR3(0,1,0));
		Enemy newStream;
		newStream.init(enemiesList[i].getPos().x,enemiesList[i].getPos().y,0,boss,-1,0,0,0,0,1,1);
		subunits.push_back(newStream);
	}
	else if (enemiesList[i].getLife() < 17000) {
		int fireType;
		//enemiesList[i].aimFire(enemyBullets, playerPos, enemiesList[i].getPos(), MAX_BULLETS, i, calcHitbox(greenBullet), greenBullet, 0);
		if (enemiesList[i].getCooldown() == 0) {
			fireDirection = 0;
			rotatingFire(&enemiesList[i], fireDirection, PI/12, 0);
			for (int j = 0; j < subunits.size(); j++) {
				subunits[j].setPos(enemiesList[i].getPos().x,enemiesList[i].getPos().y,0);
				//enemiesList[i].setTargeting(-enemiesList[i].getTargeting());
				if (fireDirection == 0) {
					fireDirection = 1;
					fireType = 1;
				}
				else {
					fireDirection = 0;
					fireType = 2;
				}
				//double test = atan(enemiesList[i].getTargeting().y/enemiesList[i].getTargeting().x);
				rotatingFire(&subunits[j], fireDirection, PI/12, fireType);
				//enemiesList[i].setCooldown(1);
			}
		}
		else enemiesList[i].setCooldown(enemiesList[i].getCooldown()-1);
	}
	else if (enemiesList[i].getLife() <= 20000)
		if (enemiesList[i].getCooldown() == 0) {
			rotatingFire(&enemiesList[i], fireDirection, PI/12, 0);
			enemiesList[i].setCooldown(1);
		}
		else enemiesList[i].setCooldown(enemiesList[i].getCooldown()-1);
		//enemiesList[i].aimFire(enemyBullets, playerPos, enemiesList[i].getPos(), MAX_BULLETS, i, calcHitbox(greenBullet), greenBullet, 0);
}

void Game::rotatingFire(Enemy* i, int direction, double angle, int type) {
	if (leveltime%50 == 0) {
		if (rand()%2 == 0) {
			if (fireDirection == 0) 
				fireDirection = 1;
			else fireDirection = 0;
		}   
	}
	D3DXVECTOR3 newTarget = rotateVector(i->getTargeting(), angle, direction);
	i->setTargeting(newTarget);
	if (abs(i->getTargeting().x) < 0.001)
		i->setTargeting(D3DXVECTOR3(0,1,0));
	newTarget.x *= 3;
	newTarget.y *= 3;
	if (type == 0)
		i->aimFire(enemyBullets, D3DXVECTOR3(newTarget.x+i->getPos(0)+10, newTarget.y+i->getPos(1)+10,0), 
			D3DXVECTOR3(i->getPos(0)+10,i->getPos(1)+10,0), MAX_BULLETS, 0, calcHitbox(blueBall), blueBall, 5, 3);
	else if (type == 1)
		i->aimFire(enemyBullets, D3DXVECTOR3(newTarget.x+i->getPos(0)+10, newTarget.y+i->getPos(1)+10,0), 
			D3DXVECTOR3(i->getPos(0)+10,i->getPos(1)+10,0), MAX_BULLETS, 0, calcHitbox(yellowStar), yellowStar, 6, 4);
	else if (type == 3)
		i->aimFire(enemyBullets, D3DXVECTOR3(newTarget.x+i->getPos(0)+10, newTarget.y+i->getPos(1)+10,0), 
			D3DXVECTOR3(i->getPos(0)+10,i->getPos(1)+10,0), MAX_BULLETS, 0, calcHitbox(blueBall), blueBall, 4, 3);
	else 
		i->aimFire(enemyBullets, D3DXVECTOR3(newTarget.x+i->getPos(0)+10, newTarget.y+i->getPos(1)+10,0), 
			D3DXVECTOR3(i->getPos(0)+10,i->getPos(1)+10,0), MAX_BULLETS, 0, calcHitbox(largeGreen), largeGreen, 7, 2.5);
}

void Game::fireSpiral(double t, Enemy* enemy_, int size) {
	playerTarget = D3DXVECTOR3(playerPos.x+14, playerPos.y+21, 0);
	D3DXVECTOR3 shot;
	double x = t*cos(t);
	double y = t*sin(t);
	for (int j = 0; j < size; j++) {
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
		enemy_->aimFire(enemyBullets, D3DXVECTOR3(shot.x+x+enemy_->getPos(0), shot.y+y+enemy_->getPos(1), 0), 
			D3DXVECTOR3(x+enemy_->getPos(0), y+enemy_->getPos(1), 0), MAX_BULLETS, 0, calcHitbox(blueBall), blueBall, 5, 3);
	}
			//enemyBullets[i].init(x+enemy_->getPos(0), y+enemy_->getPos(1), 0, calcHitbox(greenBullet), greenBullet, 0, 1);
			//D3DXVECTOR3 target = D3DXVECTOR3(targetPos.x - this->getPos(0), targetPos.y - this->getPos(1), 0);
			//enemyBullets[i].setTarget(playerPos);
}

void Game::moveSpiral(int i, float s_) {
	double x, y, s;
	if (enemyBullets[i].isActive() && enemyBullets[i].getType() == 4) {
		s = enemyBullets[i].getS();
		if (s == 0)
			enemyBullets[i].setS(s_);
		if (i%2 == 0) {
			x = s*cos(s);
			y = s*sin(s);
		}
		else {
			x = -s*cos(s);
			y = -s*sin(s);
		}
		enemyBullets[i].setPos(0.1*x+enemyBullets[i].getPos(0), 0.1*y+enemyBullets[i].getPos(1), 0);
		enemyBullets[i].setS(enemyBullets[i].getS()+0.2);
	}
}

void Game::chaosSpiral(int i) {
	double x, y, s;
	if (enemyBullets[i].isActive() && enemyBullets[i].getType() == 4) {
		s = enemyBullets[i].getS();
		if (rand()%2 == 0) {
			x = s*cos(s);
			y = s*sin(s);
		}
		else {
			x = -s*cos(s);
			y = -s*sin(s);
		}
		enemyBullets[i].setPos(x+enemyBullets[i].getPos(0), y+enemyBullets[i].getPos(1), 0);
		enemyBullets[i].setS(enemyBullets[i].getS()+0.1);
	}
}

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
	/*if (enemyBullets[i].getType() == 3) {
		D3DXMatrixRotationZ(&rotation, angle);
		D3DXMatrixScaling(&scaling, 0.3, 0.3, 1);
	}*/
	if (enemyBullets[i].getStartPos().x < enemyBullets[i].getTarget().x)
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
	else fontColor = fontColor = D3DCOLOR_ARGB(255,200,200,255); 
}

void Game::setRects() {
	/*player.left=0;
	player.right=28;
	player.top=0;
	player.bottom=43;*/
	player.left = 408;
	player.top = 1988;
	player.right = 425;
	player.bottom = 2050;
	playerBox.left = 0;
	playerBox.right=11;
	playerBox.top = 0;
	playerBox.bottom=11;
	kaguya.left=454;
	kaguya.top=1360;
	kaguya.right=500;
	kaguya.bottom=1420;
	aimedShot.left=281; 
	aimedShot.top=1235;
	aimedShot.right=330; 
	aimedShot.bottom=1290;
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
	verticalShot.left=105;
	verticalShot.top=930;
	verticalShot.right=172;
	verticalShot.bottom=990;
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
	smallGreenParticle.left = 282;
	smallGreenParticle.top = 1980;
	smallGreenParticle.right = 322;
	smallGreenParticle.bottom = 2056;
	bigGreenParticle.left = 333;
	bigGreenParticle.top = 1988;
	bigGreenParticle.right = 377;
	bigGreenParticle.bottom = 2056;
	topRight.left = 550;
	topRight.right = 800;
	topRight.top = 0;
	topRight.bottom = 40;
	largeGreen.left = 415;
	largeGreen.top = 45;
	largeGreen.right = 445;
	largeGreen.bottom = 80;
	blueBall.left = 130;
	blueBall.top = 40;
	blueBall.right = 145;
	blueBall.bottom = 65;
	yellowStar.left = 210;
	yellowStar.top = 215;
	yellowStar.right = 225;
	yellowStar.bottom = 232;
	/*383 192 445 253
	207 162 223 174
	128 32 142 46*/
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
		makeEnemy(175, -35, 0, aimedShot, 0, 60, 225, -30, 310, 50, 2);
		makeEnemy(200, -25, 0, aimedShot, 0, 70, 220, -30, 320, 50, 2);
		makeEnemy(225, -20, 0, aimedShot, 0, 80, 225, -30, 330, 50, 2);
		makeEnemy(225, -20, 0, kaguya, 1, 150, 200, -30, 340, 250, 2);
		makeEnemy(250, -40, 0, aimedShot, 0, 95, 218, -30, 350, 50, 2);
		makeEnemy(275, -35, 0, aimedShot, 0, 100, 225, -30, 360, 50, 2);
		makeEnemy(300, -25, 0, aimedShot, 0, 110, 220, -30, 370, 50, 2);
		makeEnemy(325, -20, 0, aimedShot, 0, 120, 225, -30, 380, 50, 2);
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

	if (leveltime == 3500) {
		makeEnemy(0, 200, 0, verticalShot, 4, 1000, 200, 1000, 200, 200, 3);
	}

	if (leveltime == 3700) {
		makeEnemy(900, 200, 0, verticalShot, 4, -20, 250, -20, 250, 200, 3);
	}

	if (leveltime == 3900) {
		makeEnemy(-180, 150, 0, verticalShot, 4, 1000, 200, 1000, 200, 200, 3);
		makeEnemy(-180, 200, 0, verticalShot, 4, 1000, 200, 1000, 200, 200, 3);
		makeEnemy(980, 250, 0, verticalShot, 4, -20, 250, -10, 200, 200, 3);
		makeEnemy(980, 300, 0, verticalShot, 4, -20, 300, -10, 300, 200, 3);
	}

	if (leveltime == 4050) {
		makeEnemy(200, -10, 0, kaguya, 1, 200, 250, 250, -10, 250, 10);
		makeEnemy(500, -10, 0, kaguya, 1, 500, 450, 250, -10, 250, 10);
		makeEnemy(-300, 50, 0, verticalShot, 4, 1000, 50, 1000, 50, 200, 3);
		makeEnemy(-200, 100, 0, verticalShot, 4, 1100, 100, 1000, 100, 200, 3);
		makeEnemy(-100, 150, 0, verticalShot, 4, 1200, 150, 1000, 150, 200, 3);
		makeEnemy(0, 200, 0, verticalShot, 4, 1300, 200, 1000, 200, 200, 3);
	}

	if (leveltime == 4200) {
		makeEnemy(450, -10, 0, fairy, 2, 450, 350, 400, -10, 500, 10);
		makeEnemy(250, -10, 0, fairy, 2, 200, 200, 400, -10, 500, 10);
		makeEnemy(980, 20, 0, verticalShot, 4, -320, 20, -40, 75, 200, 3);
		makeEnemy(1080, 140, 0, verticalShot, 4, -220, 140, -30, 125, 200, 3);
		makeEnemy(1180, 260, 0, verticalShot, 4, -120, 260, -20, 175, 200, 3);
		makeEnemy(1280, 380, 0, verticalShot, 4, -20, 380, -10, 225, 200, 3);
	}

	if (leveltime == 4900) {
		makeEnemy(-20, 550, 0, aimedShot, 0, 1000, 550, 1000, 550, 200, 3);
		makeEnemy(-120, -10, 0, verticalShot, 4, 1000, 500, 1000, 200, 200, 3);
		makeEnemy(820, -10, 0, verticalShot, 4, -100, 500, -20, 200, 200, 3);		
	}

	if (leveltime == 5100) {
		makeEnemy(1000, 50, 0, aimedShot, 0, -20, 50, -20, 50, 200, 3);
		makeEnemy(-120, 620, 0, verticalShot, 4, 1000, 100, 1000, 200, 200, 3);
		makeEnemy(820, 620, 0, verticalShot, 4, -100, 100, -20, 200, 200, 3);
	}

	if (leveltime == 5350) {
		makeEnemy(-20, 50, 0, spreadShot, 3, 1000, 50, 1000, 50, 200, 3);
		makeEnemy(-120, -10, 0, verticalShot, 4, 1000, 500, 1000, 200, 200, 3);
		makeEnemy(820, -10, 0, verticalShot, 4, -100, 500, -20, 200, 200, 3);	
		makeEnemy(-120, 620, 0, verticalShot, 4, 1000, 100, 1000, 200, 200, 3);
		makeEnemy(820, 620, 0, verticalShot, 4, -100, 100, -20, 200, 200, 3);
	}

	if (leveltime >= 5700 && leveltime < 5760) {
		for (int i = 5700, j = 0; i < 5760; i+=20, j+=20) {
			if (leveltime == i) {
				makeEnemy(10, -10, 0, spreadShot, 3, 10, 1000, 10, 1000, 200, 4);
				makeEnemy(700, 650, 0, spreadShot, 3, 700, -20, 700, -20, 200, 4);
			}
		}
	}

	if (leveltime == 5700) {
		makeEnemy(-20, 50, 0, verticalShot, 4, 1000, 50, 1000, 50, 200, 3);
		makeEnemy(820, 550, 0, verticalShot, 4, -20, 550, -20, 550, 200, 3);	
	}

	if (leveltime == 6300) {
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

	if (leveltime == 7000) {
		makeEnemy(0, -10, 0, boss, -1, 350, 200, 0, 0, 20000, 2);
	}

	/*for (int i = 0; i < MAX_BULLETS; i++) {
		if (enemyBullets[i].isActive())
			gameSprites->Draw(bulletTexture, &laser, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
		else gameSprites->Draw(bulletTexture, &greenBullet, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
	}*/

	/*if (leveltime >= 200 && leveltime < 240) {
		for (int i = 200, j = 0; i < 240; i+=10, j+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, fairy, 2, 300, 15*j, 100, -10, 500, 2);
			}
		}
	}*/
}