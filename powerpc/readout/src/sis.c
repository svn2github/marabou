/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           sis.c
// Purpose:        sis test
// Description:    Connect to sis and read status
//
// Modules:
// Author:         R. Lutter
// Revision:       
// Date:           Jul 2004
// Keywords:       
//
////////////////////////////////////////////////////////////////////////////*/

/* bit definitions for VME scaler SIS 3801 */
#define SIS_3801_R_STATUS			0x0L
#define SIS_3801_R_IDENT			0x4L
#define SIS_3801_R_COPY_DISABLE		0xcL
#define SIS_3801_R_WRITE_FIFO		0x10L
#define SIS_3801_R_CLEAR_FIFO		0x20L
#define SIS_3801_R_NEXT_CLOCK		0x24L
#define SIS_3801_R_CLOCK_ENABLE		0x28L
#define SIS_3801_R_CLOCK_DISABLE	0x2cL
#define SIS_3801_R_PULSER_ENABLE	0x50L
#define SIS_3801_R_PULSER_DISABLE	0x54L
#define SIS_3801_R_RESET			0x60L
#define SIS_3801_R_TEST_PULSE		0x68L
#define SIS_3801_R_PRESCALE			0x80L
#define SIS_3801_R_READ_FIFO		0x100L

#define SIS_3801_B_FIFO_EMPTY		(0x1 << 8)
#define SIS_3801_B_FIFO_HALF_FULL	(0x1 << 10)
#define SIS_3801_B_FIFO_FULL		(0x1 << 12)

#define SIS_3801_CS_B_ENA_EXT_NEXT		(0x1 << 16)

#define SIS_3801_A_STATUS(module)			((unsigned long *) (module + SIS_3801_R_STATUS))
#define SIS_3801_A_IDENT(module)		((unsigned long *) (module + SIS_3801_R_IDENT))
#define SIS_3801_A_COPY_DISABLE(module) 	((unsigned long *) (module + SIS_3801_R_COPY_DISABLE))
#define SIS_3801_A_WRITE_FIFO(module)		((unsigned long *) (module + SIS_3801_R_WRITE_FIFO))
#define SIS_3801_A_CLEAR_FIFO(module)		((unsigned long *) (module + SIS_3801_R_CLEAR_FIFO))
#define SIS_3801_A_NEXT_CLOCK(module)		((unsigned long *) (module + SIS_3801_R_NEXT_CLOCK))
#define SIS_3801_A_CLOCK_ENABLE(module) 	((unsigned long *) (module + SIS_3801_R_CLOCK_ENABLE))
#define SIS_3801_A_CLOCK_DISABLE(module)		((unsigned long *) (module + SIS_3801_R_CLOCK_DISABLE))
#define SIS_3801_A_PULSER_ENABLE(module)		((unsigned long *) (module + SIS_3801_R_PULSER_ENABLE))
#define SIS_3801_A_PULSER_DISABLE(module)		((unsigned long *) (module + SIS_3801_R_PULSER_DISABLE))
#define SIS_3801_A_RESET(module)		((unsigned long *) (module + SIS_3801_R_RESET))
#define SIS_3801_A_TEST_PULSE(module)		((unsigned long *) (module + SIS_3801_R_TEST_PULSE))
#define SIS_3801_A_PRESCALE(module) 	((unsigned long *) (module + SIS_3801_R_PRESCALE))
#define SIS_3801_A_READ_FIFO(module)		((unsigned long *) (module + SIS_3801_R_READ_FIFO))

#define SIS_3801_READ_STATUS(module)			*SIS_3801_A_STATUS(module)
#define SIS_3801_DISABLE_COPY(module, b)		*SIS_3801_A_COPY_DISABLE(module) = b
#define SIS_3801_SET_CTRL_BIT(module, b)		*SIS_3801_A_STATUS(module) = b
#define SIS_3801_RESET(module)					*SIS_3801_A_RESET(module) = 0
#define SIS_3801_CLEAR_FIFO(module)				*SIS_3801_A_CLEAR_FIFO(module) = 0
#define SIS_3801_ENABLE_NEXT_CLOCK(module)		*SIS_3801_A_CLOCK_ENABLE(module) = 0

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "s_evhe.h"
#include "s_veshe.h"
#include "vmelib.h"

#include "err_mask_def.h"
#include "errnum_def.h"


void sis_module_info(volatile unsigned char * module);

static struct pdparam_master s_param; 		/* vme segment params */

extern volatile unsigned long * sis_baseAddr;

static volatile unsigned long * addr0;		/* crate#1, N(0).A(0).F(0) */
static volatile unsigned long * sr; 		/* status register: N(0) */

int main(int argc, char *argv[]) {

	int i;
	int version;
	unsigned long addr;
	unsigned short srval;
	unsigned long bcval;
	volatile char * sis;

	if (argc <= 1) {
		fprintf(stderr, "sis: Read status of sis\nUsage: sis <addr>   (vme addr)\n");
		exit(1);
	}
	addr = strtol(argv[1], NULL, 16);

	s_param.iack = 1;						/* prepare vme segment */
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					/* forces static mapping! */

 	sis = (volatile char *) find_controller(addr, 0x200L, 0x39, 0, 0, &s_param);
	sis_module_info(sis);
}

void sis_module_info(volatile unsigned char * module) {
	char str[128];
	unsigned long ident;
	unsigned short modId;
	unsigned short version;
	ident = *SIS_3801_A_IDENT(module);
	modId = (ident >> 16) & 0xFFFF;
	version = (ident >> 12) & 0xF;
	printf("SIS module info: addr %#lx, id %#x, version %x\n", module, modId, version);
}
