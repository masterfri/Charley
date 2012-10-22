#pragma once

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "include.h"
#include "controller.h"

struct DisplayOptions 
{
	GLint 	width,
			height,
			flags;
};

class Display
{
	protected:
		static Display * instance;
		GLshort mode;
		const SDL_VideoInfo *info;
		SDL_Surface *screen,
					  *icon;
	
	public:
		static Display * getInstance();
		static GLvoid createInstance(DisplayOptions &);
		static GLvoid DestroyInstance();
	
		Display(DisplayOptions &);
		~Display();
		GLint GetWidth();
		GLint GetHeight();
		SDL_Surface * GetScreen();
		GLvoid LigthModel(GLfloat);
};

inline GLvoid Display::createInstance(DisplayOptions & options)
{
	instance = new Display(options);
}

inline Display * Display::getInstance()
{
	if (!instance) {
		fprintf(stderr, "Error: Display is not configured \n");
		Controller::getInstance()->QuitGame(1);
	}
	return instance;
}

inline GLvoid Display::DestroyInstance()
{
	if (instance) {
		delete instance;
	}
}

inline GLint Display::GetWidth()
{
	return screen->w;
}

inline GLint Display::GetHeight()
{
	return screen->h;
}

inline SDL_Surface * Display::GetScreen()
{
	return screen;
}

inline GLvoid Display::LigthModel(GLfloat val)
{
	GLfloat a[4];
	a[0] = a[1] = a[2] = val;
	a[3] = 0;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, a);
}

#endif // _DISPLAY_H_
