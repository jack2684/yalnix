// Code reference from: Zed's Awesome Debug Macros (url: http://c.learncodethehardway.org/book/ex20.html)
#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

//#define NDEBUG 1

#ifdef NDEBUG
    #define _debug(M, ...)
#else
    #define _debug(M, ...) TracePrintf(4, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

//#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define clean_errno() (0 == 0 ? "None" : strerror(errno))

#define log_err(M, ...) TracePrintf(0, "[ERROR] (%s:%d)[%s]\t" M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) TracePrintf(5, "[INFO] (%s:%d)[%s]\t" M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define user_log(M, ...) TracePrintf(5, "[USER] >> " M "\n", ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif

