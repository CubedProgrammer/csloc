// This file is part of csloc.
// Copyright (C) 2020-2022, github.com/CubedProgrammer, owner of said account.

// csloc is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// csloc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with csloc. If not, see <https://www.gnu.org/licenses/>.

#ifndef Included_csloc_h
#define Included_csloc_h
#define CSLOC_ISQUIET(n)((n) & 1)
#define CSLOC_ISIGNDOT(n)((n) & 2)
#define CSLOC_ISSIF(n)((n) & 4)
#define CSLOC_ISSORT(n)((n) & 010)
#define CSLOC_ISFSIZE(n)((n) & 020)
#define CSLOC_ISRSORT(n)((n) & 040)
#define CSLOC_ISNOLNK(n)((n) & 0100)
#define CSLOC_ISUQUIET(n)((n) & 0200)
#define CSLOC_QUIET 1
#define CSLOC_IGNDOT 2
#define CSLOC_SIF 4
#define CSLOC_SORT 014
#define CSLOC_FSIZE 020
#define CSLOC_RSORT 054
#define CSLOC_NOLNK 0100
#define CSLOC_ULTRAQUIET 0201

struct csloc_file_entry
{
	size_t val;
	char *name;
};
typedef struct csloc_file_entry csloc_filen;
typedef struct csloc_file_entry *csloc_filenp;
const char *csloc_get_ext(const char *name);
int csloc____ispref(const char *pref, const char *str);
int csloc____case_insensitive_strcmp(const char *xstr, const char *ystr);
void csloc_sort_filen(int val, csloc_filenp arr, size_t sz);
#ifdef _WIN32
long long
#else
long
#endif
cnt_single_file(const char *file, size_t cr);
#ifdef _WIN32
long long
#else
long
#endif
csloc(const char *dir, csloc_filenp *dat, size_t *sz, unsigned ops, size_t cr, const char *const*fexts, size_t fel);
#endif
