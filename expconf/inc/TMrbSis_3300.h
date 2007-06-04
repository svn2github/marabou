#ifndef __TMrbSis_3300_h__
#define __TMrbSis_3300_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSis_3300.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSis_3300        -- VME digitizer adc
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSis_3300.h,v 1.5 2007-06-04 05:54:55 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSis_3300
// Purpose:        Define a VME module type SIS 3300
// Description:    Defines a VME module SIS 3300
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSis_3300 : public TMrbVMEModule {

	public:
		enum				{	kSegSize		=	0x1000000L	};
		enum				{	kAddrMod		=	0x09	};

		enum				{	kNofGroups		=	4	};
		enum				{	kNofChannels	=	8	};
		enum				{	kClockTick		=	10	};

		enum EMrbRegisters	{	kRegAcquisitionMode,
								kRegReadGroup,
								kRegPageSize,
								kRegClockSource,
								kRegStartDelay,
								kRegStopDelay,
								kRegBankFull,
								kRegTriggerOn,
								kRegTriggerThresh,
								kRegTriggerSlope,
								kRegPeakTime,
								kRegGapTime,
								kRegPulseMode,
								kRegPulseLength
							};

		enum EMrbAcquisitionMode	{	kAcqSingleEvent 	=	0,
										kAcqMultiEvent,
										kAcqGateChaining,
										kAcqLast
									};

		enum EMrbPageSize			{	kPageSize128k 	=	0,
										kPageSize16k,
										kPageSize4k,
										kPageSize2k,
										kPageSize1k,
										kPageSize512s,
										kPageSize256s,
										kPageSize128s,
										kPageSizeLast
									};

		enum EMrbClockSource		{	kClockSource100 	=	0,
										kClockSource50,
										kClockSource25,
										kClockSource12_5,
										kClockSource6_25,
										kClockSource3_125,
										kClockSourceExtern,
										kClockSourceP2,
										kClockSourceLast
									};

		enum EMrbSendBankFull		{	kBankFullDisabled 	=	0,
										kBankFullToUser,
										kBankFullToStop,
										kBankFullToStart,
										kBankFullLast
									};

		enum EMrbTrigMode			{	kTriggerOnDisabled 	=	0,
										kTriggerOnEnabled,
										kTriggerOnEvenFIR,
										kTriggerOnOddFIR,
										kTriggerOnLast
									};

		enum EMrbTrigSlope			{	kTriggerSlopeDisabled 	=	0,
										kTriggerSlopeNegative,
										kTriggerSlopePositive,
										kTriggerSlopeBipolar,
										kTriggerSlopeLast
									};

		enum	{	kShapeShort,				// ATTENTION! Must be same as in /templates/config/Subevent_Sis33xx_Common.h
					kShapeLong,
					kShapeBoth
				};

		enum	{	kShaperMinMax	=	256 };

	public:

		TMrbSis_3300() {};  												// default ctor
		TMrbSis_3300(const Char_t * ModuleName, UInt_t BaseAddr); 			// define a new module
		~TMrbSis_3300() {};													// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("sis_3300"); }; 	// module mnemonic

		inline void SetBlockXfer(Bool_t Flag = kTRUE) { fBlockXfer = Flag; };
		inline Bool_t BlockXferEnabled() { return(fBlockXfer); };

		inline Bool_t SetAcquisitionMode(Int_t Mode) { return(this->Set(TMrbSis_3300::kRegAcquisitionMode, Mode)); };
		inline Bool_t SetAcquisitionMode(Char_t * Mode) { return(this->Set(TMrbSis_3300::kRegAcquisitionMode, Mode)); };
		inline Int_t GetAcquisitionMode() { return(this->Get(TMrbSis_3300::kRegAcquisitionMode)); };

		inline Bool_t SetReadGroup(Bool_t Flag = kTRUE, Int_t Group = -1) { return(this->Set(TMrbSis_3300::kRegReadGroup, Flag ? 1 : 0, Group)); };
		inline Bool_t GroupToBeRead(Int_t Group) { return(this->Get(TMrbSis_3300::kRegReadGroup, Group) == 1); };

		inline void SetMaxEvents(Int_t Events) { fMaxEvents = Events; };
		inline Int_t GetMaxEvents() { return(fMaxEvents); };

		inline Bool_t SetPageSize(Int_t Size) { return(this->Set(TMrbSis_3300::kRegPageSize, Size)); };
		inline Bool_t SetPageSize(Char_t * Size) {	return(this->Set(TMrbSis_3300::kRegPageSize, Size)); };
		inline Int_t GetPageSize() { return(this->Get(TMrbSis_3300::kRegPageSize)); };
		Int_t GetPageSizeChan();

		inline Bool_t SetClockSource(Int_t Source) { return(this->Set(TMrbSis_3300::kRegClockSource, Source)); };
		inline Bool_t SetClockSource(Char_t * Source) { return(this->Set(TMrbSis_3300::kRegClockSource, Source)); };
		inline Int_t GetClockSource() { return(this->Get(TMrbSis_3300::kRegClockSource)); };

		inline Bool_t SetStartDelay(Int_t Ticks) { return(this->Set(TMrbSis_3300::kRegStartDelay, Ticks)); };
		inline Int_t GetStartDelay() { return(this->Get(TMrbSis_3300::kRegStartDelay)); };

		inline Bool_t SetStopDelay(Int_t Ticks) { return(this->Set(TMrbSis_3300::kRegStopDelay, Ticks)); };
		inline Int_t GetStopDelay() { return(this->Get(TMrbSis_3300::kRegStopDelay)); };

		inline void SetTriggerToUserOut(Bool_t Flag = kTRUE) { fTriggerOut = Flag; };
		inline Bool_t TriggerToUserOut() { return(fTriggerOut); };

		inline void InvertTrigger(Bool_t Flag = kTRUE) { fInvertTrigger = Flag; };
		inline Bool_t TriggerInverted() { return(fInvertTrigger); };

		inline void RouteTriggerToStop(Bool_t Flag = kTRUE) { fRouteTrigger = Flag; };
		inline Bool_t TriggerRoutedToStop() { return(fRouteTrigger); };

		inline void EnableStartStop(Bool_t Flag = kTRUE) { fStartStopEnable = Flag; };
		inline Bool_t StartStopEnabled() { return(fStartStopEnable); };

		inline void EnableAutoStart(Bool_t Flag = kTRUE) { fAutoStartEnable = Flag; };
		inline Bool_t AutoStartEnabled() { return(fAutoStartEnable); };

		inline void EnableAutoBankSwitch(Bool_t Flag = kTRUE) { fAutoBankSwitch = Flag; };
		inline Bool_t AutoBankSwitchEnabled() { return(fAutoBankSwitch); };

		inline Bool_t SetBankFullToFront(Int_t Mode) { return(this->Set(TMrbSis_3300::kRegBankFull, Mode)); };
		inline Bool_t SetBankFullToFront(Char_t * Mode) { return(this->Set(TMrbSis_3300::kRegBankFull, Mode)); };
		inline Int_t GetBankFullToFront() { return(this->Get(TMrbSis_3300::kRegBankFull)); };

		inline void EnableWrapAround(Bool_t Flag = kTRUE) { fWrapAroundEnable = Flag; };
		inline Bool_t WrapAroundEnabled() { return(fWrapAroundEnable); };

		inline void EnableTriggerOnBankArmed(Bool_t Flag = kTRUE) { fTriggerArmed = Flag; };
		inline Bool_t TriggerOnBankArmedEnabled() { return(fTriggerArmed); };

		inline Bool_t SetTriggerOn(Int_t Mode, Int_t Group = -1) { return(this->Set(TMrbSis_3300::kRegTriggerOn, Mode, Group)); };
		inline Bool_t SetTriggerOn(Char_t * Mode, Int_t Group = -1) { return(this->Set(TMrbSis_3300::kRegTriggerOn, Mode, Group)); };
		inline Int_t GetTriggerOn(Int_t Group) { return(this->Get(TMrbSis_3300::kRegTriggerOn, Group)); };

		inline Bool_t SetTriggerThresh(Int_t Thresh, Int_t Channel = -1) { return(this->Set(TMrbSis_3300::kRegTriggerThresh, Thresh, Channel)); };
		inline Int_t GetTriggerThresh(Int_t Channel) { return(this->Get(TMrbSis_3300::kRegTriggerThresh, Channel)); };

		inline Bool_t SetTriggerSlope(Int_t Slope, Int_t Channel = -1) { return(this->Set(TMrbSis_3300::kRegTriggerSlope, Slope, Channel)); };
		inline Bool_t SetTriggerSlope(Char_t * Slope, Int_t Channel = -1) { return(this->Set(TMrbSis_3300::kRegTriggerSlope, Slope, Channel)); };
		inline Int_t GetTriggerSlope(Int_t Channel) { return(this->Get(TMrbSis_3300::kRegTriggerSlope, Channel)); };

		inline Bool_t SetPeakTime(Int_t Ticks, Int_t Group = -1) { return(this->Set(TMrbSis_3300::kRegPeakTime, Ticks, Group)); };
		inline Int_t GetPeakTime(Int_t Group) { return(this->Get(TMrbSis_3300::kRegPeakTime, Group)); };

		inline Bool_t SetGapTime(Int_t Ticks, Int_t Group = -1) { return(this->Set(TMrbSis_3300::kRegGapTime, Ticks, Group)); };
		inline Int_t GetGapTime(Int_t Group) { return(this->Get(TMrbSis_3300::kRegGapTime, Group)); };

		inline Bool_t EnablePulseMode(Bool_t Flag, Int_t Group = -1) { return(this->Set(TMrbSis_3300::kRegPulseMode, Flag ? 1 : 0, Group)); };
		inline Bool_t PulseModeEnabled(Int_t Group) { return(this->Get(TMrbSis_3300::kRegPulseMode, Group) == 1); };

		inline Bool_t SetPulseLength(Int_t Ticks, Int_t Group = -1) { return(this->Set(TMrbSis_3300::kRegPulseLength, Ticks, Group)); };
		inline Int_t GetPulseLength(Int_t Group) { return(this->Get(TMrbSis_3300::kRegPulseLength, Group)); };

		Bool_t SetSmin(Int_t Smin = 0);						// set/get sample limits
		Bool_t SetSmax(Int_t Smax = 0);
		inline Int_t GetSmin() { return(fSmin); };
		inline Int_t GetSmax() { return(fSmax); };

		inline Bool_t SetSampleBinning(Int_t PointsPerBin) {		// sample binning
			fSamplePointsPerBin = PointsPerBin;
			fSampleBinRange = (fSmax - fSmin + fSamplePointsPerBin - 1) / fSamplePointsPerBin;
			return(kTRUE);
		};
		inline Int_t GetSampleBinning() const { return(fSamplePointsPerBin); };
		inline Int_t GetSampleBinRange() const { return(fSampleBinRange); };

		Bool_t SetTmin(Int_t Tmin = 0);						// set/get trigger limits
		Bool_t SetTmax(Int_t Tmax = 0);
		inline Int_t GetTmin() { return(fTmin); };
		inline Int_t GetTmax() { return(fTmax); };

		inline Bool_t SetTriggerBinning(Int_t PointsPerBin) {		// trigger binning
			fTriggerPointsPerBin = PointsPerBin;
			fTriggerBinRange = (fTmax - fTmin + fTriggerPointsPerBin - 1) / fTriggerPointsPerBin;
			return(kTRUE);
		};
		inline Int_t GetTriggerBinning() const { return(fTriggerPointsPerBin); };
		inline Int_t GetTriggerBinRange() const { return(fTriggerBinRange); };

		inline void ShaperOn(Bool_t ShaperFlag = kTRUE) { fShaperOn = ShaperFlag; };	// shaper settings
		inline Bool_t ShaperIsOn() { return(fShaperOn); };

		Bool_t SetShmin(Int_t Shmin = -kShaperMinMax, Int_t ShaperIdx = kShapeShort);		// set/get shaper limits
		Bool_t SetShmax(Int_t Shmax = kShaperMinMax, Int_t ShaperIdx = kShapeShort);
		inline Int_t GetShmin() { return(fShmin); };
		inline Int_t GetShmax() { return(fShmax); };

		inline Bool_t SetShaperBinRange(Int_t BinRange) {		// shaper binning
			fShaperBinRange = BinRange;
			return(kTRUE);
		};
		inline Int_t GetShaperBinRange() const { return(fShaperBinRange); };

		Bool_t UseSettings(const Char_t * SettingsFile = NULL);
		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers

	protected:
		Bool_t fBlockXfer;
		Int_t fMaxEvents;
		Bool_t fTriggerOut;
		Bool_t fInvertTrigger;
		Bool_t fRouteTrigger;
		Bool_t fStartStopEnable;
		Bool_t fAutoStartEnable;
		Bool_t fAutoBankSwitch;
		Bool_t fWrapAroundEnable;
		Bool_t fTriggerArmed;

		Int_t fSampleRange;
		Int_t fTriggerRange;
		Int_t fShaperRange;

		Int_t fSmin;
		Int_t fSmax;
		Int_t fSamplePointsPerBin;
		Int_t fSampleBinRange;

		Int_t fTmin;
		Int_t fTmax;
		Int_t fTriggerPointsPerBin;
		Int_t fTriggerBinRange;
		Int_t fTriggerBaseLine;

		Bool_t fShaperOn;
		Int_t fShmin;
		Int_t fShmax;
		Int_t fShaperBinRange;
		Int_t fShaperBaseLine;

		TString fSettingsFile;

	ClassDef(TMrbSis_3300, 1)		// [Config] SIS 3300, VME digitizing adc
};

#endif
