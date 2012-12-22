#pragma once

#ifndef _LEVEL_H_
#define _LEVEL_H_

class Level;
class Spectator;

#include "config.h"
#include "include.h"
#include "strproc.h"
#include "hero.h"

#define MAPSIZE 12
#define MAX_EXIT 4
#define STEP 2
#define LVLNAME_MAX_LEN 32
#define LVLTITLE_MAX_LEN 256

#define FURNITURE_THRONE	0
#define FURNITURE_FLOWER	1
#define FURNITURE_CLOCK		2
#define FURNITURE_BIG_TABLE	3
#define FURNITURE_TABLE		4
#define FURNITURE_BOOK_CASE	5
#define FURNITURE_BED		6

#define TT_NONE 0
#define TT_TRICK 1
#define TT_FOS 2
#define TT_TELEPORT 3
#define TT_ELEVATOR 4
#define TT_BUTTON 5
#define NO_MOVEDTILE 0xff

#define ES_STANDDOWN 0
#define ES_STANDUP 1
#define ES_MOVEUP 2
#define ES_MOVEDOWN 3

#define WEST 	0
#define NORTH 	1
#define OST 	2
#define SOUTH 	3

#define STATE_LIGHT_ON 0
#define STATE_LIGHT_OFF 1
#define STATE_FADE_IN 2
#define STATE_FADE_OUT 3

struct Exit 
{
	GLboolean 	exit_open, 
				got_key;
	GLshort 	key_x, 
				key_y;
	GLshort 	x, 
				y;
	MODEL_3D 	*door;
	char 		way_to[LVLNAME_MAX_LEN];
};

struct MapCell 
{
	GLushort 	type,
				moved,
				val,
				flag;
};

struct ProgressRecord
{
	char level[LVLNAME_MAX_LEN];
	GLshort locked;
	GLshort keys[MAX_EXIT];
};

struct LevelPreview
{
	char 		szName[LVLNAME_MAX_LEN];
	char 		szTitle[LVLTITLE_MAX_LEN];
	SDL_Surface * picture;
	GLshort		exit_count;
	char 		* way_to[MAX_EXIT + 1];
	GLboolean 	keys[MAX_EXIT];
	GLboolean 	locked;
};

class Level
{
	protected:
		DataScript 	levels_data, 
					map_data;
		Exit 		exits[MAX_EXIT];
		MapCell 	cells[MAPSIZE * MAPSIZE];
		MODEL_3D 	key[MAX_EXIT],
					*tiles[MAPSIZE * MAPSIZE],
					*walls[MAPSIZE * 4],
					topwalls[4],
					*furniture,
					*base;
		GLshort		furniture_count,
					levels_count,
					hero_start_x, 
					hero_start_y,
					hero_start_d,
					light_state;
		GLfloat		darkness;
		LevelPreview * levels;
		Hero * 		hero;
		Spectator *	spectator;
		
		GLvoid InitKeys();
		GLvoid InitWalls();
		GLvoid CreateEmptyFloorTile(GLushort, GLushort);
		GLvoid CreateNormalFloorTile(GLushort, GLushort);
		GLvoid CreateCrackedFloorTile(GLushort, GLushort);
		GLvoid CreateTeleport(GLushort, GLushort, char);
		GLvoid CreateElevator(GLushort, GLushort, char, char);
		GLvoid CreateButton(GLushort, GLushort, char);
		GLvoid CreateTransporter(GLushort, GLushort);
		GLvoid DropKey(GLushort, GLushort, GLshort);
		GLshort DecodeDirection(char);
		GLvoid InitiateHeroPosition(GLshort, GLshort, GLshort);
		GLvoid CreateExit(char, GLushort, GLushort, GLushort);
		GLvoid CreateWallBlock(GLushort, GLushort, GLushort);
		GLvoid CreateDoor(GLushort, GLushort, GLushort);
		GLvoid CreateWindow(GLushort, GLushort, GLushort);
		GLvoid DisposeWall(MODEL_3D *, GLushort, GLushort);
		GLvoid CreateThrone(GLushort, GLint, GLint, GLint);
		GLvoid CreateFlower(GLushort, GLint, GLint, GLint);
		GLvoid CreateClock(GLushort, GLint, GLint, GLint);
		GLvoid CreateBigTable(GLushort, GLint, GLint, GLint);
		GLvoid CreateTable(GLushort, GLint, GLint, GLint);
		GLvoid CreateBookCase(GLushort, GLint, GLint, GLint);
		GLvoid CreateBed(GLushort, GLint, GLint, GLint);
		GLvoid GetVisibleWalls(GLboolean *);
		GLvoid DrawWalls(GLushort, GLushort, GLushort);
		GLvoid DrawFloor();
		GLboolean CanRideTo(GLshort, GLshort);
		GLvoid UpdateLighting();
		GLvoid DrawDarkMask();
		GLvoid CreateLevelsPreviews();
		GLvoid DestroyPreviews();
		GLboolean ThereIsATile(GLshort, GLshort);
		
	public:
		Level();
		~Level();
		GLboolean CreateLevel(char * szName);
		GLvoid DestroyLevel();
		GLboolean StartLevel();
		GLvoid Render();
		static GLfloat GetAngleByDirection(GLshort);
		GLboolean HeroCanGo(GLshort, GLshort, GLshort);
		GLvoid GetNextPosition(GLshort &, GLshort &, GLshort);
		GLshort ThereIsExit(GLshort, GLshort);
		GLvoid HeroIsMovingTo(GLshort, GLshort, GLshort, GLshort);
		GLshort ThereIsKey(GLshort, GLshort);
		GLvoid GrabKey(GLshort);
		GLvoid PushButton(GLshort);
		GLboolean HeroCanRideOnTile(GLshort, GLshort, GLshort);
		GLushort GetTileIndex(GLshort, GLshort);
		GLvoid GetCoordByIndex(GLushort, GLshort &, GLshort &);
		GLushort HeroRideOnTile(GLshort &, GLshort &, GLshort);
		GLvoid ShiftTile(GLushort, GLfloat, GLfloat);
		GLboolean TryUseTeleport(GLshort &, GLshort &, GLshort);
		GLvoid TurnLightOn();
		GLvoid TurnLightOff();
		GLshort GetLightState();
		GLushort GetCountLevels();
		LevelPreview * GetLevelsPreviews();
		LevelPreview * GetLevelInfo(char *);
		GLshort FindLevel(char * szName);
		GLvoid SetAsPassed(char *, char *);
		GLboolean LoadProgress();
		GLboolean SaveProgress();
		Hero * GetHero();
		Spectator * GetSpectator();
};

inline GLushort Level::GetTileIndex(GLshort x, GLshort y)
{
	return x * MAPSIZE + y;
}

inline GLvoid Level::GetCoordByIndex(GLushort index, GLshort &x, GLshort &y)
{
	x = (GLshort) index / MAPSIZE;
	y = index % MAPSIZE;
}

inline GLvoid Level::TurnLightOn()
{
	darkness = 1.0f;
	light_state = STATE_FADE_IN;
}

inline GLvoid Level::TurnLightOff()
{
	darkness = 0.0f;
	light_state = STATE_FADE_OUT;
}

inline GLshort Level::GetLightState()
{
	return light_state;
}

inline GLushort Level::GetCountLevels()
{
	return levels_count;
}

inline LevelPreview * Level::GetLevelsPreviews()
{
	return levels;
}

inline Hero * Level::GetHero()
{
	return hero;
}

inline Spectator * Level::GetSpectator()
{
	return spectator;
}

#endif // _LEVEL_H_
