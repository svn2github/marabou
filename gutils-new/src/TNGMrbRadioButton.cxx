//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbRadioButton.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of radio buttons
// Description:    Implements class methods to handle a frame of
//                 radio buttons.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbRadioButton.cxx,v 1.5 2009-09-23 10:42:52 Marabou Exp $       
// Date:           
// Layout:         A list or group of radio buttons
//Begin_Html
/*
<img src=gutils/TNGMrbRadioButtonGroup.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TNGMrbRadioButton.h"

ClassImp(TNGMrbRadioButtonList)
ClassImp(TNGMrbRadioButtonGroup)
ClassImp(TNGMrbYesNoButtonList)
ClassImp(TNGMrbYesNoButtonGroup)

TNGMrbRadioButtonList::TNGMrbRadioButtonList(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons, Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t Width, Int_t Height, Int_t ButtonWidth, Bool_t SeparatedActionButtons) :
										TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbRadioButton | TNGMrbGContext::kGMrbList,
												Buttons,FrameId,  Profile, NofRows, NofCols, Width, Height, ButtonWidth, SeparatedActionButtons) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbRadioButtonList
// Purpose:        Define a list of radio buttons arranged in a composite frame
// Arguments:      TGWindow * Parent               -- parent window
//                 Char_t * Label                  -- label text (NULL means "no label")
//                 TMrbLofNamedX * Buttons         -- button names and indices
//                 TNGMrbProfile * Profile          -- graphics profile
//                 Int_t NofRows                   -- number of rows
//                 Int_t NofCols                   -- number of columns
//                 Int_t Width                     -- frame width
//                 Int_t Height                    -- frame height
//                 Int_t ButtonWidth               -- button width
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	if (Label != NULL && *Label != '\0') {
		fLabelText = Label;
		fLabel = new TGLabel(this, new TGString(Label));
		fLabel->SetTextFont(Profile->GetGC(TNGMrbGContext::kGMrbGCLabel)->Font()); 
		fLabel->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCLabel)->FG());
		fLabel->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCLabel)->BG());
		TO_HEAP(fLabel);
		TGLayoutHints * LayoutH = new TGLayoutHints (kLHintsExpandX|kLHintsExpandY, 1,1,1,1);
		this->AddFrame(fLabel, LayoutH);
		fLabel->SetTextJustify(kTextLeft);
	}

	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	this->CreateButtons();
}

TNGMrbRadioButtonList::TNGMrbRadioButtonList(const TGWindow * Parent,
												const Char_t * Label,
												const Char_t * Buttons,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t Width, Int_t Height, Int_t ButtonWidth, Bool_t SeparatedActionButtons) :
										TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbRadioButton | TNGMrbGContext::kGMrbList,
												Buttons,FrameId, Profile, NofRows, NofCols, Width, Height, ButtonWidth, SeparatedActionButtons) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbRadioButtonList
// Purpose:        Define a list of radio buttons arranged in a composite frame
// Arguments:      TGWindow * Parent               -- parent window
//                 Char_t * Label                  -- label text (NULL means "no label")
//                 Char_t * Buttons                -- button names separated by ":"
//                 TNGMrbProfile * Profile          -- graphics profile
//                 Int_t NofRows                   -- number of rows
//                 Int_t NofCols                   -- number of columns
//                 Int_t Width                     -- frame width
//                 Int_t Height                    -- frame height
//                 Int_t ButtonWidth               -- button width
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
		TO_HEAP(fLabel);
		this->AddFrame(fLabel);
		fLabel->SetTextJustify(kTextLeft);
	}

	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	this->CreateButtons();
}

TNGMrbRadioButtonGroup::TNGMrbRadioButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t ButtonWidth, Bool_t SeparatedActionButtons) :
								TGGroupFrame(Parent, Label, Profile->GetFrameOptions()),
								TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbRadioButton | TNGMrbGContext::kGMrbGroup,
												Buttons,FrameId,  Profile, NofRows, NofCols, 0, 0, ButtonWidth, SeparatedActionButtons) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbRadioButtonGroup
// Purpose:        Define a group of radio buttons and their behaviour
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 TMrbLofNamedX * Buttons        -- button names and indices
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t NofRows                  -- number of rows
//                 Int_t NofCols                  -- number of columns
//                 Int_t ButtonWidth              -- button width
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	if (Label != NULL && *Label != '\0') fLabelText = Label;

	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	this->CreateButtons();
}

TNGMrbRadioButtonGroup::TNGMrbRadioButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												const Char_t * Buttons,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t ButtonWidth, Bool_t SeparatedActionButtons) :
								TGGroupFrame(Parent, Label, Profile->GetFrameOptions()),
								TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbRadioButton | TNGMrbGContext::kGMrbGroup,
												Buttons, FrameId,Profile, NofRows, NofCols, 0, 0, ButtonWidth, SeparatedActionButtons) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbRadioButtonGroup
// Purpose:        Define a group of radio buttons and their behaviour
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 Char_t * Buttons               -- button names separated by ":"
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t NofRows                  -- number of rows
//                 Int_t NofCols                  -- number of columns
//                 Int_t ButtonWidth              -- button width
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fFrameClient = fClient;
	if (Label != NULL && *Label != '\0') fLabelText = Label;
	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	this->CreateButtons();
}

TNGMrbYesNoButtonList::TNGMrbYesNoButtonList(const TGWindow * Parent,
												const Char_t * Label,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t Width, Int_t Height, Int_t ButtonWidth) :
										TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbRadioButton | TNGMrbGContext::kGMrbList,
												"yes:no",FrameId, Profile, 0, 0, Width, Height, ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbYesNoButtonList
// Purpose:        Define a yes/no button arranged in a composite frame
// Arguments:      TGWindow * Parent               -- parent window
//                 Char_t * Label                  -- label text (NULL means "no label")
//                 TNGMrbProfile * Profile          -- graphics profile
//                 Int_t Width                     -- frame width
//                 Int_t Height                    -- frame height
//                 Int_t ButtonWidth               -- button width
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
		TO_HEAP(fLabel);
		this->AddFrame(fLabel);
		fLabel->SetTextJustify(kTextLeft);
	}
	this->CreateButtons();
}

TNGMrbYesNoButtonGroup::TNGMrbYesNoButtonGroup(const TGWindow * Parent,
												const Char_t * Label,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t ButtonWidth) :
								TGGroupFrame(Parent, Label, Profile->GetFrameOptions()),
								TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbRadioButton | TNGMrbGContext::kGMrbGroup,
												"yes:no",FrameId, Profile, 0, 0, 0, 0, ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbRadioButtonGroup
// Purpose:        Define a yes/no button pair arranged in a group frame
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t ButtonWidth              -- button width
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
