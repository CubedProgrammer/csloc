#ifdef _WIN32
long long
#else
long
#endif
csloc(const char *dir, size_t cr, int sif, int ihf, int quiet, const char *const*fexts, size_t fel);
