/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           cc32.c
// Purpose:        cc32 test
// Description:    Connect to cc32 and read status
//
// Modules:
// Author:         R. Lutter
// Revision:       
// Date:           Jun 2002
// Keywords:       
//
////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "s_evhe.h"
#include "s_veshe.h"
#include "cc32_camio.h"
#include "vmelib.h"

#include "err_mask_def.h"
#include "errnum_def.h"

static struct pdparam_master s_param; 		/* vme segment params */

extern volatile unsigned long * cc32_baseAddr;

static volatile unsigned long * addr0;		/* crate#1, N(0).A(0).F(0) */
static volatile unsigned long * bc; 		/* broadcast register: N(26) */
static volatile unsigned long * sr; 		/* status register: N(0) */

int main() {

	int i;
	unsigned short srval;
	unsigned long bcval;
	int data[] = {	0, 0x252525, 0x525252, 0xffffff };

	s_param.iack = 1;						/* prepare vme segment */
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					/* forces static mapping! */

	cc32_init((unsigned long *) 0xee550000, 3);
	for (i = 1; i <= 3; i++) {
		sr = CC32_SET_BASE(C(i), N(0));
		srval = CC32_READ_R2B(sr, F(0), A(0));
		bc = CC32_SET_BASE(C(i), N(26));
		CC32_WRITE_R2B(bc, F(16), A(0), data[i]);
		bcval = CC32_READ_R2B(bc, F(0), A(0));
		printf("c%d = %#lx, sts = %#x, bcast = %#lx\n", i, sr, srval, bcval);
	}
}
