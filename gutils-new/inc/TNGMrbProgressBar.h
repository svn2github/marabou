#ifndef __TNGMrbProgressBar_h__
#define __TNGMrbProgressBar_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbProgressBar.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TNGMrbProgressBar       -- a transient frame
//                                             showing a progress bar
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbProgressBar.h,v 1.2 2009-03-31 06:12:06 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGProgressBar.h"

#include "TGTextView.h"
#include "TMrbLogger.h"
#include "TMrbLofNamedX.h"
#include "TNGMrbProfile.h"
#include "TNGMrbTextButton.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProgressBar
// Purpose:        Display messages in a scrollable listbox
// Description:    Extracts messages from MARaBOU's message loggin system
//                 and shows them in a scrollable listbox widget
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbProgressBar: public TGTransientFrame {

	public:
		enum {	kBarWidth = 400, kBarHeight = 50	};

	public:
		TNGMrbProgressBar(const TGWindow * Parent, const TGWindow * Main,
												TNGMrbProfile * Profile,
												const Char_t * Title,
												UInt_t Width = kBarWidth,
												const Char_t * BarColor = "blue",
												const Char_t * BarText = NULL,
												Bool_t WithDetail = kFALSE);
												
		~TNGMrbProgressBar() {};				// default dtor

		inline void SetRange(Float_t Min, Float_t Max) { fBar->SetRange(Min, Max); }; 
		inline void Increment(Float_t Incr, const Char_t * Detail = NULL) {
			fBar->Increment(Incr);
			if (Detail && fDetail)  fDetail->SetText(Detail);
		}; 
		inline void SetBarColor(const Char_t * Color) { fBar->SetBarColor(Color); }; 

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbProgressBar.html&"); };

	protected:
		TList fHeap;
	
		TGHProgressBar * fBar;			//! progress bar
		TGLabel * fDetail;				//! label to show details
		TNGMrbTextButtonList * fAction;	//! action buttons
								
		TMrbLofNamedX fLofActions;
		
	ClassDef(TNGMrbProgressBar, 1)		// [GraphUtils] MARaBOU's message viewer
};

#endif
