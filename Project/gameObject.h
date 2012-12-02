#ifndef GO_H
#define GO_H

#include <d3d9.h>
#include <d3dx9.h>

class GameObject {

private:
	D3DXVECTOR3 pos, startPos;
	RECT bounds;
	bool active, exploding;
	float s;
	int animTime, type;
	float speed;

public:
	GameObject::GameObject();
	void init(int x, int y, int z, RECT bounds_, int type_, float speed_);
	void init(int x, int y, int z, RECT bounds_, int type_, int midX, int midY, int endX, int endY);
	bool isActive();
	void setActive(bool setting);
	int getPos(int coord);
	D3DXVECTOR3 getPos();
	void setPos(int x, int y, int z);
	void move(int x, int y, int z);
	void moveTo(int dest, D3DXVECTOR3 rate);
	void setBounds(RECT bounds_);
	RECT getBounds();
	bool inBounds(GameObject test);
	D3DXVECTOR3 getStartPos();
	void setStartPos(D3DXVECTOR3 _startPos);
	float getS();
	void setS(float s_);
	bool isExploding();
	void setExploding(bool exploding_);
	bool inBounds(RECT test, int x, int y);
	int getAnimTime();
	void setAnimTime(int time);
	int getType();
	void setType(int type_);
	float getSpeed();
};
	
#endif

