#ifndef __TMrbSubevent_h__
#define __TMrbSubevent_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent         -- base class for subevents
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

#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"
#include "TMrbString.h"
#include "TMrbEvent.h"
#include "TMrbModule.h"
#include "TMrbCamacModule.h"
#include "TMrbVMEModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent
// Purpose:        Define methods for a subevent
// Description:    Defines how to generate code for a subevent
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSubevent : public TNamed {

	public:

		TMrbSubevent() {};									// default ctor

		TMrbSubevent(const Char_t * SevtName, const Char_t * SevtTitle = "", Int_t Crate = -1);	// explicit ctor

		~TMrbSubevent() {									// dtor
			fLofParams.Delete();							// remove objects stored in TMrbLofNamdeXs
			fLofModules.Delete();
		};

		TMrbSubevent(const TMrbSubevent &) {};			// default copy ctor

		inline void AddEvent(TObject * Evt) {		 	// add a parent event
			fLofEvents.Add(Evt);
			fNofEvents++;
		};

		inline TObject * FindEvent(const Char_t * EvtName) {	// find a parent
			return (fLofEvents.FindObject(EvtName));
		};

		Bool_t AddParam(TObject * Param);			 	// add a new parameter

		inline TObject * FindParam(const Char_t * ParamName) {		// find param by its name
			return(fLofParams.FindObject(ParamName));
		};

		inline Bool_t AddModule(TObject * Module) {		 	// add a new module
			fLofModules.Add(Module);
			fNofModules++;
			return(kTRUE);
		};

		inline TObject * NextModule(TObject * After = NULL) {	// get next module from list
			return((After == NULL) ? fLofModules.First() : fLofModules.After(After));
		};

		inline TObject * FindModule(const Char_t * ModuleName) {	// find a module
			return (fLofModules.FindObject(ModuleName));
		};

		TObject * FindModuleByID(TMrbConfig::EMrbModuleID ModuleID, TObject * After = NULL); 		// find module by its id
		inline TObject * FindModuleByID(TMrbNamedX * ModuleID, TObject * After = NULL) {
			return(this->FindModuleByID((TMrbConfig::EMrbModuleID) ModuleID->GetIndex(), After));
		};

		TObject * FindModuleByType(UInt_t ModuleType, TObject * After = NULL); 	// find module by its type
		TObject * FindModuleByType(TMrbNamedX * ModuleType, TObject * After = NULL) {
			return(this->FindModuleByType(ModuleType->GetIndex(), After));
		};

		TObject * FindModuleBySerial(Int_t ModuleSerial);				 		// find module by its serial number

		Bool_t Use(const Char_t * ModuleName, const Char_t * Assignment); 		// assign parameters
		inline Bool_t Use(const Char_t * Assignment) { return(Use(NULL, Assignment)); };

		inline Int_t GetSerial() { return(fSerial); };  	 					// return unique subevent id

		Bool_t Set(const Char_t * ParamName, const Char_t * RegName, Int_t Value);	// set param register by name
		Bool_t Set(const Char_t * ParamName, Int_t RegIndex, Int_t Value);			// ... by index
		Int_t Get(const Char_t * ParamName, const Char_t * RegName);				// get param register by name
		Int_t Get(const Char_t * ParamName, Int_t RegIndex);						// ... by index

		inline Int_t GetNofEvents() { return(fNofEvents); };
		inline Int_t GetNofModules() { return(fNofModules); };

		inline Int_t GetNofParams() { return(fNofParams); };					// get number of params
		inline Int_t GetNofShorts() { return(fNofShorts); };					// get number of 16 bit words
		inline UInt_t GetLegalDataTypes() { return(fLegalDataTypes); }; // data types subevent is capable of

		inline Int_t GetCrate() { return(fCrate); };					// return crate number

		inline UInt_t GetType() { return(fSevtType); }; 				// subevent's type & subtype
		inline UInt_t GetSubtype() { return(fSevtSubtype); };

		virtual Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL) { return(kFALSE); };  // generate part of code

		Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);	// generate part of analyzing code
		Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, TMrbEvent * Event, TMrbTemplate & Template, const Char_t * Prefix = NULL);

		Bool_t MakeConfigCode(ofstream & CfgStrm, TMrbConfig::EMrbConfigTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL);

		virtual Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) { return(kFALSE); };

		Bool_t SetReadoutOptions(Option_t * Options = "");					// set options for MakeReadoutCode()
		Bool_t SetAnalyzeOptions(Option_t * Options = "byName:Histograms");	// set options for MakeAnalyzeCode()
		Bool_t SetConfigOptions(Option_t * Options = "");					// set options for MakeConfigCode()
		UInt_t GetReadoutOptions(TMrbEvent * Parent = NULL); 						// return MakeReadoutCode() options
		UInt_t GetAnalyzeOptions(TMrbEvent * Parent = NULL); 						// return MakeAnalyzeCode() options
		UInt_t GetConfigOptions(TMrbEvent * Parent = NULL);							// return MakeConfigCode() options

		inline const Char_t * GetPrefix() { return(fPrefix.Data()); };
		inline Bool_t PrefixToBePrepended() { return(fPrependPrefix); };	// prepend an individual prefix?
		void PrependPrefix(const Char_t * Prefix) {
			fPrefix = Prefix;
			fPrefix.ToLower();
			fPrependPrefix = !fPrefix.IsNull();
		};
		inline void PrependPrefix(Bool_t Flag = kTRUE) { fPrependPrefix = fPrefix.IsNull() ? kFALSE : Flag; };

		void Print(ostream & OutStrm, const Char_t * Prefix = "");	// show data
		inline virtual void Print() { Print(cout, ""); };

		inline void AllocateHistograms(Bool_t Flag = kTRUE) { fHistosToBeAllocated = Flag; }; // create histograms?
		inline Bool_t HistosToBeAllocated() { return(fHistosToBeAllocated); };
		inline void FillHistogramsIfTrueHitOnly(Bool_t Flag = kTRUE) { fHistosToBeFilledIfTrueHit = Flag; };
		inline Bool_t HistosToBeFilledIfTrueHit() { return(fHistosToBeFilledIfTrueHit); };

		virtual inline Bool_t HasFixedLengthFormat() { return(kTRUE); };		// fixed length data
		virtual inline Bool_t AllowsMultipleModules() { return(kTRUE); };		// format allows multiple modules
		virtual inline Bool_t SerialToBeCreated() { return(kTRUE); }			// create unique serial internally?
		virtual inline Bool_t IsRaw() { return(kFALSE); };						// not raw (user-defined) mode
		virtual inline Bool_t CheckModuleID(TMrbModule * Module) { return(kTRUE); }; // subevent may store any modules

																				// params to be addressed in array mode?
		inline void SetArrayMode(Bool_t ArrayMode = kTRUE) { fArrayMode = ArrayMode; };	// single array to be used?
		inline Bool_t IsInArrayMode() { return(fArrayMode); };
		inline Bool_t HasIndexedParams() { return(fHasIndexedParams); };
				
		virtual inline Bool_t NeedsHitBuffer() { return(kFALSE); }; 			// no hit buffer needed
		virtual inline Bool_t NeedsBranchMode() { return(kFALSE); }; 			// may run with leaves or branches

		virtual inline Bool_t HasPrivateCode() { return(kFALSE); }; 			// normal code generation
		virtual inline const Char_t * GetPrivateCodeFile() { return(NULL); };
		virtual inline const Char_t * GetCommonCodeFile() { return(NULL); };
		
		inline TObjArray * GetLofEvents() { return(&fLofEvents); };			// get address of ...
		inline TObjArray * GetLofModules() { return(&fLofModules); };
		const Char_t * GetLofModulesAsString(TString & LofModules);
		inline TObjArray * GetLofParams() { return(&fLofParams); };
		const Char_t * GetLofParamsAsString(TString & LofParams);

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbSubevent.html&"); };

	protected:
		UInt_t fSevtType;					// subevent type
		UInt_t fSevtSubtype;				// subevent subtype
		UInt_t fLegalDataTypes; 			// datatypes legal for this type of subevent

		Int_t fSerial;						// header words: unique subevent id
		Int_t fCrate;						// crate number

		Bool_t fHistosToBeAllocated;		// kTRUE if histograms to be allocated for all params
		Bool_t fHistosToBeFilledIfTrueHit;	// fill histograms if is it true hit

		Int_t fNofEvents;					// list of parent events
		TObjArray fLofEvents;

		Int_t fNofParams;					// number of params
		Int_t fNofShorts;					// number of 16 bit words;
		TObjArray fLofParams;				// list of params

		Int_t fNofModules;					// list of modules housing these params
		TObjArray fLofModules;

		UInt_t fReadoutOptions; 			// options used in MakeReadoutCode()
		UInt_t fAnalyzeOptions; 			// ... in MakeAnalyzeCode()
		UInt_t fConfigOptions;  			// ... in MakeConfigCode()

		Bool_t fHasIndexedParams;			// if there are TArrayX params
		Bool_t fArrayMode;					// kTRUE if subevent data are stored in an array
		
		Bool_t fPrependPrefix;				// kTRUE if subevents has its own prefix
		TString fPrefix;					// prefix to be prepended to params & histos

		TMrbModule * fSoftModule;			// internal module name, simulated by software
				
	ClassDef(TMrbSubevent, 1)		// [Config] Base class describing a MBS subevent
};	

#endif
