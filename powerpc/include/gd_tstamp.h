#include "typedefs.h"
/*_______________________________________________________________[C STRUCTURE]
//////////////////////////////////////////////////////////////////////////////
// Name:           time_stamp
// Purpose:        Define a time stamp
// Struct members: unsigned long l_res       -- clock resolution in nsecs
//                 unsigned long l_sec       -- seconds since January 1, 1970
//                 unsigned long l_nsec      -- nano secs of current second
// Description:    |
// Keywords:       |
////////////////////////////////////////////////////////////////////////////*/

typedef struct {
	unsigned long l_res;
	unsigned long l_sec;
	unsigned long l_nsec;
} time_stamp;
