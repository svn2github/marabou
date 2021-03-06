#ifndef __TMbsEvtBuilder_h__
#define __TMbsEvtBuilder_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/inc/TMbsEvtBuilder.h
// Purpose:        Define a MBS setup
// Class:          TMbsEvtBuilder       -- event builder
// Description:    Class definitions to generate a MBS setup.
// Author:         R. Lutter
// Revision:       $Id: TMbsEvtBuilder.h,v 1.5 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TSystem.h"
#include "TString.h"

#include "TMrbNamedX.h"

#include "TMbsSetupCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder
// Purpose:        Define event builder
// Methods:
// Description:    Creates an event builder.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMbsEvtBuilder : public TObject {

	public:
		enum			{	kNofBuffersPerStream	=	8	};
		enum			{	kNofStreams 			=	8	};

	public:

		TMbsEvtBuilder();				// ctor

		~TMbsEvtBuilder() {};			// default dtor

		Bool_t SetProcName(const Char_t * ProcName);		// proc name/addr
		const Char_t * GetProcName();
		const Char_t * GetProcAddr();

		Bool_t SetType(const Char_t * ProcType);		// processor type
		Bool_t SetType(EMbsProcType ProcType);
		TMrbNamedX * GetType() const;

		Bool_t SetCrate(Int_t Crate);			// crate number
		Int_t GetCrate() const;

											// buffers
		void SetBuffers(Int_t BufferSize,	Int_t NofBuffers = kNofBuffersPerStream, Int_t NofStreams = kNofStreams);
		Int_t GetBufferSize() const;
		Int_t GetNofBuffers() const;
		Int_t GetNofStreams() const;

		Bool_t SetFlushTime(Int_t Seconds);		// flushtime
		Int_t GetFlushTime() const;

		Bool_t SetVSBAddr(UInt_t Addr);			// vsb addr
		UInt_t GetVSBAddr() const;

		void RemoveSetup(); 			// remove entries from setup data base
		void Reset();	 				// reset to default

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TString fName;					// temp storage: name
		TString fAddr;					// ... addr

	ClassDef(TMbsEvtBuilder, 1) 	// [MBS Setup] Event builder
};

#endif
