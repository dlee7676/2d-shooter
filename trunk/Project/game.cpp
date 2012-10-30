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
			if (GetAsyncKeyState(VK_DOWN) && !exploding && playerPos.y + moveRate < SCREEN_HEIGHT) 
				playerPos.y += moveRate;
			if (GetAsyncKeyState(VK_UP) && !exploding && playerPos.y - moveRate > 0) 
				playerPos.y -= moveRate;
			if (GetAsyncKeyState(VK_LEFT) && !exploding && playerPos.x - moveRate > 0) 
				playerPos.x -= moveRate;
			if (GetAsyncKeyState(VK_RIGHT) && !exploding && playerPos.x + moveRate < SCREEN_WIDTH)
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
								playerBullets[i].init(playerPos.x-10+num*10, playerPos.y, playerPos.z, laser, 0);
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
				enemiesList.clear();
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
				explosionAnim.left = 0;
				explosionAnim.top = 0;
				explosionAnim.right = 80;
				explosionAnim.bottom = 80;
				/*explosionAnim.left = curFrame * 80;
				explosionAnim.top = curRow * 80;
				explosionAnim.right = explosionAnim.left + 80;
				explosionAnim.bottom = explosionAnim.top + 80;*/
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
				gameSprites->Draw(bulletTexture, &laser, NULL, &D3DXVECTOR3(playerBox.left + playerPos.x + 7, playerBox.top + playerPos.y + 14, 0), 0xFFFFFFFF);
			}

			// draw player bullets
			drawPlayerBullets();

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
	cooldown = 0;
	playerBullets = new Bullet[1000];
	enemyBullets = new Bullet[1000];
	enemies = new Enemy[100];
	vector<Enemy> enemiesList; 
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
	purpleBullet.top = 18;
	purpleBullet.right = 78; 
	purpleBullet.bottom = 38;
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
	for (int i = 0; i < enemiesList.size(); i++) {
		if (enemiesList[i].isActive())
			break;
		if (i == enemiesList.size()-1) {
			enemiesList.clear();
		}
	}
	if (leveltime == 150) {
		makeEnemy(150, -25, 0, bucket, 0, 20, 215, -30, 300, 50);
		makeEnemy(175, -35, 0, bucket, 0, 40, 225, -30, 300, 50);
		makeEnemy(200, -25, 0, bucket, 0, 60, 220, -30, 300, 50);
		makeEnemy(225, -20, 0, bucket, 0, 70, 225, -30, 300, 50);
		makeEnemy(250, -40, 0, bucket, 0, 85, 218, -30, 300, 50);
		makeEnemy(275, -35, 0, bucket, 0, 100, 225, -30, 300, 50);
		makeEnemy(300, -25, 0, bucket, 0, 120, 220, -30, 300, 50);
		makeEnemy(325, -20, 0, bucket, 0, 130, 225, -30, 300, 50);
	}

	if (leveltime == 400) {
		makeEnemy(550,-40, 0, bucket, 0, 600, 215, SCREEN_WIDTH, 300, 50);
		makeEnemy(575, -35, 0, bucket, 0, 610, 225, SCREEN_WIDTH, 300, 50);
		makeEnemy(600, -25, 0, bucket, 0,  620, 220, SCREEN_WIDTH, 300, 50);
		makeEnemy(625, -20, 0, bucket, 0, 630, 225, SCREEN_WIDTH, 300, 50);
		makeEnemy(650,-40, 0, bucket, 0, 640, 218, SCREEN_WIDTH, 300, 50);
		makeEnemy(670, -35, 0, bucket, 0, 650, 225, SCREEN_WIDTH, 300, 50);
		makeEnemy(700, -25, 0, bucket, 0,  660, 220, SCREEN_WIDTH, 300, 50);
		makeEnemy(725, -20, 0, bucket, 0, 670, 225, SCREEN_WIDTH, 300, 50);
		
	}

	if (leveltime == 100) {
		makeEnemy(200,-40, 0, bucket, 0, 300, 300, -30, 500, 50);
		makeEnemy(300, -35, 0, bucket, 0, 300, 300, -30, 500, 50);
		makeEnemy(400, -25, 0, bucket, 0, 300, 300, 1000, 500, 50);
		makeEnemy(500, -20, 0, bucket, 0, 300, 300, 1000, 500, 50);
		makeEnemy(SCREEN_WIDTH/2 - 200, -20, 0, kaguya, 1, SCREEN_WIDTH/2 - 200, 300, SCREEN_WIDTH/2 - 200, -20, 100);
		makeEnemy(SCREEN_WIDTH/2 - 100, -20, 0, kaguya, 1, SCREEN_WIDTH/2 - 100, 300, SCREEN_WIDTH/2 - 100, -20, 100);
		makeEnemy(SCREEN_WIDTH/2 + 100, -20, 0, kaguya, 1, SCREEN_WIDTH/2 + 100, 300, SCREEN_WIDTH/2 + 100, -20, 100);
	}
	
	if (leveltime >= 700 && leveltime < 730) {
		for (int i = 700; i < 730; i+=10) {
			if (leveltime == i) {
				makeEnemy(0, 200, 0, bucket, 0, 1000, 250, 1000, 200, 75);
				makeEnemy(900, 200, 0, bucket, 0, -100, 250, -100, 200, 75);
			}
		}
	}

	if (leveltime >= 850 && leveltime < 900) {
		for (int i = 850; i < 900; i+=10) {
			if (leveltime == i) {
				makeEnemy(0, 200, 0, bucket, 3, 1000, 250, 1000, 200, 75);
				makeEnemy(900, 200, 0, bucket, 3, -100, 250, -100, 200, 75);
			}
		}
	}

	if (leveltime == 900) {
		makeEnemy(350, -30, 0, fairy, 2, 300, 130, 0, 0, 1000);
		makeEnemy(350, -30, 0, fairy, 2, 400, 130, SCREEN_WIDTH, 0, 1000);
	}

	if (leveltime == 960) {
		makeEnemy(250, -25, 0, bucket, 3, 20, 215, -30, 300, 50);
		makeEnemy(275, -35, 0, bucket, 3, 40, 225, -30, 300, 50);
		makeEnemy(300, -25, 0, bucket, 3, 60, 220, -30, 300, 50);
		makeEnemy(325, -20, 0, bucket, 3, 70, 225, -30, 300, 50);
		makeEnemy(350, -40, 0, bucket, 3, 85, 218, -30, 300, 50);
		makeEnemy(375, -35, 0, bucket, 3, 100, 225, -30, 300, 50);
		makeEnemy(400, -25, 0, bucket, 3, 120, 220, -30, 300, 50);
		makeEnemy(425, -20, 0, bucket, 3, 130, 225, -30, 300, 50);
	}

	moveEnemies2();
}

void Game::drawPlayerBullets() {
	for (int i = 0; i < 1000; i++) {
		if (playerBullets[i].isActive()) {
			if (playerBullets[i].isExploding()) {
				explosionAnim.left = 0;
				explosionAnim.top = 0;
				explosionAnim.right = 80;
				explosionAnim.bottom = 80;
				D3DXMatrixTranslation(&translation1,-1*playerBullets[i].getPos(0),-1*playerBullets[i].getPos(1),0);
				D3DXMatrixScaling(&scaling, 0.5, 0.5, 1);
				D3DXMatrixTranslation(&translation2,playerBullets[i].getPos(0),playerBullets[i].getPos(1),0);
				D3DXMatrixMultiply(&spriteManip, &translation1, &scaling);
				D3DXMatrixMultiply(&spriteManip, &spriteManip, &translation2);
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
				D3DXMatrixTranslation(&translation1,-1*playerBullets[i].getPos(0),-1*playerBullets[i].getPos(1),0);
				D3DXMatrixScaling(&scaling, 0.75, 0.9, 1);
				D3DXMatrixTranslation(&translation2,playerBullets[i].getPos(0),playerBullets[i].getPos(1),0);
				D3DXMatrixMultiply(&spriteManip, &translation1, &scaling);
				D3DXMatrixMultiply(&spriteManip, &spriteManip, &translation2);
				gameSprites->SetTransform(&spriteManip);
				gameSprites->Draw(bulletTexture, &laser, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
				playerBullets[i].move(0,-15,0); 
			}
			D3DXMatrixIdentity(&spriteManip);
			D3DXMatrixIdentity(&rotation);
			D3DXMatrixIdentity(&translation1);
			D3DXMatrixIdentity(&translation2);
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
			if (enemyBullets[i].inBounds(playerBox, playerPos.x + 14, playerPos.y + 21) && !exploding) {
				exploding = true;
				enemyBullets[i].setActive(false);
			}

			moveRate = enemyBullets[i].getTarget() - enemyBullets[i].getStartPos();
			D3DXVec3Normalize(&moveRate, &moveRate);

			angle = atan(moveRate.y/moveRate.x);
			D3DXMatrixTranslation(&translation1,-1*enemyBullets[i].getPos(0),-1*enemyBullets[i].getPos(1),0);
			if (enemyBullets[i].getStartPos().x < enemyBullets[i].getTarget().x)
				D3DXMatrixRotationZ(&rotation, angle + PI/4);
			else 
				D3DXMatrixRotationZ(&rotation, angle - PI/4);
			D3DXMatrixTranslation(&translation2,enemyBullets[i].getPos(0),enemyBullets[i].getPos(1),0);
			D3DXMatrixMultiply(&spriteManip, &translation1, &rotation);
			D3DXMatrixMultiply(&spriteManip, &spriteManip, &translation2);
			bulletType = enemyBullets[i].getType();
			if (bulletType != 2)
				gameSprites->SetTransform(&spriteManip);
			switch(bulletType) {
				case 0: {
					gameSprites->Draw(bulletTexture, &greenBullet, NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
					break;
				}
				case 1: {
					gameSprites->Draw(bulletTexture, &purpleBullet, NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
					break;
				}
				case 2: {
					gameSprites->Draw(bulletTexture, &redBall, NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
					break;
				}
			}
			
			enemyBullets[i].move(moveRate.x*3, moveRate.y*3, moveRate.z*3); 
			if (enemyBullets[i].getPos(1) > 620 || enemyBullets[i].getPos(1) < -10)
				enemyBullets[i].setActive(false);
			D3DXMatrixIdentity(&spriteManip);
			D3DXMatrixIdentity(&rotation);
			D3DXMatrixIdentity(&translation1);
			D3DXMatrixIdentity(&translation2);
			gameSprites->SetTransform(&spriteManip);
		}
	}
}

void Game::moveEnemies2() {
	moves.x = 0; moves.z = 0;
	for (int i = 0; i < enemiesList.size(); i++) {
		int action = enemiesList[i].getAction();
		if (enemiesList[i].isActive()) {
			//if (enemiesList[i].getPos(1) < 0) 
			//enemiesList[i].setAction(0);
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
				switch(enemiesList[i].getType()) {
					case 0: {
						gameSprites->Draw(gameTexture, &bucket, NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
						break;
					}
					case 1: {
						gameSprites->Draw(gameTexture, &kaguya, NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
						break;
					}
					case 2: {
						gameSprites->Draw(gameTexture, &fairy, NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
						break;
					}
					case 3: {
						gameSprites->Draw(gameTexture, &fairy, NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
						break;
					}
				}
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

			// move enemies
			switch(action) {
				case 0: {
					moves.y = 5;
					D3DXVec3CatmullRom(&enemyPos, &D3DXVECTOR3(enemiesList[i].getStartPos().x, enemiesList[i].getStartPos().y, 0),
												&D3DXVECTOR3(enemiesList[i].getStartPos().x, enemiesList[i].getStartPos().y, 0),
												&D3DXVECTOR3(enemiesList[i].getMid().x + 25*(i%5), enemiesList[i].getMid().y, 0),
												&D3DXVECTOR3(enemiesList[i].getMid().x + 25*(i%5), enemiesList[i].getMid().y, 0), enemiesList[i].getS());
					enemiesList[i].setS(enemiesList[i].getS() + 0.005);
					enemiesList[i].setPos(enemyPos.x, enemyPos.y, enemyPos.z);
					enemiesList[i].setWaitTime(0);
					if (enemiesList[i].getCooldown() <= 0) {
						if (enemiesList[i].getType() == 0)
							enemiesList[i].aimFire(enemyBullets, playerPos, 1000, i, calcHitbox(greenBullet), 0);
						if (enemiesList[i].getType() == 1)
							enemiesList[i].aimFire(enemyBullets, playerPos, 1000, i, calcHitbox(purpleBullet), 1);
						if (enemiesList[i].getType() == 3) {
							D3DXVECTOR3 shot = playerPos;
							for (int j = 0; j < 20; j++) {
								shot = rotateVector(shot, PI/6, 1);
								enemiesList[i].aimFire(enemyBullets, shot, 1000, i, calcHitbox(purpleBullet), 1);
								enemiesList[i].setCooldown(50);
							}
						}
						if (enemiesList[i].getType() != 3)
							enemiesList[i].setCooldown(20);
					}
					else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
					if (enemiesList[i].getPos(0) >= enemiesList[i].getMid().x - 20 && enemiesList[i].getPos(0) <= enemiesList[i].getMid().x + 20
						&& enemiesList[i].getPos(1) >= enemiesList[i].getMid().y - 20 && enemiesList[i].getPos(1) <= enemiesList[i].getMid().y + 20)
						enemiesList[i].setAction(1);
					break;
				}
				case 1: {
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
								enemiesList[i].aimFire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0), shot.y+enemiesList[i].getPos(1), shot.z), 1000, i, redBall, 2);
								enemiesList[i].setCooldown(10);
							}
							else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
							//break;
						}
						enemiesList[i].wait();
						if (enemiesList[i].getWaitTime() >= 100)
							enemiesList[i].setAction(2);
					}
					else if (enemiesList[i].getType() == 2) {
						if (enemiesList[i].getCooldown() <= 0) {
							shot = rotateVector(shot, PI/6, 1);
							for (int j = 0; j < 4; j++) {	
								enemiesList[i].aimFire(enemyBullets, shot, 1000, i, calcHitbox(redBall), 2);
								shot = rotateVector(shot, PI/12, 0);											
								enemiesList[i].setCooldown(5);	
							}
						}
						else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
						enemiesList[i].wait();
						if (enemiesList[i].getWaitTime() >= 300)
							enemiesList[i].setAction(2);
					}
					else enemiesList[i].setAction(2);
					break;
				}
				case 2: {
					moves.y = -5;
					if (enemiesList[i].getCooldown() <= 0) {
						if (enemiesList[i].getType() == 1)
							enemiesList[i].aimFire(enemyBullets, playerPos, 1000, i, calcHitbox(purpleBullet), 1);
						else enemiesList[i].aimFire(enemyBullets, playerPos, 1000, i, calcHitbox(greenBullet), 0);
						enemiesList[i].setCooldown(20);
					}
					else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);
					D3DXVec3CatmullRom(&enemyPos, &D3DXVECTOR3(enemiesList[i].getEnd().x, enemiesList[i].getEnd().y, 0),
												&D3DXVECTOR3(enemiesList[i].getEnd().x, enemiesList[i].getEnd().y, 0),
												&D3DXVECTOR3(enemiesList[i].getMid().x + 25*(i%5), enemiesList[i].getMid().y, 0),
												&D3DXVECTOR3(enemiesList[i].getMid().x + 25*(i%5), enemiesList[i].getMid().y, 0), enemiesList[i].getS());
					enemiesList[i].setS(enemiesList[i].getS() - 0.005);
					enemiesList[i].setPos(enemyPos.x, enemyPos.y, enemyPos.z);
					if (enemiesList[i].getPos(0) < 0 || enemiesList[i].getPos(1) < 0 || enemiesList[i].getPos(0) > SCREEN_WIDTH || enemiesList[i].getPos(1) > SCREEN_HEIGHT) {
   						enemiesList[i].setActive(false);
					}
					break;
				}
			}
		}
	}
}

void Game::makeEnemy(int x, int y, int z, RECT bounds, int type, int midX, int midY, int endX, int endY, int life) {
	Enemy next;
	enemiesList.push_back(next);
	enemiesList.back().init(x, y, z, bounds, type, midX, midY, endX, endY, life);
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
