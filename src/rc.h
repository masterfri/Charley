#pragma once

#ifndef _RC_REGISTRY_H_
#define _RC_REGISTRY_H_

#include "include.h"
#include "rc-conf.h"
#include "3dml.h"

#define MODEL_HERO 			0
#define MODEL_BL_NORMAL 	1
#define MODEL_BL_CRACKED 	2
#define MODEL_BL_TRANSPORT 	3
#define MODEL_TELEPORT 		4
#define MODEL_EXIT_DOOR 	5
#define MODEL_WALL 			6
#define MODEL_WINDOW		7
#define MODEL_ELEVATOR		8
#define MODEL_BUTTON		9
#define MODEL_TABLE			10
#define MODEL_BIG_TABLE		11
#define MODEL_CHAIR			12
#define MODEL_FLOWER		13
#define MODEL_CLOCK			14
#define MODEL_BOOKCASE		15
#define MODEL_KEY			16
#define MODEL_BASE			17
#define MODEL_BED			18
#define MODEL_WALL2			19

#define CLONE_MESH			0x01
#define CLONE_MATERIAL		0x02
#define CLONE_ANIMATION		0x04
#define CLONE_ALL			CLONE_MESH | CLONE_MATERIAL | CLONE_ANIMATION

#define FONT_TITLE			0
#define FONT_MENUITEMS		1
#define FONT_VERSION		2

#define IMAGE_MAIN_SCREEN	0
#define IMAGE_PAUSE_SCREEN	1
#define IMAGE_TITLE_SCREEN	2
#define IMAGE_CREDITS		3
#define IMAGE_GUI_ITEMS		4

#define MUSIC_BACKGROUND		0

#define SOUND_BUTTON_DOWN		0
#define SOUND_BUTTON_UP			1
#define SOUND_DOOR				2
#define SOUND_ROLLING			3
#define SOUND_STONE_COLLISION	4
#define SOUND_STONE_BREAK		5
#define SOUND_TELEPORT			6

#define GI_RECT_ARROW_UP	0,0,24,24
#define GI_RECT_ARROW_DOWN	0,24,24,24
#define GI_RECT_CURSOR_L	6,55,18,17
#define GI_RECT_CURSOR_R	6,78,18,17
#define GI_RECT_LOCK		29,32,56,63
#define GI_RECT_KEY0		29,0,26,29
#define GI_RECT_KEY1		59,0,26,29
#define GI_RECT_KEY2		89,0,26,29
#define GI_RECT_KEY3		89,32,26,29
#define GI_RECT_KEY4		89,64,26,29
#define GI_RECT_MAP_CURSOR	122,0,192,122

#define LOADING_IMAGE		RC_ROOT"Images/loading.png"
#define ICON_IMAGE 			RC_ROOT"Images/icon.png"

class RcRegistry
{
	protected:
		static RcRegistry * instance;
		
		GLushort		count_models;
		MODEL_3D 		*models;
		GLushort		count_fonts;
		TTF_Font		**fonts;
		GLushort		count_images;
		SDL_Surface		**images;
		GLushort		count_music;
		Mix_Music		**music;
		GLushort		count_sounds;
		Mix_Chunk		**sounds;
	
	public:
		static RcRegistry * getInstance();
		static GLvoid DestroyInstance();
		
		RcRegistry();
		~RcRegistry();
		GLvoid DoLoading();
		MODEL_3D * GetModel(GLushort);
		TTF_Font * GetFont(GLushort);
		SDL_Surface * GetImage(GLushort);
		GLvoid PlayMusic(GLushort);
		GLvoid PlaySound(GLushort);
		GLvoid HaltMusic();
		GLvoid CloneModel(MODEL_3D *, GLushort, GLushort);
		GLushort CountModels();
		GLushort CountFonts();
		GLushort CountImages();
		GLushort CountMusic();
		GLushort CountSounds();
};

inline RcRegistry * RcRegistry::getInstance()
{
	if (!instance) {
		instance = new RcRegistry();
	}
	return instance;
}

inline GLvoid RcRegistry::DestroyInstance()
{
	if (instance) {
		delete instance;
	}
}

inline GLushort RcRegistry::CountModels() 
{
	return count_models;
}

inline GLushort RcRegistry::CountImages() 
{
	return count_models;
}

inline GLushort RcRegistry::CountFonts() 
{
	return count_images;
}

inline GLushort RcRegistry::CountMusic()
{
	return count_music;
}

#endif // _RC_REGISTRY_H_
