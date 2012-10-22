#pragma once

#ifndef _HERO_H_
#define _HERO_H_

class Hero;

#include "include.h"
#include "config.h"
#include "3dml.h"
#include "level.h"
#include "spectator.h"

#define GS_HALT 0
#define GS_MOVE 1
#define GS_ROTATE_L 2
#define GS_ROTATE_R 3
#define GS_TRANSPORTATE 4
#define GS_TELEPORTATE_1 5
#define GS_TELEPORTATE_2 6

class Hero 
{
	protected:
		GLfloat		dx,
					rot,
					rrot,
					mx,
					my,
					rmx,
					rmy;
		GLshort		posx,
					posy,
					state,
					direction,
					ride_on;
		MODEL_3D *	model;
		Level *		level;
		
		GLvoid RotateModel(GLfloat);
		GLvoid MoveModel();
		
	public:
		Hero(Level *);
		~Hero() {}
		GLshort GetX();
		GLshort GetY();
		GLshort GetDirection();
		GLfloat GetModelX();
		GLfloat GetModelY();
		GLfloat GetRotation();
		GLshort GetState();
		GLvoid SetCamera(Spectator * s);
		GLboolean Draw();
		GLvoid RotateLeft();
		GLvoid RotateRight();
		GLvoid Setup(GLshort, GLshort, GLshort);
		GLboolean TryToGo();
		GLvoid UpdateMoving();
		GLvoid OnEndAnimation();
};

inline GLshort Hero::GetX()
{
	return posx;
}

inline GLshort Hero::GetY()
{
	return posy;
}

inline GLshort Hero::GetDirection()
{
	return direction;
}

inline GLfloat Hero::GetModelX()
{
	return mx;
}

inline GLfloat Hero::GetModelY()
{
	return my;
}

inline GLfloat Hero::GetRotation()
{
	return rot;
}

inline GLshort Hero::GetState()
{
	return state;
}

inline GLboolean Hero::Draw()
{
	if (model->Draw()) {
		OnEndAnimation();
		return GL_TRUE;
	}
	return GL_FALSE;
}

#endif // _HERO_H_
