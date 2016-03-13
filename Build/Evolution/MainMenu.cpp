#include "MainMenu.h"

MainMenu::MainMenu() {

	sbHilighted = false;
	sandboxBt = false;

	sra = 1.0f; sga = 0.07f; sba = 0.06f;
	shra = 1.00f; shga = 0.6f; shba = 0.6f;

	srb = 0.50f; sgb = 0.7f; sbb = 0.6f;
	shrb = 0.50f; shgb = 0.006f; shbb = 0.006f;

	sbFramesIntoHighlight = 0;

}

MainMenu::~MainMenu() {

}

void MainMenu::render() {

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if (sbHilighted) {
		glColor3f(sra, sga, sba);
		if(sbFramesIntoHighlight <= 29) {
			sbFramesIntoHighlight++;
		}
	} else {
		glColor3f(srb, sgb, sbb);
		if(sbFramesIntoHighlight > 0) {
			sbFramesIntoHighlight--;
		}
	}

	glEnable(GL_TEXTURE_2D);
	//
		//Instructions for building the sandbox mode button out of the rectangle

	smbRect.left = (w / 2) - 100;
	smbRect.top = (h / 2) + 25 + 50;
	smbRect.right = (w / 2) + 100;
	smbRect.bottom = (h / 2) - 25 + 50;

	if (sandboxBt == false) {

		glGenTextures(1, &sandboxButtonTex);
		sandboxButtonTex = SOIL_load_OGL_texture("images\\White_Texture.tex",
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		sandboxBt = true;
	}

	glBindTexture(GL_TEXTURE_2D, sandboxButtonTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//if(sbFramesIntoHighlight == 30) {
	//	int debuggte = 993939;
	//}

	//glBegin(GL_POLYGON);
	//glTexCoord2f(0.0, 1.0);/*Left Bottom*/	glVertex2f(smbRect.left + 300 + 70, smbRect.bottom);
	//glTexCoord2f(1.0, 1.0);/*Right Bottom*/	glVertex2f(smbRect.right + 30, smbRect.bottom);	
	//glTexCoord2f(1.0, 0.0);/*Right Top*/	glVertex2f(smbRect.right + 30, smbRect.top + sbFramesIntoHighlight);
	//glTexCoord2f(0.0, 0.0);/*Left Top*/		glVertex2f(smbRect.left + 300 + 70, smbRect.top + sbFramesIntoHighlight);
	//glEnd();								

	glColor3f(
		srb	+    (((sra - srb) * (sbFramesIntoHighlight * 0.033333333f)) )	, 
		sgb	+    (((sga - sgb) * (sbFramesIntoHighlight * 0.033333333f)) )	, 
		sbb	+    (((sba - sbb) * (sbFramesIntoHighlight * 0.033333333f)) )	);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);/*Left Bottom*/	glVertex2f(smbRect.left, smbRect.bottom);
	glTexCoord2f(1.0, 1.0);/*Right Bottom*/	glVertex2f(smbRect.right, smbRect.bottom);	
	glTexCoord2f(1.0, 0.0);/*Right Top*/	glVertex2f(smbRect.right, smbRect.top);
	glTexCoord2f(0.0, 0.0);/*Left Top*/		glVertex2f(smbRect.left, smbRect.top);
	glEnd();								

	glDisable(GL_TEXTURE_2D);


	// TExt
	char buffer[128];

	const char *string = "Sandbox Mode";
	va_list arg;
	va_start(arg, string);
	vsprintf_s(buffer, string, arg);
	va_end(arg);

	glColor3f(
		shrb + (((shra - shrb) * (sbFramesIntoHighlight * 0.033333333f))), 
		shgb + (((shga - shgb) * (sbFramesIntoHighlight * 0.033333333f))), 
		shbb + (((shba - shbb) * (sbFramesIntoHighlight * 0.033333333f))));

	int x = (w / 2) - 72;
	int y = (h / 2) + 55;
	glRasterPos2i(x, y);
	int32 length = (int32)strlen(buffer);
	for (int32 i = 0; i < length; ++i) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buffer[i]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void MainMenu::MouseDown(int32 button, int32 stateM, int32 x, int32 y, State &state) {
	if(button == GLUT_LEFT_BUTTON) {
		if (	
		x > smbRect.left &&
		x < smbRect.right &&
		y < smbRect.top - 50 &&
		y > smbRect.bottom - 50
	) {
			state = LiveGameS;
		}
	}
}

void MainMenu::MouseMotion(int32 x, int32 y) {
	if(	
		x > smbRect.left &&
		x < smbRect.right &&
		y < smbRect.top - 50 &&
		y > smbRect.bottom - 50
	) {
		sbHilighted = true;
	} else {
		sbHilighted = false;
	}
}