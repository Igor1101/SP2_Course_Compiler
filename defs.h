#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>

#ifdef DEBUG_APP
#define pr_debug(...) do { printf("DEBUG:" __VA_ARGS__); puts(""); } while(0)
#else
#define pr_debug(...)
#endif
#define pr_info(...) do { printf(__VA_ARGS__); puts(""); } while(0)
#define pr_err(...) do { fprintf(stderr, "ERROR:"  __VA_ARGS__); puts(""); } while(0)

#endif /* DEFS_H */
