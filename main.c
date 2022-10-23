// This file is part of csloc.
// Copyright (C) 2020-2022, github.com/CubedProgrammer, owner of said account.

// csloc is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// csloc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with csloc. If not, see <https://www.gnu.org/licenses/>.

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifdef _WIN32
#include<windows.h>
#else
#include<sys/stat.h>
#endif
#include"csloc.h"
#define VERSION_MINOR "9"
#define VERSION_PATCH "2"
int main(int argl,char*argv[])
{
	if(argl==1)
	{
		help:
		printf("csloc version 1.%s.%s\n",VERSION_MINOR,VERSION_PATCH);
		printf("Usage: %s [OPTIONS...] FILES... [-x] [EXTENSIONS...]\nCommand line options...\n\n", argv[0]);
		puts("If a file starts with '-', escape it with a \\, otherwise the first \\ of an argument is ignored.");
		puts("-y to show subtotals for each file type");
		puts("-l to ignore symbolic links.");
		puts("-o to write output to a file instead of stdout, the next argument MUST be that file.");
		puts("-e to alternate colours in -s mode, making output easier to read.");
		puts("-n to list the number before the path in -qs mode.");
		puts("-r to sort least to greatest.");
		puts("-f to count file size instead.");
		puts("-t to sort the files by number of lines or size.");
		puts("-s to show the sloc of individual files.");
		puts("The options -r and -t automatically enable -s, -rs and -st are now redundant.");
		puts("-h to not count files beginning with a ., such files are considered hidden on linux.");
		puts("-cNUM specifies that NUM non-whitespace characters are required to count as a valid line.");
		puts("-q to not output complete sentences.");
		puts("-x to specify file extensions to count, without the . in the front.");
		puts("This option must come last, as all other args after it are considered to be in the list of file extensions.");
		puts("Add ! in front of an extension to forbid counting all files with that extension.");
		puts("Example: 'csloc -r . -x h c' will count all files in current directory that end in .h or .c and list them in order of least to greatest.");
	}
	else
	{
		if(strcmp(argv[1], "--help") == 0)
			goto help;
		setvbuf(stderr, NULL, _IONBF, 0);
		setvbuf(stdout, NULL, _IONBF, 0);
		FILE *ofh = stdout;
		unsigned options = 0;
		// specified file extension
		const char **fexts = NULL, *ofn = NULL;
		// number of file extensions to check
		size_t fel = 0;
		char *dir = NULL;
		int ext = 0;
		size_t cr = 1;
		const char *cp, *fileext;
		char currop, *numend;
		char numfirst = 0, colours = 0;
		char ofile = 0, truesif = 0;
		int col = 0, dircnt = 0, extbegin = argl;
		size_t *exttots = NULL;
		size_t total;
		csloc_filenp dat;
		size_t datsz;
		int isdir, dl;
#ifndef _WIN32
		struct stat fstat;
#endif
		for(int i = 1; i < argl; ++i)
		{
			if(ext)
				fexts[i - argl + fel] = argv[i];
			else if(argv[i][0] == '-')
			{
				if(strchr(argv[i], 'x') != NULL)
				{
					ext = 1, fel = argl - i - 1;
					extbegin = i;
					fexts = malloc(sizeof(const char*) * fel);
				}
			}
		}
		for(int i = 1; i < extbegin; ++i)
		{
			if(ofile)
			{
				ofile = 0;
				ofn = argv[i];
				ofh = fopen(ofn, "w");
				if(ofh == NULL)
				{
					ofh = stdout;
					fprintf(stderr, "File %s could not be opened for writing, check permissions.\n", ofn);
				}
			}
			else if(argv[i][0] == '-')
			{
				for(const char *it = argv[i] + 1; *it != '\0'; ++it)
				{
					currop = *it;
					switch(currop)
					{
						case'y':
							options |= CSLOC_SIF;
							if(fel != 0)
								exttots = malloc(fel * sizeof(*exttots));
							break;
						case'l':
							options |= CSLOC_NOLNK;
							break;
						case'o':
							ofile = 1;
							break;
						case'e':
							colours = 1;
							break;
						case'n':
							numfirst = 1;
							break;
						case'r':
							options |= CSLOC_RSORT;
							truesif = 1;
							break;
						case'f':
							options |= CSLOC_FSIZE;
							break;
						case't':
							options |= CSLOC_SORT;
							truesif = 1;
							break;
						case's':
							options |= CSLOC_SIF;
							truesif = 1;
							break;
						case'h':
							options |= CSLOC_IGNDOT;
							break;
						case'q':
							options |= CSLOC_QUIET;
							break;
						case'c':
							cp = it + 1;
							cr = strtoul(cp, &numend, 10);
							it = numend - 1;
							break;
						default:
							fprintf(stderr, "Unrecognized option -%c, it will be ignored.\n", currop);
					}
				}
			}
			else
			{
				dir = argv[i];
				if(*dir == '\\')
					++dir;
				++dircnt;
#ifdef _WIN32
				isdir = GetFileAttributesA(dir) & FILE_ATTRIBUTE_DIRECTORY;
#else
				stat(dir, &fstat);
				isdir = S_ISDIR(fstat.st_mode);
#endif
				dl = strlen(dir);
#ifdef _WIN32
				if(dir[dl - 1] == '\\')
#else
				if(dl > 1 && dir[dl - 1] == '/')
#endif
					dir[dl - 1] = '\0';
				if(isdir)
				{
					total = csloc(dir, &dat, &datsz, options, cr, fexts, fel);
					if(CSLOC_ISSIF(options))
					{
						if(exttots)
							memset(exttots, 0, fel * sizeof(*exttots));
						for(size_t i = 0; i < datsz; ++i)
						{
							if(exttots)
							{
								fileext = csloc_get_ext(dat[i].name);
								for(size_t j = 0; j < fel; ++j)
								{
									if(strcmp(fileext, fexts[j]) == 0)
										exttots[j] += dat[i].val;
								}
							}
							if(!truesif)
								goto freedat;
							if(colours)
								fprintf(ofh, "\033\133%im", col);
							if(CSLOC_ISQUIET(options))
							{
								if(numfirst)
									fprintf(ofh, "%zu %s\n", dat[i].val, dat[i].name);
								else
									fprintf(ofh, "%s %zu\n", dat[i].name, dat[i].val);
							}
							else
							{
								if(CSLOC_ISFSIZE(options))
									fprintf(ofh, "File %s is %zu bytes.\n", dat[i].name, dat[i].val);
								else
									fprintf(ofh, "File %s has %zu source lines of code.\n", dat[i].name, dat[i].val);
							}
							freedat:
							free(dat[i].name);
							col = col == 0 ? 36 : 0;
						}
						free(dat);
						if(colours)
							fputs("\033\133m", ofh);
					}
					if(CSLOC_ISQUIET(options))
					{
						if(numfirst)
							fprintf(ofh, "%zu %s\n", total, dir);
						else
							fprintf(ofh, "%s %zu\n", dir, total);
					}
					else if(CSLOC_ISFSIZE(options))
						fprintf(ofh, "All files in %s combined have a grand total of %zu bytes.\n",dir,total);
					else
						fprintf(ofh, "All files in %s combined have %zu source lines of code.\n",dir,total);
					if(exttots)
					{
						for(size_t j = 0; j < fel; ++j)
							fprintf(ofh, "Total ending with .%s: %zu\n", fexts[j], exttots[j]);
					}
				}
				else
				{
					total = cnt_single_file(dir, cr);
					if(CSLOC_ISQUIET(options))
					{
						if(numfirst)
							fprintf(ofh, "%zu %s\n", total, dir);
						else
							fprintf(ofh, "%s %zu\n", dir, total);
					}
					else if(CSLOC_ISFSIZE(options))
						fprintf(ofh, "The file %s has %zu bytes.\n",dir,total);
					else
						fprintf(ofh, "The file %s has %zu source lines of code.\n",dir,total);
				}
			}
		}
		if(dircnt == 0)
			puts("Specify a directory, run with no arguments for help message.");
		if(fexts)
			free(fexts);
		if(exttots)
			free(exttots);
		if(ofh != stdout)
			fclose(ofh);
	}
	return 0;
}
