#include "iGraphics.h"   //// nadira told me to make a change
#include <windows.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
//change
int horizonY = 380;
float lineOffset = 0.0f;
float scrollSpeed = 0.015f;

// Game State: 0 = Playing, 1 = Game Over
int gameState = 0;

// Driving flag: track if player started moving
int isDriving = 0;

// Rickshaw Animation & Position
char rickshaw[8][15] = { "R1.bmp", "R2.bmp", "R3.bmp", "R4.bmp", "R5.bmp", "R6.bmp", "R7.bmp", "R8.bmp" };
int rickshawCordinateX = 425;
int rickshawCordinateY = 10;
int rickshawIndex = 0;

// Base Rickshaw size
int rickshawWidth = 45;
int rickshawHeight = 50;

// Obstacle Dimensions
int obstacleWidth = 65;
int obstacleHeight = 70;

// ---------- Obstacle System ----------
#define MAX_OBSTACLES 3

typedef struct {
	float x;
	float y;
	float speed;
	int type;           // 0: Yellow Taxi, 1: White Van
	int active;
} Obstacle;

Obstacle obstacles[MAX_OBSTACLES];
int obstacleSpawnTimer = 0;

// ---------- Declarations ----------
void resetObstacle(int index);
void initObstacles();
void resetGame();

void resetObstacle(int index) {
	obstacles[index].y = (float)horizonY;
	obstacles[index].x = 320 + (rand() % 260);
	obstacles[index].speed = 3.0f + (rand() % 100) / 50.0f;
	obstacles[index].type = rand() % 2;
	obstacles[index].active = 0;
}

void initObstacles() {
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		resetObstacle(i);
	}
	obstacleSpawnTimer = 0;
}

void resetGame() {
	rickshawCordinateX = 425;
	rickshawCordinateY = 10;
	lineOffset = 0.0f;
	isDriving = 0;
	initObstacles();
	gameState = 0;
}

// Strict Omnidirectional Axis-Aligned Bounding Box (AABB) Collision Detection
// Returns 1 if touching from top, bottom, left, or right side
int checkCollision(float rX, float rY, float rW, float rH, float oX, float oY, float oW, float oH) {
	if (rX < (oX + oW) &&
		(rX + rW) > oX &&
		rY < (oY + oH) &&
		(rY + rH) > oY) {
		return 1;
	}
	return 0;
}

// ---------- Visual Elements ----------
void drawSideBuildings() {
	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.2f + lineOffset, 1.0f);
		if (progress < 0) progress += 1.0f;

		int bw = 60 - progress * 35;
		int bh = 180 - progress * 90;
		int bx = 10 + progress * 160;
		int by = 10 + progress * 300;

		iSetColor(180, 120, 90);
		iFilledRectangle(bx, by, bw, bh);

		iSetColor(100, 50, 40);
		iFilledRectangle(bx, by + bh, bw, 6);

		iSetColor(255, 230, 150);
		for (int c = 0; c < bw / 18; c++) {
			for (int r = 0; r < bh / 25; r++) {
				iFilledRectangle(bx + 4 + c * 16, by + 5 + r * 22, 6, 10);
			}
		}
	}

	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.2f + lineOffset, 1.0f);
		if (progress < 0) progress += 1.0f;

		int bw = 60 - progress * 35;
		int bh = 180 - progress * 90;
		int bx = SCREEN_WIDTH - 10 - bw - progress * 160;
		int by = 10 + progress * 300;

		iSetColor(170, 110, 85);
		iFilledRectangle(bx, by, bw, bh);

		iSetColor(100, 50, 40);
		iFilledRectangle(bx, by + bh, bw, 6);

		iSetColor(255, 230, 150);
		for (int c = 0; c < bw / 18; c++) {
			for (int r = 0; r < bh / 25; r++) {
				iFilledRectangle(bx + 4 + c * 16, by + 5 + r * 22, 6, 10);
			}
		}
	}
}

void drawSideTreesAndPoles() {
	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.25f + lineOffset, 1.0f);
		if (progress < 0) progress += 1.0f;

		int tx = 20 + progress * 200;
		int ty = 20 + progress * 300;
		int scale = (1.0f - progress) * 35 + 10;

		if (i % 2 == 0) {
			iSetColor(101, 67, 33);
			iFilledRectangle(tx - 3, ty, 6, scale);
			iSetColor(34, 139, 34);
			iFilledCircle(tx, ty + scale, scale * 0.7);
		}
		else {
			iSetColor(90, 90, 90);
			iFilledRectangle(tx - 2, ty, 4, scale + 15);
			iSetColor(60, 60, 60);
			iFilledRectangle(tx - 10, ty + scale + 10, 20, 3);
		}
	}

	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.25f + lineOffset, 1.0f);
		if (progress < 0) progress += 1.0f;

		int tx = SCREEN_WIDTH - 20 - progress * 200;
		int ty = 20 + progress * 300;
		int scale = (1.0f - progress) * 35 + 10;

		if (i % 2 == 0) {
			iSetColor(101, 67, 33);
			iFilledRectangle(tx - 3, ty, 6, scale);
			iSetColor(34, 139, 34);
			iFilledCircle(tx, ty + scale, scale * 0.7);
		}
		else {
			iSetColor(90, 90, 90);
			iFilledRectangle(tx - 2, ty, 4, scale + 15);
			iSetColor(60, 60, 60);
			iFilledRectangle(tx - 10, ty + scale + 10, 20, 3);
		}
	}
}

// Rear-View Yellow Taxi (65 x 70)
void drawTaxi(int vx, int vy) {
	int w = obstacleWidth;

	iSetColor(30, 30, 30);
	iFilledRectangle(vx + 6, vy, 12, 9);
	iFilledRectangle(vx + w - 18, vy, 12, 9);

	iSetColor(50, 50, 50);
	iFilledRectangle(vx + 4, vy + 6, w - 8, 8);

	iSetColor(250, 200, 0);
	iFilledRectangle(vx + 4, vy + 14, w - 8, 25);

	double roofX[] = { (double)vx + 4, (double)vx + 12, (double)vx + w - 12, (double)vx + w - 4 };
	double roofY[] = { (double)vy + 39, (double)vy + 62, (double)vy + 62, (double)vy + 39 };
	iFilledPolygon(roofX, roofY, 4);

	iSetColor(40, 50, 65);
	double glassX[] = { (double)vx + 8, (double)vx + 15, (double)vx + w - 15, (double)vx + w - 8 };
	double glassY[] = { (double)vy + 41, (double)vy + 59, (double)vy + 59, (double)vy + 41 };
	iFilledPolygon(glassX, glassY, 4);

	for (int i = 0; i < 9; i++) {
		if (i % 2 == 0) iSetColor(20, 20, 20);
		else iSetColor(255, 255, 255);
		iFilledRectangle(vx + 7 + (i * 6), vy + 33, 6, 6);
	}

	iSetColor(245, 245, 245);
	iFilledRectangle(vx + 22, vy + 17, 20, 8);

	iSetColor(220, 30, 30);
	iFilledRectangle(vx + 6, vy + 22, 14, 7);
	iFilledRectangle(vx + w - 20, vy + 22, 14, 7);

	iSetColor(230, 180, 0);
	iFilledRectangle(vx - 3, vy + 40, 5, 8);
	iFilledRectangle(vx + w - 2, vy + 40, 5, 8);

	iSetColor(240, 240, 240);
	iFilledRectangle(vx + 23, vy + 62, 18, 7);
	iSetColor(20, 20, 20);
	iRectangle(vx + 23, vy + 62, 18, 7);
}

// Rear-View White Van (65 x 70)
void drawVan(int vx, int vy) {
	int w = obstacleWidth;

	iSetColor(30, 30, 30);
	iFilledRectangle(vx + 7, vy, 10, 8);
	iFilledRectangle(vx + w - 17, vy, 10, 8);

	iSetColor(100, 105, 110);
	iFilledRectangle(vx + 4, vy + 6, w - 8, 10);
	iSetColor(40, 40, 40);
	iFilledRectangle(vx + 12, vy + 9, 10, 3);
	iFilledRectangle(vx + w - 22, vy + 9, 10, 3);

	iSetColor(230, 230, 230);
	iFilledRectangle(vx + 10, vy + 60, w - 20, 9);
	iSetColor(180, 180, 180);
	iRectangle(vx + 10, vy + 60, w - 20, 9);
	iSetColor(60, 60, 60);
	iFilledRectangle(vx + 7, vy + 57, 3, 6);
	iFilledRectangle(vx + w - 10, vy + 57, 3, 6);

	iSetColor(245, 245, 250);
	iFilledRectangle(vx + 5, vy + 15, w - 10, 43);

	iSetColor(200, 205, 210);
	iRectangle(vx + 5, vy + 15, w - 10, 43);

	iSetColor(70, 80, 90);
	iFilledRectangle(vx + 9, vy + 37, 21, 16);
	iFilledRectangle(vx + w - 30, vy + 37, 21, 16);

	iSetColor(110, 125, 140);
	iFilledRectangle(vx + 11, vy + 39, 17, 12);
	iFilledRectangle(vx + w - 28, vy + 39, 17, 12);

	iSetColor(140, 145, 150);
	iFilledRectangle(vx + (w / 2), vy + 15, 2, 38);
	iSetColor(50, 50, 50);
	iFilledRectangle(vx + (w / 2) + 3, vy + 26, 3, 7);

	iSetColor(240, 140, 0);
	iFilledRectangle(vx + 1, vy + 22, 4, 14);
	iFilledRectangle(vx + w - 5, vy + 22, 4, 14);

	iSetColor(220, 40, 40);
	iFilledRectangle(vx + 1, vy + 16, 4, 6);
	iFilledRectangle(vx + w - 5, vy + 16, 4, 6);

	iSetColor(70, 70, 70);
	iFilledRectangle(vx - 3, vy + 37, 4, 7);
	iFilledRectangle(vx + w - 1, vy + 37, 4, 7);
}

void drawObstacleVehicle(Obstacle obs) {
	if (!obs.active) return;

	if (obs.type == 0) {
		drawTaxi((int)(obs.x - obstacleWidth / 2.0f), (int)obs.y);
	}
	else {
		drawVan((int)(obs.x - obstacleWidth / 2.0f), (int)obs.y);
	}
}

// Game Over Overlay Interface
void drawGameOverScreen() {
	iSetColor(0, 0, 0);
	iFilledRectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 100, 400, 200);

	iSetColor(220, 20, 20);
	iRectangle(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 100, 400, 200);
	iRectangle(SCREEN_WIDTH / 2 - 198, SCREEN_HEIGHT / 2 - 98, 396, 196);

	iSetColor(255, 40, 40);
	iText(SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 + 40, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	iText(SCREEN_WIDTH / 2 - 130, SCREEN_HEIGHT / 2 - 10, "CRASHED INTO A VEHICLE!", GLUT_BITMAP_HELVETICA_18);
	iSetColor(255, 215, 0);
	iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, "Press 'R' to Try Again", GLUT_BITMAP_HELVETICA_18);
}

// ---------- Render Loop ----------
void iDraw() {
	iClear();

	// 1. Sky
	iSetColor(135, 206, 235);
	iFilledRectangle(0, horizonY, SCREEN_WIDTH, SCREEN_HEIGHT - horizonY);

	// Sun
	iSetColor(255, 200, 0);
	iFilledCircle(760, 540, 35);

	// 2. Grass
	iSetColor(102, 178, 60);
	iFilledRectangle(0, 0, SCREEN_WIDTH, horizonY);

	// 3. Scenery
	drawSideBuildings();
	drawSideTreesAndPoles();

	// 4. Road
	double roadX[] = { 250, 650, (double)SCREEN_WIDTH, 0 };
	double roadY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(70, 70, 70);
	iFilledPolygon(roadX, roadY, 4);

	// 5. Curbs
	double leftCurbX[] = { 250, 260, 15, 0 };
	double leftCurbY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(230, 230, 230);
	iFilledPolygon(leftCurbX, leftCurbY, 4);

	double rightCurbX[] = { 650, 640, SCREEN_WIDTH - 15, (double)SCREEN_WIDTH };
	double rightCurbY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(230, 230, 230);
	iFilledPolygon(rightCurbX, rightCurbY, 4);

	// 6. Yellow Lanes
	iSetColor(255, 215, 0);
	for (float t = lineOffset; t < 1.0f; t += 0.2f) {
		double currentY = horizonY - (t * horizonY);
		double nextY = horizonY - ((t + 0.1f) * horizonY);

		double currentWidth = 2 + (t * 16);
		double nextWidth = 2 + ((t + 0.1f) * 16);

		double lineX[] = { SCREEN_WIDTH / 2.0 - currentWidth, SCREEN_WIDTH / 2.0 + currentWidth, SCREEN_WIDTH / 2.0 + nextWidth, SCREEN_WIDTH / 2.0 - nextWidth };
		double lineY[] = { currentY, currentY, nextY, nextY };

		if (nextY >= 0) {
			iFilledPolygon(lineX, lineY, 4);
		}
	}

	// 7. Active Obstacles
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		drawObstacleVehicle(obstacles[i]);
	}

	// 8. Player Rickshaw
	iShowBMP2(rickshawCordinateX, rickshawCordinateY, rickshaw[rickshawIndex], 0);

	// Start prompt
	if (!isDriving && gameState == 0) {
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 110, 80, "Press UP / W to Start Driving", GLUT_BITMAP_HELVETICA_18);
	}

	// 9. Game Over Overlay
	if (gameState == 1) {
		drawGameOverScreen();
	}
}

// ---------- Logic Update Loop ----------
void updateGame() {
	// Restart key triggers anytime
	if (GetAsyncKeyState('R') & 0x8000) {
		resetGame();
		return;
	}

	// Freeze logic when Game Over occurs
	if (gameState == 1) return;

	// Movement Controls
	if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000)) {
		isDriving = 1;
		lineOffset += scrollSpeed;
		if (lineOffset >= 0.2f) {
			lineOffset -= 0.2f;
		}
		rickshawIndex = (rickshawIndex + 1) % 8;
	}

	if ((GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState('A') & 0x8000)) {
		if (rickshawCordinateX > 50) rickshawCordinateX -= 10;
	}

	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState('D') & 0x8000)) {
		if (rickshawCordinateX < 800) rickshawCordinateX += 10;
	}

	// Vehicle Spawning & Movement
	if (isDriving) {
		obstacleSpawnTimer++;
		if (obstacleSpawnTimer > 40) {
			for (int i = 0; i < MAX_OBSTACLES; i++) {
				if (!obstacles[i].active) {
					obstacles[i].active = 1;
					obstacles[i].y = (float)horizonY;
					obstacles[i].x = 300 + (rand() % 300);
					break;
				}
			}
			obstacleSpawnTimer = 0;
		}

		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (!obstacles[i].active) continue;

			// Move vehicle forward
			obstacles[i].y -= obstacles[i].speed;

			// Perspective spread
			float centerDist = obstacles[i].x - (SCREEN_WIDTH / 2.0f);
			obstacles[i].x += centerDist * 0.005f;

			if (obstacles[i].y < -70) {
				resetObstacle(i);
			}

			// --- STRICT OMNIDIRECTIONAL COLLISION CHECK ---
			float vehicleLeft = obstacles[i].x - (obstacleWidth / 2.0f);
			float vehicleBottom = obstacles[i].y;

			if (checkCollision((float)rickshawCordinateX, (float)rickshawCordinateY,
				(float)rickshawWidth, (float)rickshawHeight,
				vehicleLeft, vehicleBottom,
				(float)obstacleWidth, (float)obstacleHeight)) {
				gameState = 1; // Trigger Game Over on touch from any side
			}
		}
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

	initObstacles();

	iSetTimer(60, updateGame);

	iStart();
	return 0;
}
