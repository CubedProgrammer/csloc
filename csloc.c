#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"csloc.h"
#include"get_sub_dir.h"
#ifdef _WIN32
#include<windows.h>
#else
#include<sys/stat.h>
#endif
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
#ifdef _WIN32
long long
#else
long
#endif
cnt_single_file(const char *file, size_t cr)
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
	return cnt;
}
#ifdef _WIN32
long long
#else
long
#endif
csloc(const char *dir, size_t cr, int sif, int ihf, const char *const*fexts, size_t fel)
{
	// prepare to get the files and subdirectories
	char *subdir = malloc(100000);
	csloc_check_pointer(subdir);
	size_t len=strlen(dir), cnt=csloc____cnt_sub_dirs(dir);
	strcpy(subdir, dir);
#ifdef _WIN32
	subdir[len]='\\';
#else
	subdir[len]='/';
#endif
	char **names=malloc(sizeof(char*)*cnt);
	csloc_check_pointer(names);
	enum cfs____file_or_directory *tps = malloc(cnt * sizeof(enum cfs____file_or_directory));
	csloc_check_pointer(tps);

	// get the files and subdirectories
	csloc____get_sub_dirs(dir, names, tps);

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
	int sfl;
	int valid;

	// put all subdirectories in
	for(size_t i = 0; i < cnt; ++i)
	{
		strcpy(subdir + len + 1, names[i]);

		// get rid of hidden files if enabled
		if(ihf && names[i][0] == '.')
			continue;

		if(NFILE==tps[i])
		{
			valid = fel == 0 ? 1 : 0;
			for(size_t j = 0; j < fel; ++j)
			{
				if(has_file_extension(names[i],fexts[j]))
					valid++;
			}
			if(valid)
			{
				sfl = cnt_single_file(subdir, cr);
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
			csloc_check_pointer(stack);
			fcnt=olr;
			olr=rm;
			rm+=fcnt;
			fcnt=olr;
		}

		stack[fcnt]=malloc(len + strlen(names[i]) + 2);
		csloc_check_pointer(stack[fcnt]);
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
		len=strlen(currf), cnt=csloc____cnt_sub_dirs(currf);
		strcpy(subdir, currf);
#ifdef _WIN32
		subdir[len]='\\';
#else
		subdir[len]='/';
#endif

		// get subdirectories and files
		names=malloc(sizeof(char*)*cnt);
		csloc_check_pointer(names);
		tps = malloc(cnt * sizeof(enum cfs____file_or_directory));
		csloc_check_pointer(tps);
		csloc____get_sub_dirs(currf, names, tps);

		// put all subdirectories in
		for(size_t i = 0; i < cnt; ++i)
		{
			strcpy(subdir + len + 1, names[i]);

			// get rid of hidden files if enabled
			if(ihf && names[i][0] == '.')
				continue;

			if(NFILE==tps[i])
			{
				valid = fel == 0 ? 1 : 0;
				for(size_t j = 0; j < fel; ++j)
				{
					if(has_file_extension(names[i],fexts[j]))
						valid++;
				}
				if(valid)
				{
					sfl = cnt_single_file(subdir, cr);
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
				csloc_check_pointer(stack);
				fcnt=olr;
				olr=rm;
				rm+=fcnt;
				fcnt=olr;
			}

			stack[fcnt]=malloc(len + strlen(names[i]) + 2);
			csloc_check_pointer(stack[fcnt]);
			strcpy(stack[fcnt], subdir);
			fcnt++;
			free(names[i]);
		}

		free(names);
		free(tps);
		free(currf);
	}
	free(stack);
	free(subdir);
	return sloc;
}
int main(int argl,char*argv[])
{
	if(argl==1)
	{
		puts("Specify a directory.\nCommand line options...\n");
		puts("-s to show the sloc of individual files.");
		puts("-h to not count files beginning with a ., such files are considered hidden on linux.");
		puts("-cNUM specifies that NUM non-whitespace characters are required to count as a valid line.");
		puts("-ext to specify file extensions to count, this option must come last, as all other args after it are considered to be in the list of file extensions.");
	}
	else
	{
		setvbuf(stderr, NULL, _IONBF, 0);
		// show individual files
		int sif = 0;
		// specified file extension
		const char **fexts = NULL;
		// number of file extensions to check
		size_t fel = 0;
		// ignore hidden files
		int ihf = 0;
		int quiet = 0;
		char *dir = NULL;
		int ext = 0;
		size_t cr = 1;
		char *cp;
		for(int i = 1; i < argl; ++i)
		{
			if(ext)
			{
				fexts[i - argl + fel] = argv[i];
				continue;
			}

			if(argv[i][0] == '-')
			{
				if(strcmp(argv[i], "-ext") == 0)
				{
					ext = 1, fel = argl - i - 1;
					fexts = malloc(sizeof(const char*) * fel);
					csloc_check_pointer(fexts);
				}
				else
				{
					if(strchr(argv[i], 's') != NULL)
						sif = 1;
					if(strchr(argv[i], 'h') != NULL)
						ihf = 1;
					if(strchr(argv[i], 'q') != NULL)
						quiet = 1;
					cp = strchr(argv[i], 'c');
					if(cp != NULL)
						cr = atoi(cp + 1);
				}
			}
			else
				dir = argv[i];
		}
		if(dir == NULL)
			puts("Specify a directory");
		else
		{
#ifdef _WIN32
			int isdir = GetFileAttributesA(dir) & FILE_ATTRIBUTE_DIRECTORY;
#else
			struct stat fstat;
			stat(dir, &fstat);
			int isdir = S_ISDIR(fstat.st_mode);
#endif
			int dl = strlen(dir);
#ifdef _WIN32
			if(dir[dl - 1] == '\\')
#else
			if(dl > 1 && dir[dl - 1] == '/')
#endif
				dir[dl - 1] = '\0';
			if(quiet)
				printf("%ld\n", isdir ? csloc(dir, cr, sif, ihf, fexts, fel) : cnt_single_file(dir, cr));
			else
			{
				if(isdir)
					printf("All files in %s combined have %ld source lines of code.\n",dir,csloc(dir, cr, sif, ihf, fexts, fel));
				else
					printf("The file %s has %ld source lines of code.\n",dir,cnt_single_file(dir, cr));
			}
		}
		if(fexts)
			free(fexts);
	}
	return 0;
}
