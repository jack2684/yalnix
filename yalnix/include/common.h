/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
/* Some useful library for both user and kernal
 *  */
#ifndef _COMMON_LIB
#define _COMMON_LIB

#include "debug.h"

#define YALNIX_BUILD    0

#define _ENABLE         1
#define _UNABLE         0

#define _VALID          1
#define _INVALID        0

#define _SUCCESS         0
#define _FAILURE         -1

#define WORD_LEN 32

typedef unsigned int    uint32;
/*#define min(x,y) ({ \
typeof(x) _min1 = (x);  \
typeof(y) _min2 = (y);  \
(void) (&_min1 == &_min2);  \
_min1 < _min2 ? _min1 ：_min2;})*/


#endif
