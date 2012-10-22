#include "rc.h"

char* szModelFileNames[] = {
	(char*)"Modeles/hero.3dm",	// 		MODEL_HERO 			0
	(char*)"Modeles/block1.3dm", // 	MODEL_BL_NORMAL 	1
	(char*)"Modeles/block2.3dm", // 	MODEL_BL_CRACKED 	2
	(char*)"Modeles/block3.3dm", //		MODEL_BL_TRANSPORT 	3
	(char*)"Modeles/teleport.3dm", //	MODEL_TELEPORT 		4
	(char*)"Modeles/door.3dm",	//		MODEL_EXIT_DOOR 	5
	(char*)"Modeles/wall.3dm", //		MODEL_WALL 			6
	(char*)"Modeles/window.3dm", //		MODEL_WINDOW		7
	(char*)"Modeles/elevator.3dm", //	MODEL_ELEVATOR		8
	(char*)"Modeles/button.3dm", //		MODEL_BUTTON		9
	(char*)"Modeles/table.3dm", //		MODEL_TABLE			10
	(char*)"Modeles/bigtable.3dm", // 	MODEL_BIG_TABLE		11
	(char*)"Modeles/chair.3dm", //		MODEL_CHAIR			12
	(char*)"Modeles/flower.3dm", //		MODEL_FLOWER		13
	(char*)"Modeles/clock.3dm", //		MODEL_CLOCK			14
	(char*)"Modeles/bookcase.3dm", //	MODEL_BOOKCASE		15
	(char*)"Modeles/key.3dm", //		MODEL_KEY			16
	(char*)"Modeles/base.3dm", //		MODEL_BASE			17
	(char*)"Modeles/bed.3dm", //		MODEL_BED			18
	(char*)"Modeles/wall2.3dm" //		MODEL_WALL2			19
};

struct FontConf {
	char* szName;
	GLint size;
} fontConfigs[] = {
	{(char*)"Fonts/EagleLake-Regular.ttf", 24}, 	// 	FONT_TITLE		0
	{(char*)"Fonts/vinque.ttf", 18},		 		//	FONT_MENUITEMS	1
	{(char*)"Fonts/EagleLake-Regular.ttf", 10}, 	// 	FONT_VERSION	2
};

char* szImageFileNames[] = {
	(char*)"Images/main.png",	// 		IMAGE_MAIN_SCREEN 	0
	(char*)"Images/pause.png", // 		IMAGE_PAUSE_SCREEN 	1
	(char*)"Images/title.png", // 		IMAGE_TITLE_SCREEN 	2
	(char*)"Images/credits.png", //		IMAGE_CREDITS 		3
	(char*)"Images/gui-items.png" //	IMAGE_GUI_ITEMS		4
};

char* szMusicFileNames[] = {
	(char*)"Sounds/shelkunchik.ogg",	// 	MUSIC_BACKGROUND 		0
};

char* szSoundFileNames[] = {
	(char*)"Sounds/button_down.wav",	// 	SOUND_BUTTON_DOWN 		0
	(char*)"Sounds/button_up.wav",		// 	SOUND_BUTTON_UP 		1
	(char*)"Sounds/door.wav",			// 	SOUND_DOOR		 		2
	(char*)"Sounds/rolling.wav",		// 	SOUND_ROLLING	 		3
	(char*)"Sounds/stone_col.wav",		// 	SOUND_STONE_COLLISION 	4
	(char*)"Sounds/stone_break.wav",	// 	SOUND_STONE_BREAK 		5
	(char*)"Sounds/teleport.wav"		// 	SOUND_TELEPORT	 		6
};

RcRegistry * RcRegistry::instance = NULL;

RcRegistry::RcRegistry() 
{
	GLshort i;
	count_models = (GLushort)(sizeof(szModelFileNames) / sizeof(char*));
	count_fonts = (GLushort)(sizeof(fontConfigs) / sizeof(FontConf));
	count_images = (GLushort)(sizeof(szImageFileNames) / sizeof(char*));
	count_music = (GLushort)(sizeof(szMusicFileNames) / sizeof(char*));
	count_sounds = (GLushort)(sizeof(szSoundFileNames) / sizeof(char*));
	models = new MODEL_3D[count_models];
	fonts = new TTF_Font*[count_fonts];
	for (i = 0; i < count_fonts; i++) {
		fonts[i] = NULL;
	}
	images = new SDL_Surface*[count_images];
	for (i = 0; i < count_images; i++) {
		images[i] = NULL;
	}
	music = new Mix_Music*[count_music];
	for (i = 0; i < count_music; i++) {
		music[i] = NULL;
	}
	sounds = new Mix_Chunk*[count_sounds];
	for (i = 0; i < count_sounds; i++) {
		sounds[i] = NULL;
	}
}

RcRegistry::~RcRegistry() 
{
	GLshort i;
	delete[] models;
	
	for (i = 0; i < count_fonts; i++) {
		if (fonts[i]) {
			TTF_CloseFont(fonts[i]);
		}
	}
	delete[] fonts;
	TTF_Quit();
	
	for (i = 0; i < count_images; i++) {
		if (images[i]) {
			SDL_FreeSurface(images[i]);
		}
	}
	delete[] images;
	
	Mix_HaltMusic();
	for (i = 0; i < count_music; i++) {
		if (music[i]) {
			Mix_FreeMusic(music[i]);
		}
	}
	delete[] music;
	
	for (i = 0; i < count_sounds; i++) {
		if (sounds[i]) {
			Mix_FreeChunk(sounds[i]);
		}
	}
	delete[] sounds;
	Mix_CloseAudio();
}

GLvoid RcRegistry::DoLoading()
{
	GLushort i;
	for (i = 0; i < count_models; i++) {
		if (!models[i].Load(szModelFileNames[i])) {
			fprintf(stderr, "Error: model can't be loaded %s \n", szModelFileNames[i]);
		}
	}
	if (TTF_Init() == -1) {
		fprintf(stderr, "Unable to initialize SDL_ttf: %s \n", TTF_GetError());
	} 
	for (i = 0; i < count_fonts; i++) {
		if (NULL == (fonts[i] = TTF_OpenFont(fontConfigs[i].szName, fontConfigs[i].size))) {
			fprintf(stderr, "Error: font can't be opened %s \n", fontConfigs[i].szName);
		}
	}
	for (i = 0; i < count_images; i++) {
		if (NULL == (images[i] = IMG_Load(szImageFileNames[i]))) {
			fprintf(stderr, "Error: image can't be loaded %s \n", szImageFileNames[i]);
		}
	}
	
	if(Mix_OpenAudio(22050, AUDIO_S16, 1, 4096)) {
		fprintf(stderr, "Error: Unable to open audio \n");
	} else {
		for (i = 0; i < count_music; i++) {
			if (NULL == (music[i] = Mix_LoadMUS(szMusicFileNames[i]))) {
				fprintf(stderr, "Error: music can't be loaded %s \n", szMusicFileNames[i]);
			}
		}
		for (i = 0; i < count_sounds; i++) {
			if (NULL == (sounds[i] = Mix_LoadWAV(szSoundFileNames[i]))) {
				fprintf(stderr, "Error: sound can't be loaded %s \n", szSoundFileNames[i]);
			}
		}
	}
}

MODEL_3D * RcRegistry::GetModel(GLushort id)
{
	if (id >= count_models) {
		fprintf(stderr, "Error: model with id <%d> id is not in registry \n", id);
		return 0;
	} else {
		return &models[id];
	}
}

GLvoid RcRegistry::CloneModel(MODEL_3D * src, GLushort id, GLushort flag)
{
	MODEL_3D * dest = GetModel(id);
	if (id) {
		if (flag & CLONE_MESH) {
			src->SetMeshLink(*dest);
		}
		if (flag & CLONE_MATERIAL) {
			src->SetMaterialLink(*dest);
		}
		if (flag & CLONE_ANIMATION) {
			src->SetAnimationLink(*dest);
		}
	}
}

TTF_Font * RcRegistry::GetFont(GLushort id)
{
	if (id >= count_fonts) {
		fprintf(stderr, "Error: font with id <%d> id is not in registry \n", id);
		return 0;
	} else {
		return fonts[id];
	}
}

SDL_Surface * RcRegistry::GetImage(GLushort id)
{
	if (id >= count_images) {
		fprintf(stderr, "Error: image with id <%d> id is not in registry \n", id);
		return 0;
	} else {
		return images[id];
	}
}

GLvoid RcRegistry::PlayMusic(GLushort id)
{
	if (id >= count_music) {
		fprintf(stderr, "Error: music with id <%d> id is not in registry \n", id);
	} else {
		if (music[id]) {
			Mix_PlayMusic(music[id], -1);
		}
	}
}

GLvoid RcRegistry::PlaySound(GLushort id)
{
	if (id >= count_sounds) {
		fprintf(stderr, "Error: sound with id <%d> id is not in registry \n", id);
	} else {
		if (sounds[id]) {
			Mix_PlayChannel(-1, sounds[id], 0);
		}
	}
}

GLvoid RcRegistry::HaltMusic()
{
	Mix_HaltMusic();
}
