#ifndef __TMrbWildcard_h__
#define __TMrbWildcard_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbWildcard.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbWildcard    -- wildcard utility
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbWildcard.h,v 1.2 2004-12-15 08:09:45 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TSystem.h"
#include "TString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWildcard
// Purpose:        A wildcard function in MARaBOU
// Description:    Methods to implement wildcard in MARaBOU
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbWildcard {

	public:
		TMrbWildcard(const Char_t * Wildcard) { this->SetMask(Wildcard); };	// ctor
		virtual ~TMrbWildcard() {};						// dtor

		void SetMask(const Char_t * Wildcard);			// define wildcard mask

		Bool_t Match(const Char_t * String) const;
		inline Bool_t Match(TString & String) const { return(this->Match(String.Data())); };
		inline Bool_t IsWildcarded() const { return(fIsWildcarded); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbWildcard.html&"); };

	protected:

		Bool_t fIsWildcarded; 		// kTRUE if mask contains * and/or ?
		TString fWildcardMask;		// wildcard mask

	ClassDef(TMrbWildcard, 0) 		// [Utils] Wildcard utility
};

#endif
