#ifndef __TGMrbFocusList_h__
#define __TGMrbFocusList_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbFocusList.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbFocusList               -- focus handling for text entries
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbFocusList.h,v 1.3 2004-09-28 13:47:33 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TGTextEntry.h"
#include "TList.h"
#include "TSystem.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFocusList
// Purpose:        Maintain a list of focusable objects
// Description:    Handles focus changes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbFocusList : public TList {

	public:
		TGMrbFocusList() {};				// default ctor
		~TGMrbFocusList() {};				// default dtor

		void StartFocus(TGTextEntry * Entry = NULL);
		void FocusForward(TGTextEntry * Entry = NULL);
		void FocusBackward(TGTextEntry * Entry = NULL);

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbFocusList.html&"); };

	ClassDef(TGMrbFocusList, 0) 	// [GraphUtils] Keep track of the input focus
};

#endif
