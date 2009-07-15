#ifndef __CAEN_V1190_H__
#define __CAEN_V1190_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <allParam.h>
#include <ces/bmalib.h>

#include "block.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			caen_v1190.h
//! \brief			Definitions for CAEN V1190 tdc
//! \details		Contains definitions for a CAEN V1190 tdc
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.4 $
//! $Date: 2009-07-15 11:45:36 $
////////////////////////////////////////////////////////////////////////////*/

#define BIT(n)	(1 << n)

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			register map
////////////////////////////////////////////////////////////////////////////*/

/*!
  This file contains register definitions for the CAEN V1190, V1290 TDC
  modules.  We will assume that the module will be acessed via VmePtrs:
  - The module address space is divided into 4 segments:
    - The Output buffer.
    - The Control registers.
    - The Configuration ROM
    - The compensation SRAM.
  - We will define the offsets of each of these segments relative to the
    module base address as byte offsets.
  - We will Define offsets within each region as byte offsets from the segment
    base (e.g. the InterruptVector register offset will be 0xc not 0x100c.

*/

#define CAEN_V1190_A_OUTPUTBUFFER			0x0
#define CAEN_V1190_S_OUTPUTBUFFERSIZE		0x1000
#define CAEN_V1190_A_REGISTERS				0x1000
#define CAEN_V1190_S_REGISTERSIZE			0x1300
#define CAEN_V1190_A_CONFIGROM				0x4000
#define CAEN_V1190_S_CONFIGROMSIZE			0x0200
#define CAEN_V1190_A_COMPENSATIONSRAM		0x8000
#define CAEN_V1190_S_COMPENSATIONSRAMSIZE	0x0200

/*!	Offsets within to register page.  These are prefixed
	W for word sized registers and L for longword sized registers. */

#define CAEN_V1190_A_CONTROLREGISTER		0x1000
#define CAEN_V1190_A_STATUSREGISTER			0x1002
#define CAEN_V1190_A_INTERRUPTLEVEL			0x100A
#define CAEN_V1190_A_INTERRUPTVECTOR		0x100C
#define CAEN_V1190_A_VIRTUALSLOT			0x100E
#define CAEN_V1190_A_MULTICASTBASE			0x1010
#define CAEN_V1190_A_MULTICASTCONTROL 		0x1012
#define CAEN_V1190_A_RESET					0x1014
#define CAEN_V1190_A_CLEAR					0x1016
#define CAEN_V1190_A_EVENTRESET				0x1018
#define CAEN_V1190_A_SWTRIGGER				0x101A
#define CAEN_V1190_A_EVENTCOUNTER 			0x101C
#define CAEN_V1190_A_EVENTSTORED			0x1020
#define CAEN_V1190_A_ALMOSTFULLLEVEL		0x1022
#define CAEN_V1190_A_BLTEVENTNUMBER			0x1024
#define CAEN_V1190_A_FIRMWAREREVISION 		0x1026
#define CAEN_V1190_A_TESTREGISTER 			0x1028
#define CAEN_V1190_A_OUTPUTCONTROL			0x102C
#define CAEN_V1190_A_MICRODATA				0x102E
#define CAEN_V1190_A_MICROHANDSHAKE			0x1030
#define CAEN_V1190_A_SELECTFLASH			0x1032
#define CAEN_V1190_A_FLASHMEMORY			0x1034
#define CAEN_V1190_A_SRAMPAGE 				0x1036
#define CAEN_V1190_A_EVENTFIFO				0x1038
#define CAEN_V1190_A_EVENTFIFOSTORED		0x103C
#define CAEN_V1190_A_EVENTFIFOSTATUS		0x103E
#define CAEN_V1190_A_DUMMY32				0x1200
#define CAEN_V1190_A_DUMMY16				0x1204

/*!	Offsets within the configuration prom.  Note that while these locations
	all accept D16 accesses, usually, only the bottom byte has useful
	information.  The top byte will usually be indeterminate.	*/

#define CAEN_V1190_CPROM_CHECKSUM 			0x00
#define CAEN_V1190_CPROM_CHECKSUM_LENGTH2 	0x04
#define CAEN_V1190_CPROM_CHECKSUM_LENGTH1 	0x08
#define CAEN_V1190_CPROM_CHECKSUM_LENGTH0 	0x0c
#define CAEN_V1190_CPROM_CONSTANT2			0x10
#define CAEN_V1190_CPROM_CONSTANT1			0x14
#define CAEN_V1190_CPROM_CONSTANT0			0x18
#define CAEN_V1190_CPROM_C_CODE				0x1c
#define CAEN_V1190_CPROM_R_CODE				0x20
#define CAEN_V1190_CPROM_OUI2 				0x24
#define CAEN_V1190_CPROM_OUI1 				0x28
#define CAEN_V1190_CPROM_OUI0 				0x2c
#define CAEN_V1190_CPROM_BOARDVERSION 		0x30
#define CAEN_V1190_CPROM_MODELNUMBER2 		0x34
#define CAEN_V1190_CPROM_MODELNUMBER1 		0x38
#define CAEN_V1190_CPROM_MODELNUMBER0 		0x3c
#define CAEN_V1190_CPROM_REVISION3			0x40
#define CAEN_V1190_CPROM_REVISION2			0x44
#define CAEN_V1190_CPROM_REVISION1			0x48
#define CAEN_V1190_CPROM_REVISION0			0x4c
#define CAEN_V1190_CPROM_SERIALNUMBER1		0x80
#define CAEN_V1190_CPROM_SERIALNUMBER0		0x84

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			error bits
////////////////////////////////////////////////////////////////////////////*/

#define CAEN_V1190_B_CREG_BERREN				BIT(0)
#define CAEN_V1190_B_CREG_TERM					BIT(1)
#define CAEN_V1190_B_CREG_TERM_SW				BIT(2)
#define CAEN_V1190_B_CREG_EMPTY_EVENT			BIT(3)
#define CAEN_V1190_B_CREG_ALIGN64				BIT(4)
#define CAEN_V1190_B_CREG_COMPENSATION_ENABLE	BIT(5)
#define CAEN_V1190_B_CREG_TEST_FIFO_ENABLE		BIT(6)
#define CAEN_V1190_B_CREG_READ_SRAM_ENABLE		BIT(7)
#define CAEN_V1190_B_CREG_EVENT_FIFO_ENABLE		BIT(8)
#define CAEN_V1190_B_CREG_TRIGGER_TAG_ENABLE	BIT(9)

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			status register bits
////////////////////////////////////////////////////////////////////////////*/

#define CAEN_V1190_B_DATA_READY 				BIT(0)
#define CAEN_V1190_B_ALM_FULL 					BIT(1)
#define CAEN_V1190_B_FULL 						BIT(2)
#define CAEN_V1190_B_TRG_MATCH 					BIT(3)
#define CAEN_V1190_B_HEADER_EN 					BIT(4)
#define CAEN_V1190_B_TERM_ON 					BIT(5)
#define CAEN_V1190_B_CHIP0_ERROR 				BIT(6)
#define CAEN_V1190_B_CHIP1_ERROR 				BIT(7)
#define CAEN_V1190_B_CHIP2_ERROR 				BIT(8)
#define CAEN_V1190_B_CHIP3_ERROR 				BIT(9)
#define CAEN_V1190_B_BERR_FLAG 					BIT(10)
#define CAEN_V1190_B_PURGE 						BIT(11)
#define CAEN_V1190_B_RESOLUTION 				BIT(12)
#define CAEN_V1190_B_PAIR 						BIT(14)
#define CAEN_V1190_B_TRIGGERLOST 				BIT(15)

#define CAEN_V1190_M_RESOLUTION 				(BIT(12) | BIT(13)

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			micro handshake
////////////////////////////////////////////////////////////////////////////*/

#define CAEN_V1190_B_WRITE_OK					BIT(0)
#define CAEN_V1190_B_READ_OK					BIT(1)

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			fifo status
////////////////////////////////////////////////////////////////////////////*/

#define CAEN_V1190_B_EVFIFODATA_READY			BIT(0)
#define CAEN_V1190_B_EVFIFO_FULL				BIT(1)

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			data bits & bytes
////////////////////////////////////////////////////////////////////////////*/

/*! Definitions of bits that appear in all data words */

#define CAEN_V1190_M_TYPE						0xf8000000	/* Data type field */
#define CAEN_V1190_B_GLOBAL_HEADER				0x40000000	/* GBL header type fld */
#define CAEN_V1190_B_TDC_HEADER 				0x08000000	/* TDC header */
#define CAEN_V1190_B_TDC_TRAILER				0x18000000	/* TDC Trailer */
#define CAEN_V1190_B_MEASUREMENT				0x00000000	/* TDC Measurement */
#define CAEN_V1190_B_TDC_ERROR					0x20000000	/* TDC Error flag */
#define CAEN_V1190_B_TRIGGER_TIME				0x88000000	/* Trigger time tag */
#define CAEN_V1190_B_GLOBAL_TRAILER 			0x80000000	/* Global trailer */
#define CAEN_V1190_B_FILLER_LONG				0xc0000000	/* Filler longs */

/*! Definitions in the global header only */

#define CAEN_V1190_M_EVENTCOUNT					0x07ffffe0
#define CAEN_V1190_B_EVENT_RSHIFT				0x5
#define CAEN_V1190_B_GEO_MASK					0x1f

/*! Definitions in the TDC header longword */

#define CAEN_V1190_M_TDC						0x03000000
#define CAEN_V1190_SH_TDC						24
#define CAEN_V1190_M_EVENTID					0x007ff800
#define CAEN_V1190_SH_EVENTID					12
#define CAEN_V1190_M_BUNCHID					0x000007ff
#define CAEN_V1190_M_TDCWORDCOUNT				0x000007ff

/*! Definitions in the TDC measurement word */

#define CAEN_V1190_B_TRAILING					0x04000000
#define CAEN_V1190_M_CHANNEL					0x03f80000
#define CAEN_V1190_SH_CHANNEL					19
#define CAEN_V1190_M_DATA						0x0007ffff

/*! Mask bits in the TDC Error word.  There are no functions
    to check individual errors. Call TDCErrorbits to extract
    the bits and then and against the masks below. */

#define CAEN_V1190_B_HITLOST_0_FIFO 			BIT(0)	/* hits lost group 0 FIFO overflow */
#define CAEN_V1190_B_HITLOST_0_L1 				BIT(1)	/* hits lost group 0 L1 overflow */
#define CAEN_V1190_B_HITERROR_0 				BIT(2)	/* hit error in group 0 */
#define CAEN_V1190_B_HITLOST_1_FIFO 			BIT(3)	/* hits lost group 1 FIFO overflow */
#define CAEN_V1190_B_HITLOST_1_L1 				BIT(4)	/* hits lost group 1 L1 overflow */
#define CAEN_V1190_B_HITERROR_1 				BIT(5)	/* hit error in group 1 */
#define CAEN_V1190_B_HITLOST_2_FIFO 			BIT(6)	/* hits lost group 2 FIFO overflow */
#define CAEN_V1190_B_HITLOST_2_L1 				BIT(7)	/* hits lost group 2 L1 overflow */
#define CAEN_V1190_B_HITERROR_2 				BIT(8)	/* hit error in group 2 */
#define CAEN_V1190_B_HITLOST_3_FIFO 			BIT(9)	/* hits lost group 3 FIFO overflow */
#define CAEN_V1190_B_HITLOST_3_L1 				BIT(10)	/* hits lost group 3 L1 overflow */
#define CAEN_V1190_B_HITERROR_3 				BIT(11)	/* hit error in group 3 */
#define CAEN_V1190_B_HITS_EXCEEDED 				BIT(12)	/* Hits lost, size limit exceeded */
#define CAEN_V1190_B_EVENTLOST_FIFO 			BIT(13)	/* Event lost due to trigger fifo overflow */
#define CAEN_V1190_B_FATALCHIP_ERROR 			BIT(14)	/* fatal chip error detected */
#define CAEN_V1190_M_ERROR						0x7fff

/*! Extended trigger time */

#define CAEN_V1190_M_TRIGGERTIME				0x07ffffff

/*! Global trailer */

#define CAEN_V1190_M_TRIGGERLOST				0x04000000
#define CAEN_V1190_M_TDCERROR					0x02000000
#define CAEN_V1190_M_OVERFLOW					0x01000000
#define CAEN_V1190_M_WORDCOUNT					0x001fffe0
#define CAEN_V1190_SH_WORDCOUNT 				5

/*! Trigger matching bit */

#define CAEN_V1190_B_TRIGGER_MATCHING_ON		BIT(0)
#define CAEN_V1190_B_TRIGGER_TIME_SUBTR_ON		BIT(0)

/*! Edge detection and resolution */

#define CAEN_V1190_B_EDGE_PAIR					0x0
#define CAEN_V1190_B_EDGE_TRAILING				0x1
#define CAEN_V1190_B_EDGE_LEADING				0x2
#define CAEN_V1190_B_EDGE_BOTH					0x3
#define CAEN_V1190_M_EDGE_RESOLUTION			0x3
#define CAEN_V1190_M_PAIR_RESOLUTION			0x7
#define CAEN_V1190_M_PAIR_WIDTH					0xF
#define CAEN_V1190_SH_PAIR_WIDTH				8

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			op codes
////////////////////////////////////////////////////////////////////////////*/

/*! Acquisition mode */

#define CAEN_V1190_OP_TRG_MATCH 				0x0000
#define CAEN_V1190_OP_CONT_STOR 				0x0100
#define CAEN_V1190_OP_READ_ACQ_MOD				0x0200
#define CAEN_V1190_OP_SET_KEEP_TOKEN			0x0300
#define CAEN_V1190_OP_CLEAR_KEEP_TOKEN			0x0400
#define CAEN_V1190_OP_LOAD_DEF_CONFIG			0x0500
#define CAEN_V1190_OP_SAVE_USER_CONFIG			0x0600
#define CAEN_V1190_OP_LOAD_USER_CONFIG			0x0700
#define CAEN_V1190_OP_AUTOLOAD_USER_CONFIG		0x0800
#define CAEN_V1190_OP_AUTOLOAD_DEF_CONFIG		0x0900

/*! Trigger setup */

#define CAEN_V1190_OP_SET_WIN_WIDTH 			0x1000
#define CAEN_V1190_OP_SET_WIN_OFFS				0x1100
#define CAEN_V1190_OP_SET_SW_MARGIN 			0x1200
#define CAEN_V1190_OP_SET_REJ_MARGIN			0x1300
#define CAEN_V1190_OP_EN_SUB_TRG				0x1400
#define CAEN_V1190_OP_DIS_SUB_TRG				0x1500
#define CAEN_V1190_OP_READ_TRG_CONF 			0x1600

/*! TDC Edge detection & resolution */

#define CAEN_V1190_OP_SET_DETECTION 			0x2200
#define CAEN_V1190_OP_READ_DETECTION			0x2300
#define CAEN_V1190_OP_SET_TR_LEAD_LSB			0x2400
#define CAEN_V1190_OP_SET_PAIR_RES				0x2500
#define CAEN_V1190_OP_READ_RES  				0x2600
#define CAEN_V1190_OP_SET_DEAD_TIME 			0x2800
#define CAEN_V1190_OP_READ_DEAD_TIME			0x2900

/*! TDC Readout-data structure */

#define CAEN_V1190_OP_EN_HEADER_TRAILER 		0x3000
#define CAEN_V1190_OP_DIS_HEADER_TRAILER		0x3100
#define CAEN_V1190_OP_READ_HEADER_TRAILER		0x3200
#define CAEN_V1190_OP_SET_EVENT_SIZE			0x3300
#define CAEN_V1190_OP_READ_EVENT_SIZE			0x3400
#define CAEN_V1190_OP_EN_ERROR_MARK 			0x3500
#define CAEN_V1190_OP_DIS_ERROR_MARK			0x3600
#define CAEN_V1190_OP_EN_ERROR_BYPASS			0x3700
#define CAEN_V1190_OP_DIS_ERROR_BYPASS			0x3800
#define CAEN_V1190_OP_SET_ERROR_TYPES			0x3900
#define CAEN_V1190_OP_READ_ERROR_TYPES			0x3a00
#define CAEN_V1190_OP_SET_FIFO_SIZE 			0x3b00
#define CAEN_V1190_OP_READ_FIFO_SIZE			0x3c00

/*! Channel enable/disable control */

#define CAEN_V1190_OP_EN_CHANNEL				0x4000
#define CAEN_V1190_OP_DIS_CHANNEL				0x4100
#define CAEN_V1190_OP_EN_ALL_CH 				0x4200
#define CAEN_V1190_OP_DIS_ALL_CH				0x4300
#define CAEN_V1190_OP_WRITE_EN_PATTERN			0x4400
#define CAEN_V1190_OP_READ_EN_PATTERN			0x4500
#define CAEN_V1190_OP_WRITE_EN_PATTERN32		0x4600
#define CAEN_V1190_OP_READ_EN_PATTERN32 		0x4700

/*! Adjustment opcodes */

#define CAEN_V1190_OP_SET_GLOB_OFFS 			0x5000
#define CAEN_V1190_OP_READ_GLOB_OFFS			0x5100
#define CAEN_V1190_OP_SET_ADJUST_CH 			0x5200
#define CAEN_V1190_OP_READ_ADJUST_CH			0x5300
#define CAEN_V1190_OP_SET_RC_ADJ				0x5400
#define CAEN_V1190_OP_READ_RC_ADJ				0x5500
#define CAEN_V1190_OP_SAVE_RC_ADJ				0x5600

/*! Miscellaneous */

#define CAEN_V1190_OP_READ_TDC_ID				0x6000
#define CAEN_V1190_OP_READ_MICRO_REV			0x6100
#define CAEN_V1190_OP_RESET_DLL_PLL 			0x6200


#define NOF_CHANNELS							128
#define NOF_CHANNEL_WORDS						8

#define NOT_USED								0xFFFFFFFF
#define NO_ARG									0xAFFE
#define DATA_ERROR								0xAFFE

#define HANDSHAKE_TIMEOUT						1000000

#define WAIT_FIFO_READY							10
#define WAIT_DATA_READY 						100

#define CAEN_V1190_B_FIFO_READY					BIT(0)
#define CAEN_V1190_B_DATA_READY 				BIT(0)

/*! Defaults */

#define CAEN_V1190_WINDOW_WIDTH_DEFAULT 		0x14
#define CAEN_V1190_WINDOW_OFFSET_DEFAULT 		0xFFFFFFD8
#define CAEN_V1190_REJECT_MARGIN_DEFAULT		0x4
#define CAEN_V1190_SEARCH_MARGIN_DEFAULT		0x8
#define CAEN_V1190_EDGE_DETECTION_DEFAULT		CAEN_V1190_B_EDGE_LEADING
#define CAEN_V1190_EDGE_RESOLUTION_DEFAULT		0x2
#define CAEN_V1190_PAIR_RESOLUTION_DEFAULT		0x0
#define CAEN_V1190_PAIR_WIDTH_DEFAULT			0x0
#define CAEN_V1190_DEAD_TIME_DEFAULT			0x0
#define CAEN_V1190_EVENT_SIZE_DEFAULT			0x9
#define CAEN_V1190_FIFO_SIZE_DEFAULT			0x7
#define CAEN_V1190_ALMOST_FULL_DEFAULT			64

#define MICRO_HSHAKE(s_module)					*((volatile unsigned short *) (s_module->baseAddr + CAEN_V1190_A_MICROHANDSHAKE))
#define GET_MICRO_DATA(s_module)				*((volatile unsigned short *) (s_module->baseAddr + CAEN_V1190_A_MICRODATA))
#define SET_MICRO_DATA(s_module, value) 		*((volatile unsigned short *) (s_module->baseAddr + CAEN_V1190_A_MICRODATA)) = value
#define GET_DATA(s_module, offset)				*((volatile unsigned short *) (s_module->baseAddr + offset))
#define SET_DATA(s_module, offset, value) 		*((volatile unsigned short *) (s_module->baseAddr + offset)) = value

struct s_caen_v1190 {
	unsigned long vmeAddr;							/* phys addr given by module switches */
	volatile unsigned char * baseAddr;				/* addr mapped via find_controller() */

	char moduleName[100];
	char prefix[100];								/* "m_read_meb" (default) or any other */
	char mpref[20]; 								/* "caen_v1190: " or "" */

	int serial; 									/* MARaBOU's serial number */

	bool_t verbose;
	bool_t verify;
	bool_t dumpRegsOnInit;

	bool_t updSettings;
	int updInterval;
	int updCountDown;

	bool_t trigMatchingOn;
	uint16_t windowWidth;
	uint16_t windowOffset;
	uint16_t rejectMargin;
	uint16_t searchMargin;
	bool_t enaTrigSubtraction;
	uint16_t edgeDetectionMode;
	uint16_t edgeResolution;
	uint16_t pairResolution;
	uint16_t pairWidth;
	uint16_t deadTime;
	uint16_t eventSize;
	uint16_t fifoSize;
	uint16_t almostFullLevel;
	bool_t enaHeaderTrailer;
	bool_t enaExtTrigTag;
	bool_t enaEmptyEvent;
	bool_t enaEventFifo;

	uint16_t channels[NOF_CHANNEL_WORDS];

	struct s_bma * bma; 					/* block mode access */
	uint32_t bltBufferSize;
	uint32_t bltBlockSize;
	uint8_t * bltBuffer;
};

#endif
