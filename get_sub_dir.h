// This file is part of csloc.
// Copyright (C) 2020-2023, github.com/CubedProgrammer, owner of said account.

// csloc is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// csloc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with csloc. If not, see <https://www.gnu.org/licenses/>.

#ifndef GET_SUB_DIR_H_
#define GET_SUB_DIR_H_
#define csloc_check_pointer(ptr)if(ptr == NULL)fprintf(stderr, "%s is NULL at %s, line %i\n", #ptr, __FILE__, __LINE__)
enum cfs____file_or_directory
{NFILE = 997, DIRECTORY, CSLOCSYMLINK, CSLOCOTHER};
int csloc____cnt_sub_dirs(const char *dir);
void csloc____get_sub_dirs(const char *dir,char *names[],enum cfs____file_or_directory *fd);
#endif
