#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <sys/stat.h>

#define mkdir(path, mode) _mkdir(path)
#define fsync(fd) _commit(fd)
#define lstat(path, st) stat(path, st)
#endif
