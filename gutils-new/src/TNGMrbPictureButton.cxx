//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbPictureButton.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of picture buttons
// Description:    Implements class methods to handle a frame of
//                 picture buttons.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbPictureButton.cxx,v 1.3 2009-05-27 07:36:49 Marabou Exp $       
// Date:           
// Layout:         A list or group of picture buttons
//Begin_Html
/*
<img src=gutils/TNGMrbPictureButtonGroup.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TNGMrbPictureButton.h"

ClassImp(TNGMrbPictureButtonList)
ClassImp(TNGMrbPictureButtonGroup)

TNGMrbPictureButtonList::TNGMrbPictureButtonList(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons, Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t Width, Int_t Height, Int_t ButtonWidth) :
										TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbPictureButton | TNGMrbGContext::kGMrbButtonList,
												Buttons,FrameId, Profile, NofRows, NofCols, Width, Height, ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbPictureButtonList
// Purpose:        Define a list of check buttons arranged in a composite frame
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text ("" means "no label")
//                 TMrbLofNamedX * Buttons        -- picture names and indices
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t NofRows                  -- number of rows
//                 Int_t NofCols                  -- number of columns
//                 Int_t Width                    -- frame width
//                 Int_t Height                   -- frame height
//                 Int_t ButtonWidth              -- button width
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
		TO_HEAP(fLabel);
		this->AddFrame(fLabel);
		fLabel->SetTextJustify(kTextLeft);
	}
	this->CreateButtons();
}

TNGMrbPictureButtonList::TNGMrbPictureButtonList(const TGWindow * Parent,
												const Char_t * Label,
												const Char_t * Buttons,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t Width, Int_t Height, Int_t ButtonWidth) :
										TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions()),
										TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbPictureButton | TNGMrbGContext::kGMrbButtonList,
												Buttons,FrameId,  Profile, NofRows, NofCols, Width, Height, ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbPictureButtonList
// Purpose:        Define a list of check buttons arranged in a composite frame
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text ("" means "no label")
//                 Char_t * Buttons               -- picture names separated by ":"
//                 TNGMrbProfile * Profile         -- graphics profile
//                 Int_t NofRows                  -- number of rows
//                 Int_t NofCols                  -- number of columns
//                 Int_t Width                    -- frame width
//                 Int_t Height                   -- frame height
//                 Int_t ButtonWidth              -- button width
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
		TO_HEAP(fLabel);
		this->AddFrame(fLabel);
		fLabel->SetTextJustify(kTextLeft);
	}
	this->CreateButtons();
}

TNGMrbPictureButtonGroup::TNGMrbPictureButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t ButtonWidth) :
							TGGroupFrame(Parent, Label, Profile->GetFrameOptions()),
							TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbPictureButton | TNGMrbGContext::kGMrbButtonGroup,
												Buttons,FrameId, Profile, NofRows, NofCols, 0, 0, ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbPictureButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 TMrbLofNamedX * Buttons        -- picture names and indices
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
	this->CreateButtons();
}

TNGMrbPictureButtonGroup::TNGMrbPictureButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												const Char_t * Buttons,Int_t FrameId,
												TNGMrbProfile * Profile,
												Int_t NofRows, Int_t NofCols,
												Int_t ButtonWidth) :
							TGGroupFrame(Parent, Label, Profile->GetFrameOptions()),
							TNGMrbButtonFrame(Parent, TNGMrbGContext::kGMrbPictureButton | TNGMrbGContext::kGMrbButtonGroup,
												Buttons,FrameId, Profile, NofRows, NofCols, 0, 0, ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbPictureButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 Char_t * Buttons               -- picture names separated by ":"
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
	this->CreateButtons();
}
