#ifndef __TMrbEvent_h__
#define __TMrbEvent_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbEvent.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbEvent            -- event connected to a trigger
// Description:    Class definitions to implement a configuration front-end for MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbEvent.h,v 1.13 2008-01-14 09:48:51 Rudolf.Lutter Exp $       
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
#include "TList.h"

#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"

class TMrbSubevent;

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbEvent
// Purpose:        Define an event
// Description:    Defines an event connected to a trigger
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbEvent : public TNamed {

	public:		
		enum					{	kNofHitBufferEntries		= 1000	};

	public:
		TMrbEvent() {};										// default ctor

		TMrbEvent(Int_t TrigNumber, const Char_t * EvtName = "", const Char_t * EvtTitle = "");	// explicit ctor

		~TMrbEvent() {	 									// dtor
			fLofSubevents.Delete();
 		};

		TMrbEvent(const TMrbEvent &) : TNamed() {};					// default copy ctor

		Bool_t HasSubevent(const Char_t * Assignment);		// add subevent(s)
		Bool_t ShareSubevents(TMrbEvent * Event);			// share subevent(s) with another event

		inline TMrbSubevent * FindSubevent(const Char_t * SevtName) const {					// find a subevent
			return ((TMrbSubevent *) fLofSubevents.FindObject(SevtName));
		};
		TMrbSubevent * FindSubeventByCrate(Int_t Crate, TMrbSubevent * After = NULL) const;	// find a subevent with specified crate

		inline Int_t GetTrigger() const { return(fTrigger); };							// return trigger number
		inline TMrbConfig::EMrbTriggerStatus GetTriggerStatus() const { return(fTriggerStatus); };	// return trigger status

		inline void SetBaseClass(const Char_t * BaseClass) { fBaseClass = BaseClass; };			// base class
		inline const Char_t * GetBaseClass() { return(fBaseClass.Data()); };
		inline void SetPointerName(const Char_t * PointerName) { fPointerName = PointerName; };	// name of ptr to obj
		inline const Char_t * GetPointerName() { return(fPointerName.Data()); };

		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL); // generate readout code

		Bool_t MakeAnalyzeCode(ofstream & RdoStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);
		Bool_t MakeAnalyzeCode(ofstream & RdoStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL);

		Bool_t MakeConfigCode(ofstream & CfgStrm, TMrbConfig::EMrbConfigTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL);

		virtual Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) { return(kFALSE); };

		Bool_t SetReadoutOptions(Option_t * Options = "");						// set options for MakeReadoutCode()
		Bool_t SetAnalyzeOptions(Option_t * Options = "Subevents:Histograms");	// set options for MakeAnalyzeCode()
		Bool_t SetConfigOptions(Option_t * Options = "");						// set options for MakeConfigCode()

		UInt_t GetReadoutOptions() const; 												// return MakeReadoutCode() options
		UInt_t GetAnalyzeOptions() const; 												// return MakeAnalyzeCode() options
		UInt_t GetConfigOptions() const;													// return MakeConfigCode() options

		inline const Char_t * GetPrefix() const { return(fPrefix.Data()); };
		void AllocPrivateHistograms(const Char_t * Prefix) {
			fPrefix = Prefix;
			fPrefix.ToLower();
			fPrivateHistograms = !fPrefix.IsNull();
		};
		inline void AllocPrivateHistograms(Bool_t Flag) { fPrivateHistograms = fPrefix.IsNull() ? kFALSE : Flag; };
		inline Bool_t HasPrivateHistograms() const { return(fPrivateHistograms); };

		inline void SetSizeOfHitBuffer(Int_t NofEntries, Int_t HighWater = 0) {
			fSizeOfHitBuffer = NofEntries;
			fHBHighWaterLimit = HighWater;
		}
		inline Int_t GetSizeOfHitBuffer() const { return(fSizeOfHitBuffer); };
		inline Int_t GetHBHighWaterLimit() const { return(fHBHighWaterLimit); };;

		virtual inline Bool_t HasPrivateCode() const { return(kFALSE); }; 				// normal code generation
		virtual inline const Char_t * GetPrivateCodeFile() const { return(NULL); };
		virtual inline const Char_t * GetCommonCodeFile() const { return(NULL); };
		
		Bool_t SetMbsBranch(Int_t MbsBranchNo, const Char_t * MbsBranchName = NULL);	// mbs branch
		inline Bool_t SetMbsBranch(TMrbNamedX * MbsBranch) { return(this->SetMbsBranch(MbsBranch->GetIndex(), MbsBranch->GetName())); };
		inline TMrbNamedX * GetMbsBranch() { return(&fMbsBranch); };
		inline Int_t GetMbsBranchNo() { return(fMbsBranch.GetIndex()); };
		Bool_t SelectMbsBranch(Int_t MbsBranchNo);
		inline Int_t GetSelectedBranchNo() { return(fSelectedBranchNo); };

		inline Int_t GetNofSubevents() const { return(fNofSubevents); };					// number of subevents
		inline TMrbLofNamedX * GetLofSubevents() { return(&fLofSubevents); };		// list of subevents
		const Char_t * GetLofSubeventsAsString(TString & LofSubevents) const;

		inline Int_t GetType() const { return(fEventType); };
		inline Int_t GetSubtype() const { return(fEventSubtype); };

		inline void SetAutoSave(Int_t AutoSave = TMrbConfig::kAutoSave) { fAutoSave = AutoSave; }; 	// auto save mechanism
		inline Int_t GetAutoSave() const { return(fAutoSave); };
			
		inline Bool_t IsStartEvent() { return(fTrigger == TMrbConfig::kTriggerStartAcq); };
		inline Bool_t IsStopEvent() { return(fTrigger == TMrbConfig::kTriggerStopAcq); };
		inline Bool_t IsReservedEvent() { return(this->IsStartEvent() || this->IsStopEvent()); };	// start or stop event?

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & OutStrm, const Char_t * Prefix = "") const;	// show data
		inline virtual void Print() const { Print(cout, ""); };

		inline void CreateHistoArray(const Char_t * ArrayName = NULL) {		// create array of histograms and write .histlist file
			fCreateHistoArray = kTRUE;
			fHistoArrayName = ArrayName ? ArrayName : this->GetName();
		}
		inline Bool_t HistoArrayToBeCreated() { return(fCreateHistoArray); };
		inline const Char_t * GetHistoArrayName() { return(fHistoArrayName.Data()); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		UInt_t fEventType;
		UInt_t fEventSubtype;

		Int_t fTrigger;									// associated trigger number
		TMrbConfig::EMrbTriggerStatus fTriggerStatus;	// trigger status: assigned, pattern

		Int_t fNofSubevents;				// list of subevents
		TMrbLofNamedX fLofSubevents;

		TString fBaseClass; 				// name of base class
		TString fPointerName;				// name of ptr to event

		UInt_t fReadoutOptions; 			// options used in MakeReadoutCode()
		UInt_t fAnalyzeOptions; 			// ... in MakeAnalyzeCode()
		UInt_t fConfigOptions;  			// ... in MakeConfigCode()

		Int_t fAutoSave;					// auto save after N bytes

		TMrbNamedX fMbsBranch;				// branch within MBS
		Int_t fSelectedBranchNo;			// selected branch

		Bool_t fPrivateHistograms;			// kTRUE if private histograms to be accumualted
		TString fPrefix;					// prefix to be prepended to private histos

		Int_t fSizeOfHitBuffer;
		Int_t fHBHighWaterLimit;

		Bool_t fCreateHistoArray;			// kTRUE if histo array / .histlist file is to be created
		TString fHistoArrayName;			// name of histo array / .histlist file

	ClassDef(TMrbEvent, 1)		// [Config] Base class describing a MBS event
};

#endif
