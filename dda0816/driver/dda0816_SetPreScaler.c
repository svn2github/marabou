#define __NO_VERSION__


#include <dda0816_P.h>


dda0816_SetPreScaler( struct inode *inode,int period )
{

			u8 dummy;

			outb(DDA08_BCSR_MISC, DDA08_BASE+DDA08_SP);
			dummy = inb(DDA08_BASE+DDA08_BCSR) & !DDA08_PS;
			dummy = dummy | period;
			outb(dummy, DDA08_BASE+DDA08_BCSR);
			return(0);
				
		
}
