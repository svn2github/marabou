//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbButtonFrame.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of buttons
// Description:    Implements basic class methods to handle a frame of buttons
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

#include "TGToolTip.h"
#include "TGMrbButtonFrame.h"

ClassImp(TGMrbButtonFrame)
ClassImp(TGMrbSpecialButton)

TGMrbButtonFrame::TGMrbButtonFrame(const TGWindow * Parent, const Char_t * Label, UInt_t ButtonType,
											TMrbLofNamedX * Buttons, Int_t NofCL,
											Int_t Width, Int_t Height,
											TGMrbLayout * FrameGC, TGMrbLayout * LabelGC, TGMrbLayout * ButtonGC,
											UInt_t FrameOptions, UInt_t ButtonOptions) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame
// Purpose:        Base class for lists of buttons
// Arguments:      TGWindow * Parent             -- parent widget
//                 Char_t * Label                -- label text
//                 UInt_t ButtonType             -- type of button (radio/check, composite/group)
//                 TMrbLofNamedX Buttons         -- button names and their indices
//                 Int_t NofCL                   -- button grid CxL: number of columns/lines
//                 Int_t Width                   -- frame width
//                 Int_t Height                  -- frame height
//                 TGMrbLayout * FrameLayout     -- frame layout
//                 TGMrbLayout * LabelLayout     -- label layout
//                 TGMrbLayout * ButtonLayout    -- button layout
//                 UInt_t FrameOptions           -- frame options
//                 UInt_t ButtonOptions          -- button options
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;

	fParent = Parent;
	fLabel = Label ? Label : "";
	fType = ButtonType;
	fNofCL = NofCL;
	fWidth = Width;
	fHeight = Height;
	fFrameGC = FrameGC;
	fLabelGC = this->SetupGC(LabelGC, FrameOptions);
	fButtonGC = this->SetupGC(ButtonGC, FrameOptions);
	fFrameOptions = FrameOptions;
	fButtonOptions = ButtonOptions;

	fButtons.Delete();

	if (Buttons != NULL) {
		namedX = (TMrbNamedX *) Buttons->First();
		while (namedX) {
			fButtons.AddNamedX(namedX);
			namedX = (TMrbNamedX *) Buttons->After((TObject *) namedX);
		}
	}
};

void TGMrbButtonFrame::PlaceButtons() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::PlaceButtons
// Purpose:        Place buttons
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Places buttons in the frame.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t btnNo;
	TMrbNamedX * nx;
	TGCompositeFrame * btnFrame;
	TGCompositeFrame * subFrame = NULL;
	TGLayoutHints * frameLayout;
	UInt_t btnOptions, subOptions;

	TGButton * btn = NULL;
	TGCheckButton * cbtn;
	TGRadioButton * rbtn;
	TGTextButton * tbtn;
	TGPictureButton * pbtn;
	TGMrbSpecialButton * sbtn1, * sbtn2;
	
	subOptions = fFrameOptions & (kVerticalFrame | kHorizontalFrame | kChildFrame);
 	if (fNofCL > 1) {
		btnOptions = (fFrameOptions & kVerticalFrame) ? kHorizontalFrame : kVerticalFrame;
	} else {
		btnOptions = fFrameOptions & (kVerticalFrame | kHorizontalFrame);
	}
	subOptions |= kChildFrame;

	if (fWidth == 0) fWidth = fFrame->GetDefaultWidth();
	if (fHeight == 0) fHeight = fFrame->GetDefaultHeight();

	if (fNofCL > 1) {
		if (fType & kGMrbTextButton)	frameLayout = new TGLayoutHints(kLHintsExpandX | kLHintsLeft, 0, 0, 0);
		else							frameLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 0);
		fHeap.AddFirst((TObject *) frameLayout);
	} else {
		frameLayout = fFrameGC->LH();
	}
	
	btnFrame = new TGCompositeFrame(fFrame, fWidth, fHeight, btnOptions, fFrameGC->BG());
	fHeap.AddFirst((TObject *) btnFrame);
	fFrame->AddFrame(btnFrame, frameLayout);

	btnNo = 0;
	nx = (TMrbNamedX *) fButtons.First();
	while (nx) {
		if (fNofCL > 1) {
			if ((btnNo % fNofCL) == 0) {
				subFrame = new TGCompositeFrame(btnFrame, fWidth, fHeight, subOptions, fFrameGC->BG());
				fHeap.AddFirst((TObject *) subFrame);
				btnFrame->AddFrame(subFrame, frameLayout);
			}
		} else {
			subFrame = btnFrame;
		}
		switch (fType & (kGMrbCheckButton | kGMrbRadioButton | kGMrbTextButton | kGMrbPictureButton)) {
			case kGMrbCheckButton:
				cbtn = new TGCheckButton(subFrame,	nx->GetName(),
													nx->GetIndex(),
													fButtonGC->GC(), fButtonGC->Font(), fButtonOptions);
				fHeap.AddFirst((TObject *) cbtn);
				btn = (TGButton *) cbtn;
				break;
			case kGMrbRadioButton:
				rbtn = new TGRadioButton(subFrame,	nx->GetName(),
													nx->GetIndex(),
													fButtonGC->GC(), fButtonGC->Font(), fButtonOptions);
				fHeap.AddFirst((TObject *) rbtn);
				btn = (TGButton *) rbtn;
				break;
			case kGMrbTextButton:
				tbtn = new TGTextButton(subFrame,	nx->GetName(),
													nx->GetIndex(),
													fButtonGC->GC(), fButtonGC->Font(), fButtonOptions);
				fHeap.AddFirst((TObject *) tbtn);
				btn = (TGButton *) tbtn;
				break;
			case kGMrbPictureButton:
				pbtn = new TGPictureButton(subFrame, fParentClient->GetPicture(nx->GetName()),
													nx->GetIndex(),
													fButtonGC->GC(), fButtonOptions);
				fHeap.AddFirst((TObject *) pbtn);
				btn = (TGButton *) pbtn;
				break;
		}
		btn->ChangeBackground(fButtonGC->BG());
		subFrame->AddFrame(btn, fButtonGC->LH());
		btn->Associate(fFrame);
		nx->AssignObject(btn);
		if (nx->HasTitle()) btn->SetToolTipText(nx->GetTitle(), 500);
		btnNo++;
		nx = (TMrbNamedX *) fButtons.After(nx);
	}

	if (fType & kGMrbCheckButton && fLofSpecialButtons) {
		sbtn1 = (TGMrbSpecialButton *) fLofSpecialButtons->First();
		while (sbtn1) {
			sbtn2 = new TGMrbSpecialButton(	sbtn1->GetIndex(), sbtn1->GetName(), sbtn1->GetTitle(),
											sbtn1->GetPattern(), sbtn1->GetPicture());
			fButtons.Add(sbtn2);
			if (fNofCL > 1) {
				if ((btnNo % fNofCL) == 0) {
					subFrame = new TGCompositeFrame(btnFrame, fWidth, fHeight, subOptions, fFrameGC->BG());
					fHeap.AddFirst((TObject *) subFrame);
					btnFrame->AddFrame(subFrame, frameLayout);
				}
			} else {
				subFrame = btnFrame;
			}
			if (sbtn2->GetPicture() == NULL) {
				cbtn = new TGCheckButton(subFrame,	sbtn2->GetName(),
													sbtn2->GetIndex(),
													fButtonGC->GC(), fButtonGC->Font(), fButtonOptions);
				fHeap.AddFirst((TObject *) cbtn);
				btn = (TGButton *) cbtn;
			} else {
				pbtn = new TGPictureButton(subFrame, fParentClient->GetPicture(sbtn2->GetPicture()),
													sbtn2->GetIndex(),
													fButtonGC->GC(), fButtonOptions | kRaisedFrame);
				fHeap.AddFirst((TObject *) pbtn);
				btn = (TGButton *) pbtn;
			}
			btn->ChangeBackground(fButtonGC->BG());
			subFrame->AddFrame(btn, new TGLayoutHints(kLHintsCenterY,	fButtonGC->LH()->GetPadLeft(),
															fButtonGC->LH()->GetPadRight(),
															fButtonGC->LH()->GetPadTop(),
															fButtonGC->LH()->GetPadBottom()));
			btn->Associate(fFrame);
			sbtn2->AssignObject(btn);
			if (sbtn2->HasTitle()) btn->SetToolTipText(sbtn2->GetTitle(), 500);
			btnNo++;
			sbtn1 = (TGMrbSpecialButton *) fLofSpecialButtons->After(sbtn1);
		}
	}
}
				
void TGMrbButtonFrame::Associate(const TGWindow * Window) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::Associate
// Purpose:        Associate button events with specified window
// Arguments:      TGWindow * Window    -- window which will care about button events
// Results:        
// Exceptions:     
// Description:    Redirects button events to another window.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;


	namedX = (TMrbNamedX *) fButtons.First();
	while (namedX) {
		button = (TGButton *) namedX->GetAssignedObject();
		button->Associate(Window);
		namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
	}
}

void TGMrbButtonFrame::ClearAll() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::ClearAll
// Purpose:        Clear all buttons in the list
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Sets all buttons to inactive.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;

	namedX = (TMrbNamedX *) fButtons.First();
	while (namedX) {
		button = (TGButton *) namedX->GetAssignedObject();
		if (button->GetState() == kButtonEngaged) button->SetState(kButtonUp);
		namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
	}
}

void TGMrbButtonFrame::SetState(UInt_t Pattern, EButtonState State) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::SetState
// Purpose:        Mark button(s) as (in)active
// Arguments:      UInt_t Pattern       -- pattern describing button(s) to be set
//                 EButtonState State   -- button up or down
// Results:        
// Exceptions:     
// Description:    Sets button(s) given by pattern to (in)active.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;
	TGMrbSpecialButton * sbutton;

	namedX = (TMrbNamedX *) fButtons.First();
	if (fType & kGMrbRadioButton) {
		if (State == kButtonEngaged || State == kButtonDisabled) {
			while (namedX) {
				button = (TGButton *) namedX->GetAssignedObject();
				if ((namedX->GetIndex() & Pattern) == (UInt_t) namedX->GetIndex()) button->SetState(State);
				namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
			}
		} else {
			while (namedX) {
				button = (TGButton *) namedX->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if ((UInt_t) namedX->GetIndex() == Pattern) {
						button->SetState(State);
					} else {
						button->SetState(kButtonUp);
					}
				}
				namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
			}
		}
	} else if (fType & kGMrbCheckButton) {
		sbutton = this->FindSpecialButton(Pattern);
		if (sbutton) Pattern = sbutton->GetPattern();
		if (State == kButtonEngaged || State == kButtonDisabled) {
			while (namedX) {
				button = (TGButton *) namedX->GetAssignedObject();
				if (namedX->GetIndex() & Pattern) button->SetState(State);
				namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
			}
		} else {
			while (namedX) {
				button = (TGButton *) namedX->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if (namedX->GetIndex() & Pattern) {
						button->SetState(State);
					} else if (sbutton) {
						button->SetState(kButtonUp);
					}
				}
				namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
			}
		}
	} else if (fType & kGMrbTextButton) {
		while (namedX) {
			button = (TGButton *) namedX->GetAssignedObject();
			if ((UInt_t) namedX->GetIndex() == Pattern) {
				button->SetState(State);
			}
			namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
		}
	}
}

UInt_t TGMrbButtonFrame::GetActive() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::GetActive
// Purpose:        Get pattern of active button(s)
// Arguments:      --
// Results:        UInt_t Pattern    -- pattern of active buttons
// Exceptions:     
// Description:    Reads pattern of active buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;
	UInt_t pattern;

	pattern = 0;

	namedX = (TMrbNamedX *) fButtons.First();
	while (namedX) {
		button = (TGButton *) namedX->GetAssignedObject();
		if (button->GetState() == kButtonDown) pattern |= namedX->GetIndex();
		namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
	}
	return(pattern);
}

void TGMrbButtonFrame::FlipState(UInt_t Pattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::FlipState
// Purpose:        Flip current check button state
// Arguments:      UInt_t Pattern       -- pattern describing button(s)
//                                         to be flipped
// Results:        
// Exceptions:     
// Description:    Flip state for given (check) button(s).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;
	UInt_t idx;
	EButtonState state;

	if (fType & kGMrbCheckButton) {
		namedX = (TMrbNamedX *) fButtons.First();
		while (namedX) {
			button = (TGButton *) namedX->GetAssignedObject();
			if ((idx = namedX->GetIndex()) & Pattern) {
				state = button->GetState();
				if (state == kButtonDown) {
					button->SetState(kButtonUp);
				} else if (state == kButtonUp) {
					button->SetState(kButtonDown);
				}
			}
			namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
		}
	}
}

void TGMrbButtonFrame::UpdateState(UInt_t Pattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::UpdateState
// Purpose:        Update check button state
// Arguments:      UInt_t Pattern       -- pattern describing check button(s)
//                                         to be updated
// Results:        
// Exceptions:     
// Description:    Update state for given check button(s).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;
	TGMrbSpecialButton * sbutton;
	UInt_t pat;

	if (fType & kGMrbCheckButton) {
		sbutton = this->FindSpecialButton(Pattern);
		if (sbutton) {
			pat = sbutton->GetPattern();
			namedX = (TMrbNamedX *) fButtons.First();
			while (namedX) {
				button = (TGButton *) namedX->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if ((UInt_t) namedX->GetIndex() == Pattern) {
						button->SetState(kButtonDown);
					} else if (namedX->GetIndex() & pat) {
						button->SetState(kButtonDown);
					} else {
						button->SetState(kButtonUp);
					}
				}
				namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
			}
		}
	}
}

void TGMrbButtonFrame::AddButton(TGButton * Button, TMrbNamedX * ButtonSpecs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::AddButton
// Purpose:        Add a button to the list
// Arguments:      TGButton * Button         -- button widget
//                 TMrbNamedX * ButtonSpecs  -- name and index
// Results:        
// Exceptions:     
// Description:    Adds a button to the list. No window mapping is done.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	ButtonSpecs->AssignObject((TObject *) Button);
	Button->Associate((const TGWindow *) this);
	fButtons.AddNamedX(ButtonSpecs);
}

TGButton * TGMrbButtonFrame::GetButton(Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::GetButton
// Purpose:        Get a button by its index
// Arguments:      Int_t ButtonIndex      -- button index
// Results:        TGButton * Button      -- addr of button or NULL
// Exceptions:     NULL if button not found
// Description:    Searches for a button by its index and return its address.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;

	if ((namedX = fButtons.FindByIndex(ButtonIndex)) == NULL) return(NULL);
	return((TGButton *) namedX->GetAssignedObject());
}

void TGMrbButtonFrame::SetButtonWidth(Int_t Width, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::SetButtonWidth
// Purpose:        Set label width of a button list
// Arguments:      Int_t Width          -- width
//                 Int_t ButtonIndex    -- button index
// Results:        
// Exceptions:     
// Description:    Sets width of specified button (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;

	if (ButtonIndex != 0) {
		if ((namedX = fButtons.FindByIndex(ButtonIndex)) != NULL) {
			button = (TGButton *) namedX->GetAssignedObject();
			button->Resize(Width, button->GetDefaultHeight());
			button->Layout();
		}
	} else {
		namedX = (TMrbNamedX *) fButtons.First();
		while (namedX) {
			button = (TGButton *) namedX->GetAssignedObject();
			button->Resize(Width, button->GetDefaultHeight());
			button->Layout();
			namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
		}
	}
}

Int_t TGMrbButtonFrame::GetButtonWidth(Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::SetButtonWidth
// Purpose:        Set label width of a button list
// Arguments:      Int_t ButtonIndex    -- button index
// Results:        Int_t Width          -- width
// Exceptions:     
// Description:    Gets width of specified button
//                 (if index = 0 -> max of all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;
	Int_t width = 0;
	Int_t wd;

	if (ButtonIndex != 0) {
		if ((namedX = fButtons.FindByIndex(ButtonIndex)) != NULL) {
			button = (TGButton *) namedX->GetAssignedObject();
			width = button->GetWidth();
		}
	} else {
		namedX = (TMrbNamedX *) fButtons.First();
		width = 0;
		while (namedX) {
			button = (TGButton *) namedX->GetAssignedObject();
			wd = button->GetWidth();
			width = (wd > width) ? wd : width;
			namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
		}
	}
	return(width);
}

void TGMrbButtonFrame::ChangeButtonBackground(ULong_t Color, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::ChangeButtonBackground
// Purpose:        Set background color of a button list
// Arguments:      ULong_t Color        -- color
//                 Int_t ButtonIndex   -- button index
// Results:        
// Exceptions:     
// Description:    Sets background color of specified button (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGButton * button;

	if (ButtonIndex != 0) {
		if ((namedX = fButtons.FindByIndex(ButtonIndex)) != NULL) {
			button = (TGButton *) namedX->GetAssignedObject();
			button->ChangeBackground(Color);
			button->Layout();
		}
	} else {
		namedX = (TMrbNamedX *) fButtons.First();
		while (namedX) {
			button = (TGButton *) namedX->GetAssignedObject();
			button->ChangeBackground(Color);
			button->Layout();
			namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
		}
	}
}

void TGMrbButtonFrame::JustifyButton(ETextJustification Justify, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::JustifyButton
// Purpose:        Justify button text
// Arguments:      ETextJustification Justify  -- how to justify the text
//                 Int_t ButtonIndex           -- button index
// Results:        
// Exceptions:     
// Description:    Justifies button text (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * namedX;
	TGTextButton * button;

	if ((fType & kGMrbTextButton) == 0) return;

	if (ButtonIndex != 0) {
		if ((namedX = fButtons.FindByIndex(ButtonIndex)) != NULL) {
			button = (TGTextButton *) namedX->GetAssignedObject();
			button->SetTextJustify(Justify);
			button->Layout();
		}
	} else {
		namedX = (TMrbNamedX *) fButtons.First();
		while (namedX) {
			button = (TGTextButton *) namedX->GetAssignedObject();
			button->SetTextJustify(Justify);
			button->Layout();
			namedX = (TMrbNamedX *) fButtons.After((TObject *) namedX);
		}
	}
}

Bool_t TGMrbButtonFrame::ButtonFrameMessage(Long_t MsgId, Long_t MsgParm) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::ProcessMessage
// Purpose:        Message handler for a list of buttons
// Arguments:      Long_t MsgId      -- message id
//                 Long_t MsgParmX   -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to the list of buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (GET_MSG(MsgId) == kC_COMMAND) {
		switch(GET_SUBMSG(MsgId)) {
			case kCM_RADIOBUTTON:	this->SetState((UInt_t) MsgParm); return(kTRUE);
			case kCM_CHECKBUTTON:	this->UpdateState((UInt_t) MsgParm); return(kTRUE);
			case kCM_BUTTON:		this->UpdateState((UInt_t) MsgParm); return(kTRUE);
		}
	}
	return(kTRUE);
}

TGMrbSpecialButton * TGMrbButtonFrame::FindSpecialButton(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame::FindSpecialButton
// Purpose:        Search a special button
// Arguments:      Int_t Index      -- button index
// Results:        
// Exceptions:     
// Description:    Searches a special button by a given index.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbSpecialButton * sbtn;
	
	if (fLofSpecialButtons == NULL) return(NULL);
	
	sbtn = (TGMrbSpecialButton *) fLofSpecialButtons->First();
	while (sbtn) {
		if (sbtn->GetIndex() == Index) return(sbtn);
		sbtn = (TGMrbSpecialButton *) fLofSpecialButtons->After(sbtn);
	}
	return(NULL);
}
