#ifndef __MQDC32_DATABASE_H__
#define __MQDC32_DATABASE_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <allParam.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>

#include "block.h"
#include "mapping_database.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mqdc32_database.h
//! \brief			Database for Mesytec Mqdc32 QDC
//! \details		Structure describing Mesytec Mqdc32 QDCs
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/

#define NOF_CHANNELS	32

struct s_mqdc32 {
	char moduleName[100];
	char prefix[100];			/* "m_read_meb" (default) or any other */
	char mpref[10]; 			/* "madc32: " or "" */

	struct s_mapDescr * md;			/* mapping descriptor */

	int serial; 				/* MARaBOU's serial number */

	bool_t verbose;
	bool_t dumpRegsOnInit;

	bool_t updSettings;
	int updInterval;
	int updCountDown;

	uint16_t threshold[NOF_CHANNELS];
	uint16_t addrSource;
	uint16_t addrReg;
	uint16_t moduleId;
	uint16_t fifoLength;
	uint16_t dataWidth;
	uint16_t xferData;
	uint16_t multiEvent;
	uint16_t markingType;
	uint16_t bankOperation;
	uint16_t adcResolution;
	uint16_t bankOffset[2];
	uint16_t gateLimit[2];
	bool_t slidingScaleOff;
	bool_t skipOutOfRange;
	bool_t ignoreThresh;
	uint16_t inputCoupling;
	uint16_t eclTerm;
	uint16_t eclG1OrOsc;
	uint16_t eclFclOrRts;
	uint16_t gateSelect;
	uint16_t nimG1OrOsc;
	uint16_t nimFclOrRts;
	uint16_t nimBusy;
	uint16_t testPulserStatus;
	uint16_t ctraTsSource;
	uint16_t ctraTsDivisor;

	int memorySize;

	bool_t blockXfer;			/* TRUE if block xfer enabled */

	unsigned long mcstSignature;		/* MCST signature */
	volatile char * mcstAddr;		/* ... after mapping */
	unsigned long cbltSignature;		/* CBLT signature */
	volatile char * cbltAddr;		/* ... after mapping */
	bool_t firstInChain;			/* TRUE if head of chain */
	bool_t lastInChain;			/* TRUE if end of chain */

};

#endif
