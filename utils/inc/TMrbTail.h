#ifndef __TMrbTail_h__
#define __TMrbTail_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbTail.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbTail    -- tail utility
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbTail.h,v 1.10 2008-08-18 08:18:57 Rudolf.Lutter Exp $       
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

#include "TMrbNamedX.h"
#include "TMrbLogger.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTail
// Purpose:        Tail utility for MARaBOU
// Description:    Methods to implement a tail function in MARaBOU
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbTail: public TMrbNamedX {

	public:
		enum EMrbTailOut		{	kMrbTailOutUndef = 0,
									kMrbTailOutLogger,
									kMrbTailOutStdio,
									kMrbTailOutArray
								};

	public:
		TMrbTail() {};												 	// default ctor
		TMrbTail(const Char_t * TailName, const Char_t * TailFile); 	// explicit ctors
		TMrbTail(const Char_t * TailName, FILE * TailStrm);

		virtual ~TMrbTail() {};											// dtor

		Bool_t SetOutput(TMrbLogger * Logger);							// define output: logger
		Bool_t SetOutput(ostream & Stdout, ostream & StdErr);			// ... stdio
		Bool_t SetOutput(TObject * Receiver, TObjArray * TextArray);	// ... array of TObjStrings

		void Start(Bool_t SkipToEnd = kFALSE);		// start
		inline void Stop() { fTimer->TurnOff(); fStopIt = kTRUE; };

		virtual Bool_t HandleTimer(TTimer * Timer);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		void ToLogger(TString & Text);
		void ToStdio(TString & Text);
		void ToArray(TString & Text);

	protected:
		TString fTailFile;
		Int_t fFilDes;  			// file descriptor (open(2))
		FILE * fStrm;				//! ... stream (fdopen(3))
		Bool_t fStopIt;				// kTRUE stops output
		TTimer * fTimer;			//!

		EMrbTailOut fOutputMode;	// output mode: logger, stdio, array
		TMrbLogger * fLogger;
		ostream * fStdout;
		ostream * fStderr;
		TObject * fReceiver;
		TObjArray * fTextArray;

	ClassDef(TMrbTail, 1) 		// [Utils] Tail utility
};

#endif
