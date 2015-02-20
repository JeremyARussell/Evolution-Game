#pragma once
#include "Box2D\Box2D.h"
#include "glui\glui.h"
#include "SOIL\SOIL.h"

enum _power {
    GRAB = 1,
    SELECT = 2,
	DESTROY = 4,
	SPAWN_SEED = 8,
	SPAWN_WHEELER = 16//,
	//FEED
	//Control
	//Raise Land - Can't wait to start this.
	//
};


class PowerHUD
{
public:
	PowerHUD();
	PowerHUD(float x, float y, _power _worldPowers, _power _activePower);
	~PowerHUD(void);

	void render();

	void setActivePower(_power _activePower);
	//void getActivePower(_power _activePower);

	//Position and dimension variables
	float getHudX();
	float getHudY();
	float getHudWidth();
	float getHudHeight();
	_power getActivePower();

	void click(b2Vec2 rp);

private:
	_power worldPowers;
	_power activePower;
	int getPowerCount();
	int pbTracker;

	GLuint hudTex, hilTex, powerTexs[8];

	//texture initialization tracking bools
	bool hudBt, hilBt,//HUD and hilighter 
		 bt[8];//powers

	//UI positions, widths and heights
	float hudPhX, hudPhY, hilPhX, hilPhY,
		  phX[8], phY[8],
		  hudPhWidth, hudPhHeight, hilPhWidth, hilPhHeight,
		  phWidth[8], phHeight[8];

};

