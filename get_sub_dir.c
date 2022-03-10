#ifdef _WIN32
#include<windows.h>
#else
#include<dirent.h>
#include<errno.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"get_sub_dir.h"

int csloc____cnt_sub_dirs(const char *dir)
{
	// make the wildcard search
	char search[300];
	strcpy(search, dir);
	int cnt=0;
#ifdef _WIN32
	size_t len=strlen(dir);
	strcpy(search + len, "\\*");
#endif
	// find data and start looking
#ifdef _WIN32
	WIN32_FIND_DATA wff;
	HANDLE ff = FindFirstFileA(search, &wff);
#else
	DIR *dr = opendir(search);
	if(dr == NULL)
	{
		if(errno == EACCES)
			fprintf(stderr, "Could not go into %s, permission denied, maybe try running as superuser.\n", search);
		goto fini;
	}
	struct dirent *de = readdir(dr);
#endif

	// keep looking while it can find more
	do
		++cnt
#ifdef _WIN32
		;
	while(FindNextFileA(ff, &wff));
#else
		, de = readdir(dr);
	while(de);
	closedir(dr);
#endif
	fini:
	return cnt;
}
void csloc____get_sub_dirs(const char *dir,char *names[],enum cfs____file_or_directory fd[])
{
	// make the wildcard search
	char search[3000];
	strcpy(search, dir);
#ifdef _WIN32
	size_t len=strlen(dir);
	strcpy(search + len, "\\*");
#endif

	// find data and start looking
#ifdef _WIN32
	WIN32_FIND_DATA wff;
	HANDLE ff = FindFirstFileA(search, &wff);
#else
	DIR *dr = opendir(search);
	csloc_check_pointer(dr);
	struct dirent *de = readdir(dr);
	csloc_check_pointer(de);
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
		csloc_check_pointer(names[cnt]);
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
#ifndef _WIN32
		de = readdir(dr);
#endif
	}
#ifdef _WIN32
	while(FindNextFileA(ff, &wff));
#else
	while(de);
	closedir(dr);
#endif
}
