#ifndef GO_H
#define GO_H

#include <d3d9.h>
#include <d3dx9.h>

class GameObject {

private:
	D3DXVECTOR3 pos;
	RECT bounds;
	bool active;

public:
	GameObject::GameObject();
	void init(int x, int y, int z, RECT bounds_);
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
	bool inBounds(RECT test, int x, int y);
};
	
#endif

