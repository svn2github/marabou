/*____________________________________________________________[C MAIN PROGRAM]
//////////////////////////////////////////////////////////////////////////////
//! \file			mqdc32.c
//! \brief			test program for Mesytec MQDC32
//! \details		Provides some basic tests for MQDC32
//!
//! $Author: Rudolf.Lutter $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.5 $
//! $Date: 2009-10-27 13:30:44 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <smem.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "vmelib.h"

#include "mesy.h"
#include "mesy_database.h"
#include "mesy_protos.h"

#include "mqdc32.h"
#include "mqdc32_database.h"
#include "mqdc32_protos.h"

#include "gd_readout.h"

#include "root_env.h"
#include "mapping_database.h"
#include "mapping_protos.h"

#include "Version.h"

void f_ut_send_msg(char * prefix, char * msg, int flag);

int main(int argc, char *argv[]) {

	int i;
	int version;
	unsigned int physAddr;
	unsigned int addrMod;
	unsigned short srval;
	unsigned long bcval;
	volatile char * mqdc32;
	unsigned int mappingMod;
	int reg, val;

	s_mqdc32 * s_qdc;
	s_mesy * m_qdc;
	s_mapDescr * md;

	char fct;
	char file[256];
	FILE * out;

	if (argc <= 4) {
		fprintf(stderr, "mqdc32: Basic tests for Mesytec MQDC32 modules\n\n");
		fprintf(stderr, "Usage: mqdc32 <addr> <addrMod> <mapMod> [<fct> [<args>]]\n\n");
		fprintf(stderr, "       addr           VME addr, high order bits NNNN (16-31): NNNNXXXX\n");
		fprintf(stderr, "       addrMod        addr modifier, 0x09 (A32) or 0x39 (A24)\n");
		fprintf(stderr, "       mapMod         mapping mode: (bit pattern) 1 (direct, RIO4 only), 2 (static), 4 (dynamic)\n");
		fprintf(stderr, "       fct            function to be executed\n");
		fprintf(stderr, "            v         read firmware version\n");
		fprintf(stderr, "            l file    load settings from file <file>\n");
		fprintf(stderr, "            d file    dump settings to file <file>\n\n");
		fprintf(stderr, "            s reg val set register at relative addr <reg> with value <val>\n");
		fprintf(stderr, "            g reg     get register at relative addr <reg>\n\n");
		fprintf(stderr, "            a         access module in a loop - DTACK led should be ON\n\n");
		exit(1);
	}

	physAddr = (unsigned int) strtol(argv[1], NULL, 16);
	physAddr <<= 16;
	addrMod =  (unsigned int) strtol(argv[2], NULL, 16);

	mappingMod = (unsigned int) strtol(argv[3], NULL, 16);

	if (mappingMod == 2) smem_remove("mqdc32");		/* remove previous mappings */

	md = mapVME("mqdc32", physAddr, 0x10000L, addrMod, mappingMod, 0);
	if (md == NULL) {
		fprintf(stderr, "mqdc32: Can't map addr %#lx (mod=%#lx)\n", physAddr, addrMod);
		exit(1);
	}

	s_qdc = (s_mqdc32 *) mesy_alloc("mqdc32", sizeof(s_mqdc32), md, 0x1);	/* allocate data struct */
	if (s_qdc == NULL) exit(1);

	m_qdc = &s_qdc->m;
	
	mesy_setPrefix(m_qdc, "mqdc32");

	fct = *argv[4];

	switch (fct) {
		case 'v':	mesy_moduleInfo(m_qdc);				/* read module info */
					break;
		case 'l':	file[0] = '\0';
					if (argc >= 6) strcpy(file, argv[5]);
					if (file[0] == '\0') {
						fprintf(stderr, "mqdc32: File name missing\n");
						exit(1);
					}
					mqdc32_fillStruct(s_qdc, file);			/* fill struct from environment data */
					mqdc32_printDb(s_qdc);
					mqdc32_loadFromDb(s_qdc, 0xFFFFFFFF); 				/* load module regs from database */
					break;
		case 'd':	file[0] = '\0';
					if (argc >= 6) strcpy(file, argv[5]);
					if (file[0] == '\0') {
						fprintf(stderr, "mqdc32: File name missing\n");
						exit(1);
					}
					m_qdc->dumpRegsOnInit = TRUE;
					mqdc32_dumpRegisters(s_qdc, file); 		/* dump registers to file */
					mesy_dumpRaw(m_qdc, "rawDump.dat");
					break;
		case 's':	reg = -1;
					if (argc >= 6) reg = catoi(argv[5]);
					if (reg == -1) {
						fprintf(stderr, "mqdc32: Register addr missing\n");
						exit(1);
					}
					val = -1;
					if (argc >= 7) val = catoi(argv[6]);
					if (val == -1) {
						fprintf(stderr, "mqdc32: Register value missing\n");
						exit(1);
					}
					SET16(m_qdc->md->vmeBase, reg, val);
					printf("madc32: Setting register %x to %x\n", reg, val);
					break;				
		case 'g':	reg = -1;
					if (argc >= 6) reg = catoi(argv[5]);
					if (reg == -1) {
						fprintf(stderr, "mqdc32: Register addr missing\n");
						exit(1);
					}
					val = GET16(m_qdc->md->vmeBase, reg);
					printf("madc32: Register %#x: %#x\n", reg, val);
					break;
		case 'a':	while (1) {
						val = GET16(m_qdc->md->vmeBase, 0x6030);
					}
					break;
		default:	fprintf(stderr, "mqdc32: Illegal function - %s\n", argv[4]);
					exit(1);
	}
	exit(0);
}

void f_ut_send_msg(char * prefix, char * msg, int flag) { printf("%s\n", msg); }
