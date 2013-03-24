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
	if (GetAsyncKeyState(VK_ESCAPE)) {
		screen = 0;
		//curLevel.init(&enemiesList);
		//delete levelBackgroundTexture;
		enemiesList.clear();
		//subunits.clear();
		initMenuScreen();
	}
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
				playerObject.setSpeed(3);
			else playerObject.setSpeed(6);
			//if (focus)
				
			//else moveRate = 6;
			if (GetAsyncKeyState(VK_DOWN) && (!playerObject.isExploding()) && playerObject.getPos(1) + playerObject.getSpeed() + 32 < SCREEN_HEIGHT) 
				playerObject.setPos(1, (playerObject.getPos(1) + playerObject.getSpeed()));
			if (GetAsyncKeyState(VK_UP) && (!playerObject.isExploding()) && playerObject.getPos(1) - playerObject.getSpeed() > 0) 
				playerObject.setPos(1, (playerObject.getPos(1) - playerObject.getSpeed()));
			if (GetAsyncKeyState(VK_LEFT) && (!playerObject.isExploding()) && playerObject.getPos(0) - playerObject.getSpeed() > 0) 
				playerObject.setPos(0, (playerObject.getPos(0) - playerObject.getSpeed()));
			if (GetAsyncKeyState(VK_RIGHT) && (!playerObject.isExploding()) && playerObject.getPos(0) + playerObject.getSpeed() + 22 < SCREEN_WIDTH)
				playerObject.setPos(0, (playerObject.getPos(0) + playerObject.getSpeed()));
			/*if (GetAsyncKeyState('Z')) 
				playerObject.getPos().z -= 5;
			if (GetAsyncKeyState('A'))
				playerObject.getPos().z += 5;*/
			if (GetAsyncKeyState(VK_SPACE) && (!playerObject.isExploding())) {
				for (int num = 0; num < 6; num++) {
					for (int i = 0; i < MAX_BULLETS; i++) {
						//if (cooldown <= 0) {
							if (!playerBullets[i].isActive()) {
								playerBullets[i].setActive(true);
 								playerBullets[i].init(playerObject.getPos(0)-20+num*10, playerObject.getPos(1), 0, drawBoundaries["laser"], num, 1);
								break;
							}
							//cooldown = 1;
						//}
						//else cooldown--;
					}
				}
				for (int num = 0; num < 4; num++) {
					for (int i = 0; i < MAX_BULLETS; i++) {
						//if (cooldown <= 0) {
							if (!playerBullets[i].isActive()) {
								playerBullets[i].setActive(true);
 								playerBullets[i].init(playerObject.getPos(0)-10+num*10, playerObject.getPos(1), 0, drawBoundaries["laser"], 3, 1);
								break;
							}
							//cooldown = 1;
						//}
						//else cooldown--;
					}
				}
			}
			if (GetAsyncKeyState(VK_RETURN)) {

				if(curLevel.isClear())
					screen = 2;
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
			// draw background
			scrollBackground();	
			//draw player
			drawPlayer();
			//draw particles
			sceneryParticles();
			// draw player bullets
			drawPlayerBullets();
			// draw enemy bullets
			drawEnemyBullets();
			// add any new enemies
			curLevel.level1Script();
			// draw enemies
			drawEnemy();
			checkEnemyHits();
			// move enemies
			moveEnemies();
			refreshEnemies();
			gameSprites->End();

			drawTitle();
			wchar_t buffer[16];
			wsprintf(buffer, TEXT("%d"), hits);
			wstring s(TEXT("Hits taken: "));
			s += wstring(buffer);
			//font->DrawText(NULL, s.c_str(), -1, &topRight, 0, fontColor2); 
			curLevel.incrementTime();
			if (curLevel.isClear()) {
				font->DrawText(NULL, TEXT("Stage Clear!"), -1, &levelText, 0, fontColor2); 
				font->DrawText(NULL, s.c_str(), -1, &subText1, 0, fontColor2);
			}
			break;
		}

		case 2: {
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			scrollBackground();
			gameSprites->End();
			font->DrawText(NULL, TEXT("Add further stages here"), -1, &levelText, 0, fontColor2); 
			break;
		}
	}
	pDev->EndScene();
	pDev->Present(NULL, NULL, NULL, NULL);
}

void Game::cleanup() {
	d3d->Release();
	pDev->Release();
	gameSprites->Release();
	menuBackgroundTexture->Release();
}

/*------------------*/

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
	if (curLevel.getTime() >= 0 && curLevel.getTime() < 240) {
		levelText.left = SCREEN_WIDTH/2-100;
		levelText.right = SCREEN_WIDTH/2+200;
		subText1.left = SCREEN_WIDTH/2-100;
		subText1.right = SCREEN_WIDTH/2+200;
		for (int i = 0; i < 60; i++) {
			if (curLevel.getTime() == i)  {
				curAlpha += 3;
				fontColor = D3DCOLOR_ARGB(curAlpha,200,200,255); 
			}
		}
		for (int i = 160; i < 240; i++) {
			if (curLevel.getTime() == i && curAlpha-3 >= 0)  {
				curAlpha -= 3;
				fontColor = D3DCOLOR_ARGB(curAlpha,200,200,255); 
			}
		}
		font->DrawText(NULL, TEXT("- Stage 1 -"), -1, &levelText, 0, fontColor);
	}
	else fontColor = fontColor = D3DCOLOR_ARGB(255,200,200,255); 
}

void Game::initMenuScreen() {
	// screen 0
	if (FAILED(D3DXCreateSprite(pDev, &gameSprites))) {
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
}

void Game::initLevel1() {
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
	//curLevel.getTime() = 0;
	fireDirection = 0;
	srand(time(0));
	offset = 0;
	fontColor2 = D3DCOLOR_ARGB(255,240,240,240);
	curFrame = 0;
	curRow = 0;
	curAlpha = 50;
	currentT = 0;
	hits = 0;
	levelText.top = 250;
	levelText.bottom = 300;
	subText1.top = 310;
	subText1.bottom = 360;
	playerBullets = new Bullet[MAX_BULLETS];
	enemyBullets = new Bullet[MAX_BULLETS];
	playerObject.init(SCREEN_WIDTH/2, SCREEN_HEIGHT*8/10, 0, drawBoundaries["playerBox"], 0, 6);
	curLevel.init(&enemiesList);
	drawBoundaries = curLevel.getBoundaries();
}

void Game::scrollBackground() {
	offset++;
	if (offset >= 1200)
		offset = 0;
	D3DXVECTOR3 bgPos(0,0,0);
	bgTop.left = 0;
	bgTop.right = SCREEN_WIDTH;
	bgTop.top = 1200 - offset;
	bgTop.bottom = 1200;
	gameSprites->Draw(levelBackgroundTexture, &bgTop, NULL, &bgPos, 0xFFFFFFFF);
	bgBottom.left = 0;
	bgBottom.right = SCREEN_WIDTH;
	bgBottom.top = 0;
	bgBottom.bottom = SCREEN_HEIGHT - offset;
	bgPos.y = offset;
	gameSprites->Draw(levelBackgroundTexture, &bgBottom, NULL, &bgPos, 0xFFFFFFFF);
}

void Game::sceneryParticles() {
	if (curLevel.getTime()%50 == 0) {
		for (int i = 0; i < 10; i++) {
			if (rand()%2 == 0)
				particleHandler.emit(D3DXVECTOR3(rand()%800, rand()%600, 0), rand()%1, rand()%200+150, (rand()%2+1)*0.75, 
					D3DXVECTOR3((rand()%2-1)*0.3, (rand()%2+2)*0.3, 0));
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
				gameSprites->Draw(enemyTexture, &drawBoundaries["smallGreenParticle"], NULL, &display[i].getPos(), currentColor);
			else gameSprites->Draw(enemyTexture, &drawBoundaries["bigGreenParticle"], NULL, &display[i].getPos(), currentColor);
		}
	}
	particleHandler.update();
}

void Game::drawPlayer() {
	if (playerObject.isExploding()) {
		drawExplosion.left = curFrame*100;
		drawExplosion.top = curRow*100+5*curRow;
		drawExplosion.right = drawExplosion.left+100;
		drawExplosion.bottom = drawExplosion.top+100-5*curRow;
		gameSprites->Draw(explosionTexture, &drawExplosion, NULL, &playerObject.getPos(), 0xFFFFFFFF);
		playerObject.setAnimTime(playerObject.getAnimTime()-1);
		if (playerObject.getAnimTime() <= 0) {
			curFrame++;
			playerObject.setAnimTime(6);
		}
		if (curFrame > 4 && curRow > 4) {
			//playerObject.getPos(0) = SCREEN_WIDTH/2; 
			//playerObject.getPos(1) = SCREEN_HEIGHT*8/10;
			playerObject.setExploding(false);
			curFrame = 0;
			curRow = 0;
			playerObject.setAnimTime(100);
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
		if ((invincible && curLevel.getTime()%2 == 0) || !invincible) 
			gameSprites->Draw(enemyTexture, &drawBoundaries["player"], NULL, &playerObject.getPos(), 0xFFFFFFFF);
	}
	if (playerObject.getAnimTime() > 5) {
		playerObject.setAnimTime(playerObject.getAnimTime()-1);
		if (playerObject.getAnimTime() <= 5)
			invincible = false;
	}
}

void Game::drawPlayerBullets() {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (playerBullets[i].isActive()) {
			if (playerBullets[i].isExploding()) {
				drawExplosion.left = 0;
				drawExplosion.top = 0;
				drawExplosion.right = 80;
				drawExplosion.bottom = 80;
				spriteManip = scale(translation1, translation2, playerBullets[i].getPos(0), playerBullets[i].getPos(1), scaling, 0.5, 0.5);
				gameSprites->SetTransform(&spriteManip);
				gameSprites->Draw(explosionTexture, &drawExplosion, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
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
				gameSprites->Draw(bulletTexture, &drawBoundaries["laser"], NULL, &playerBullets[i].getPos(), 0xB1FFFFFF);
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
			if (enemyBullets[i].inBounds(drawBoundaries["playerBox"], playerObject.getPos(0) + 10, playerObject.getPos(1) + 5) && (!playerObject.isExploding()) && !invincible) {
				playerObject.setExploding(true);
				hits++;
				enemyBullets[i].setActive(false);
			}
			moveRate = enemyBullets[i].getTarget() - enemyBullets[i].getStartPos();
			D3DXVec3Normalize(&moveRate, &moveRate);
			angle = atan(moveRate.y/moveRate.x);
			if (enemyBullets[i].getType() == 0)
			rotateBullets(angle, i);
			gameSprites->SetTransform(&spriteManip);
			gameSprites->Draw(bulletTexture, &enemyBullets[i].getInitialBounds(), NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
			enemyBullets[i].move(moveRate.x*enemyBullets[i].getSpeed(), moveRate.y*enemyBullets[i].getSpeed(), 0); 
			if (enemyBullets[i].getType() == 3)
				chaosSpiral(i, 0.01f);
			if (enemyBullets[i].getType() == 2)
				moveSpiral(i, 0.01f, 15.0f);
			if (enemyBullets[i].getPos(1) > 620 || enemyBullets[i].getPos(1) < 0 || enemyBullets[i].getPos(0) < 0 || enemyBullets[i].getPos(0) > SCREEN_WIDTH)
				enemyBullets[i].setActive(false);
			if (curLevel.isClear())
				enemyBullets[i].setActive(false);
			resetMatrices();
			gameSprites->SetTransform(&spriteManip);
		}
	}
}

void Game::drawEnemy() {
	// draw enemies
	for (int i = 0; i < enemiesList.size(); i++) {
		if (enemiesList[i].isActive()) {
			if (enemiesList[i].isExploding()) {
				drawExplosion.left = 0;
				drawExplosion.top = 0;
				drawExplosion.right = 80;
				drawExplosion.bottom = 80;
				gameSprites->Draw(explosionTexture, &drawExplosion, NULL, &enemiesList[i].getPos(), 0xFFFFFFFF);
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
		}
	}
}

void Game::checkEnemyHits() {
	for (int i = 0; i < enemiesList.size(); i++) {
		if (enemiesList[i].getPos(1) >= 0) { 
			for (int j = 0; j < MAX_BULLETS; j++) {
				if (enemiesList[i].inBounds(playerBullets[j]) && playerBullets[j].isActive() && !playerBullets[j].isExploding() 
					&& enemiesList[i].getPos(1) > 0 && !enemiesList[i].isExploding()) {
					if (enemiesList[i].getLife() <= 0) {
						enemiesList[i].setExploding(true);
					}
					else enemiesList[i].setLife(enemiesList[i].getLife()-1);
					playerBullets[j].setExploding(true);
				}
			}
		}
	}
}

void Game::refreshEnemies() {
	for (int i = 0; i < enemiesList.size(); i++) {
		if (enemiesList[i].isActive())
			break;
		if (i == enemiesList.size()-1) {
			enemiesList.clear();
		}
	}
}

void Game::moveEnemies() {
	//moves.x = 0; moves.z = 0;
	playerTarget = D3DXVECTOR3(playerObject.getPos(0)+14, playerObject.getPos(1)+21, 0);
	D3DXVECTOR3 shot = playerObject.getPos();
	for (int i = 0; i < enemiesList.size(); i++) {
		int action = enemiesList[i].getAction();
		if (enemiesList[i].isActive()) {
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
					//moves.y = -5;
					if (enemiesList[i].getCooldown() <= 0) {
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
					enemiesList[i].setSpeed(2);
					enemiesList[i].bossPattern(100, curLevel.getTime());
					curLevel.boss1Actions(enemyBullets, i);
				}
			}
		}
	}
}

void Game::advance(int i) {
	//moves.y = 5;
	playerTarget = D3DXVECTOR3(playerObject.getPos(0)+14, playerObject.getPos(1)+21, 0);
	D3DXVECTOR3 shot = playerObject.getPos();
	enemiesList[i].moveTo(1);
	if (enemiesList[i].getCooldown() <= 0) {
		if (enemiesList[i].getType() == 0)
			enemiesList[i].fire(enemyBullets, playerTarget, enemiesList[i].getPos(), MAX_BULLETS, drawBoundaries["greenBullet"], 0, 3);
		if (enemiesList[i].getType() == 1)
			enemiesList[i].fire(enemyBullets, playerTarget, enemiesList[i].getPos(), MAX_BULLETS, drawBoundaries["purpleBullet"], 0, 3);
		if (enemiesList[i].getType() == 3) {
			for (int j = 0; j < 5; j++) {
				enemiesList[i].fire(enemyBullets, shot, enemiesList[i].getPos(), MAX_BULLETS, drawBoundaries["purpleBullet"], 0, 3);
				shot = rotateVector(shot, PI/6, 1);
				enemiesList[i].setCooldown(50);
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
					enemiesList[i].fire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1)+10, shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1)+10, 0), MAX_BULLETS, drawBoundaries["redBall"], 1, 3);
					enemiesList[i].setCooldown(6);
				}
			}
			else enemiesList[i].setCooldown(enemiesList[i].getCooldown()-1);
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
	D3DXVECTOR3 shot = playerObject.getPos();
	shot.z = 0;
	if (enemiesList[i].getType() == 1) {
		if (enemiesList[i].getCooldown() <= 0) {
			for (int j = 0; j < 8; j++) {
				shot = enemiesList[i].aim8Ways(j);
				enemiesList[i].fire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1)+10, shot.z), 
					D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1)+10, 0), MAX_BULLETS, drawBoundaries["redBall"], 1, 3);
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
		if (enemiesList[i].getCooldown() <= 0) {
			shot = D3DXVECTOR3(0,1,0); 
			shot = rotateVector(shot, PI/4, 1);
			for (int j = 0; j < 2; j++) {	
				enemiesList[i].fire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1), shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1), 0), MAX_BULLETS, drawBoundaries["largeGreen"], 1, 2);
				shot = rotateVector(shot, PI/4, 0);
				/*if (j == 0) {
					shot.x = -1*sqrt(3.0f);
					shot.y = 1;
					enemiesList[i].fire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1), shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1), 0), MAX_BULLETS, drawBoundaries["largeGreen"], 1, 3);
				}
				if (j == 1) {
					shot.x = 0;
					shot.y = 1;
					enemiesList[i].fire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0)+10, shot.y+enemiesList[i].getPos(1), shot.z), 
						D3DXVECTOR3(enemiesList[i].getPos(0)+10, enemiesList[i].getPos(1), 0), MAX_BULLETS, drawBoundaries["largeGreen"], 1, 3);
				}
				if (j == 2) {
					shot.x = sqrt(3.0f);
					shot.y = 1;
					enemiesList[i].fire(enemyBullets, D3DXVECTOR3(shot.x+enemiesList[i].getPos(0), shot.y+enemiesList[i].getPos(1), shot.z), 
						enemiesList[i].getPos(), MAX_BULLETS, drawBoundaries["largeGreen"], 1, 3);
				}*/		
			}
			enemiesList[i].setCooldown(35);	
		}
		else enemiesList[i].setCooldown(enemiesList[i].getCooldown() - 1);		
		enemiesList[i].wait();
		if (enemiesList[i].getWaitTime() >= 300)
			enemiesList[i].setAction(2);
	}
	else enemiesList[i].setAction(2);
}

void Game::moveSpiral(int i, float s_, float initS) {
	double x, y, s;
	if (enemyBullets[i].isActive()) {
		s = enemyBullets[i].getS();
		if (s == 0)
			enemyBullets[i].setS(initS);
		if (i%2 == 0) {
			x = s*cos(s);
			y = s*sin(s);
		}
		else {
			x = -s*cos(s);
			y = -s*sin(s);
		}
		enemyBullets[i].setPos(0.1*x+enemyBullets[i].getPos(0), 0.1*y+enemyBullets[i].getPos(1), 0);
		enemyBullets[i].setS(enemyBullets[i].getS()+s_);
	}
}

void Game::chaosSpiral(int i, float s_) {
	double x, y, s;
	if (enemyBullets[i].isActive()) {
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
		enemyBullets[i].setS(enemyBullets[i].getS()+s_);
	}
}

void Game::makeEnemy(int x, int y, int z, RECT bounds, int type, int midX, int midY, int endX, int endY, int life, int speed) {
	Enemy next;
	enemiesList.push_back(next);
	enemiesList.back().init(x, y, z, bounds, type, midX, midY, endX, endY, life, speed);
}


/* ---------------- */


