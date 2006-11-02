/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mpiorl.c
// Purpose:        mpiorl test
// Description:    Test MPIROL register
//
// Modules:
// Author:         R. Lutter
// Revision:       
// Date:           Apr 2003
// Keywords:       
//
////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "s_evhe.h"
#include "s_veshe.h"
#include "vmelib.h"

#include "err_mask_def.h"
#include "errnum_def.h"

#define HD_IOReg_R_DATART	0x00
#define HD_IOReg_R_DATARS	0x02
#define HD_IOReg_R_DATARL	0x04
#define HD_IOReg_R_DATAWS	0x06
#define HD_IOReg_R_DATAW	0x08
#define HD_IOReg_R_STROUT	0x0a
#define HD_IOReg_R_IRQMASK	0x0c
#define HD_IOReg_R_DRESET	0x0e
#define HD_IOReg_R_IRQENA	0x10
#define HD_IOReg_R_IRQDIS	0x12
#define HD_IOReg_R_IRQVEC	0x14
#define HD_IOReg_R_DATAWR	0x16
#define HD_IOReg_R_BRESET	0x18

#define HD_IOReg_B_EIRQ 	0x100
#define HD_IOReg_B_BUSY 	0x200
#define HD_IOReg_B_IBUSY	0x400
#define HD_IOReg_B_OBUSY	0x800

#define HD_IOReg_A_DATART(module) 		((unsigned short *) (module + HD_IOReg_R_DATART))
#define HD_IOReg_A_DATARS(module) 		((unsigned short *) (module + HD_IOReg_R_DATARS))
#define HD_IOReg_A_DATARL(module) 		((unsigned short *) (module + HD_IOReg_R_DATARL))
#define HD_IOReg_A_DATAWS(module) 		((unsigned short *) (module + HD_IOReg_R_DATAWS))
#define HD_IOReg_A_DATAW(module) 		((unsigned short *) (module + HD_IOReg_R_DATAW))
#define HD_IOReg_A_STROUT(module) 		((unsigned short *) (module + HD_IOReg_R_STROUT))
#define HD_IOReg_A_IRQMASK(module) 		((unsigned short *) (module + HD_IOReg_R_IRQMASK))
#define HD_IOReg_A_DRESET(module) 		((unsigned short *) (module + HD_IOReg_R_DRESET))
#define HD_IOReg_A_IRQENA(module) 		((unsigned short *) (module + HD_IOReg_R_IRQENA))
#define HD_IOReg_A_IRQDIS(module) 		((unsigned short *) (module + HD_IOReg_R_IRQDIS))
#define HD_IOReg_A_IRQVEC(module) 		((unsigned short *) (module + HD_IOReg_R_IRQVEC))
#define HD_IOReg_A_DATAWR(module) 		((unsigned short *) (module + HD_IOReg_R_DATAWR))
#define HD_IOReg_A_BRESET(module) 		((unsigned short *) (module + HD_IOReg_R_BRESET))

#define HD_IOReg_CLEAR_DATA(module) 	 	*(HD_IOReg_A_DRESET(module)) = 0; hd_ioreg_bits = 0
#define HD_IOReg_RESET_BUSY(module) 		*(HD_IOReg_A_BRESET(module)) = 0
#define HD_IOReg_READ_DATA(module)			*(HD_IOReg_A_DATARS(module))
#define HD_IOReg_WRITE_DATA(module, bits)	hd_ioreg_bits = bits; *(HD_IOReg_A_DATAW(module)) = hd_ioreg_bits
#define HD_IOReg_SET_BITS(module, bits) 	hd_ioreg_bits |= bits; *(HD_IOReg_A_DATAW(module)) = hd_ioreg_bits
#define HD_IOReg_CLEAR_BITS(module, bits) 	hd_ioreg_bits &= ~bits; *(HD_IOReg_A_DATAW(module)) = hd_ioreg_bits
#define HD_IOReg_TOGGLE_BITS(module, bits)	HD_IOReg_SET_BITS(module, bits); HD_IOReg_CLEAR_BITS(module, bits)
#define HD_IOReg_DISABLE_IRQ(module)		*(HD_IOReg_A_IRQDIS(module)) = 0

#define MODE_INPUT	0
#define MODE_OUTPUT 1
#define MODE_SET 2
#define MODE_CLEAR 3
#define MODE_TOGGLE 4

static struct pdparam_master s_param; 		/* vme segment params */

volatile unsigned char * mpiorl_baseAddr;

int main(int argc, char *argv[]) {

	int bits;
	int modeFlag;
	unsigned short hd_ioreg_bits;

	if (argc <= 1) {
		fprintf(stderr, "mpiorl: Test MPIROL register\n        Usage: mpiorl <mode> <bits>\n");
		fprintf(stderr, "mpiorl: Test MPIROL register\n        mode = i(nput), o(utput), s(et), c(lear), or t(oggle)\n");
		exit(1);
	}

	if (*argv[1] == 'i') {
		modeFlag = MODE_INPUT;
	} else if (*argv[1] == 'o') {
		modeFlag = MODE_OUTPUT;
		if (argc == 3) {
			bits = strtol(argv[2], NULL, 0);
		} else {
			fprintf(stderr, "mpiorl: Test MPIROL register\n        Usage: (output) mpiorl o <bits>\n");
			exit(1);
		}
	} else if (*argv[1] == 's') {
		modeFlag = MODE_SET;
		if (argc == 3) {
			bits = strtol(argv[2], NULL, 0);
		} else {
			fprintf(stderr, "mpiorl: Test MPIROL register\n        Usage: (output) mpiorl s <bits>\n");
			exit(1);
		}
	} else if (*argv[1] == 'c') {
		modeFlag = MODE_CLEAR;
		if (argc == 3) {
			bits = strtol(argv[2], NULL, 0);
		} else {
			fprintf(stderr, "mpiorl: Test MPIROL register\n        Usage: (output) mpiorl c <bits>\n");
			exit(1);
		}
	} else if (*argv[1] == 't') {
		modeFlag = MODE_TOGGLE;
		if (argc == 3) {
			bits = strtol(argv[2], NULL, 0);
		} else {
			fprintf(stderr, "mpiorl: Test MPIROL register\n        Usage: (toggle) mpiorl t <bits>\n");
			exit(1);
		}
	} else {
		fprintf(stderr, "mpiorl: Test MPIROL register\n        Usage: mpiorl <mode> <bits>\n");
		fprintf(stderr, "mpiorl: Test MPIROL register\n        mode = i(nput), o(utput), s(et), c(lear), or t(oggle)\n");
		exit(1);
	}

	s_param.iack = 1;						/* prepare vme segment */
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					/* forces static mapping! */

	mpiorl_baseAddr = (volatile unsigned char *) find_controller(0x0, 256, 0x29, 0, 0, &s_param);

	HD_IOReg_DISABLE_IRQ(mpiorl_baseAddr);
	HD_IOReg_CLEAR_DATA(mpiorl_baseAddr);
	HD_IOReg_RESET_BUSY(mpiorl_baseAddr);

	if (modeFlag == MODE_INPUT) {
		bits = HD_IOReg_READ_DATA(mpiorl_baseAddr);
		printf("mpiorl: input reg = %#x\n", bits);
	} else if (modeFlag == MODE_OUTPUT) {
		HD_IOReg_WRITE_DATA(mpiorl_baseAddr, bits);
		printf("mpiorl: setting output reg = %#x\n", bits);
	} else if (modeFlag == MODE_SET) {
		HD_IOReg_SET_BITS(mpiorl_baseAddr, bits);
		printf("mpiorl: setting bits = %#x\n", bits);
	} else if (modeFlag == MODE_CLEAR) {
		HD_IOReg_CLEAR_BITS(mpiorl_baseAddr, bits);
		printf("mpiorl: clearing bits = %#x\n", bits);
	} else if (modeFlag == MODE_TOGGLE) {
		printf("mpiorl: toggling output reg = %#x - ^C to stop\n", bits);
		for (;;) {
			HD_IOReg_SET_BITS(mpiorl_baseAddr, bits);
			HD_IOReg_CLEAR_BITS(mpiorl_baseAddr, bits);		
		}
	}
}
