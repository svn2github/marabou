#ifndef __MADC32_DATABASE_H__
#define __MADC32_DATABASE_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <ces/uiocmd.h>
#include <ces/bmalib.h>

#include "block.h"

#include "mesy_database.h"

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


#define MADC_NOF_CHANNELS	32

typedef struct {
	s_mesy m;	/* common data for all mesytec modules */
				/* attention: s_mesy has to be the very first member of struct s_madc32! */
	
/* proprietary registers for Mesytec MADC32 */
	uint16_t threshold[MADC_NOF_CHANNELS];
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
	uint16_t eclG1Osc;
	uint16_t eclFclRts;
	uint16_t nimG1Osc;
	uint16_t nimFclRts;
	uint16_t nimBusy;
	uint16_t pulserStatus;
} s_madc32;

#endif

