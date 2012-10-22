#include "controller.h"
#include "display.h"

int main(int argc, char* argv[])
{
	Config * config = Config::getInstance();
	config->Load();
	DisplayOptions opts = {800, 600, 0};
	if (config->GetFullscreen()) {
		opts.flags = SDL_FULLSCREEN;
	}
	Display::createInstance(opts);
    Controller::getInstance()->StartGame();
    return 0;
}
