//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbCheckButton.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of check buttons
// Description:    Implements class methods to handle a frame of
//                 check buttons.
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbCheckButton.cxx,v 1.2 2004-09-28 13:47:33 rudi Exp $       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbCheckButton1.gif>
<img src=gutils/TGMrbCheckButton2.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TGMrbCheckButton.h"

ClassImp(TGMrbCheckButtonList)
ClassImp(TGMrbCheckButtonGroup)

TGMrbCheckButtonList::TGMrbCheckButtonList(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons, Int_t NofCL,
												Int_t Width, Int_t Height,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * ButtonGC,
												TObjArray * LofSpecialButtons,
												UInt_t FrameOptions,
												UInt_t ButtonOptions) :
										TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()),
										TGMrbButtonFrame(Parent, Label, kGMrbCheckButton | kGMrbButtonList,
												Buttons, NofCL, Width, Height,
												FrameGC, LabelGC, ButtonGC, FrameOptions, ButtonOptions) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbCheckButtonList
// Purpose:        Define a list of check buttons arranged in a composite frame
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text ("" means "no label")
//                 TMrbLofNamedX * Buttons        -- button names and indices
//                 Int_t NofCL                    -- button grid CxL: number of columns/lines
//                 Int_t Width                    -- frame width
//                 Int_t Height                   -- frame height
//                 TGMrbContext * FrameGC         -- graphic context (frame)
//                 TGMrbContext * LabelGC         -- ... (label)
//                 TGMrbContext * ButtonGC        -- ... (button)
//                 TObjArray * LofSpecialButtons  -- special buttons
//                 UInt_t FrameOptions            -- frame options
//                 UInt_t ButtonOptions           -- options to configure buttons
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGLabel * label;

	fFrame = this;
	fLofSpecialButtons = LofSpecialButtons;
	fParentClient = fClient;
		
	if (Label != NULL) {
		label = new TGLabel(this, new TGString(Label), fLabelGC->GC(), fLabelGC->Font(), kChildFrame, fLabelGC->BG());
		fHeap.AddFirst((TObject *) label);
		this->AddFrame(label, fLabelGC->LH());
		label->SetTextJustify(kTextLeft);
	}

	this->PlaceButtons();
}

TGMrbCheckButtonGroup::TGMrbCheckButtonGroup(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Buttons, Int_t NofCL,
												TGMrbLayout * FrameGC,
												TGMrbLayout * ButtonGC,
												TObjArray * LofSpecialButtons,
												UInt_t FrameOptions,
												UInt_t ButtonOptions) :
							TGGroupFrame(Parent, Label, FrameOptions, FrameGC->GC(), FrameGC->Font(), FrameGC->BG()),
							TGMrbButtonFrame(Parent, Label, kGMrbCheckButton | kGMrbButtonGroup,
												Buttons, NofCL, 0, 0,
												FrameGC, NULL, ButtonGC, FrameOptions, ButtonOptions) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbCheckButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Arguments:      TGWindow * Parent              -- parent window
//                 Char_t * Label                 -- label text
//                 TMrbLofNamedX * Buttons        -- button names and indices
//                 Int_t NofCL                    -- button grid CxL: number of columns/lines
//                 TGMrbLayout * FrameGC          -- graphic context & layout (frame)
//                 TGMrbLayout * ButtonGC         -- ... (button)
//                 TObjArray * LofSpecialButtons  -- special buttons
//                 UInt_t FrameOptions            -- frame options
//                 UInt_t ButtonOptions           -- options to configure buttons
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fFrame = this;
	fLofSpecialButtons = LofSpecialButtons;
	fParentClient = fClient;

	this->PlaceButtons();
}
