#include "level.h"

Level::Level() {};

/* void init(vector<Enemy>* pEnemies_, size_t level): initializes an uncleared level with time 0.
	Parameters:
	vector<Enemy>* pEnemies_: pointer to a vector of enemies that will be modified in the level.
	size_t level: the stage to run events for. */

void Level::init(vector<Enemy>* pEnemies_, size_t level) {
	pEnemies = pEnemies_;
	leveltime = 0;
	setRects();
	clear = false;
}

/* void incrementTime(): increases the level time by 1. */
void Level::incrementTime() {
	leveltime++;
}

/* int getTime(): returns the current level time. */
int Level::getTime() {
	return leveltime;
}

/* map<string, RECT> getBoundaries(): returns a map of all RECTs to be used in this level. */
map<string,RECT> Level::getBoundaries() {
	return boundaries;
}

/* void makeEnemy(int x, int y, int z, RECT bounds, int type, int midX, int midY, int endX, int endY, int life, int speed):
	Adds a new enemy to the enemy vector for the level, and sets its characteristics.
	Parameters:
	int x, y, z: The coordinates to place the new enemy at.
	RECT bounds: The bounding box to use for the enemy.
	int type: The type of enemy to add. 
		0 = aims shots directly at the player
		1 = fires bullets 8 ways
		2 = fires 3 large bullets
		3 = fires a spread of bullets aimed at the player
		4 = fires bullets upward and downward
		-1 = level 1 boss
	int midX, midY: Coordinates for the first control point in a spline determining the enemy's movement.
	int endX, endY: The end point of the spline (the final destination point).
	int life: The amount of damage the enemy can take.
	float speed: A higher value makes the enemy move faster. */

void Level::makeEnemy(float x, float y, float z, RECT bounds, int type, float midX, float midY, float endX, float endY, float life, float speed) {
	Enemy next;
	pEnemies->push_back(next);
	pEnemies->back().init(x, y, z, bounds, type, midX, midY, endX, endY, life, speed);
}

/* bool isClear(): returns whether the stage is clear or not. */
bool Level::isClear() {
	return clear;
}

/* void setRects(): adds necessary bounding rectangle information for the level to a map. */
void Level::setRects() {
	RECT player;
	player.left = 405;
	player.top = 1988;
	player.right = 430;
	player.bottom = 2050;
	boundaries["player"] = player;
	RECT playerBox;
	playerBox.left = 0;
	playerBox.right=3;
	playerBox.top = 0;
	playerBox.bottom=3;
	boundaries["playerBox"] = playerBox;
	RECT kaguya;
	kaguya.left=454;
	kaguya.top=1360;
	kaguya.right=500;
	kaguya.bottom=1420;
	boundaries ["kaguya"] = kaguya;
	RECT aimedShot;
	aimedShot.left=281; 
	aimedShot.top=1235;
	aimedShot.right=330; 
	aimedShot.bottom=1290;
	boundaries["aimedShot"] = aimedShot;
	RECT largeShot;
	largeShot.left=172; 
	largeShot.top=310;
	largeShot.right=219; 
	largeShot.bottom=371;
	boundaries["largeShot"] = largeShot;
	RECT spreadShot;
	spreadShot.left=454;
	spreadShot.top=380;
	spreadShot.right=500;
	spreadShot.bottom=435;
	boundaries["spreadShot"] = spreadShot;
	RECT boss;
	boss.left=0;
	boss.top=1050;
	boss.right=55;
	boss.bottom=1115;
	boundaries["boss"] = boss;
	RECT verticalShot;
	verticalShot.left=105;
	verticalShot.top=930;
	verticalShot.right=172;
	verticalShot.bottom=990;
	boundaries["verticalShot"] = verticalShot;
	RECT laser;
	laser.left=127;
	laser.right=143;
	laser.top=0;
	laser.bottom=16;
	boundaries["laser"] = laser;
	RECT greenBullet;
	greenBullet.left = 159;
	greenBullet.right = 175;
	greenBullet.top = 22;
	greenBullet.bottom = 40;
	boundaries["greenBullet"] = greenBullet;
	RECT redBall;
	redBall.left = 30; 
	redBall.top = 40;
	redBall.right = 48; 
	redBall.bottom = 62;
	boundaries["redBall"] = redBall;
	RECT blueBall;
	blueBall.left = 130;
	blueBall.top = 40;
	blueBall.right = 145;
	blueBall.bottom = 65;
	boundaries["blueBall"] = blueBall;
	RECT greenBall;
	greenBall.left = 177;
	greenBall.top = 40;
	greenBall.right = 193;
	greenBall.bottom = 65;
	boundaries["greenBall"] = greenBall;
	RECT yellowBall;
	yellowBall.left = 208;
	yellowBall.top = 62;
	yellowBall.right = 225;
	yellowBall.bottom = 82;
	boundaries["yellowBall"] = yellowBall;
	RECT orangeBall;
	orangeBall.left = 225;
	orangeBall.top = 62;
	orangeBall.right = 240;
	orangeBall.bottom = 82;
	boundaries["orangeBall"] = orangeBall;
	RECT purpleBall;
	purpleBall.left = 63;
	purpleBall.top = 62;
	purpleBall.right = 78;
	purpleBall.bottom = 82;
	boundaries["purpleBall"] = purpleBall;
	RECT purpleBullet;
	purpleBullet.left = 62; 
	purpleBullet.top = 20;
	purpleBullet.right = 78; 
	purpleBullet.bottom = 38;
	boundaries["purpleBullet"] = purpleBullet;
	RECT smallGreenParticle;
	smallGreenParticle.left = 282;
	smallGreenParticle.top = 1980;
	smallGreenParticle.right = 322;
	smallGreenParticle.bottom = 2056;
	boundaries["smallGreenParticle"] = smallGreenParticle;
	RECT bigGreenParticle;
	bigGreenParticle.left = 333;
	bigGreenParticle.top = 1988;
	bigGreenParticle.right = 377;
	bigGreenParticle.bottom = 2056;
	boundaries["bigGreenParticle"] = bigGreenParticle;
	RECT topRight;
	topRight.left = 550;
	topRight.right = 800;
	topRight.top = 0;
	topRight.bottom = 40;
	boundaries["topRight"] = topRight;
	RECT largeGreen;
	largeGreen.left = 415;
	largeGreen.top = 45;
	largeGreen.right = 445;
	largeGreen.bottom = 80;
	boundaries["largeGreen"] = largeGreen;
	RECT yellowStar;
	yellowStar.left = 210;
	yellowStar.top = 215;
	yellowStar.right = 225;
	yellowStar.bottom = 232;
	boundaries["yellowStar"] = yellowStar;
}

/* void runLevel(int level_): performs the set of events specific to a given level.
	Parameters:
	int level_: the current stage. */

void Level::runLevel(int level_) {
	switch(level_) {
		case 1: {
			level1Script();
		}
		default: {
			break;
		}
	}
}

/* void level1Script(): makes enemies at the appropriate times and places for level 1. */
void Level::level1Script() {
// waves of enemies are initialized at specified times in the level
	if (leveltime == 250) {
		makeEnemy(150, -25, 0, boundaries["aimedShot"], 0, 50, 215, -30, 300, 50, 6);
		makeEnemy(175, -35, 0, boundaries["aimedShot"], 0, 60, 225, -30, 310, 50, 6);
		makeEnemy(200, -25, 0, boundaries["aimedShot"], 0, 70, 220, -30, 320, 50, 6);
		makeEnemy(225, -20, 0, boundaries["aimedShot"], 0, 80, 225, -30, 330, 50, 6);
		makeEnemy(225, -20, 0, boundaries["kaguya"], 1, 150, 150, -30, 340, 250, 7);
		makeEnemy(250, -40, 0, boundaries["aimedShot"], 0, 95, 218, -30, 350, 50, 6);
		makeEnemy(275, -35, 0, boundaries["aimedShot"], 0, 100, 225, -30, 360, 50, 6);
		makeEnemy(300, -25, 0, boundaries["aimedShot"], 0, 110, 220, -30, 370, 50, 6);
		makeEnemy(325, -20, 0, boundaries["aimedShot"], 0, 120, 225, -30, 380, 50, 6);
	}

	if (leveltime == 450) {
		makeEnemy(550,-40, 0, boundaries["aimedShot"], 0, 550, 215, SCREEN_WIDTH, 300, 50, 6);
		makeEnemy(575, -35, 0, boundaries["aimedShot"], 0, 560, 225, SCREEN_WIDTH, 310, 50, 6);
		makeEnemy(600, -25, 0, boundaries["aimedShot"], 0,  570, 220, SCREEN_WIDTH, 320, 50, 6);
		makeEnemy(625, -20, 0, boundaries["aimedShot"], 0, 580, 225, SCREEN_WIDTH, 330, 50, 6);
		makeEnemy(625, -20, 0, boundaries["kaguya"], 1, 500, 150, SCREEN_WIDTH, 340, 250, 7);
		makeEnemy(650, -40, 0, boundaries["aimedShot"], 0, 600, 218, SCREEN_WIDTH, 350, 50, 6);
		makeEnemy(670, -35, 0, boundaries["aimedShot"], 0, 610, 225, SCREEN_WIDTH, 360, 50, 6);
		makeEnemy(700, -25, 0, boundaries["aimedShot"], 0,  620, 220, SCREEN_WIDTH, 370, 50, 6);
		makeEnemy(725, -20, 0, boundaries["aimedShot"], 0, 630, 225, SCREEN_WIDTH, 380, 50, 6);
	}

	if (leveltime == 700) {
		makeEnemy(SCREEN_WIDTH/2 - 200, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 - 200, 250, SCREEN_WIDTH/2 - 200, -20, 250, 7);
		makeEnemy(SCREEN_WIDTH/2 - 100, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 - 100, 175, SCREEN_WIDTH/2 - 100, -20, 250, 7);
		makeEnemy(SCREEN_WIDTH/2 + 100, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 + 100, 250, SCREEN_WIDTH/2 + 100, -20, 250, 7);
	}

	if (leveltime == 750) {
		makeEnemy(200,-40, 0, boundaries["aimedShot"], 0, 300, 300, -30, 500, 50, 6);
		makeEnemy(300, -35, 0, boundaries["aimedShot"], 0, 300, 300, -30, 510, 50, 6);
		makeEnemy(400, -25, 0, boundaries["aimedShot"], 0, 300, 300, 1000, 490, 50, 6);
		makeEnemy(500, -20, 0, boundaries["aimedShot"], 0, 300, 300, 1000, 480, 50, 6);
		makeEnemy(100,-40, 0, boundaries["aimedShot"], 0, 300, 300, -30, 520, 50, 6);
		makeEnemy(600, -35, 0, boundaries["aimedShot"], 0, 300, 300, -30, 530, 50, 6);
		makeEnemy(700, -25, 0, boundaries["aimedShot"], 0, 300, 300, 1000, 470, 50, 6);
		makeEnemy(800, -20, 0, boundaries["aimedShot"], 0, 300, 300, 1000, 460, 50, 6);
		makeEnemy(205,-40, 0, boundaries["aimedShot"], 0, 305, 300, -30, 540, 50, 6);
		makeEnemy(305, -35, 0, boundaries["aimedShot"], 0, 305, 300, -30, 550, 50, 6);
		makeEnemy(405, -25, 0, boundaries["aimedShot"], 0, 305, 300, 1000, 560, 50, 6);
		makeEnemy(505, -20, 0, boundaries["aimedShot"], 0, 305, 300, 1000, 450, 50, 6);
		makeEnemy(105,-40, 0, boundaries["aimedShot"], 0, 305, 300, -30, 440, 50, 6);
		makeEnemy(605, -35, 0, boundaries["aimedShot"], 0, 305, 300, -30, 430, 50, 6);
		makeEnemy(705, -25, 0, boundaries["aimedShot"], 0, 305, 300, 1000, 420, 50, 6);
		makeEnemy(805, -20, 0, boundaries["aimedShot"], 0, 305, 300, 1000, 500, 50, 6);
	}

	if (leveltime == 1000) {
		makeEnemy(550,-40, 0, boundaries["aimedShot"], 0, 600, 215, SCREEN_WIDTH, 400, 50, 6);
		makeEnemy(575, -35, 0, boundaries["aimedShot"], 0, 610, 225, SCREEN_WIDTH, 390, 50, 6);
		makeEnemy(600, -25, 0, boundaries["aimedShot"], 0,  620, 220, SCREEN_WIDTH, 380, 50, 6);
		makeEnemy(625, -20, 0, boundaries["aimedShot"], 0, 630, 225, SCREEN_WIDTH, 370, 50, 6);
		makeEnemy(650, -40, 0, boundaries["aimedShot"], 0, 640, 218, SCREEN_WIDTH, 410, 50, 6);
		makeEnemy(670, -35, 0, boundaries["aimedShot"], 0, 650, 225, SCREEN_WIDTH, 420, 50, 6);
		makeEnemy(700, -25, 0, boundaries["aimedShot"], 0,  660, 220, SCREEN_WIDTH, 430, 50, 6);
		makeEnemy(725, -20, 0, boundaries["aimedShot"], 0, 670, 225, SCREEN_WIDTH, 440, 50, 6);
	}

	if (leveltime >= 1020 && leveltime < 1100) {
		for (int i = 1020; i < 1100; i+=10) {
			if (leveltime == i) {
				makeEnemy(0, 200, 0, boundaries["spreadShot"], 3, 1000, 250, -100, 200, 25, 6);
			}
		}
	}

	if (leveltime == 1250) {
		makeEnemy(150, -25, 0, boundaries["aimedShot"], 0, 50, 215, -30, 400, 50, 6);
 		makeEnemy(175, -35, 0, boundaries["aimedShot"], 0, 60, 225, -30, 390, 50, 6);
		makeEnemy(200, -25, 0, boundaries["aimedShot"], 0, 70, 220, -30, 380, 50, 6);
		makeEnemy(225, -20, 0, boundaries["aimedShot"], 0, 80, 225, -30, 370, 50, 6);
		makeEnemy(250, -40, 0, boundaries["aimedShot"], 0, 95, 218, -30, 410, 50, 6);
		makeEnemy(275, -35, 0, boundaries["aimedShot"], 0, 100, 225, -30, 420, 50, 6);
		makeEnemy(300, -25, 0, boundaries["aimedShot"], 0, 110, 220, -30, 430, 50, 6);
		makeEnemy(325, -20, 0, boundaries["aimedShot"], 0, 120, 225, -30, 440, 50, 6);
	}

	if (leveltime >= 1260 && leveltime < 1340) {
		for (int i = 1260; i < 1340; i+=10) {
			if (leveltime == i) {
				makeEnemy(900, 200, 0, boundaries["spreadShot"], 3, -100, 250, 1000, 200, 25, 6);
			}
		}
	}

	if (leveltime >= 1500 && leveltime < 1550) {
		for (int i = 1500; i < 1550; i+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, boundaries["aimedShot"], 0, 100, 250, -10, 350, 25, 6);
				makeEnemy(600, -10, 0, boundaries["aimedShot"], 0, 600, 250, 1000, 350, 25, 6);
			}
		}
	}

	if (leveltime == 1550) {
		makeEnemy(300, -30, 0, boundaries["largeShot"], 2, 300, 230, SCREEN_WIDTH, 0, 350, 6);
	}

	if (leveltime >= 1700 && leveltime < 1750) {
		for (int i = 1700; i < 1750; i+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, boundaries["spreadShot"], 3, 100, 250, -10, 400, 25, 6);
				makeEnemy(600, -10, 0, boundaries["spreadShot"], 3, 600, 250, 1000, 400, 25, 6);
			}
		}
	}

	if (leveltime == 1700) {
		makeEnemy(140, -30, 0, boundaries["largeShot"], 2, 200, 230, -20, -20, 350, 6);
		makeEnemy(300, -30, 0, boundaries["largeShot"], 2, 300, 230, SCREEN_WIDTH, 0, 350, 6);
		makeEnemy(500, -30, 0, boundaries["largeShot"], 2, 450, 230, SCREEN_WIDTH, 0, 350, 6);
	}

	if (leveltime == 1950) {
		makeEnemy(240, -30, 0, boundaries["largeShot"], 2, 200, 130, -20, -20, 350, 6);
		makeEnemy(300, -30, 0, boundaries["largeShot"], 2, 300, 130, SCREEN_WIDTH, 0, 350, 6);
		makeEnemy(400, -30, 0, boundaries["largeShot"], 2, 450, 130, SCREEN_WIDTH, 0, 350, 6);
	}

	if (leveltime >= 2050 && leveltime < 2100) {
		for (int i = 2050; i < 2100; i+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, boundaries["aimedShot"], 0, 100, 250, -10, 300, 25, 6);
				makeEnemy(600, -10, 0, boundaries["aimedShot"], 0, 600, 250, 1000, 300, 25, 6);
			}
		}
	}

	if (leveltime == 2150) {
		makeEnemy(140, -30, 0, boundaries["largeShot"], 2, 200, 170, -20, -20, 350, 6);
		makeEnemy(300, -30, 0, boundaries["largeShot"], 2, 300, 170, SCREEN_WIDTH, 0, 350, 6);
		makeEnemy(500, -30, 0, boundaries["largeShot"], 2, 450, 170, SCREEN_WIDTH, 0, 350, 6);
	}

	if (leveltime == 2350) {
		makeEnemy(240, -30, 0, boundaries["largeShot"], 2, 200, 190, -20, -20, 350, 6);
		makeEnemy(320, -30, 0, boundaries["largeShot"], 2, 300, 190, SCREEN_WIDTH, 0, 350, 6);
		makeEnemy(400, -30, 0, boundaries["largeShot"], 2, 450, 190, SCREEN_WIDTH, 0, 350, 6);
		makeEnemy(700, -30, 0, boundaries["largeShot"], 2, 450, 190, SCREEN_WIDTH, 0, 350, 6);
	}

	if (leveltime >= 2300 && leveltime < 2400) {
		for (int i = 2300; i < 2400; i+=10) {
			if (leveltime == i) {
				makeEnemy(100, -10, 0, boundaries["spreadShot"], 3, 100, 250, -10, 300, 25, 6);
				makeEnemy(600, -10, 0, boundaries["spreadShot"], 3, 600, 250, 1000, 300, 25, 6);
			}
		}
	}

	if (leveltime >= 2600 && leveltime < 2650) {
		for (int i = 2600, j = 0; i < 2650; i+=10, j+=10) {
			if (leveltime == i) {
				makeEnemy(600, -10, 0, boundaries["largeShot"], 2, 600, (float)(15*j), 600, -10, 500, 6);
			}
		}
	}

	if (leveltime >= 2750 && leveltime < 2800) {
		for (int i = 2750, j = 0; i < 2800; i+=10, j+=10) 
			if (leveltime == i) 
				makeEnemy(100, -10, 0, boundaries["spreadShot"], 3, 100, 250, -10, 300, 25, 4);
	}


	if (leveltime >= 2900 && leveltime < 2950) {
		for (int i = 2900, j = 0; i < 2950; i+=10, j+=10) 
			if (leveltime == i) 
				makeEnemy(100, -10, 0, boundaries["largeShot"], 2, 100, (float)(15*j), 100, -10, 500, 6);
	}

	if (leveltime >= 3050 && leveltime < 3100) {
		for (int i = 3050, j = 0; i < 3100; i+=10, j+=10) 
			if (leveltime == i) 
				makeEnemy(700, -10, 0, boundaries["spreadShot"], 3, 700, 250, 1000, 300, 25, 4);
	}

	if (leveltime == 3500) {
		makeEnemy(500, -10, 0, boundaries["aimedShot"], 0, 500, 450, 250, -10, 250, 10);
		makeEnemy(0, 200, 0, boundaries["verticalShot"], 4, 1000, 200, 1000, 200, 200, 3);
	}

	if (leveltime == 3700) {
		makeEnemy(200, -10, 0, boundaries["aimedShot"], 0, 200, 250, 250, -10, 250, 10);
		makeEnemy(900, 200, 0, boundaries["verticalShot"], 4, -20, 250, -20, 250, 200, 3);
	}

	if (leveltime == 3900) {
		makeEnemy(-180, 150, 0, boundaries["verticalShot"], 4, 1000, 200, 1000, 200, 200, 3);
		makeEnemy(-180, 200, 0, boundaries["verticalShot"], 4, 1000, 200, 1000, 200, 200, 3);
		makeEnemy(980, 250, 0, boundaries["verticalShot"], 4, -20, 250, -10, 200, 200, 3);
		makeEnemy(980, 300, 0, boundaries["verticalShot"], 4, -20, 300, -10, 300, 200, 3);
	}

	if (leveltime == 4200) {
		makeEnemy(200, -10, 0, boundaries["aimedShot"], 0, 200, 250, 250, -10, 250, 10);
		makeEnemy(500, -10, 0, boundaries["aimedShot"], 0, 500, 450, 250, -10, 250, 10);
		makeEnemy(-300, 50, 0, boundaries["verticalShot"], 4, 1000, 50, 1000, 50, 200, 3);
		makeEnemy(-200, 100, 0, boundaries["verticalShot"], 4, 1100, 100, 1000, 100, 200, 3);
		makeEnemy(-100, 150, 0, boundaries["verticalShot"], 4, 1200, 150, 1000, 150, 200, 3);
		makeEnemy(0, 200, 0, boundaries["verticalShot"], 4, 1300, 200, 1000, 200, 200, 3);
	}


	if (leveltime == 4250) {
		makeEnemy(1300, 50, 0, boundaries["verticalShot"], 4, 0, 50, 1000, 50, 200, 3);
		makeEnemy(1200, 100, 0, boundaries["verticalShot"], 4, -100, 100, 1000, 100, 200, 3);
		makeEnemy(1100, 150, 0, boundaries["verticalShot"], 4, -200, 150, 1000, 150, 200, 3);
		makeEnemy(1000, 200, 0, boundaries["verticalShot"], 4, -300, 200, 1000, 200, 200, 3);
	}

	if (leveltime == 4450) {
		makeEnemy(450, -10, 0, boundaries["largeShot"], 2, 450, 350, 400, -10, 500, 10);
		makeEnemy(250, -10, 0, boundaries["largeShot"], 2, 200, 200, 400, -10, 500, 10);
		makeEnemy(980, 20, 0, boundaries["verticalShot"], 4, -320, 20, -40, 75, 200, 3);
		makeEnemy(1080, 140, 0, boundaries["verticalShot"], 4, -220, 140, -30, 125, 200, 3);
		makeEnemy(1180, 260, 0, boundaries["verticalShot"], 4, -120, 260, -20, 175, 200, 3);
		makeEnemy(1280, 380, 0, boundaries["verticalShot"], 4, -20, 380, -10, 225, 200, 3);
	}

	if (leveltime == 4900) {
		makeEnemy(-20, 550, 0, boundaries["aimedShot"], 0, 1000, 550, 1000, 550, 200, 3);
		makeEnemy(-120, -10, 0, boundaries["verticalShot"], 4, 1000, 500, 1000, 200, 200, 3);
		makeEnemy(820, -10, 0, boundaries["verticalShot"], 4, -100, 500, -20, 200, 200, 3);		
	}

	if (leveltime == 5100) {
		makeEnemy(1000, 50, 0, boundaries["aimedShot"], 0, -20, 50, -20, 50, 200, 3);
		makeEnemy(-120, 620, 0, boundaries["verticalShot"], 4, 1000, 100, 1000, 200, 200, 3);
		makeEnemy(820, 620, 0, boundaries["verticalShot"], 4, -100, 100, -20, 200, 200, 3);
	}

	if (leveltime == 5350) {
		makeEnemy(-20, 50, 0, boundaries["spreadShot"], 3, 1000, 50, 1000, 50, 200, 3);
		makeEnemy(-120, -10, 0, boundaries["verticalShot"], 4, 1000, 500, 1000, 200, 200, 3);
		makeEnemy(820, -10, 0, boundaries["verticalShot"], 4, -100, 500, -20, 200, 200, 3);	
		makeEnemy(-120, 620, 0, boundaries["verticalShot"], 4, 1000, 100, 1000, 200, 200, 3);
		makeEnemy(820, 620, 0, boundaries["verticalShot"], 4, -100, 100, -20, 200, 200, 3);
	}

	if (leveltime >= 5700 && leveltime < 5760) {
		for (int i = 5700, j = 0; i < 5760; i+=20, j+=20) {
			if (leveltime == i) {
				makeEnemy(10, -10, 0, boundaries["spreadShot"], 3, 10, 1000, 10, 1000, 200, 4);
				makeEnemy(700, 650, 0, boundaries["spreadShot"], 3, 700, -20, 700, -20, 200, 4);
			}
		}
	}

	if (leveltime == 5700) {
		makeEnemy(-20, 50, 0, boundaries["verticalShot"], 4, 1000, 50, 1000, 50, 200, 3);
		makeEnemy(820, 550, 0, boundaries["verticalShot"], 4, -20, 550, -20, 550, 200, 3);	
	}

	if (leveltime == 6350) {
		makeEnemy(200,-40, 0, boundaries["aimedShot"], 0, 300, 300, -30, 500, 50, 5);
		makeEnemy(300, -35, 0, boundaries["aimedShot"], 0, 300, 300, -30, 500, 50, 5);
		makeEnemy(400, -25, 0, boundaries["aimedShot"], 0, 300, 300, 1000, 500, 50, 5);
		makeEnemy(500, -20, 0, boundaries["aimedShot"], 0, 300, 300, 1000, 500, 50, 5);
		makeEnemy(100,-40, 0, boundaries["aimedShot"], 0, 300, 300, -30, 500, 50, 5);
		makeEnemy(600, -35, 0, boundaries["aimedShot"], 0, 300, 300, -30, 500, 50, 5);
		makeEnemy(700, -25, 0, boundaries["aimedShot"], 0, 300, 300, 1000, 500, 50, 5);
		makeEnemy(800, -20, 0, boundaries["aimedShot"], 0, 300, 300, 1000, 500, 50, 5);
		makeEnemy(205,-40, 0, boundaries["aimedShot"], 0, 305, 300, -30, 500, 50, 5);
		makeEnemy(305, -35, 0, boundaries["aimedShot"], 0, 305, 300, -30, 500, 50, 5);
		makeEnemy(405, -25, 0, boundaries["aimedShot"], 0, 305, 300, 1000, 500, 50, 5);
		makeEnemy(505, -20, 0, boundaries["aimedShot"], 0, 305, 300, 1000, 500, 50, 5);
		makeEnemy(105,-40, 0, boundaries["aimedShot"], 0, 305, 300, -30, 500, 50, 5);
		makeEnemy(605, -35, 0, boundaries["aimedShot"], 0, 305, 300, -30, 500, 50, 5);
		makeEnemy(705, -25, 0, boundaries["aimedShot"], 0, 305, 300, 1000, 500, 50, 5);
		makeEnemy(805, -20, 0, boundaries["aimedShot"], 0, 305, 300, 1000, 500, 50, 5);

		makeEnemy(200,-40, 0, boundaries["spreadShot"], 3, 300, 300, -30, 500, 50, 5);
		makeEnemy(300, -35, 0, boundaries["spreadShot"], 3, 300, 300, -30, 500, 50, 5);
		makeEnemy(400, -25, 0, boundaries["spreadShot"], 3, 300, 300, 1000, 500, 50, 5);
		makeEnemy(500, -20, 0, boundaries["spreadShot"], 3, 300, 300, 1000, 500, 50, 5);
		makeEnemy(100,-40, 0, boundaries["spreadShot"], 3, 300, 300, -30, 500, 50, 5);
		makeEnemy(600, -35, 0, boundaries["spreadShot"], 3, 300, 300, -30, 500, 50, 5);
		makeEnemy(700, -25, 0, boundaries["spreadShot"], 3, 300, 300, 1000, 500, 50, 5);
		makeEnemy(800, -20, 0, boundaries["spreadShot"], 3, 300, 300, 1000, 500, 50, 5);
		makeEnemy(205,-40, 0, boundaries["spreadShot"], 3, 305, 300, -30, 500, 50, 5);
		makeEnemy(305, -35, 0, boundaries["spreadShot"], 3, 305, 300, -30, 500, 50, 5);
		makeEnemy(405, -25, 0, boundaries["spreadShot"], 3, 305, 300, 1000, 500, 50, 5);
		makeEnemy(505, -20, 0, boundaries["spreadShot"], 3, 305, 300, 1000, 500, 50, 5);
		makeEnemy(105,-40, 0, boundaries["spreadShot"], 3, 305, 300, -30, 500, 50, 5);
		makeEnemy(605, -35, 0, boundaries["spreadShot"], 3, 305, 300, -30, 500, 50, 5);
		makeEnemy(705, -25, 0, boundaries["spreadShot"], 3, 305, 300, 1000, 500, 50, 5);
		makeEnemy(805, -20, 0, boundaries["spreadShot"], 3, 305, 300, 1000, 500, 50, 5);

		makeEnemy(SCREEN_WIDTH/2 - 200, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 - 200, 250, SCREEN_WIDTH/2 - 200, -20, 250, 7);
		makeEnemy(SCREEN_WIDTH/2 - 100, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 - 100, 250, SCREEN_WIDTH/2 - 100, -20, 250, 7);
		makeEnemy(SCREEN_WIDTH/2 + 100, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 + 100, 250, SCREEN_WIDTH/2 + 100, -20, 250, 7);
		makeEnemy(SCREEN_WIDTH/2 - 300, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 - 300, 250, SCREEN_WIDTH/2 - 200, -20, 250, 7);
		makeEnemy(SCREEN_WIDTH/2 - 150, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 - 150, 250, SCREEN_WIDTH/2 - 100, -20, 250, 7);
		makeEnemy(SCREEN_WIDTH/2 + 200, -20, 0, boundaries["kaguya"], 1, SCREEN_WIDTH/2 + 200, 250, SCREEN_WIDTH/2 + 100, -20, 250, 7);
	}

	if (leveltime == 7000) {
		makeEnemy(0, -10, 0, boundaries["boss"], -1, 350, 200, 0, 0, 25000, 6);
		bool newCard = false;
		for(int i=0; i < 2; i++)
			attackTypes.push_back(newCard);
	}
}
/* void boss1Actions(vector<Bullet>* enemyBullets, size_t i): behaviour pattern for the level 1 boss.
	Parameters: 
	vector<Bullet>* enemyBullets: a vector of bullets for the enemy to fire.
	size_t i: the index of the enemy to perform this behaviour. */
void Level::boss1Actions(vector<Bullet>* enemyBullets, size_t i) {
	D3DXVECTOR3 newTarget;
	// set the level to be clear if the boss is at 0 life
	if ((*pEnemies)[i].getLife() <= 0) {
		clear = true;
	}
	// fire erratically moving bullets (type 3) at less than 4000 life
	if ((*pEnemies)[i].getLife() < 4000) {
		if ((*pEnemies)[i].getCooldown() <= 0) {
			newTarget = (*pEnemies)[i].rotateAim((*pEnemies)[i].getRotateDirection(), PI/(4+rand()%12), 3.0f);
			// alternate the colours of the bullets
			if (leveltime%6 == 0)
				(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
					D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["blueBall"], 3, 3);
			else if (leveltime%5 == 0)
				(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
					D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["purpleBall"], 3, 3);
			else if (leveltime%4 == 0)
				(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
					D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["redBall"], 3, 3);
			else if (leveltime%3 == 0)
				(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
					D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["orangeBall"], 3, 3);
			else if (leveltime%2 == 0)
				(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
					D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["yellowBall"], 3, 3);
			else 
				(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
					D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["greenBall"], 3, 3);
			(*pEnemies)[i].setCooldown(2);
		}
		(*pEnemies)[i].setCooldown((*pEnemies)[i].getCooldown()-1);
	}
	// fire bullets that move in spirals (type 2) at less than 9000 life
	else if ((*pEnemies)[i].getLife() < 9000) {
		if ((*pEnemies)[i].getCooldown() <= 0) {
			newTarget = (*pEnemies)[i].rotateAim((*pEnemies)[i].getRotateDirection(), PI/6, 3.0f);
			(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
				D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["purpleBall"], 2, 1.5);
			(*pEnemies)[i].setCooldown(3);
		}
		(*pEnemies)[i].setCooldown((*pEnemies)[i].getCooldown()-1);
	}
	// spawn bullets in a spiral pattern at less than 13000 life
	else if ((*pEnemies)[i].getLife() < 13000) {
		if ((*pEnemies)[i].getCooldown() <= 0) {
			for (int j = 0; j < 8; j++) {
				D3DXVECTOR3 shot = (*pEnemies)[i].aim8Ways(j);
				D3DXVECTOR3 spiralPos = (*pEnemies)[i].fireSpiral((*pEnemies)[i].getT());
				if (j%2 == 0)
					(*pEnemies)[i].fire(enemyBullets,D3DXVECTOR3(shot.x+spiralPos.x+(*pEnemies)[i].getPos(0)+15, shot.y+spiralPos.y+(*pEnemies)[i].getPos(1)+15, 0), 
						D3DXVECTOR3(spiralPos.x+(*pEnemies)[i].getPos(0)+15, spiralPos.y+(*pEnemies)[i].getPos(1)+15, 0), enemyBullets->size(), boundaries["greenBall"], 4, 3);
				else (*pEnemies)[i].fire(enemyBullets,D3DXVECTOR3(shot.x+spiralPos.x+(*pEnemies)[i].getPos(0)+15, shot.y+spiralPos.y+(*pEnemies)[i].getPos(1)+15, 0), 
					D3DXVECTOR3(spiralPos.x+(*pEnemies)[i].getPos(0)+15, spiralPos.y+(*pEnemies)[i].getPos(1)+15, 0), enemyBullets->size(), boundaries["yellowBall"], 4, 3);
			}
			(*pEnemies)[i].setT((*pEnemies)[i].getT()+0.5f);
			(*pEnemies)[i].setCooldown(10);
		}
		if ((*pEnemies)[i].getT() > 100) {
			(*pEnemies)[i].setCooldown(100);
			(*pEnemies)[i].setT(25);
		}
		(*pEnemies)[i].setCooldown((*pEnemies)[i].getCooldown()-1);
	}
	// add a new set of bullets to fire at less than 17000 life
	else if ((*pEnemies)[i].getLife() <= 17000 && !attackTypes[1]) {
		attackTypes[1] = true;
		(*pEnemies)[i].setTargeting(D3DXVECTOR3(0,1,0));
		// create a new Enemy object that cannot be hit at the position of the boss; this will fire the new set of bullets
		Enemy newStream;
		newStream.init((*pEnemies)[i].getPos().x,(*pEnemies)[i].getPos().y,0,boundaries["boss"],-1,0,0,0,0,1,1);
		newStream.setTargeting(D3DXVECTOR3(0,-1,0));
		subunits.push_back(newStream);
	}
	// add a new set of bullets to fire at less than 21000 life
	else if ((*pEnemies)[i].getLife() <= 21000 && !attackTypes[0]) {
		attackTypes[0] = true;
		(*pEnemies)[i].setTargeting(D3DXVECTOR3(0,1,0));
		Enemy newStream;
		newStream.init((*pEnemies)[i].getPos().x,(*pEnemies)[i].getPos().y,0,boundaries["boss"],-1,0,0,0,0,1,1);
		subunits.push_back(newStream);
	}
	// fire additional bullets in a rotating pattern at less than 21000 life
	else if ((*pEnemies)[i].getLife() < 21000) {
		if ((*pEnemies)[i].getCooldown() == 0) {
			// fire the initial set of bullets
			(*pEnemies)[i].setRotateDirection(0);
			newTarget = (*pEnemies)[i].rotateAim((*pEnemies)[i].getRotateDirection(), PI/16 , 3.0f);
			(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
				D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["blueBall"], 1, 3);
			// fire any additional sets of bullets
			for (size_t j = 0; j < subunits.size(); j++) {
				// each additional Enemy object fires bullets
				subunits[j].setPos((*pEnemies)[i].getPos().x,(*pEnemies)[i].getPos().y,0);
				// one fires small, faster bullets rotating clockwise
				if ((*pEnemies)[i].getRotateDirection() == 0) {
					(*pEnemies)[i].setRotateDirection(1);
					newTarget = subunits[j].rotateAim((*pEnemies)[i].getRotateDirection(), PI/12, 3.0f);
					subunits[j].fire(enemyBullets, D3DXVECTOR3(newTarget.x+subunits[j].getPos(0)+15, newTarget.y+subunits[j].getPos(1)+15,0), 
						D3DXVECTOR3(subunits[j].getPos(0)+15,subunits[j].getPos(1)+15,0), enemyBullets->size(), boundaries["yellowStar"], 1, 3.5);
				}
				// the other fires larger, slower bullets rotating counterclockwise
				else {
					(*pEnemies)[i].setRotateDirection(0);
					newTarget = subunits[j].rotateAim((*pEnemies)[i].getRotateDirection(), PI/12, 3.0f);
					subunits[j].fire(enemyBullets, D3DXVECTOR3(newTarget.x+subunits[j].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
						D3DXVECTOR3(subunits[j].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["largeGreen"], 1, 2);
				}
				(*pEnemies)[i].setCooldown(1);
			}
		}
		else (*pEnemies)[i].setCooldown((*pEnemies)[i].getCooldown()-1);
	}
	// fire bullets in a rotating pattern
	else if ((*pEnemies)[i].getLife() <= 25000) {
		if ((*pEnemies)[i].getCooldown() == 0) {
			if (leveltime%50 == 0) {
				if ((*pEnemies)[i].getRotateDirection() == 0) { 
					(*pEnemies)[i].setRotateDirection(1);
				}
				else (*pEnemies)[i].setRotateDirection(0);  
			}
			newTarget = (*pEnemies)[i].rotateAim((*pEnemies)[i].getRotateDirection(), PI/24, 3.0f);
			(*pEnemies)[i].fire(enemyBullets, D3DXVECTOR3(newTarget.x+(*pEnemies)[i].getPos(0)+15, newTarget.y+(*pEnemies)[i].getPos(1)+15,0), 
				D3DXVECTOR3((*pEnemies)[i].getPos(0)+15,(*pEnemies)[i].getPos(1)+15,0), enemyBullets->size(), boundaries["blueBall"], 1, 3);
			(*pEnemies)[i].setCooldown(1);
		}
		else (*pEnemies)[i].setCooldown((*pEnemies)[i].getCooldown()-1);
	}
}