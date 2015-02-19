#include "PowerHUD.h"

PowerHUD::PowerHUD() 
{

}

PowerHUD::PowerHUD(float x, float y, _power _worldPowers, _power _activePower)
{

	worldPowers = _worldPowers;
	activePower = _activePower;

	pbTracker = 0;

	hudBt = false;
	hilBt = false;
	bt[0] = false;
	bt[1] = false;
	bt[2] = false;
	bt[3] = false;
	bt[4] = false;
	bt[5] = false;
	bt[6] = false;
	bt[7] = false;


	hudPhX = x;
	hudPhY = y;
	hudPhHeight = 50.0f;
	hudPhWidth = 50.0f * getPowerCount();

	hilPhY = y;

	//TODO - MOVE all the positions up here
	
	//TODO - Will probably need to make a reposition powers function if I want to activate powers halfway through
	//		 a level or what not. Like in story mode, unlocking, etc. I could also destroy the hud instance and 
	//		 

	if (worldPowers & GRAB) {
		phX[0] = x + (pbTracker * 50);
		phY[0] = y;
		pbTracker++;
	}

	if (worldPowers & SELECT) {
		phX[1] = x + (pbTracker * 50);
		phY[1] = y;
		pbTracker++;
	}

	if (worldPowers & DESTROY) {
		phX[2] = x + (pbTracker * 50);
		phY[2] = y;
		pbTracker++;
	}

	if (worldPowers & SPAWN_SEED) {
		phX[3] = x + (pbTracker * 50);
		phY[3] = y;
		pbTracker++;
	}

	if (worldPowers & SPAWN_WHEELER) {
		phX[4] = x + (pbTracker * 50);
		phY[4] = y;
		pbTracker++;
	}

	//Some logic for incrementing and what not the positions, etc.
	/*
	phX[5] = x;
	phY[5] = y;
	phX[6] = x;
	phY[6] = y;
	phX[7] = x;
	phY[7] = y;
	*/

	float debug = 888;

}


PowerHUD::~PowerHUD(void)
{
}


void PowerHUD::setActivePower(_power _activePower) {
	activePower = _activePower;
}


int PowerHUD::getPowerCount()
	{
		int count = 0;

		int powersAsInt = (int)worldPowers;
		//Loop the value while there are still bits
		while (powersAsInt != 0)
		{
			//Remove the end bit
			powersAsInt = powersAsInt & (powersAsInt - 1);

			//Increment the count
			count++;
		}

		//Return the count
		return count;
	}

void PowerHUD::render() {
	//char buffer[128];
	//const char *string;
	//string = "TEST STRING";
	//va_list arg;
	//va_start(arg, string);
	//vsprintf_s(buffer, string, arg);
	//va_end(arg);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.9f, 0.6f, 0.6f);
	//glRasterPos2i(x, y);
	//int length = (int)strlen(buffer);
	//for (int i = 0; i < length; ++i) {
	//	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
	//}


	glEnable(GL_TEXTURE_2D);
	glColor3f(10.0, 10.0, 10.0);
		
	if (hudBt == false) {
		glGenTextures(1, &hudTex);

		hudTex = SOIL_load_OGL_texture("images\\Simple_PowersHUD.tex",
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		hudBt = true;
	}

	glBindTexture(GL_TEXTURE_2D, hudTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//The main HUD background
	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f(w - hudPhWidth - hudPhX, hudPhY + hudPhHeight );
											//Left Bottom

	glTexCoord2f(1.0, 1.0); glVertex2f(w - hudPhX, hudPhY + hudPhHeight);		
											//Right Bottom

	glTexCoord2f(1.0, 0.0); glVertex2f(w - hudPhX, hudPhY);
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f(w - hudPhWidth - hudPhX, hudPhY);
	glEnd();								//Left Top
	/////


	//Hilighter
	if (hilBt == false) {
		glGenTextures(1, &hilTex);
		hilTex = SOIL_load_OGL_texture ( "images\\Simple_Powers_Hilighter.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		hilBt = true;
	}

	glBindTexture(GL_TEXTURE_2D, hilTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	char buf[256];

	switch (activePower) {
	case GRAB:
		hilPhX = 300.0f;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Grab - Throw those Wheelers around, they don't mind.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SELECT:
		hilPhX = 250.0f;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Select - When you want to pick a favorite.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case DESTROY:
		hilPhX = 200.0f;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Destroy - When you have a least favorite.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SPAWN_SEED:
		hilPhX = 150.0f;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Spawn a Seed - Pull food from the ether.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SPAWN_WHEELER:
		hilPhX = 100.0f;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Spawn Wheeler - Spawn a random Wheeler, results may vary.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	}

	hilPhHeight = 50.0f;
	hilPhWidth = 50.0f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f(w - hilPhX , hilPhY + hilPhHeight);		
											//Left Bottom 

	glTexCoord2f(1.0, 1.0); glVertex2f(w - hilPhX + hilPhWidth, hilPhY + hilPhHeight);		
											//Right Bottom 

	glTexCoord2f(1.0, 0.0); glVertex2f(w - hilPhX + hilPhWidth, hilPhY);
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f(w - hilPhX, hilPhY);
	glEnd();								//Left Top 
	///



	///Grab
	if (bt[0] == false) {
		glGenTextures(1, &powerTexs[1]);
		powerTexs[1] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Grab.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[0] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[0] = 250.0f + 5.0f;
	phY[0] = 55.00f;
	phHeight[0] = 40.0f;
	phWidth[0] = 40.0f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f(w - phWidth[0] - phX[0], phY[0] + phHeight[0] );
											//Left Bottom

	glTexCoord2f(1.0, 1.0); glVertex2f(w - phX[0], phY[0] + phHeight[0]);		
											//Right Bottom

	glTexCoord2f(1.0, 0.0); glVertex2f(w - phX[0], phY[0]);
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f(w - phWidth[0] - phX[0], phY[0]);
	glEnd();								//Left Top 

	///Select
	if (bt[1] == false) {
		glGenTextures(1, &powerTexs[1]);
		powerTexs[2] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Select.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[1] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[1] = 200.0f + 5.0f;
	phY[1] = 55.00f;
	phHeight[1] = 40.0f;
	phWidth[1] = 40.0f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f(w - phWidth[1] - phX[1], phY[1] + phHeight[1] );
											//Left Bottom

	glTexCoord2f(1.0, 1.0); glVertex2f(w - phX[1], phY[1] + phHeight[1]);		
											//Right Bottom

	glTexCoord2f(1.0, 0.0); glVertex2f(w - phX[1], phY[1]);
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f(w - phWidth[1] - phX[1], phY[1]);
	glEnd();								//Left Top 

	//Destroy
	if (bt[2] == false) {
		glGenTextures(1, &powerTexs[2]);
		powerTexs[3] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Destroy.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[2] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[2]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[2] = 150.0f + 5.0f;
	phY[2] = 55.00f;
	phHeight[2] = 40.0f;
	phWidth[2] = 40.0f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f(w - phWidth[2] - phX[2], phY[2] + phHeight[2] );
											//Left Bottom

	glTexCoord2f(1.0, 1.0); glVertex2f(w - phX[2], phY[2] + phHeight[2]);		
											//Right Bottom

	glTexCoord2f(1.0, 0.0); glVertex2f(w - phX[2], phY[2]);
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f(w - phWidth[2] - phX[2], phY[2]);
	glEnd();								//Left Top 

	//Seeds
	if (bt[3] == false) {
		glGenTextures(1, &powerTexs[3]);
		powerTexs[4] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Seed.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[3] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[3]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[3] = 100.0f + 5.0f;
	phY[3] = 55.00f;
	phHeight[3] = 40.0f;
	phWidth[3] = 40.0f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f(w - phWidth[3] - phX[3], phY[3] + phHeight[3] );
											//Left Bottom

	glTexCoord2f(1.0, 1.0); glVertex2f(w - phX[3], phY[3] + phHeight[3]);		
											//Right Bottom

	glTexCoord2f(1.0, 0.0); glVertex2f(w - phX[3], phY[3]);
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f(w - phWidth[3] - phX[3], phY[3]);
	glEnd();								//Left Top 

	//Wheelers
	if (bt[4] == false) {
		glGenTextures(1, &powerTexs[4]);
		powerTexs[4] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Wheeler.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[4] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[4]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[4] = 50.0f + 5.0f;
	phY[4] = 55.00f;
	phHeight[4] = 40.0f;
	phWidth[4] = 40.0f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f(w - phWidth[4] - phX[4], phY[4] + phHeight[4] );
											//Left Bottom

	glTexCoord2f(1.0, 1.0); glVertex2f(w - phX[4], phY[4] + phHeight[4]);		
											//Right Bottom

	glTexCoord2f(1.0, 0.0); glVertex2f(w - phX[4], phY[4]);
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f(w - phWidth[4] - phX[4], phY[4]);
	glEnd();								//Left Top 

	glDisable(GL_TEXTURE_2D);



	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}