#define __NO_VERSION__


#include <dda0816_P.h>


dda0816_StartPacer( struct inode *inode  )
{

  u8 dummy;
  int i;
  int data;

  extern struct dda0816_kernel dda0816_resource[];

  for (i=0; i < 8 ; i++) {
	if (dda0816_resource[i].curve.points) {
	  dda0816_resource[i].cycle.curcycl = 0;
	  outb(i, DDA08_BASE+DDA08_DP);
	  data = dda0816_resource[i].curve.points[dda0816_resource[i].step++];
	  outb(data, DDA08_BASE+DDA08_DATAL);
	  outb(data>>8, DDA08_BASE+DDA08_DATAH);
	  if (dda0816_resource[i].step >= dda0816_resource[i].curve.size) {
		dda0816_resource[i].step=0;				
	  }
	}
  }
  outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
  dummy = inb(DDA08_BASE+DDA08_BCSR) | DDA08_RCT;
  outb(dummy, DDA08_BASE+DDA08_BCSR);
		
}
