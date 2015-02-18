#pragma once


enum _power {
    GRAB = 0x0001,
    SELECT = 0x0002,
	DESTROY = 0x0004,
	SPAWN_SEED = 0x0008,
	SPAWN_WHEELER = 0x00016//,
	//FEED
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

private:
	_power worldPowers;
	_power activePower;
	int getPowerCount();

	int i[8];//8 Texture for now, one for the background, 3 for powers, probably more.
	float phX[8];
	float phY[8];
	float phWidth[8];
	float phHeight[8];

};

