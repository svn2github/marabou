#ifndef __TMrbXia_DGF_4C_h__
#define __TMrbXia_DGF_4C_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbXia_DGF_4C.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbXia_DGF_4C     -- xia's digital gamma finder
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

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

		Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName);
		
		virtual inline const Char_t * GetMnemonic() { return("dgf"); }; 	// module mnemonic

		virtual Bool_t CheckSubeventType(TObject * Subevent);		// check if subevent type is [10,2x]

		Bool_t ReadNameTable(); 							 		// read parameter names from file
		inline TMrbNamedX * FindParam(const Char_t * ParamName) { return((TMrbNamedX *) fParamNames.FindByName(ParamName)); };
		inline TMrbNamedX * FindParam(Int_t Offset) { return((TMrbNamedX *) fParamNames.FindByIndex(Offset)); };
		TMrbNamedX * FindParam(Int_t Channel, const Char_t * ParamName);

		inline TMrbNamedX * FirstParam() { return((TMrbNamedX *) fParamNames.First()); };
		inline TMrbNamedX * NextParam(TMrbNamedX * Param) { return((TMrbNamedX *) fParamNames.After(Param)); };
		inline Int_t GetNofParams() { return(fNofParams); };

		virtual inline Bool_t HasRandomReadout() { return(kFALSE); };	// block mode only

		inline void SetMaxEvents(Int_t MaxEvents) { fMaxEvents = MaxEvents; };
		inline Int_t GetMaxEvents() { return(fMaxEvents); };
		inline void SetTraceLength(Int_t TraceLength = 0) { fTraceLength = TraceLength; };
		inline Int_t GetTraceLength() { return(fTraceLength); };
		inline void SetRunTask(Int_t RunTask = 0x100) { fRunTask = RunTask; };
		inline Int_t GetRunTask() { return(fRunTask); };
		inline void SetSwitchBusTerm(Bool_t TermFlag = kTRUE) { fSwitchBusTerm = TermFlag; };
		inline Bool_t SwitchBusToBeTerminated() { return(fSwitchBusTerm); };
		
		inline void SetClusterID(Int_t Serial, const Char_t * Color, const Char_t * Segments) { // cluster ids
			fClusterID.Set(Serial, Color, Segments);
		};
		inline Int_t GetClusterSerial() { return(fClusterID.GetIndex()); }; 		// use TMrbNamedX object:
		inline const Char_t * GetClusterColor() { return(fClusterID.GetName()); };	// index <- serial, name <- color
		inline const Char_t * GetClusterSegments() { return(fClusterID.GetTitle()); };	// title <- segment info

		inline Int_t GetRelease() { return(fRelease); };								// return xia release
		inline Bool_t DSPLoadedFromBinary() { return(fDSPLoadedFromBinary); };			// return dsp format
		
		virtual inline Bool_t HasPrivateCode() { return(kTRUE); }; 						// use private code files
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbXia_DGF_4C.html&"); };

	protected:
		void DefineRegisters(); 			// define camac registers

	protected:
		Int_t fRelease;						// xia release
		Bool_t fDSPLoadedFromBinary;		// dsp format
		
		TString fParamFile; 				// file containing param names
		Int_t fNofParams;					// number of params
		TMrbLofNamedX fParamNames; 			// DSP param table, pairs of (ParamName, Offset)

		TMrbNamedX fClusterID;				// cluster serial, color, segment info

		Int_t fMaxEvents;					// parameter MAXEVENTS
		Int_t fTraceLength; 				// parameter TRACELENGTH (common to all 4 channels)
		Int_t fRunTask; 					// parameter RUNTASK

		Bool_t fSwitchBusTerm;				// kTRUE if switch bus is to be terminated

	ClassDef(TMrbXia_DGF_4C, 1)	// [Config] Xia DGF-4C, digital gamma finder
};

#endif
