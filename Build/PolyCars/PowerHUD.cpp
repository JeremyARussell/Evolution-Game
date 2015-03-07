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

	/*Left*/	//left = w - hudPhWidth - hudPhX;
	/*Bottom*/	//bottom = hudPhY + hudPhHeight;	
	/*Right*/	//right = w - hudPhX;
	/*Top*/		//top = hudPhY;



	hilPhY = y;
	hilPhHeight = 50.0f;
	hilPhWidth = 50.0f;
	
	phY[0] = y + 5.0f; phY[1] = y + 5.0f;
	phY[2] = y + 5.0f; phY[3] = y + 5.0f;
	phY[4] = y + 5.0f;
	phHeight[0] = 40.0f; phHeight[1] = 40.0f;
	phHeight[2] = 40.0f; phHeight[3] = 40.0f;
	phHeight[4] = 40.0f;
	phWidth[0] = 40.0f; phWidth[1] = 40.0f;
	phWidth[2] = 40.0f; phWidth[3] = 40.0f;
	phWidth[4] = 40.0f;




	//TODO - Will probably need to make a reposition powers function if I want to activate powers halfway through
	//		 a level or what not. Like in story mode, unlocking, etc. I could also destroy the hud instance and 
	//		 rebuild it in the world that powers can become "unlocked" in.

	if (worldPowers & GRAB) {
		phX[0] = (pbTracker * 50) + 5;
		pbTracker++;
		//phY[0] = y;
	}

	if (worldPowers & SELECT) {
		phX[1] = (pbTracker * 50) + 5;
		pbTracker++;
		//phY[1] = y;
	}

	if (worldPowers & DESTROY) {
		phX[2] = (pbTracker * 50) + 5;
		pbTracker++;
		//phY[2] = y;
	}

	if (worldPowers & SPAWN_SEED) {
		phX[3] = (pbTracker * 50) + 5;
		pbTracker++;
		//phY[3] = y;
	}

	if (worldPowers & SPAWN_WHEELER) {
		phX[4] = (pbTracker * 50) + 5;
		pbTracker++;
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
	//if not a world power set the old power as the active power
	//if it is allowed go ahead and make it the active power
}

float PowerHUD::getHudX() {
	int w = glutGet(GLUT_WINDOW_WIDTH);
	return w - hudPhWidth - hudPhX;
}

float PowerHUD::getHudY() {
	return hudPhY;
}

float PowerHUD::getHudWidth() {
	return hudPhWidth;
}

float PowerHUD::getHudHeight() {
	return hudPhHeight;
}

_power PowerHUD::getActivePower() {
	return activePower;
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

	glDisable(GL_TEXTURE_2D);
	char buf[256];

	switch (activePower) {
	case GRAB:
		hilPhX = phX[0];
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Grab - Throw those Wheelers around, they don't mind.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SELECT:
		hilPhX = phX[1];
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Select - When you want to pick a favorite.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case DESTROY:
		hilPhX = phX[2];
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Destroy - When you have a least favorite.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SPAWN_SEED:
		hilPhX = phX[3];
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Spawn a Seed - Pull food from the ether.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SPAWN_WHEELER:
		hilPhX = phX[4];
		glRasterPos2f(w - hudPhX - hudPhWidth, hudPhY + hudPhHeight + 10);
		sprintf_s(buf, "Spawn Wheeler - Spawn a random Wheeler, results may vary.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	}

	glEnable(GL_TEXTURE_2D);

	//Hilighter
	if (hilBt == false) {
		glGenTextures(1, &hilTex);
		hilTex = SOIL_load_OGL_texture ( "images\\Simple_Powers_Hilighter.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		hilBt = true;
	}

	glBindTexture(GL_TEXTURE_2D, hilTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/ glVertex2f(w - hudPhWidth - hudPhX - hilPhWidth + hilPhX + 45, hilPhY + hilPhHeight );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - hudPhWidth - hudPhX + hilPhX + 45, hilPhY + hilPhHeight);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/ glVertex2f(w - hudPhWidth - hudPhX + hilPhX + 45, hilPhY);
	glTexCoord2f(0.0, 0.0);/*Left Top*/ glVertex2f(w - hudPhWidth - hudPhX - hilPhWidth + hilPhX + 45, hilPhY);
	glEnd();								

	//Not sure what this is for anymore :/ - 03/07/15.
	//glTexCoord2f(0.0, 1.0);/*Left Bottom*/	glVertex2f(w - hilPhX - hilPhWidth, hilPhY + hilPhHeight);		
	//glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - hilPhX, hilPhY + hilPhHeight);		
	//glTexCoord2f(1.0, 0.0);/*Right Top*/	glVertex2f(w - hilPhX, hilPhY);
	//glTexCoord2f(0.0, 0.0);/*Left Top*/		glVertex2f(w - hilPhX - hilPhWidth, hilPhY);

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
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[0] + phX[0] + 40, phY[0] + phHeight[0] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - hudPhWidth - hudPhX + phX[0] + 40, phY[0] + phHeight[0]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/ glVertex2f(w - hudPhWidth - hudPhX + phX[0] + 40, phY[0]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[0] + phX[0] + 40, phY[0]);
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
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[1] + phX[1] + 40, phY[1] + phHeight[1] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - hudPhWidth - hudPhX + phX[1] + 40, phY[1] + phHeight[1]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/ glVertex2f(w - hudPhWidth - hudPhX + phX[1] + 40, phY[1]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[1] + phX[1] + 40, phY[1]);
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
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[2] + phX[2] + 40, phY[2] + phHeight[2] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - hudPhWidth - hudPhX + phX[2] + 40, phY[2] + phHeight[2]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/ glVertex2f(w - hudPhWidth - hudPhX + phX[2] + 40, phY[2]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[2] + phX[2] + 40, phY[2]);
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
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[3] + phX[3] + 40, phY[3] + phHeight[3] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - hudPhWidth - hudPhX + phX[3] + 40, phY[3] + phHeight[3]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/ glVertex2f(w - hudPhWidth - hudPhX + phX[3] + 40, phY[3]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[3] + phX[3] + 40, phY[3]);
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
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[4] + phX[4] + 40, phY[4] + phHeight[4] );
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/ glVertex2f(w - hudPhWidth - hudPhX + phX[4] + 40, phY[4] + phHeight[4]);		
	glTexCoord2f(1.0, 0.0);/*Right Top*/ glVertex2f(w - hudPhWidth - hudPhX + phX[4] + 40, phY[4]);
	glTexCoord2f(0.0, 0.0);/*Left Top*/ glVertex2f(w - hudPhWidth - hudPhX - phWidth[4] + phX[4] + 40, phY[4]);
	glEnd();								 


	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void PowerHUD::click(b2Vec2 rp) {
	int w = glutGet(GLUT_WINDOW_WIDTH);
	
	float disFromLeft = w - hudPhWidth - hudPhX;
	float disFromTop = hudPhY;

	b2Vec2 hudP = b2Vec2(rp.x - disFromLeft, 
		rp.y - disFromTop + 13);


/////

	//Mouse clicking on HUD stuff
	if (hudP.x > phX[0] &&				//Left Side
		hudP.x < phX[0] + phWidth[0])  //Right Side
	{
		activePower = GRAB;
		return;
	}

	if (hudP.x > phX[1] &&				//Left Side
		hudP.x < phX[1] + phWidth[1])  //Right Side
	{
		activePower = SELECT;
		return;
	}

	if (hudP.x > phX[2] &&				//Left Side
		hudP.x < phX[2] + phWidth[2])  //Right Side
	{
		activePower = DESTROY;
		return;
	}

	if (hudP.x > phX[3] &&				//Left Side
		hudP.x < phX[3] + phWidth[3])  //Right Side
	{
		activePower = SPAWN_SEED;
		return;
	}

	if (hudP.x > phX[4] &&				//Left Side
		hudP.x < phX[4] + phWidth[4])  //Right Side
	{
		activePower = SPAWN_WHEELER;
		return;
	}

/////

	int dbint = 9999;
}