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

#define YALNIX_BUILD 0

#endif
