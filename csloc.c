#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"get_sub_dir.h"
#ifndef DEBUG
inline
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
		else if(curr != ' ' && curr != '\t')
			ne = 1;
	}

	// if there is not a new line character at the end
	if(ne)
		++cnt;
	return cnt;
}
int csloc(const char *dir)
{
	// prepare to get the files and subdirectories
	char subdir[1000];
	size_t len=strlen(dir), cnt=cnt_sub_dirs(dir);
	strcpy(subdir, dir);
	subdir[len]='\\';
	char **names=malloc(sizeof(char*)*cnt);
	enum file_or_directory *tps = malloc(cnt * sizeof(enum file_or_directory));

	// get the files and subdirectories
	get_sub_dirs(dir, names, tps);

	// set up stack for simulating recursion
	size_t fcnt=0, rm=5, olr=3;
	char **stack=malloc(sizeof(char*)*rm);

	// source lines of code
	int sloc = 0;

	// put all subdirectories in
	for(size_t i = 0; i < cnt; ++i)
	{
		strcpy(subdir + len + 1, names[i]);
		if(NFILE==tps[i])
		{
			sloc += cnt_single_file(subdir);
			printf("sloc %d\n", sloc);
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
		fflush(stdout);
		// get last file
		currf = stack[--fcnt];
		len=strlen(currf), cnt=cnt_sub_dirs(currf);
		strcpy(subdir, currf);
		subdir[len]='\\';

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
				sloc += cnt_single_file(subdir);
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
		printf("All files in %s combined have %d source lines of code.\n",argv[1],csloc(argv[1]));
	return 0;
}
