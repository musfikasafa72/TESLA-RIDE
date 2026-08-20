#include "iGraphics.h"
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>

int WIDTH = 900;
int HEIGHT = 600;

int HORIZON_Y = 380;   // Where road meets sky

// Auto-rickshaw position and horizontal speed
float rickshawX = 450;
float rickshawY = 30;   // Fixed vertical position for driving view
float moveSpeedX = 6.0;

// Dynamic motion control
float roadOffset = 0.0;
float roadSpeed = 7.0;  // Speed of background elements moving

// ---------- Helper Functions ----------
float getRoadWidthAtY(float y)
{
	float t = (y - 0) / (float)HORIZON_Y;
	return (1.0f - t) * WIDTH + t * 400.0f;
}

// ---------- Audio Control (MCI API - Error Free) ----------
void startEngineSound()
{
	// Opens and plays engine.wav (or engine.mp3) in a loop
	mciSendString("open \"engine.wav\" type waveaudio alias bg_audio", NULL, 0, NULL);
	mciSendString("play bg_audio repeat", NULL, 0, NULL);
}

void stopEngineSound()
{
	mciSendString("stop bg_audio", NULL, 0, NULL);
	mciSendString("close bg_audio", NULL, 0, NULL);
}

// ---------- Background ----------
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

// ---------- Black & Yellow Auto-Rickshaw (Rear View) ----------
void drawBlackYellowAutoRickshaw(float x, float y)
{
	// Ground Shadow
	iSetColor(40, 40, 40);
	iFilledEllipse(x, y + 2, 70, 8);

	// --- Bottom Wheels / Mud Flaps ---
	iSetColor(15, 15, 15);
	iFilledRectangle(x - 48, y + 5, 10, 20);
	iFilledRectangle(x - 5, y + 5, 10, 20);
	iFilledRectangle(x + 38, y + 5, 10, 20);

	// --- Yellow Bottom Bumper Beam ---
	iSetColor(255, 204, 0);
	iFilledRectangle(x - 52, y + 23, 104, 8);

	// --- Lower Black Body Panel ---
	iSetColor(20, 20, 20);
	iFilledRectangle(x - 52, y + 31, 104, 32);

	// --- Yellow Accent Band Across Rear Body ---
	iSetColor(255, 204, 0);
	iFilledRectangle(x - 52, y + 63, 104, 14);

	// --- Tail Lights & Indicators ---
	iSetColor(255, 165, 0); // Indicator (Amber)
	iFilledRectangle(x - 46, y + 70, 6, 5);
	iSetColor(220, 20, 20);  // Brake light (Red)
	iFilledRectangle(x - 46, y + 65, 6, 5);

	iSetColor(255, 165, 0);
	iFilledRectangle(x + 40, y + 70, 6, 5);
	iSetColor(220, 20, 20);
	iFilledRectangle(x + 40, y + 65, 6, 5);

	// --- Upper Cabin Canopy ---
	iSetColor(20, 20, 20);
	iFilledRectangle(x - 52, y + 77, 104, 60);

	// Roof Top Curved Cap
	iSetColor(20, 20, 20);
	iFilledEllipse(x, y + 137, 52, 10);

	// --- Oval Rear Window ---
	iSetColor(40, 40, 40);
	iFilledEllipse(x, y + 108, 25, 12);
	iSetColor(160, 165, 170);
	iFilledEllipse(x, y + 108, 22, 10);

	// --- Side Mirrors ---
	iSetColor(20, 20, 20);
	iLine(x - 52, y + 112, x - 62, y + 118);
	iFilledRectangle(x - 66, y + 112, 5, 12);

	iLine(x + 52, y + 112, x + 62, y + 118);
	iFilledRectangle(x + 61, y + 112, 5, 12);
}

// ---------- Controls & Logic ----------
void iDraw()
{
	iClear();
	drawBackground();
	drawBlackYellowAutoRickshaw(rickshawX, rickshawY);
}

void fixedUpdate()
{
	// FORWARD CONTROL
	if (isSpecialKeyPressed(GLUT_KEY_UP) || isKeyPressed('w') || isKeyPressed('W'))
	{
		roadOffset += roadSpeed;
	}

	// BACKWARD CONTROL
	if (isSpecialKeyPressed(GLUT_KEY_DOWN) || isKeyPressed('s') || isKeyPressed('S'))
	{
		roadOffset -= roadSpeed;
	}

	// Horizontal boundary limits
	float roadMargin = getRoadWidthAtY(rickshawY) / 2.0f - 50;
	float minX = (WIDTH / 2.0f) - roadMargin;
	float maxX = (WIDTH / 2.0f) + roadMargin;

	// LEFT Movement
	if (isSpecialKeyPressed(GLUT_KEY_LEFT) || isKeyPressed('a') || isKeyPressed('A'))
	{
		if (rickshawX > minX)
			rickshawX -= moveSpeedX;
	}

	// RIGHT Movement
	if (isSpecialKeyPressed(GLUT_KEY_RIGHT) || isKeyPressed('d') || isKeyPressed('D'))
	{
		if (rickshawX < maxX)
			rickshawX += moveSpeedX;
	}

	if (isKeyPressed(27)) // ESC key
	{
		stopEngineSound();
		exit(0);
	}
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}
void iMouse(int button, int state, int mx, int my) {}

int main()
{
	iInitialize(WIDTH, HEIGHT, "Auto-Rickshaw Driving Game");

	// Start background audio (Make sure engine.wav or engine.mp3 is in project folder)
	startEngineSound();

	iStart();
	return 0;
}