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

int dda0816_write(struct inode *inode, struct file *file, char *buffer, int count)
{
  int retval=0;

  DBGin("dda0816_write");
  {
		u8 value;
		int cntr=0;
		int minor = MINOR(inode->i_rdev);

		if (minor < 0 && minor > DDA08_NRCHAN ) {
			printk("Channel out of range %d\n", minor);
			return -1;
		}
		printk("Count %d",count);
		if (count <= 1) {
			printk("Not enough data\n");
			return 0;
		}
		outb(minor, DDA08_BASE+DDA08_DP);
		while (cntr < count) {
			get_user(value, buffer+cntr);
			outb(value, DDA08_BASE+DDA08_DATAL);
			printk("Value low: %d ", value);
			get_user(value, buffer+cntr+1);
			outb(value&0xf, DDA08_BASE+DDA08_DATAH);
			printk("Value high: %d\n", value&0xf);
			cntr += 2;
		}

		return count;
	
	
  }
  DBGout();
  return retval;
}

