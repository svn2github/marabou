//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbTextButton.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of text buttons
// Description:    Implements class methods to handle a frame of
//                 text buttons.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbTextButton.cxx,v 1.5 2009-05-29 07:09:18 Marabou Exp $       
// Date:           
//
//Begin_Html
/*
<img src=gutils/TNGMrbTextButtonGroup.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TNGMrbTextButton.h"

ClassImp(TNGMrbTextButtonList)
ClassImp(TNGMrbTextButtonGroup)

TNGMrbTextButtonList::TNGMrbTextButtonList(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons,
												Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t Width, Int_t Height, Int_t ButtonWidth) :
										TGCompositeFrame(Parent, Width, Height, (Int_t) Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbTextButton | TNGMrbGContext::kGMrbList,
												Buttons,FrameId, Profile, NofRows, NofCols, Width, Height, ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbTextButtonList
// Purpose:        Define a list of text buttons arranged in a composite frame
// Arguments:      TGWindow * Parent             -- parent window
//                 Char_t * Label                -- label text (NULL means "no label")
//                 TMrbLofNamedX * Buttons       -- button names and indices
//                 TNGMrbProfile * Profile        -- graphics profile
//                 Int_t NofRows                 -- number of rows
//                 Int_t NofCols                 -- number of columns
//                 Int_t Width                   -- frame width
//                 Int_t Height                  -- frame height
//                 Int_t ButtonWidth             -- button width
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	if (Label != NULL && *Label != '\0') {
		fLabelText = Label;
		fLabel = new TGLabel(Parent, new TGString(Label));
		fLabel->SetTextFont(fLabelGC->Font());
		fLabel->SetForegroundColor(fLabelGC->FG());
		fLabel->SetBackgroundColor(fLabelGC->BG());
		fLabel->ChangeOptions(fLabelGC->GetOptions());
		TO_HEAP(fLabel);
		this->AddFrame(fLabel);
		fLabel->SetTextJustify(kTextLeft);
	}
	this->CreateButtons();
}

TNGMrbTextButtonList::TNGMrbTextButtonList(const TGWindow * Parent,
												const Char_t * Label,
												const Char_t * Buttons,
												Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t Width, Int_t Height, Int_t ButtonWidth) :
										TGCompositeFrame(Parent, Width, Height, (Int_t) Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbTextButton | TNGMrbGContext::kGMrbList,
												Buttons,FrameId, Profile, NofRows, NofCols, Width, Height, ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbTextButtonList
// Purpose:        Define a list of text buttons arranged in a composite frame
// Arguments:      TGWindow * Parent             -- parent window
//                 Char_t * Label                -- label text (NULL means "no label")
//                 Char_t * Buttons              -- button names separated by ":"
//                 TNGMrbProfile * Profile        -- graphics profile
//                 Int_t NofRows                 -- number of rows
//                 Int_t NofCols                 -- number of columns
//                 Int_t Width                   -- frame width
//                 Int_t Height                  -- frame height
//                 Int_t ButtonWidth             -- button width
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	if (Label != NULL && *Label != '\0') {
		fLabelText = Label;
		fLabel = new TGLabel(Parent, new TGString(Label));
		fLabel->SetTextFont(fLabelGC->Font());
		fLabel->SetForegroundColor(fLabelGC->FG());
		fLabel->SetBackgroundColor(fLabelGC->BG());
		fLabel->ChangeOptions(fLabelGC->GetOptions());
		TO_HEAP(fLabel);
		this->AddFrame(fLabel);
		fLabel->SetTextJustify(kTextLeft);
	}
	this->CreateButtons();
}

TNGMrbTextButtonGroup::TNGMrbTextButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons,
												Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t ButtonWidth) :
								TGGroupFrame(Parent, Label, (Int_t) Profile->GetFrameOptions()),
								TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbTextButton | TNGMrbGContext::kGMrbGroup,
												Buttons,FrameId, Profile, NofRows, NofCols, 0, 0, ButtonWidth) {
///__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbTextButtonGroup
// Purpose:        Define a group of text buttons and their behaviour
// Arguments:      TGWindow * Parent             -- parent window
//                 Char_t * Label                -- label text
//                 TMrbLofNamedX * Buttons       -- button names and indices
//                 TNGMrbProfile * Profile        -- graphics profile
//                 Int_t NofRows                 -- number of rows
//                 Int_t NofCols                 -- number of columns
//                 Int_t ButtonWidth             -- button width
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	if (Label != NULL && *Label != '\0') fLabelText = Label;
	this->CreateButtons();
}

TNGMrbTextButtonGroup::TNGMrbTextButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												const Char_t * Buttons,
												Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t ButtonWidth) :
								TGGroupFrame(Parent, Label, (Int_t) Profile->GetFrameOptions()),
								TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbTextButton | TNGMrbGContext::kGMrbGroup,
												Buttons,FrameId, Profile, NofRows, NofCols, 0, 0, ButtonWidth) {
///__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbTextButtonGroup
// Purpose:        Define a group of text buttons and their behaviour
// Arguments:      TGWindow * Parent             -- parent window
//                 Char_t * Label                -- label text
//                 Char_t * Buttons              -- button names separated by ":"
//                 TNGMrbProfile * Profile        -- graphics profile
//                 Int_t NofRows                 -- number of rows
//                 Int_t NofCols                 -- number of columns
//                 Int_t ButtonWidth             -- button width
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	if (Label != NULL && *Label != '\0') fLabelText = Label;
	this->CreateButtons();
}
