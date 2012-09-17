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
			if (GetAsyncKeyState(VK_DOWN)) 
				playerPos.y += 5;
			if (GetAsyncKeyState(VK_UP)) 
				playerPos.y -= 5;
			if (GetAsyncKeyState(VK_LEFT)) 
				playerPos.x -= 5;
			if (GetAsyncKeyState(VK_RIGHT))
				playerPos.x += 5;
			if (GetAsyncKeyState('Z')) 
				playerPos.z -= 5;
			if (GetAsyncKeyState('A'))
				playerPos.z += 5;
			if (GetAsyncKeyState(VK_SPACE)) {
				for (int i = 0; i < 100; i++) {
					if (!playerBullets[i].isActive()) {
						playerBullets[i].setActive(true);
						playerBullets[i].setPos(playerPos.x, playerPos.y, playerPos.z);
						break;
					}
				}
			}
			if (GetAsyncKeyState(VK_ESCAPE)) {
				screen = 0;
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
			// draw player
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			gameSprites->Draw(gameTexture, &player, NULL, &playerPos, 0xFFFFFFFF);

			// draw player bullets
			for (int i = 0; i < 100; i++) {
				if (playerBullets[i].isActive()) {
					gameSprites->Draw(laserTexture, &laser, NULL, &playerBullets[i].getPos(), 0xFFFFFFFF);
					playerBullets[i].move(0,-10,0); 
					if (playerBullets[i].getPos(1) < 0)
						playerBullets[i].setActive(false);
				}
			}

			// draw enemy bullets
			D3DXVECTOR3 moveRate;
			D3DXMATRIX rotation;
			D3DXMATRIX translation1, translation2;
			double angle; 
			int startX, startY;
			for (int i = 0; i < 1000; i++) {
				if (enemyBullets[i].isActive()) {
					angle = atan(moveRate.y/moveRate.x);
					D3DXMatrixTranslation(&translation1,-1*enemyBullets[i].getPos(0),-1*enemyBullets[i].getPos(1),0);
					D3DXMatrixRotationZ(&rotation, angle+PI/2);
					D3DXMatrixTranslation(&translation2,enemyBullets[i].getPos(0),enemyBullets[i].getPos(1),0);
					D3DXMatrixMultiply(&spriteManip, &translation1, &rotation);
					D3DXMatrixMultiply(&spriteManip, &spriteManip, &translation2);
					gameSprites->SetTransform(&spriteManip);
					gameSprites->Draw(laserTexture, &laser, NULL, &enemyBullets[i].getPos(), 0xFFFFFFFF);
					moveRate = enemyBullets[i].getTarget() - enemyBullets[i].getStartPos();
					D3DXVec3Normalize(&moveRate, &moveRate);
					enemyBullets[i].move(moveRate.x*3, moveRate.y*3, moveRate.z*3); 
					if (enemyBullets[i].getPos(1) > 600)
						enemyBullets[i].setActive(false);
					D3DXMatrixIdentity(&spriteManip);
					gameSprites->SetTransform(&spriteManip);
				}
			}

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
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("balloon.jpg"), &gameTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("laser.jpg"), &laserTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	playerPos.x = SCREEN_WIDTH/2; playerPos.y = SCREEN_HEIGHT*8/10; playerPos.z = 0;
	setRects();
	playerBullets = new Bullet[100];
	enemyBullets = new Bullet[1000];
	enemies = new Enemy[100];
}

void Game::setRects() {
	player.left=0;
	player.right=28;
	player.top=0;
	player.bottom=43;
	laser.left=0;
	laser.right=12;
	laser.top=0;
	laser.bottom=33;
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

void Game::level1Script() {
// events
	if (leveltime == 150) {
		enemies[0].setPos(50,-20, 0);
		enemies[1].setPos(100, -22, 0);
		enemies[2].setPos(125, -30, 0);
	}
	if (leveltime > 150) {
		eventType1(0, 3, 300, kaguya);
	}
	if (leveltime == 300) {
		enemies[3].setPos(650,-40, 0);
		enemies[4].setPos(670, -35, 0);
		enemies[5].setPos(700, -25, 0);
		enemies[6].setPos(725, -20, 0);
	}
	if (leveltime > 300) {
		eventType1(3, 7, 225, bucket);
	}
	if (leveltime == 300) {
		enemies[3].setPos(650,-40, 0);
		enemies[4].setPos(670, -35, 0);
		enemies[5].setPos(700, -25, 0);
		enemies[6].setPos(725, -20, 0);
	}
	if (leveltime > 300) {
		eventType1(3, 7, 225, bucket);
	}
	if (leveltime == 350) {
		enemies[7].setPos(200,-40, 0);
		enemies[8].setPos(300, -35, 0);
		enemies[9].setPos(400, -25, 0);
		enemies[10].setPos(500, -20, 0);
	}
	if (leveltime > 350) {
		eventType1(7, 11, 225, bucket);
	}
	if (leveltime == 800) {
		enemies[7].setPos(350, -30, 0);
	}
	if (leveltime > 800) {
		eventType1(7, 8, 50, fairy);
	}
}

void Game::eventType1(int start, int end, int dest, RECT sprite) {
	moves.x = 0; moves.z = 0;
	for (int i = start; i < end; i++) {
		int action = enemies[i].getAction();
		if (enemies[i].getPos(1) >= 0) {
			gameSprites->Draw(gameTexture, &sprite, NULL, &enemies[i].getPos(), 0xFFFFFFFF);
		}
		switch(action) {
			case 0: {
				moves.y = 5;
				enemies[i].moveTo(dest - 2*i, moves);
				if (enemies[i].getCooldown() <= 0) {
					enemies[i].aimFire(enemyBullets, playerPos, 1000, i);
					enemies[i].setCooldown(3);
				}
				else enemies[i].setCooldown(enemies[i].getCooldown() - 1);
				if (enemies[i].getPos(1) >= dest - 2*i)
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
				enemies[i].moveTo(-5, moves);
				break;
			}
		}
	}
}