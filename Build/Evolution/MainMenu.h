#pragma once
#include "glui/glui.h"
#include "Evolution\Framework\World.h"
#include "SOIL\SOIL.h"

typedef signed int int32;

class MainMenu {
public:
	MainMenu();
	~MainMenu();

	void render();

	void MouseDown(int32 button, int32 stateM, int32 x, int32 y, State &state);

	void MouseMotion(int32 x, int32 y);

private:

	int sbFramesIntoHighlight;

	bool sbHilighted;
	float32 sra, sga, sba, shra, shga, shba, 
			srb, sgb, sbb, shrb, shgb, shbb;

	bool sandboxBt;
	GLuint sandboxButtonTex;
	RECT smbRect;

};

