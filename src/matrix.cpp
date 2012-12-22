#include "matrix.h"

GLfloat * E(GLfloat * target)
{
	if (!target) {
		target = new GLfloat [16];
	}
	for (GLint i = 0; i < 4; i++) {
		for (GLint j = 0; j < 4; j++) {
			if (i == j) {
				target[4 * i + j] = 1.0f;
			} else {
				target[4 * i + j] = 0.0f;
			}
		}
	}
	return target;
}

GLfloat * e(GLfloat * target)
{
	if (!target) {
		target = new GLfloat [9];
	}
	for (GLint  i = 0; i < 3; i++) {
		for (GLint  j = 0; j < 3; j++) {
			if (i == j) {
				target[3 * i + j] = 1.0f;
			} else {
				target[3 * i + j] = 0.0f;
			}
		}
	}
	return target;
}

GLboolean mmult(GLfloat * A, GLint A_W, GLint A_H, GLfloat * B, GLint B_W, GLint B_H, GLfloat * R)
{
	if (A_W != B_H) {
		return GL_FALSE;
	}
	GLint i, j, k;
	GLfloat sum;
	for (i = 0; i < A_H; i++) {
		for (j = 0; j < B_W; j++) {
			sum = 0;
			for (k = 0; k < B_H; k++) {
				sum = sum + A[i * A_W + k] * B[k * B_W + j];
			}
			R[i * B_W + j] = sum;
		}
	}
	return GL_TRUE;
}

GLvoid mcopy(GLfloat * DET, GLfloat * SOUR, GLint Width, GLint Heigth)
{
	GLint looplen = Width * Heigth;
	for (GLint i = 0; i < looplen; i++) {
		DET[i] = SOUR[i];
	}
}

GLvoid Move16fv(GLfloat * m, GLfloat x, GLfloat y, GLfloat z)
{
	m[12] += x;
	m[13] += y;
	m[14] += z;
}

GLvoid Rotate16fv(GLfloat * m, GLfloat a, GLint ort)
{
	a = RAD(a);
	GLuint i;
	GLfloat tmp;
	switch (ort) {
		case ORT_X:
			for (i = 1; i < 12; i += 4) {
				tmp = GLfloat ( m[i] * cos(a) - m[i + 1] * sin(a));
				m[i + 1] = GLfloat ( m[i] * sin(a) + m[i + 1] * cos(a));
				m[i] = tmp;
			}
			break;
		case ORT_Y:
			for (i = 0; i < 12; i += 4) {
				tmp = GLfloat (m[i] * cos(a) + m[i + 2] * sin(a));
				m[i + 2] = GLfloat ( -m[i] * sin(a) + m[i + 2] * cos(a));
				m[i] = tmp;
			}			
			break;
		case ORT_Z:
			for (i = 0; i < 12; i += 4) {
				tmp = GLfloat (m[i] * cos(a) - m[i + 1] * sin(a));
				m[i + 1] = GLfloat (m[i] * sin(a) + m[i + 1] * cos(a));
				m[i] = tmp;
			}			
			break;
	}
}

GLvoid Rotate_ex16fv(GLfloat * m, GLfloat a, GLint ort)
{
	a = RAD(a);
	GLuint i;
	GLfloat tmp;
	switch (ort) {
		case ORT_X:
			for (i = 1; i < 16; i += 4) {
				tmp = GLfloat ( m[i] * cos(a) - m[i + 1] * sin(a));
				m[i + 1] = GLfloat ( m[i] * sin(a) + m[i + 1] * cos(a));
				m[i] = tmp;
			}
			break;
		case ORT_Y:
			for (i = 0; i < 16; i += 4) {
				tmp = GLfloat (m[i] * cos(a) + m[i + 2] * sin(a));
				m[i + 2] = GLfloat ( -m[i] * sin(a) + m[i + 2] * cos(a));
				m[i] = tmp;
			}			
			break;
		case ORT_Z:
			for (i = 0; i < 16; i += 4) {
				tmp = GLfloat (m[i] * cos(a) - m[i + 1] * sin(a));
				m[i + 1] = GLfloat (m[i] * sin(a) + m[i + 1] * cos(a));
				m[i] = tmp;
			}			
			break;
	}
}

GLvoid Scale16fv(GLfloat * m, GLfloat x, GLfloat y, GLfloat z)
{
	m[0]  *= x; m[1]  *= y; m[2]  *= z;
	m[4]  *= x; m[5]  *= y; m[6]  *= z;
	m[8]  *= x; m[9]  *= y; m[10] *= z;	
}
