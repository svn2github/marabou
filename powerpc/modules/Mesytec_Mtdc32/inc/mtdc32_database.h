#ifndef __MTDC32_DATABASE_H__
#define __MTDC32_DATABASE_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <ces/uiocmd.h>
#include <ces/bmalib.h>

#include "block.h"
#include "mapping_database.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mtdc32_database.h
//! \brief			Database for Mesytec Mtdc32 TDC
//! \details		Structure describing Mesytec Mtdc32 TDCs
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/


#define MTDC_NOF_CHANNELS	32
#define MTDC_NOF_BANKS		2

typedef struct {
	char moduleName[100];
	char prefix[100];			/* "m_read_meb" (default) or any other */
	char mpref[10]; 			/* "mtdc32: " or "" */

	s_mapDescr * md;			/* mapping descriptor */

	int serial; 				/* MARaBOU's serial number */

	bool_t verbose;
	bool_t dumpRegsOnInit;

	uint16_t addrSource;
	uint16_t addrReg;
	uint16_t moduleId;
	uint16_t fifoLength;
	uint16_t dataWidth;
	uint16_t xferData;
	uint16_t multiEvent;
	uint16_t markingType;
	uint16_t bankOperation;
	uint16_t tdcResolution;
	uint16_t outputFormat;
	uint16_t bufferThresh;
	int16_t winStart[MTDC_NOF_BANKS];
	int16_t winWidth[MTDC_NOF_BANKS];
	uint16_t trigSrcTrig[MTDC_NOF_BANKS];
	uint16_t trigSrcChan[MTDC_NOF_BANKS];
	uint16_t trigSrcBank[MTDC_NOF_BANKS];
	uint16_t firstHit;
	uint16_t negEdge;
	uint16_t eclTerm;
	uint16_t eclT1Osc;
	uint16_t eclFclRts;
	uint16_t trigSelect;
	uint16_t nimT1Osc;
	uint16_t nimFclRts;
	uint16_t nimBusy;
	uint16_t pulserStatus;
	uint16_t pulserPattern;
	uint16_t inputThresh[MTDC_NOF_BANKS];
	uint16_t ctraTsSource;
	uint16_t ctraTsDivisor;
	uint16_t multHighLimit[MTDC_NOF_BANKS];
	uint16_t multLowLimit[MTDC_NOF_BANKS];

	int memorySize;

	bool_t blockXfer;			/* TRUE if block xfer enabled */
	bool_t repairRawData;		/* TRUE if raw data should be repaired (missing EOEs) */

	unsigned long mcstSignature;		/* MCST signature */
	volatile char * mcstAddr;		/* ... after mapping */
	bool_t mcstMaster;				/* TRUE if MCST master */
	unsigned long cbltSignature;		/* CBLT signature */
	volatile char * cbltAddr;		/* ... after mapping */
	bool_t firstInCbltChain;			/* TRUE if head of CBLT chain */
	bool_t lastInCbltChain;			/* TRUE if end of CBLT chain */
} s_mtdc32;

#endif

