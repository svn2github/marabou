#ifndef __MADC32_DATABASE_H__
#define __MADC32_DATABASE_H__

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
//! \file			madc32_database.h
//! \brief			Database for Mesytec Madc32 ADC
//! \details		Structure describing Mesytec Madc32 ADCs
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/


#define NOF_CHANNELS	32

struct s_madc32 {
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
	uint16_t outputFormat;
	uint16_t bufferThresh;
	uint16_t adcOverride;
	bool_t slidingScaleOff;
	bool_t skipOutOfRange;
	bool_t ignoreThresh;
	uint16_t ggHoldDelay[2];
	uint16_t ggHoldWidth[2];
	uint16_t useGG;
	uint16_t inputRange;
	uint16_t eclTerm;
	uint16_t eclG1OrOsc;
	uint16_t eclFclOrRts;
	uint16_t nimG1OrOsc;
	uint16_t nimFclOrRts;
	uint16_t nimBusy;
	uint16_t testPulserStatus;
	uint16_t ctraTsSource;
	uint16_t ctraTsDivisor;

	int memorySize;

	bool_t blockXfer;			/* TRUE if block xfer enabled */

	unsigned long mcstAddr;			/* MCST signature */
	unsigned long cbltAddr;			/* CBLT signature */
	bool_t firstInChain;			/* TRUE if head of chain */
	bool_t lastInChain;			/* TRUE if end of chain */

};

#endif

