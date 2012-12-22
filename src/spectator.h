#pragma once

#ifndef _SPECTATOR_H_
#define _SPECTATOR_H_

#include "include.h"
#include "config.h"
#include "3dml.h"
#include "hero.h"

#define MIN_AANGLE 10
#define MAX_AANGLE 70
#define MIN_ZOOM 15
#define MAX_ZOOM 45

class Spectator 
{
	protected:
		GLfloat x, y,
				rx, ry,
				rot, rrot,
				speed, rotspeed,
				S,
				angatt;
		Hero *	hero;

	public:	
		Spectator(Hero *);
		~Spectator() {}
		GLfloat GetX();
		GLfloat GetY();
		GLfloat GetRotation();
		GLshort GetNormalRotation();
		GLfloat GetAngle();
		GLfloat GetDistance();
		GLvoid RotateLeft();
		GLvoid RotateRight();
		GLvoid LookAt(GLfloat x, GLfloat y);
		GLvoid SetupView(GLfloat, GLfloat, GLfloat);
		GLvoid MoveEyes();
		GLvoid RotateBitLeft();
		GLvoid RotateBitRight();
		GLvoid ZoomIn();
		GLvoid ZoomOut();
		GLvoid MoveUp();
		GLvoid MoveDown();
};

inline GLfloat Spectator::GetX()
{
	return x;
}

inline GLfloat Spectator::GetY()
{
	return y;
}

inline GLfloat Spectator::GetRotation()
{
	return rot;
}

inline GLshort Spectator::GetNormalRotation()
{
	GLushort r = (rot > 0) ? GLushort(ABS(rot)) % 360 : 360 - GLushort(ABS(rot)) % 360;
	return r == 360 ? 0 : r;
}

inline GLfloat Spectator::GetAngle()
{
	return angatt;
}

inline GLfloat Spectator::GetDistance()
{
	return S;
}

inline GLvoid Spectator::RotateLeft()
{
	rrot -= 90.0f;
	if (rot - rrot > 360.0f) {
		rrot += 360.0f;
	}
}

inline GLvoid Spectator::RotateRight()
{
	rrot += 90.0f;
	if (rrot - rot > 360.0f) {
		rrot -= 360.0f;
	}
}

inline GLvoid Spectator::LookAt(GLfloat x, GLfloat y)
{
	rx = -x;
	ry = -y;
}

#endif // _SPECTATOR_H_
