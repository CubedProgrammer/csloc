#ifdef _WIN32
#include<windows.h>
#else
#include<dirent.h>
#endif
#include<stdlib.h>
#include<string.h>
#include"get_sub_dir.h"
int cnt_sub_dirs(const char *dir)
{
	// make the wildcard search
	size_t len=strlen(dir);
	char search[300];
	strcpy(search, dir);
#ifdef _WIN32
	strcpy(search + len, "\\*");
#endif
	// find data and start looking
#ifdef _WIN32
	WIN32_FIND_DATA wff;
	HANDLE ff = FindFirstFileA(search, &wff);
#else
	DIR *dr = opendir(search);
	struct dirent *de;
#endif
	int cnt=0;

	// keep looking while it can find more
	do
		++cnt;
#ifdef _WIN32
	while(FindNextFileA(ff, &wff));
#else
	while(de = readdir(dr));
#endif
	return cnt;
}
void get_sub_dirs(const char *dir,char *names[],enum file_or_directory fd[])
{
	// make the wildcard search
	size_t len=strlen(dir);
	char search[len+3];
	strcpy(search, dir);
	strcpy(search + len, "\\*");

	// find data and start looking
#ifdef _WIN32
	WIN32_FIND_DATA wff;
	HANDLE ff = FindFirstFileA(search, &wff);
#else
	DIR *dr = opendir(search);
	struct dirent *de;
#endif
	size_t cnt=0, fnlen;

	// keep looking while it can find more
	do
	{
#ifdef _WIN32
		fnlen = strlen(wff.cFileName);
#else
		fnlen = strlen(de->d_name);
#endif
		names[cnt]=malloc(fnlen + sizeof(char));
#ifdef _WIN32
		strcpy(names[cnt], wff.cFileName);
#else
		strcpy(names[cnt], de->d_name);
#endif
#ifdef _WIN32
		if(wff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
#else
		if(de->d_type == DT_DIR)
#endif
			fd[cnt]=DIRECTORY;
		else
			fd[cnt]=NFILE;
		++cnt;
	}
#ifdef _WIN32
	while(FindNextFileA(ff, &wff));
#else
	while(de = readdir(dr));
#endif;
}
