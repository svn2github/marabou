#ifndef __TMrbModule_h__
#define __TMrbModule_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbModule.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbModule           -- base class for camac & vme modules
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbModule.h,v 1.19 2008-12-04 14:53:11 Rudolf.Lutter Exp $       
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
#include "TNamed.h"
#include "TString.h"
#include "TObjArray.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"

#include "TMrbConfig.h"

class TMrbSubevent;
class TMrbModuleChannel;

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
		Int_t Get(const Char_t * RegName, Int_t Channel = -1) const;  			// get channel register by name
		Int_t Get(Int_t RegIndex, Int_t Channel = -1) const;				 	// ... by index

		Bool_t Set(const Char_t * RegName, const Char_t * Value, Int_t Channel = -1);	// set channel register mnemonically
		Bool_t Set(Int_t RegIndex, const Char_t * Value, Int_t Channel = -1); 	// ... by index

		inline TMrbNamedX * FindRegister(const Char_t * RegName) const {				// find register by its name
			return(fLofRegisters.FindByName(RegName, TMrbLofNamedX::kFindUnique | TMrbLofNamedX::kFindIgnoreCase));
		};
																				// find register by index
		inline TMrbNamedX * FindRegister(Int_t RegIndex) const { return(fLofRegisters.FindByIndex(RegIndex)); };

		inline Bool_t HasRegisters() const { return(fLofRegisters.GetEntriesFast() > 0); };	// check if there are some registers

		inline Bool_t AllocateHistograms(Bool_t Flag = kTRUE) { fHistosToBeAllocated = Flag; return(kTRUE); }; // create histograms?
		inline Bool_t HistosToBeAllocated() const { return(fHistosToBeAllocated); };
		inline Bool_t ConvertToInt() const { return(kTRUE); };						// convert short to int

		inline void SetActive(Bool_t ActiveFlag = kTRUE) { fIsActive = ActiveFlag; };
		inline Bool_t IsActive() const { return(fIsActive); };						// module active?

		inline void ExcludeFromReadout(Bool_t ExcludeFlag = kTRUE) { fExcludeFromReadout = ExcludeFlag; };
		inline Bool_t ToBeExcludedFromReadout() const { return(fExcludeFromReadout); };						// module active?

		Bool_t SetBlockReadout(Bool_t Flag = kTRUE);							// turn block mode on
		inline Bool_t HasBlockReadout() const { return(fBlockReadout); }; 			// check if block mode

		inline TMrbLofNamedX * GetLofRegisters() { return(&fLofRegisters); };	// list of registers

		inline Int_t GetRange() { return(fRange); };							// get module range
		inline TMrbNamedX * GetDataType() const { return(fDataType); };				// get data type

		inline Bool_t SetBinning(Int_t PointsPerBin = 1) {						// define bin size for histograms
			fPointsPerBin = PointsPerBin;
			fBinRange = (fXmax - fXmin + fPointsPerBin - 1) / fPointsPerBin;
			return(kTRUE);
		};

		inline Int_t GetBinning() const { return(fPointsPerBin); };				// get bin values
		inline Int_t GetBinRange() const { return(fBinRange); };

		Bool_t SetXmin(Int_t Xmin = 0);											// set min / max in X
		Bool_t SetXmax(Int_t Xmax = 0);
		inline Int_t GetXmin() { return(fXmin); };
		inline Int_t GetXmax() { return(fXmax); };

		inline Int_t GetSubDevice() const { return(fSubDevice); };					// get subdevice number

		void SetType(UInt_t ModuleType, Bool_t OrFlag = kTRUE);					// set module type
		inline TMrbNamedX * GetType() { return(&fModuleType); }; 				// return type bits
		inline TMrbNamedX * GetModuleID() { return(&fModuleID); }; 				// return id

		virtual inline const Char_t * GetMnemonic() const { return("unknown"); }; 	// module mnemonic (usually 3 to 4 char code)

		inline const Char_t * GetPosition() const { return(fPosition.Data()); };

		inline Int_t GetCrate() const { return(fCrate); };							// return crate number
		inline Int_t GetSerial() const { return(fSerial); };  	 					// return unique module id

		Bool_t SetMbsBranch(Int_t MbsBranchNo, const Char_t * MbsBranchName = NULL);	// mbs branch
		inline Bool_t SetMbsBranch(TMrbNamedX * MbsBranch) { return(this->SetMbsBranch(MbsBranch->GetIndex(), MbsBranch->GetName())); };
		inline TMrbNamedX * GetMbsBranch() { return(&fMbsBranch); };
		inline Int_t GetMbsBranchNo() { return(fMbsBranch.GetIndex()); };

		inline void SetTimeOffset(Int_t TimeOffset) { fTimeOffset = TimeOffset; };	// time offset
		inline Int_t GetTimeOffset() const { return(fTimeOffset); };

		inline Bool_t CheckID(TMrbConfig::EMrbModuleID ModuleID) const { return(fModuleID.GetIndex() == ModuleID); };
		
		inline Int_t GetNofShortsPerChannel() const { return(fNofShortsPerChannel); };	// 16 bit words per channel
		inline Int_t GetNofShortsPerDatum() const { return(fNofShortsPerDatum); };		// 16 bit words per datum
		inline Int_t GetNofDataBits() const { return(fNofDataBits); };					// data bits

		inline Int_t GetNofChannels() const { return(fNofChannels); }; 				// max number of channels
		Int_t GetNofChannelsUsed() const; 											// calculate number of channels in use
		UInt_t GetPatternOfChannelsUsed() const; 										// pattern of channels actually used
		Bool_t CheckIfPatternIsContiguous() const;						// check if channels have been assigned contiguously 

		inline TObjArray * GetLofChannels() { return(&fChannelSpec); };			// list of channels

		TMrbModuleChannel * GetChannel(Int_t) const;											// get channel addr

		Bool_t LoadCodeTemplates(const Char_t * TemplateFile);					// load code templates

		virtual Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL) { return(kFALSE); }; // generate readout code

		virtual Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) { return(kFALSE); };  	// generate code for given channel
		virtual Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex, TMrbModuleChannel * Channel, Int_t Value = 0) { return(kFALSE); }; 

		virtual Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);	// generate part of analyzing code

		virtual Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) { return(kFALSE); };
		
		inline Bool_t IsCamac() const { return((fModuleType.GetIndex() & TMrbConfig::kModuleCamac) != 0); }; // camac or vme?
		inline Bool_t IsVME() const { return((fModuleType.GetIndex() & TMrbConfig::kModuleVME) != 0); }; 	// ...

		void PrintRegister(ostream & OutStrm, const Char_t * RegName, const Char_t * Prefix = "") const;	 // show register settings
		inline void PrintRegister(const Char_t * RegName) const { PrintRegister(cout, RegName, ""); };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, const Char_t * Prefix = "") const {};	// show data
		inline virtual void Print() const { Print(cout, ""); };

		virtual inline Int_t GetNofSubDevices() const { return(1); }; 						// 1 subdevice per default
		virtual inline Bool_t HasRandomReadout() const { return(kTRUE); };					// modules allow random readout normally
		virtual inline Bool_t IsRaw() const { return(kFALSE); };								// not raw (user-defined) mode
		virtual inline Bool_t CheckSubeventType(TMrbSubevent * Subevent) const { return(kTRUE); }; // module may be stored in any subevent

		virtual inline Bool_t HasPrivateCode() const { return(kFALSE); }; 					// normal code generation
		virtual inline const Char_t * GetPrivateCodeFile() const { return(NULL); };
		virtual inline const Char_t * GetCommonCodeFile() const { return(NULL); };
		
		inline void CreateHistoArray(const Char_t * ArrayName = NULL) {		// create array of histograms and write .histlist file
			fCreateHistoArray = kTRUE;
			fHistoArrayName = ArrayName ? ArrayName : this->GetName();
		}
		inline Bool_t HistoArrayToBeCreated() { return(fCreateHistoArray); };
		inline const Char_t * GetHistoArrayName() { return(fHistoArrayName.Data()); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

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
		Bool_t fExcludeFromReadout; 			// kTRUE if to be EXcluded from readout

		TMrbNamedX * fDataType;					// data type
		Int_t fNofShortsPerChannel; 			// number of 16 bit words per channel
		Int_t fNofShortsPerDatum;  				// number of 16 bit words per datum
		Int_t fNofDataBits; 					// length of data

		TMrbNamedX fMbsBranch;					// branch within MBS

		Int_t fCrate;							// crate number

		TMrbString fPosition;					// crate & addr

		Int_t fRange;							// number of data points
		Int_t fXmin;							// xmin / xmax
		Int_t fXmax;
		Int_t fPointsPerBin;					// points per bin
		Int_t fBinRange;						// number of bins
		Int_t fSubDevice;						// subdevice

		Int_t fTimeOffset;						// time offset

		Bool_t fBlockReadout;					// kTRUE if block readout;

		Bool_t fHistosToBeAllocated;			// kTRUE if histograms to be allocated for each channel

		TMrbLofNamedX fLofRegisters;	 		// list of registers

		TMrbTemplate fCodeTemplates; 			// store template code

		Bool_t fCreateHistoArray;				// kTRUE if histo array / .histlist file is to be created
		TString fHistoArrayName;				// name of histo array / .histlist file

		TObjArray fChannelSpec;					// channel specifications (type TMrbModuleChannel *)

	ClassDef(TMrbModule, 1) 	// [Config] Base class describing a module (CAMAC or VME)
};	

#endif
