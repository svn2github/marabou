#ifndef __TMrbModule_h__
#define __TMrbModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbModule.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModule           -- base class for camac & vme modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream.h>

#include "Rtypes.h"
#include "TSystem.h"
#include "TObject.h"
#include "TNamed.h"
#include "TString.h"
#include "TObjArray.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"

#include "TMrbConfig.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule
// Purpose:        Define methods for a camac or vme module
// Description:    Defines a camac or vme module to be used in the MARaBOU environment.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbModule : public TNamed {

	public:

		TMrbModule() {};								// default ctor

														// explicit ctor:	name, id, nofchannels, range
		TMrbModule(const Char_t * ModuleName, const Char_t * ModuleID, Int_t NofChannels, Int_t Range);

		~TMrbModule() {									// dtor
			fChannelSpec.Delete();
			DeleteRegisters();
		};

		Bool_t Set(const Char_t * RegName, Int_t Value, Int_t Channel = -1);	// set channel register by name
		Bool_t Set(Int_t RegIndex, Int_t Value, Int_t Channel = -1); 			// ... by index
		Int_t Get(const Char_t * RegName, Int_t Channel = -1);  				// get channel register by name
		Int_t Get(Int_t RegIndex, Int_t Channel = -1);				 			// ... by index

		Bool_t Set(const Char_t * RegName, const Char_t * Value, Int_t Channel = -1);	// set channel register mnemonically
		Bool_t Set(Int_t RegIndex, const Char_t * Value, Int_t Channel = -1); 	// ... by index

		inline TMrbNamedX * FindRegister(const Char_t * RegName) {				// find register by its name
			return(fLofRegisters.FindByName(RegName, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase));
		};
																				// find register by index
		inline TMrbNamedX * FindRegister(Int_t RegIndex) { return(fLofRegisters.FindByIndex(RegIndex)); };

		inline Bool_t HasRegisters() { return(fLofRegisters.GetEntriesFast() > 0); };	// check if there are some registers

		inline Bool_t AllocateHistograms(Bool_t Flag = kTRUE) { fHistosToBeAllocated = Flag; return(kTRUE); }; // create histograms?
		inline Bool_t HistosToBeAllocated() { return(fHistosToBeAllocated); };
		inline Bool_t ConvertToInt() { return(kTRUE); };						// convert short to int

		inline void SetActive(Bool_t ActiveFlag = kTRUE) { fIsActive = ActiveFlag; };
		inline Bool_t IsActive() { return(fIsActive); };						// module active?

		Bool_t SetBlockReadout(Bool_t Flag = kTRUE);							// turn block mode on
		inline Bool_t HasBlockReadout() { return(fBlockReadout); }; 			// check if block mode

		inline TMrbLofNamedX * GetLofRegisters() { return(&fLofRegisters); };	// list of registers

		inline Int_t GetRange() { return(fRange); };							// get module range
		inline TMrbNamedX * GetDataType() { return(fDataType); };				// get data type

		inline Bool_t SetBinning(Int_t PointsPerBin) {							// define bin size for histograms
			fPointsPerBin = PointsPerBin;
			fBinRange = (fRange + fPointsPerBin - 1) / fPointsPerBin;
			return(kTRUE);
		};

		inline Int_t GetBinning() { return(fPointsPerBin); };					// get bin values
		inline Int_t GetBinRange() { return(fBinRange); };

		inline Int_t GetSubDevice() { return(fSubDevice); };					// get subdevice number

		void SetType(UInt_t ModuleType, Bool_t OrFlag = kTRUE);					// set module type
		inline TMrbNamedX * GetType() { return(&fModuleType); }; 				// return type bits
		inline TMrbNamedX * GetModuleID() { return(&fModuleID); }; 				// return id

		virtual inline const Char_t * GetMnemonic() { return("unknown"); }; 	// module mnemonic (usually 3 to 4 char code)

		inline Int_t GetCrate() { return(fCrate); };							// return crate number
		inline Int_t GetSerial() { return(fSerial); };  	 					// return unique module id

		inline void SetTimeOffset(Int_t TimeOffset) { fTimeOffset = TimeOffset; };	// time offset
		inline Int_t GetTimeOffset() { return(fTimeOffset); };

		inline Bool_t CheckID(TMrbConfig::EMrbModuleID ModuleID) { return(fModuleID.GetIndex() == ModuleID); };
		
		inline Int_t GetNofShortsPerChannel() { return(fNofShortsPerChannel); };	// 16 bit words per channel

		inline Int_t GetNofChannels() { return(fNofChannels); }; 				// max number of channels
		Int_t GetNofChannelsUsed(); 											// calculate number of channels in use
		UInt_t GetPatternOfChannelsUsed(); 										// pattern of channels actually used

		inline TObjArray * GetLofChannels() { return(&fChannelSpec); };			// list of channels

		TObject * GetChannel(Int_t);											// get channel addr

		Bool_t LoadCodeTemplates(const Char_t * TemplateFile);					// load code templates

		virtual Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL) { return(kFALSE); }; // generate readout code

		virtual Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) { return(kFALSE); };  	// generate code for given channel
		virtual Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TObject * Subevent, Int_t Value = 0) { return(kFALSE); }; 

		virtual Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);	// generate part of analyzing code

		virtual Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) { return(kFALSE); };
		
		inline Bool_t IsCamac() { return((fModuleType.GetIndex() & TMrbConfig::kModuleCamac) != 0); }; // camac or vme?
		inline Bool_t IsVME() { return((fModuleType.GetIndex() & TMrbConfig::kModuleVME) != 0); }; 	// ...

		void PrintRegister(ostream & OutStrm, const Char_t * RegName, const Char_t * Prefix = "");	 // show register settings
		inline void PrintRegister(const Char_t * RegName) { PrintRegister(cout, RegName, ""); };

		virtual inline Int_t GetNofSubDevices() { return(1); }; 						// 1 subdevice per default
		virtual inline Bool_t HasRandomReadout() { return(kTRUE); };					// modules allow random readout normally
		virtual inline Bool_t IsRaw() { return(kFALSE); };								// not raw (user-defined) mode
		virtual inline Bool_t CheckSubeventType(TObject * Subevent) { return(kTRUE); }; // module may be stored in any subevent

		virtual inline Bool_t HasPrivateCode() { return(kFALSE); }; 					// normal code generation
		virtual inline const Char_t * GetPrivateCodeFile() { return(NULL); };
		virtual inline const Char_t * GetCommonCodeFile() { return(NULL); };
		
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbModule.html&"); };

	protected:
		virtual void DefineRegisters() {};
		void DeleteRegisters();

	protected:
		TMrbNamedX fModuleID;					// module id
		TMrbNamedX fModuleType;					// module type
		Int_t fSerial;							// unique serial number
		Bool_t UtilitiesLoaded; 				// kTRUE if some utility procedures already loaded
		Int_t fNofChannels;						// number of channels available

		Bool_t fIsActive; 						// kTRUE if module active

		TMrbNamedX * fDataType;					// data type
		Int_t fNofShortsPerChannel; 			// number of 16 bit words per channel

		Int_t fCrate;							// crate number

		Int_t fRange;							// number of data points
		Int_t fPointsPerBin;					// points per bin
		Int_t fBinRange;						// number of bins
		Int_t fSubDevice;						// subdevice

		Int_t fTimeOffset;						// time offset

		Bool_t fBlockReadout;					// kTRUE if block readout;

		Bool_t fHistosToBeAllocated;			// kTRUE if histograms to be allocated for each channel

		TMrbLofNamedX fLofRegisters;	 		// list of registers

		TMrbTemplate fCodeTemplates; 			// store template code

		TObjArray fChannelSpec;					// channel specifications (type TMrbModuleChannel *)

	ClassDef(TMrbModule, 1) 	// [Config] Base class describing a module (CAMAC or VME)
};	

#endif
