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

int dda0816_open(struct inode* inode,struct file* file)
{
  int retval=0;

  DBGin("dda0816_open");
  MOD_INC_USE_COUNT;
  {
		int minor = MINOR(inode->i_rdev);
		printk("dda0816 module has been opened, %d\n", minor);
	
  }
  DBGout();
  return retval;
}

