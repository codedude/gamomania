#ifndef __FILE__H
#define __FILE__H

#ifndef PROJECT_PATH
#error "No project path defined during compilation"
#endif

const char *_concatPath(const char *base, ...);
const char *readFile(const char *path);

#define concatPath(...) _concatPath(__VA_ARGS__, NULL)

#endif
