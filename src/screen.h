#pragma once
#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "config.h"
#include "include.h"
#include "level.h"

#define ALIGN_LEFT 1
#define ALIGN_CENTER 2
#define ALIGN_RIGHT 3

#define MAX_MENU_ITEMS	15

#define MENU_MAIN 		0
#define MENU_PAUSE 		1
#define MENU_CONFIG 	2
#define MENU_NEXT_ROOM 	3

#define COUNT_MENUS 	4
#define ITEMS_SHOW_LIMIT 5
#define MAP_ITEMS_SHOW_LIMIT 9

#define MI_PAUSE_TO_ROOM 	0
#define MI_PAUSE_RESTART	1
#define MI_PAUSE_TO_MAP 	2
#define MI_PAUSE_TO_MAIN	3
#define MI_MAIN_PLAY		0
#define MI_MAIN_OPTS		1
#define MI_MAIN_EXIT		2
#define MI_OPTS_FULLSCREEN	0
#define MI_OPTS_CAMERA		1
#define MI_OPTS_DRAW_WALLS	2
#define MI_OPTS_SOUND		3
#define MI_OPTS_MUSIC		4
#define MI_OPTS_APPLY		5
#define MI_OPTS_BACK		6
#define MI_NEXT_ROOM		0
#define MI_NEXT_BACK		1
#define MI_NEXT_MAP			2
#define MI_NEXT_MAIN		3

#define OPT_VALUE_ON		0
#define OPT_VALUE_OFF		1
#define OPT_VALUE_CAML		CP_LEFT_ALWAYS
#define OPT_VALUE_CAMR		CP_RIGHT_ALWAYS
#define OPT_VALUE_CAMB		CP_BACK_ALWAYS
#define OPT_VALUE_CAMF		CP_FREE

#define CMD_BACK_TO_ROOM 	(MENU_PAUSE << 8 | MI_PAUSE_TO_ROOM)
#define CMD_RESTART 		(MENU_PAUSE << 8 | MI_PAUSE_RESTART)
#define CMD_TO_MAP 			(MENU_PAUSE << 8 | MI_PAUSE_TO_MAP)
#define CMD_TO_MAIN			(MENU_PAUSE << 8 | MI_PAUSE_TO_MAIN)
#define CMD_PLAY			(MENU_MAIN << 8 | MI_MAIN_PLAY)
#define CMD_CONFIG			(MENU_MAIN << 8 | MI_MAIN_OPTS)
#define CMD_EXIT			(MENU_MAIN << 8 | MI_MAIN_EXIT)
#define CMD_APPLY_OPTS		(MENU_CONFIG << 8 | MI_OPTS_APPLY)
#define CMD_CANCEL_OPTS		(MENU_CONFIG << 8 | MI_OPTS_BACK)
#define CMD_NEXT_ROOM		(MENU_NEXT_ROOM << 8 | MI_NEXT_ROOM)
#define CMD_TURN_BACK		(MENU_NEXT_ROOM << 8 | MI_NEXT_BACK)
#define CMD_NEXT_MAP		(MENU_NEXT_ROOM << 8 | MI_NEXT_MAP)
#define CMD_NEXT_MAIN		(MENU_NEXT_ROOM << 8 | MI_NEXT_MAIN)

#define MAP_COLS 3
#define MAP_ROWS 3
#define MAP_ROW_H 120
#define MAP_HEIGHT MAP_ROW_H * 3

#define SCREEN_FADE_IN	0
#define SCREEN_FADE_OUT	1
#define SCREEN_NORMAL	2
#define SCREEN_FADED	3

class MenuItem
{
	protected:
		char * label;
		GLshort count_options,
				current_option;
		char ** options;
		
	public:
		MenuItem(char *, GLshort, char **);
		~MenuItem() {}
		char * GetLabel();
		char * GetOptionLabel();
		GLboolean isOption(); 
		GLvoid ToggleOption();
		GLshort GetOptionValue();
		GLvoid SetOptionValue(GLshort);
};

class MenuData
{
	protected:
		GLshort 	id,
					items_count;
		char		* title,
					* subtitle,
					* ok,
					* cancel;
		MenuItem 	** items;
		SDL_Surface *bgimage;
		
	public:
		MenuData(GLshort, char *, char *, char *, GLshort, SDL_Surface *);
		~MenuData();
		GLvoid SetItem(GLshort index, MenuItem* item);
		MenuItem * GetItem(GLshort index);
		GLshort CountItems();
		GLshort GetId();
		char * GetTitle();
		char * GetSubtitle();
		GLvoid SetSubtitle(char *);
		char * GetOk();
		char * GetCancel();
		SDL_Surface * GetBackground();
		GLint GetHeight();
};

class Screen
{
	protected:
		static Screen 	*instance;
		
		GLboolean 		repaint;
		MenuData 		* menu,
						* menu_templates[COUNT_MENUS];
		LevelPreview 	* map_items;
		GLshort			selected_item,
						items_offset,
						map_items_count,
						state;
		SDL_Surface 	* screen_image;
		GLfloat			transparency;
						
		GLvoid DrawCursor(SDL_Rect, SDL_Color);
		GLvoid RenderText(char *, TTF_Font *, SDL_Color, GLint, GLint, GLshort, GLboolean);
		GLvoid RenderBgImage(SDL_Surface *, SDL_Surface *);
		GLvoid RenderButtons(SDL_Surface *, GLint, GLint &);
		GLvoid RenderItems(SDL_Surface *, GLint, GLint &);
		GLvoid Reset();
		GLvoid RenderGUI(GLint, GLint, SDL_Rect);
		GLvoid Fade(SDL_Surface *);
		
	public:
		static Screen * getInstance();
		static GLvoid DestroyInstance();
		
		Screen() {};
		~Screen();
		
		GLvoid InitMenus();
		GLvoid RenderMenu();
		GLvoid RenderMainMenu();
		GLvoid RenderMap();
		GLvoid Render();
		GLvoid SetRepaint();
		GLvoid PrepareMenu(GLshort);
		GLvoid PrepareMap(LevelPreview *, GLushort);
		GLvoid SetCursorPosition(GLshort);
		GLvoid SetMapCursorPosition(GLshort);
		GLvoid CursorUp();
		GLvoid CursorDown();
		GLvoid SelectItem();
		GLvoid PrevMapRow();
		GLvoid NextMapRow();
		GLvoid PrevMapItem();
		GLvoid NextMapItem();
		LevelPreview * GetMapItem();
		GLshort GetOptionValue(GLshort);
		GLvoid SetOptionValue(GLshort, GLshort);
		GLvoid SetSubtitle(char *);
		GLvoid FadeIn();
		GLvoid FadeOut();
		GLvoid SetImage(SDL_Surface *);
		GLvoid LoadImage(char *);
		GLvoid DestroyImage();
		SDL_Surface * GetImage();
		GLshort GetState();
};

inline Screen * Screen::getInstance()
{
	if (!instance) {
		instance = new Screen();
	}
	return instance;
}

inline GLvoid Screen::DestroyInstance()
{
	if (instance) {
		delete instance;
	}
}

inline GLvoid Screen::SetRepaint()
{
	repaint = GL_TRUE;
}

inline GLvoid MenuData::SetItem(GLshort index, MenuItem* item)
{
	if (index < items_count) {
		items[index] = item;
	}
}

inline MenuItem * MenuData::GetItem(GLshort index)
{
	if (index < items_count) {
		return items[index];
	}
	return NULL;
}

inline GLshort MenuData::CountItems()
{
	return items_count;
}

inline GLshort MenuData::GetId()
{
	return id;
}

inline char * MenuData::GetTitle()
{
	return title;
}

inline char * MenuData::GetOk()
{
	return ok;
}

inline char * MenuData::GetCancel()
{
	return cancel;
}

inline SDL_Surface * MenuData::GetBackground()
{
	return bgimage;
}

inline char * MenuItem::GetLabel()
{
	return label;
}

inline char * MenuItem::GetOptionLabel()
{
	return options[current_option];
}

inline GLboolean MenuItem::isOption()
{
	return options != NULL;
}

inline GLvoid MenuItem::ToggleOption()
{
	current_option++;
	if (current_option >= count_options) {
		current_option = 0;
	}
}

inline GLshort MenuItem::GetOptionValue()
{
	return current_option;
}

inline GLvoid MenuItem::SetOptionValue(GLshort value)
{
	if (value < count_options) {
		current_option = value;
	}
}

inline GLshort Screen::GetOptionValue(GLshort id)
{
	MenuItem * item = menu->GetItem(id);
	return (item && item->isOption()) ? item->GetOptionValue() : -1;
}

inline GLvoid Screen::SetOptionValue(GLshort id, GLshort value)
{
	MenuItem * item = menu->GetItem(id);
	if (item && item->isOption()) {
		item->SetOptionValue(value);
	}
}

inline char * MenuData::GetSubtitle()
{
	return subtitle;
}

inline GLvoid MenuData::SetSubtitle(char * value)
{
	subtitle = value;
}

inline GLvoid Screen::SetSubtitle(char * value)
{
	menu->SetSubtitle(value);
}

inline GLshort Screen::GetState()
{
	return state;
}

inline GLvoid Screen::FadeIn()
{
	SetRepaint();
	state = SCREEN_FADE_IN;
	transparency = 0.0f;
}

inline GLvoid Screen::FadeOut()
{
	SetRepaint();
	state = SCREEN_FADE_OUT;
	transparency = 1.0f;
}

inline GLvoid Screen::SetImage(SDL_Surface * img)
{
	SetRepaint();
	state = SCREEN_NORMAL;
	screen_image = img;
}

inline SDL_Surface * Screen::GetImage()
{
	return screen_image;
}

inline GLvoid Screen::LoadImage(char * szName)
{
	SetRepaint();
	screen_image = IMG_Load(szName);
}

inline GLvoid Screen::DestroyImage()
{
	if (screen_image) {
		SDL_FreeSurface(screen_image);
	}
}

#endif // _SCREEN_H_
