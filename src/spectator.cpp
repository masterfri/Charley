#include "spectator.h"
#include "controller.h"

Spectator::Spectator(Hero * h)
{
	x = 0; 
	y = 0;
	rx = 0;
	ry = 0;
	rot = 180;
	rrot = 180;
	speed = 0.05f;
	rotspeed = 1.2f;
	S = 30;
	angatt = 40;
	hero = h;
}

GLvoid Spectator::SetupView(GLfloat sx, GLfloat sy, GLfloat sr)
{
	switch (Config::getInstance()->GetCamera()) {
		case CP_LEFT_ALWAYS:
			rot = rrot = 270.0f + sr;
			break;
		case CP_RIGHT_ALWAYS:
			rot = rrot = 90.0f + sr;
			break;
		case CP_BACK_ALWAYS:
		default:
			rot = rrot = 180.0f + sr;
			break;
	}
	x = rx = sx;
	y = ry = sy;
}

GLvoid Spectator::MoveEyes()
{
	Controller * controller = Controller::getInstance();
	if (ABS(x - rx) > speed) {
		if (x < rx) x += speed * GAME_SPEED * controller->GetTimePast();
		else x -= speed * GAME_SPEED * controller->GetTimePast();
	}
	if (ABS(y - ry) > speed) {
		if (y < ry) y += speed * GAME_SPEED * controller->GetTimePast();
		else y -= speed * GAME_SPEED * controller->GetTimePast();
	}
	if (ABS((rot - rrot)) > rotspeed) {
		if (rot < rrot) rot += rotspeed * GAME_SPEED * controller->GetTimePast();
		else rot -= rotspeed * GAME_SPEED * controller->GetTimePast();
	}
	hero->SetCamera(this);
}

GLvoid Spectator::RotateBitLeft()
{
	rrot -= rotspeed * GAME_SPEED * Controller::getInstance()->GetTimePast();
	rot = rrot;
}

GLvoid Spectator::RotateBitRight()
{
	rrot += rotspeed * GAME_SPEED * Controller::getInstance()->GetTimePast();
	rot = rrot;
}

GLvoid Spectator::ZoomIn()
{
	if (S > MIN_ZOOM) {
		S -= speed * GAME_SPEED * Controller::getInstance()->GetTimePast();
	}
}

GLvoid Spectator::ZoomOut()
{
	if (S < MAX_ZOOM) {
		S += speed * GAME_SPEED * Controller::getInstance()->GetTimePast();
	}
}

GLvoid Spectator::MoveUp()
{
	if (angatt > MIN_AANGLE) {
		angatt -= rotspeed * GAME_SPEED * Controller::getInstance()->GetTimePast();
	}
}

GLvoid Spectator::MoveDown()
{
	if (angatt < MAX_AANGLE) {
		angatt += rotspeed * GAME_SPEED * Controller::getInstance()->GetTimePast();
	}
}
