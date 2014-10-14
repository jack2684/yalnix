/* Some useful library for both user and kernal
 *  */
#ifndef _COMMON_LIB
#define _COMMON_LIB

#define DEBUG 1
#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define YALNIX_BUILD    0

#define _ENABLE         1
#define _UNABLE         0

#define _VALID          1
#define _INVALID        0

#define _SUCCESS         0
#define _FAILURE         1

typedef unsigned int    uint32;
typedef unsigned long   uint32;

#endif
