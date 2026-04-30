#include "MainMenu.h"

MainMenu::MainMenu() {

	sbHilighted = false;
	sandboxBt = false;

	sra = 1.0f; sga = 0.07f; sba = 0.06f;
	shra = 1.00f; shga = 0.6f; shba = 0.6f;

	srb = 0.50f; sgb = 0.7f; sbb = 0.6f;
	shrb = 0.50f; shgb = 0.006f; shbb = 0.006f;

	sbFramesIntoHighlight = 0;

	// Wheeler button — blue-ish
	m_wheelerBtn = { {0,0,0,0}, 0, false,
		0.18f, 0.36f, 0.78f,   // base
		0.50f, 0.72f, 1.00f }; // highlight

	// Abiogenesis button — teal-ish
	m_abioBtn = { {0,0,0,0}, 0, false,
		0.06f, 0.50f, 0.44f,   // base
		0.30f, 0.90f, 0.78f }; // highlight

	// Cells & Bodies button — purple-ish
	m_cellsBtn = { {0,0,0,0}, 0, false,
		0.38f, 0.18f, 0.62f,   // base
		0.65f, 0.40f, 1.00f }; // highlight
}

MainMenu::~MainMenu() {

}

// ---------------------------------------------------------------------------
//  Helper: advance highlight frame counter for one button
// ---------------------------------------------------------------------------
void MainMenu::updateSelectButton(SelectButton &btn) {
	if (btn.hilighted) {
		if (btn.framesHighlight < 30) btn.framesHighlight++;
	} else {
		if (btn.framesHighlight > 0) btn.framesHighlight--;
	}
}

// ---------------------------------------------------------------------------
//  Helper: draw one world-select button with animated colour + label
// ---------------------------------------------------------------------------
void MainMenu::drawSelectButton(SelectButton &btn, const char *label, int w, int h) {
	float32 t = btn.framesHighlight * 0.033333333f;

	glColor3f(
		btn.ra + (btn.rh - btn.ra) * t,
		btn.ga + (btn.gh - btn.ga) * t,
		btn.ba + (btn.bh - btn.ba) * t);

	glBegin(GL_POLYGON);
	glVertex2f((float32)btn.rect.left,  (float32)btn.rect.bottom);
	glVertex2f((float32)btn.rect.right, (float32)btn.rect.bottom);
	glVertex2f((float32)btn.rect.right, (float32)btn.rect.top);
	glVertex2f((float32)btn.rect.left,  (float32)btn.rect.top);
	glEnd();

	// Label (white text)
	glColor3f(1.0f, 1.0f, 1.0f);
	// Centre the text roughly inside the button
	int lx = btn.rect.left + 14;
	int ly = (btn.rect.top + btn.rect.bottom) / 2 + 8;
	glRasterPos2i(lx, ly);
	for (const char *c = label; *c; ++c)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
}

// ---------------------------------------------------------------------------
//  Helper: point-in-RECT test
//
//  The GLUI subwindow at the bottom shifts the GL viewport up by ~50px, so
//  GL coordinate Y = screen_pixel_Y + 50.  We apply the same -50 correction
//  the original sandbox button hit test uses to convert back to screen pixels.
// ---------------------------------------------------------------------------
bool MainMenu::hitTestRect(const RECT &r, int32 x, int32 y) {
	return x > r.left && x < r.right && y > r.top - 50 && y < r.bottom - 50;
}

// ---------------------------------------------------------------------------
//  render() — main menu screen (single "Sandbox World" button)
// ---------------------------------------------------------------------------
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

	smbRect.left   = (w / 2) - 100;
	smbRect.top    = (h / 2) + 25 + 50;
	smbRect.right  = (w / 2) + 100;
	smbRect.bottom = (h / 2) - 25 + 50;

	if (sandboxBt == false) {
		glGenTextures(1, &sandboxButtonTex);
		sandboxButtonTex = SOIL_load_OGL_texture("images\\White_Texture.tex",
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB );
		sandboxBt = true;
	}

	glBindTexture(GL_TEXTURE_2D, sandboxButtonTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor3f(
		srb + (((sra - srb) * (sbFramesIntoHighlight * 0.033333333f))),
		sgb + (((sga - sgb) * (sbFramesIntoHighlight * 0.033333333f))),
		sbb + (((sba - sbb) * (sbFramesIntoHighlight * 0.033333333f))));

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0); glVertex2f((float32)smbRect.left,  (float32)smbRect.bottom);
	glTexCoord2f(1.0, 1.0); glVertex2f((float32)smbRect.right, (float32)smbRect.bottom);
	glTexCoord2f(1.0, 0.0); glVertex2f((float32)smbRect.right, (float32)smbRect.top);
	glTexCoord2f(0.0, 0.0); glVertex2f((float32)smbRect.left,  (float32)smbRect.top);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Label
	char buffer[128];
	strcpy_s(buffer, "Sandbox Mode");

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

// ---------------------------------------------------------------------------
//  renderWorldSelect() — sub-menu: choose which universe
// ---------------------------------------------------------------------------
void MainMenu::renderWorldSelect() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Title
	glColor3f(0.9f, 0.9f, 0.9f);
	int tx = (w / 2) - 90;
	int ty = (h / 2) - 60;
	glRasterPos2i(tx, ty);
	const char *title = "Choose a Universe";
	for (const char *c = title; *c; ++c)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);

	// Layout: three buttons side-by-side, centred
	const int BW = 170, BH = 50, GAP = 20;
	int totalW = BW * 3 + GAP * 2;
	int startX = (w - totalW) / 2;
	int btnY    = (h / 2);

	// Add +50 to Y to match the GL-coordinate convention used by the main menu
	// button: GL_Y = screen_pixel_Y + GLUI_viewport_offset (~50px).
	int glBtnY = btnY + 50;
	m_wheelerBtn.rect = { startX,                 glBtnY, startX + BW,           glBtnY + BH };
	m_abioBtn.rect    = { startX + BW + GAP,       glBtnY, startX + BW*2 + GAP,   glBtnY + BH };
	m_cellsBtn.rect   = { startX + BW*2 + GAP*2,  glBtnY, startX + BW*3 + GAP*2, glBtnY + BH };

	updateSelectButton(m_wheelerBtn);
	updateSelectButton(m_abioBtn);
	updateSelectButton(m_cellsBtn);

	drawSelectButton(m_wheelerBtn, "Wheeler's Universe",  w, h);
	drawSelectButton(m_abioBtn,    "Abiogenesis Universe", w, h);
	drawSelectButton(m_cellsBtn,   "Cells & Bodies",       w, h);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// ---------------------------------------------------------------------------
//  MouseDown — handles both main-menu and world-select states
// ---------------------------------------------------------------------------
void MainMenu::MouseDown(int32 button, int32 stateM, int32 x, int32 y,
	                     State &state, int32 &worldSelection) {
	if (button != GLUT_LEFT_BUTTON) return;

	if (state == MainMenuS) {
		// Hit-test sandbox button (same Y-offset as before)
		if (x > smbRect.left  && x < smbRect.right &&
		    y < smbRect.top - 50 && y > smbRect.bottom - 50) {
			state = WorldSelectS;
		}
		return;
	}

	if (state == WorldSelectS) {
		if (hitTestRect(m_wheelerBtn.rect, x, y)) {
			worldSelection = 0;   // SandboxWorld (Wheeler)
			state = LiveGameS;
		} else if (hitTestRect(m_abioBtn.rect, x, y)) {
			worldSelection = 1;   // Abiogenesis
			state = LiveGameS;
		} else if (hitTestRect(m_cellsBtn.rect, x, y)) {
			worldSelection = 2;   // Cells & Bodies
			state = LiveGameS;
		}
	}
}

// ---------------------------------------------------------------------------
//  MouseMotion — hover highlighting, works in both menu states
// ---------------------------------------------------------------------------
void MainMenu::MouseMotion(int32 x, int32 y, State state) {
	if (state == MainMenuS) {
		sbHilighted = (x > smbRect.left  && x < smbRect.right &&
		               y < smbRect.top - 50 && y > smbRect.bottom - 50);
		return;
	}

	if (state == WorldSelectS) {
		m_wheelerBtn.hilighted = hitTestRect(m_wheelerBtn.rect, x, y);
		m_abioBtn.hilighted    = hitTestRect(m_abioBtn.rect, x, y);
		m_cellsBtn.hilighted   = hitTestRect(m_cellsBtn.rect, x, y);
	}
}

