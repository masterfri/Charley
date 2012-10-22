#pragma once
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "include.h"
#include "strproc.h"

//#define DEBUG_MODE

// key binding
#define K_ESCAPE		SDLK_ESCAPE
#define K_LEFT			SDLK_LEFT
#define K_RIGHT			SDLK_RIGHT
#define K_WALK			SDLK_UP
#define K_CAMERA_LEFT	SDLK_PAGEDOWN
#define K_CAMERA_RIGHT	SDLK_PAGEUP
#define K_CAMERA_NEAR	SDLK_HOME
#define K_CAMERA_AWAY	SDLK_END
#define K_CAMERA_UP		SDLK_INSERT
#define K_CAMERA_DOWN	SDLK_DELETE
#define K_UP			SDLK_UP
#define K_DOWN			SDLK_DOWN
#define K_SELECT		SDLK_RETURN

// OpenGL
#define LIGHT_DIFFUSE 0.8f
#define LIGHT_MODEL_AMBIENT 0.4f
#define LIGHT_POSITION 0,20,0,0

// menu settings
#define MENU_TITLE_SPACE 60
#define MENU_SUBTITLE_SPACE 50
#define MENU_ITEM_SPACE 30
#define MENU_BUTTON_SPACE 20
#define MENU_TITLE_COLOR 88,31,0
#define MENU_SUBTITLE_COLOR 20,40,106
#define MENU_ITEM_COLOR 56,56,56
#define MENU_ACTIVE_ITEM_COLOR 0,0,0
#define MENU_BUTTON_COLOR 25,95,22
#define MENU_ACTIVE_BUTTON_COLOR 8,63,5
#define MENU_OPT_LABEL_COLOR 20,40,106
#define MENU_CURSOR_PAD 5
#define MENU_CURSOR_SY 3
#define MAIN_MENU_SX 620
#define MAIN_MENU_SY 325
#define MAIN_MENU_ITEM_COLOR 0,0,0
#define MAIN_MENU_ACTIVE_ITEM_COLOR 235,255,50
#define MAIN_MENU_ITEM_SPACE 37

// map
#define MAP_CURSOR_PAD 3
#define MAP_SPACING 30
#define MAP_TITLE_SPACE 30
#define MAP_KEY_PADDING 2
#define MAP_KEYS_SX 4
#define MAP_KEYS_SY 4

// misc
#define GAME_SPEED	0.05f
#define LIGHT_SWITCH_SPEED 0.001f
#define DATA_DIR ".charleytheghost"
#define CONFIG_FILE "config.ds"
#define PROGRESS_FILE "progress.dat"
#define TITLE_TIMER 3000
#define CREDITS_TIMER 2000
#define SKIP_CREDITS
#define VERSION_COLOR 250,250,255
#define VERSION_STR "Version 1.0, by Grigory Ponomar"
#define VERSION_SX 15
#define VERSION_SY 35

// strings
#define GAME_TITLE "Charley The Ghost"
#define STR_PAUSE "Pause"
#define STR_RETURN_TO_ROOM "Return to the room"
#define STR_RESTART_ROOM "Restart the room"
#define STR_BACK_TO_MAP "Back to map"
#define STR_BACK_MAIN "To main menu"
#define STR_MAP "Select a Room"
#define STR_CONFIG "Options"
#define STR_FULLSCREEN "Fullscreen"
#define STR_ON "On"
#define STR_OFF "Off"
#define STR_CAMERA "Camera"
#define STR_ALWAYS_LEFT "On the left"
#define STR_ALWAYS_RIGHT "On the right"
#define STR_ALWAYS_BACK "Behind"
#define STR_FREE "Free"
#define STR_DRAW_WALLS "Draw walls"
#define STR_SOUND "Sound"
#define STR_MUSIC "Music"
#define STR_MAIN "Main"
#define STR_PLAY "Play"
#define STR_EXIT "Exit"
#define STR_APPLY "Apply"
#define STR_BACK "Back"
#define STR_THERE_IS_NEXT_ROOM "There is the next room"
#define STR_ENTER_THIS_ROOM "Enter this room"
#define STR_RETURN_BACK "Return back"

#define CP_BACK_ALWAYS 	0
#define CP_RIGHT_ALWAYS 1
#define CP_LEFT_ALWAYS 	2
#define CP_FREE 		3

class Config
{
	protected:
		static Config * instance;
		
		GLint	fullscreen,
				camera,
				draw_walls,
				sound,
				music;
		char	current_level[16];
		
	public:
		static Config * getInstance();
		static GLvoid DestroyInstance();
		
		Config();
		~Config() {}
		GLint GetFullscreen();
		GLint GetCamera();
		GLint GetDrawWalls();
		GLint GetSound();
		GLint GetMusic();
		char * GetCurrentLevel();
		GLvoid SetFullscreen(GLint);
		GLvoid SetCamera(GLint);
		GLvoid SetDrawWalls(GLint);
		GLvoid SetSound(GLint);
		GLvoid SetMusic(GLint);
		GLvoid SetCurrentLevel(char *);
		GLboolean Load();
		GLboolean Save();
		GLvoid GetDataDir(char *);
};

inline Config * Config::getInstance()
{
	if (!instance) {
		instance = new Config();
	}
	return instance;
}

inline GLvoid Config::DestroyInstance()
{
	if (instance) {
		delete instance;
	}
}

inline GLint Config::GetFullscreen()
{
	return fullscreen;
}

inline GLint Config::GetCamera()
{
	return camera;
}

inline GLint Config::GetDrawWalls()
{
	return draw_walls;
}
	
inline GLint Config::GetSound()
{
	return sound;
}

inline GLint Config::GetMusic()
{
	return music;
}

inline char * Config::GetCurrentLevel()
{
	return current_level;
}

inline GLvoid Config::SetFullscreen(GLint value)
{
	fullscreen = value;
}
	
inline GLvoid Config::SetCamera(GLint value)
{
	camera = value;
}

inline GLvoid Config::SetDrawWalls(GLint value)
{
	draw_walls = value;
}

inline GLvoid Config::SetSound(GLint value)
{
	sound = value;
}

inline GLvoid Config::SetMusic(GLint value)
{
	music = value;
}

inline GLvoid Config::SetCurrentLevel(char * name)
{
	StrCpy(current_level, name);
}

#endif // _CONFIG_H_
