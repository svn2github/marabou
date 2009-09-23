//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbCheckButton.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of check buttons
// Description:    Implements class methods to handle a frame of
//                 check buttons.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbCheckButton.cxx,v 1.6 2009-09-23 10:42:51 Marabou Exp $       
// Date:           
// Layout:         A list or group of checkbuttons
//Begin_Html
/*
<img src=gutils/TNGMrbCheckButtonGroup.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TNGMrbCheckButton.h"

ClassImp(TNGMrbCheckButtonList)
ClassImp(TNGMrbCheckButtonGroup)

TNGMrbCheckButtonList::TNGMrbCheckButtonList(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons, Int_t FrameId, 
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t Width, Int_t Height, Int_t ButtonWidth,
												TObjArray * LofSpecialButtons, Bool_t SeparatedActionButtons) :
										TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbCheckButton | TNGMrbGContext::kGMrbList,
												Buttons,FrameId, Profile, NofRows, NofCols, Width, Height, ButtonWidth, SeparatedActionButtons) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbCheckButtonList
// Purpose:        Define a list of check buttons arranged in a composite frame
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text ("" means "no label")
//                 TMrbLofNamedX * Buttons        -- button names and indices
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t NofRows                  -- number of rows
//                 Int_t NofCols                  -- number of columns
//                 Int_t Width                    -- frame width
//                 Int_t Height                   -- frame height
//                 Int_t ButtonWidth              -- button width
//                 TObjArray * LofSpecialButtons  -- special buttons
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	fLofSpecialButtons = LofSpecialButtons;
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
	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	this->CreateButtons();
}

TNGMrbCheckButtonList::TNGMrbCheckButtonList(const TGWindow * Parent,
												const Char_t * Label,
												const Char_t * Buttons, Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t Width, Int_t Height, Int_t ButtonWidth,
												TObjArray * LofSpecialButtons, Bool_t SeparatedActionButtons) :
										TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbCheckButton | TNGMrbGContext::kGMrbList,
												Buttons,FrameId, Profile, NofRows, NofCols, Width, Height, ButtonWidth,SeparatedActionButtons) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbCheckButtonList
// Purpose:        Define a list of check buttons arranged in a composite frame
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text ("" means "no label")
//                 Char_t * Buttons               -- button names separated by ":"
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t NofRows                  -- number of rows
//                 Int_t NofCols                  -- number of columns
//                 Int_t Width                    -- frame width
//                 Int_t Height                   -- frame height
//                 Int_t ButtonWidth              -- button width
//                 TObjArray * LofSpecialButtons  -- special buttons
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	fLofSpecialButtons = LofSpecialButtons;
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
	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	this->CreateButtons();
}

TNGMrbCheckButtonGroup::TNGMrbCheckButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons, Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t ButtonWidth,
												TObjArray * LofSpecialButtons,
												Bool_t SeparatedActionButtons) :
							TGGroupFrame(Parent, Label, Profile->GetFrameOptions()),
							TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbCheckButton | TNGMrbGContext::kGMrbGroup,
												Buttons, FrameId, Profile, NofRows, NofCols, 0, 0, ButtonWidth,SeparatedActionButtons) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbCheckButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 TMrbLofNamedX * Buttons        -- button names and indices
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t NofRows                  -- number of rows
//                 Int_t NofCols                  -- number of columns
//                 Int_t ButtonWidth              -- button width
//                 TObjArray * LofSpecialButtons  -- special buttons
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	fLofSpecialButtons = LofSpecialButtons;
	if (Label != NULL && *Label != '\0') fLabelText = Label;
	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	this->CreateButtons();

}

TNGMrbCheckButtonGroup::TNGMrbCheckButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												const Char_t * Buttons,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t ButtonWidth,
												TObjArray * LofSpecialButtons, 
												Bool_t SeparatedActionButtons) :
							TGGroupFrame(Parent, Label, Profile->GetFrameOptions()),
							TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbCheckButton | TNGMrbGContext::kGMrbGroup,
												Buttons,FrameId, Profile, NofRows, NofCols, 0, 0, ButtonWidth, SeparatedActionButtons) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbCheckButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 Char_t * Buttons               -- button names separated by ":"
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t NofRows                  -- number of rows
//                 Int_t NofCols                  -- number of columns
//                 Int_t ButtonWidth              -- button width
//                 TObjArray * LofSpecialButtons  -- special buttons
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	fLofSpecialButtons = LofSpecialButtons;
	if (Label != NULL && *Label != '\0') fLabelText = Label;
	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	this->CreateButtons();

}
