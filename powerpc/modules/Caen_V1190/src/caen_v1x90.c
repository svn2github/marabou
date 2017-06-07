/*____________________________________________________________[C MAIN PROGRAM]
//////////////////////////////////////////////////////////////////////////////
//! \file			caen_v1x90.c
//! \brief			test program for Mesytec caen_v1x90
//! \details		Provides some basic tests for caen_v1x90
//!
//! $Author: Marabou $
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.1 $       
//! $Date: 2009-07-28 08:34:57 $
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allParam.h>
#include <ces/bmalib.h>
#include <errno.h>

#include "vmelib.h"

#include "caen_v1x90.h"
#include "caen_v1x90_protos.h"

#include "root_env.h"
#include "gd_readout.h"

#include "Version.h"

static struct pdparam_master s_param; 		/* vme segment params */
static s_caen_v1x90 * s_tdc;

void f_ut_send_msg(char * prefix, char * msg, int flag);
void read_micro_reg(s_caen_v1x90 * s, char *argv[]);
void write_micro_reg(s_caen_v1x90 * s, char *argv[]);

int main(int argc, char *argv[]) {

	int i;
	int version;
	unsigned int physAddr;
	unsigned int addrMod;
	unsigned short content;
	unsigned short srval;
	unsigned long bcval;
	volatile char * caen_v1x90;
	int offset, cnt;
	unsigned short data;
	
	char file[256];
	FILE * out;

	if (argc <= 2) {
		fprintf(stderr, "caen_v1x90: Basic tests for CAEN V1190 tdc modules\n\n");
		fprintf(stderr, "Usage: caen_v1x90 <addr> [<fct> [<args>]]\n\n");
		fprintf(stderr, "       addr                    VME addr, high order bits 17-31, A32 or A24\n");
		fprintf(stderr, "       fct                     function to be executed\n");
		fprintf(stderr, "            v                  read firmware version\n");
		fprintf(stderr, "            r                  reset module\n");
		fprintf(stderr, "            c                  soft clear\n");
		fprintf(stderr, "            rr reg             read micro controller register <reg>\n");
		fprintf(stderr, "            wr reg data        write <data> to micro controller register <reg>\n");
		fprintf(stderr, "            ra offset cnt      read at <addr + offset> <cnt> times\n");
		fprintf(stderr, "            wa offset data cnt write at <addr + offset> <data> <cnt> times\n");
		fprintf(stderr, "            hs                 test handshake\n");
		fprintf(stderr, "            rd                 read data from micro controller\n");
		fprintf(stderr, "            wd data            write data to micro controller\n");
		fprintf(stderr, "            l  file            load settings from file <file>\n");
		fprintf(stderr, "            d  file            dump settings to file <file>\n");
		fprintf(stderr, "            p                  print settings\n\n");
		exit(1);
	}

	physAddr = (unsigned int) strtol(argv[1], NULL, 0);
	if (physAddr & 0xFF00) addrMod = 0x9; else addrMod = 0x39;
	physAddr <<= 16;

	s_param.iack = 1;						/* prepare vme segment */
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					/* forces static mapping! */

 	caen_v1x90 = (volatile char *) find_controller(physAddr, 0x10000L, addrMod, 0, 0, &s_param);
	if (caen_v1x90 == (volatile char *) -1) {
		fprintf(stderr, "caen_v1x90: Can't map addr %#lx (mod=%#lx)\n", physAddr, addrMod);
		exit(1);
	}

	s_tdc = caen_v1x90_alloc(physAddr, caen_v1x90, "caen_v1x90", 0xBB);	/* allocate data struct */
	if (s_tdc == NULL) exit(1);

	caen_v1x90_setPrefix(s_tdc, "caen_v1x90");

	file[0] = '\0';
	if (argc >= 4) strcpy(file, argv[3]);

	if (strcmp(argv[2], "v") == 0) {
		caen_v1x90_moduleInfo(s_tdc);				/* read module info */
	} else if (strcmp(argv[2], "r") == 0) {
		caen_v1x90_reset(s_tdc);					/* reset module */
	} else if (strcmp(argv[2], "c") == 0) {
		caen_v1x90_softClear(s_tdc);				/* clear module */
	} else if (strcmp(argv[2], "ra") == 0) {
		offset = (unsigned int) strtol(argv[3], NULL, 0);
		cnt = atoi(argv[4]);
		for (i = 0; i < cnt; i++) content = GET_DATA(s_tdc, offset);
		printf("Content @ %#x: %#x\n", s_tdc->baseAddr + offset, content);
	} else if (strcmp(argv[2], "wa") == 0) {
		offset = (unsigned int) strtol(argv[3], NULL, 0);
		content = (unsigned int) strtol(argv[4], NULL, 0);
		cnt = atoi(argv[5]);
		for (i = 0; i < cnt; i++) SET_DATA(s_tdc, offset, content);
	} else if (strcmp(argv[2], "l") == 0) {
		if (file[0] == '\0') {
			fprintf(stderr, "caen_v1x90: File name missing\n");
			exit(1);
		}
		caen_v1x90_fillStruct(s_tdc, file);			/* fill struct from environment data */
		caen_v1x90_loadFromDb(s_tdc); 				/* load module regs from database */
	} else if (strcmp(argv[2], "d") == 0) {
		if (file[0] == '\0') {
			fprintf(stderr, "caen_v1x90: File name missing\n");
			exit(1);
		}
		s_tdc->dumpRegsOnInit = TRUE;
		caen_v1x90_dumpRegisters(s_tdc, file); 		/* dump registers to file */
	} else if (strcmp(argv[2], "p") == 0) {
		s_tdc->dumpRegsOnInit = TRUE;
		caen_v1x90_dumpRegisters(s_tdc, NULL); 		/* print registers */
	} else if (strcmp(argv[2], "rr") == 0) {
		read_micro_reg(s_tdc, argv);
	} else if (strcmp(argv[2], "wr") == 0) {
		write_micro_reg(s_tdc, argv);
	} else if (strcmp(argv[2], "hs") == 0) {
		data = MICRO_HSHAKE(s_tdc);
		printf("handshake = %#x\n", data);
	} else if (strcmp(argv[2], "rd") == 0) {
		data = GET_MICRO_DATA(s_tdc);
		printf("data = %#x\n", data);
	} else if (strcmp(argv[2], "wd") == 0) {
		data = strtol(argv[3], NULL, 0);
		SET_MICRO_DATA(s_tdc, data);
	} else {
		fprintf(stderr, "caen_v1x90: Illegal function - %s\n", argv[2]);
		exit(1);
	}
	exit(0);
}

void f_ut_send_msg(char * prefix, char * msg, int flag) { printf("%s\n", msg); }

void read_micro_reg(s_caen_v1x90 * s, char *argv[]) {

	int i;
	char reg[256];

	strcpy(reg, argv[3]);

	if (strcmp(reg, "tm") == 0) {
		printf("Trigger matching      : %s\n", (caen_v1x90_triggerMatchingIsOn(s) ? "On" : "Off"));
	} else if (strcmp(reg, "ut") == 0) {
		caen_v1x90_readTriggerSettings(s);
		printf("Trigger conf updated\n");
	} else if (strcmp(reg, "ww") == 0) {
		caen_v1x90_readTriggerSettings(s);
		printf("Window width          : %#x\n", caen_v1x90_getWindowWidth(s));
	} else if (strcmp(reg, "wo") == 0) {
		caen_v1x90_readTriggerSettings(s);
		printf("Window offset         : %#x\n", caen_v1x90_getWindowOffset(s));
	} else if (strcmp(reg, "sm") == 0) {
		caen_v1x90_readTriggerSettings(s);
		printf("Search margin         : %#x\n", caen_v1x90_getSearchMargin(s));
	} else if (strcmp(reg, "rm") == 0) {
		caen_v1x90_readTriggerSettings(s);
		printf("Reject margin         : %#x\n", caen_v1x90_getRejectMargin(s));
	} else if (strcmp(reg, "st") == 0) {
		caen_v1x90_readTriggerSettings(s);
		printf("Subtract trigger time : %s\n", (caen_v1x90_triggerTimeSubtractionEnabled(s) ? "Yes" : "No"));
	} else if (strcmp(reg, "ur") == 0) {
		caen_v1x90_readResolutionSettings(s);
		printf("Resolution updated\n");
	} else if (strcmp(reg, "ed") == 0) {
		caen_v1x90_readResolutionSettings(s);
		printf("Edge detection        : %#x\n", caen_v1x90_getEdgeDetection(s));
	} else if (strcmp(reg, "er") == 0) {
		caen_v1x90_readResolutionSettings(s);
		printf("Edge resolution       : %#x\n", caen_v1x90_getEdgeResolution(s));
	} else if (strcmp(reg, "pr") == 0) {
		caen_v1x90_readResolutionSettings(s);
		printf("Pair resolution       : %#x\n", caen_v1x90_getPairResolution(s));
		printf("Pair width            : %#x\n", caen_v1x90_getPairWidth(s));
	} else if (strcmp(reg, "dt") == 0) {
		printf("Dead time             : %#x\n", caen_v1x90_getDeadTime(s));
	} else if (strcmp(reg, "es") == 0) {
		printf("Event size            : %#x\n", caen_v1x90_getEventSize(s));
	} else if (strcmp(reg, "fs") == 0) {
		printf("Fifo size             : %#x\n", caen_v1x90_getFifoSize(s));
	} else if (strcmp(reg, "ht") == 0) {
		printf("Write header/trailer  : %s\n", (caen_v1x90_headerTrailerEnabled(s) ? "Yes" : "No"));
	} else if (strcmp(reg, "tt") == 0) {
		printf("Write ext trigger tag : %s\n", (caen_v1x90_extendedTriggerTagEnabled(s) ? "Yes" : "No"));
	} else if (strcmp(reg, "af") == 0) {
		printf("Almost full level     : %d\n", caen_v1x90_getAlmostFullLevel(s));
	} else if (strcmp(reg, "ch") == 0) {
		caen_v1x90_getChannelEnabled(s);
		printf("Channel pattern       :\n");
		for (i = 0; i < NOF_CHANNEL_WORDS; i++) {
			printf("         %3d - %3d    : %#x\n", (i * 16), ((i + 1) * 16 - 1), s->channels[i] & 0xFFFF);
		}
	} else {
		fprintf(stderr, "caen_v1x90: Wrong register name - %s\n", reg);
	}
}

void write_micro_reg(s_caen_v1x90 * s, char *argv[]) {

	int i;
	unsigned short res, width, data;
	char reg[256];

	strcpy(reg, argv[3]);
	data = strtol(argv[4], NULL, 0);

	if (strcmp(reg, "tm") == 0) {
		caen_v1x90_setTriggerMatchingOn(s, (data == 0) ? FALSE : TRUE);
	} else if (strcmp(reg, "ww") == 0) {
		caen_v1x90_setWindowWidth(s, data);
	} else if (strcmp(reg, "wo") == 0) {
		caen_v1x90_setWindowOffset(s, data);
	} else if (strcmp(reg, "sm") == 0) {
		caen_v1x90_setSearchMargin(s, data);
	} else if (strcmp(reg, "rm") == 0) {
		caen_v1x90_setRejectMargin(s, data);
	} else if (strcmp(reg, "st") == 0) {
		caen_v1x90_enableTriggerTimeSubtraction(s, (data == 0) ? FALSE : TRUE);
	} else if (strcmp(reg, "rs") == 0) {
		res = strtol(argv[5], NULL, 0);
		if (data == 0) {
			width = strtol(argv[6], NULL, 0);
			caen_v1x90_setResolution(s, data, res, width);
		} else {
			caen_v1x90_setResolution(s, data, res, NO_ARG);
		}
	} else if (strcmp(reg, "dt") == 0) {
		caen_v1x90_setDeadTime(s, data);
	} else if (strcmp(reg, "es") == 0) {
		caen_v1x90_setEventSize(s, data);
	} else if (strcmp(reg, "fs") == 0) {
		caen_v1x90_setFifoSize(s, data);
	} else if (strcmp(reg, "ht") == 0) {
		caen_v1x90_enableHeaderTrailer(s, (data == 0) ? FALSE : TRUE);
	} else if (strcmp(reg, "tt") == 0) {
		caen_v1x90_enableExtendedTriggerTag(s, (data == 0) ? FALSE : TRUE);
	} else if (strcmp(reg, "af") == 0) {
		caen_v1x90_setAlmostFullLevel(s, data);
	} else {
		fprintf(stderr, "caen_v1x90: Wrong register name - %s\n", reg);
	}
}
