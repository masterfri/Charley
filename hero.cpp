#include "hero.h"
#include "rc.h"
#include "controller.h"
#include "level.h"

Hero::Hero(Level * l)
{
	level = l;
	model = RcRegistry::getInstance()->GetModel(MODEL_HERO);
	dx = STEP / float(model->GetAnimaLen((char*)"walk"));
}

GLvoid Hero::Setup(GLshort x, GLshort y, GLshort dir)
{
	posx = x;
	posy = y;
	direction = dir;
	mx = GLfloat(posx) * STEP;
	my = GLfloat(posy) * STEP;
	rot = Level::GetAngleByDirection(direction);
	model->ResetMatrix();
	model->MoveAt(mx, 1, my);
	model->Rotate(rot, ORT_Y);
	model->SelectAnimation((char*)"stand0");
	model->LoopAnimation();			
	model->SetupPerspective(45, 4.0f/3.0f, 0.1f, 100.0f);
	state = GS_HALT;
}

GLvoid Hero::RotateLeft()
{
	state = GS_ROTATE_L;
	rrot = rot;
	rot += 90.0f;
	if (direction == WEST) {
		direction = SOUTH;
	} else {
		direction--;
	}
}

GLvoid Hero::RotateRight()
{
	state = GS_ROTATE_R;
	rrot = rot;
	rot -= 90.0f;
	if (direction == SOUTH) {
		direction = WEST;
	} else {
		direction++;
	}
}

GLboolean Hero::TryToGo()
{
	ride_on = -1;
	if (level->HeroCanGo(posx, posy, direction)) {
		level->GetNextPosition(posx, posy, direction);
		rmx = mx;
		rmy = my;
		mx = GLfloat(posx) * STEP;
		my = GLfloat(posy) * STEP;
		state = GS_MOVE;
		model->SelectAnimation((char*)"walk");
		model->ResetAnimation();
		
#ifdef DEBUG_MODE
printf("Hero made step to %d %d \n", posx, posy);
#endif

		return GL_TRUE;
	} else if (level->HeroCanRideOnTile(posx, posy, direction)) {
		ride_on = level->HeroRideOnTile(posx, posy, direction);
		rmx = mx;
		rmy = my;
		mx = GLfloat(posx) * STEP;
		my = GLfloat(posy) * STEP;
		state = GS_TRANSPORTATE;
		
#ifdef DEBUG_MODE
printf("Hero rides on tile to %d %d \n", posx, posy);
#endif

		return GL_TRUE;
	} else if (level->TryUseTeleport(posx, posy, direction)) {
		state = GS_TELEPORTATE_1;
		model->SelectAnimation((char*)"hide");
		model->ResetAnimation();
		
#ifdef DEBUG_MODE
printf("Hero teleportates to %d %d \n", posx, posy);
#endif

		return GL_TRUE;
	}
	return GL_FALSE;
}

inline GLvoid Hero::RotateModel(GLfloat speed)
{
	GLfloat delta = speed * GAME_SPEED * Controller::getInstance()->GetTimePast();
	rrot += delta;
	if (delta > 0) {
		if (rrot >= rot) {
			delta -= rrot - rot;
			state = GS_HALT;
		}
	} else {
		if (rrot <= rot) {
			delta += rot - rrot;
			state = GS_HALT;
		}
	}
	model->Rotate(delta, ORT_Y);
}

inline GLvoid Hero::MoveModel()
{
	GLfloat delta = dx * GAME_SPEED * Controller::getInstance()->GetTimePast();
	if (mx != rmx) {
		if (mx > rmx) {
			rmx += delta;
			if (rmx >= mx) {
				delta -= rmx - mx;
				rmx = mx;
				state = GS_HALT;
			}
			model->MoveRel(delta, 0, 0);
		} else {
			rmx -= delta;
			if (rmx <= mx) {
				delta -= mx - rmx;
				rmx = mx;
				state = GS_HALT;
			}
			model->MoveRel(-delta, 0, 0);
		}
	} else {
		if (my > rmy) {
			rmy += delta;
			if (rmy >= my) {
				delta -= rmy - my;
				rmy = my;
				state = GS_HALT;
			}
			model->MoveRel(0, 0, delta);
		} else {
			rmy -= delta;
			if (rmy <= my) {
				delta -= my - rmy;
				rmy = my;
				state = GS_HALT;
			}
			model->MoveRel(0, 0, -delta);
		}
	}
	if (ride_on != -1) {
		level->ShiftTile(ride_on, rmx, rmy);
	}
	if (GS_HALT == state) {
		if (ride_on != -1) {
			ride_on = -1;
			if (Config::getInstance()->GetSound()) {
				RcRegistry::getInstance()->PlaySound(SOUND_STONE_COLLISION);
			}
		} else {
			model->SelectAnimation((char*)"stand0");
			model->ResetAnimation();
		}
	}
}

GLvoid Hero::UpdateMoving()
{
	switch (state) {
		case GS_ROTATE_L:
			RotateModel(5.0f);
			break;
		case GS_ROTATE_R:
			RotateModel(-5.0f);
			break;
		case GS_MOVE:
		case GS_TRANSPORTATE:
			MoveModel();
			break;
	}
}

GLvoid Hero::OnEndAnimation()
{
	if (state == GS_TRANSPORTATE || state == GS_HALT) {
		if (rand() % 5) {
			model->SelectAnimation((char*)"stand0");
		} else {
			model->SelectAnimation((char*)"stand1");
		}
	} else if(state == GS_TELEPORTATE_1) {
		model->SelectAnimation((char*)"show");
		state = GS_TELEPORTATE_2;
		mx = GLfloat(posx) * STEP;
		my = GLfloat(posy) * STEP;
		model->MoveAt(mx, 1, my);
	} else 	if(state == GS_TELEPORTATE_2) {
		model->SelectAnimation((char*)"stand0");
		state = GS_HALT;
	}
}

GLvoid Hero::SetCamera(Spectator * s)
{
	model->SetCamera(s->GetX(), 0, s->GetY(), s->GetRotation(), 0, s->GetAngle(), s->GetDistance());
}
