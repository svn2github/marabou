#ifndef __TNGMrbFocusList_h__
#define __TNGMrbFocusList_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbFocusList.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TNGMrbFocusList               -- focus handling for text entries
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbFocusList.h,v 1.1 2009-03-27 09:39:35 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TGTextEntry.h"
#include "TList.h"
#include "TSystem.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFocusList
// Purpose:        Maintain a list of focusable objects
// Description:    Handles focus changes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbFocusList : public TList {

	public:
		TNGMrbFocusList() {};				// default ctor
		~TNGMrbFocusList() {};				// default dtor

		void StartFocus(TGTextEntry * Entry = NULL);
		void FocusForward(TGTextEntry * Entry = NULL);
		void FocusBackward(TGTextEntry * Entry = NULL);

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbFocusList.html&"); };

	ClassDef(TNGMrbFocusList, 0) 	// [GraphUtils] Keep track of the input focus
};

#endif
