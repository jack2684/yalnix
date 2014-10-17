#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include "trace.h"
#include "manuallink.h"

static char tty_buff[4096];		/* really big */

#ifndef TR_IM_WHO
#define TR_IM_WHO TR_IM_USER
#endif

int
TtyPrintf(int tty_id, char * fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vsprintf(tty_buff, fmt, args);
    va_end(args);

    return (TtyWrite(tty_id, tty_buff, strlen(tty_buff)));
}


void
TracePrintf(int level, char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	CALL_V_TRACE_PRINTF(TR_IM_WHO, level, fmt, args);
	va_end(args);
}


