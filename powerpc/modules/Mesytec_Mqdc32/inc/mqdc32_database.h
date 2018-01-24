#ifndef __MQDC32_DATABASE_H__
#define __MQDC32_DATABASE_H__

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

#include "mesy_database.h"

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

#define MQDC_NOF_CHANNELS	32

typedef struct {
	s_mesy m;	/* common data for all mesytec modules */
				/* attention: s_mesy has to be the very first member of struct s_mqdc32! */
	
/* proprietary data for Mesytec MQDC32 */	
	uint16_t threshold[MQDC_NOF_CHANNELS];
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
	uint16_t eclG1Osc;
	uint16_t eclFclRts;
	uint16_t gateSelect;
	uint16_t nimG1Osc;
	uint16_t nimFclRts;
	uint16_t nimBusy;
	uint16_t pulserStatus;
	uint16_t ctraTsSource;
	uint16_t ctraTsDivisor;
	uint16_t multHighLimit[2];
	uint16_t multLowLimit[2];
} s_mqdc32;

#endif
