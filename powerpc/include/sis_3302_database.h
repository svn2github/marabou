#ifndef __SIS3302_DATABASE_H__
#define __SIS3302_DATABASE_H__

#include "LwrTypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <allParam.h>
#include <ces/vmelib.h>
#include <ces/bmalib.h>

#include "block.h"
#include "mapping_database.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			sis3302_Database.h
//! \brief			Definitions for SIS3302 ADC
//! \details		Structures describing a SIS3302 ADC
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.5 $
//! $Date: 2011-07-26 08:41:50 $
////////////////////////////////////////////////////////////////////////////*/


#define SIS_NOF_CHANNELS	8
#define SIS_NOF_GROUPS		4
#define SIS3302_STRLEN			100


#define SIS_GROUP(adc)		(adc >> 1)

typedef struct {
	Char_t moduleName[SIS3302_STRLEN];
	Char_t prefix[SIS3302_STRLEN];				/* "m_read_meb" (default) or any other */
	Char_t mpref[10]; 					/* "sis3302: " or "" */

	Int_t boardId;						/* module info */
	Int_t majorVersion;
	Int_t minorVersion;

	Int_t serial; 						/* MARaBOU's serial number */

	Bool_t verbose;
	Bool_t dumpRegsOnInit;

	s_mapDescr * md;				/* mapping descriptor */

	Bool_t blockXfer;					/* block xfer */
	Int_t bufferSize;					/* max buffer size */

	UInt_t status;

	Bool_t updSettings;
	Int_t updInterval;
	Int_t updCountDown;

	Bool_t reducedAddressSpace;				/* 128 or 16 MB address space */

	Int_t currentSampling;					/* sampling: bank 1 or 2 */

	Bool_t tracingMode;					/* ON:  keep raw and energy tracing length values */
								/* OFF: save length values, set to zero, restore on stop */
	UInt_t activeChannels;					/* pattern of active channels */

	Int_t dacValues[SIS_NOF_CHANNELS];

	UInt_t controlStatus;
	UInt_t headerBits[SIS_NOF_GROUPS];
	UInt_t triggerMode[SIS_NOF_CHANNELS];
	UInt_t gateMode[SIS_NOF_CHANNELS];
	UInt_t nextNeighborTrigger[SIS_NOF_CHANNELS];
	UInt_t nextNeighborGate[SIS_NOF_CHANNELS];
	Bool_t invertSignal[SIS_NOF_CHANNELS];
	Int_t endAddrThresh[SIS_NOF_GROUPS];
	Int_t pretrigDelay[SIS_NOF_GROUPS];
	Int_t trigGateLength[SIS_NOF_GROUPS];
	Int_t rawDataSampleLength[SIS_NOF_GROUPS];
	Int_t rawDataSampleStart[SIS_NOF_GROUPS];
	Int_t trigPeakTime[SIS_NOF_CHANNELS];
	Int_t trigGapTime[SIS_NOF_CHANNELS];
	Int_t trigPulseLength[SIS_NOF_CHANNELS];
	Int_t trigInternalGate[SIS_NOF_CHANNELS];
	Int_t trigInternalDelay[SIS_NOF_CHANNELS];
	Int_t trigDecimation[SIS_NOF_CHANNELS];
	Int_t trigThresh[SIS_NOF_CHANNELS];
	Int_t trigGT[SIS_NOF_CHANNELS];
	Int_t trigOut[SIS_NOF_CHANNELS];
	Int_t energyPeakTime[SIS_NOF_GROUPS];
	Int_t energyGapTime[SIS_NOF_GROUPS];
	Int_t energyDecimation[SIS_NOF_GROUPS];
	Int_t energyGateLength[SIS_NOF_GROUPS];

	Int_t clockSource;
	Bool_t mcaMode;
	Int_t lemoOutMode;
	Int_t lemoInMode;
	Int_t lemoInEnableMask;
	Bool_t triggerFeedback;

	Int_t energyTestBits[SIS_NOF_GROUPS];
	Int_t energySampleLength[SIS_NOF_GROUPS];
	Int_t energySampleStart[3][SIS_NOF_GROUPS];
	Int_t energyTauFactor[SIS_NOF_CHANNELS];

	unsigned long mcstAddr;			/* MCST signature */
} s_sis_3302;
#endif
