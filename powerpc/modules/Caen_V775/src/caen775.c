/*____________________________________________________________[C MAIN PROGRAM]
//////////////////////////////////////////////////////////////////////////////
//! \file			madc32.c
//! \brief			test program for Mesytec MADC32
//! \details		Provides some basic tests for MADC32
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

#include "root_env.h"
#include "mapping_database.h"
#include "mapping_protos.h"

#include "caen775_layout.h"
#include "gd_readout.h"

#include "Version.h"

void f_ut_send_msg(char * prefix, char * msg, int flag);

int main(int argc, char *argv[]) {

	int i;
	int version;
	unsigned int physAddr;
	unsigned int addrMod;
	unsigned short srval;
	unsigned long bcval;
	volatile char * madc32;
	unsigned int mappingMod;
	int reg, val;

	s_mapDescr * md;
	
	char fct;
	char file[256];
	FILE * out;

	if (argc <= 4) {
		fprintf(stderr, "caen775: Basic tests for CAEN V775 modules\n\n");
		fprintf(stderr, "Usage: caen775 <addr> <addrMod> <mapMod> [<fct> [<args>]]\n\n");
		fprintf(stderr, "       addr           VME addr, high order bits NNNN (16-31): NNNNXXXX\n");
		fprintf(stderr, "       addrMod        addr modifier, 0x09 (A32) or 0x39 (A24)\n");
		fprintf(stderr, "       mapMod         mapping mode: (bit pattern) 1 (direct, RIO4 only), 2 (static), 4 (dynamic)\n");
		fprintf(stderr, "       fct            function to be executed\n");
		fprintf(stderr, "            v         read firmware version\n");
		fprintf(stderr, "            l         list thresholds\n");
		exit(1);
	}

	physAddr = (unsigned int) strtol(argv[1], NULL, 16);
	physAddr <<= 16;
	addrMod =  (unsigned int) strtol(argv[2], NULL, 16);

	mappingMod = (unsigned int) strtol(argv[3], NULL, 16);
	
	if (mappingMod == 2) smem_remove("caen775");		/* remove previous mappings */

	md = mapVME("caen775", physAddr, 0x10000L, addrMod, mappingMod, 0);
	if (md == NULL) {
		fprintf(stderr, "caen775: Can't map addr %#lx (mod=%#lx)\n", physAddr, addrMod);
		exit(1);
	}

	fct = *argv[4];

	switch (fct) {
		case 'v':	caen_module_info(physAddr, md->vmeBase, addrMod);				/* read module info */
					break;
					
		case 'l':	caen_list_thresh(md->vmeBase);								/* list thresholds */
					break;
					
		default:	fprintf(stderr, "caen775: Illegal function - %s\n", argv[4]);
					exit(1);
	}
	exit(0);
}

void caen_module_info(unsigned long physAddr, volatile unsigned char * vmeAddr, unsigned long addrMod) {
	unsigned short revision;
	unsigned short serialMSB, serialLSB;
	unsigned short boardIdMSB, boardIdLSB;
	int serial, boardId;
	char str[256];
	
	volatile unsigned short * firmWare = CAEN_V775_A_ROM(vmeAddr);

	boardIdMSB = *(firmWare + 0x3A / sizeof(unsigned short)) & 0xFF;
	boardIdLSB = *(firmWare + 0x3E / sizeof(unsigned short)) & 0xFF;
	boardId = (int) (boardIdMSB << 8) + (int) boardIdLSB;
	revision = *(firmWare + 0x4E / sizeof(unsigned short)) & 0xFF;
	serialLSB = *(firmWare + 0xF06 / sizeof(unsigned short)) & 0xFF;
	serialMSB = *(firmWare + 0xF02 / sizeof(unsigned short)) & 0xFF;
	serial = (int) (serialMSB << 8) + (int) serialLSB;
	printf("CAEN module info: addr (phys) %#lx, addr (vme) %#lx, mod %#lx, type V%d, serial# %d, revision %d\n", physAddr, vmeAddr, addrMod, boardId, serial, revision);
	if (boardId != 775) {
		printf("CAEN module info: Illegal board ID %d - should be 775\n", boardId);
	}
}

void caen_list_thresh(volatile unsigned char * vmeAddr) {
	int chn;
	unsigned int thresh;
	printf("\nThresholds:\n");
	for (chn = 0; chn < 32; chn++) {
		thresh = CAEN_V775_GET_THRESHOLD(vmeAddr, chn);
		if (thresh & CAEN_V775_B_THRESH_KILL_CHANNEL) {
			printf("%3d: not active\n", chn);
		} else {
			printf("%3d: %d\n", chn, thresh);
		}
	}
}

void f_ut_send_msg(char * prefix, char * msg, int flag) { printf("%s\n", msg); }
