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


  int irq_handler() {
	extern struct dda0816_kernel dda0816_resource[];
	int i;
	int data;
	int stop = 0;

	u8 dummy;

	outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
	dummy = inb(DDA08_BASE+DDA08_BCSR);
	dda0816_resource[0].irq++;
	if (dummy & DDA08_UPD) {
	  for (i=0; i < 8 ; i++) {
	    if (dda0816_resource[i].curve.points) {
	      if (dda0816_resource[i].scaler <= 0) {
	    	outb(i, DDA08_BASE+DDA08_DP);
	    	data = dda0816_resource[i].curve.points[dda0816_resource[i].step++];
		outb(data, DDA08_BASE+DDA08_DATAL);
		outb(data>>8, DDA08_BASE+DDA08_DATAH);
		if (dda0816_resource[i].step >= dda0816_resource[i].curve.size) {
		  dda0816_resource[i].step=0;				
		  dda0816_resource[i].cycle.curcycl++;
		  if ( dda0816_resource[i].cycle.cycles ) {
		    if ( dda0816_resource[i].cycle.curcycl >= dda0816_resource[i].cycle.cycles ) {
	              outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
	              dummy = inb(DDA08_BASE+DDA08_BCSR) & !DDA08_RCT;
	              outb(dummy, DDA08_BASE+DDA08_BCSR);
		    }
                  }
		}
		dda0816_resource[i].scaler = dda0816_resource[i].curve.SoftScale;
	      } else {
		dda0816_resource[i].scaler--;
	      }
	    }
	  }
	}
  }
	
