#ifndef GET_SUB_DIR_H_
#define GET_SUB_DIR_H_
enum file_or_directory
{NFILE = 997, DIRECTORY};
int cnt_sub_dirs(const char *dir);
void get_sub_dirs(const char *dir,char *names[],enum file_or_directory *fd);
#endif
