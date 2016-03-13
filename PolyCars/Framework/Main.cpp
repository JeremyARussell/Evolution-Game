/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "Render.h"
#include "World.h"
#include "Build/PolyCars/MainMenu.h"
#include "glui/glui.h"

#include <cstdio>
using namespace std;

namespace {
	int32 major = 0;
	int32 minor = 1;
	int32 revision = 8;
	char  bugRevision = ' ';

	int32 worldIndex = 0;
	int32 worldSelection = 0;
	int32 worldCount = 0;
	WorldEntry* entry;
	World* world;
	Settings settings;
	int32 width = 1100;
	int32 height = 660;
	int32 framePeriod = 16;
	int32 mainWindow;
	float settingsHz = 60.0;
	GLUI *glui;
	bool gluiHidden;
	float32 viewZoom = 3.5f;
	int tx, ty, tw, th;
	bool rMouseDown;
	b2Vec2 lastp;

	//Game State stuff
	State state;

	MainMenu mainMenu;
}

static void Resize(int32 w, int32 h) {
	width = w;
	height = h;

	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float32 ratio = float32(tw) / float32(th);

	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	// L/R/B/T
	gluOrtho2D(lower.x, upper.x, lower.y, upper.y);
}

static b2Vec2 ConvertScreenToWorld(int32 x, int32 y) {
	float32 u = x / float32(tw);
	float32 v = (th - y) / float32(th);

	float32 ratio = float32(tw) / float32(th);
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
}

// This is used to control the frame rate (60Hz).
static void Timer(int) {
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 0);
}

static void SimulationLoop() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Main Menu Stuff
	if(state == MainMenuS) {
		if (!gluiHidden) {
			glui->hide();
			gluiHidden = true;
		}
		mainMenu.render();
	}

	//Live Game stuff
	if (state == LiveGameS) {	
		if (gluiHidden) {
			glui->show();
			gluiHidden = false;
		}
		b2Vec2 oldCenter = settings.viewCenter;

		world->SetTextLine(30);
		settings.hz = settingsHz;

		//if (settings.pause) {
		//Need stasis for interactions, no keyboard, no mouse. etc - Will have to put these checks inside the control code...
		/// Mouse and Keyboard
		//settings.viewCenter = oldCenter; -- Checkout the right click move stuff
		//Zoom goes in other side
		//some stuff you only do when paused...Hmm
		////world->Step(&settings);
		////world->powerHUD.render();
		///TODO////Overlay translucent menu options and pause screen.
		//} else {
		//Allow us to zoom and interact
		//settings.zoomLevel = viewZoom; -- Disable scroll when paused in mouse area. - Can still zoom after latest changes
		world->Step(&settings);
		world->powerHUD.render();
		//}
		//TODO - Test comment this below part out to see if main menu will resize still
		if (oldCenter.x != settings.viewCenter.x || oldCenter.y != settings.viewCenter.y)//If size of window has changed
		{
			Resize(width, height);//Runs the ortho rezise stuff;
		}

	}

	//world->DrawTitle(5, 20, entry->name);//TODO - Decide if we will keep

	glutSwapBuffers();

	if (state == LiveGameS) {
		if (worldSelection != worldIndex) {
			worldIndex = worldSelection;
			delete world;
			entry = g_worldEntries + worldIndex;
			world = entry->createFcn();
			viewZoom = 1.0f;
			settings.viewCenter.Set(0.0f, 20.0f);
			Resize(width, height);
		}
	}
}

static void Keyboard(unsigned char key, int x, int y) {
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key) {
	case ' ':
		break;
	case 27://ESC key pauses
//#ifndef __APPLE__
//		// freeglut specific function
//		glutLeaveMainLoop();
//#endif
//		exit(0);
	case 'p':
		settings.pause = !settings.pause;
		break;

	default:
		if (world) {
			world->Keyboard(key);
		}
	}
}

static void KeyboardSpecial(int key, int x, int y) {
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key) {
	case GLUT_ACTIVE_SHIFT:
		break;
	// Press left to pan left.
	case GLUT_KEY_LEFT:
		settings.viewCenter.x -= 0.5f;
		Resize(width, height);
		break;
	// Press right to pan right.
	case GLUT_KEY_RIGHT:
		settings.viewCenter.x += 0.5f;
		Resize(width, height);
		break;
	// Press down to pan down.
	case GLUT_KEY_DOWN:
		settings.viewCenter.y -= 0.5f;
		Resize(width, height);
		break;
	// Press up to pan up.
	case GLUT_KEY_UP:
		settings.viewCenter.y += 0.5f;
		Resize(width, height);
		break;
	// Press home to reset the view.
	case GLUT_KEY_HOME:
		viewZoom = 1.0f;
		settings.viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
		break;
	}
}

static void KeyboardUp(unsigned char key, int x, int y) {
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	if (world) {
		world->KeyboardUp(key);
	}
}

static void Mouse(int32 button, int32 stateM, int32 x, int32 y) {
	//Cheating and putting mouse stealing function for the Main menu here, then returning, instead of wrapping the 
	//below stuff in extra functions or classes for a live game state.
	if (state == MainMenuS) {
		mainMenu.MouseDown(button, stateM, x, y, state);
		return;
	}
	
	//TODO - Might need to cheat for the pause screen real quick to.

	// Use the mouse to activate world powers.
	if (button == GLUT_LEFT_BUTTON) {
		int mod = glutGetModifiers();
		b2Vec2 p = ConvertScreenToWorld(x, y);
		if (stateM == GLUT_DOWN) {
			b2Vec2 p = ConvertScreenToWorld(x, y);
			b2Vec2 rp = b2Vec2(x, y);

			if (rp.x > world->powerHUD.getHudX() &&			//Left Side
				rp.x < world->powerHUD.getHudX() + world->powerHUD.getHudWidth() &&	//Right Side
				rp.y > world->powerHUD.getHudY() - 15 &&				//Top Side
				rp.y < world->powerHUD.getHudY() + world->powerHUD.getHudHeight() - 18)	//Bottom side
			{
				world->powerHUD.click(rp);
				return;
			}
			//Bounds limiter
			if (p.x < world->left	|
				p.x > world->right	|
				p.y < world->bottom |
				p.y > world->top ) 
			{
				return;//If the player clicks outside the worlds bounds, than the click is ignored.
			}

			if (mod == GLUT_ACTIVE_SHIFT) {
				world->ShiftMouseDown(p);
			} else {
				world->MouseDown(p);
			}
		}
		
		if (stateM == GLUT_UP) {
			world->MouseUp(p);
		}
	} else if (button == GLUT_RIGHT_BUTTON) {
		if (stateM == GLUT_DOWN) {	
			lastp = ConvertScreenToWorld(x, y);
			rMouseDown = true;
		}

		if (stateM == GLUT_UP) {
			rMouseDown = false;
		}
	}
}

static void MousePassiveMotion(int32 x, int32 y) {
	//TODO - MainMenu MouseMotion interception to allow for hovering over button animations.
	if (state == MainMenuS) {
		mainMenu.MouseMotion(x, y);
		return;
	}
}

static void MouseMotion(int32 x, int32 y) {


	b2Vec2 p = ConvertScreenToWorld(x, y);
	world->MouseMove(p);
	
	if (rMouseDown)	{
		b2Vec2 diff = p - lastp;
		settings.viewCenter.x -= diff.x;
		settings.viewCenter.y -= diff.y;
		Resize(width, height);
		lastp = ConvertScreenToWorld(x, y);
	}
}

static void MouseWheel(int wheel, int direction, int x, int y) {
	B2_NOT_USED(wheel);
	B2_NOT_USED(x);
	B2_NOT_USED(y);
	if(state == MainMenuS) return;//don't process the mouse wheel if things are at the main menu.
	if (direction > 0) {
		viewZoom /= 1.1f;
	} else {
		viewZoom *= 1.1f;
	}
	Resize(width, height);
}

static void Restart(int) {
	delete world;
	entry = g_worldEntries + worldIndex;
	world = entry->createFcn();
    Resize(width, height);
}

static void Pause(int) {
	settings.pause = !settings.pause;
}

static void Save(int) {
	world->saveWorld();
}
static void Load(int) {
	Restart(1);
	world->loadWorld();
}

static void Exit(int code) {
	// TODO: freeglut is not building on OSX
#ifdef FREEGLUT
	glutLeaveMainLoop();
#endif
	exit(code);
}

static void SingleStep(int) {
	settings.pause = 1;
	settings.singleStep = 1;
}

static void nextWheeler(int) {
	world->nextWheeler();
}
static void previusWheeler(int) {
	world->nextWheeler();
}

static void destroyCreature(int) {
	world->destroyCreature();
}

static void exportCreature(int) {
	world->exportCreature();
}
static void importCreature(int) {
	world->importCreature();
}

int main(int argc, char** argv) {
	srand (time(NULL));
	worldCount = 1;
	//while (g_worldEntries[worldCount].createFcn != NULL) { ++worldCount; }//Keep for later

	state = MainMenuS;

	worldIndex = 0;//b2Clamp(worldIndex, 0, worldCount - 1);
	worldSelection = worldIndex;

	entry = g_worldEntries + worldIndex;
	world = entry->createFcn();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 0);
	char title[32];
	sprintf_s(title, "Evolution Ver. %d.%d.%d", major, minor, revision);
	title[20] = bugRevision;
	title[21] = NULL;
	mainWindow = glutCreateWindow(title);

	glutDisplayFunc(SimulationLoop);
	GLUI_Master.set_glutReshapeFunc(Resize);  
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutMouseFunc(Mouse);
	//Change background color
	glClearColor(0.0f, 0.054f, 0.124f, 0.5f);
#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);

	glutKeyboardUpFunc(KeyboardUp);

	glui = GLUI_Master.create_glui_subwindow( mainWindow, 
		GLUI_SUBWINDOW_BOTTOM );

	glui->add_button("Save", 0, Save);
	glui->add_button("Load", 0, Load);
	glui->add_button("Import", 0, importCreature);

	glui->add_column(true);
	glui->add_button("Export", 0, exportCreature);

	glui->add_column(true);
	glui->add_separator();
	glui->add_separator();
	glui->add_button("Previous", 0, previusWheeler);
	glui->add_separator();
	glui->add_separator();

	glui->add_column(true);

	glui->add_checkbox("Follow Creature", &settings.followCreature);
	glui->add_checkbox("Draw Genes", &settings.drawGenes);
	glui->add_button("Destroy", 0, destroyCreature);

	glui->add_column(true);
	glui->add_separator();
	glui->add_separator();
	glui->add_button("Next", 0, nextWheeler);
	glui->add_separator();
	glui->add_separator();

	//GLUI_Spinner* grassSpawnSpinner =
	//	glui->add_spinner("Grass Rate", GLUI_SPINNER_INT, &settings.grassSpawnRate);
	//grassSpawnSpinner->set_int_limits(1, 10);
	//grassSpawnSpinner->set_speed(0.1f);

	glui->add_column(true);
	glui->add_button("Pause", 0, Pause);
	///*#if defined (_DEBUG)*/ glui->add_button("Single Step", 0, SingleStep);
	///*#endif*/
	glui->add_button("Restart", 0, Restart);
	glui->add_button("Quit", 0,(GLUI_Update_CB)Exit);
		
	glui->add_separator();

	glui->set_main_gfx_window( mainWindow );
	
	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);

	glutMainLoop();

	return 0;
}