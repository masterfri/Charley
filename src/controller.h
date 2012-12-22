#pragma once
#ifndef _CONRTOLLER_H_
#define _CONRTOLLER_H_

#include "config.h"
#include "include.h"
#include "level.h"
#include "screen.h"

#define ST_PLAY 1
#define ST_HIDE_LEVEL 2
#define ST_SHOW_LEVEL 3
#define ST_LEVELCOMPLETE 4
#define ST_PAUSE 5
#define ST_MAP 6
#define ST_MAIN 7
#define ST_CONFIG 8
#define ST_NEXT_LEVEL_MENU 9
#define ST_FADE_IN_SCREEN 10
#define ST_FADE_OUT_SCREEN 11
#define ST_TITLE_SCREEN 12
#define ST_AFTER_TITLE_SCREEN 13
#define ST_AFTER_LOADING 14
#define ST_CREDITS_SCREEN 15
#define ST_END_TITLE_SCREEN 16
#define ST_END_CREDITS_SCREEN 17

#define KEY_LEFT			0
#define KEY_RIGHT			1
#define KEY_WALK			2
#define KEY_CAMERA_LEFT		3
#define KEY_CAMERA_RIGHT	4
#define KEY_CAMERA_NEAR		5
#define KEY_CAMERA_AWAY		6
#define KEY_CAMERA_UP		7
#define KEY_CAMERA_DOWN		8

#define KEY_STATES	9

class Controller
{
	protected:
		static Controller *instance;
	
		GLshort 	game_state,
					next_game_state,
					next_video_mode;
		GLboolean 	key_pressed[KEY_STATES];
		GLuint		timer_value,
					time_past,
					timer;
		char 		current_level[16],
					next_level[16];
		Level *		level;
		Screen *	screen;
		
		GLvoid DrawScreen();
		GLvoid HandleEvents();
		GLvoid DoActions();
		GLvoid DoActionsPlaystate();
		GLvoid HandleKeyDown(SDL_keysym *);
		GLvoid HandleKeyUp(SDL_keysym *);
		GLvoid HandleMenuKeyDown(GLushort);
		GLvoid HandleMapKeyDown(GLushort);
		GLvoid UpdateKeyState(GLushort, GLboolean);
		GLvoid UpdateTimer();
		GLvoid DoShowHideLevel();
		GLvoid DoFadeScreen();
		GLvoid PauseGame();
		GLvoid ResumeGame();
		GLvoid ToMap();
		GLvoid ToMain();
		GLvoid ToConfig();
		GLvoid ToTitleScreen();
		GLvoid ToCredits();
		GLvoid SelectLevel(LevelPreview *);
		GLvoid ApplyConfig();
		GLvoid NextLevelMenu();
		GLvoid ChangeState(GLshort, GLshort);
		
	public:
		static Controller * getInstance();
		static GLvoid DestroyInstance();
			
		Controller();
		~Controller();
		GLvoid MainLoop();
		GLvoid QuitGame(GLint);
		GLvoid ResetKeyStates();
		GLuint GetTimePast();
		GLvoid StartLevel(char *);
		GLvoid StartNextLevel();
		GLvoid RestartLevel();
		GLvoid EndLevel(char *);
		GLvoid StartGame();
		GLvoid HandleMenuCommand(GLuint);
		Level * GetLevel();
};

inline Controller * Controller::getInstance()
{
	if (!instance) {
		instance = new Controller();
	}
	return instance;
}

inline GLvoid Controller::DestroyInstance()
{
	if (instance) {
		delete instance;
	}
}

inline GLuint Controller::GetTimePast()
{
	return time_past;
}

inline Level * Controller::GetLevel()
{
	return level;
}

#endif // _CONRTOLLER_H_
