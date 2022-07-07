// This file is part of csloc.
// Copyright (C) 2020-2022, github.com/CubedProgrammer, owner of said account.

// csloc is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// csloc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with csloc. If not, see <https://www.gnu.org/licenses/>.

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
		else if
#ifdef _WIN32
		(wff.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
#else
		(de->d_type == DT_REG)
#endif
			fd[cnt]=NFILE;
		else if
#ifdef _WIN32
		(wff.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
#else
		(de->d_type == DT_LNK)
#endif
			fd[cnt]=CSLOCSYMLINK;
		else
			fd[cnt]=CSLOCOTHER;
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
