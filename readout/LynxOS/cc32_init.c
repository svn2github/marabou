/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           cc32_init.c
// Purpose:        Initialize VC32/CC32 controller
// Description:    VME address mapping for VC32/CC32 controllers
// Author:         R. Lutter
// Revision:       
// Date:           Jun 2002
// Keywords:       
//
//EXP_SETUP
////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "cc32_camio.h"
#include "vmelib.h"

#define camAddrSpace	0x8000L

static struct pdparam_master s_param; 			/* vme segment params */

volatile unsigned long * cc32_baseAddr;

void cc32_init(unsigned long * physAddrCrate1, int nofCrates) {

	unsigned long * physAddr;
	volatile unsigned long * baseAddr;
	int i;

	s_param.iack = 1;						/* prepare vme segment */
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					/* forces static mapping! */

	physAddr = physAddrCrate1;
	cc32_baseAddr = (volatile unsigned long *) find_controller(physAddr, camAddrSpace * nofCrates, 0x39, 0, 0, &s_param);
}

