//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbFocusList.cxx
// Purpose:        MARaBOU utilities: Handle focus for GUI objects
// Description:    Implements focus handling for a list of text entries
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbFocusList.cxx,v 1.1 2009-03-27 09:39:35 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include "TNGMrbFocusList.h"

ClassImp(TNGMrbFocusList)

void TNGMrbFocusList::StartFocus(TGTextEntry * Entry) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFocusList::StartFocus
// Purpose:        Set focus to a given entry in list
// Arguments:      TGTextEntry * Entry   -- current entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a focus.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGTextEntry * entry;

	entry = NULL;
	if (Entry) entry = (TGTextEntry *) this->FindObject(Entry);
	if (entry == NULL) entry = (TGTextEntry *) this->First();
	if (entry) entry->SetFocus();
}

void TNGMrbFocusList::FocusForward(TGTextEntry * Entry) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFocusList::FocusForward
// Purpose:        Set focus to next entry in list
// Arguments:      TGTextEntry * Entry   -- current entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a focus.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGTextEntry * entry;

	entry = NULL;
	if (Entry) entry = (TGTextEntry *) this->After(Entry);
	if (entry == NULL) entry = (TGTextEntry *) this->First();
	if (entry) entry->SetFocus();
}

void TNGMrbFocusList::FocusBackward(TGTextEntry * Entry) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFocusList::FocusBackward
// Purpose:        Set focus to previous entry in list
// Arguments:      TGTextEntry * Entry   -- current entry
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets a focus.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGTextEntry * entry;

	entry = NULL;
	if (Entry) entry = (TGTextEntry *) this->Before(Entry);
	if (entry == NULL) entry = (TGTextEntry *) this->Last();
	if (entry) entry->SetFocus();
}
