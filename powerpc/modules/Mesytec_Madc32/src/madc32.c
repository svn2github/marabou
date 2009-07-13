/*____________________________________________________________[C MAIN PROGRAM]
//////////////////////////////////////////////////////////////////////////////
//! \file			madc32.c
//! \brief			test program for Mesytec MADC32
//! \details		Provides some basic tests for MADC32
//!
//! $Author: Rudolf.Lutter $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $       
//! $Date: 2009-07-13 06:22:39 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "vmelib.h"

#include "madc32.h"
#include "madc32_protos.h"

#include "root_env.h"

#include "Version.h"

static struct pdparam_master s_param; 		/* vme segment params */
static struct s_madc32 * s_adc;

void f_ut_send_msg(char * prefix, char * msg, int flag);

int main(int argc, char *argv[]) {

	int i;
	int version;
	unsigned int physAddr;
	unsigned int addrMod;
	unsigned short srval;
	unsigned long bcval;
	volatile char * madc32;

	char fct;
	char file[256];
	FILE * out;

	if (argc <= 2) {
		fprintf(stderr, "madc32: Basic tests for Mesytec MADC32 modules\n\n");
		fprintf(stderr, "Usage: madc32 <addr> [<fct> [<args>]]\n\n");
		fprintf(stderr, "       addr          VME addr, high order bits 17-31, A32 or A24\n");
		fprintf(stderr, "       fct           function to be executed\n");
		fprintf(stderr, "            v        read firmware version\n");
		fprintf(stderr, "            r        reset module\n");
		fprintf(stderr, "            l file   load settings from file <file>\n");
		fprintf(stderr, "            d [file] dump settings to file <file> (default: stdout)\n\n");
		exit(1);
	}

	physAddr = (unsigned int) strtol(argv[1], NULL, 16);
	if (physAddr & 0xFF00) addrMod = 0x9; else addrMod = 0x39;
	physAddr <<= 16;

	s_param.iack = 1;						/* prepare vme segment */
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					/* forces static mapping! */

 	madc32 = (volatile char *) find_controller(physAddr, 0x10000L, addrMod, 0, 0, &s_param);
	if (madc32 == (volatile char *) -1) {
		fprintf(stderr, "madc32: Can't map addr %#lx (mod=%#lx)\n", physAddr, addrMod);
		exit(1);
	}

	s_adc = madc32_alloc(physAddr, madc32, "madc32", 0xBB);	/* allocate data struct */
	if (s_adc == NULL) exit(1);

	madc32_setPrefix(s_adc, "madc32");

	fct = *argv[2];

	file[0] = '\0';
	if (argc >= 4) strcpy(file, argv[3]);

	switch (fct) {
		case 'v':	madc32_moduleInfo(s_adc);				/* read module info */
					break;
		case 'l':	if (file[0] == '\0') {
						fprintf(stderr, "madc32: File name missing\n");
						exit(1);
					}
					madc32_fillStruct(s_adc, file);			/* fill struct from environment data */
					madc32_printDb(s_adc);
					madc32_loadFromDb(s_adc); 				/* load module regs from database */
					break;
		case 'd':	if (file[0] == '\0') {
						fprintf(stderr, "madc32: File name missing\n");
						exit(1);
					}
					s_adc->dumpRegsOnInit = TRUE;
					madc32_dumpRegisters(s_adc, file); 		/* dump registers to file */
					madc32_dumpRaw(s_adc, "rawDump.dat");
					break;
		default:	fprintf(stderr, "madc32: Illegal function - %s\n", argv[2]);
					exit(1);
	}
	exit(0);
}

void f_ut_send_msg(char * prefix, char * msg, int flag) { printf("%s\n", msg); }
