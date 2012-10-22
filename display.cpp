#include "display.h"
#include "rc.h"

Display * Display::instance = NULL;

Display::Display(DisplayOptions & options) 
{
	icon = NULL;
	
	// setup SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
        Controller::getInstance()->QuitGame(1);
    }
    
    info = SDL_GetVideoInfo();
    if(!info) {
        fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
        Controller::getInstance()->QuitGame(1);
    }
    
    GLint bpp = info->vfmt->BitsPerPixel;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    icon = IMG_Load((char*)ICON_IMAGE);
    SDL_WM_SetIcon(icon, NULL);
    
    screen = SDL_SetVideoMode(options.width, options.height, bpp, options.flags | SDL_OPENGLBLIT);
    if(screen == 0) {
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		Controller::getInstance()->QuitGame(1);
	}
	SDL_WM_SetCaption(GAME_TITLE, NULL);
	SDL_ShowCursor(0);

    // Setup OpenGL
    GLfloat light_diffuse[] = {LIGHT_DIFFUSE, LIGHT_DIFFUSE, LIGHT_DIFFUSE, 0},
			light_position[] = {LIGHT_POSITION};
	glClearColor(0, 0, 0, 0.0f);	
	glClearDepth(1);					
	glDepthFunc(GL_LESS);				
	glEnable(GL_DEPTH_TEST);			
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	LigthModel(LIGHT_MODEL_AMBIENT);
	glShadeModel(GL_SMOOTH);	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)options.width / (GLfloat)options.height, 1, 50);
	glMatrixMode(GL_MODELVIEW);	
}

Display::~Display() 
{
	if (icon) {
		SDL_FreeSurface(icon);
	}
}
