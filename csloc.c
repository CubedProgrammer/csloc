// This file is part of csloc.
// Copyright (C) 2020-2023, github.com/CubedProgrammer, owner of said account.

// csloc is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// csloc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with csloc. If not, see <https://www.gnu.org/licenses/>.

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"csloc.h"
#include"get_sub_dir.h"
#ifdef _WIN32
#include<windows.h>
#else
#include<errno.h>
#include<sys/stat.h>
#endif
const char *csloc_get_ext(const char *name)
{
	const char *ext = NULL;
	for(const char *it = name; *it != '\0'; ++it)
	{
		if(*it == '.')
			ext = it + 1;
		else if(*it == '/')
			ext = NULL;
	}
	if(*ext == '\0')
		ext = NULL;
	return ext;
}
static inline int has_file_extension(const char *fname, const char *ext)
{
	if(ext == NULL)
		return 1;
	else
	{
		size_t flen = strlen(fname), elen = strlen(ext);
		int status = 0;
		if(flen > elen)
		{
			if(fname[flen - elen - 1] == '.' && strcmp(ext, fname + flen - elen) == 0)
				status = 1;
		}
		return status;
	}
}
int csloc____ispref(const char *pref, const char *str)
{
	int p = 1;
	for(; *pref != '\0'; ++pref, ++str)
	{
		if(*str == '\0')
		{
			p = 0;
			pref = str - 1;
		}
		else if(*pref != *str)
			p = 0;

	}
	return p;
}
#ifdef _WIN32
long long
#else
long
#endif
cnt_single_file(const char *file, size_t cr)
{
	FILE *f = fopen(file, "r");
	long cnt = 0;

	if(f == NULL)
	{
		if(errno == EACCES)
			fprintf(stderr, "Could not go into %s, permission denied, maybe try running as superuser.\n", file);
		goto fini;
	}

	int curr;
	int ne = 0;
	cr += cr == 0;

	// read the file
	while(feof(f) == 0)
	{
		curr = fgetc(f);
		if(curr == '\n')
		{
			if(ne >= cr)
				cnt++;
			ne = 0;
		}
		else if(curr != -1 && curr != ' ' && curr != '\t' && curr != '\r')
			++ne;
	}

	// if there is not a new line character at the end
	if(ne >= cr)
		++cnt;
	fclose(f);
	fini:
	return cnt;
}
void csloc_sort_filen(int val, csloc_filenp arr, size_t sz)
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
	csloc_filenp auxn = malloc(sz * sizeof(*arr));
	char status;
	int cmp;
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
				if(val)
				{
					cmp = arr[indx].val > arr[indy].val ? -1 : arr[indx].val < arr[indy].val ? 1 : 0;
					if(cmp == 0)
						cmp = csloc____case_insensitive_strcmp(arr[indx].name, arr[indy].name);
				}
				else
					cmp = csloc____case_insensitive_strcmp(arr[indx].name, arr[indy].name);
				if(cmp < 0)
				{
					auxn[indz] = arr[indx];
					++indx;
				}
				else
				{
					auxn[indz] = arr[indy];
					++indy;
				}
				++indz;
			}
			if(indy <= r)
			{
				for(;indy <= r;indy++,indz++)
					auxn[indz] = arr[indy];
			}
			else
			{
				for(;indz <= r;indx++,indz++)
					auxn[indz] = arr[indx];
			}
			memcpy(arr + l, auxn + l, (r - l + 1) * sizeof(*arr));
		}
	}
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
#ifdef _WIN32
long long
#else
long
#endif
csloc(const char *dir, csloc_filenp *dat, size_t *sz, unsigned ops, size_t cr, const char *const*fexts, size_t fel)
{
	// prepare to get the files and subdirectories
	char subdir[3000], lnpath[3000];
	char *apath=NULL;
	size_t len=strlen(dir), cnt=0;//csloc____cnt_sub_dirs(dir);
	strcpy(subdir, dir);
#ifdef _WIN32
	subdir[len]='\\';
#else
	subdir[len]='/';
#endif
	char **names;
	enum cfs____file_or_directory *tps;

	// set up stack for simulating recursion
	size_t fcnt=0, rm=5, olr=3;
	char **stack=malloc(sizeof(char*)*rm);
	csloc_check_pointer(stack);

	// source lines of code and single file lines
#ifndef _WIN32
	long sloc = 0;
#else
	long long sloc = 0;
#endif
	long sfl;
	int valid;

	// store data if requested
	size_t datsz, datc;
	csloc_filenp d = NULL;
	if(CSLOC_ISSIF(ops))
	{
		datsz = 0;
		datc = 96;
		d = malloc(datc * sizeof(*d));
	}

	// adds the starting directory to the stack
	char *maindircpy = malloc(strlen(dir) + 1);
	strcpy(maindircpy, dir);
	stack[fcnt] = maindircpy;
	++fcnt;
#ifdef _WIN32
	WIN32_FIND_DATA fdat;
#else
	struct stat fdat;
#endif
	// simulate recursion
	char *currf;
	while(fcnt)
	{
		// get last file
		currf = stack[--fcnt];
		cnt=csloc____cnt_sub_dirs(currf);
		if(cnt <= 2)
		{
			free(currf);
			continue;
		}

		len=strlen(currf);
		// get subdirectories and files
		names=malloc(sizeof(char*)*cnt);
		csloc_check_pointer(names);
		tps = malloc(cnt * sizeof(enum cfs____file_or_directory));
		csloc_check_pointer(tps);
		csloc____get_sub_dirs(currf, names, tps);

		// put all subdirectories in
		for(size_t i = 0; i < cnt; ++i)
		{
			strcpy(subdir, currf);
#ifdef _WIN32
			subdir[len]='\\';
#else
			subdir[len]='/';
#endif
			strcpy(subdir + len + 1, names[i]);

			// get rid of hidden files if enabled
			redirect:
			if(CSLOC_ISIGNDOT(ops) && names[i][0] == '.')
			{
				free(names[i]);
				continue;
			}

			if(CSLOCSYMLINK==tps[i])
#ifdef _WIN32
			{
				tps[i] = NFILE;
				goto redirect;
			}
#else
			{
				if(!CSLOC_ISNOLNK(ops))
				{
					realpath(subdir, lnpath);
					if(apath!=NULL)
						free(apath);
					apath=realpath(dir,NULL);
					if(!csloc____ispref(apath, lnpath))
					{
						strcpy(subdir, lnpath);
						realpath(currf, lnpath);
						if(!csloc____ispref(lnpath, subdir))
						{
							if(stat(subdir, &fdat)==0)
							{
								if(S_ISDIR(fdat.st_mode))
									tps[i]=DIRECTORY;
								else if(S_ISREG(fdat.st_mode))
									tps[i]=NFILE;
								else
									tps[i]=CSLOCOTHER;
								goto redirect;
							}
							else
								fprintf(stderr, "Could not stat %s.\n", subdir);
						}
					}
				}
				else if(CSLOC_ISFSIZE(ops))
				{
					tps[i] = NFILE;
					goto redirect;
				}
			}
#endif
			else if(NFILE==tps[i])
			{
				valid = fel == 0 ? 1 : 0;
				for(size_t j = 0; j < fel; ++j)
				{
					if(fexts[j][0] == '!')
					{
						if(!has_file_extension(names[i],fexts[j]+1))
							valid++;
						else
						{
							valid = 0;
							j = 999999;
						}
					}
					else if(has_file_extension(names[i],fexts[j]))
						valid++;
				}
				if(valid)
				{
					if(CSLOC_ISFSIZE(ops))
					{
#ifdef _WIN32
						FindFirstFileA(subdir, &fdat);
						ULONGLONG sz = fdat.nFileSizeHigh;
						sz = (sz << 32) + fdat.nFileSizeLow;
						sfl = sz;
#else
						if(lstat(subdir, &fdat) == 0)
							sfl = fdat.st_size;
						else
							sfl = 0;
#endif
					}
					else
						sfl = cnt_single_file(subdir, cr);
					if(CSLOC_ISSIF(ops))
					{
						if(datsz == datc)
						{
							datc += datc >> 1;
							d = realloc(d, sizeof(*d) * datc);
						}
						d[datsz].val = sfl;
						d[datsz].name = malloc(strlen(subdir) + 1);
						strcpy(d[datsz].name, subdir);
						++datsz;
					}
					sloc += sfl;
				}
			}
			else if(DIRECTORY==tps[i])
			{
				// get rid of parent and self
				if(strcmp(".", names[i]) != 0 && strcmp("..", names[i]) != 0)
				{
					if(fcnt==rm)
					{
						stack=realloc(stack, (rm+olr)*sizeof(char*));
						csloc_check_pointer(stack);
						fcnt=olr;
						olr=rm;
						rm+=fcnt;
						fcnt=olr;
					}

					stack[fcnt]=malloc(strlen(subdir) + 2);
					csloc_check_pointer(stack[fcnt]);
					strcpy(stack[fcnt], subdir);
					fcnt++;
					{
					}
				}
			}
			free(names[i]);
		}

		free(names);
		free(tps);
		free(currf);
	}
	free(stack);
	if(apath)
		free(apath);
	if(CSLOC_ISSIF(ops))
	{
		if(datsz)
			csloc_sort_filen(CSLOC_ISSORT(ops), d, datsz);
		if(CSLOC_ISRSORT(ops))
		{
			csloc_filen tmp;
			for(size_t i = 0; i < datsz >> 1; ++i)
			{
				tmp = d[i];
				d[i] = d[datsz - i - 1];
				d[datsz - i - 1] = tmp;
			}
		}
		*dat = d;
		*sz = datsz;
	}
	return sloc;
}
