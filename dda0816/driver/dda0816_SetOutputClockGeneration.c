#define __NO_VERSION__


#include <dda0816_P.h>


dda0816_SetOutputClockGeneration( struct inode *inode  )
{

			u8 dummy;

			outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
			dummy = inb(DDA08_BASE+DDA08_BCSR)|DDA08_OON;
			outb(dummy, DDA08_BASE+DDA08_BCSR);
		
}
