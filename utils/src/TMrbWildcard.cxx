//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbWildcard.cxx
// Purpose:        MARaBOU utilities:
//                 MARaBOU's wildcard utility
// Description:    Implements class methods for wildcarding
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbWildcard.cxx,v 1.3 2004-12-15 08:09:45 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TFile.h"
#include "SetColor.h"
#include "TMrbWildcard.h"

ClassImp(TMrbWildcard)

void TMrbWildcard::SetMask(const Char_t * Wildcard) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWildcard::SetMask
// Purpose:        Set wildcrad mask
// Arguments:      Char_t * Wildcard   -- masked to be used in match calls
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines wildcard mask.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fWildcardMask = Wildcard;
	fIsWildcarded = fWildcardMask.Index("*", 0) >= 0 || fWildcardMask.Index("?", 0) >= 0;
}

Bool_t TMrbWildcard::Match(const Char_t * String) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbWildcard::Match
// Purpose:        Test if strings match
// Arguments:      Char_t * String   -- string to be analyzed
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Compares string to wildcard mask.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsWildcarded()) return(fWildcardMask.CompareTo(String) == 0);

	const Char_t * wp = fWildcardMask.Data();
	const Char_t * sp = String;
	while (*wp && *sp) {
		if (*wp == '\\') {
			wp++;
			if (*wp == '\0')  return(kFALSE);
		}
		if (*wp == *sp) {
			sp++;
			wp++;
		} else if (*wp == '?' && *sp != '/') {
			sp++;
			wp++;
		} else if (*wp == '*') {
			wp++;
			if (*wp == '\0' || *wp == '?') return(kFALSE);
			if (*wp == '\\') {
				wp++;
				if (*wp == '\0')  return(kFALSE);
			}
			while (*sp && (*sp != *wp) && (*sp != '/')) sp++;
		} else if (*wp != *sp) {
			return(kFALSE);
		}
	}
	Bool_t match = (*wp == '\0') && (*sp == '\0');
	return(match);
}
