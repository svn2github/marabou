//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbProgressBar.cxx
// Purpose:        MARaBOU graphic utilities: a progress bar
// Description:    Implements class methods to show messages
//                 in a scrollable listbox.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbProgressBar.cxx,v 1.1 2009-03-27 09:39:35 Rudolf.Lutter Exp $       
// Date:           
//
//Begin_Html
/*
<img src=gutils/TNGMrbProgressBar.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TNGMrbProgressBar.h"

// lifo to store objects located on heap
#define HEAP(x)	fHeap.AddFirst((TObject *) x)

ClassImp(TNGMrbProgressBar)

TNGMrbProgressBar::TNGMrbProgressBar( const TGWindow * Parent,
										const TGWindow * Main,
										const Char_t * Title,
										UInt_t Width,
										const Char_t * BarColor,
										const Char_t * BarText,
										Bool_t WithDetail) : TGTransientFrame(Parent, Main, Width, 0) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProgressBar
// Purpose:        Display a progress bar
// Arguments:      TGWindow * Parent             -- parent window
//                 TGWindow * Main               -- main window
//                 Char_t * Title                -- title
//                 UInt_t Width                  -- window width
//                 Char_t * BarColor             -- fill color
//                 Char_t * BarText              -- text to be displayed
//                 Bool_t WithDetail             -- show detail
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Char_t * font;
	ULong_t black, white, gray, blue;

//	Initialize colors
	gClient->GetColorByName("black", black);
	gClient->GetColorByName("lightblue2", blue);
	gClient->GetColorByName("gray75", gray);
	gClient->GetColorByName("white", white);

//	Initialize graphic contexts
	TNGMrbGContext * frameGC = new TNGMrbGContext(font, black, gray);
	HEAP(frameGC);
	font = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TNGMrbGContext * buttonGC = new TNGMrbGContext(font, black, gray);
	HEAP(buttonGC);
	TNGMrbGContext * labelGC = new TNGMrbGContext(font, black, gray);
	HEAP(labelGC);


	if (BarText == NULL || *BarText == '\0') {
		fBar = new TGHProgressBar(this, TGProgressBar::kStandard, Width - 20);
		fBar->SetFillType(TGProgressBar::kBlockFill);
	} else {
		fBar = new TGHProgressBar(this, TGProgressBar::kFancy, Width - 20);
		fBar->ShowPosition(kTRUE, kFALSE, BarText);
	}
	fBar->SetBarColor(BarColor);
	HEAP(fBar);
	this->AddFrame(fBar);
	
	fDetail = NULL;
	if (WithDetail) {
		fDetail = new TGLabel(this, new TGString(""));
		HEAP(fBar);
		this->AddFrame(fDetail);
		fDetail->SetTextJustify(kTextCenterX);
	}

	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(Main->GetId(), Parent->GetId(),
								(((TGFrame *) Main)->GetWidth() / 2 - this->GetWidth() / 2),
								(((TGFrame *) Main)->GetHeight() / 2), ax, ay, wdum);
	this->Move(ax, ay);

	SetWindowName(Title);
	SetIconName(Title);
	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(Width, 0);
	MapWindow();
}
