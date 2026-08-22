#include "iGraphics.h"
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>

int WIDTH = 900;
int HEIGHT = 600;
int HORIZON_Y = 380;   // Where road meets sky

// Game State: 0 = Playing, 1 = Game Over
int gameState = 0;

// Auto-rickshaw position and horizontal speed
float rickshawX = 450;
float rickshawY = 30;   // Fixed vertical position for driving view
float moveSpeedX = 6.0;

// Dynamic motion control
float roadOffset = 0.0;
float roadSpeed = 7.0;  // Speed of background elements moving

// ---------- Obstacle System ----------
#define MAX_OBSTACLES 3

typedef struct {
	float x;
	float y;            // Y position on screen
	float speed;        // Movement speed multiplier
	int type;           // 0: Taxi, 1: Van
	int active;         // 1 if active
} Obstacle;

Obstacle obstacles[MAX_OBSTACLES];

// ---------- Function Declarations ----------
void resetObstacle(int index);
void initObstacles();
void resetGame();
int checkCollision(float rX, float rY, float rW, float rH, float oX, float oY, float oW, float oH);

// Helper to spawn/reset an obstacle at the horizon
void resetObstacle(int index) {
	obstacles[index].y = (float)HORIZON_Y;
	obstacles[index].x = 300 + (rand() % 300);
	obstacles[index].speed = 2.0f + (rand() % 300) / 100.0f;
	obstacles[index].type = rand() % 2; // 0: Taxi, 1: Van
	obstacles[index].active = 1;
}

void initObstacles() {
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		resetObstacle(i);
		obstacles[i].y = HORIZON_Y - (i * 120);
	}
}

// Axis-Aligned Bounding Box (AABB) Collision Check
int checkCollision(float rX, float rY, float rW, float rH, float oX, float oY, float oW, float oH)
{
	if (rX < oX + oW &&
		rX + rW > oX &&
		rY < oY + oH &&
		rY + rH > oY)
	{
		return 1; // Collision detected
	}
	return 0;
}

// Reset everything to start fresh
void resetGame()
{
	rickshawX = 450;
	rickshawY = 30;
	roadOffset = 0.0;

	for (int i = 0; i < MAX_OBSTACLES; i++) {
		resetObstacle(i);
		obstacles[i].y = HORIZON_Y - (i * 120);
	}

	gameState = 0; // Return to Playing state
}

// ---------- Helper Functions ----------
float getRoadWidthAtY(float y)
{
	float t = (y - 0) / (float)HORIZON_Y;
	return (1.0f - t) * WIDTH + t * 400.0f;
}

// ---------- Audio Control ----------
void startEngineSound()
{
	mciSendString("open \"engine.wav\" type waveaudio alias bg_audio", NULL, 0, NULL);
	mciSendString("play bg_audio repeat", NULL, 0, NULL);
}

void stopEngineSound()
{
	mciSendString("stop bg_audio", NULL, 0, NULL);
	mciSendString("close bg_audio", NULL, 0, NULL);
}

// ---------- Background Drawing ----------
void drawSky()
{
	iSetColor(135, 206, 235);
	iFilledRectangle(0, HORIZON_Y, WIDTH, HEIGHT - HORIZON_Y);

	// Sun
	iSetColor(255, 223, 0);
	iFilledCircle(760, 540, 35);

	// Clouds
	iSetColor(255, 255, 255);
	iFilledCircle(150, 500, 20);
	iFilledCircle(175, 505, 25);
	iFilledCircle(200, 500, 18);

	iFilledCircle(550, 470, 15);
	iFilledCircle(570, 475, 20);
	iFilledCircle(590, 470, 14);
}

void drawGrassGround()
{
	iSetColor(102, 178, 60);
	iFilledRectangle(0, 0, WIDTH, HORIZON_Y);
}

void drawRoad()
{
	// Trapezoid Road
	iSetColor(70, 70, 70);
	double roadX[4] = { 250, 650, WIDTH, 0 };
	double roadY[4] = { HORIZON_Y, HORIZON_Y, 0, 0 };
	iFilledPolygon(roadX, roadY, 4);

	// Curbs
	iSetColor(230, 230, 230);
	double curbL_x[4] = { 250, 260, 15, 0 };
	double curbL_y[4] = { HORIZON_Y, HORIZON_Y, 0, 0 };
	iFilledPolygon(curbL_x, curbL_y, 4);

	double curbR_x[4] = { 650, 640, WIDTH - 15, WIDTH };
	double curbR_y[4] = { HORIZON_Y, HORIZON_Y, 0, 0 };
	iFilledPolygon(curbR_x, curbR_y, 4);

	// Center Dashed Line
	iSetColor(255, 255, 255);
	for (int i = 0; i < 10; i++)
	{
		float t = fmod((i / 8.0f) - (roadOffset / 300.0f), 1.0f);
		if (t < 0) t += 1.0f;

		int lineY = (int)(20 + t * (HORIZON_Y - 50));
		int lineW = (int)(18 - t * 14);
		int lineH = (int)(18 - t * 12);
		if (lineW < 3) lineW = 3;
		if (lineH < 3) lineH = 3;

		iFilledRectangle(WIDTH / 2 - lineW / 2, lineY, lineW, lineH);
	}
}

void drawSideBuildings()
{
	for (int i = 0; i < 5; i++)
	{
		float progress = fmod(i * 0.2f + (roadOffset / 400.0f), 1.0f);
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
		for (int c = 0; c < bw / 18; c++)
		{
			for (int r = 0; r < bh / 25; r++)
			{
				iFilledRectangle(bx + 4 + c * 16, by + 5 + r * 22, 6, 10);
			}
		}
	}

	for (int i = 0; i < 5; i++)
	{
		float progress = fmod(i * 0.2f + (roadOffset / 400.0f), 1.0f);
		if (progress < 0) progress += 1.0f;

		int bw = 60 - progress * 35;
		int bh = 180 - progress * 90;
		int bx = WIDTH - 10 - bw - progress * 160;
		int by = 10 + progress * 300;

		iSetColor(170, 110, 85);
		iFilledRectangle(bx, by, bw, bh);

		iSetColor(100, 50, 40);
		iFilledRectangle(bx, by + bh, bw, 6);

		iSetColor(255, 230, 150);
		for (int c = 0; c < bw / 18; c++)
		{
			for (int r = 0; r < bh / 25; r++)
			{
				iFilledRectangle(bx + 4 + c * 16, by + 5 + r * 22, 6, 10);
			}
		}
	}
}

void drawSideTreesAndPoles()
{
	for (int i = 0; i < 5; i++)
	{
		float progress = fmod(i * 0.25f + (roadOffset / 300.0f), 1.0f);
		if (progress < 0) progress += 1.0f;

		int tx = 20 + progress * 200;
		int ty = 20 + progress * 300;
		int scale = (1.0f - progress) * 35 + 10;

		if (i % 2 == 0)
		{
			iSetColor(101, 67, 33);
			iFilledRectangle(tx - 3, ty, 6, scale);
			iSetColor(34, 139, 34);
			iFilledCircle(tx, ty + scale, scale * 0.7);
		}
		else
		{
			iSetColor(90, 90, 90);
			iFilledRectangle(tx - 2, ty, 4, scale + 15);
			iSetColor(60, 60, 60);
			iFilledRectangle(tx - 10, ty + scale + 10, 20, 3);
		}
	}

	for (int i = 0; i < 5; i++)
	{
		float progress = fmod(i * 0.25f + (roadOffset / 300.0f), 1.0f);
		if (progress < 0) progress += 1.0f;

		int tx = WIDTH - 20 - progress * 200;
		int ty = 20 + progress * 300;
		int scale = (1.0f - progress) * 35 + 10;

		if (i % 2 == 0)
		{
			iSetColor(101, 67, 33);
			iFilledRectangle(tx - 3, ty, 6, scale);
			iSetColor(34, 139, 34);
			iFilledCircle(tx, ty + scale, scale * 0.7);
		}
		else
		{
			iSetColor(90, 90, 90);
			iFilledRectangle(tx - 2, ty, 4, scale + 15);
			iSetColor(60, 60, 60);
			iFilledRectangle(tx - 10, ty + scale + 10, 20, 3);
		}
	}
}

void drawBackground()
{
	drawSky();
	drawGrassGround();
	drawSideBuildings();
	drawSideTreesAndPoles();
	drawRoad();
}

// ---------- Vehicle Drawings ----------
void drawVan(int vx, int vy, int w, int h)
{
	iSetColor(40, 40, 40);
	iFilledRectangle(vx + w * 0.05, vy, w * 0.18, h * 0.12);
	iFilledRectangle(vx + w * 0.77, vy, w * 0.18, h * 0.12);

	iSetColor(100, 100, 100);
	iFilledRectangle(vx, vy + h * 0.08, w, h * 0.12);

	iSetColor(240, 240, 240);
	iFilledRectangle(vx + w * 0.05, vy + h * 0.2, w * 0.9, h * 0.62);

	iSetColor(60, 60, 60);
	iFilledRectangle(vx - w * 0.08, vy + h * 0.45, w * 0.08, h * 0.15);
	iFilledRectangle(vx + w, vy + h * 0.45, w * 0.08, h * 0.15);

	iSetColor(80, 90, 100);
	iFilledRectangle(vx + w * 0.12, vy + h * 0.5, w * 0.35, h * 0.28);
	iFilledRectangle(vx + w * 0.53, vy + h * 0.5, w * 0.35, h * 0.28);

	iSetColor(160, 160, 160);
	iRectangle(vx + w * 0.5, vy + h * 0.2, 1, h * 0.62);

	iSetColor(255, 140, 0);
	iFilledRectangle(vx + w * 0.01, vy + h * 0.25, w * 0.04, h * 0.18);
	iFilledRectangle(vx + w * 0.95, vy + h * 0.25, w * 0.04, h * 0.18);

	iSetColor(220, 20, 20);
	iFilledRectangle(vx + w * 0.01, vy + h * 0.12, w * 0.04, h * 0.13);
	iFilledRectangle(vx + w * 0.95, vy + h * 0.12, w * 0.04, h * 0.13);

	iSetColor(200, 200, 200);
	iFilledRectangle(vx + w * 0.1, vy + h * 0.85, w * 0.8, h * 0.12);
	iSetColor(70, 70, 70);
	iRectangle(vx + w * 0.08, vy + h * 0.82, w * 0.84, h * 0.04);
}

void drawTaxi(int vx, int vy, int w, int h)
{
	iSetColor(30, 30, 30);
	iFilledRectangle(vx + w * 0.06, vy, w * 0.2, h * 0.15);
	iFilledRectangle(vx + w * 0.74, vy, w * 0.2, h * 0.15);

	iSetColor(255, 204, 0);
	iFilledRectangle(vx, vy + h * 0.12, w, h * 0.45);
	iFilledRectangle(vx + w * 0.12, vy + h * 0.55, w * 0.76, h * 0.3);

	iSetColor(50, 60, 75);
	iFilledRectangle(vx + w * 0.18, vy + h * 0.58, w * 0.64, h * 0.22);

	iSetColor(0, 0, 0);
	for (int i = 0; i < 7; i++) {
		if (i % 2 == 0) {
			iFilledRectangle(vx + (i * (w / 7.0f)), vy + h * 0.48, w / 7.0f, h * 0.07);
		}
	}

	iSetColor(230, 0, 0);
	iFilledRectangle(vx + w * 0.04, vy + h * 0.28, w * 0.22, h * 0.12);
	iFilledRectangle(vx + w * 0.74, vy + h * 0.28, w * 0.22, h * 0.12);

	iSetColor(240, 240, 240);
	iFilledRectangle(vx + w * 0.38, vy + h * 0.18, w * 0.24, h * 0.1);

	iSetColor(240, 240, 240);
	iFilledRectangle(vx + w * 0.35, vy + h * 0.85, w * 0.3, h * 0.12);
	iSetColor(0, 0, 0);
	iRectangle(vx + w * 0.35, vy + h * 0.85, w * 0.3, h * 0.12);
}

void drawObstacleVehicle(Obstacle obs)
{
	if (!obs.active) return;

	float progress = (HORIZON_Y - obs.y) / (float)HORIZON_Y;
	if (progress < 0) progress = 0;
	if (progress > 1) progress = 1;

	int w, h;

	if (obs.type == 0) {
		w = 40 + progress * 50;
		h = 30 + progress * 40;
		int vx = (int)(obs.x - w / 2.0f);
		int vy = (int)obs.y;
		drawTaxi(vx, vy, w, h);
	}
	else {
		w = 42 + progress * 52;
		h = 45 + progress * 55;
		int vx = (int)(obs.x - w / 2.0f);
		int vy = (int)obs.y;
		drawVan(vx, vy, w, h);
	}
}

void drawBlackYellowAutoRickshaw(float x, float y)
{
	int w = 110;
	int h = 130;
	int rx = (int)x - w / 2;
	int ry = (int)y;

	iSetColor(30, 30, 30);
	iFilledRectangle(rx + 8, ry, 16, 32);
	iFilledRectangle(rx + w - 24, ry, 16, 32);

	iSetColor(200, 205, 210);
	iFilledRectangle(rx + 4, ry + 22, 24, 12);
	iFilledRectangle(rx + w - 28, ry + 22, 24, 12);

	iSetColor(200, 30, 30);
	for (int i = 0; i < 4; i++) {
		iFilledRectangle(rx + 22, ry + 6 + (i * 4), 6, 2);
		iFilledRectangle(rx + w - 28, ry + 6 + (i * 4), 6, 2);
	}

	iSetColor(0, 85, 180);
	iFilledRectangle(rx + 12, ry + 28, w - 24, 42);

	iSetColor(0, 60, 140);
	iRectangle(rx + 14, ry + 32, w - 28, 34);
	iFilledRectangle(rx + 12, ry + 28, w - 24, 4);

	iSetColor(255, 140, 0);
	iFilledRectangle(rx + 18, ry + 42, 12, 10);
	iFilledRectangle(rx + w - 30, ry + 42, 12, 10);

	iSetColor(220, 20, 20);
	iFilledRectangle(rx + 18, ry + 34, 12, 8);
	iFilledRectangle(rx + w - 30, ry + 34, 12, 8);

	iSetColor(245, 245, 245);
	iFilledRectangle(rx + (w / 2) - 18, ry + 34, 36, 18);
	iSetColor(30, 30, 30);
	iRectangle(rx + (w / 2) - 18, ry + 34, 36, 18);

	iSetColor(200, 25, 25);
	iFilledRectangle(rx + 14, ry + 66, w - 28, 12);
	iSetColor(160, 15, 15);
	iFilledRectangle(rx + 14, ry + 66, w - 28, 3);

	iSetColor(190, 20, 20);
	iFilledRectangle(rx + 22, ry + 82, 30, 20);
	iFilledRectangle(rx + w - 52, ry + 82, 30, 20);

	iSetColor(40, 70, 50);
	iFilledRectangle(rx + 24, ry + 88, 26, 18);
	iSetColor(40, 30, 25);
	iFilledCircle(rx + 37, ry + 110, 8);

	iSetColor(210, 180, 140);
	iFilledRectangle(rx + w - 50, ry + 88, 26, 18);
	iSetColor(30, 25, 20);
	iFilledCircle(rx + w - 37, ry + 110, 8);

	iSetColor(30, 30, 30);
	iFilledRectangle(rx + 14, ry + 70, 4, 45);
	iFilledRectangle(rx + w - 18, ry + 70, 4, 45);

	iSetColor(220, 220, 220);
	iFilledRectangle(rx + 6, ry + 95, 5, 10);
	iFilledRectangle(rx + w - 11, ry + 95, 5, 10);

	iSetColor(0, 102, 204);
	iFilledRectangle(rx + 8, ry + 110, w - 16, 20);
	iFilledCircle(rx + 20, ry + 120, 12);
	iFilledCircle(rx + w - 20, ry + 120, 12);
	iFilledRectangle(rx + 20, ry + 120, w - 40, 12);

	iSetColor(51, 153, 255);
	iFilledRectangle(rx + 25, ry + 126, w - 50, 4);
}

// ---------- UI Overlay ----------
void drawGameOverScreen()
{
	iSetColor(0, 0, 0);
	iFilledRectangle(WIDTH / 2 - 200, HEIGHT / 2 - 120, 400, 240);

	iSetColor(220, 20, 20);
	iRectangle(WIDTH / 2 - 200, HEIGHT / 2 - 120, 400, 240);
	iRectangle(WIDTH / 2 - 196, HEIGHT / 2 - 116, 392, 232);

	iSetColor(255, 50, 50);
	iText(WIDTH / 2 - 70, HEIGHT / 2 + 60, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	iText(WIDTH / 2 - 110, HEIGHT / 2 + 10, "CRASHED INTO TRAFFIC!", GLUT_BITMAP_HELVETICA_18);

	iSetColor(0, 150, 70);
	iFilledRectangle(WIDTH / 2 - 90, HEIGHT / 2 - 80, 180, 45);

	iSetColor(255, 255, 255);
	iRectangle(WIDTH / 2 - 90, HEIGHT / 2 - 80, 180, 45);
	iText(WIDTH / 2 - 75, HEIGHT / 2 - 62, "RESTART (Press R)", GLUT_BITMAP_HELVETICA_12);
}

// ---------- Game Logic Updates ----------
void updateObstacles() {
	if (gameState != 0) return;

	int isDrivingForward = (isSpecialKeyPressed(GLUT_KEY_UP) || isKeyPressed('w') || isKeyPressed('W'));
	int isDrivingBackward = (isSpecialKeyPressed(GLUT_KEY_DOWN) || isKeyPressed('s') || isKeyPressed('S'));

	float playerW = 80.0f;
	float playerH = 90.0f;
	float playerX = rickshawX - playerW / 2.0f;
	float playerY = rickshawY;

	for (int i = 0; i < MAX_OBSTACLES; i++) {
		if (!obstacles[i].active) continue;

		if (isDrivingForward) {
			obstacles[i].y -= (roadSpeed + obstacles[i].speed);
			float centerDist = obstacles[i].x - (WIDTH / 2.0f);
			obstacles[i].x += centerDist * 0.005f;
		}
		else if (isDrivingBackward) {
			obstacles[i].y += (roadSpeed - obstacles[i].speed);
		}

		if (obstacles[i].y < -60) {
			resetObstacle(i);
		}

		float progress = (HORIZON_Y - obstacles[i].y) / (float)HORIZON_Y;
		if (progress < 0) progress = 0;
		if (progress > 1) progress = 1;

		float obsW = (obstacles[i].type == 0) ? (40 + progress * 50) : (42 + progress * 52);
		float obsH = (obstacles[i].type == 0) ? (30 + progress * 40) : (45 + progress * 55);
		float obsX = obstacles[i].x - obsW / 2.0f;
		float obsY = obstacles[i].y;

		if (checkCollision(playerX, playerY, playerW, playerH, obsX, obsY, obsW, obsH)) {
			gameState = 1;
		}
	}
}

void fixedUpdate()
{
	if (isKeyPressed('r') || isKeyPressed('R'))
	{
		resetGame();
		return;
	}

	if (gameState == 1) return;

	updateObstacles();

	if (isSpecialKeyPressed(GLUT_KEY_UP) || isKeyPressed('w') || isKeyPressed('W'))
	{
		roadOffset += roadSpeed;
	}

	if (isSpecialKeyPressed(GLUT_KEY_DOWN) || isKeyPressed('s') || isKeyPressed('S'))
	{
		roadOffset -= roadSpeed;
	}

	float roadMargin = getRoadWidthAtY(rickshawY) / 2.0f - 50;
	float minX = (WIDTH / 2.0f) - roadMargin;
	float maxX = (WIDTH / 2.0f) + roadMargin;

	if (isSpecialKeyPressed(GLUT_KEY_LEFT) || isKeyPressed('a') || isKeyPressed('A'))
	{
		if (rickshawX > minX)
			rickshawX -= moveSpeedX;
	}

	if (isSpecialKeyPressed(GLUT_KEY_RIGHT) || isKeyPressed('d') || isKeyPressed('D'))
	{
		if (rickshawX < maxX)
			rickshawX += moveSpeedX;
	}

	if (isKeyPressed(27))
	{
		stopEngineSound();
		exit(0);
	}
}

// ---------- iGraphics Mandatory Callbacks ----------
void iDraw()
{
	iClear();
	drawBackground();

	for (int i = 0; i < MAX_OBSTACLES; i++) {
		drawObstacleVehicle(obstacles[i]);
	}

	drawBlackYellowAutoRickshaw(rickshawX, rickshawY);

	if (gameState == 1) {
		drawGameOverScreen();
	}
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}

void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (gameState == 1)
		{
			if (mx >= WIDTH / 2 - 90 && mx <= WIDTH / 2 + 90 &&
				my >= HEIGHT / 2 - 80 && my <= HEIGHT / 2 - 35)
			{
				resetGame();
			}
		}
	}
}

int main()
{
	iInitialize(WIDTH, HEIGHT, "Auto-Rickshaw Driving Game");

	initObstacles();
	startEngineSound();

	iStart();
	return 0;
}
