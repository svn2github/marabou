#define __NO_VERSION__


#include <dda0816_P.h>


dda0816_SetClockSource( struct inode *inode,int source )
{

			u8 dummy;

			outb(DDA08_BCSR_CSR, DDA08_BASE+DDA08_SP);
			dummy = inb(DDA08_BASE+DDA08_BCSR);
			dummy = (dummy & !DDA08_CS) | (source & DDA08_CS);
			outb(dummy, DDA08_BASE+DDA08_BCSR);
		
}
