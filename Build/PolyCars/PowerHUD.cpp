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
	hilPhHeight = 50.0f;
	hilPhWidth = 50.0f;

	phY[0] = 55.00f; phY[1] = 55.00f;
	phY[2] = 55.00f; phY[3] = 55.00f;
	phY[4] = 55.00f;
	phHeight[0] = 40.0f; phHeight[1] = 40.0f;
	phHeight[2] = 40.0f; phHeight[3] = 40.0f;
	phHeight[4] = 40.0f;
	phWidth[0] = 40.0f; phWidth[1] = 40.0f;
	phWidth[2] = 40.0f; phWidth[3] = 40.0f;
	phWidth[4] = 40.0f;

	//TODO - MOVE all the positions up here
	
	//TODO - Will probably need to make a reposition powers function if I want to activate powers halfway through
	//		 a level or what not. Like in story mode, unlocking, etc. I could also destroy the hud instance and 
	//		 rebuild it in the world that powers can become "unlocked" in.

	if (worldPowers & GRAB) {
		pbTracker++;
		phX[0] = x + (pbTracker * -50) + hudPhWidth + 5;
		//phY[0] = y;
	}

	if (worldPowers & SELECT) {
		pbTracker++;
		phX[1] = x + (pbTracker * -50) + hudPhWidth + 5;
		//phY[1] = y;
	}

	if (worldPowers & DESTROY) {
		pbTracker++;
		phX[2] = x + (pbTracker * -50) + hudPhWidth + 5;
		//phY[2] = y;
	}

	if (worldPowers & SPAWN_SEED) {
		pbTracker++;
		phX[3] = x + (pbTracker * -50) + hudPhWidth + 5;
		//phY[3] = y;
	}

	if (worldPowers & SPAWN_WHEELER) {
		pbTracker++;
		phX[4] = x + (pbTracker * -50) + hudPhWidth + 5;
		//phY[4] = y;
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
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glColor3f(10.0, 10.0, 10.0);
		
	//The main HUD background
	if (hudBt == false) {
		glGenTextures(1, &hudTex);

		hudTex = SOIL_load_OGL_texture("images\\Simple_PowersHUD.tex",
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		hudBt = true;
	}

	glBindTexture(GL_TEXTURE_2D, hudTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/	glVertex2f(w - hudPhWidth - hudPhX, hudPhY + hudPhHeight );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/	glVertex2f(w - hudPhX, hudPhY + hudPhHeight);	
	glTexCoord2f(1.0, 0.0);/*Right Top*/	glVertex2f(w - hudPhX, hudPhY);
	glTexCoord2f(0.0, 0.0);/*Left Top*/		glVertex2f(w - hudPhWidth - hudPhX, hudPhY);
	glEnd();								


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
		hilPhX = phX[0] - 5;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Grab - Throw those Wheelers around, they don't mind.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SELECT:
		hilPhX = phX[1] - 5;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Select - When you want to pick a favorite.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case DESTROY:
		hilPhX = phX[2] - 5;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Destroy - When you have a least favorite.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SPAWN_SEED:
		hilPhX = phX[3] - 5;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Spawn a Seed - Pull food from the ether.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SPAWN_WHEELER:
		hilPhX = phX[4] - 5;
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Spawn Wheeler - Spawn a random Wheeler, results may vary.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	}

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/	glVertex2f(w - hilPhX - hilPhWidth, hilPhY + hilPhHeight);		
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - hilPhX, hilPhY + hilPhHeight);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/	glVertex2f(w - hilPhX, hilPhY);
	glTexCoord2f(0.0, 0.0);/*Left Top*/		glVertex2f(w - hilPhX - hilPhWidth, hilPhY);
	glEnd();								
	///


	///Grab
	if (bt[0] == false) {
		glGenTextures(1, &powerTexs[0]);
		powerTexs[0] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Grab.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[0] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/ glVertex2f(w - phWidth[0] - phX[0], phY[0] + phHeight[0] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - phX[0], phY[0] + phHeight[0]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/ glVertex2f(w - phX[0], phY[0]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/ glVertex2f(w - phWidth[0] - phX[0], phY[0]);
	glEnd();								

	///Select
	if (bt[1] == false) {
		glGenTextures(1, &powerTexs[1]);
		powerTexs[1] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Select.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[1] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/	glVertex2f(w - phWidth[1] - phX[1], phY[1] + phHeight[1] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - phX[1], phY[1] + phHeight[1]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/	glVertex2f(w - phX[1], phY[1]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/		glVertex2f(w - phWidth[1] - phX[1], phY[1]);
	glEnd();								 

	//Destroy
	if (bt[2] == false) {
		glGenTextures(1, &powerTexs[2]);
		powerTexs[2] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Destroy.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[2] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[2]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/	glVertex2f(w - phWidth[2] - phX[2], phY[2] + phHeight[2] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/	glVertex2f(w - phX[2], phY[2] + phHeight[2]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/	glVertex2f(w - phX[2], phY[2]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/		glVertex2f(w - phWidth[2] - phX[2], phY[2]);
	glEnd();								

	//Seeds
	if (bt[3] == false) {
		glGenTextures(1, &powerTexs[3]);
		powerTexs[3] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Seed.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[3] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[3]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/	glVertex2f(w - phWidth[3] - phX[3], phY[3] + phHeight[3] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - phX[3], phY[3] + phHeight[3]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/	glVertex2f(w - phX[3], phY[3]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/		glVertex2f(w - phWidth[3] - phX[3], phY[3]);
	glEnd();								 

	//Wheelers
	if (bt[4] == false) {
		glGenTextures(1, &powerTexs[4]);
		powerTexs[4] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Wheeler.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		bt[4] = true;
	}

	glBindTexture(GL_TEXTURE_2D, powerTexs[4]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0); /*Left Bottom*/		glVertex2f(w - phWidth[4] - phX[4], phY[4] + phHeight[4] );
	glTexCoord2f(1.0, 1.0); /*Right Bottom*/	glVertex2f(w - phX[4], phY[4] + phHeight[4]);		
	glTexCoord2f(1.0, 0.0); /*Right Top*/		glVertex2f(w - phX[4], phY[4]);
	glTexCoord2f(0.0, 0.0); /*Left Top*/		glVertex2f(w - phWidth[4] - phX[4], phY[4]);
	glEnd();								 


	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}