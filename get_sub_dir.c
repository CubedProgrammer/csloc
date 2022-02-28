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

void csloc____sort_dir_entries(size_t sz, const char *fnames[], enum cfs____file_or_directory fd[])
{
	size_t lstk[100], rstk[100];
	char sstk[100];
	size_t ssz = 0;
	lstk[ssz] = 0;
	rstk[ssz] = sz - 1;
	sstk[ssz] = 97;
	++ssz;
	size_t l, r;
	size_t indx, indy, indz;
	const char **auxn = malloc(sz * sizeof(const char*));
	csloc_check_pointer(auxn);
	enum cfs____file_or_directory *auxt = malloc(sz * sizeof(enum cfs____file_or_directory));
	csloc_check_pointer(auxt);
	char status;
	while(ssz)
	{
		--ssz;
		l = lstk[ssz];
		r = rstk[ssz];
		status = sstk[ssz];
		if(l == r)
			continue;
		if(status == 97)
		{
			lstk[ssz] = l;
			rstk[ssz] = r;
			sstk[ssz] = 98;
			++ssz;
			lstk[ssz] = l;
			rstk[ssz] = (l + r) >> 1;
			sstk[ssz] = 97;
			lstk[ssz + 1] = rstk[ssz] + 1;
			rstk[ssz + 1] = r;
			sstk[ssz + 1] = 97;
			ssz += 2;
		}
		else
		{
			indx = l;
			indy = (l + r) / 2 + 1;
			indz = l;
			while(indx <= (l + r) / 2 && indy <= r)
			{
				if(csloc____case_insensitive_strcmp(fnames[indx], fnames[indy]) < 0)
				{
					auxn[indz] = fnames[indx];
					auxt[indz] = fd[indx];
					++indx;
				}
				else
				{
					auxn[indz] = fnames[indy];
					auxt[indz] = fd[indy];
					++indy;
				}
				++indz;
			}
			if(indy <= r)
			{
				for(;indy <= r;indy++,indz++)
				{
					auxn[indz] = fnames[indy];
					auxt[indz] = fd[indy];
				}
			}
			else
			{
				for(;indz <= r;indx++,indz++)
				{
					auxn[indz] = fnames[indx];
					auxt[indz] = fd[indx];
				}
			}
			memcpy(fnames + l, auxn + l, (r - l + 1) * sizeof(const char *));
			memcpy(fd + l, auxt + l, (r - l + 1) * sizeof(enum cfs____file_or_directory));
		}
	}
	free(auxt);
	free(auxn);
}

int csloc____case_insensitive_strcmp(const char *xstr, const char *ystr)
{
	size_t xlen = strlen(xstr), ylen = strlen(ystr);
	size_t len = xlen < ylen ? xlen : ylen;
	char u, v;
	int res = 0;
	for(size_t i = 0; i < len; ++i)
	{
		u = xstr[i], v = ystr[i];
		if(u >= 'A' && u <= 'Z')
			u+=0x20;
		if(v >= 'A' && v <= 'Z')
			v+=0x20;
		if(u == v)
			u = xstr[i], v = ystr[i];
		if(u < v)
			res = -1, len = 0;
		else if(u > v)
			res = 1, len = 0;
	}
	return res;
}
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
	csloc____sort_dir_entries(cnt, (const char **)names, fd);
#endif
}
