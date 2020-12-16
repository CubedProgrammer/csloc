#include<windows.h>
#include<stdlib.h>
#include<string.h>
#include"get_sub_dir.h"
int cnt_sub_dirs(const char *dir)
{
	// make the wildcard search
	size_t len=strlen(dir);
	char search[len+3];
	strcpy(search, dir);
	strcpy(search + len, "\\*");

	// find data and start looking
	WIN32_FIND_DATA wff;
	HANDLE ff = FindFirstFileA(search, &wff);
	int cnt=0;

	// keep looking while it can find more
	do
		++cnt;
	while(FindNextFileA(ff, &wff));
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
	WIN32_FIND_DATA wff;
	HANDLE ff = FindFirstFileA(search, &wff);
	size_t cnt=0;

	// keep looking while it can find more
	do
	{
		names[cnt]=malloc(strlen(wff.cFileName) + sizeof(char));
		strcpy(names[cnt], wff.cFileName);
		if(wff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			fd[cnt]=DIRECTORY;
		else
			fd[cnt]=NFILE;
		++cnt;
	}
	while(FindNextFileA(ff, &wff));
}
