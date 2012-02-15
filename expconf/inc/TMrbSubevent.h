#ifndef __TMrbSubevent_h__
#define __TMrbSubevent_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbSubevent.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbSubevent         -- base class for subevents
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSubevent.h,v 1.26 2011-12-15 16:33:23 Marabou Exp $
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

#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"
#include "TMrbEvent.h"
#include "TMrbModule.h"
#include "TMrbCamacModule.h"
#include "TMrbVMEModule.h"

class TMrbEvent;
class TMrbModule;
class TMrbModuleChannel;

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

		TMrbSubevent(const TMrbSubevent &) : TNamed() {};			// default copy ctor

		inline void AddEvent(TMrbEvent * Evt) {		 	// add a parent event
			fLofEvents.Add(Evt);
			fNofEvents++;
		};

		inline TMrbEvent * FindEvent(const Char_t * EvtName) const {	// find a parent
			return ((TMrbEvent *) fLofEvents.FindObject(EvtName));
		};

		Bool_t AddParam(TMrbModuleChannel * Param);			 	// add a new parameter

		inline TMrbModuleChannel * FindParam(const Char_t * ParamName) const {		// find param by its name
			return((TMrbModuleChannel *) fLofParams.FindObject(ParamName));
		};

		inline Bool_t AddModule(TMrbModule * Module) {		 	// add a new module
			fLofModules.Add(Module);
			fNofModules++;
			return(kTRUE);
		};

		inline TMrbModule * NextModule(TMrbModule * After = NULL) const {	// get next module from list
			return((After == NULL) ? (TMrbModule *) fLofModules.First() : (TMrbModule *) fLofModules.After((TObject *) After));
		};

		inline TMrbModule * FindModule(const Char_t * ModuleName) const {	// find a module
			return ((TMrbModule *) fLofModules.FindObject(ModuleName));
		};

		TMrbModule * FindModuleByID(TMrbConfig::EMrbModuleID ModuleID, TMrbModule * After = NULL) const; 		// find module by its id
		inline TMrbModule * FindModuleByID(TMrbNamedX * ModuleID, TMrbModule * After = NULL) const {
			return(this->FindModuleByID((TMrbConfig::EMrbModuleID) ModuleID->GetIndex(), After));
		};

		TMrbModule * FindModuleByType(UInt_t ModuleType, TMrbModule * After = NULL) const; 	// find module by its type
		inline TMrbModule * FindModuleByType(TMrbNamedX * ModuleType, TMrbModule * After = NULL) const {
			return(this->FindModuleByType(ModuleType->GetIndex(), After));
		};

		TMrbModule * FindModuleBySerial(Int_t ModuleSerial) const;				 		// find module by its serial number

		Bool_t Use(const Char_t * ModuleName, const Char_t * Assignment, Bool_t BookHistos = kTRUE); 		// assign parameters
		inline Bool_t Use(const Char_t * Assignment, Bool_t BookHistos = kTRUE) { return(Use(NULL, Assignment, BookHistos)); };

		inline Int_t GetSerial() const { return(fSerial); };  	 					// return unique subevent id

		Bool_t Set(const Char_t * ParamName, const Char_t * RegName, Int_t Value);	// set param register by name
		Bool_t Set(const Char_t * ParamName, Int_t RegIndex, Int_t Value);			// ... by index
		Int_t Get(const Char_t * ParamName, const Char_t * RegName) const;			// get param register by name
		Int_t Get(const Char_t * ParamName, Int_t RegIndex) const;					// ... by index

		inline Int_t GetNofEvents() const { return(fNofEvents); };
		inline Int_t GetNofModules() const { return(fNofModules); };

		inline Int_t GetNofParams() const { return(fNofParams); };					// get number of params
		inline Int_t GetNofShorts() const { return(fNofShorts); };					// get number of 16 bit words
		inline UInt_t GetLegalDataTypes() const { return(fLegalDataTypes); }; // data types subevent is capable of

		inline Int_t GetCrate() const { return(fCrate); };					// return crate number

		inline UInt_t GetType() const { return(fSevtType); }; 				// subevent's type & subtype
		inline UInt_t GetSubtype() const { return(fSevtSubtype); };
		inline const Char_t * GetDescr() { return(fSevtDescr.Data()); };

		virtual Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL) { return(kFALSE); };  // generate part of code

		Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);	// generate part of analyzing code
		Bool_t MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, TMrbEvent * Event, TMrbTemplate & Template, const Char_t * Prefix = NULL);

		virtual Bool_t MakeSpecialAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, TMrbTemplate & Template) { return(kFALSE); };  // generate part of code

		Bool_t MakeConfigCode(ofstream & CfgStrm, TMrbConfig::EMrbConfigTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL);

		virtual Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) { return(kFALSE); };

		Bool_t SetReadoutOptions(Option_t * Options = "");					// set options for MakeReadoutCode()
		Bool_t SetAnalyzeOptions(Option_t * Options = "byName:Histograms");	// set options for MakeAnalyzeCode()
		Bool_t SetConfigOptions(Option_t * Options = "");					// set options for MakeConfigCode()
		UInt_t GetReadoutOptions(TMrbEvent * Parent = NULL) const; 			// return MakeReadoutCode() options
		UInt_t GetAnalyzeOptions(TMrbEvent * Parent = NULL) const; 			// return MakeAnalyzeCode() options
		UInt_t GetConfigOptions(TMrbEvent * Parent = NULL) const;			// return MakeConfigCode() options

		Bool_t SetParamName(Int_t ParamNo, Char_t * ParamName);						// set param name explicitly
		inline void UseExplicitParamNames(Bool_t Flag = kTRUE) { fExplicitParamNames = Flag; };
		inline Bool_t ExplicitParamNamesToBeUsed() { return(fExplicitParamNames); };

		inline const Char_t * GetPrefix() const { return(fPrefix.Data()); };
		inline Bool_t PrefixToBePrepended() const { return(fPrependPrefix); };	// prepend an individual prefix?
		void PrependPrefix(const Char_t * Prefix) {
			fPrefix = Prefix;
			fPrefix.ToLower();
			fPrependPrefix = !fPrefix.IsNull();
		};
		inline void PrependPrefix(Bool_t Flag = kTRUE) { fPrependPrefix = fPrefix.IsNull() ? kFALSE : Flag; };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, const Char_t * Prefix = "") const;	// show data
		inline virtual void Print() const { Print(cout, ""); };

		inline void AllocateHistograms(Bool_t Flag = kTRUE) { fHistosToBeAllocated = Flag; }; // create histograms?
		inline Bool_t HistosToBeAllocated() const { return(fHistosToBeAllocated); };
		inline void FillHistogramsIfTrueHitOnly(Bool_t Flag = kTRUE) { fHistosToBeFilledIfTrueHit = Flag; };
		inline Bool_t HistosToBeFilledIfTrueHit() const { return(fHistosToBeFilledIfTrueHit); };

		Bool_t SetMbsBranch(Int_t MbsBranchNo, const Char_t * MbsBranchName = NULL);							// mbs branch
		inline Bool_t SetMbsBranch(TMrbNamedX * MbsBranch) { return(this->SetMbsBranch(MbsBranch->GetIndex(), MbsBranch->GetName())); };
		inline TMrbNamedX * GetMbsBranch() { return(&fMbsBranch); };
		inline Int_t GetMbsBranchNo() { return(fMbsBranch.GetIndex()); };

		virtual inline Bool_t HasFixedLengthFormat() const { return(kTRUE); };		// fixed length data
		virtual inline Bool_t AllowsMultipleModules() const { return(kTRUE); };		// format allows multiple modules
		virtual inline Bool_t SerialToBeCreated() const { return(kTRUE); }			// create unique serial internally?
		virtual inline Bool_t IsRaw() const { return(kFALSE); };						// not raw (user-defined) mode
		virtual inline Bool_t CheckModuleID(TMrbModule * Module) const { return(kTRUE); }; // subevent may store any modules

																				// params to be addressed in array mode?
		inline void SetArrayMode(Bool_t ArrayMode = kTRUE) { fArrayMode = ArrayMode; };	// single array to be used?
		inline Bool_t IsInArrayMode() const { return(fArrayMode); };
		inline Bool_t HasIndexedParams() const { return(fHasIndexedParams); };

		Bool_t UseXhit(const Char_t * HitName, Int_t DataLength = -1);			// define a special hit object to be used
		inline Bool_t UseXhit(Int_t DataLength) { return(this->UseXhit("Xhit", DataLength)); };
		inline const Char_t * GetNameOfXhit() { return(fXhit ? fXhit->GetName() : ""); };
		inline Int_t GetHitDataLength() { return(fXhit ? fXhit->GetIndex() : 0); };
		inline Bool_t HasXhit() { return(fXhit != NULL); };

		virtual inline Bool_t NeedsHitBuffer() const { return(kFALSE); }; 			// no hit buffer needed
		virtual inline Bool_t NeedsBranchMode() const { return(kFALSE); }; 			// may run with leaves or branches
		virtual inline Bool_t NeedsModulesToBeAssigned() const { return(kTRUE); };	// normal subevents need modules

		virtual inline Bool_t HasPrivateCode() const { return(kFALSE); }; 			// normal code generation
		virtual inline Bool_t SingleModuleOnly() const { return(kFALSE); }; 		// only module per subevent only
		virtual inline const Char_t * GetPrivateCodeFile() const { return(NULL); };
		virtual inline const Char_t * GetCommonCodeFile() const { return(NULL); };

		inline TObjArray * GetLofEvents() { return(&fLofEvents); };			// get address of ...
		inline TObjArray * GetLofModules() { return(&fLofModules); };
		const Char_t * GetLofModulesAsString(TString & LofModules) const;
		inline TObjArray * GetLofParams() { return(&fLofParams); };
		const Char_t * GetLofParamsAsString(TString & LofParams) const;

		inline void CreateHistoArray(const Char_t * ArrayName = NULL) {		// create array of histograms and write .histlist file
			fCreateHistoArray = kTRUE;
			fHistoArrayName = ArrayName ? ArrayName : this->GetName();
		}
		inline Bool_t HistoArrayToBeCreated() { return(fCreateHistoArray); };
		inline const Char_t * GetHistoArrayName() { return(fHistoArrayName.Data()); };

		void Browse(TBrowser * Browser);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		UInt_t fSevtType;					// subevent type
		UInt_t fSevtSubtype;				// subevent subtype
		UInt_t fLegalDataTypes; 			// datatypes legal for this type of subevent
		TString fSevtDescr; 				// description

		Int_t fSerial;						// header words: unique subevent id
		Int_t fCrate;						// crate number

		Bool_t fHistosToBeAllocated;		// kTRUE if histograms to be allocated for all params
		Bool_t fHistosToBeFilledIfTrueHit;	// fill histograms if is it true hit

		TMrbNamedX fMbsBranch;				// branch within MBS

		Bool_t fExplicitParamNames;			// kTRUE if explicit param names to be used

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

		TMrbNamedX * fXhit;				// a special (extended) hit

		TString fInheritsFrom;				// subevent parent class
		TString fCommonCodeFile;			// name of common code

		Bool_t fCreateHistoArray;			// kTRUE if histo array / .histlist file is to be created
		TString fHistoArrayName;			// name of histo array / .histlist file

		TMrbModule * fSoftModule;			// internal module name, simulated by software

	ClassDef(TMrbSubevent, 1)		// [Config] Base class describing a MBS subevent
};

#endif
