#define __NO_VERSION__


#include <dda0816_P.h>


int dda0816_GetPacer( struct inode *inode  )
{

  int retval;

  outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
  retval = (inb(DDA08_BASE+DDA08_BCSR) & DDA08_RCT)?1:0;
  
  return (retval);
}
