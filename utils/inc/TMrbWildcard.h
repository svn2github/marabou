#ifndef __TMrbWildcard_h__
#define __TMrbWildcard_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbWildcard.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbWildcard    -- wildcard utility
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbWildcard.h,v 1.1 2004-12-14 11:46:51 rudi Exp $       
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

class TMrbWildcard : public TObject {

	public:
		TMrbWildcard(const Char_t * Wildcard) { fWildcard = Wildcard; }; 	// ctor
		virtual ~TMrbWildcard() {};						// dtor

		Bool_t Match(const Char_t * String) const;
		inline Bool_t Match(TString & String) const { return(this->Match(String.Data())); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMrbWildcard.html&"); };

	protected:

		TString fWildcard;			// wildcard string

	ClassDef(TMrbWildcard, 1) 		// [Utils] Wildcard utility
};

#endif
