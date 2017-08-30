/*____________________________________________________________[C MAIN PROGRAM]
//////////////////////////////////////////////////////////////////////////////
//! \file			sis3302.c
//! \brief			test program for SIS3302
//! \details		Provides some basic tests for SIS3302
//!
//! $Author: Rudolf.Lutter $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <smem.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "vmelib.h"

#include "sis_3302_layout.h"
#include "sis_3302_database.h"
#include "sis_3302_protos.h"

#include "root_env.h"
#include "mapping_database.h"
#include "mapping_protos.h"

#include "gd_readout.h"

#include "Version.h"

static s_sis_3302 * s_sis;

void f_ut_send_msg(char * prefix, char * msg, int flag);

int main(int argc, char *argv[]) {

	int i;
	int version;
	unsigned int physAddr;
	unsigned int addrMod;
	unsigned short srval;
	unsigned long bcval;
	volatile char * sis3302;
	unsigned int mappingMod;
	int reg, val;

	s_mapDescr * md;

	char fct;
	char file[256];
	FILE * out;

	if (argc <= 4) {
		fprintf(stderr, "sis3302: Basic tests for SIS3302 modules\n\n");
		fprintf(stderr, "Usage: sis3302 <addr> <addrMod> <mapMod> [<fct> [<args>]]\n\n");
		fprintf(stderr, "       addr           VME addr, high order bits NN (24-31): NNXXXXXX\n");
		fprintf(stderr, "       addrMod        addr modifier, 0x09 (A32) or 0x39 (A24)\n");
		fprintf(stderr, "       mapMod         mapping mode: (bit pattern) 1 (direct, RIO4 only), 2 (static), 4 (dynamic)\n");
		fprintf(stderr, "       fct            function to be executed\n");
		fprintf(stderr, "            v         read firmware version\n");
		fprintf(stderr, "            r         reset module\n");
		fprintf(stderr, "            l file    load settings from file <file>\n");
		fprintf(stderr, "            d file    dump settings to file <file>\n\n");
		fprintf(stderr, "            s reg val set register at relative addr <reg> with value <val>\n");
		fprintf(stderr, "            g reg     get register at relative addr <reg>\n\n");
		exit(1);
	}

	physAddr = (unsigned int) strtol(argv[1], NULL, 16);
	physAddr <<= 24;
	addrMod =  (unsigned int) strtol(argv[2], NULL, 16);

	mappingMod = (unsigned int) strtol(argv[3], NULL, 16);

	md = mapVME("sis3302", physAddr, 0x10000L, addrMod, mappingMod, 0);
	if (md == NULL) {
		fprintf(stderr, "sis3302: Can't map addr %#lx (mod=%#lx)\n", physAddr, addrMod);
		exit(1);
	}

	s_sis = sis3302_alloc("sis3302", md, 0x1);	/* allocate data struct */
	if (s_sis == NULL) exit(1);

	fct = *argv[4];

	switch (fct) {
		case 'v':	sis3302_moduleInfo(s_sis);				/* read module info */
					break;
		case 'l':	file[0] = '\0';
					if (argc >= 6) strcpy(file, argv[5]);
					if (file[0] == '\0') {
						fprintf(stderr, "sis3302: File name missing\n");
						exit(1);
					}
					sis3302_fillStruct(s_sis, file);			/* fill struct from environment data */
					sis3302_loadFromDb(s_sis); 				/* load module regs from database */
					break;
		case 'd':	file[0] = '\0';
					if (argc >= 6) strcpy(file, argv[5]);
					if (file[0] == '\0') {
						fprintf(stderr, "sis3302: File name missing\n");
						exit(1);
					}
					s_sis->dumpRegsOnInit = TRUE;
					sis3302_dumpRegisters(s_sis, file); 		/* dump registers to file */
					break;
		case 's':	reg = -1;
					if (argc >= 6) reg = catoi(argv[5]);
					if (reg == -1) {
						fprintf(stderr, "sis3302: Register addr missing\n");
						exit(1);
					}
					val = -1;
					if (argc >= 7) val = catoi(argv[6]);
					if (val == -1) {
						fprintf(stderr, "sis3302: Register value missing\n");
						exit(1);
					}
					SET16(s_sis->md->vmeBase, reg, val);
					printf("sis3302: Setting register %x to %x\n", reg, val);
					break;				
		case 'g':	reg = -1;
					if (argc >= 6) reg = catoi(argv[5]);
					if (reg == -1) {
						fprintf(stderr, "sis3302: Register addr missing\n");
						exit(1);
					}
					val = GET16(s_sis->md->vmeBase, reg);
					printf("sis3302: Register %#x: %#x\n", reg, val);
					break;
		default:	fprintf(stderr, "sis3302: Illegal function - %s\n", argv[2]);
					exit(1);
	}
	exit(0);
}

void f_ut_send_msg(char * prefix, char * msg, int flag) { printf("%s: %s\n", prefix, msg); }
