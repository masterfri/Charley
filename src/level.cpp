#include "level.h"
#include "3dml.h"
#include "rc.h"
#include "controller.h"

char * furniture_names[] = {
	(char*)"throne", //		FURNITURE_THRONE	0
	(char*)"flower", //		FURNITURE_FLOWER	1
	(char*)"mvclock", //	FURNITURE_CLOCK		2
	(char*)"bigtable", //	FURNITURE_BIG_TABLE	3
	(char*)"table", //		FURNITURE_TABLE		4
	(char*)"stillage", //	FURNITURE_BOOK_CASE	5
	(char*)"bed", //		FURNITURE_BED		6
	(char*)0
};

Level::Level()
{
	hero = new Hero(this);
	spectator = new Spectator(hero);
	
	if (levels_data.Read((char *)RC_ROOT"Levels/levels.ds") != 0) {
		fprintf(stderr, "Error: Data from `Levels/levels.ds` was not loaded \n");
		Controller::getInstance()->QuitGame(1);
	}
	CreateLevelsPreviews();
	
	InitKeys();
	InitWalls();
	base = RcRegistry::getInstance()->GetModel(MODEL_BASE);
	
	memset(tiles, 0, MAPSIZE * MAPSIZE * sizeof(MODEL_3D *));
	memset(walls, 0, MAPSIZE * 4 * sizeof(MODEL_3D *));
	furniture = NULL;
	for (GLshort i = 0; i < MAX_EXIT; i++) {
		exits[i].door = NULL;
	}
}

Level::~Level()
{
	DestroyLevel();
	DestroyPreviews();
	delete hero;
	delete spectator;
}

GLvoid Level::InitKeys()
{
	char mn[] = "face_";
	RcRegistry * registry = RcRegistry::getInstance();
	for (GLshort i = 0; i < MAX_EXIT; i++) {
		registry->CloneModel(&key[i], MODEL_KEY, CLONE_ALL);
		key[i].SelectObj((char*)"Ring");
		mn[4] = 0x30 + i;
		key[i].SetObjMaterial(mn);
	}
}

GLvoid Level::InitWalls()
{
	RcRegistry * registry = RcRegistry::getInstance();
	for (GLushort i = 0; i < 4; i++) {
		registry->CloneModel(&topwalls[i], MODEL_WALL2, CLONE_MESH | CLONE_MATERIAL);
	}
	topwalls[0].MoveAt(11, 11.38f, -1);
	topwalls[1].MoveAt(11, 11.38f, 23);
	topwalls[1].Rotate(180, ORT_Y);
	topwalls[2].MoveAt(-1, 11.38f, 11);
	topwalls[2].Rotate(90, ORT_Y);
	topwalls[3].MoveAt(23, 11.38f, 11);
	topwalls[3].Rotate(270, ORT_Y);
}

GLvoid Level::CreateLevelsPreviews()
{
	GLushort i = 0, j;
	char file[256], path[64], tmp[256];
	levels_data.BeginEnumeration(NULL);
	levels_count = 0;
	while (levels_data.Enumerate(0, 0)) {
		levels_count++;
	}
	levels = new LevelPreview[levels_count];
	
	// Read basic info
	levels_data.BeginEnumeration(NULL);
	while (levels_data.Enumerate(levels[i].szName, 0)) {
		sprintf(path, "%s.name", levels[i].szName);
		levels_data.ExtractData((char*)path, levels[i].szTitle, 0);
		sprintf(path, "%s.preview", levels[i].szName);
		levels_data.ExtractData((char*)path, tmp, 0);
		sprintf(file, "%s%s", RC_ROOT, tmp);
		levels[i].picture = IMG_Load(file);
		if (! levels[i].picture) {
			fprintf(stderr, "Error: Image `%s` not found \n", file);
		}
		levels[i].locked = GL_TRUE;
		sprintf(path, "%s.exit_to", levels[i].szName);
		levels_data.ExtractData(path, levels[i].way_to, 0);
		levels[i].exit_count = 0;
		for (j = 0; j < MAX_EXIT; j++) {
			if (levels[i].way_to[j]) {
				levels[i].exit_count++;
				levels[i].keys[j] = GL_FALSE;
			} else {
				break;
			}
		}
		i++;
	}
	
	GLshort start = FindLevel((char*) "start");
	if (start == -1) {
		fprintf(stderr, "Error: Initial level (start) not found \n");
		Controller::getInstance()->QuitGame(1);
	}
	levels[start].locked = GL_FALSE;
}

GLvoid Level::DestroyPreviews()
{
	GLshort i;
	for (i = 0; i < levels_count; i++) {
		if (levels[i].picture) {
			SDL_FreeSurface(levels[i].picture);
		}
	}
	delete[] levels;
}

GLboolean Level::CreateLevel(char * szName)
{
#ifdef DEBUG_MODE
printf("Building level `%s` \n", szName);
#endif
	
	GLshort i, j;
	
	// destroy previous level
	DestroyLevel();
	
	for (i = 0; i < MAX_EXIT; i++) {
		// all doors are opened by default
		exits[i].exit_open = GL_TRUE;
	}

	char *buf[MAPSIZE + 1], *ptr;
	SpNodePtr epos = levels_data.GoToNode(szName);
	if (!epos) {
		fprintf(stderr, "Error: Level `%s` was not found \n", szName);
		return GL_FALSE;
	}

	// read floor data
	if (! levels_data.ExtractData((char*)"way", buf, 0, epos)) {
		fprintf(stderr, "Error: `way` is missing in level description \n");
		return GL_FALSE;
	}
	for (i = 0; i < MAPSIZE; i++){
		ptr = buf[i];
		for (j = 0; j < MAPSIZE; j++){			
			if (*ptr == ' ') {
				CreateEmptyFloorTile(i, j);
			} else {
				switch (*ptr) {
					case 'N':
					case 'K':
					case 'S':
						CreateNormalFloorTile(i, j);
						if (*ptr == 'S') {
							InitiateHeroPosition(i, j, DecodeDirection(*(++ptr)));
						} else if (*ptr == 'K') {
							DropKey(i, j, *(++ptr) - 0x30);
						}
						break;
					case 'D':
						CreateCrackedFloorTile(i, j);
						break;
					case 'T':
						CreateTeleport(i, j, *(++ptr));
						break;
					case 'E':
						CreateElevator(i, j, *(++ptr), *(++ptr));
						break;
					case 'B':
						CreateButton(i, j, *(++ptr));
						break;
					case 'M':
						CreateTransporter(i, j);
						break;
				}
			}
			ptr++;
		}
	}

	// read wall data
	if (!levels_data.ExtractData((char*)"walls", buf, 0, epos)) {
		fprintf(stderr, "Error: `walls` is missing in level description \n");
		return GL_FALSE;
	}
	for (i = 0; i < 4; i++) {
		ptr = buf[i];
		for (j = 0; j < MAPSIZE; j++) {
			switch (*ptr) {
				case 'E':
					CreateExit(*(++ptr), i, MAPSIZE * i + j, j + 1);
					j += 2;
					break;
				case 'D':
					CreateDoor(MAPSIZE * i + j, i, j + 1);
					j += 2;
					break;
				case 'W':
					CreateWindow(MAPSIZE * i + j, i, j + 1);
					j += 2;
					break;
				case 'P':
					CreateWallBlock(MAPSIZE * i + j, i, j);
					break;
			}
			ptr++;
		}
	}
	
	// create wallpapers
	char mn[8];
	levels_data.ExtractData((char*)"skin", mn, 0, epos);
	char * lsym = mn + StrLen(mn);
	for (i = 0; i < 4; i++) {
		lsym[0] = (rand() % 3) + 0x31;
		lsym[1] = 0;
		topwalls[i].SelectObj((char*)"Wallp_T");
		topwalls[i].SetObjMaterial(mn);
	}
	
	// create furniture
	furniture_count = 0;
	if (levels_data.BeginEnumeration((char*)"mebles", epos)) {
		while (levels_data.Enumerate(0, 0)) {
			furniture_count++;
		}
		GLint x, y, r;
		char szBuf[64];
		if (furniture_count > 0) {
			furniture = new MODEL_3D[furniture_count];
			levels_data.BeginEnumeration((char*)"mebles", epos);
			for (i = 0; i < furniture_count; i++) {
				SpNodePtr hpos = levels_data.Enumerate(szBuf, 0);
				GLushort furniture_id = GetStrIndex(furniture_names, szBuf);
				levels_data.ExtractData((char*)"x", &x, 0, hpos);
				levels_data.ExtractData((char*)"y", &y, 0, hpos);
				levels_data.ExtractData((char*)"r", &r, 0, hpos);
				switch (furniture_id) {
					case FURNITURE_THRONE:
						CreateThrone(i, x, y, r);
						break;
					case FURNITURE_FLOWER:
						CreateFlower(i, x, y, r);
						break;
					case FURNITURE_CLOCK:
						CreateClock(i, x, y, r);
						break;
					case FURNITURE_BIG_TABLE:
						CreateBigTable(i, x, y, r);
						break;
					case FURNITURE_TABLE:
						CreateTable(i, x, y, r);
						break;
					case FURNITURE_BOOK_CASE:
						CreateBookCase(i, x, y, r);
						break;
					case FURNITURE_BED:
						CreateBed(i, x, y, r);
						break;
				}
			}
		} else {
			furniture = NULL;
		}
	} else {
		furniture = NULL;
	}
	
	// get exit ways
	char * way_to[MAX_EXIT + 1];
	memset(way_to, 0, sizeof(char*) * (MAX_EXIT + 1));
	levels_data.ExtractData((char*)"exit_to", way_to, 0, epos);
	for (i = 0; i < MAX_EXIT; i++) {
		if (way_to[i]) {
			StrCpy(exits[i].way_to, way_to[i]);
			
#ifdef DEBUG_MODE
printf("Exit #%d at %d %d leads to %s \n", i, exits[i].x, exits[i].y, exits[i].way_to);
#endif
		}
	}
	
	return StartLevel();
}

GLvoid Level::CreateEmptyFloorTile(GLushort x, GLushort y)
{
	GLushort index = GetTileIndex(x, y);
	cells[index].moved = NO_MOVEDTILE;
	cells[index].type = TT_NONE;
	tiles[index] = NULL;
}

GLvoid Level::CreateNormalFloorTile(GLushort x, GLushort y)
{
	GLushort index = GetTileIndex(x, y);
	tiles[index] = new MODEL_3D();
	tiles[index]->MoveAt(GLfloat(x) * STEP, 0, GLfloat(y) * STEP);
	RcRegistry::getInstance()->CloneModel(tiles[index], MODEL_BL_NORMAL, CLONE_MESH | CLONE_MATERIAL);
	cells[index].moved = NO_MOVEDTILE;
	cells[index].type = TT_TRICK; 
	cells[index].val = 0;
}

GLvoid Level::CreateCrackedFloorTile(GLushort x, GLushort y)
{
	GLushort index = GetTileIndex(x, y);
	tiles[index] = new MODEL_3D();
	tiles[index]->MoveAt(GLfloat(x) * STEP, 0, GLfloat(y) * STEP);
	RcRegistry::getInstance()->CloneModel(tiles[index], MODEL_BL_CRACKED, CLONE_ALL);
	cells[index].moved = NO_MOVEDTILE;
	cells[index].type = TT_FOS;
	cells[index].val = 1;
}

GLvoid Level::CreateTeleport(GLushort x, GLushort y, char id)
{
	GLushort index = GetTileIndex(x, y);
	char mn[] = "mr1";
	mn[2] = id;
	tiles[index] = new MODEL_3D();
	tiles[index]->MoveAt(GLfloat(x) * STEP, 0, GLfloat(y) * STEP);
	RcRegistry::getInstance()->CloneModel(tiles[index], MODEL_TELEPORT, CLONE_ALL);
	tiles[index]->SelectAnimation((char*)"stand");
	tiles[index]->LoopAnimation();
	tiles[index]->SelectObj((char*)"Mark1");
	tiles[index]->SetObjMaterial(mn);
	tiles[index]->SelectObj((char*)"Mark2");
	tiles[index]->SetObjMaterial(mn);
	cells[index].moved = NO_MOVEDTILE;
	cells[index].type = TT_TELEPORT;
	cells[index].val = id - 0x30;

#ifdef DEBUG_MODE
printf("Teleport added at %d %d, ID = %c \n", x, y, id);
#endif	
}

GLvoid Level::CreateElevator(GLushort x, GLushort y, char state, char id)
{
	GLushort index = GetTileIndex(x, y);
	char mn[] = "mat_";
	mn[3] = id;
	tiles[index] = new MODEL_3D();
	tiles[index]->MoveAt(GLfloat(x) * STEP, 0, GLfloat(y) * STEP);
	RcRegistry::getInstance()->CloneModel(tiles[index], MODEL_ELEVATOR, CLONE_ALL);
	tiles[index]->SelectObj((char*)"block");
	tiles[index]->SetObjMaterial(mn);
	tiles[index]->SelectAnimation((char*)"up");
	cells[index].moved = NO_MOVEDTILE;
	cells[index].type = TT_ELEVATOR;
	cells[index].val = id - 0x30;
	if (state == 'D') {
		cells[index].flag = ES_STANDDOWN;
	} else if (state == 'U') {
		cells[index].flag = ES_STANDUP;
		tiles[index]->SetTimerValue(tiles[index]->GetAnimaLen(NULL) - 1);
	}

#ifdef DEBUG_MODE
printf("Elevator added at %d %d, ID = %c (%c) \n", x, y, id, state);
#endif	
}

GLvoid Level::CreateButton(GLushort x, GLushort y, char id)
{
	GLushort index = GetTileIndex(x, y);
	char mn[] = "mark_";
	mn[4] = id;
	tiles[index] = new MODEL_3D();
	tiles[index]->MoveAt(GLfloat(x) * STEP, 0, GLfloat(y) * STEP);
	RcRegistry::getInstance()->CloneModel(tiles[index], MODEL_BUTTON, CLONE_ALL);
	tiles[index]->SelectObj((char*)"button");
	tiles[index]->SetObjMaterial(mn);
	cells[index].moved = NO_MOVEDTILE;
	cells[index].type = TT_BUTTON;
	cells[index].val = id - 0x30; 
	cells[index].flag = 0;
	
#ifdef DEBUG_MODE
printf("Button added at %d %d, ID = %c \n", x, y, id);
#endif	
}

GLvoid Level::CreateTransporter(GLushort x, GLushort y)
{
	GLushort index = GetTileIndex(x, y);
	tiles[index] = new MODEL_3D();
	tiles[index]->MoveAt(GLfloat(x) * STEP, 0, GLfloat(y) * STEP);
	RcRegistry::getInstance()->CloneModel(tiles[index], MODEL_BL_TRANSPORT, CLONE_MESH | CLONE_MATERIAL);
	cells[index].type = TT_NONE; 
	cells[index].moved = index;
	
#ifdef DEBUG_MODE
printf("Transporter added at %d %d \n", x, y);
#endif	
}

GLvoid Level::DropKey(GLushort x, GLushort y, GLshort index)
{
	exits[index].exit_open = GL_FALSE;
	exits[index].key_x = x;
	exits[index].key_y = y;
	key[index].MoveAt(GLfloat(x) * STEP, 0, GLfloat(y) * STEP);
	key[index].SelectAnimation((char*)"stand");
	key[index].LoopAnimation();
	
#ifdef DEBUG_MODE
printf("Key added at %d %d, ID = %d \n", x, y, index);
#endif	
}

GLvoid Level::CreateExit(char id, GLushort wall, GLushort wallindex, GLushort offset)
{
	GLushort index = id - 0x30;
	char mn[] = "EM_";
	mn[2] = id + 1;
	exits[index].door = new MODEL_3D();
	RcRegistry::getInstance()->CloneModel(exits[index].door, MODEL_EXIT_DOOR, CLONE_ALL);
	exits[index].door->SelectObj((char*)"Exit_Mark");
	exits[index].door->SetObjMaterial(mn);
	DisposeWall(exits[index].door, wall, offset);
	switch (wall) {
		case WEST:
			exits[index].y = offset;
			exits[index].x = -1;
			break;
		case NORTH:
			exits[index].y = -1;
			exits[index].x = offset;
			break;
		case OST:
			exits[index].y = offset;
			exits[index].x = MAPSIZE;
			break;
		case SOUTH:
			exits[index].y = MAPSIZE;
			exits[index].x = offset;
			break;
	}
	walls[wallindex] = NULL;
	walls[wallindex + 1] = NULL;
	walls[wallindex + 2] = NULL;
}

GLvoid Level::CreateWallBlock(GLushort index, GLushort wall, GLushort offset)
{
	walls[index] = new MODEL_3D();
	RcRegistry::getInstance()->CloneModel(walls[index], MODEL_WALL, CLONE_MESH | CLONE_MATERIAL);
	DisposeWall(walls[index], wall, offset);
}

GLvoid Level::CreateDoor(GLushort index, GLushort wall, GLushort offset)
{
	walls[index] = new MODEL_3D();
	RcRegistry::getInstance()->CloneModel(walls[index], MODEL_EXIT_DOOR, CLONE_MESH | CLONE_MATERIAL);
	DisposeWall(walls[index], wall, offset);
	walls[index + 1] = NULL;
	walls[index + 2] = NULL;
}

GLvoid Level::CreateWindow(GLushort index, GLushort wall, GLushort offset)
{
	walls[index] = new MODEL_3D();
	RcRegistry::getInstance()->CloneModel(walls[index], MODEL_WINDOW, CLONE_ALL);
	if ((rand() % 2)) {
		walls[index]->SelectAnimation((char*)"stand");
		walls[index]->LoopAnimation();					
	}
	DisposeWall(walls[index], wall, offset);
	walls[index + 1] = NULL;
	walls[index + 2] = NULL;
}

GLvoid Level::DisposeWall(MODEL_3D * model, GLushort wall, GLushort offset)
{
	switch (wall) {
		case WEST:
			model->MoveAt(-1, 0, GLfloat(offset) * STEP);
			model->Rotate(90, ORT_Y);
			break;
		case NORTH:
			model->MoveAt(GLfloat(offset) * STEP, 0, -1);
			break;
		case OST:
			model->MoveAt(MAPSIZE * STEP - 1, 0, GLfloat(offset) * STEP);
			model->Rotate(270, ORT_Y);
			break;
		case SOUTH:
			model->MoveAt(GLfloat(offset) * STEP, 0, MAPSIZE * STEP - 1);					
			model->Rotate(180, ORT_Y);
			break;
	}
}

GLvoid Level::CreateThrone(GLushort index, GLint x, GLint y, GLint r)
{
	RcRegistry::getInstance()->CloneModel(&furniture[index], MODEL_CHAIR, CLONE_MESH | CLONE_MATERIAL);
	furniture[index].MoveAt(GLfloat(STEP * x), 0, GLfloat(STEP * y));
	furniture[index].Rotate(GLfloat(r), ORT_Y);
	cells[GetTileIndex(x, y)].val = 1;
	
#ifdef DEBUG_MODE
printf("Throne added at %d %d, %d \n", x, y, r);
#endif
}

GLvoid Level::CreateFlower(GLushort index, GLint x, GLint y, GLint r)
{
	RcRegistry::getInstance()->CloneModel(&furniture[index], MODEL_FLOWER, CLONE_MESH | CLONE_MATERIAL);
	furniture[index].MoveAt(GLfloat(STEP * x), 0, GLfloat(STEP * y));
	furniture[index].Rotate(GLfloat(r), ORT_Y);
	cells[GetTileIndex(x, y)].val = 1;
	
#ifdef DEBUG_MODE
printf("Flower added at %d %d, %d \n", x, y, r);
#endif
}

GLvoid Level::CreateClock(GLushort index, GLint x, GLint y, GLint r)
{
	RcRegistry::getInstance()->CloneModel(&furniture[index], MODEL_CLOCK, CLONE_ALL);
	furniture[index].SelectAnimation((char*)"stand");
	furniture[index].LoopAnimation();
	furniture[index].MoveAt(GLfloat(STEP * x), 0, GLfloat(STEP * y));
	furniture[index].Rotate(GLfloat(r), ORT_Y);
	cells[GetTileIndex(x, y)].val = 1;
	
#ifdef DEBUG_MODE
printf("Clock added at %d %d, %d \n", x, y, r);
#endif
}

GLvoid Level::CreateBigTable(GLushort index, GLint x, GLint y, GLint r)
{
	RcRegistry::getInstance()->CloneModel(&furniture[index], MODEL_BIG_TABLE, CLONE_MESH | CLONE_MATERIAL);
	furniture[index].MoveAt(GLfloat(STEP * x) + 1, 0, GLfloat(STEP * y) + 1);
	furniture[index].Rotate(GLfloat(r), ORT_Y);
	cells[GetTileIndex(x, y)].val = 1;
	cells[GetTileIndex(x + 1, y)].val = 1;
	cells[GetTileIndex(x, y + 1)].val = 1;
	cells[GetTileIndex(x + 1, y + 1)].val = 1;
						
#ifdef DEBUG_MODE
printf("Big table added at %d %d, %d \n", x, y, r);
#endif
}

GLvoid Level::CreateTable(GLushort index, GLint x, GLint y, GLint r)
{
	RcRegistry::getInstance()->CloneModel(&furniture[index], MODEL_TABLE, CLONE_ALL);
	if (r == 90 || r == 270) {
		furniture[index].MoveAt(GLfloat(STEP * x), 0, GLfloat(STEP * y) + 1);						
		cells[GetTileIndex(x, y + 1)].val = 1;
	} else {
		furniture[index].MoveAt(GLfloat(STEP * x) + 1, 0, GLfloat(STEP * y));
		cells[GetTileIndex(x + 1, y)].val = 1;
	}
	furniture[index].Rotate(GLfloat(r), ORT_Y);
	furniture[index].SelectAnimation((char*)"stand");
	furniture[index].LoopAnimation();					
	cells[GetTileIndex(x, y)].val = 1;
	
#ifdef DEBUG_MODE
printf("Table added at %d %d, %d \n", x, y, r);
#endif
}

GLvoid Level::CreateBookCase(GLushort index, GLint x, GLint y, GLint r)
{
	RcRegistry::getInstance()->CloneModel(&furniture[index], MODEL_BOOKCASE, CLONE_MESH | CLONE_MATERIAL);
	if (r == 90 || r == 270){
		furniture[index].MoveAt(GLfloat(STEP * x), 0, GLfloat(STEP * y) + 1);
		cells[GetTileIndex(x, y + 1)].val = 1;
	} else {
		furniture[index].MoveAt(GLfloat(STEP * x) + 1, 0, GLfloat(STEP * y));
		cells[GetTileIndex(x + 1, y)].val = 1;
	}
	furniture[index].Rotate(GLfloat(r), ORT_Y);
	cells[GetTileIndex(x, y)].val = 1;	
	
#ifdef DEBUG_MODE
printf("Bookcase added at %d %d, %d \n", x, y, r);
#endif
}

GLvoid Level::CreateBed(GLushort index, GLint x, GLint y, GLint r)
{
	RcRegistry::getInstance()->CloneModel(&furniture[index], MODEL_BED, CLONE_MESH | CLONE_MATERIAL);
	furniture[index].MoveAt(GLfloat(STEP * x) + 1, 0, GLfloat(STEP * y) + 1);
	furniture[index].Rotate(GLfloat(r), ORT_Y);
	cells[GetTileIndex(x, y)].val = 1;
	cells[GetTileIndex(x + 1, y)].val = 1;
	cells[GetTileIndex(x, y + 1)].val = 1;
	cells[GetTileIndex(x + 1, y + 1)].val = 1;
	
#ifdef DEBUG_MODE
printf("Bed added at %d %d, %d \n", x, y, r);
#endif	
}

GLshort Level::DecodeDirection(char dir)
{
	switch (dir) {
		case 'N':
			return NORTH;
		case 'S':
			return SOUTH;
		case 'W':
			return WEST;
		case 'O':
			return OST;
	}
	return NORTH;
}

GLfloat Level::GetAngleByDirection(GLshort dir)
{
	switch (dir) {
		case NORTH:
			return 0;
		case SOUTH:
			return 180;
		case WEST:
			return 90;
		case OST:
			return 270;
	}
	return 0;
}

inline GLvoid Level::InitiateHeroPosition(GLshort x, GLshort y, GLshort d)
{
	hero_start_x = x;
	hero_start_y = y;
	hero_start_d = d;
}

GLboolean Level::StartLevel()
{
	// setup hero
	hero->Setup(hero_start_x, hero_start_y, hero_start_d);
	
	// setup view
	spectator->SetupView(-GLfloat(hero->GetX()) * STEP, -GLfloat(hero->GetY()) * STEP, hero->GetRotation());
	
	// setup doors
	for (GLshort i = 0; i < MAX_EXIT; i++) {
		exits[i].got_key = GL_FALSE;
		if (exits[i].door) {
			if (exits[i].exit_open) {
				exits[i].door->SelectAnimation((char*)"opened");
				exits[i].door->SetTimerValue(exits[i].door->GetAnimaLen(NULL) - 1);
			}
		}
	}
	
	return GL_TRUE;
}

GLvoid Level::Render()
{
	glMatrixMode (GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();
	
	GLshort i;
	hero->SetCamera(spectator);
	
	DrawFloor();
	
	// drawing furniture
	for (i = 0; i < furniture_count; i++) {
		furniture[i].Draw();
	}
	
	// keys drawind
	for (i = 0; i < MAX_EXIT; i++){
		if (exits[i].door != NULL) {
			if (!exits[i].exit_open) {
				if (key[i].Draw() && exits[i].got_key) {
					exits[i].exit_open = GL_TRUE;
				}
			}
		}
	}
	
	// exit drawing
	for (i = 0; i < MAX_EXIT; i++){
		if (exits[i].door) {
			if (exits[i].door->Draw() && exits[i].got_key) {		
				exits[i].door->SetTimerValue(exits[i].door->GetAnimaLen(NULL) - 1);
			}
		}
	}
	
	// Draw walls
	if (Config::getInstance()->GetDrawWalls()) {
		GLboolean show_walls[4];
		GetVisibleWalls(show_walls);
		
		if (show_walls[OST]) {
			DrawWalls(0, MAPSIZE, OST);
		}
		if (show_walls[WEST]) {
			DrawWalls(MAPSIZE, 2 * MAPSIZE, WEST);
		}
		if (show_walls[SOUTH]) {
			DrawWalls(2 * MAPSIZE, 3 * MAPSIZE, SOUTH);
		}
		if (show_walls[NORTH]) {
			DrawWalls(3 * MAPSIZE, 4 * MAPSIZE, NORTH);
		}
	}
	base->Draw();
	// end draw walls
	
	hero->Draw();
	
	// lighting
	if (light_state != STATE_LIGHT_ON) {
		UpdateLighting();
	}
	
	SDL_GL_SwapBuffers();
}

inline GLvoid Level::DrawFloor()
{
	for (GLshort i = 0; i < MAPSIZE * MAPSIZE; i++) {
		if (cells[i].type == TT_NONE) {
			if (cells[i].moved != NO_MOVEDTILE) {
				tiles[cells[i].moved]->Draw();
			}
		} else if(tiles[i]->Draw()) {
			if (cells[i].type == TT_FOS && cells[i].val == 0) {
				cells[i].type = TT_NONE;
				tiles[i]->Free();
				delete tiles[i];
				tiles[i] = NULL;
			} else if (cells[i].type == TT_ELEVATOR) {
				if (cells[i].flag == ES_MOVEUP) {
					cells[i].flag = ES_STANDUP;
					tiles[i]->SetTimerValue(tiles[i]->GetAnimaLen(NULL) - 1);
				} else if (cells[i].flag == ES_MOVEDOWN) {
					cells[i].flag = ES_STANDDOWN;
					tiles[i]->SetTimerValue(0);
				}
			} else if (cells[i].type == TT_BUTTON){
				tiles[i]->SetTimerValue(tiles[i]->GetAnimaLen(NULL) - 1);
			}
		}
		if (cells[i].type == TT_ELEVATOR && cells[i].moved != NO_MOVEDTILE) {
			tiles[cells[i].moved]->Draw();
		}
	}
}

inline GLvoid Level::DrawWalls(GLushort s, GLushort e, GLushort w)
{
	for (GLshort i = s; i < e; i++) {
		if (walls[i]) {
			walls[i]->Draw();
		}
	}
	topwalls[w].Draw();
}

inline GLvoid Level::GetVisibleWalls(GLboolean * w)
{
	GLshort r = spectator->GetNormalRotation();
	w[WEST] = ((r < 70 || r > 290) || (((r < 95 || r > 265) && spectator->GetY() < -6)));
	w[NORTH] = ((r < 250 && r > 110) || (r < 285 && r > 75 && spectator->GetY() > -MAPSIZE * STEP + 6));
	w[OST] = ((r > 200 && r < 340)  || (((r > 175 || r < 5) && spectator->GetX() < -6)));
	w[SOUTH] = (r < 160 && r > 20 || (((r < 185 || r > 355) && spectator->GetX() > -MAPSIZE * STEP + 6)));
}

GLboolean Level::HeroCanGo(GLshort x, GLshort y, GLshort direction)
{
	GLshort next_x = x, next_y = y;
	GLshort index;
	GetNextPosition(next_x, next_y, direction);
	if (ThereIsExit(next_x, next_y) != -1) {
		return GL_TRUE;
	}
	// hero should't leave game area
	if (next_x >= MAPSIZE || next_y >= MAPSIZE || next_x < 0 || next_y < 0) {
		return GL_FALSE;
	}
	// there is no tile
	index = next_x * MAPSIZE + next_y;
	if (TT_NONE == cells[index].type && NO_MOVEDTILE == cells[index].moved) {
		return GL_FALSE;
	}
	// there is teleport
	if (TT_TELEPORT == cells[index].type) {
		return GL_FALSE;
	}
	// there is something on the way
	if (TT_TRICK == cells[index].type && 1 == cells[index].val) {
		return GL_FALSE;
	}
	// there is elevator, but it is lifted
	if (TT_ELEVATOR == cells[index].type && ES_STANDDOWN != cells[index].flag && NO_MOVEDTILE == cells[index].moved) {
		return GL_FALSE;
	}
	return GL_TRUE;
}

GLvoid Level::GetNextPosition(GLshort &x, GLshort &y, GLshort direction)
{
	switch (direction) {
		case NORTH:
			y++;
			break;
		case SOUTH:
			y--;
			break;
		case WEST:
			x++;
			break;
		case OST:
			x--;
			break;
	}
}

GLshort Level::ThereIsExit(GLshort x, GLshort y)
{
	for (GLshort n = 0; n < MAX_EXIT; n++) {
		if (exits[n].door == NULL || !exits[n].exit_open) {
			continue;
		}
		if (x == exits[n].x && y == exits[n].y) {
			return n;
		}
	}
	return -1;
}

GLvoid Level::HeroIsMovingTo(GLshort x, GLshort y, GLshort from_x, GLshort from_y)
{
	GLshort exit = ThereIsExit(x, y);
	// exit is reached?
	if (exit != -1) {
		Controller::getInstance()->EndLevel(exits[exit].way_to);
		
#ifdef DEBUG_MODE
printf("Hero reached the exit %d (%s) \n", exit, exits[exit].way_to);
#endif	
	}
	GLshort index = GetTileIndex(x, y), from_index = GetTileIndex(from_x, from_y);
	// leaving tile
	if (cells[from_index].type == TT_FOS) {
		// crack the tile
		tiles[from_index]->SelectAnimation((char*)"dead");
		tiles[from_index]->PlayAnimation();
		cells[from_index].val = 0;
		if (Config::getInstance()->GetSound()) {
			RcRegistry::getInstance()->PlaySound(SOUND_STONE_BREAK);
		}

#ifdef DEBUG_MODE
printf("The tile at %d %d breaks \n", from_x, from_y);
#endif
	} else if (cells[from_index].type == TT_BUTTON) {
		// release the button
		tiles[from_index]->SetTimerValue(0);
		tiles[from_index]->SelectAnimation((char*)"release");
		tiles[from_index]->PlayAnimation();
		if (Config::getInstance()->GetSound()) {
			RcRegistry::getInstance()->PlaySound(SOUND_BUTTON_UP);
		}
		cells[from_index].flag = 0;
	}
	// step on tile
	GLshort key = ThereIsKey(x, y);
	if (key != -1) {
		GrabKey(key);
	} else if (cells[index].type == TT_BUTTON) {
		PushButton(index);
	}
}

GLshort Level::ThereIsKey(GLshort x, GLshort y)
{
	for (GLshort n = 0; n < MAX_EXIT; n++) {
		if (exits[n].door == NULL || exits[n].exit_open) {
			continue;
		}
		if (x == exits[n].key_x && y == exits[n].key_y) {
			return n;
		}
	}
	return -1;
}

GLvoid Level::GrabKey(GLshort index)
{
	key[index].SelectAnimation((char*)"death");
	key[index].ResetAnimation();
	exits[index].door->SelectAnimation((char*)"opened");
	exits[index].door->PlayAnimation();
	exits[index].got_key = GL_TRUE;
	if (Config::getInstance()->GetSound()) {
		RcRegistry::getInstance()->PlaySound(SOUND_DOOR);
	}
	
#ifdef DEBUG_MODE
printf("Hero picks up the key %d \n", index);
#endif
}

GLvoid Level::PushButton(GLshort index)
{
#ifdef DEBUG_MODE
GLshort tx, ty;
printf("Hero pushes the button %d \n", cells[index].val);
#endif

	tiles[index]->SelectAnimation((char*)"push");
	tiles[index]->SetTimerValue(0);
	tiles[index]->PlayAnimation();
	cells[index].flag = 1;
	// TODO Play sound;
	GLboolean roll_on = GL_FALSE;
	for (GLshort i = 0; i < MAPSIZE * MAPSIZE; i++) {
		if (cells[i].type == TT_ELEVATOR && cells[i].val == cells[index].val) {
			if (cells[i].flag == ES_STANDDOWN) {
				// lift the elevator
				cells[i].flag = ES_MOVEUP;
				tiles[i]->SetAnimaSpeed(1);
				tiles[i]->PlayAnimation();
				if (!roll_on) {
					roll_on = GL_TRUE;
					if (Config::getInstance()->GetSound()) {
						RcRegistry::getInstance()->PlaySound(SOUND_BUTTON_DOWN);
						RcRegistry::getInstance()->PlaySound(SOUND_ROLLING);
					}
				}
				
#ifdef DEBUG_MODE
GetCoordByIndex(i, tx, ty);
printf("Tile at %d %d is going up \n", tx, ty);
#endif
			} else if (cells[i].flag == ES_STANDUP && cells[i].moved == NO_MOVEDTILE) {
				// lower the elevator
				cells[i].flag = ES_MOVEDOWN;
				tiles[i]->SetAnimaSpeed(-1);
				tiles[i]->PlayAnimation();
				if (!roll_on){
					roll_on = GL_TRUE;
					if (Config::getInstance()->GetSound()) {
						RcRegistry::getInstance()->PlaySound(SOUND_BUTTON_DOWN);
						RcRegistry::getInstance()->PlaySound(SOUND_ROLLING);
					}
				}

#ifdef DEBUG_MODE
GetCoordByIndex(i, tx, ty);
printf("Tile at %d %d is going down \n", tx, ty);
#endif
			}
		}
	}
}

GLboolean Level::HeroCanRideOnTile(GLshort x, GLshort y, GLshort direction)
{
	if (cells[GetTileIndex(x, y)].moved == NO_MOVEDTILE) {
		return GL_FALSE;
	}
	GLshort next_x = x, next_y = y;
	GetNextPosition(next_x, next_y, direction);
	return CanRideTo(next_x, next_y);
}

GLboolean Level::CanRideTo(GLshort x, GLshort y)
{
	GLushort index;
	
	// hero should't leave game area
	if (x >= MAPSIZE || y >= MAPSIZE || x < 0 || y < 0) {
		return GL_FALSE;
	}
	
	index = GetTileIndex(x, y);
	// there is nothing
	if (cells[index].type == TT_NONE && cells[index].moved == NO_MOVEDTILE) {
		return GL_TRUE;
	}
	// there is elevator, hovever it is lifted
	if (cells[index].type == TT_ELEVATOR && cells[index].flag == ES_STANDUP) {
		return GL_TRUE;
	}
	return GL_FALSE;
}

GLushort Level::HeroRideOnTile(GLshort &x, GLshort &y, GLshort direction)
{
	GLshort next_x = x, next_y = y, len = 0;
	GLushort final_tile_index, current_tile_index = GetTileIndex(x, y);
	for (;;) {
		GetNextPosition(next_x, next_y, direction);
		if (! CanRideTo(next_x, next_y)) {
			break;
		}
		x = next_x;
		y = next_y;
		len++;
	}
	if (len > 0) {
		final_tile_index = GetTileIndex(x, y);
		cells[final_tile_index].moved = cells[current_tile_index].moved;
		cells[current_tile_index].moved = NO_MOVEDTILE;
		return cells[final_tile_index].moved;
	}
	return -1;
}

GLvoid Level::ShiftTile(GLushort index, GLfloat x, GLfloat y)
{
	tiles[index]->MoveAt(x, 0, y);
}

GLboolean Level::ThereIsATile(GLshort x, GLshort y)
{
	if (x >= MAPSIZE || y >= MAPSIZE) {
		return GL_TRUE;
	}
	GLushort index = GetTileIndex(x, y);
	if (cells[index].type == TT_NONE && cells[index].moved == NO_MOVEDTILE) {
		return GL_FALSE;
	}
	if (cells[index].type == TT_ELEVATOR && cells[index].flag == ES_STANDUP) {
		return GL_FALSE;
	}
	return GL_TRUE;
}

GLboolean Level::TryUseTeleport(GLshort &x, GLshort &y, GLshort direction)
{
	GLshort next_x = x, next_y = y;
	GLushort indexA, indexB;
	GetNextPosition(next_x, next_y, direction);
	indexA = GetTileIndex(next_x, next_y);
	
	// there is no portal!
	if (cells[indexA].type != TT_TELEPORT) {
		return GL_FALSE;
	}
	
	for (GLshort i = 0; i < MAPSIZE; i++) {
		for (GLshort j = 0; j < MAPSIZE; j++) {
			if (i == next_y && j == next_x) {
				continue;
			}
			indexB = GetTileIndex(j, i);
			if (cells[indexB].type == TT_TELEPORT && cells[indexA].val == cells[indexB].val) {
				if (HeroCanGo(j, i, direction)) {
					GetNextPosition(j, i, direction);
					x = j;
					y = i;
					if (Config::getInstance()->GetSound()) {
						RcRegistry::getInstance()->PlaySound(SOUND_TELEPORT);
					}
					return GL_TRUE;
				} else {
					return GL_FALSE;
				}
			}
		}
	}
	return GL_FALSE;
}

inline GLvoid Level::DrawDarkMask()
{
	glPushMatrix();
	glLoadIdentity();		
	glTranslatef(-spectator->GetX(), 0, -spectator->GetY());
	glRotatef(-spectator->GetRotation(), 0, 1, 0);
	glRotatef(-spectator->GetAngle(), 1, 0, 0);
	glTranslatef(0, 0, spectator->GetDistance() - 1);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0, 0, 0, darkness);
	glBegin(GL_QUADS);
		glVertex3f(-5, -5, 0);
		glVertex3f(5, -5, 0);
		glVertex3f(5, 5, 0);
		glVertex3f(-5, 5, 0);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();	
}

inline GLvoid Level::UpdateLighting()
{
	DrawDarkMask();
	
	if (light_state == STATE_FADE_IN) {
		darkness -= LIGHT_SWITCH_SPEED * Controller::getInstance()->GetTimePast();
		if (darkness <= 0.0f) {
			darkness = 0.0f;
			light_state = STATE_LIGHT_ON;
		}
	} else if (light_state == STATE_FADE_OUT) {
		darkness += LIGHT_SWITCH_SPEED * Controller::getInstance()->GetTimePast();
		if (darkness >= 1.0f) {
			darkness = 1.0f;
			light_state = STATE_LIGHT_OFF;
		}
	}
}

GLvoid Level::DestroyLevel()
{
	GLshort i;
	// destroy tiles
	for (i = 0; i < MAPSIZE * MAPSIZE; i++) {
		if (tiles[i] != NULL) {
			tiles[i]->Free();
			delete tiles[i];
			tiles[i] = NULL;
		}
	}
	for (i = 0; i < 4 * MAPSIZE; i++) {
		if (walls[i] != NULL) {
			walls[i]->Free();
			delete walls[i];
			walls[i] = NULL;
		}
	}
	for (i = 0; i < MAX_EXIT; i++) {
		if (exits[i].door != NULL) {
			exits[i].door->Free();
			delete exits[i].door;
			exits[i].door = NULL;
		}
	}
	if (furniture != NULL) {
		for (i = 0; i < furniture_count; i++) {
			furniture[i].Free();
		}
		delete[] furniture;
		furniture = NULL;
	}
}

GLshort Level::FindLevel(char * szName)
{
	for (GLshort i = 0; i < levels_count; i++) {
		if (!StrCmp(szName, levels[i].szName)) {
			return i;
		}
	}
	return -1;
}

LevelPreview * Level::GetLevelInfo(char * szName)
{
	GLshort i = FindLevel(szName);
	if (i == -1) {
		return NULL;
	}
	return &levels[i];
}

GLvoid Level::SetAsPassed(char * current, char * next)
{
	GLshort current_n = FindLevel(current), 
			next_n = FindLevel(next);
	if (next_n == -1) {
		printf("Level %s not found! \n", next);
	} else if (current_n == -1) {
		printf("Level %s not found! \n", current);
	} else {
		levels[next_n].locked = GL_FALSE;
		for (GLshort i = 0; i < levels[current_n].exit_count; i++) {
			if (!StrCmp(next, levels[current_n].way_to[i])) {
				levels[current_n].keys[i] = GL_TRUE;
				break;
			}
		}
	}
}

GLboolean Level::LoadProgress()
{
	ProgressRecord r;
	GLshort i, j;
	char szFileName[256];
	Config::getInstance()->GetDataDir(szFileName);
	StrCat(szFileName, (char*) PROGRESS_FILE);
	FILE * f = fopen(szFileName, "r");
	if (f == NULL) {
		fprintf(stderr, "Progress file %s was not loaded \n", szFileName);
		return GL_FALSE;
	}
	while (!feof(f)) {
		fread(&r, sizeof(ProgressRecord), 1, f);
		i = FindLevel(r.level);
		if (i != -1) {
			levels[i].locked = r.locked;
			for (j = 0; j < levels[i].exit_count; j++) {
				levels[i].keys[j] = r.keys[j];
			}
		}
	}
	fclose(f);
	return GL_TRUE;
}

GLboolean Level::SaveProgress()
{
	ProgressRecord r;
	GLshort i, j;
	char szFileName[256];
	Config::getInstance()->GetDataDir(szFileName);
	StrCat(szFileName, (char*) PROGRESS_FILE);
	FILE * f = fopen(szFileName, "w");
	if (f == NULL) {
		fprintf(stderr, "Error: can't create progress file %s \n", szFileName);
		return GL_FALSE;
	}
	for (i = 0; i < levels_count; i++) {
		StrCpy(r.level, levels[i].szName);
		r.locked = levels[i].locked;
		for (j = 0; j < MAX_EXIT; j++) {
			r.keys[j] = j < levels[i].exit_count ? levels[i].keys[j] : 0;
		}
		fwrite(&r, sizeof(ProgressRecord), 1, f);
	}
	fclose(f);
	return GL_TRUE;
}
