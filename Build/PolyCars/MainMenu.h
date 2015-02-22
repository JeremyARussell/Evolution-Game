#pragma once
typedef signed int int32;

class MainMenu {
public:
	MainMenu(void);
	~MainMenu(void);

	void render();

	void MainMenuMouse(int32 button, int32 stateM, int32 x, int32 y);
};

