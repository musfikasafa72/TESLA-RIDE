#include "iGraphics.h"
#include <windows.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600

int horizonY = 380;
float lineOffset = 0.0f;

// REDUCED: Road scrolling speed for a slower pace
float scrollSpeed = 0.007f;

// Game State: 0 = Playing, 1 = Game Over
int gameState = 0;

// Driving flag: track if player started moving
int isDriving = 0;

// Rickshaw Animation & Position
char rickshaw[8][15] = { "char\\R1.bmp", "char\\R2.bmp", "char\\R3.bmp", "char\\R4.bmp", "char\\R5.bmp", "char\\R6.bmp", "char\\R7.bmp", "char\\R8.bmp" };
int rickshawCordinateX = 425;
int rickshawCordinateY = 10;
int rickshawIndex = 0;

// ==========================================
// Adjustable Dimensions & Scaling Settings
// ==========================================
int rickshawWidth = 60;
int rickshawHeight = 75;

int obstacleWidth = 75;
int obstacleHeight = 85;

float minObstacleScale = 0.70f;
float maxObstacleScale = 1.15f;

// ---------- Obstacle System ----------
#define MAX_OBSTACLES 3

typedef struct {
	float x;
	float y;
	float speed;
	int type; // 0: Yellow Taxi, 1: White Van
	int active;
} Obstacle;

Obstacle obstacles[MAX_OBSTACLES];
int obstacleSpawnTimer = 0;

// ---------- Helper Functions ----------
float getObstacleScale(float y) {
	float progress = (float)(horizonY - y) / (float)horizonY;
	if (progress < 0.0f) progress = 0.0f;
	if (progress > 1.0f) progress = 1.0f;
	return minObstacleScale + (maxObstacleScale - minObstacleScale) * progress;
}

void resetObstacle(int index) {
	obstacles[index].y = (float)horizonY;

	// Tighter horizontal spawn range near horizon (X = 400 to 500)
	obstacles[index].x = 400.0f + (rand() % 100);

	// Obstacle speed (1.0 - 2.0)
	obstacles[index].speed = 1.0f + (rand() % 50) / 50.0f;

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

// REALISTIC FULL-SURFACE COLLISION DETECTION
// Strict Axis-Aligned Bounding Box (AABB) overlap check without margin padding
int checkCollision(float rX, float rY, float rW, float rH, float oX, float oY, float oW, float oH) {
	float rLeft = rX;
	float rRight = rX + rW;
	float rBottom = rY;
	float rTop = rY + rH;

	float oLeft = oX;
	float oRight = oX + oW;
	float oBottom = oY;
	float oTop = oY + oH;

	// Triggers if ANY part of the bounding boxes overlap
	if (rRight >= oLeft && rLeft <= oRight && rTop >= oBottom && rBottom <= oTop) {
		return 1;
	}
	return 0;
}

// ---------- Visual Elements ----------
void drawSideBuildings() {
	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.2f + lineOffset, 1.0f);
		if (progress < 0) progress += 1.0f;

		int bw = (int)(60 - progress * 35);
		int bh = (int)(180 - progress * 90);
		int bx = (int)(10 + progress * 160);
		int by = (int)(10 + progress * 300);

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

		int bw = (int)(60 - progress * 35);
		int bh = (int)(180 - progress * 90);
		int bx = (int)(SCREEN_WIDTH - 10 - bw - progress * 160);
		int by = (int)(10 + progress * 300);

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

		int tx = (int)(20 + progress * 200);
		int ty = (int)(20 + progress * 300);
		int scale = (int)((1.0f - progress) * 35 + 10);

		if (i % 2 == 0) {
			iSetColor(101, 67, 33);
			iFilledRectangle(tx - 3, ty, 6, scale);
			iSetColor(34, 139, 34);
			iFilledCircle(tx, ty + scale, (int)(scale * 0.7));
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

		int tx = (int)(SCREEN_WIDTH - 20 - progress * 200);
		int ty = (int)(20 + progress * 300);
		int scale = (int)((1.0f - progress) * 35 + 10);

		if (i % 2 == 0) {
			iSetColor(101, 67, 33);
			iFilledRectangle(tx - 3, ty, 6, scale);
			iSetColor(34, 139, 34);
			iFilledCircle(tx, ty + scale, (int)(scale * 0.7));
		}
		else {
			iSetColor(90, 90, 90);
			iFilledRectangle(tx - 2, ty, 4, scale + 15);
			iSetColor(60, 60, 60);
			iFilledRectangle(tx - 10, ty + scale + 10, 20, 3);
		}
	}
}

// Yellow Taxi Drawing (Unchanged)
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

// White Van Drawing (Unchanged)
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

// Unchanged Drawing Router
void drawObstacleVehicle(Obstacle obs) {
	if (!obs.active) return;

	if (obs.type == 0) {
		drawTaxi((int)(obs.x - obstacleWidth / 2.0f), (int)obs.y);
	}
	else {
		drawVan((int)(obs.x - obstacleWidth / 2.0f), (int)obs.y);
	}
}

// Game Over Overlay
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

	// Sky
	iSetColor(135, 206, 235);
	iFilledRectangle(0, horizonY, SCREEN_WIDTH, SCREEN_HEIGHT - horizonY);

	// Sun
	iSetColor(255, 200, 0);
	iFilledCircle(760, 540, 35);

	// Grass
	iSetColor(102, 178, 60);
	iFilledRectangle(0, 0, SCREEN_WIDTH, horizonY);

	// Scenery
	drawSideBuildings();
	drawSideTreesAndPoles();

	// Road
	double roadX[] = { 250, 650, (double)SCREEN_WIDTH, 0 };
	double roadY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(70, 70, 70);
	iFilledPolygon(roadX, roadY, 4);

	// Curbs
	double leftCurbX[] = { 250, 260, 15, 0 };
	double leftCurbY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(230, 230, 230);
	iFilledPolygon(leftCurbX, leftCurbY, 4);

	double rightCurbX[] = { 650, 640, SCREEN_WIDTH - 15, (double)SCREEN_WIDTH };
	double rightCurbY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(230, 230, 230);
	iFilledPolygon(rightCurbX, rightCurbY, 4);

	// Yellow Road Lanes
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

	// Active Obstacles
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		drawObstacleVehicle(obstacles[i]);
	}

	// Player Rickshaw
	iShowBMP2(rickshawCordinateX, rickshawCordinateY, rickshaw[rickshawIndex], 0);

	// Start prompt
	if (!isDriving && gameState == 0) {
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 110, 80, "Press UP / W to Start Driving", GLUT_BITMAP_HELVETICA_18);
	}

	// Game Over Overlay
	if (gameState == 1) {
		drawGameOverScreen();
	}
}

// ---------- Logic Update Loop ----------
void updateGame() {
	if (GetAsyncKeyState('R') & 0x8000) {
		resetGame();
		return;
	}

	if (gameState == 1) return;

	// Driving Controls
	if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000)) {
		isDriving = 1;
		lineOffset += scrollSpeed;
		if (lineOffset >= 0.2f) {
			lineOffset -= 0.2f;
		}
		rickshawIndex = (rickshawIndex + 1) % 8;
	}

	// Steering Controls
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState('A') & 0x8000)) {
		if (rickshawCordinateX > 40) rickshawCordinateX -= 6;
	}

	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState('D') & 0x8000)) {
		if (rickshawCordinateX < SCREEN_WIDTH - rickshawWidth - 40) rickshawCordinateX += 6;
	}

	if (isDriving) {
		// Spawn Timer
		obstacleSpawnTimer++;
		if (obstacleSpawnTimer > 80) {
			for (int i = 0; i < MAX_OBSTACLES; i++) {
				if (!obstacles[i].active) {
					obstacles[i].active = 1;
					obstacles[i].y = (float)horizonY;

					// Tighter horizontal spawn range near horizon (X = 400 to 500)
					obstacles[i].x = 400.0f + (rand() % 100);
					break;
				}
			}
			obstacleSpawnTimer = 0;
		}

		// Update Obstacles & Check Collisions
		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (!obstacles[i].active) continue;

			// 1. Move downward
			float scale = getObstacleScale(obstacles[i].y);
			obstacles[i].y -= obstacles[i].speed * (0.5f + scale);

			// 2. Reduced perspective outward expansion factor
			float centerDist = obstacles[i].x - (SCREEN_WIDTH / 2.0f);
			obstacles[i].x += centerDist * 0.0055f;

			// 3. Strict Dynamic Boundary Clamping (keeps vehicles on the road)
			float roadProgress = (float)(horizonY - obstacles[i].y) / (float)horizonY;
			if (roadProgress < 0.0f) roadProgress = 0.0f;

			float scaledW = obstacleWidth * scale;
			float minRoadX = (260.0f * (1.0f - roadProgress)) + (scaledW / 2.0f);
			float maxRoadX = (640.0f * (1.0f - roadProgress)) + (SCREEN_WIDTH * roadProgress) - (scaledW / 2.0f);

			if (obstacles[i].x < minRoadX) obstacles[i].x = minRoadX;
			if (obstacles[i].x > maxRoadX) obstacles[i].x = maxRoadX;

			// 4. Deactivate if off the bottom
			if (obstacles[i].y < -120) {
				resetObstacle(i);
				continue;
			}

			// 5. ACCURATE SYNCHRONIZED BOUNDING BOX
			// Exactly matches drawObstacleVehicle render placement
			float obstacleLeft = obstacles[i].x - (obstacleWidth / 2.0f);
			float obstacleBottom = obstacles[i].y;

			// 6. Collision Check (triggers on any visual overlap)
			if (checkCollision((float)rickshawCordinateX, (float)rickshawCordinateY,
				(float)rickshawWidth, (float)rickshawHeight,
				obstacleLeft, obstacleBottom,
				(float)obstacleWidth, (float)obstacleHeight)) {
				gameState = 1;
			}
		}
	}
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}
void iMouse(int button, int state, int mx, int my) {}

void iKeyboard(unsigned char key) {
	if (key == 'r' || key == 'R') {
		resetGame();
	}
}

void iSpecialKeyboard(unsigned char key) {}
void fixedUpdate() {}

int main() {
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Perspective Road View");
	initObstacles();
	iSetTimer(15, updateGame);
	iStart();
	return 0;
}
