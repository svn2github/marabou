/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           testParam.c
// Purpose:        Test paramaters of a DGF-4C module
// Description:    Loops in DSP memory to test a parameter
// Author:         R. Lutter
// Revision:       
// Date:           Oct 2002
// Keywords:       
//
////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "cc32_camio.h"
#include "vmelib.h"

#include "SetColor.h"

#include "unix_string.h"
#include "root_env.h"

#include "Version.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Useful macro defs
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define CC32_AUTOREAD_ON(ccar)		CC32_WRITE_R2B(ccar, A(0), F(3), CC32_READ_R2B(ccar, A(0), F(3)) | (0x1 << 12))
#define CC32_AUTOREAD_OFF(ccar)		CC32_WRITE_R2B(ccar, A(0), F(3), CC32_READ_R2B(ccar, A(0), F(3)) & ~(0x1 << 12))

#define CC32_READ_DWORD_ON(ccrdw)	CC32_WRITE_R2B(ccrdw, A(2), F(16), 0)
#define CC32_READ_DWORD_OFF(ccrdw)	CC32_WRITE_R2B(ccrdw, A(3), F(16), 0)

#define CC32_READ_STATUS(ccsts)		CC32_READ_R2B(ccsts, A(0), F(0))
#define CC32_READ_I(ccsts)			(CC32_READ_R2B(ccsts, A(0), F(0)) & 0x1)
#define CC32_READ_L(ccsts)			(CC32_READ_R2B(ccsts, A(0), F(0)) & 0x2)
#define CC32_READ_X(ccsts)			(CC32_READ_R2B(ccsts, A(0), F(0)) & 0x4)
#define CC32_READ_Q(ccsts)			(CC32_READ_R2B(ccsts, A(0), F(0)) & 0x8)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Device-dependent definitions
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define DGF_INPAR_START_ADDR	0x4000

/* DGF-4C inline functions */
#define DGF_WRITE_ICSR(dgf, data)	CC32_WRITE_R2B(dgf, A(8), F(17), data)
#define DGF_READ_ICSR(dgf)		CC32_READ_R2B_16(dgf, A(8), F(1))
#define DGF_WRITE_CSR(dgf, data)	CC32_WRITE_R2B(dgf, A(0), F(17), data)
#define DGF_READ_CSR(dgf)		CC32_READ_R2B_16(dgf, A(0), F(1))
#define DGF_WRITE_TSAR(dgf, addr)	CC32_WRITE_R2B(dgf, A(1), F(17), addr)
#define DGF_READ_TSAR(dgf) 		CC32_READ_R2B_16(dgf, A(1), F(1))
#define DGF_WRITE_WCR(dgf, data)	CC32_WRITE_R2B(dgf, A(2), F(17), data)
#define DGF_READ_WCR(dgf)		CC32_READ_R2B_16(dgf, A(2), F(1))
#define DGF_WRITE_PSA(dgf, offset)	CC32_WRITE_R2B(dgf, A(1), F(17), DGF_INPAR_START_ADDR + offset)
#define DGF_READ_DSP(dgf)		CC32_READ_R2B_16(dgf, A(0), F(0))
#define DGF_WRITE_DSP(dgf, data)	CC32_WRITE_R2B(dgf, A(0), F(16), data)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Global pointers
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
extern volatile unsigned long * cc32_baseAddr;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Prototypes
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* camac controller */
void cc32_init(unsigned long * physAddrCrate1, int nofCrates);


int main(int argc, char * argv[]) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           testParams
// Purpose:        Test parameters of a DGF-4C module
// Arguments:      int crate             -- crate number
//                 int station           -- station number
//                 int poffset           -- param offset
//                 unsigned int pvalue   -- param value
//                 int loop              -- loop count
//                 int setparam          -- param to be set if 1
// Results:        
// Exceptions:     
// Description:      
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	int crate;
	int station;
	int poffset;
	unsigned int pvalue, addr;
	unsigned int data;
	int loop;
	int setparam;
	int parErrors, tsarErrors;
	float errPerc;
	int w;

	volatile unsigned long * dgf;

	if (argc < 7) {
		fprintf(stderr, "%stestParam: Illegal number of arguments - %d%s\n", setred, argc - 1, setblack);
		fprintf(stderr, "\n           Usage: testParam <crate> <station> <poffset> <pvalue> <loop> <setparam>\n\n");
		exit(1);
	}

	crate = atoi(argv[1]);
	station = atoi(argv[2]);
	poffset = atoi(argv[3]);
	pvalue = atoi(argv[4]);
	loop = atoi(argv[5]);
	setparam = atoi(argv[6]);

 	cc32_init((unsigned long *) 0xee550000L, crate);				/* vme addr mapping for crates 1 ... 3 */

	printf("----------------------------------------------------------------------\n");
	printf("testParam.C:\n");
	printf("    DGF-4C in C%d.N%d\n", crate, station);
	printf("    Param offset %d\n", poffset);
	if (setparam) 	printf("    Param value set to %d\n", pvalue);
	else				printf("    Param value expected to be %d\n", pvalue);
	printf("    Loop is %d times\n", loop);
	printf("----------------------------------------------------------------------\n");

	dgf = CC32_SET_BASE(C(crate), N(station));

	if (setparam) {
		DGF_WRITE_PSA(dgf, poffset);
		DGF_WRITE_DSP(dgf, pvalue);
	}

	parErrors = 0;
	tsarErrors = 0;
	printf("\n");
	for (i = 0; i < loop; i++) {
		if ((i % 10) == 0) {
			printf("\r==> Loop count = %04d, error count = %04d", i, parErrors);
			fflush(stdout);
		}
		DGF_WRITE_PSA(dgf, poffset);
		w++;
/*		if (addr != (poffset + DGF_INPAR_START_ADDR)) tsarErrors++;	*/
		data = DGF_READ_DSP(dgf);
		if (data != pvalue) parErrors++;
	}
	errPerc = (((float) parErrors / loop) * 100);

	printf("\n\ntestParam: DGF-4C in C%d.N%d, param offset %d, value %d, %d steps, %d error(s) (%4.1f %%)\n",
									crate, station, poffset, pvalue, loop, parErrors, errPerc);
	if (tsarErrors) printf("testParam: %d TSAR I/O error(s)\n", tsarErrors);

	exit(0);
}
