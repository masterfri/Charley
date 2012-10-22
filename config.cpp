#include "config.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

Config * Config::instance = NULL;

Config::Config()
{
	SetFullscreen(0);
	SetCamera(CP_FREE);
	SetDrawWalls(1);
	SetSound(1);
	SetMusic(1);
	SetCurrentLevel((char*)"start");
}

GLboolean Config::Load()
{
	DataScript cfg;
	char szFileName[256], buf[32];
	GLint intval;
	GetDataDir(szFileName);
	StrCat(szFileName, (char*) CONFIG_FILE);
	if (cfg.Read(szFileName) != 0) {
		fprintf(stderr, "Configuration file %s was not loaded \n", szFileName);
		return GL_FALSE;
	}
	if (cfg.ExtractData((char*)"fullscreen", &intval, 0)) {
		SetFullscreen(intval);
	}
	if (cfg.ExtractData((char*)"camera", &intval, 0)) {
		SetCamera(intval);
	}
	if (cfg.ExtractData((char*)"draw_walls", &intval, 0)) {
		SetDrawWalls(intval);
	}
	if (cfg.ExtractData((char*)"sound", &intval, 0)) {
		SetSound(intval);
	}
	if (cfg.ExtractData((char*)"music", &intval, 0)) {
		SetMusic(intval);
	}
	if (cfg.ExtractData((char*)"current_level", buf, 0)) {
		StrCpy(GetCurrentLevel(), buf);
	}
	return GL_TRUE;
}

GLboolean Config::Save()
{
	char szFileName[256], line[100];
	GetDataDir(szFileName);
	StrCat(szFileName, (char*) CONFIG_FILE);
	FILE * f = fopen(szFileName, "w");
	if (f == NULL) {
		fprintf(stderr, "Error: can't create configuration file %s \n", szFileName);
		return GL_FALSE;
	}
	fputs("datascript {\n", f);
	sprintf(line, "\tfullscreen=%d;\n", GetFullscreen());
	fputs(line, f);
	sprintf(line, "\tcamera=%d;\n", GetCamera());
	fputs(line, f);
	sprintf(line, "\tdraw_walls=%d;\n", GetDrawWalls());
	fputs(line, f);
	sprintf(line, "\tsound=%d;\n", GetSound());
	fputs(line, f);
	sprintf(line, "\tmusic=%d;\n", GetMusic());
	fputs(line, f);
	sprintf(line, "\tcurrent_level=\"%s\";\n", GetCurrentLevel());
	fputs(line, f);
	fputs("}", f);
	fclose(f);
	return GL_TRUE;
}

GLvoid Config::GetDataDir(char * buffer)
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	sprintf(buffer, "%s/%s", homedir, DATA_DIR);
	char cmd[256];
	sprintf(cmd, "[ -d %s ] || mkdir %s", buffer, buffer);
	system(cmd);
	StrCat(buffer, (char*) "/");
}
