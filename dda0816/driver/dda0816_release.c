/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/
#define __NO_VERSION__

#include <dda0816_P.h>

extern struct dda0816_kernel dda0816_resource[16];

void dda0816_release ( struct inode* inode,struct file* file )
{
  DBGin("dda0816_release");
  MOD_DEC_USE_COUNT;
  {
		int minor = MINOR(inode->i_rdev);

		u8 dummy;
		u8 bit_to_clear;
		u8	channel;

		printk("dda0816 module has been released, %d\n", minor);
		outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
		dummy = inb(DDA08_BASE+DDA08_BCSR) & !DDA08_RCT;
		outb(dummy, DDA08_BASE+DDA08_BCSR);
	    if (dda0816_resource[minor].curve.points) {
			vfree(dda0816_resource[minor].curve.points);
			dda0816_resource[minor].curve.points = NULL;
			dda0816_resource[minor].curve.size = 0;
		}
    	switch (minor>>2) {

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

		/* Set channel to 0V at end */
		outb(minor, DDA08_BASE+DDA08_DP);
		outb(0, DDA08_BASE+DDA08_DATAL);
		outb(0x8, DDA08_BASE+DDA08_DATAH);
	
  }
  DBGout();
}
