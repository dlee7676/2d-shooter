#ifndef GO_H
#define GO_H

#include <d3d9.h>
#include <d3dx9.h>

class GameObject {

private:
	D3DXVECTOR3 pos;
	bool active;

public:
	GameObject::GameObject();
	bool isActive();
	void setActive(bool setting);
	int getPos(int coord);
	D3DXVECTOR3 getPos();
	void setPos(int x, int y, int z);
	void move(int x, int y, int z);
	void moveTo(int dest, D3DXVECTOR3 rate);
};
	
#endif

