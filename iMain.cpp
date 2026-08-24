 #include "iGraphics.h"
#include <windows.h> // Direct Windows keyboard handling

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int horizonY = 380;
float lineOffset = 0.0f;
float scrollSpeed = 0.015f;

char rickshaw[8][15] = { "char\\R1.bmp", "char\\R2.bmp", "char\\R3.bmp", "char\\R4.bmp", "char\\R5.bmp", "char\\R6.bmp", "char\\R7.bmp", "char\\R8.bmp" };

int rickshawCordinateX = 300;
int rickshawCordinateY = 10;
int rickshawIndex = 0;

void iDraw() {
	iClear();

	// 1. Sky & Background
	iSetColor(135, 206, 235);
	iFilledRectangle(0, horizonY, SCREEN_WIDTH, SCREEN_HEIGHT - horizonY);

	// Sun
	iSetColor(255, 200, 0);
	iFilledCircle(400, 480, 40);

	// 2. Grass Sides
	iSetColor(34, 139, 34);
	iFilledRectangle(0, 0, SCREEN_WIDTH, horizonY);

	// 3. Perspective Road
	double roadX[] = { 350, 450, 750, 50 };
	double roadY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(60, 60, 60);
	iFilledPolygon(roadX, roadY, 4);

	// 4. Curbs
	double leftCurbX[] = { 340, 350, 50, 10 };
	double leftCurbY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(200, 0, 0);
	iFilledPolygon(leftCurbX, leftCurbY, 4);

	double rightCurbX[] = { 450, 460, 790, 750 };
	double rightCurbY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(200, 0, 0);
	iFilledPolygon(rightCurbX, rightCurbY, 4);

	// 5. Center Yellow Lane Markers
	iSetColor(255, 215, 0);
	for (float t = lineOffset; t < 1.0f; t += 0.2f) {
		double currentY = horizonY - (t * horizonY);
		double nextY = horizonY - ((t + 0.1f) * horizonY);

		double currentWidth = 2 + (t * 16);
		double nextWidth = 2 + ((t + 0.1f) * 16);

		double lineX[] = { 400 - currentWidth, 400 + currentWidth, 400 + nextWidth, 400 - nextWidth };
		double lineY[] = { currentY, currentY, nextY, nextY };

		if (nextY >= 0) {
			iFilledPolygon(lineX, lineY, 4);
		}
	}

	// Draw Rickshaw
	iShowBMP2(rickshawCordinateX, rickshawCordinateY, rickshaw[rickshawIndex], 0);
}

// Check hardware keys directly every frame
void updateGame() {
	// Check if UP Arrow OR 'W' is physically held down
	if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000)) {
		// Scroll road
		lineOffset += scrollSpeed;
		if (lineOffset >= 0.2f) {
			lineOffset -= 0.2f;
		}
		// Animate rickshaw frames
		rickshawIndex = (rickshawIndex + 1) % 8;
	}

	// Check if LEFT Arrow OR 'A' is physically held down
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState('A') & 0x8000)) {
		if (rickshawCordinateX > 50) rickshawCordinateX -= 10;
	}

	// Check if RIGHT Arrow OR 'D' is physically held down
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState('D') & 0x8000)) {
		if (rickshawCordinateX < 550) rickshawCordinateX += 10;
	}
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}
void iMouse(int button, int state, int mx, int my) {}
void iKeyboard(unsigned char key) {}
void iSpecialKeyboard(unsigned char key) {}
void fixedUpdate() {}

int main() {
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Perspective Road View");

	// Game Loop Timer (Updates input, road movement, and rickshaw every 30ms)
	iSetTimer(60, updateGame);

	iStart();
	return 0;
}
