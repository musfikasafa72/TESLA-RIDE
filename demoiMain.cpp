#include "iGraphics.h"

void iDraw()
{

}

void iMouseMove(int mx, int my)
{
	
}

void iPassiveMouseMove(int mx, int my)
{
	
}

void iMouse(int button, int state, int mx, int my)
{
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

		
	}
	
	
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		
	}
}

// Special Keys:
// GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12, 
// GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT

void fixedUpdate()
{
	if (isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP))
	{

	}
	if (isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT))
	{
		
	}
	if (isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		
	}
	if (isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT))
	{
		
	}

	if (isKeyPressed(' ')) {
		// Playing the audio once
		//mciSendString("play ggsong from 0", NULL, 0, NULL);
	}
}


int main()
{
	// Opening/Loading the audio files
	mciSendString("open \"Audios//background.mp3\" alias bgsong", NULL, 0, NULL);
	mciSendString("open \"Audios//gameover.mp3\" alias ggsong", NULL, 0, NULL);

	// Playing the background audio on repeat
	mciSendString("play bgsong repeat", NULL, 0, NULL);

	// If the use of an audio is finished, close it to free memory
	// mciSendString("close bgsong", NULL, 0, NULL);
	// mciSendString("close ggsong", NULL, 0, NULL);

	iInitialize(800, 420, "Project Title");
	iStart();
	return 0;
}