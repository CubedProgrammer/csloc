#ifndef Included_csloc_h
#define Included_csloc_h
#define CSLOC_ISQUIET(n)((n) & 1)
#define CSLOC_ISIGNDOT(n)((n) & 2)
#define CSLOC_ISSIF(n)((n) & 4)
#define CSLOC_ISSORT(n)((n) & 010)
#define CSLOC_ISFSIZE(n)((n) & 020)
#define CSLOC_QUIET 1
#define CSLOC_IGNDOT 2
#define CSLOC_SIF 4
#define CSLOC_SORT 010
#define CSLOC_FSIZE 020
struct csloc_file_entry
{
	size_t val;
	char *name;
};
typedef struct csloc_file_entry csloc_filen;
typedef struct csloc_file_entry *csloc_filenp;
int csloc____case_insensitive_strcmp(const char *xstr, const char *ystr);
void csloc_sort_filen(int val, csloc_filenp arr, size_t sz);
#ifdef _WIN32
long long
#else
long
#endif
csloc(const char *dir, csloc_filenp *dat, size_t *sz, unsigned ops, size_t cr, const char *const*fexts, size_t fel);
#endif
