#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"get_sub_dir.h"
// show individual files
int sif;
// specified file extension
const char *fext = NULL;
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
#ifndef DEBUG
static inline
#endif
int cnt_single_file(const char *file)
{
	FILE *f = fopen(file, "r");
	int cnt = 0;
	char curr;
	int ne = 0;

	// read the file
	while(feof(f) == 0)
	{
		curr = fgetc(f);
		if(curr == '\n')
		{
			if(ne)
				cnt++;
			ne = 0;
		}
		else if(curr != -1 && curr != ' ' && curr != '\t')
			ne = 1;
	}

	// if there is not a new line character at the end
	if(ne)
		++cnt;
	fclose(f);
	return cnt;
}
int csloc(const char *dir)
{
	// prepare to get the files and subdirectories
	char subdir[1000];
	size_t len=strlen(dir), cnt=cnt_sub_dirs(dir);
	strcpy(subdir, dir);
#ifdef _WIN32
	subdir[len]='\\';
#else
	subdir[len]='/';
#endif
	char **names=malloc(sizeof(char*)*cnt);
	enum file_or_directory *tps = malloc(cnt * sizeof(enum file_or_directory));

	// get the files and subdirectories
	get_sub_dirs(dir, names, tps);

	// set up stack for simulating recursion
	size_t fcnt=0, rm=5, olr=3;
	char **stack=malloc(sizeof(char*)*rm);

	// source lines of code and single file lines
	int sloc = 0;
	int sfl;

	// put all subdirectories in
	for(size_t i = 0; i < cnt; ++i)
	{
		strcpy(subdir + len + 1, names[i]);
		if(NFILE==tps[i])
		{
			if(has_file_extension(names[i],fext))
			{
				sfl = cnt_single_file(subdir);
				if(sif)
					printf("File %s has %d source lines of code.\n", names[i], sfl);
				sloc += sfl;
			}
			continue;
		}

		// get rid of parent and self
		if(strcmp(".", names[i]) == 0 || strcmp("..", names[i]) == 0)
			continue;

		if(fcnt==rm)
		{
			stack=realloc(stack, (rm+olr)*sizeof(char*));
			fcnt=olr;
			olr=rm;
			rm+=fcnt;
			fcnt=olr;
		}

		stack[fcnt]=malloc(len + strlen(names[i]) + 2);
		strcpy(stack[fcnt], subdir);
		fcnt++;
		free(names[i]);
	}

	free(names);
	free(tps);

	// simulate recursion
	char *currf;
	while(fcnt)
	{
		// get last file
		currf = stack[--fcnt];
		len=strlen(currf), cnt=cnt_sub_dirs(currf);
		strcpy(subdir, currf);
#ifdef _WIN32
		subdir[len]='\\';
#else
		subdir[len]='/';
#endif

		// get subdirectories and files
		names=malloc(sizeof(char*)*cnt);
		tps = malloc(cnt * sizeof(enum file_or_directory));
		get_sub_dirs(currf, names, tps);

		// put all subdirectories in
		for(size_t i = 0; i < cnt; ++i)
		{
			strcpy(subdir + len + 1, names[i]);
			if(NFILE==tps[i])
			{
				if(has_file_extension(names[i],fext))
				{
					sfl = cnt_single_file(subdir);
					if(sif)
						printf("File %s has %d source lines of code.\n", names[i], sfl);
					sloc += sfl;
				}
				continue;
			}

			// get rid of parent and self
			if(strcmp(".", names[i]) == 0 || strcmp("..", names[i]) == 0)
				continue;

			if(fcnt==rm)
			{
				stack=realloc(stack, (rm+olr)*sizeof(char*));
				fcnt=olr;
				olr=rm;
				rm+=fcnt;
				fcnt=olr;
			}

			stack[fcnt]=malloc(len + strlen(names[i]) + 2);
			strcpy(stack[fcnt], subdir);
			fcnt++;
			free(names[i]);
		}

		free(names);
		free(tps);
		free(currf);
	}
	return sloc;
}
int main(int argl,char*argv[])
{
	if(argl==1)
		puts("Specify a directory");
	else
	{
		const char *dir = NULL;
		int ext = 0;
		for(int i = 1; i < argl; ++i)
		{
			if(ext)
			{
				fext = argv[i];
				ext = 0;
				continue;
			}

			if(strcmp(argv[i], "-s") == 0)
				sif = 1;
			else if(strcmp(argv[i], "-ext") == 0)
				ext = 1;
			else
				dir = argv[i];
		}
		if(dir == NULL)
			puts("Specify a directory dummy.");
		else
			printf("All files in %s combined have %d source lines of code.\n",dir,csloc(dir));
	}
	return 0;
}
