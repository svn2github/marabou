#ifndef __TSnkDDAMessage_h__
#define __TSnkDDAMessage_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TSnkDDAMessage.h
// Purpose:        Class defs to describe a dac DDA0816
// Class:          TSnkDDAMessage: commumication with server
// Description:    Class definitions to handle a dac type DDA0816
// Author:         R. Lutter
// Revision:       
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
		const Char_t * ToString(TMrbString & Msg);	// convert message to string
		Bool_t FromString(TMrbString & Msg); 			// convert string to message
		void Print(ostream & Out);					// print message
		inline void Print() { this->Print(cout); };
				
		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TSnkDDAMessage.html&"); };

	public:
		TString fHost;
		Int_t fPort;
		Int_t fPid;
		TMrbString fText;
		TString fProgram;
		TString fAction;
		TString fExecMode;
		Double_t fInterval;
		TString fPath;
		TString fSaveScan;
						
	ClassDef(TSnkDDAMessage, 1) 	// [SNAKE] Client/server communication
};

#endif
