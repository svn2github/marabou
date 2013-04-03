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


#define NOF_CHANNELS	8
#define NOF_GROUPS		4
#define SIS3302_STRLEN			100


#define GROUP(adc)		(adc >> 1)

struct s_sis_3302 {
	Char_t moduleName[SIS3302_STRLEN];
	Char_t prefix[SIS3302_STRLEN];				/* "m_read_meb" (default) or any other */
	Char_t mpref[10]; 					/* "sis3302: " or "" */

	Int_t boardId;						/* module info */
	Int_t majorVersion;
	Int_t minorVersion;

	Int_t serial; 						/* MARaBOU's serial number */

	Bool_t verbose;
	Bool_t dumpRegsOnInit;

	struct s_mapDescr * md;				/* mapping descriptor */

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

	Int_t dacValues[NOF_CHANNELS];

	UInt_t controlStatus;
	UInt_t headerBits[NOF_GROUPS];
	UInt_t triggerMode[NOF_CHANNELS];
	UInt_t gateMode[NOF_CHANNELS];
	UInt_t nextNeighborTrigger[NOF_CHANNELS];
	UInt_t nextNeighborGate[NOF_CHANNELS];
	Bool_t invertSignal[NOF_CHANNELS];
	Int_t endAddrThresh[NOF_GROUPS];
	Int_t pretrigDelay[NOF_GROUPS];
	Int_t trigGateLength[NOF_GROUPS];
	Int_t rawDataSampleLength[NOF_GROUPS];
	Int_t rawDataSampleStart[NOF_GROUPS];
	Int_t trigPeakTime[NOF_CHANNELS];
	Int_t trigGapTime[NOF_CHANNELS];
	Int_t trigPulseLength[NOF_CHANNELS];
	Int_t trigInternalGate[NOF_CHANNELS];
	Int_t trigInternalDelay[NOF_CHANNELS];
	Int_t trigDecimation[NOF_CHANNELS];
	Int_t trigThresh[NOF_CHANNELS];
	Int_t trigGT[NOF_CHANNELS];
	Int_t trigOut[NOF_CHANNELS];
	Int_t energyPeakTime[NOF_GROUPS];
	Int_t energyGapTime[NOF_GROUPS];
	Int_t energyDecimation[NOF_GROUPS];
	Int_t energyGateLength[NOF_GROUPS];

	Int_t clockSource;
	Bool_t mcaMode;
	Int_t lemoOutMode;
	Int_t lemoInMode;
	Int_t lemoInEnableMask;
	Bool_t triggerFeedback;

	Int_t energyTestBits[NOF_GROUPS];
	Int_t energySampleLength[NOF_GROUPS];
	Int_t energySampleStart[3][NOF_GROUPS];
	Int_t energyTauFactor[NOF_CHANNELS];

	unsigned long mcstAddr;			/* MCST signature */
};
#endif
