#define __NO_VERSION__


#include <dda0816_P.h>


dda0816_ClearUpdateGroup( struct inode *inode  )
{

			int minor = MINOR(inode->i_rdev);
			u8 bit_to_clear;
			u8	channel;
			u8 	dummy;
			
			minor = minor >> 2;
			switch (minor) {

			case 0:	bit_to_clear = !DDA08_TG0;
				channel = DDA08_CCSR_DAC01;
				break;
			case 1:	bit_to_clear = !DDA08_TG1;
				channel = DDA08_CCSR_DAC01;
				break;
			case 2:	bit_to_clear = !DDA08_TG0;
				channel = DDA08_CCSR_DAC23;
				break;
			case 3:	bit_to_clear = !DDA08_TG1;
				channel = DDA08_CCSR_DAC23;
				break;
			}
			outb(channel, DDA08_BASE+DDA08_SP);
			dummy = inb(DDA08_BASE+DDA08_CCSR);
			outb(dummy&bit_to_clear, DDA08_BASE+DDA08_CCSR);
			return(0);
		
}
