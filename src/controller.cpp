#include "controller.h"
#include "display.h"
#include "rc.h"

Controller * Controller::instance = NULL;

Controller::Controller() 
{
	level = NULL;
	screen = NULL;
	game_state = ST_PLAY;
	time_past = 0;
	timer_value = SDL_GetTicks();
	ResetKeyStates();
}

GLvoid Controller::HandleKeyDown(SDL_keysym * key)
{
	switch (game_state) {
		case ST_PLAY:
			if (key->sym == K_ESCAPE) {
				PauseGame();
			} else {
				UpdateKeyState(key->sym, GL_TRUE);
			}
			break;
			
		case ST_PAUSE:
			if (key->sym == K_ESCAPE) {
				ResumeGame();
			} else {
				HandleMenuKeyDown(key->sym);
			}
			break;
			
		case ST_MAIN:
			HandleMenuKeyDown(key->sym);
			break;
		
		case ST_NEXT_LEVEL_MENU:
		case ST_CONFIG:
			if (key->sym == K_ESCAPE) {
				ToMain();
			} else {
				HandleMenuKeyDown(key->sym);
			}
			break;
		
		case ST_MAP:
			if (key->sym == K_ESCAPE) {
				ToMain();
			} else {
				HandleMapKeyDown(key->sym);
			}
			break;
			
		case ST_TITLE_SCREEN:
		case ST_CREDITS_SCREEN:
			ChangeState(next_game_state, 0);
			break;
	}
}

GLvoid Controller::HandleKeyUp(SDL_keysym * key)
{
	switch (game_state) {
		case ST_PLAY:
			UpdateKeyState(key->sym, GL_FALSE);
			break;
	}
}

inline GLvoid Controller::UpdateKeyState(GLushort code, GLboolean state)
{
	switch(code) {
		case K_LEFT:
			key_pressed[KEY_LEFT] = state;
			break;
			
		case K_RIGHT:
			key_pressed[KEY_RIGHT] = state;
			break;
			
		case K_WALK:
			key_pressed[KEY_WALK] = state;
			break;
			
		case K_CAMERA_LEFT:
			key_pressed[KEY_CAMERA_LEFT] = state;
			break;
			
		case K_CAMERA_RIGHT:
			key_pressed[KEY_CAMERA_RIGHT] = state;
			break;
			
		case K_CAMERA_NEAR:
			key_pressed[KEY_CAMERA_NEAR] = state;
			break;
			
		case K_CAMERA_AWAY:
			key_pressed[KEY_CAMERA_AWAY] = state;
			break;
			
		case K_CAMERA_UP:
			key_pressed[KEY_CAMERA_UP] = state;
			break;
			
		case K_CAMERA_DOWN:
			key_pressed[KEY_CAMERA_DOWN] = state;
			break;
	}
}

inline GLvoid Controller::HandleMenuKeyDown(GLushort code)
{
	switch(code) {
		case K_UP:
			screen->CursorUp();
			break;
		case K_DOWN:
			screen->CursorDown();
			break;
		case K_SELECT:
			screen->SelectItem();
			break;
	}
}

inline GLvoid Controller::HandleMapKeyDown(GLushort code)
{
	switch(code) {
		case K_UP:
			screen->PrevMapRow();
			break;
		case K_DOWN:
			screen->NextMapRow();
			break;
		case K_LEFT:
			screen->PrevMapItem();
			break;
		case K_RIGHT:
			screen->NextMapItem();
			break;
		case K_SELECT:
			SelectLevel(screen->GetMapItem());
			break;
	}
}

GLvoid Controller::HandleMenuCommand(GLuint command)
{
	switch (command) {
		case CMD_BACK_TO_ROOM:
			ResumeGame();
			break;
			
		case CMD_TURN_BACK:
		case CMD_RESTART:
			RestartLevel();
			break;
		
		case CMD_NEXT_MAP:
		case CMD_TO_MAP:
			ToMap();
			break;
			
		case CMD_NEXT_MAIN:
		case CMD_TO_MAIN:
			ToMain();
			break;
			
		case CMD_PLAY:
			ToMap();
			break;
			
		case CMD_CONFIG:
			ToConfig();
			break;
			
		case CMD_EXIT:
#ifdef SKIP_CREDITS
			QuitGame(0);
#else
			ToCredits();
#endif
			break;
		
		case CMD_APPLY_OPTS:
			ApplyConfig();
			ToMain();
			break;
			
		case CMD_CANCEL_OPTS:
			ToMain();
			break;
			
		case CMD_NEXT_ROOM:
			StartNextLevel();
			break;
	}
}

GLvoid Controller::QuitGame(GLint code)
{
	if (0 == code) {
		// normal exit
		Config::getInstance()->Save();
		level->SaveProgress();
	}
	Display::DestroyInstance();
	Screen::DestroyInstance();
	RcRegistry::DestroyInstance();
	Config::DestroyInstance();
	DestroyInstance();
	SDL_Quit();
    exit(code);
}

GLvoid Controller::ResetKeyStates()
{
	for (GLushort i = 0; i < KEY_STATES; i++) {
		key_pressed[i] = GL_FALSE;
	}
}

inline GLvoid Controller::DrawScreen()
{
	switch (game_state) {
		case ST_PLAY:
		case ST_HIDE_LEVEL:
		case ST_SHOW_LEVEL:
			level->Render();
			break;
			
		case ST_PAUSE:
		case ST_CONFIG:
		case ST_NEXT_LEVEL_MENU:
			screen->RenderMenu();
			break;
			
		case ST_MAIN:
			screen->RenderMainMenu();
			break;
			
		case ST_MAP:
			screen->RenderMap();
			break;
			
		case ST_FADE_IN_SCREEN:
		case ST_FADE_OUT_SCREEN:
			screen->Render();
			break;
	}
}

inline GLvoid Controller::UpdateTimer()
{
	GLuint now = SDL_GetTicks();
	time_past = now - timer_value;
	timer_value = now;
	// Update models animation timer
	MODEL_3D::SetAnimaTimerDelta(GLfloat(time_past) * GAME_SPEED);
}

inline GLvoid Controller::HandleEvents()
{
	SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
			case SDL_KEYDOWN:
				HandleKeyDown(&event.key.keysym);
				break;
			case SDL_KEYUP:
				HandleKeyUp(&event.key.keysym);
				break;
			case SDL_QUIT:
				QuitGame(0);
				break;
        }
    }
}

inline GLvoid Controller::DoActions()
{
	switch (game_state) {
		case ST_SHOW_LEVEL:
		case ST_HIDE_LEVEL:
			DoShowHideLevel();
		case ST_PLAY:
			DoActionsPlaystate();
			break;

		case ST_FADE_IN_SCREEN:
		case ST_FADE_OUT_SCREEN:
			DoFadeScreen();
			break;
			
		case ST_TITLE_SCREEN:
		case ST_CREDITS_SCREEN:
			timer -= GetTimePast();
			if (timer <= 0) {
				ChangeState(next_game_state, 0);
			}
			break;
	}
}

inline GLvoid Controller::DoShowHideLevel()
{
	switch (level->GetLightState()) {
		case STATE_LIGHT_ON:
		case STATE_LIGHT_OFF:
			ChangeState(next_game_state, 0);
			break;
	}
}

inline GLvoid Controller::DoFadeScreen()
{
	switch (screen->GetState()) {
		case SCREEN_NORMAL:
		case SCREEN_FADED:
			ChangeState(next_game_state, 0);
			break;
	}
}

inline GLvoid Controller::DoActionsPlaystate()
{
	Hero * hero = level->GetHero();
	Spectator * spectator = level->GetSpectator();
	
	if (hero->GetState() == GS_HALT) {
		if (key_pressed[KEY_WALK]) {
			GLshort old_x = hero->GetX(), old_y = hero->GetY();
			if (hero->TryToGo()) {
				spectator->LookAt(hero->GetX() * STEP, hero->GetY() * STEP);
				level->HeroIsMovingTo(hero->GetX(), hero->GetY(), old_x, old_y);
			}
		} else if (key_pressed[KEY_LEFT]) {
			hero->RotateLeft();
			if (Config::getInstance()->GetCamera() != CP_FREE) {
				spectator->RotateLeft();
			}
		} else if (key_pressed[KEY_RIGHT]) {
			hero->RotateRight();
			if (Config::getInstance()->GetCamera() != CP_FREE) {
				spectator->RotateRight();
			}
		}
	} else {
		hero->UpdateMoving();
	}
	if (key_pressed[KEY_CAMERA_LEFT]) {
		if (Config::getInstance()->GetCamera() == CP_FREE) {
			spectator->RotateBitLeft();
		}
	} else if (key_pressed[KEY_CAMERA_RIGHT]) {
		if (Config::getInstance()->GetCamera() == CP_FREE) {
			spectator->RotateBitRight();
		}
	}
	if (key_pressed[KEY_CAMERA_NEAR]) {
		spectator->ZoomIn();
	} else if (key_pressed[KEY_CAMERA_AWAY]) {
		spectator->ZoomOut();
	}
	if (key_pressed[KEY_CAMERA_UP]) {
		spectator->MoveUp();
	} else if (key_pressed[KEY_CAMERA_DOWN]) {
		spectator->MoveDown();
	}
	
	spectator->MoveEyes();
}

GLvoid Controller::MainLoop()
{
	while(1) {
        HandleEvents();
        DoActions();
        UpdateTimer();
        DrawScreen();
    }
}

GLvoid Controller::StartLevel(char * name)
{
	ResetKeyStates();
	UpdateTimer();
	StrCpy(current_level, name);
	Config::getInstance()->SetCurrentLevel(current_level);
	level->CreateLevel(current_level);
	level->TurnLightOn();
	ChangeState(ST_SHOW_LEVEL, ST_PLAY);
}

GLvoid Controller::EndLevel(char * next_level_name)
{
	StrCpy(next_level, next_level_name);
	level->SetAsPassed(current_level, next_level);
	level->TurnLightOff();
	ChangeState(ST_HIDE_LEVEL, ST_LEVELCOMPLETE);
}

GLvoid Controller::StartNextLevel()
{
	StartLevel(next_level);
}

GLvoid Controller::RestartLevel()
{
	StartLevel(current_level);
}

GLvoid Controller::StartGame()
{
	Config * config = Config::getInstance();
	screen = Screen::getInstance();
	screen->LoadImage((char*)LOADING_IMAGE);
	screen->Render();
	RcRegistry::getInstance()->DoLoading();
	level = new Level();
	level->LoadProgress();
	screen->InitMenus();
	StrCpy(current_level, config->GetCurrentLevel());
	if (config->GetMusic()) {
		RcRegistry::getInstance()->PlayMusic(MUSIC_BACKGROUND);
	}
	GetTimePast();
	screen->FadeOut();
	ChangeState(ST_FADE_OUT_SCREEN, ST_AFTER_LOADING);
    MainLoop();
}

GLvoid Controller::PauseGame()
{
	level->TurnLightOff();
	ChangeState(ST_HIDE_LEVEL, ST_PAUSE);
	screen->PrepareMenu(MENU_PAUSE);
}

GLvoid Controller::ResumeGame()
{
	level->TurnLightOn();
	ChangeState(ST_SHOW_LEVEL, ST_PLAY);
}

GLvoid Controller::ToMap()
{
	ChangeState(ST_MAP, 0);
	screen->PrepareMap(level->GetLevelsPreviews(), level->GetCountLevels());
	screen->SetMapCursorPosition(level->FindLevel(current_level));
}

GLvoid Controller::SelectLevel(LevelPreview * l)
{
	if (!l->locked) {
		StartLevel(l->szName);
	}
}

GLvoid Controller::ToMain()
{
	ChangeState(ST_MAIN, 0);
	screen->PrepareMenu(MENU_MAIN);
}

GLvoid Controller::ToConfig()
{
	ChangeState(ST_CONFIG, 0);
	Config * config = Config::getInstance();
	screen->PrepareMenu(MENU_CONFIG);
	screen->SetOptionValue(MI_OPTS_FULLSCREEN, config->GetFullscreen() ? OPT_VALUE_ON : OPT_VALUE_OFF);
	screen->SetOptionValue(MI_OPTS_CAMERA, config->GetCamera());
	screen->SetOptionValue(MI_OPTS_DRAW_WALLS, config->GetDrawWalls() ? OPT_VALUE_ON : OPT_VALUE_OFF);
	screen->SetOptionValue(MI_OPTS_SOUND, config->GetSound() ? OPT_VALUE_ON : OPT_VALUE_OFF);
	screen->SetOptionValue(MI_OPTS_MUSIC, config->GetMusic() ? OPT_VALUE_ON : OPT_VALUE_OFF);
}

GLvoid Controller::ApplyConfig()
{
	Config * config = Config::getInstance();
	SDL_Surface *vs = SDL_GetVideoSurface();
	// toggle fullscreen
	if (screen->GetOptionValue(MI_OPTS_FULLSCREEN) == OPT_VALUE_ON) {
		if (!(vs->flags & SDL_FULLSCREEN)) {
			SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
		}
	} else if (screen->GetOptionValue(MI_OPTS_FULLSCREEN) != OPT_VALUE_ON) {
		if (vs->flags & SDL_FULLSCREEN) {
			SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
		}
	}
	// toggle music
	if (screen->GetOptionValue(MI_OPTS_MUSIC) == OPT_VALUE_ON) {
		if (!Mix_PlayingMusic()) {
			RcRegistry::getInstance()->PlayMusic(MUSIC_BACKGROUND);
		}
	} else if (screen->GetOptionValue(MI_OPTS_MUSIC) != OPT_VALUE_ON) {
		if (Mix_PlayingMusic()) {
			Mix_HaltMusic();
		}
	}
	config->SetFullscreen(screen->GetOptionValue(MI_OPTS_FULLSCREEN) == OPT_VALUE_ON ? 1 : 0);
	config->SetCamera(screen->GetOptionValue(MI_OPTS_CAMERA));
	config->SetDrawWalls(screen->GetOptionValue(MI_OPTS_DRAW_WALLS) == OPT_VALUE_ON ? 1 : 0);
	config->SetSound(screen->GetOptionValue(MI_OPTS_SOUND) == OPT_VALUE_ON ? 1 : 0);
	config->SetMusic(screen->GetOptionValue(MI_OPTS_MUSIC) == OPT_VALUE_ON ? 1 : 0);
}

GLvoid Controller::NextLevelMenu()
{
	ChangeState(ST_NEXT_LEVEL_MENU, 0);
	screen->PrepareMenu(MENU_NEXT_ROOM);
	LevelPreview * l = level->GetLevelInfo(next_level);
	if (l) {
		screen->SetSubtitle(l->szTitle);
	} else {
		fprintf(stderr, "Error: Level name not found %s \n", next_level);
		QuitGame(1);
	}
}

GLvoid Controller::ToTitleScreen()
{
	screen->SetImage(RcRegistry::getInstance()->GetImage(IMAGE_TITLE_SCREEN));
	screen->FadeIn();
	timer = TITLE_TIMER;
	ChangeState(ST_FADE_IN_SCREEN, ST_TITLE_SCREEN);
}

GLvoid Controller::ToCredits()
{
	screen->SetImage(RcRegistry::getInstance()->GetImage(IMAGE_CREDITS));
	screen->Render();
	timer = CREDITS_TIMER;
	ChangeState(ST_CREDITS_SCREEN, ST_END_CREDITS_SCREEN);
}

GLvoid Controller::ChangeState(GLshort state, GLshort next_state)
{
	switch (state) {
		case ST_AFTER_LOADING:
			screen->DestroyImage();
			ToTitleScreen();
			return;
		case ST_AFTER_TITLE_SCREEN:
			ToMain();
			return;
		case ST_LEVELCOMPLETE:
			NextLevelMenu();
			return;
		case ST_TITLE_SCREEN:
			next_state = ST_END_TITLE_SCREEN;
			break;
		case ST_END_TITLE_SCREEN:
			screen->FadeOut();
			ChangeState(ST_FADE_OUT_SCREEN, ST_AFTER_TITLE_SCREEN);
			return;
		case ST_END_CREDITS_SCREEN:
			QuitGame(0);
			return;
	}
	game_state = state;
	next_game_state = next_state;
}

Controller::~Controller() 
{
	if (level) {
		delete level;
	}
}
