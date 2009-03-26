#ifndef __TSnkDDAMessage_h__
#define __TSnkDDAMessage_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAMessage.h
// Purpose:        Class defs to describe a dac DDA0816
// Class:          TSnkDDAMessage: commumication with server
// Description:    Class definitions to handle a dac type DDA0816
// Author:         R. Lutter
// Revision:       $Id: TSnkDDAMessage.h,v 1.1 2009-03-26 11:18:28 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TString.h"

#include "TSnkDDACommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAMessage
// Purpose:        Class defs to decsribe a single dac channel
// Description:    Describes a single dac channel.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TSnkDDAMessage : public TObject {

	public:
		TSnkDDAMessage() {};						// default ctor
		~TSnkDDAMessage() {};						// default dtor

		void Reset();								// reset message
		const Char_t * ToString(TString & Msg); 	// convert message to string
		Bool_t FromString(TString & Msg); 			// convert string to message
		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & Out);					// print message
		inline void Print() { this->Print(cout); };
				
		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	public:
		TString fHost;
		Int_t fPort;
		Int_t fPid;
		TString fText;
		TString fProgram;
		TString fAction;
		TString fExecMode;
		Double_t fInterval;
		Int_t fCycles;
		TString fPath;
		TString fSaveScan;
						
	ClassDef(TSnkDDAMessage, 1) 	// [SNAKE] Client/server communication
};

#endif
