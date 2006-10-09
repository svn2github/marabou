#ifndef __TMrbXia_DGF_4C_h__
#define __TMrbXia_DGF_4C_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbXia_DGF_4C.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbXia_DGF_4C     -- xia's digital gamma finder
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbXia_DGF_4C.h,v 1.19 2006-10-09 10:30:35 Rudolf.Lutter Exp $       
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

#include "TMrbCamacModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C
// Purpose:        Define a module of type Xia DGF-4C
// Description:    Defines an module of type Xia DGF-4C.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbXia_DGF_4C : public TMrbCamacModule {

	public:

		TMrbXia_DGF_4C() {};														// default ctor
		TMrbXia_DGF_4C(const Char_t * ModuleName, const Char_t * ModulePosition);	// define a new dgf module
		virtual ~TMrbXia_DGF_4C() {};												// remove module from list

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex);  	// generate part of code
		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Channel, Int_t Value = 0);  	// generate code for given channel

		Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);	// generate part of analyzing code

		Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName);
		
		virtual inline const Char_t * GetMnemonic() const { return("dgf"); }; 	// module mnemonic

		virtual Bool_t CheckSubeventType(TObject * Subevent) const;		// check if subevent type is [10,2x]

		Bool_t ReadNameTable(); 							 		// read parameter names from file
		inline TMrbNamedX * FindParam(const Char_t * ParamName) const { return((TMrbNamedX *) fParamNames.FindByName(ParamName)); };
		inline TMrbNamedX * FindParam(Int_t Offset) const { return((TMrbNamedX *) fParamNames.FindByIndex(Offset)); };
		TMrbNamedX * FindParam(Int_t Channel, const Char_t * ParamName);

		inline TMrbNamedX * FirstParam() const { return((TMrbNamedX *) fParamNames.First()); };
		inline TMrbNamedX * NextParam(TMrbNamedX * Param) const { return((TMrbNamedX *) fParamNames.After(Param)); };
		inline Int_t GetNofParams() const { return(fNofParams); };

		virtual inline Bool_t HasRandomReadout() const { return(kFALSE); };	// block mode only

		inline void SetMaxEvents(Int_t MaxEvents) { fMaxEvents = MaxEvents; };
		inline Int_t GetMaxEvents() const { return(fMaxEvents); };
		inline void SetTraceLength(Int_t TraceLength = 40) { fTraceLength = TraceLength; };
		inline Int_t GetTraceLength() const { return(fTraceLength); };
		inline void SetRunTask(Int_t RunTask = 0x100) { fRunTask = RunTask; };
		inline Int_t GetRunTask() const { return(fRunTask); };
		inline void SetSynchWait(Int_t SynchWait = 1) { fSynchWait = SynchWait; };
		inline Int_t GetSynchWait() const { return(fSynchWait); };
		inline void SetInSynch(Int_t InSynch = 0) { fInSynch = InSynch; };
		inline Int_t GetInSynch() const { return(fInSynch); };
		inline void SetSwitchBusTerm(Bool_t IndivFlag = kTRUE, Bool_t TermFlag = kTRUE, Bool_t MasterFlag = kTRUE) {
							fSwitchBusIndiv = IndivFlag;
							fSwitchBusTerm = TermFlag; 
							fSwitchBusTermIfMaster = MasterFlag; 
		};
		inline Bool_t SwitchBusToBeTerminated() { return(fSwitchBusTerm); };
		inline Bool_t SwitchBusToBeTerminatedIfMaster() { return(fSwitchBusTermIfMaster); };
		inline void ActivateUserPSACode(Bool_t ActiveFlag = kTRUE) { fActivateUserPSA = ActiveFlag; };
		inline Bool_t UserPSAIsActive() const { return(fActivateUserPSA); };

		inline void SetClusterID(Int_t Serial, const Char_t * Color, const Char_t * Segments, Int_t HexNum = 0) { // cluster ids
			fClusterID.Set((Int_t) (Serial << 12) + HexNum, Color, Segments);
		};
		inline Int_t GetClusterSerial() const { return((Int_t) (fClusterID.GetIndex() >> 12)); }; 		// use TMrbNamedX object:
		inline Int_t GetClusterHexNum() const { return(fClusterID.GetIndex() & 0xFFF); }; 		// use TMrbNamedX object:
		inline const Char_t * GetClusterColor() const { return(fClusterID.GetName()); };	// index <- serial, name <- color
		inline const Char_t * GetClusterSegments() const { return(fClusterID.GetTitle()); };	// title <- segment info

		inline Int_t GetRelease() const { return(fRelease); };								// return xia release
		inline Bool_t DSPLoadedFromBinary() const { return(fDSPLoadedFromBinary); };			// return dsp format
		
		Bool_t SetChannelLayout(const Char_t * LayoutName, Int_t NofChannels, const Char_t * ChannelNames); // define channel names to be used
		const Char_t * GetChannelName(Int_t Channel, const Char_t * Layout = "6fold");					// get channel name from layout
		const Char_t * GetChannelLayout(Int_t ModuleNumber, const Char_t * LayoutName = "6fold");		// get channel layout per module

		virtual inline Bool_t HasPrivateCode() const { return(kTRUE); }; 							// use private code files
		
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void DefineRegisters(); 			// define camac registers

	protected:
		Int_t fRelease;						// xia release
		Bool_t fDSPLoadedFromBinary;		// dsp format
		
		TString fParamFile; 				// file containing param names
		Int_t fNofParams;					// number of params
		TMrbLofNamedX fParamNames; 			// DSP param table, pairs of (ParamName, Offset)

		TMrbNamedX fClusterID;				// cluster serial, color, segment info

		TMrbLofNamedX fLofChannelLayouts; 	// layouts how to name dgf channels

		Int_t fMaxEvents;					// parameter MAXEVENTS
		Int_t fTraceLength; 				// parameter TRACELENGTH (common to all 4 channels)
		Int_t fRunTask; 					// parameter RUNTASK
		Int_t fSynchWait;					// parameter SYNCHWAIT
		Int_t fInSynch; 					// parameter INSYNCH

		Bool_t fSwitchBusTerm;				// kTRUE if switch bus is to be terminated
		Bool_t fSwitchBusTermIfMaster;		// kTRUE if switch bus is to be terminated for a master module
		Bool_t fSwitchBusIndiv;				// kTRUE if switch bus is to be terminated individually
		Bool_t fActivateUserPSA;			// kTRUE if user PSA code should be activated

	ClassDef(TMrbXia_DGF_4C, 1)	// [Config] Xia DGF-4C, digital gamma finder
};

#endif
