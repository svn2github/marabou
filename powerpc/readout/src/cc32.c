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
#include "ces/vmelib.h"

#include "err_mask_def.h"
#include "errnum_def.h"

extern volatile unsigned long * cc32_baseAddr;

static volatile unsigned long * addr0;		/* crate#1, N(0).A(0).F(0) */
static volatile unsigned long * sr; 		/* status register: N(0) */

int main(int argc, char *argv[]) {

	int i;
	int nofCrates;
	int version;
	unsigned short srval;
	unsigned long bcval;
	unsigned long * addr;

	if (argc <= 1) {
		fprintf(stderr, "cc32: Read status of CC32\nUsage: cc32 <n>   (n = number of crates)\n");
		exit(1);
	}
	nofCrates = atoi(argv[1]);

	if (argc == 3) {
		addr = (unsigned long *) strtol(argv[2], (char **)NULL, 16);
	} else {
		addr = (unsigned long *) 0xee550000;
	}
	
	printf("cc32: nofCrates=%d, baseAddr=%#lx\n", nofCrates, addr);
	
	cc32_init(addr, nofCrates);
	for (i = 1; i <= nofCrates; i++) {
		sr = CC32_SET_BASE(C(i), N(0));
		srval = CC32_READ_R2B(sr, F(0), A(0));
		version = (srval >> 8) & 0xF;
		printf("c%d: sts_addr = %#lx, sts = %#x, version = %#x\n", i, sr, srval, version);
	}
}
