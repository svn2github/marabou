#ifndef __SIS3302_DATABASE_H__
#define __SIS3302_DATABASE_H__

#include "LwrTypes.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			Sis3302_Database.h
//! \brief			Definitions for SIS3302 ADC
//! \details		Structures describing a SIS3302 ADC
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $
//! $Date: 2010-12-09 11:43:39 $
////////////////////////////////////////////////////////////////////////////*/


#define NOF_CHANNELS	8
#define NOF_GROUPS		4
#define STRLEN			100

#define GROUP(adc)		(adc >> 1)

struct s_sis_3302 {
	ULong_t vmeAddr;						/* phys addr given by module switches */
	volatile Char_t * baseAddr;				/* addr mapped via find_controller() */
	Int_t segSize;
	UInt_t lowerBound;
	UInt_t upperBound;
	UInt_t mappedAddr;
	UInt_t addrMod;

	Char_t moduleName[STRLEN];
	Char_t prefix[STRLEN];					/* "m_read_meb" (default) or any other */
	Char_t mpref[10]; 						/* "madc32: " or "" */

	Int_t boardId;							/* module info */
	Int_t majorVersion;
	Int_t minorVersion;

	Int_t serial; 							/* MARaBOU's serial number */

	Bool_t verbose;
	Bool_t dumpRegsOnInit;

	UInt_t status;

	Bool_t updSettings;
	Int_t updInterval;
	Int_t updCountDown;

	Int_t currentSampling;					/* sampling: bank 1 or 2 */

	Bool_t tracingMode;						/* ON:  keep raw and energy tracing length values */
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

	Int_t energyTestBits[NOF_GROUPS];
	Int_t energySampleLength[NOF_GROUPS];
	Int_t energySampleStart[3][NOF_GROUPS];
	Int_t energyTauFactor[NOF_CHANNELS];
};
#endif