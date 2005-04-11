/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           cptm.c
// Purpose:        cptm test
// Description:    Connect to cptm and perform some test cnafs
//
// Modules:
// Author:         R. Lutter
// Revision:       
// Date:           Apr 2005
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

static volatile unsigned long * cptm;

int main(int argc, char *argv[]) {

	int i;
	short rdptr, wrptr;
	short data;
	int camac_n;

	if (argc <= 1) {
		fprintf(stderr, "cptm: Test C_PTM module\nUsage: cptm <Nxx>   (Nxx = Camac station)\n");
		exit(1);
	}

	camac_n = atoi(argv[1]);

	cc32_init((unsigned long *) 0x550000, 1);
	cptm = CC32_SET_BASE(C(1), N(camac_n));
	wrptr = CC32_READ_R2B(cptm, F(0), A(13));
	rdptr = CC32_READ_R2B(cptm, F(0), A(14));
	printf("before r+w reset: read addr = %d    write addr = %d\n", rdptr, wrptr);
	CC32_CTRL_R2B(cptm, F(8), A(3));
	wrptr = CC32_READ_R2B(cptm, F(0), A(13));
	rdptr = CC32_READ_R2B(cptm, F(0), A(14));
	printf("after r+w reset: read addr = %d    write addr = %d\n", rdptr, wrptr);
	for (i = 0; i < 10; i++) {
		data = CC32_READ_R2B(cptm, F(0), A(6));
		printf("data[%d] = %d\n", i, data);
	}
	wrptr = CC32_READ_R2B(cptm, F(0), A(13));
	rdptr = CC32_READ_R2B(cptm, F(0), A(14));
	printf("after read: read addr = %d    write addr = %d\n", rdptr, wrptr);
	CC32_CTRL_R2B(cptm, F(8), A(3));
	wrptr = CC32_READ_R2B(cptm, F(0), A(13));
	rdptr = CC32_READ_R2B(cptm, F(0), A(14));
	printf("after r+w reset: read addr = %d    write addr = %d\n", rdptr, wrptr);
}
