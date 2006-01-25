#ifndef __TMrbTail_h__
#define __TMrbTail_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbTail.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbTail    -- tail utility
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbTail.h,v 1.9 2006-01-25 12:21:27 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TTimer.h"

#include "TMrbLogger.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail
// Purpose:        Tail utility for MARaBOU
// Description:    Methods to implement a tail function in MARaBOU
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTail: public TMrbLogger {

	public:
		TMrbTail() {};												 	// default ctor
		TMrbTail(const Char_t * TailName, const Char_t * TailFile); 	// ctor
		virtual ~TMrbTail() {};						// dtor

		inline void SkipOrigPrefix(Bool_t Skip = kTRUE) { fSkipOrigPrefix = Skip; };
		inline void SetNewPrefix(const Char_t * Prefix = NULL) { fNewPrefix = (Prefix == NULL) ? "" : Prefix; };

		void Start(Bool_t SkipToEnd = kFALSE);		// start
		inline void Stop() { fTimer->TurnOff(); fStopIt = kTRUE; };

		virtual Bool_t HandleTimer(TTimer * Timer);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		TString fTailFile;
		Int_t fFilDes;  		// file descriptor (open(2))
		FILE * fStrm;			//! ... stream (fdopen(3))
		Bool_t fStopIt;			// kTRUE stops output
		TTimer * fTimer;		//!

		Bool_t fSkipOrigPrefix;		// kTRUE if line prefix (=date) is to be skipped on output
		TString fNewPrefix; 		// prefix to replace the original one

	ClassDef(TMrbTail, 1) 		// [Utils] Tail utility
};

#endif
