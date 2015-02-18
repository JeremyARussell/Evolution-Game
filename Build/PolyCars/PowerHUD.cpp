#include "PowerHUD.h"
#include "glui\glui.h"
#include "SOIL\SOIL.h";

PowerHUD::PowerHUD() 
{

}

PowerHUD::PowerHUD(float x, float y, _power _worldPowers, _power _activePower)
{

	worldPowers = _worldPowers;
	activePower = _activePower;

	i[0] = 0;
	i[1] = 0;
	i[2] = 0;
	i[3] = 0;
	i[4] = 0;
	i[5] = 0;
	i[6] = 0;
	i[7] = 0;


	//TODO - MOVE all the positions up here
	phX[0] = x;
	phY[0] = y;




	phHeight[0] = 50.5f;
	phWidth[0] = 50.5f * getPowerCount();

	float debug = 888;

}


PowerHUD::~PowerHUD(void)
{
}

	GLuint tex_2d[8];

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
		
	if (i[0] == 0) {
		glGenTextures(1, &tex_2d[0]);

		tex_2d[0] = SOIL_load_OGL_texture("images\\Simple_PowersHUD.tex",
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		i[0] = 1;
	}

	glBindTexture(GL_TEXTURE_2D, tex_2d[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//The main HUD background
	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f(w - phWidth[0] - phX[0], phY[0] + phHeight[0] );
											//Left Bottom

	glTexCoord2f(1.0, 1.0); glVertex2f(w - phX[0], phY[0] + phHeight[0]);		
											//Right Bottom

	glTexCoord2f(1.0, 0.0); glVertex2f(w - phX[0], phY[0]);
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f( w - phWidth[0] - phX[0], phY[0]);
	glEnd();								//Left Top
	/////


	//Hilighter
	if (i[7] == 0) {
		glGenTextures(1, &tex_2d[7]);
		tex_2d[7] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Hilighter.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		i[7] = 1;
	}

	glBindTexture(GL_TEXTURE_2D, tex_2d[7]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	char buf[256];

	switch (activePower) {
	case GRAB:
		phX[7] = -9.0f;
		phY[7] = 20.5f;
		glRasterPos2f(w - phX[0] - phWidth[0] , phY[0] + phHeight[0] + 10);
		sprintf_s(buf, "Grab - Throw those Wheelers around, they don't mind.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SELECT:
		phX[7] = -6.0f;
		phY[7] = 20.5f;
		glRasterPos2f(w - phX[0] - phWidth[0] , phY[0] + phHeight[0] + 10);
		sprintf_s(buf, "Select - When you want to pick a favorite.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case DESTROY:
		phX[7] = -3.0f;
		phY[7] = 20.5f;
		glRasterPos2f(w - phX[0] - phWidth[0] , phY[0] + phHeight[0] + 10);
		sprintf_s(buf, "Destroy - When you have a least favorite.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SPAWN_SEED:
		phX[7] = 0.0f;
		phY[7] = 20.5f;
		glRasterPos2f(w - phX[0] - phWidth[0] , phY[0] + phHeight[0] + 10);
		sprintf_s(buf, "Spawn a Seed - Pull food from the ether.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	case SPAWN_WHEELER:
		phX[7] = 3.0f;
		phY[7] = 20.5f;
		glRasterPos2f(w - phX[0] - phWidth[0] , phY[0] + phHeight[0] + 10);
		sprintf_s(buf, "Spawn Wheeler - Spawn a random Wheeler, results may vary.");
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buf);
		break;
	}
	/*
	//phX[7] = -9.0f;
	//phY[7] = 20.5f;
	phHeight[7] = 3.0f;
	phWidth[7] = 3.0f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[7]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											(( phY[7] - phHeight[7] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Left Bottom 

	glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[7] + phWidth[7]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[7] - phHeight[7] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Right Bottom 

	glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[7] + phWidth[7]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											(( phY[7]				) * settings->zoomLevel)  + settings->viewCenter.y );
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[7]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[7]				) * settings->zoomLevel)  + settings->viewCenter.y );
	glEnd();								//Left Top 
	///


	/*

	///Grab
	if (i[1] == 0) {
		glGenTextures(1, &tex_2d[1]);
		tex_2d[1] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Grab.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		i[1] = 1;
	}

	glBindTexture(GL_TEXTURE_2D, tex_2d[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[1] = -8.75f;
	phY[1] = 20.25f;
	phHeight[1] = 2.5f;
	phWidth[1] = 2.5f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[1]				 ) * settings->zoomLevel)  + settings->viewCenter.x ),    
											(( phY[1] - phHeight[1] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Left Bottom 

	glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[1] + phWidth[1]	 ) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[1] - phHeight[1] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Right Bottom 

	glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[1] + phWidth[1]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											(( phY[1]				) * settings->zoomLevel)  + settings->viewCenter.y );
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[1]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[1]				) * settings->zoomLevel)  + settings->viewCenter.y );
	glEnd();								//Left Top 

	///Select
	if (i[2] == 0) {
		glGenTextures(1, &tex_2d[2]);
		tex_2d[2] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Select.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		i[2] = 1;
	}

	glBindTexture(GL_TEXTURE_2D, tex_2d[2]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[2] = -5.75f;
	phY[2] = 20.25f;
	phHeight[2] = 2.5f;
	phWidth[2] = 2.5f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[2]				 ) * settings->zoomLevel)  + settings->viewCenter.x ),    
											(( phY[2] - phHeight[2] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Left Bottom 

	glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[2] + phWidth[2]	 ) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[2] - phHeight[2] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Right Bottom 

	glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[2] + phWidth[2]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											(( phY[2]				) * settings->zoomLevel)  + settings->viewCenter.y );
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[2]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[2]				) * settings->zoomLevel)  + settings->viewCenter.y );
	glEnd();								//Left Top 

	//Destroy
	if (i[3] == 0) {
		glGenTextures(1, &tex_2d[3]);
		tex_2d[3] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Destroy.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		i[3] = 1;
	}

	glBindTexture(GL_TEXTURE_2D, tex_2d[3]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[3] = -2.75f;
	phY[3] = 20.25f;
	phHeight[3] = 2.5f;
	phWidth[3] = 2.5f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[3]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											(( phY[3] - phHeight[3] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Left Bottom 

	glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[3] + phWidth[3]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[3] - phHeight[3] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Right Bottom 

	glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[3] + phWidth[3]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											(( phY[3]				) * settings->zoomLevel)  + settings->viewCenter.y );
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[3]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[3]				) * settings->zoomLevel)  + settings->viewCenter.y );
	glEnd();								//Left Top 

	//Seeds
	if (i[4] == 0) {
		glGenTextures(1, &tex_2d[4]);
		tex_2d[4] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Seed.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		i[4] = 1;
	}

	glBindTexture(GL_TEXTURE_2D, tex_2d[4]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	phX[4] = 0.25f;
	phY[4] = 20.25f;
	phHeight[4] = 2.5f;
	phWidth[4] = 2.5f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[4]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											(( phY[4] - phHeight[4] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Left Bottom 

	glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[4] + phWidth[4]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[4] - phHeight[4] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Right Bottom 

	glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[4] + phWidth[4]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											(( phY[4]				) * settings->zoomLevel)  + settings->viewCenter.y );
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[4]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[4]				) * settings->zoomLevel)  + settings->viewCenter.y );
	glEnd();								//Left Top 

	//Wheelers
	if (i[5] == 0) {
		glGenTextures(1, &tex_2d[5]);
		tex_2d[5] = SOIL_load_OGL_texture ( "images\\Simple_Powers_Wheeler.tex", 
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		i[5] = 1;
	}

	glBindTexture(GL_TEXTURE_2D, tex_2d[5]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//float32 padWheeler = 0;
	//if (activePower == SPAWN_WHEELER) {
	//	padWheeler = 0.25f;
	//} else {
	//	padWheeler = 0;
	//}

	phX[5] = 3.25f;
	phY[5] = 20.25f;
	phHeight[5] = 2.5f;
	phWidth[5] = 2.5f;

	glBegin(GL_POLYGON);				   // Size + position							|--Aplying zoom			|--Adding the new center
											// from center + pad adjust					v--to retain size		v--to maintain position
	glTexCoord2f(0.0, 1.0); glVertex2f((   (( phX[5]				) * settings->zoomLevel)  + settings->viewCenter.x ),    
											(( phY[5] - phHeight[5] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Left Bottom 

	glTexCoord2f(1.0, 1.0); glVertex2f((   (( phX[5] + phWidth[5]	) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[5] - phHeight[5] ) * settings->zoomLevel)  + settings->viewCenter.y );		
											//Right Bottom 

	glTexCoord2f(1.0, 0.0); glVertex2f((   (( phX[5] + phWidth[5]	) * settings->zoomLevel)  + settings->viewCenter.x ),
											(( phY[5]				) * settings->zoomLevel)  + settings->viewCenter.y );
											//Right Top

	glTexCoord2f(0.0, 0.0); glVertex2f((   (( phX[5]				) * settings->zoomLevel)  + settings->viewCenter.x ),	
											(( phY[5]				) * settings->zoomLevel)  + settings->viewCenter.y );
	glEnd();								//Left Top 
		
	*/

	glDisable(GL_TEXTURE_2D);



	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}