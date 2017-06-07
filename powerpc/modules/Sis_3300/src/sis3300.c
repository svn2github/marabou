/*____________________________________________________________[C MAIN PROGRAM]
//////////////////////////////////////////////////////////////////////////////
//! \file			sis3300.c
//! \brief			test program for SIS 3300
//! \details		Provides some basic tests for a SIS3300 flash adc
//!
//! $Author: Rudolf.Lutter $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.1 $       
//! $Date: 2009-04-30 10:42:57 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "vmelib.h"

#include "sis_3300.h"
#include "sis_3300_protos.h"

#include "root_env.h"

#include "Version.h"

static struct pdparam_master s_param; 		/* vme segment params */
static s_sis_3300 * s_adc;

void f_ut_send_msg(char * prefix, char * msg, int flag);

int main(int argc, char *argv[]) {

	int i;
	int version;
	unsigned int physAddr;
	unsigned int addrMod;
	unsigned short srval;
	unsigned long bcval;
	volatile char * sis3300;

	char fct;
	char file[256];
	FILE * out;

	if (argc <= 2) {
		fprintf(stderr, "sis3300: Basic tests for SIS3300 modules\n\n");
		fprintf(stderr, "Usage: sis3300 <addr> [<fct> [<args>]]\n\n");
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

 	sis3300 = (volatile char *) find_controller(physAddr, 0x10000L, addrMod, 0, 0, &s_param);
	if (sis3300 == (volatile char *) -1) {
		fprintf(stderr, "sis3300: Can't map addr %#lx (mod=%#lx)\n", physAddr, addrMod);
		exit(1);
	}

	s_adc = sis_3300_alloc(physAddr, sis3300, "sis3300", 1);	/* allocate data struct */
	if (s_adc == NULL) exit(1);

	fct = *argv[2];

	file[0] = '\0';
	if (argc >= 4) strcpy(file, argv[3]);

	switch (fct) {
		case 'v':	sis_3300_moduleInfo(s_adc);				/* read module info */
					break;
		case 'l':	if (file[0] == '\0') {
						fprintf(stderr, "sis3300: File name missing\n");
						exit(1);
					}
					sis_3300_fillStruct(s_adc, file);			/* fill struct from environment data */
					sis_3300_printDb(s_adc);
					sis_3300_loadFromDb(s_adc); 				/* load module regs from database */
					break;
		case 'd':	if (file[0] == '\0') {
						fprintf(stderr, "sis3300: File name missing\n");
						exit(1);
					}
					s_adc->dumpRegsOnInit = TRUE;
					sis_3300_dumpRegisters(s_adc, file); 		/* dump registers to file */
					break;
		default:	fprintf(stderr, "sis3300: Illegal function - %s\n", argv[2]);
					exit(1);
	}
	exit(0);
}

void f_ut_send_msg(char * prefix, char * msg, int flag) { printf("%s\n", msg); }
