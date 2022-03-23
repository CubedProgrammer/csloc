#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifdef _WIN32
#include<windows.h>
#else
#include<sys/stat.h>
#endif
#include"csloc.h"
int main(int argl,char*argv[])
{
	if(argl==1)
	{
		puts("Specify a directory.\nCommand line options...\n");
		puts("-r to sort least to greatest.");
		puts("-f to count file size instead.");
		puts("-t to sort the files by number of lines.");
		puts("-s to show the sloc of individual files.");
		puts("-h to not count files beginning with a ., such files are considered hidden on linux.");
		puts("-cNUM specifies that NUM non-whitespace characters are required to count as a valid line.");
		puts("-q to not output complete sentences.");
		puts("-ext or -x to specify file extensions to count, without the . in the front.");
		puts("This option must come last, as all other args after it are considered to be in the list of file extensions.");
		puts("Add ! in front of an extension to forbid counting all files with that extension.");
	}
	else
	{
		setvbuf(stderr, NULL, _IONBF, 0);
		setvbuf(stdout, NULL, _IONBF, 0);
		unsigned options = 0;
		// specified file extension
		const char **fexts = NULL;
		// number of file extensions to check
		size_t fel = 0;
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
					extlb:
					ext = 1, fel = argl - i - 1;
					fexts = malloc(sizeof(const char*) * fel);
				}
				else
				{
					if(strchr(argv[i], 'r') != NULL)
						options |= CSLOC_RSORT;
					if(strchr(argv[i], 'f') != NULL)
						options |= CSLOC_FSIZE;
					if(strchr(argv[i], 't') != NULL)
						options |= CSLOC_SORT;
					if(strchr(argv[i], 's') != NULL)
						options |= CSLOC_SIF;
					if(strchr(argv[i], 'h') != NULL)
						options |= CSLOC_IGNDOT;
					if(strchr(argv[i], 'q') != NULL)
						options |= CSLOC_QUIET;
					if(strchr(argv[i], 'x') != NULL)
						goto extlb;
					cp = strchr(argv[i], 'c');
					if(cp != NULL)
						cr = atoi(cp + 1);
				}
			}
			else
				dir = argv[i];
		}
		if(dir == NULL)
			puts("Specify a directory, run with no arguments for help message.");
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
			size_t total;
			if(isdir)
			{
				csloc_filenp dat;
				size_t datsz;
				total = csloc(dir, &dat, &datsz, options, cr, fexts, fel);
				if(CSLOC_ISSIF(options))
				{
					for(size_t i = 0; i < datsz; ++i)
					{
						if(CSLOC_ISQUIET(options))
							printf("%s %zu\n", dat[i].name, dat[i].val);
						else
							printf("File %s has %zu source lines of code.\n", dat[i].name, dat[i].val);
						free(dat[i].name);
					}
					free(dat);
				}
				if(CSLOC_ISQUIET(options))
					printf("%zu\n", total);
				else
					printf("All files in %s combined have %zu source lines of code.\n",dir,total);
			}
			else
			{
				total = cnt_single_file(dir, cr);
				if(CSLOC_ISQUIET(options))
					printf("%zu\n", total);
				else
					printf("The file %s has %zu source lines of code.\n",dir,total);
			}
		}
		if(fexts)
			free(fexts);
	}
	return 0;
}
