/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           caen.c
// Purpose:        caen test
// Description:    Connect to caen and read status
//
// Modules:
// Author:         R. Lutter
// Revision:       
// Date:           Jul 2004
// Keywords:       
//
////////////////////////////////////////////////////////////////////////////*/

/* bit definitions for VME ADC CAEN V785 */
#define CAEN_V785_R_OUTPUT						0x0
#define CAEN_V785_R_FIRMWARE						0x1000
#define CAEN_V785_785_R_GEOADDR						0x1002
#define CAEN_V785_R_BITSET1						0x1006
#define CAEN_V785_R_BITCLEAR1					0x1008
#define CAEN_V785_R_STATUS1						0x100E
#define CAEN_V785_R_CONTROL1						0x1010
#define CAEN_V785_R_ADDR_HIGH					0x1012
#define CAEN_V785_R_ADDR_LOW						0x1014
#define CAEN_V785_R_EVT_TRIG						0x1020
#define CAEN_V785_R_STATUS2						0x1022
#define CAEN_V785_R_EVT_CNT_LOW					0x1024
#define CAEN_V785_R_EVT_CNT_HIGH 				0x1026
#define CAEN_V785_R_INCR_EVT						0x1028
#define CAEN_V785_R_INCR_OFFS					0x102A
#define CAEN_V785_R_BITSET2						0x1032
#define CAEN_V785_R_BITCLEAR2					0x1034
#define CAEN_V785_R_CRATE_SEL					0x103C
#define CAEN_V785_R_TEST_EVT_WRITE				0x103E
#define CAEN_V785_R_EVT_CNT_RESET				0x1040
#define CAEN_V785_R_THRESH 						0x1080
#define CAEN_V785_R_ROM	 						0x8000

#define CAEN_V785_B_BITSET1_SEL_ADDR_INTERN 		(0x1 << 4)
#define CAEN_V785_B_BITSET1_RESET			 		(0x1 << 7)
#define CAEN_V785_B_CONTROL1_PROG_RESET_MODULE	(0x1 << 4)
#define CAEN_V785_B_BITSET2_OFFLINE				(0x1 << 1)
#define CAEN_V785_B_BITSET2_CLEAR_DATA		(0x1 << 2)
#define CAEN_V785_B_BITSET2_OVER_RANGE_DIS		(0x1 << 3)
#define CAEN_V785_B_BITSET2_LOW_THRESH_DIS		(0x1 << 4)
#define CAEN_V785_B_BITSET2_SLIDE_ENA			(0x1 << 7)
#define CAEN_V785_B_BITSET2_STEP_TH 			(0x1 << 8)
#define CAEN_V785_B_BITSET2_AUTO_INCR_ENA 		(0x1 << 11)
#define CAEN_V785_B_BITSET2_EMPTY_PROG_ENA		(0x1 << 12)
#define CAEN_V785_B_BITSET2_ALL_TRIG_ENA			(0x1 << 14)
#define CAEN_V785_B_STATUS2_BUFFER_EMPTY			(0x1 << 1)
#define CAEN_V785_B_THRESH_KILL_CHANNEL 			(0x1 << 8)

#define CAEN_V785_SH_WC							8

#define CAEN_V785_D_HDR							0x02000000
#define CAEN_V785_D_DATA							0x00000000
#define CAEN_V785_D_EOB							0x04000000
#define CAEN_V785_D_INVALID						0x06000000

#define CAEN_V785_M_MSERIAL						0xFF
#define CAEN_V785_M_WC							0x3F
#define CAEN_V785_M_CHN							0xFF
#define CAEN_V785_M_ID							0x07000000
#define CAEN_V785_M_HDR							0x07FFFFFF
#define CAEN_V785_M_DATA						0x07FFFFFF
#define CAEN_V785_M_ADCDATA						0x00000FFF

#define CAEN_V785_B_OVERFLOW	 				(0x1 << 12)
#define CAEN_V785_B_UNDERTHRESH 				(0x1 << 13)

#define CAEN_V785_N_MAXEVENTS					32

#define CAEN_V785_A_OUTPUT(module)			((unsigned int *) (module + CAEN_V785_R_OUTPUT))
#define CAEN_V785_A_FIRMWARE(module) 		((unsigned short *) (module + CAEN_V785_R_FIRMWARE))
#define CAEN_V785_A_GEOADDR(module)			((unsigned short *) (module + CAEN_V785_R_GEOADDR))
#define CAEN_V785_A_BITSET1(module)			((unsigned short *) (module + CAEN_V785_R_BITSET1))
#define CAEN_V785_A_BITCLEAR1(module)		((unsigned short *) (module + CAEN_V785_R_BITCLEAR1))
#define CAEN_V785_A_STATUS1(module)			((unsigned short *) (module + CAEN_V785_R_STATUS1))
#define CAEN_V785_A_CONTROL1(module) 		((unsigned short *) (module + CAEN_V785_R_CONTROL1))
#define CAEN_V785_A_ADDR_HIGH(module) 		((unsigned short *) (module + CAEN_V785_R_ADDR_HIGH))
#define CAEN_V785_A_ADDR_LOW(module)			((unsigned short *) (module + CAEN_V785_R_ADDR_LOW))
#define CAEN_V785_A_EVT_TRIG(module)			((unsigned short *) (module + CAEN_V785_R_EVT_TRIG))
#define CAEN_V785_A_STATUS2(module)			((unsigned short *) (module + CAEN_V785_R_STATUS2))
#define CAEN_V785_A_EVT_CNT_LOW(module)		((unsigned short *) (module + CAEN_V785_R_EVT_CNT_LOW))
#define CAEN_V785_A_EVT_CNT_HIGH(module) 	((unsigned short *) (module + CAEN_V785_R_EVT_CNT_HIGH))
#define CAEN_V785_A_INCR_EVT(module)			((unsigned short *) (module + CAEN_V785_R_INCR_EVT))
#define CAEN_V785_A_INCR_OFFS(module) 		((unsigned short *) (module + CAEN_V785_R_INCR_OFFS))
#define CAEN_V785_A_BITSET2(module)			((unsigned short *) (module + CAEN_V785_R_BITSET2))
#define CAEN_V785_A_BITCLEAR2(module)		((unsigned short *) (module + CAEN_V785_R_BITCLEAR2))
#define CAEN_V785_A_CRATE_SEL(module) 		((unsigned short *) (module + CAEN_V785_R_CRATE_SEL))
#define CAEN_V785_A_TEST_EVT_WRITE(module)	((unsigned short *) (module + CAEN_V785_R_TEST_EVT_WRITE))
#define CAEN_V785_A_EVT_CNT_RESET(module)	((unsigned short *) (module + CAEN_V785_R_EVT_CNT_RESET))
#define CAEN_V785_A_THRESH(module)			((unsigned short *) (module + CAEN_V785_R_THRESH))
#define CAEN_V785_A_ROM(module)				((unsigned short *) (module + CAEN_V785_R_ROM))

#define CAEN_V785_SET_THRESHOLD(module,chn,data) 	*(CAEN_V785_A_THRESH(module) + chn) = data

#define CAEN_V785_AUTO_INCR_ON(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_AUTO_INCR_ENA
#define CAEN_V785_AUTO_INCR_OFF(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_AUTO_INCR_ENA

#define CAEN_V785_ZERO_SUPPR_ON(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_LOW_THRESH_DIS
#define CAEN_V785_ZERO_SUPPR_OFF(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_LOW_THRESH_DIS

#define CAEN_V785_ALL_TRIG_ON(module)		*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_ALL_TRIG_ENA
#define CAEN_V785_ALL_TRIG_OFF(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_ALL_TRIG_ENA

#define CAEN_V785_THRESH_RES_COARSE(module) *CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_STEP_TH
#define CAEN_V785_THRESH_RES_FINE(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_STEP_TH

#define CAEN_V785_SLIDING_SCALE_ON(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_SLIDE_ENA
#define CAEN_V785_SLIDING_SCALE_OFF(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_SLIDE_ENA

#define CAEN_V785_OVER_RANGE_CHECK_ON(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_OVER_RANGE_DIS
#define CAEN_V785_OVER_RANGE_CHECK_OFF(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_OVER_RANGE_DIS

#define CAEN_V785_EMPTY_PROG_ENABLE(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_EMPTY_PROG_ENA
#define CAEN_V785_EMPTY_PROG_DISABLE(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_EMPTY_PROG_ENA

#define CAEN_V785_CLEAR_DATA(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_CLEAR_DATA; \
											*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_CLEAR_DATA

#define CAEN_V785_RESET_EVENT_COUNTER(module)	*CAEN_V785_A_EVT_CNT_RESET(module) = 1

#define CAEN_V785_SET_KILL_BIT(module,chn)	*(CAEN_V785_A_THRESH(module) + chn) = CAEN_V785_B_THRESH_KILL_CHANNEL

#define CAEN_V785_READ_BITSET2(module)	*CAEN_V785_A_BITSET2(module)

/* bit definitions for VME ADC CAEN V775 */
#define CAEN_V775_R_OUTPUT						0x0
#define CAEN_V775_R_FIRMWARE						0x1000
#define CAEN_V775_775_R_GEOADDR						0x1002
#define CAEN_V775_R_BITSET1						0x1006
#define CAEN_V775_R_BITCLEAR1					0x1008
#define CAEN_V775_R_STATUS1						0x100E
#define CAEN_V775_R_CONTROL1						0x1010
#define CAEN_V775_R_ADDR_HIGH					0x1012
#define CAEN_V775_R_ADDR_LOW						0x1014
#define CAEN_V775_R_EVT_TRIG						0x1020
#define CAEN_V775_R_STATUS2						0x1022
#define CAEN_V775_R_EVT_CNT_LOW					0x1024
#define CAEN_V775_R_EVT_CNT_HIGH 				0x1026
#define CAEN_V775_R_INCR_EVT						0x1028
#define CAEN_V775_R_INCR_OFFS					0x102A
#define CAEN_V775_R_BITSET2						0x1032
#define CAEN_V775_R_BITCLEAR2					0x1034
#define CAEN_V775_R_CRATE_SEL					0x103C
#define CAEN_V775_R_TEST_EVT_WRITE				0x103E
#define CAEN_V775_R_EVT_CNT_RESET				0x1040
#define CAEN_V775_R_FULL_SCALE_RANGE			0x1060
#define CAEN_V775_R_THRESH 						0x1080
#define CAEN_V775_R_ROM	 						0x8000

#define CAEN_V775_B_BITSET1_SEL_ADDR_INTERN 		(0x1 << 4)
#define CAEN_V775_B_BITSET1_RESET			 		(0x1 << 7)
#define CAEN_V775_B_CONTROL1_PROG_RESET_MODULE	(0x1 << 4)
#define CAEN_V775_B_BITSET2_OFFLINE				(0x1 << 1)
#define CAEN_V775_B_BITSET2_CLEAR_DATA		(0x1 << 2)
#define CAEN_V775_B_BITSET2_OVER_RANGE_DIS		(0x1 << 3)
#define CAEN_V775_B_BITSET2_LOW_THRESH_DIS		(0x1 << 4)
#define CAEN_V775_B_BITSET2_VALID_CONTROL_DIS	(0x1 << 5)
#define CAEN_V775_B_BITSET2_SLIDE_ENA			(0x1 << 7)
#define CAEN_V775_B_BITSET2_COMMON_START_DIS 	(0x1 << 10)
#define CAEN_V775_B_BITSET2_AUTO_INCR_ENA 		(0x1 << 11)
#define CAEN_V775_B_BITSET2_EMPTY_PROG_ENA		(0x1 << 12)
#define CAEN_V775_B_BITSET2_ALL_TRIG_ENA			(0x1 << 14)
#define CAEN_V775_B_STATUS2_BUFFER_EMPTY			(0x1 << 1)
#define CAEN_V775_B_THRESH_KILL_CHANNEL 			(0x1 << 8)

#define CAEN_V775_SH_WC							8

#define CAEN_V775_D_HDR							0x02000000
#define CAEN_V775_D_DATA							0x00000000
#define CAEN_V775_D_EOB							0x04000000
#define CAEN_V775_D_INVALID						0x06000000

#define CAEN_V775_M_MSERIAL						0xFF
#define CAEN_V775_M_WC							0x3F
#define CAEN_V775_M_CHN							0xFF
#define CAEN_V775_M_ID							0x07000000
#define CAEN_V775_M_HDR							0x07FFFFFF
#define CAEN_V775_M_DATA							0x07FFFFFF
#define CAEN_V775_M_ADCDATA						0x00000FFF

#define CAEN_V775_B_OVERFLOW	 				(0x1 << 12)
#define CAEN_V775_B_UNDERTHRESH 				(0x1 << 13)

#define CAEN_V775_N_MAXEVENTS					32

#define CAEN_V775_A_OUTPUT(module)			((unsigned int *) (module + CAEN_V775_R_OUTPUT))
#define CAEN_V775_A_FIRMWARE(module) 		((unsigned short *) (module + CAEN_V775_R_FIRMWARE))
#define CAEN_V775_A_GEOADDR(module)			((unsigned short *) (module + CAEN_V775_R_GEOADDR))
#define CAEN_V775_A_BITSET1(module)			((unsigned short *) (module + CAEN_V775_R_BITSET1))
#define CAEN_V775_A_BITCLEAR1(module)		((unsigned short *) (module + CAEN_V775_R_BITCLEAR1))
#define CAEN_V775_A_STATUS1(module)			((unsigned short *) (module + CAEN_V775_R_STATUS1))
#define CAEN_V775_A_CONTROL1(module) 		((unsigned short *) (module + CAEN_V775_R_CONTROL1))
#define CAEN_V775_A_ADDR_HIGH(module) 		((unsigned short *) (module + CAEN_V775_R_ADDR_HIGH))
#define CAEN_V775_A_ADDR_LOW(module)			((unsigned short *) (module + CAEN_V775_R_ADDR_LOW))
#define CAEN_V775_A_EVT_TRIG(module)			((unsigned short *) (module + CAEN_V775_R_EVT_TRIG))
#define CAEN_V775_A_STATUS2(module)			((unsigned short *) (module + CAEN_V775_R_STATUS2))
#define CAEN_V775_A_EVT_CNT_LOW(module)		((unsigned short *) (module + CAEN_V775_R_EVT_CNT_LOW))
#define CAEN_V775_A_EVT_CNT_HIGH(module) 	((unsigned short *) (module + CAEN_V775_R_EVT_CNT_HIGH))
#define CAEN_V775_A_INCR_EVT(module)			((unsigned short *) (module + CAEN_V775_R_INCR_EVT))
#define CAEN_V775_A_INCR_OFFS(module) 		((unsigned short *) (module + CAEN_V775_R_INCR_OFFS))
#define CAEN_V775_A_BITSET2(module)			((unsigned short *) (module + CAEN_V775_R_BITSET2))
#define CAEN_V775_A_BITCLEAR2(module)		((unsigned short *) (module + CAEN_V775_R_BITCLEAR2))
#define CAEN_V775_A_CRATE_SEL(module) 		((unsigned short *) (module + CAEN_V775_R_CRATE_SEL))
#define CAEN_V775_A_TEST_EVT_WRITE(module)	((unsigned short *) (module + CAEN_V775_R_TEST_EVT_WRITE))
#define CAEN_V775_A_EVT_CNT_RESET(module)	((unsigned short *) (module + CAEN_V775_R_EVT_CNT_RESET))
#define CAEN_V775_A_FULL_SCALE_RANGE(module)	((unsigned short *) (module + CAEN_V775_R_FULL_SCALE_RANGE))
#define CAEN_V775_A_THRESH(module)			((unsigned short *) (module + CAEN_V775_R_THRESH))
#define CAEN_V775_A_ROM(module)				((unsigned short *) (module + CAEN_V775_R_ROM))

#define CAEN_V775_SET_THRESHOLD(module,chn,data) 	*(CAEN_V775_A_THRESH(module) + chn) = data

#define CAEN_V775_SET_FULL_SCALE_RANGE(module,data) 	*CAEN_V775_A_FULL_SCALE_RANGE(module) = data

#define CAEN_V775_AUTO_INCR_ON(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_AUTO_INCR_ENA
#define CAEN_V775_AUTO_INCR_OFF(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_AUTO_INCR_ENA

#define CAEN_V775_ZERO_SUPPR_ON(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_LOW_THRESH_DIS
#define CAEN_V775_ZERO_SUPPR_OFF(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_LOW_THRESH_DIS

#define CAEN_V775_VALID_CONTROL_ON(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_VALID_CONTROL_DIS
#define CAEN_V775_VALID_CONTROL_OFF(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_VALID_CONTROL_DIS

#define CAEN_V775_COMMON_START_ON(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_COMMON_START_DIS
#define CAEN_V775_COMMON_STOP_ON(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_COMMON_START_DIS

#define CAEN_V775_ALL_TRIG_ON(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_ALL_TRIG_ENA
#define CAEN_V775_ALL_TRIG_OFF(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_ALL_TRIG_ENA

#define CAEN_V775_SLIDING_SCALE_ON(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_SLIDE_ENA
#define CAEN_V775_SLIDING_SCALE_OFF(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_SLIDE_ENA

#define CAEN_V775_OVER_RANGE_CHECK_ON(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_OVER_RANGE_DIS
#define CAEN_V775_OVER_RANGE_CHECK_OFF(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_OVER_RANGE_DIS

#define CAEN_V775_CLEAR_DATA(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_CLEAR_DATA; \
											*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_CLEAR_DATA

#define CAEN_V775_RESET_EVENT_COUNTER(module)	*CAEN_V775_A_EVT_CNT_RESET(module) = 1

#define CAEN_V775_SET_KILL_BIT(module,chn)	*(CAEN_V775_A_THRESH(module) + chn) = CAEN_V775_B_THRESH_KILL_CHANNEL

#define CAEN_V775_READ_BITSET2(module)	*CAEN_V775_A_BITSET2(module)

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "vmelib.h"

void caen_module_info(unsigned long physAddr, volatile unsigned char * vmeAddr, unsigned long addrMod);

static struct pdparam_master s_param; 		/* vme segment params */

extern volatile unsigned long * caen_baseAddr;

static volatile unsigned long * addr0;		/* crate#1, N(0).A(0).F(0) */
static volatile unsigned long * sr; 		/* status register: N(0) */

int main(int argc, char *argv[]) {

	int i;
	int version;
	unsigned int physAddr;
	unsigned int addrMod;
	unsigned short srval;
	unsigned long bcval;
	volatile char * caen;

	if (argc <= 1) {
		fprintf(stderr, "caen: Read status of CAEN modules\nUsage: sis <addr> [<mod=0x39>]\n");
		exit(1);
	}
	physAddr = strtol(argv[1], NULL, 16);
	if (argc > 2) addrMod = strtol(argv[2], NULL, 16); else addrMod = 0x39;

	s_param.iack = 1;						/* prepare vme segment */
 	s_param.rdpref = 0;
 	s_param.wrpost = 0;
 	s_param.swap = SINGLE_AUTO_SWAP;
 	s_param.dum[0] = 0; 					/* forces static mapping! */

 	caen = (volatile char *) find_controller(physAddr, 0x10000L, addrMod, 0, 0, &s_param);
	if (caen == (volatile char *) -1) {
		fprintf(stderr, "caen: Can't map addr %#lx (mod=%#lx)\n", physAddr, addrMod);
		exit(1);
	}
	caen_module_info(physAddr, caen, addrMod);
}

void caen_module_info(unsigned long physAddr, volatile unsigned char * vmeAddr, unsigned long addrMod) {
	unsigned short revision;
	unsigned short serialMSB, serialLSB;
	unsigned short boardIdMSB, boardIdLSB;
	int serial, boardId;
	char str[256];
	
	volatile unsigned short * firmWare = CAEN_V785_A_ROM(vmeAddr);

	boardIdMSB = *(firmWare + 0x3A / sizeof(unsigned short)) & 0xFF;
	boardIdLSB = *(firmWare + 0x3E / sizeof(unsigned short)) & 0xFF;
	boardId = (int) (boardIdMSB << 8) + (int) boardIdLSB;
	revision = *(firmWare + 0x4E / sizeof(unsigned short)) & 0xFF;
	serialLSB = *(firmWare + 0xF06 / sizeof(unsigned short)) & 0xFF;
	serialMSB = *(firmWare + 0xF02 / sizeof(unsigned short)) & 0xFF;
	serial = (int) (serialMSB << 8) + (int) serialLSB;
	printf("CAEN module info: addr (phys) %#lx, addr (vme) %#lx, mod %#lx, type V%d, serial# %d, revision %d\n", physAddr, vmeAddr, addrMod, boardId, serial, revision);
	if (boardId != 785 && boardId != 775 && boardId != 965) {
		printf("CAEN module info: Illegal board ID %d - should be 785\n", boardId);
	}
}
