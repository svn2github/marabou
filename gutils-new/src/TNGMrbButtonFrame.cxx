//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbButtonFrame.cxx
// Purpose:        MARaBOU graphic utilities: a list or group of buttons
// Description:    Implements basic class methods to handle a frame of buttons
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbButtonFrame.cxx,v 1.2 2009-03-31 06:12:06 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TObjString.h"
#include "TGToolTip.h"
#include "TMrbString.h"
#include "TNGMrbButtonFrame.h"

ClassImp(TNGMrbButtonFrame)
ClassImp(TNGMrbSpecialButton)

TNGMrbButtonFrame::TNGMrbButtonFrame(const TGWindow * Parent, UInt_t ButtonType,
											TMrbLofNamedX * Buttons, TNGMrbProfile * Profile,
											Int_t NofRows, Int_t NofCols,
											Int_t Width, Int_t Height, Int_t ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame
// Purpose:        Base class for lists of buttons
// Arguments:      TGWindow * Parent             -- parent widget
//                 UInt_t ButtonType             -- type of button (radio/check, composite/group)
//                 TMrbLofNamedX Buttons         -- button names and their indices
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

	fParent = Parent;
	fType = ButtonType;
	fLabel = NULL;
	fLabelText = "";
	fNofRows = NofRows;
	fNofCols = NofCols;
	fWidth = Width;
	fHeight = Height;
	fButtonWidth = ButtonWidth;
	fLabelGC = Profile->GetGC(TNGMrbGContext::kGMrbGCLabel);
	UInt_t bType = fType & (	TNGMrbGContext::kGMrbCheckButton
							|	TNGMrbGContext::kGMrbRadioButton
							|	TNGMrbGContext::kGMrbTextButton
							|	TNGMrbGContext::kGMrbPictureButton);
	TNGMrbGContext::EGMrbGCType gcType;
	switch (bType) {
		case TNGMrbGContext::kGMrbCheckButton:		gcType = TNGMrbGContext::kGMrbGCCheckButton; break;
		case TNGMrbGContext::kGMrbRadioButton:		gcType = TNGMrbGContext::kGMrbGCRadioButton; break;
		case TNGMrbGContext::kGMrbTextButton:		gcType = TNGMrbGContext::kGMrbGCTextButton; break;
		case TNGMrbGContext::kGMrbPictureButton: 	gcType = TNGMrbGContext::kGMrbGCPictureButton; break;
		default:									gcType = TNGMrbGContext::kGMrbGCButton; break;
	}
	fButtonGC = Profile->GetGC(gcType);

	fProfile = Profile;

	fFrameOptions = Profile->GetOptions(TNGMrbGContext::kGMrbGCFrame);
	fButtonOptions = Profile->GetOptions(gcType);

	fLofButtons.Delete();

	if (Buttons != NULL) {
		TIterator * iter = Buttons->MakeIterator();
		TMrbNamedX * nx;
		while (nx = (TMrbNamedX *) iter->Next()) fLofButtons.AddNamedX(nx);
	}
}

TNGMrbButtonFrame::TNGMrbButtonFrame(const TGWindow * Parent, UInt_t ButtonType,
											const Char_t * Buttons, TNGMrbProfile * Profile,
											Int_t NofRows, Int_t NofCols,
											Int_t Width, Int_t Height, Int_t ButtonWidth) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame
// Purpose:        Base class for lists of buttons
// Arguments:      TGWindow * Parent             -- parent widget
//                 UInt_t ButtonType             -- type of button (radio/check, composite/group)
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

	TMrbNamedX * nx;

	fParent = Parent;
	fType = ButtonType;
	fLabel = NULL;
	fLabelText = "";
	fNofRows = NofRows;
	fNofCols = NofCols;
	fWidth = Width;
	fHeight = Height;
	fButtonWidth = ButtonWidth;
	fLabelGC = Profile->GetGC(TNGMrbGContext::kGMrbGCLabel);
	UInt_t bType = fType & (	TNGMrbGContext::kGMrbCheckButton
							|	TNGMrbGContext::kGMrbRadioButton
							|	TNGMrbGContext::kGMrbTextButton
							|	TNGMrbGContext::kGMrbPictureButton);
	TNGMrbGContext::EGMrbGCType gcType;
	switch (bType) {
		case TNGMrbGContext::kGMrbCheckButton:		gcType = TNGMrbGContext::kGMrbGCCheckButton; break;
		case TNGMrbGContext::kGMrbRadioButton:		gcType = TNGMrbGContext::kGMrbGCRadioButton; break;
		case TNGMrbGContext::kGMrbTextButton:		gcType = TNGMrbGContext::kGMrbGCTextButton; break;
		case TNGMrbGContext::kGMrbPictureButton: 	gcType = TNGMrbGContext::kGMrbGCPictureButton; break;
		default:									gcType = TNGMrbGContext::kGMrbGCButton; break;
	}
	fButtonGC = Profile->GetGC(gcType);

	fProfile = Profile;

	fFrameOptions = Profile->GetOptions(TNGMrbGContext::kGMrbGCFrame);
	fButtonOptions = Profile->GetOptions(gcType);

	fLofButtons.Delete();

	if (Buttons != NULL) {
		TObjArray btns;
		TMrbString bstr = Buttons;
		Int_t nbtns = bstr.Split(btns);
		Int_t idx = 1;
		for (Int_t i = 0; i < nbtns; i++) {
			TString bTxt = ((TObjString *) btns[i])->GetString();
			Int_t bar = bTxt.Index("|", 0);
			if (bar > 0) {
				TString bCmt = bTxt(bar + 1, 1000);
				bTxt.Resize(bar);
				nx = new TMrbNamedX(idx, bTxt.Data(), bCmt.Data());
			} else {
				nx = new TMrbNamedX(idx, bTxt.Data(), bTxt.Data());
			}
			fLofButtons.AddNamedX(nx);
			if (ButtonType & TNGMrbGContext::kGMrbTextButton) idx++; else idx <<= 1;
		}
		fLofButtons.SetPatternMode((ButtonType & TNGMrbGContext::kGMrbTextButton) == 0);
	}
}

void TNGMrbButtonFrame::CreateButtons() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::CreateButtons
// Purpose:        Create button objects
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Instantiates button object. Buttons will be placed by
//                 the layout manager.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGButton * btn = NULL;
	TGCheckButton * cbtn;
	TGRadioButton * rbtn;
	TGTextButton * tbtn;
	TGPictureButton * pbtn;
	TNGMrbSpecialButton * sbtn2;
	
	fLofButtonWidths.Set(fLofButtons.GetEntriesFast() + 100);	// max 100 special buttons :-)

	if (fWidth == 0) fWidth = fFrame->GetDefaultWidth();
	if (fHeight == 0) fHeight = fFrame->GetDefaultHeight();

	fNofButtons = 0;
	fButtonHeight = 0;
	TMrbNamedX * nx;
	TIterator * iter = fLofButtons.MakeIterator();
	while (nx = (TMrbNamedX *) iter->Next()) {
		switch (fType & (TNGMrbGContext::kGMrbCheckButton | TNGMrbGContext::kGMrbRadioButton | TNGMrbGContext::kGMrbTextButton | TNGMrbGContext::kGMrbPictureButton)) {
			case TNGMrbGContext::kGMrbCheckButton:
				cbtn = new TGCheckButton(fFrame, nx->GetName(), nx->GetIndex());
				cbtn->SetFont(fButtonGC->Font());
				cbtn->SetForegroundColor(fButtonGC->FG());
				cbtn->SetBackgroundColor(fButtonGC->BG());
				cbtn->ChangeOptions(fButtonOptions);
				TO_HEAP(cbtn);
				btn = (TGButton *) cbtn;
				break;
			case TNGMrbGContext::kGMrbRadioButton:
				rbtn = new TGRadioButton(fFrame, nx->GetName(), nx->GetIndex());
				rbtn->SetFont(fButtonGC->Font());
				rbtn->SetForegroundColor(fButtonGC->FG());
				rbtn->SetBackgroundColor(fButtonGC->BG());
				rbtn->ChangeOptions(fButtonOptions);
				TO_HEAP(rbtn);
				btn = (TGButton *) rbtn;
				break;
			case TNGMrbGContext::kGMrbTextButton:
				tbtn = new TGTextButton(fFrame, nx->GetName(), nx->GetIndex());
				tbtn->SetFont(fButtonGC->Font());
				tbtn->SetForegroundColor(fButtonGC->FG());
				tbtn->SetBackgroundColor(fButtonGC->BG());
				tbtn->ChangeOptions(fButtonOptions);
				TO_HEAP(tbtn);
				btn = (TGButton *) tbtn;
				break;
			case TNGMrbGContext::kGMrbPictureButton:
				pbtn = new TGPictureButton(fFrame, fFrameClient->GetPicture(nx->GetName()), nx->GetIndex());
				pbtn->ChangeOptions(fButtonOptions);
				TO_HEAP(pbtn);
				btn = (TGButton *) pbtn;
				break;
		}
		btn->ChangeBackground(fButtonGC->BG());
		fFrame->AddFrame(btn);
		btn->Associate(fFrame);
		nx->AssignObject(btn);
		if (nx->HasTitle()) btn->SetToolTipText(nx->GetTitle(), 500);
		Int_t btnw = btn->GetDefaultWidth();
		fLofButtonWidths[fNofButtons] = btnw;
		if (btnw > fButtonWidth) fButtonWidth = btnw;
		Int_t btnh = btn->GetDefaultHeight();
		if (btnh > fButtonHeight) fButtonHeight = btnh;
		fNofButtons++;
	}

	fNofSpecialButtons = 0;
	if (fType & TNGMrbGContext::kGMrbCheckButton && fLofSpecialButtons) {
		TIterator * iter = fLofSpecialButtons->MakeIterator();
		TNGMrbSpecialButton * sbtn1;
		while (sbtn1 = (TNGMrbSpecialButton *) iter->Next()) {
			sbtn2 = new TNGMrbSpecialButton(	sbtn1->GetIndex(), sbtn1->GetName(), sbtn1->GetTitle(),
											sbtn1->GetPattern(), sbtn1->GetPicture());
			fLofButtons.Add(sbtn2);
			if (sbtn2->GetPicture() == NULL) {
				cbtn = new TGCheckButton(fFrame, sbtn2->GetName(), sbtn2->GetIndex());
				cbtn->SetFont(fButtonGC->Font());
				cbtn->SetForegroundColor(fButtonGC->FG());
				cbtn->SetBackgroundColor(fButtonGC->BG());
				cbtn->ChangeOptions(fButtonOptions);
				TO_HEAP(cbtn);
				btn = (TGButton *) cbtn;
			} else {
				pbtn = new TGPictureButton(fFrame, fFrameClient->GetPicture(sbtn2->GetPicture()), sbtn2->GetIndex());
				pbtn->ChangeOptions(fButtonOptions);
				TO_HEAP(pbtn);
				btn = (TGButton *) pbtn;
			}
			btn->ChangeBackground(fButtonGC->BG());
			fFrame->AddFrame(btn);
			btn->Associate(fFrame);
			sbtn2->AssignObject(btn);
			if (sbtn2->HasTitle()) btn->SetToolTipText(sbtn2->GetTitle(), 500);
			Int_t btnw = btn->GetDefaultWidth();
			fLofButtonWidths[fNofButtons + fNofSpecialButtons] = btnw;
			if (btnw > fButtonWidth) fButtonWidth = btnw;
			Int_t btnh = btn->GetDefaultHeight();
			if (btnh > fButtonHeight) fButtonHeight = btnh;
			fNofSpecialButtons++;
		}
	}

	Int_t btnNo = fNofButtons + fNofSpecialButtons;
	if (fNofRows == 0 && fNofCols == 0) {
		if (fFrameOptions & kHorizontalFrame) {
			fNofRows = 1;
			fNofCols = btnNo;
		} else {
			fNofRows = btnNo;
			fNofCols = 1;
		}
	} else if (fNofRows == 0) {
		fNofRows = (btnNo + fNofCols - 1) / fNofCols;
		fFrameOptions &= ~kHorizontalFrame;
		fFrameOptions |= kVerticalFrame;
	} else if (fNofCols == 0) {
		fNofCols = (btnNo + fNofRows - 1) / fNofRows;
		fFrameOptions &= ~kVerticalFrame;
		fFrameOptions |= kHorizontalFrame;
	}

	if (fNofRows * fNofCols < btnNo) {
		if (fNofRows < fNofCols) {
			fNofRows = (btnNo + fNofCols - 1) / fNofCols;
		} else {
			fNofCols = (btnNo + fNofRows - 1) / fNofRows;
		}
	}

	fColWidth.Set(fNofCols);
	fColWidth.Reset(fButtonWidth);		// columns width calculated for all buttons commonly
	fIndivColumnWidth = kFALSE;
	fPadLeft = -1;						// padding: left:right = .5 -> center
	fPadLeftFrac = .5;

	if (fWidth == 0) fWidth = fFrame->GetDefaultWidth();
	if (fHeight == 0) fHeight = fFrame->GetDefaultHeight();
	fFrame->Resize(fWidth, fHeight);
	fFrame->SetLayoutManager(new TNGMrbButtonFrameLayout(this));
}
				
void TNGMrbButtonFrame::Associate(const TGWindow * Window) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::Associate
// Purpose:        Associate button events with specified window
// Arguments:      TGWindow * Window    -- window which will care about button events
// Results:        
// Exceptions:     
// Description:    Redirects button events to another window.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TIterator * iter = fLofButtons.MakeIterator();
	TMrbNamedX * nx;
	while (nx = (TMrbNamedX *) iter->Next()) {
		 TGButton * button = (TGButton *) nx->GetAssignedObject();
		button->Associate(Window);
	}
}

void TNGMrbButtonFrame::ClearAll() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::ClearAll
// Purpose:        Clear all buttons in the list
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Sets all buttons to inactive.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TIterator * iter = fLofButtons.MakeIterator();
	TMrbNamedX * nx;
	while (nx = (TMrbNamedX *) iter->Next()) {
		TGButton * button = (TGButton *) nx->GetAssignedObject();
		if (button->GetState() == kButtonEngaged) button->SetState(kButtonUp);
	}
}

void TNGMrbButtonFrame::SetState(UInt_t Pattern, EButtonState State) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::SetState
// Purpose:        Mark button(s) as (in)active
// Arguments:      UInt_t Pattern       -- pattern describing button(s) to be set
//                 EButtonState State   -- button up or down
// Results:        
// Exceptions:     
// Description:    Sets button(s) given by pattern to (in)active.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TIterator * iter = fLofButtons.MakeIterator();
	TMrbNamedX * nx;
	if (fType & TNGMrbGContext::kGMrbRadioButton) {
		if (State == kButtonEngaged || State == kButtonDisabled) {
			while (nx = (TMrbNamedX *) iter->Next()) {
				TGButton * button = (TGButton *) nx->GetAssignedObject();
				if ((nx->GetIndex() & Pattern) == (UInt_t) nx->GetIndex()) button->SetState(State);
			}
		} else {
			while (nx = (TMrbNamedX *) iter->Next()) {
				TGButton * button = (TGButton *) nx->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if ((UInt_t) nx->GetIndex() == Pattern) {
						button->SetState(State);
					} else {
						button->SetState(kButtonUp);
					}
				}
			}
		}
	} else if (fType & TNGMrbGContext::kGMrbCheckButton) {
		TNGMrbSpecialButton * sbutton = this->FindSpecialButton(Pattern);
		if (sbutton) Pattern = sbutton->GetPattern();
		if (State == kButtonEngaged || State == kButtonDisabled) {
			while (nx = (TMrbNamedX *) iter->Next()) {
				TGButton * button = (TGButton *) nx->GetAssignedObject();
				if (nx->GetIndex() & Pattern) button->SetState(State);
			}
		} else {
			while (nx = (TMrbNamedX *) iter->Next()) {
				TGButton * button = (TGButton *) nx->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if (nx->GetIndex() & Pattern) {
						button->SetState(State);
					} else if (sbutton) {
						button->SetState(kButtonUp);
					}
				}
			}
		}
	} else if (fType & TNGMrbGContext::kGMrbTextButton) {
		while (nx = (TMrbNamedX *) iter->Next()) {
			TGButton * button = (TGButton *) nx->GetAssignedObject();
			if ((UInt_t) nx->GetIndex() == Pattern) {
				button->SetState(State);
			}
		}
	}
}

UInt_t TNGMrbButtonFrame::GetActive() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::GetActive
// Purpose:        Get pattern of active button(s)
// Arguments:      --
// Results:        UInt_t Pattern    -- pattern of active buttons
// Exceptions:     
// Description:    Reads pattern of active buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGButton * button;
	UInt_t pattern;

	pattern = 0;

	TIterator * iter = fLofButtons.MakeIterator();
	TMrbNamedX * nx;
	while (nx = (TMrbNamedX *) iter->Next()) {
		button = (TGButton *) nx->GetAssignedObject();
		if (button->GetState() == kButtonDown) pattern |= nx->GetIndex();
	}
	return(pattern);
}

void TNGMrbButtonFrame::FlipState(UInt_t Pattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::FlipState
// Purpose:        Flip current check button state
// Arguments:      UInt_t Pattern       -- pattern describing button(s)
//                                         to be flipped
// Results:        
// Exceptions:     
// Description:    Flip state for given (check) button(s).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fType & TNGMrbGContext::kGMrbCheckButton) {
		TIterator * iter = fLofButtons.MakeIterator();
		TMrbNamedX * nx;
		while (nx = (TMrbNamedX *) iter->Next()) {
			TGButton * button = (TGButton *) nx->GetAssignedObject();
			UInt_t idx;
			if ((idx = nx->GetIndex()) & Pattern) {
				EButtonState state = button->GetState();
				if (state == kButtonDown) {
					button->SetState(kButtonUp);
				} else if (state == kButtonUp) {
					button->SetState(kButtonDown);
				}
			}
		}
	}
}

void TNGMrbButtonFrame::UpdateState(UInt_t Pattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::UpdateState
// Purpose:        Update check button state
// Arguments:      UInt_t Pattern       -- pattern describing check button(s)
//                                         to be updated
// Results:        
// Exceptions:     
// Description:    Update state for given check button(s).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fType & TNGMrbGContext::kGMrbCheckButton) {
		TNGMrbSpecialButton *sbutton = this->FindSpecialButton(Pattern);
		if (sbutton) {
			UInt_t pat = sbutton->GetPattern();
			TIterator * iter = fLofButtons.MakeIterator();
			TMrbNamedX * nx;
			while (nx = (TMrbNamedX *) iter->Next()) {
				TGButton * button = (TGButton *) nx->GetAssignedObject();
				if (button->GetState() != kButtonDisabled) {
					if ((UInt_t) nx->GetIndex() == Pattern) {
						button->SetState(kButtonDown);
					} else if (nx->GetIndex() & pat) {
						button->SetState(kButtonDown);
					} else {
						button->SetState(kButtonUp);
					}
				}
			}
		}
	}
}

void TNGMrbButtonFrame::AddButton(TGButton * Button, TMrbNamedX * ButtonSpecs) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::AddButton
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
	fLofButtons.AddNamedX(ButtonSpecs);
}

TGButton * TNGMrbButtonFrame::GetButton(Int_t ButtonIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::GetButton
// Purpose:        Get a button by its index
// Arguments:      Int_t ButtonIndex      -- button index
// Results:        TGButton * Button      -- addr of button or NULL
// Exceptions:     NULL if button not found
// Description:    Searches for a button by its index and return its address.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;

	if ((nx = fLofButtons.FindByIndex(ButtonIndex)) == NULL) return(NULL);
	return((TGButton *) nx->GetAssignedObject());
}

void TNGMrbButtonFrame::SetButtonWidth(Int_t Width, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::SetButtonWidth
// Purpose:        Set width of a button
// Arguments:      Int_t Width          -- width
//                 Int_t ButtonIndex    -- button index
// Results:        
// Exceptions:     
// Description:    Sets width of specified button (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (ButtonIndex != 0) {
		TMrbNamedX * nx;
		if ((nx = fLofButtons.FindByIndex(ButtonIndex)) != NULL) {
			TGButton * button = (TGButton *) nx->GetAssignedObject();
			button->Resize(Width, button->GetDefaultHeight());
			button->Layout();
		}
	} else {
		TIterator * iter = fLofButtons.MakeIterator();
		TMrbNamedX * nx;
		while (nx = (TMrbNamedX *) iter->Next()) {
			TGButton * button = (TGButton *) nx->GetAssignedObject();
			button->Resize(Width, button->GetDefaultHeight());
			button->Layout();
		}
	}
}

Int_t TNGMrbButtonFrame::GetButtonWidth(Int_t ButtonIndex)  const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::SetButtonWidth
// Purpose:        Get label width of a button
// Arguments:      Int_t ButtonIndex    -- button index
// Results:        Int_t Width          -- width
// Exceptions:     
// Description:    Gets width of specified button
//                 (if index = 0 -> max of all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t width = 0;
	if (ButtonIndex != 0) {
		TMrbNamedX * nx;
		if ((nx = fLofButtons.FindByIndex(ButtonIndex)) != NULL) {
			TGButton * button = (TGButton *) nx->GetAssignedObject();
			width = button->GetWidth();
		}
	} else {
		width = 0;
		TIterator * iter = fLofButtons.MakeIterator();
		TMrbNamedX * nx;
		while (nx = (TMrbNamedX *) iter->Next()) {
			TGButton * button = (TGButton *) nx->GetAssignedObject();
			Int_t wd = button->GetWidth();
			width = (wd > width) ? wd : width;
		}
	}
	return(width);
}

void TNGMrbButtonFrame::ChangeButtonBackground(ULong_t Color, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::ChangeButtonBackground
// Purpose:        Set background color of a button list
// Arguments:      ULong_t Color        -- color
//                 Int_t ButtonIndex   -- button index
// Results:        
// Exceptions:     
// Description:    Sets background color of specified button (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (ButtonIndex != 0) {
		TMrbNamedX * nx;
		if ((nx = fLofButtons.FindByIndex(ButtonIndex)) != NULL) {
			TGButton * button = (TGButton *) nx->GetAssignedObject();
			button->ChangeBackground(Color);
			button->Layout();
		}
	} else {
		TIterator * iter = fLofButtons.MakeIterator();
		TMrbNamedX * nx;
		while (nx = (TMrbNamedX *) iter->Next()) {
			TGButton * button = (TGButton *) nx->GetAssignedObject();
			button->ChangeBackground(Color);
			button->Layout();
		}
	}
}

void TNGMrbButtonFrame::JustifyButton(ETextJustification Justify, Int_t ButtonIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::JustifyButton
// Purpose:        Justify button text
// Arguments:      ETextJustification Justify  -- how to justify the text
//                 Int_t ButtonIndex           -- button index
// Results:        
// Exceptions:     
// Description:    Justifies button text (if index = 0 -> all buttons).
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if ((fType & TNGMrbGContext::kGMrbTextButton) == 0) return;

	if (ButtonIndex != 0) {
		TMrbNamedX * nx;
		if ((nx = fLofButtons.FindByIndex(ButtonIndex)) != NULL) {
			TGTextButton * button = (TGTextButton *) nx->GetAssignedObject();
			button->SetTextJustify(Justify);
			button->Layout();
		}
	} else {
		TIterator * iter = fLofButtons.MakeIterator();
		TMrbNamedX * nx;
		while (nx = (TMrbNamedX *) iter->Next()) {
			TGTextButton * button = (TGTextButton *) nx->GetAssignedObject();
			button->SetTextJustify(Justify);
			button->Layout();
		}
	}
}

Bool_t TNGMrbButtonFrame::ButtonFrameMessage(Long_t MsgId, Long_t MsgParm) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::ProcessMessage
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

TNGMrbSpecialButton * TNGMrbButtonFrame::FindSpecialButton(Int_t Index) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::FindSpecialButton
// Purpose:        Search a special button
// Arguments:      Int_t Index      -- button index
// Results:        
// Exceptions:     
// Description:    Searches a special button by a given index.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fLofSpecialButtons == NULL) return(NULL);
	
	TIterator * iter = fLofSpecialButtons->MakeIterator();
	TNGMrbSpecialButton * sbtn;
	while (sbtn = (TNGMrbSpecialButton *) iter->Next()) {
		if (sbtn->GetIndex() == Index) return(sbtn);
	}
	return(NULL);
}

void TNGMrbButtonFrame::SetIndivColumnWidth(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::SetIndivColumnWidth
// Purpose:        Calculate columns widths indiviually
// Arguments:      Bool_t Flag       -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Calculate column widths.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fIndivColumnWidth = Flag;
	Int_t btnNo = fNofButtons + fNofSpecialButtons;
	if (fIndivColumnWidth) {
		fColWidth.Reset();
		for (Int_t i = 0; i < btnNo; i++) {
			Int_t col = i % fNofCols;
			if (fColWidth[col] < fLofButtonWidths[i]) fColWidth[col] = fLofButtonWidths[i];
		}
	} else {
		fColWidth.Reset(fButtonWidth);
	}
}
	
Int_t TNGMrbButtonFrame::GetPadLeft(Int_t Points) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame::GetPadLeft
// Purpose:        Calculate left padding
// Arguments:      Int_t Points      -- points available, left + right
// Results:        Int_t PadLeft     -- how many points to shift from left
// Exceptions:     
// Description:    Calculate amount of points to be shifted from left.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t padLeft;
	Double_t padLeftFrac;

	if (fPadLeftFrac < 0.) {
		padLeft = Points - fPadLeft;
		if (padLeft < 0) padLeft = Points;
		else if (padLeft > Points) padLeft = 0;
	} else {
		padLeftFrac = fPadLeftFrac;
		if (padLeftFrac < 0.) padLeftFrac = 0.;
		else if (padLeftFrac > 1.) padLeftFrac = 1.;
		padLeft = (Int_t) ((Double_t) Points * padLeftFrac +.5);
	}
	return(padLeft);
}	

void TNGMrbButtonFrameLayout::Layout() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrameLayout::Layout
// Purpose:        Layout manager
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Placement of widget elements
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fWidget) {
		Int_t boxHeight = (Int_t) fWidget->GetHeight();
		Int_t btnHeight = fWidget->GetButtonHeight();
		Int_t btnboxHeight = fWidget->GetNofRows() * (btnHeight + TNGMrbButtonFrame::kGMrbButtonPadH) + 2 * TNGMrbButtonFrame::kGMrbButtonPadH;
		Int_t border = fWidget->GetFrame()->GetBorderWidth();
		if (btnboxHeight + border > boxHeight) boxHeight = btnboxHeight + border + 4 * TNGMrbButtonFrame::kGMrbButtonPadH;

		Int_t labelWidth = fWidget->HasLabel() ? (Int_t) fWidget->GetLabel()->GetWidth() : 0;	// label width
		Int_t boxWidth = (Int_t) fWidget->GetWidth();
		Int_t btnboxWidth = 0;
		for (Int_t i = 0; i < fWidget->GetNofCols(); i++) {
			btnboxWidth += fWidget->GetColWidth()->At(i)+ TNGMrbButtonFrame::kGMrbButtonPadW;
		}
		btnboxWidth += 3 * TNGMrbButtonFrame::kGMrbButtonPadW;
	
		if (labelWidth + btnboxWidth > boxWidth) boxWidth = labelWidth + btnboxWidth;
		if ((fWidget->GetFrameOptions() & kHorizontalFrame) && fWidget->HasIndivColumnWidth()) boxWidth = labelWidth + btnboxWidth;

		fWidget->GetFrame()->Resize((UInt_t) boxWidth, (UInt_t) boxHeight);
			
		Int_t y = fWidget->GetFrame()->GetBorderWidth() + TNGMrbButtonFrame::kGMrbButtonPadH;
		TMrbNamedX * nx = (TMrbNamedX *) fWidget->GetLofButtons()->First();

		Int_t padLeft = fWidget->GetPadLeft(boxWidth - btnboxWidth);
		for (Int_t row = 0; row < fWidget->GetNofRows(); row++) {
			Int_t x = boxWidth - padLeft - btnboxWidth + 2 * TNGMrbButtonFrame::kGMrbButtonPadW;
			for (Int_t col = 0; col < fWidget->GetNofCols(); col++) {
				TGButton * btn = (TGButton *) nx->GetAssignedObject();
				Int_t bw = fWidget->GetColWidth()->At(col);
				btn->MoveResize(x, y, bw, btnHeight);
				x += (bw + TNGMrbButtonFrame::kGMrbButtonPadW);
				nx = (TMrbNamedX *) fWidget->GetLofButtons()->After(nx);
				if (nx == NULL) return;
			}
			y += (btnHeight + TNGMrbButtonFrame::kGMrbButtonPadH);
		}
	}
}
