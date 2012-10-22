
#pragma once
#ifndef _3DMATRIX_
#define _3DMATRIX_

#include "include.h"
#include <math.h>

#define RAD(x)					((x) * PI / 180.0f)
#define GRA(x)					((x) * 180.0f / PI)
#define ABS(x)					(((x) < 0)? -(x) : (x))
#define PI						3.1415926535f
#define ORT_X					0x00
#define ORT_Y					0x01
#define ORT_Z					0x02

GLfloat * E(GLfloat *);
GLfloat * e(GLfloat *);
GLboolean mmult(GLfloat *, GLint, GLint, GLfloat *, GLint, GLint, GLfloat *);
GLvoid mcopy(GLfloat *, GLfloat *, GLint, GLint);
GLvoid Move16fv(GLfloat *, GLfloat, GLfloat, GLfloat);
GLvoid Rotate16fv(GLfloat *, GLfloat, GLint);
GLvoid Rotate_ex16fv(GLfloat *, GLfloat, GLint);
GLvoid Scale16fv(GLfloat * m, GLfloat x, GLfloat y, GLfloat z);

#endif // _3DMATRIX_
