#define __NO_VERSION__


#include <dda0816_P.h>


dda0816_SetCycle( struct inode *inode,int cycles )
{

  extern struct dda0816_kernel dda0816_resource[16];
  int minor = MINOR(inode->i_rdev);

  dda0816_resource[minor].cycle.cycles = cycles;

  return(0);
				
		
}
