#ifndef __TMrbEvent_h__
#define __TMrbEvent_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/inc/TMrbEvent.h
// Purpose:        Define experimental configuration for MARaBOU
// Class:          TMrbEvent            -- event connected to a trigger
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
#include "TNamed.h"
#include "TList.h"

#include "TMrbLofNamedX.h"
#include "TMrbTemplate.h"

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

		TMrbEvent(const TMrbEvent &) {};					// default copy ctor

		Bool_t HasSubevent(const Char_t * Assignment);		// add subevent(s)
		Bool_t ShareSubevents(TMrbEvent * Event);			// share subevent(s) with another event

		inline TObject * FindSubevent(const Char_t * SevtName) {					// find a subevent
			return (fLofSubevents.FindObject(SevtName));
		};
		TObject * FindSubeventByCrate(Int_t Crate, TObject * After = NULL);	// find a subevent with specified crate

		inline Int_t GetTrigger() { return(fTrigger); };							// return trigger number
		inline TMrbConfig::EMrbTriggerStatus GetTriggerStatus() { return(fTriggerStatus); };	// return trigger status


		Bool_t MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL); // generate readout code

		Bool_t MakeAnalyzeCode(ofstream & RdoStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension);
		Bool_t MakeAnalyzeCode(ofstream & RdoStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL);

		Bool_t MakeConfigCode(ofstream & CfgStrm, TMrbConfig::EMrbConfigTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix = NULL);

		virtual Bool_t MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) { return(kFALSE); };

		Bool_t SetReadoutOptions(Option_t * Options = "");						// set options for MakeReadoutCode()
		Bool_t SetAnalyzeOptions(Option_t * Options = "Subevents:Histograms");	// set options for MakeAnalyzeCode()
		Bool_t SetConfigOptions(Option_t * Options = "");						// set options for MakeConfigCode()

		UInt_t GetReadoutOptions(); 												// return MakeReadoutCode() options
		UInt_t GetAnalyzeOptions(); 												// return MakeAnalyzeCode() options
		UInt_t GetConfigOptions();													// return MakeConfigCode() options

		inline const Char_t * GetPrefix() { return(fPrefix.Data()); };
		void AllocPrivateHistograms(const Char_t * Prefix) {
			fPrefix = Prefix;
			fPrefix.ToLower();
			fPrivateHistograms = !fPrefix.IsNull();
		};
		inline void AllocPrivateHistograms(Bool_t Flag) { fPrivateHistograms = fPrefix.IsNull() ? kFALSE : Flag; };
		inline Bool_t HasPrivateHistograms() { return(fPrivateHistograms); };

		inline void SetSizeOfHitBuffer(Int_t NofEntries, Int_t HighWater = 0) {
			fSizeOfHitBuffer = NofEntries;
			fHBHighWaterLimit = HighWater;
		}
		inline Int_t GetSizeOfHitBuffer() { return(fSizeOfHitBuffer); };
		inline Int_t GetHBHighWaterLimit() { return(fHBHighWaterLimit); };;

		virtual inline Bool_t HasPrivateCode() { return(kFALSE); }; 				// normal code generation
		virtual inline const Char_t * GetPrivateCodeFile() { return(NULL); };
		virtual inline const Char_t * GetCommonCodeFile() { return(NULL); };
		
		inline Int_t GetNofSubevents() { return(fNofSubevents); };					// number of subevents
		inline TMrbLofNamedX * GetLofSubevents() { return(&fLofSubevents); };		// list of subevents
		const Char_t * GetLofSubeventsAsString(TString & LofSubevents);

		inline Int_t GetType() { return(fEventType); };
		inline Int_t GetSubtype() { return(fEventSubtype); };

		inline void SetAutoSave(Int_t AutoSave = TMrbConfig::kAutoSave) { fAutoSave = AutoSave; }; 	// auto save mechanism
		inline Int_t GetAutoSave() { return(fAutoSave); };
			
		void Print(ostream & OutStrm, const Char_t * Prefix = "");	// show data
		inline virtual void Print() { Print(cout, ""); };

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbEvent.html&"); };

	protected:
		UInt_t fEventType;
		UInt_t fEventSubtype;

		Int_t fTrigger;									// associated trigger number
		TMrbConfig::EMrbTriggerStatus fTriggerStatus;	// trigger status: assigned, pattern

		Int_t fNofSubevents;				// list of subevents
		TMrbLofNamedX fLofSubevents;

		UInt_t fReadoutOptions; 			// options used in MakeReadoutCode()
		UInt_t fAnalyzeOptions; 			// ... in MakeAnalyzeCode()
		UInt_t fConfigOptions;  			// ... in MakeConfigCode()

		Int_t fAutoSave;					// auto save after N bytes

		Bool_t fPrivateHistograms;			// kTRUE if private histograms to be accumualted
		TString fPrefix;					// prefix to be prepended to private histos

		Int_t fSizeOfHitBuffer;
		Int_t fHBHighWaterLimit;

	ClassDef(TMrbEvent, 1)		// [Config] Base class describing a MBS event
};

#endif
