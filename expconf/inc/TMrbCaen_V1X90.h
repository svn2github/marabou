#ifndef __TMrbCaen_V1X90_h__
#define __TMrbCaen_V1X90_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbCaen_V1X90.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbCaen_V1X90        -- 128 chn TDC
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbCaen_V1X90.h,v 1.1 2009-07-20 07:09:43 Marabou Exp $       
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
#include "TArrayS.h"

#include "TMrbVMEModule.h"

class TMrbSubevent;
class TMrbVMEChannel;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCaen_V1X90
// Purpose:        Define a VME adc type CAEN V1X90
// Description:    Defines a VME adc CAEN V1X90
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbCaen_V1X90 : public TMrbVMEModule {

	public:
		enum				{	kSegSize			=	0x10000	};
		enum				{	kAddrMod			=	0x39	};

		enum				{	kAllChannels		=	-1		};
		enum				{	kNofChannels		=	128		};

		enum EMrbRegisters	{	kRegTriggerMode,
								kRegWindowWidth,
								kRegWindowOffset,
								kRegSearchMargin,
								kRegRejectMargin,
								kRegTrigTimeSubtr,
								kRegEdgeDetection,
								kRegEdgeResolution,
								kRegPairResolutionEdge,
								kRegPairResolutionWidth,
								kRegDeadTime,
								kRegHeaderTrailer,
								kRegEventSize,
								kRegFifoSize,
								kRegChannelEnable,
								kRegAlmostFullLevel
							};
		
		enum EMrbEdgeDetect {	kEdgeDetectPair,
								kEdgeDetectTrailing,
								kEdgeDetectLeading,
								kEdgeDetectBoth
							};
		enum				{	kEdgeDetectDefault	=	kEdgeDetectLeading	};

		enum EMrbEdgeRes	{	kEdgeRes800,
								kEdgeRes200,
								kEdgeRes100,
								kEdgeResOff
							};
		enum				{	kEdgeResDefault	=	kEdgeRes100	};

		enum EMrbPairRes	{	kPairRes100,
								kPairRes200,
								kPairRes400,
								kPairRes800,
								kPairRes1600,
								kPairRes3125,
								kPairRes6250,
								kPairRes12500,
								kPairRes25ns,
								kPairRes50ns,
								kPairRes100ns,
								kPairRes200ns,
								kPairRes400ns,
								kPairRes800ns
							};
		enum				{	kPairResDefault	=	kPairRes100	};

		enum EMrbDeadTime 	{	kDtime5,
								kDtime10,
								kDtime30,
								kDtime100
							};
		enum				{	kDtimeDefault	=	kDtime5	};

		enum EMrbEventSize 	{	kEvtSiz0,
								kEvtSiz1,
								kEvtSiz2,
								kEvtSiz4,
								kEvtSiz8,
								kEvtSiz16,
								kEvtSiz32,
								kEvtSiz64,
								kEvtSiz128,
								kEvtSizNoLim,
							};
		enum				{	kEvtSizDefault	=	kEvtSizNoLim	};

		enum EMrbFifoSize 	{	kFifoSiz2,
								kFifoSiz4,
								kFifoSiz8,
								kFifoSiz16,
								kFifoSiz32,
								kFifoSiz64,
								kFifoSiz128,
								kFifoSiz256
							};
		enum				{	kFifoSizDefault	=	kFifoSiz256	};

		enum				{	kWindowWidthDefault =		20	};
		enum				{	kWindowOffsetDefault =		-40	};
		enum				{	kSearchMarginDefault =		8	};
		enum				{	kRejectMarginDefault =		4	};
		enum				{	kAlmostFullLevelDefault =	64	};

	public:

		TMrbCaen_V1X90() {};  												// default ctor
		TMrbCaen_V1X90(const Char_t * ModuleName, UInt_t BaseAddr); 	// define a new adc
		~TMrbCaen_V1X90() {};												// default dtor

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbVMEChannel * Channel, Int_t Value = 0);  	// generate code for given channel

		virtual inline const Char_t * GetMnemonic() const { return("caen_V1X90"); }; 	// module mnemonic

		inline void SetTriggerMatchingOn(Bool_t Flag = kTRUE) { fTriggerMatching = Flag; };
		inline Bool_t TriggerMatchingIsOn() { return(fTriggerMatching); };

		inline Bool_t SetWindowWidthTicks(Int_t Width) { return(this->Set(TMrbCaen_V1X90::kRegWindowWidth, Width)); };
		inline Int_t GetWindowWidthTicks() { return(this->Get(TMrbCaen_V1X90::kRegWindowWidth)); };
		inline Int_t GetWindowWidthNsec() { return(this->GetWindowWidthTicks() * 25); };

		inline Bool_t SetWindowOffsetTicks(Int_t Offset) { return(this->Set(TMrbCaen_V1X90::kRegWindowOffset, Offset)); };
		inline Int_t GetWindowOffsetTicks() { return(this->Get(TMrbCaen_V1X90::kRegWindowOffset)); };
		inline Int_t GetWindowOffsetNsec() { return(this->GetWindowOffsetTicks() * 25); };

		inline Bool_t SetSearchMarginTicks(Int_t Margin) { return(this->Set(TMrbCaen_V1X90::kRegSearchMargin, Margin)); };
		inline Int_t GetSearchMarginTicks() { return(this->Get(TMrbCaen_V1X90::kRegSearchMargin)); };
		inline Int_t GetSearchMarginNsec() { return(this->GetSearchMarginTicks() * 25); };

		inline Bool_t SetRejectMarginTicks(Int_t Margin) { return(this->Set(TMrbCaen_V1X90::kRegRejectMargin, Margin)); };
		inline Int_t GetRejectMarginTicks() { return(this->Get(TMrbCaen_V1X90::kRegRejectMargin)); };
		inline Int_t GetRejectMarginNsec() { return(this->GetRejectMarginTicks() * 25); };

		inline void EnableTriggerTimeSubtraction(Bool_t Flag) { fSubtractTrigTime = Flag; };
		inline Bool_t TriggerTimeSubtractionEnabled() { return(fSubtractTrigTime); };


		Bool_t SetResolution(Int_t Mode, Int_t Eticks, Int_t Wticks = -1);
		TMrbNamedX * GetEdgeDetection();
		TMrbNamedX * GetEdgeResolution();
		TMrbNamedX * GetPairResolution();
		TMrbNamedX * GetPairWidth();

		inline void EnableErrorMark(Bool_t Flag = kTRUE) { fErrorMark = Flag; };
		inline Bool_t ErrorMarkEnabled() { return(fErrorMark); };

		inline Bool_t SetDeadTime(Int_t Ticks) { return(this->Set(TMrbCaen_V1X90::kRegDeadTime, Ticks)); };
		TMrbNamedX * GetDeadTime();

		inline Bool_t SetEventSize(Int_t Size) { return(this->Set(TMrbCaen_V1X90::kRegEventSize, Size)); };
		TMrbNamedX * GetEventSize();

		inline Bool_t SetFifoSize(Int_t Size) { return(this->Set(TMrbCaen_V1X90::kRegFifoSize, Size)); };
		TMrbNamedX * GetFifoSize();

		inline Bool_t SetAlmostFullLevel(Int_t Level) { return(this->Set(TMrbCaen_V1X90::kRegAlmostFullLevel, Level)); };
		inline Int_t GetAlmostFullLevel() { return(this->Get(TMrbCaen_V1X90::kRegAlmostFullLevel)); };

		inline void EnableHeaderTrailer(Bool_t Flag = kTRUE) { fHeaderTrailer = Flag; };
		inline Bool_t HeaderTrailerEnabled() { return(fHeaderTrailer); };

		inline void EnableExtendedTriggerTag(Bool_t Flag = kTRUE) { fExtendedTriggerTag = Flag; };
		inline Bool_t ExtendedTriggerTagEnabled() { return(fExtendedTriggerTag); };

		inline void EnableEmptyEvent(Bool_t Flag = kTRUE) { fEmptyEvent = Flag; };
		inline Bool_t EmptyEventEnabled() { return(fEmptyEvent); };

		inline void EnableEventFifo(Bool_t Flag = kTRUE) { fEventFifo = Flag; };
		inline Bool_t EventFifoEnabled() { return(fEventFifo); };

		Bool_t EnableChannel(Int_t Channel = kAllChannels);
		Bool_t EnableChannel(Int_t ChannelFrom, Int_t ChannelTo);
		Bool_t DisableChannel(Int_t Channel = kAllChannels);
		Bool_t DisableChannel(Int_t ChannelFrom, Int_t ChannelTo);
		Bool_t IsChannelEnabled(Int_t Channel);
		inline TArrayS * GetChannelPattern() { return(&fChannelPattern); };

		Bool_t UseSettings(const Char_t * SettingsFile = NULL);
		Bool_t SaveSettings(const Char_t * SettingsFile = NULL);

		inline void SetVerboseMode(Bool_t Flag = kTRUE) { fVerboseMode = Flag; };
		inline Bool_t IsVerboseMode() { return(fVerboseMode); };
		inline void SetVerify(Bool_t Flag = kTRUE) { fVerify = Flag; };
		inline Bool_t IsVerify() { return(fVerify); };
		inline void DumpRegisters(Bool_t Flag = kTRUE) { fDumpRegisters = Flag; };
		inline Bool_t RegistersToBeDumped() { return(fDumpRegisters); };

		inline void UpdateSettings(Bool_t Flag = kTRUE) { fUpdateSettings = Flag; };	// update settings
		inline Bool_t SettingsToBeUpdated() { return(fUpdateSettings); };
		inline void SetUpdateInterval(Int_t Interval = 0) { fUpdateInterval = Interval; };
		inline Int_t GetUpdateInterval() { return(fUpdateInterval); };

		void PrintSettings(ostream & OutStrm);
		inline void PrintSettings() { this->PrintSettings(cout); };

		inline Char_t * GetDeviceStruct() { return(Form("s_%s", this->GetName())); };

 		virtual Bool_t CheckSubeventType(TMrbSubevent * Subevent) const;		// check if subevent type is [10,4x]

		inline Bool_t HasRandomReadout() const { return(kFALSE); };

		virtual inline Bool_t HasPrivateCode() const { return(kTRUE); }; 			// use private code files
		virtual inline const Char_t * GetPrivateCodeFile() const { return("Module_Caen_V1X90"); };
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 							// define vme registers
		void SetChannelEnable(Int_t Channel);

	protected:
		Bool_t fVerboseMode;
		Bool_t fVerify;
		Bool_t fDumpRegisters;

		Bool_t fUpdateSettings;
		Int_t fUpdateInterval;

		Bool_t fTriggerMatching;
		Bool_t fSubtractTrigTime;
		Bool_t fHeaderTrailer;
		Bool_t fExtendedTriggerTag;
		Bool_t fEmptyEvent;
		Bool_t fEventFifo;
		Bool_t fErrorMark;

		TMrbNamedX * fEdgeDetection;
		TMrbNamedX * fResolution;
		TMrbNamedX * fDoubleHitRes;
		TMrbNamedX * fPairResEdge;
		TMrbNamedX * fPairResWidth;
		TMrbNamedX * fEventSize;
		TArrayS fChannelPattern;

		TString fSettingsFile;

	ClassDef(TMrbCaen_V1X90, 1)		// [Config] CAEN V1X90, 128 chn VME TDC
};

#endif
