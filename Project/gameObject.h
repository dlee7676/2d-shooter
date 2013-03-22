#ifndef GO_H
#define GO_H

#include <d3d9.h>
#include <d3dx9.h>

class GameObject {

private:
	D3DXVECTOR3 pos, startPos;
	RECT bounds, initialBounds;
	bool active, exploding;
	float s, t, speed;
	int animTime, type;

public:
	GameObject::GameObject();
	void init(int x, int y, int z, RECT bounds_, int type_, float speed_);
	bool isActive();
	void setActive(bool setting);
	int getPos(int coord);
	D3DXVECTOR3 getPos();
	void setPos(double x, double y, double z);
	void setPos(int coord, double value);
	void move(double x, double y, double z);
	void setBounds(RECT bounds_);
	void setBounds(RECT bounds_, int x, int y); 
	RECT getBounds();
	RECT getInitialBounds();
	bool inBounds(GameObject test);
	bool inBounds(RECT test, int x, int y);
	bool GameObject::inBounds(RECT test, int x, int y, int xMod, int yMod); 
	D3DXVECTOR3 getStartPos();
	void setStartPos(D3DXVECTOR3 _startPos);
	float getS();
	void setS(float s_);
	float getT();
	void setT(float t_);
	bool isExploding();
	void setExploding(bool exploding_);
	int getAnimTime();
	void setAnimTime(int time);
	int getType();
	void setType(int type_);
	float getSpeed();
	void setSpeed(float speed_);
};
	
#endif

