#include "kernelLib.h"

int Y_Brk(void * addr){
	//IF new break space impose on the  current user stack space
		//REPORT ERROR
	//END IF

	//IF new break segment is larger than current user break segment
		//ENLARGE the user heap
	//END IF

	//IF new break segment is smaller than the current user break segment
		//SHRINK the heap space
	//END IF
}
