#pragma once
#include "PolyCars\Framework\World.h"

typedef signed int int32;

class LiveGame {
public:
	LiveGame();
	~LiveGame();

	void render();

	void MouseDown(int32 button, int32 stateM, int32 x, int32 y);

	void MouseMotion(int32 x, int32 y);

private:

};

