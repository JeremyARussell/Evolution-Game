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
	void renderWorldSelect();

	void MouseDown(int32 button, int32 stateM, int32 x, int32 y, State &state, int32 &worldSelection);
	void MouseMotion(int32 x, int32 y, State state);

private:

	// ---- Main menu (Sandbox World button) ----
	int sbFramesIntoHighlight;

	bool sbHilighted;
	float32 sra, sga, sba, shra, shga, shba, 
			srb, sgb, sbb, shrb, shgb, shbb;

	bool sandboxBt;
	GLuint sandboxButtonTex;
	RECT smbRect;

	// ---- World-select sub-menu ----
	struct SelectButton {
		RECT   rect;
		int    framesHighlight;   // 0..30
		bool   hilighted;
		// base colour and highlight colour (same scheme as sandbox button)
		float32 ra, ga, ba;       // base
		float32 rh, gh, bh;       // highlight
	};

	SelectButton m_wheelerBtn;
	SelectButton m_abioBtn;

	void updateSelectButton(SelectButton &btn);
	void drawSelectButton(SelectButton &btn, const char *label, int w, int h);
	bool hitTestRect(const RECT &r, int32 x, int32 y);
};


