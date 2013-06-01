/* class GameObject
Contains information required for describing all elements that interact on the game screen, like the player,
enemies, and bullets.  Functions in the class modify or access the information stored in these objects. */  

#ifndef GO_H
#define GO_H

#include <d3d9.h>
#include <d3dx9.h>

class GameObject {

private:
	// The current and initial positions of the object
	D3DXVECTOR3 pos, startPos;
	// The bounding box of the object expressed from the origin and as coordinates on the sprite sheet to use for drawing
	RECT bounds, drawingBounds;
	// Flags for if the object is active (being used on screen) and if it has been destroyed
	bool active, exploding;
	// Parameters for moving the object along curves; s is for splines and t is for spirals
	float s, t;
	// Modifier to the speed the object moves at
	float speed;
	// Time that a frame of animation for the object should be shown for
	int animTime;
	// The type of the object (affects how it is handled in gameplay)
	int type;

public:
	GameObject::GameObject();
	void init(float x, float y, float z, RECT bounds_, int type_, float speed_);
	bool isActive();
	void setActive(bool setting);
	float getPos(int coord);
	D3DXVECTOR3 getPos();
	void setPos(float x, float y, float z);
	void setPos(int coord, float value);
	void move(float x, float y, float z);
	void setBounds(RECT bounds_);
	void setBounds(RECT bounds_, float x, float y); 
	RECT getBounds();
	RECT getDrawingBounds();
	bool inBounds(GameObject test, float x, float y);
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

