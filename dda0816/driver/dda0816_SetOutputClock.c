#define __NO_VERSION__


#include <dda0816_P.h>


dda0816_SetOutputClock( struct inode *inode,int clock )
{

			outb(DDA08_OCLCK, DDA08_BASE+DDA08_DP);
			outb(clock, DDA08_BASE+DDA08_DATAL);
			return(0);
		
}
