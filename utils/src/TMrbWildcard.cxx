//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbWildcard.cxx
// Purpose:        MARaBOU utilities:
//                 MARaBOU's wildcard utility
// Description:    Implements class methods for wildcarding
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbWildcard.cxx,v 1.1 2004-12-14 11:46:51 rudi Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TFile.h"
#include "SetColor.h"
#include "TMrbWildcard.h"

ClassImp(TMrbWildcard)

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

	const Char_t * wp = fWildcard.Data();
	const Char_t * sp = String;
	cout << "@@----- " << sp << " ------------------------" << endl;
	while (*wp && *sp) {
		cout << "@@ *wp=" << *wp << " *sp=" << *sp << endl;
		if (*wp == *sp) {
			sp++;
			wp++;
			cout << "@@ sp++, wp++" << endl;
		} else if (*wp == '?' && *sp != '/') {
			sp++;
			wp++;
			cout << "@@ sp++, wp++" << endl;
		} else if (*wp == '*') {
			wp++;
			cout << "@@ *wp=" << *wp << endl;
			while (*sp && (*sp != *wp) && (*sp != '/')) {
				cout << "@@ skipping *sp=" << *sp << endl;
				sp++;
			}
		} else if (*wp != *sp) {
			cout << "@@ no match" << endl;
			return(kFALSE);
		}
	}
	Bool_t match = (*wp == '\0') && (*sp == '\0');
	cout << (match ? "@@ match" : "@@ no match") << endl;
	return(match);
}
