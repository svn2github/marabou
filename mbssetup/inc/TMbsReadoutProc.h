#ifndef __TMbsReadoutProc_h__
#define __TMbsReadoutProc_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/inc/TMbsReadoutProc.h
// Purpose:        Define a MBS setup
// Class:          TMbsReadoutProc      -- readout proc
// Description:    Class definitions to generate a MBS setup.
// Author:         R. Lutter
// Revision:       $Id: TMbsReadoutProc.h,v 1.7 2011-03-08 10:31:22 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TSystem.h"
#include "TString.h"
#include "TArrayI.h"

#include "TMrbNamedX.h"

#include "TMbsSetupCommon.h"
#include "TMbsTriggerModule.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsReadoutProc
// Purpose:        Define readout process
// Methods:
// Description:    Creates a readout process.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMbsReadoutProc : public TObject {

	public:

		TMbsReadoutProc() {};				// default ctor

		TMbsReadoutProc(Int_t ProcNo);		// explicit ctor

		~TMbsReadoutProc() {};				// default dtor

		Bool_t SetProcName(const Char_t * ProcName);	// proc name/addr
		const Char_t * GetProcName();
		const Char_t * GetProcAddr();

		Bool_t SetType(const Char_t * ProcType);	// processor type
		Bool_t SetType(EMbsProcType ProcType);
		TMrbNamedX * GetType() const;

		Bool_t SetPath(const Char_t * Path, Bool_t Create = kFALSE);		// subdir where to put setup files
		const Char_t * GetPath();

		Bool_t SetCrate(Int_t Crate);		// crate number
		Int_t GetCrate() const;

		Bool_t SetCratesToBeRead(Int_t C1 = -1, Int_t C2 = -1, Int_t C3 = -1, Int_t C4 = -1, Int_t C5 = -1);	// crates connected
		Int_t GetCratesToBeRead(TArrayI & LofCrates) const;

		Bool_t SetController(const Char_t * ContrlType);		// crate controller
		Bool_t SetController(EMbsControllerType ContrlType);
		TMrbNamedX * GetController() const;

		Bool_t SetVSBAddr(UInt_t Addr);			// vsb addr
		UInt_t GetVSBAddr() const;
		Bool_t SetPipeBase(UInt_t Addr); 		// pipe base addr
		UInt_t GetPipeBase() const;
		Bool_t SetPipeType(Int_t Type); 		// pipe base type
		Int_t GetPipeType() const;
		Bool_t SetPipeSegLength(Int_t Length); 	// pipe seg length
		Int_t GetPipeSegLength() const;
		Bool_t SetPipeLength(Int_t NofSevts);	 	// pipe length (= max number of subevents in pipe)
		Int_t GetPipeLength() const;
		Bool_t SetSevtSize(Int_t Trigger, Int_t SevtSize); 	// subevent size
		Int_t GetSevtSize(Int_t Trigger = 0) const;

		inline Int_t GetID() const { return(fId); }; // readout id

		inline TMbsTriggerModule * TriggerModule() const { return(fTriggerModule); };	// trigger module

		Bool_t SetCodeName(const Char_t * Source); 					// define name code files
		const Char_t * GetCodeName();								// ... return code file name
		Bool_t CopyMakefile(const Char_t * SrcDir = "");			// copy makefile to readout dir
		Bool_t CompileReadout(const Char_t * Version = "deve"); 	// compile readout source

		void RemoveSetup(); 				// remove entries from setup data base
		void Reset();	 					// reset to default

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Int_t fId; 						// id number for this readout proc

 		TMbsTriggerModule * fTriggerModule;	//! trigger module

		TString fName;						// temp storage: name
		TString fAddr;						// ... addr
		TString fPath;						// ... path
		TString fCode;						// ... name of code file

	ClassDef(TMbsReadoutProc, 1)		// [MBS Setup] Readout processor
};

#endif
