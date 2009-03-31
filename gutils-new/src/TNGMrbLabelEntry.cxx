//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbLabelEntry.cxx
// Purpose:        MARaBOU graphic utilities: a labelled entry
// Description:    Implements class methods to handle a labelled entry
//                 Based on TGNumberEntry object (Daniel Sigg)
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbLabelEntry.cxx,v 1.3 2009-03-31 14:34:32 Rudolf.Lutter Exp $       
// Date:           
// Layout:         A labelled entry field with up/down/begin/end buttons
//Begin_Html
/*
<img src=gutils/TNGMrbLabelEntry.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGClient.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGToolTip.h"
#include "TMrbString.h"
#include "TMrbLogger.h"
#include "TNGMrbLabelEntry.h"

#include "SetColor.h"

ClassImp(TNGMrbLabelEntry)

extern TMrbLogger * gMrbLog;		// access to message logging

TNGMrbLabelEntry::TNGMrbLabelEntry(	const TGWindow * Parent,
									const Char_t * Label,
									TNGMrbProfile * Profile,
									Int_t EntryId, Int_t Width, Int_t Height, Int_t EntryWidth,
									UInt_t EntryOptions,
									TMrbNamedX * Action) :
							TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions() | kHorizontalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry
// Purpose:        Define a label together with an text entry field
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 TNGMrbProfile * Profile      -- graphics profile
//                 Int_t EntryId               -- id to be used in ProcessMessage
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- width of the entry field
//                 UInt_t EntryOptions         -- options to configure the entry
//                 TMrbNamedX * Action         -- action button
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	TNGMrbGContext * labelGC = Profile->GetGC(TNGMrbGContext::kGMrbGCLabel);
	TNGMrbGContext * entryGC = Profile->GetGC(TNGMrbGContext::kGMrbGCTextEntry);
	TNGMrbGContext * buttonGC = Profile->GetGC(TNGMrbGContext::kGMrbGCButton);

	fEntryWidth = EntryWidth;
	fLabel = NULL;
	fNumberEntry = NULL;
	fNumberEntryField = NULL;
	fTextEntry = NULL;
	fAction = NULL;
	fButtonBegin = NULL;
	fButtonEnd = NULL;

	if (Label != NULL) {
		fLabel = new TGLabel(this, new TGString(Label));
		fLabel->SetTextFont(labelGC->Font());
		fLabel->SetForegroundColor(labelGC->FG());
		fLabel->SetBackgroundColor(labelGC->BG());
		fLabel->ChangeOptions(labelGC->GetOptions());
		TO_HEAP(fLabel);
		this->AddFrame(fLabel);
		fLabel->SetTextJustify(kTextLeft);
	}

	if (Action && Action->GetAssignedObject()) {
		fAction = new TGTextButton(this, Action->GetName(), Action->GetIndex());
		TO_HEAP(fAction);
		fAction->SetFont(buttonGC->Font());
		fAction->SetToolTipText(Action->GetTitle(), 500);
		fAction->SetBackgroundColor(buttonGC->BG());
		this->AddFrame(fAction);			
		fAction->Associate((const TGWindow *) Action->GetAssignedObject());
	}

	if (EntryOptions & kGMrbEntryIsNumber) {
		if (EntryOptions & kGMrbEntryHasUpDownButtons) {
			fNumberEntry = new TGNumberEntry(this, 0., EntryWidth, EntryId);
			TO_HEAP(fNumberEntry);
			fNumberEntryField = fNumberEntry->GetNumberEntry();
			fTextEntry = fNumberEntryField;
			this->AddFrame(fNumberEntry);
			fNumberEntry->GetButtonUp()->SetToolTipText("StepUp", 500);
			fNumberEntry->GetButtonDown()->SetToolTipText("StepDown", 500);
			if (EntryOptions & kGMrbEntryHasBeginEndButtons) {
				fButtonEnd = new TGPictureButton(this, fClient->GetPicture("arrow_rightright.xpm"), kGMrbEntryButtonEnd);
				TO_HEAP(fButtonEnd);
				fButtonEnd->ChangeBackground(buttonGC->BG());
				fButtonEnd->SetToolTipText("ToEnd", 500);
				fButtonEnd->SetState(kButtonDisabled);
				fButtonEnd->Associate(this);
				this->AddFrame(fButtonEnd);
				fButtonBegin = new TGPictureButton(this, fClient->GetPicture("arrow_leftleft.xpm"), kGMrbEntryButtonBegin);
				TO_HEAP(fButtonBegin);
				fButtonBegin->ChangeBackground(buttonGC->BG());
				fButtonBegin->SetToolTipText("ToBegin", 500);
				fButtonBegin->SetState(kButtonDisabled);
				fButtonBegin->Associate(this);
				this->AddFrame(fButtonBegin);
			}
			this->GetButtonUp()->ChangeBackground(buttonGC->BG());
			this->GetButtonDown()->ChangeBackground(buttonGC->BG());
			this->GetButtonUp()->Associate(this);
			this->GetButtonDown()->Associate(this);
		} else {
			fNumberEntry = NULL;
			fNumberEntryField = new TGNumberEntryField(this, EntryId, 0.);
			TO_HEAP(fNumberEntryField);
			fTextEntry = fNumberEntryField;
			this->AddFrame(fNumberEntryField);
		}
	} else {
		fNumberEntry = NULL;
		fNumberEntryField = NULL;
		fTextEntry = new TGTextEntry(this, new TGTextBuffer(), EntryId);
		TO_HEAP(fTextEntry);
		this->AddFrame(fTextEntry);
	}

	this->SetFont(entryGC->Font());
	this->SetEntryBackground(entryGC->BG());
	this->Resize(Width, Height);
	this->SetLayoutManager(new TNGMrbLabelEntryLayout(this));
}

Bool_t TNGMrbLabelEntry::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::ProcessMessage
// Purpose:        Message handler for up/down buttons
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages reveived from up/down buttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (GET_MSG(MsgId)) {
		case kC_COMMAND:

			switch (GET_SUBMSG(MsgId)) {

				case kCM_BUTTON:
					switch (Param1) {
						case kGMrbEntryButtonDown:
						case kGMrbEntryButtonUp:
							fNumberEntry->ProcessMessage(MsgId, Param1, Param2);
							switch(this->GetNumStyle()) {
								case TGNumberEntry::kNESHex:
									this->SetHexNumber((Int_t) this->GetNumber());
									break;
							}
							break;
						case kGMrbEntryButtonBegin:
							if (this->HasBeginEndButtons()) {
								UInt_t l = this->GetNumLimits();
								UInt_t s = this->GetNumStyle();
								if (l == TGNumberEntry::kNELLimitMin || l == TGNumberEntry::kNELLimitMinMax) {
									switch (s) {
										case TGNumberEntry::kNESInteger:
										case TGNumberEntry::kNESDegree:
										case TGNumberEntry::kNESMinSec:
										case TGNumberEntry::kNESHourMin:
										case TGNumberEntry::kNESHourMinSec:
										case TGNumberEntry::kNESDayMYear:
										case TGNumberEntry::kNESMDayYear:
											this->SetIntNumber((Int_t) this->GetNumMin());
											break;
										case TGNumberEntry::kNESHex:
											this->SetHexNumber((Int_t) this->GetNumMin());
											break;
										case TGNumberEntry::kNESRealOne:
										case TGNumberEntry::kNESRealTwo:
										case TGNumberEntry::kNESRealThree:
										case TGNumberEntry::kNESRealFour:
										case TGNumberEntry::kNESReal:
											this->SetNumber(this->GetNumMin());
											break;
									}
								}
							}
							break;
						case kGMrbEntryButtonEnd:
							if (this->HasBeginEndButtons()) {
								UInt_t l = this->GetNumLimits();
								UInt_t s = this->GetNumStyle();
								if (l == TGNumberEntry::kNELLimitMax || l == TGNumberEntry::kNELLimitMinMax) {
									switch (s) {
										case TGNumberEntry::kNESInteger:
										case TGNumberEntry::kNESDegree:
										case TGNumberEntry::kNESMinSec:
										case TGNumberEntry::kNESHourMin:
										case TGNumberEntry::kNESHourMinSec:
										case TGNumberEntry::kNESDayMYear:
										case TGNumberEntry::kNESMDayYear:
											this->SetIntNumber((Int_t) this->GetNumMax());
											break;
										case TGNumberEntry::kNESHex:
											this->SetHexNumber((Int_t) this->GetNumMax());
											break;
										case TGNumberEntry::kNESRealOne:
										case TGNumberEntry::kNESRealTwo:
										case TGNumberEntry::kNESRealThree:
										case TGNumberEntry::kNESRealFour:
										case TGNumberEntry::kNESReal:
											this->SetNumber(this->GetNumMax());
											break;
									}
								}
							}
							break;
					}
			}
			break;
	}
	return(kTRUE);
}

void TNGMrbLabelEntry::UpDownButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::UpDownButtonEnable
// Purpose:        Enable/disable up/down buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables up/down buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntry) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fNumberEntry->GetButtonUp()->SetState(btnState);
		fNumberEntry->GetButtonDown()->SetState(btnState);
	}
}

void TNGMrbLabelEntry::BeginEndButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::fNumberEntry->GetButtonUp()->SetState(btnState);
// Purpose:        Enable/disable begin/end buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables begin/end buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fButtonBegin) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fButtonBegin->SetState(btnState);
		fButtonEnd->SetState(btnState);
	}
}

void TNGMrbLabelEntry::ActionButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::ActionButtonEnable
// Purpose:        Enable/disable action button
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables action button.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fAction) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fAction->SetState(btnState);
	}
}

void TNGMrbLabelEntry::SetFormat(TGNumberEntry::EStyle Style, TGNumberEntry::EAttribute Attr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetFormat
// Purpose:        Set limits
// Arguments:      EStyle Style     -- style
//                 EAttribute Attr  -- attribute
// Results:        --
// Exceptions:     
// Description:    Sets number style & attr
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntryField) fNumberEntryField->SetFormat((TGNumberEntry::EStyle) Style, (TGNumberEntry::EAttribute) Attr);
}

void TNGMrbLabelEntry::SetLimits(TGNumberEntry::ELimit Limits, Double_t Min, Double_t Max) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetLimits
// Purpose:        Set limits
// Arguments:      ELimit Limits   -- limit specifier
//                 Double_t Min    -- min value
//                 Double_t Max    -- max value
// Results:        --
// Exceptions:     
// Description:    Sets limits & enables/disables begin/end buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntryField) {
		fNumberEntryField->SetLimits((TGNumberEntry::ELimit) Limits, Min, Max);
		fNumberEntryField->SetToolTipText(Form("%g ... %g", Min, Max), 500);
		if (fButtonBegin) {
			switch (Limits) {
				case TGNumberEntry::kNELNoLimits:
					fButtonBegin->SetState(kButtonDisabled);
					fButtonEnd->SetState(kButtonDisabled);
					break;
				case TGNumberEntry::kNELLimitMin:
					fButtonBegin->SetState(kButtonUp);
					fButtonEnd->SetState(kButtonDisabled);
					break;
				case TGNumberEntry::kNELLimitMax:
					fButtonBegin->SetState(kButtonDisabled);
					fButtonEnd->SetState(kButtonUp);
					break;
				case TGNumberEntry::kNELLimitMinMax:
					fButtonBegin->SetState(kButtonUp);
					fButtonEnd->SetState(kButtonUp);
					break;
			}
		}
	}
} 

void TNGMrbLabelEntry::SetHexNumber(UInt_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetHexNumber
// Purpose:        Display in hex format
// Arguments:      UInt_t Value    -- integer value to be displayed
// Results:        --
// Exceptions:     
// Description:    Displays an integer value in hex format.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntryField) {
		fNumberEntryField->SetHexNumber((ULong_t) Value);
		TString x = "0x";
		x += fNumberEntryField->GetText();
		fNumberEntryField->TGTextEntry::SetText(x.Data());
	}
}

void TNGMrbLabelEntry::SetOctNumber() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetOctNumber
// Purpose:        Display in octal format
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Displays an integer value in octal format.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

}

void TNGMrbLabelEntry::SetBinNumber() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetOctNumber
// Purpose:        Display in binary format
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Displays an integer value in binary format.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

}

void TNGMrbLabelEntry::SetTimeLimit(TGNumberEntry::ELimit Limit, Int_t Hour, Int_t Min, Int_t Sec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetTimeLimit
// Purpose:        Set time limit
// Arguments:      ELimit Limit    -- limit specifier (min or max)
//                 Int_t Hour      -- hour
//                 Int_t Min       -- min
//                 Int_t Sec       -- sec
// Results:        --
// Exceptions:     
// Description:    Sets time limit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Double_t t;
	TGNumberEntry::EStyle s = (TGNumberEntry::EStyle) this->GetNumStyle();
	if (this->CheckStyle(s, kGMrbEntryTime)) {
		switch (s) {
			case TGNumberEntry::kNESHourMinSec:
				if (!this->CheckRange(Hour, 0, 24) || !this->CheckRange(Min, 0, 60) || !this->CheckRange(Sec, 0, 60)) return;
				t = 3600 * Hour + 60 * Min + Sec;
				break;
			case TGNumberEntry::kNESHourMin:
				if (!this->CheckRange(Hour, 0, 24) || !this->CheckRange(Min, 0, 60) || !this->CheckRange(Sec, 0, 0)) return;
				t = 60 * Hour + Min;
				break;
			case TGNumberEntry::kNESMinSec:
				if (!this->CheckRange(Hour, 0, 0) || !this->CheckRange(Min, 0, 60) || !this->CheckRange(Sec, 0, 60)) return;
				t = 60 * Min + Sec;
				break;
		}
		if (Limit == TGNumberEntry::kNELLimitMin) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), t, this->GetNumMax());
		} else if (Limit == TGNumberEntry::kNELLimitMax) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), this->GetNumMin(), t);
		}
	}
}

void TNGMrbLabelEntry::SetDateLimit(TGNumberEntry::ELimit Limit, Int_t Year, Int_t Month, Int_t Day) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetDateLimit
// Purpose:        Set date limit
// Arguments:      ELimit Limit    -- limit specifier (min or max)
//                 Int_t Year      -- year
//                 Int_t Month     -- month
//                 Int_t Day       -- day
// Results:        --
// Exceptions:     
// Description:    Sets time limit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGNumberEntry::EStyle s = (TGNumberEntry::EStyle) this->GetNumStyle();
	if (this->CheckStyle(s, kGMrbEntryDate) && this->CheckRange(Month, 1, 12) && this->CheckRange(Day, 1, 31)) {
		Double_t d = 10000 * Year + 100 * Month + Day;
		if (Limit == TGNumberEntry::kNELLimitMin) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), d, this->GetNumMax());
		} else if (Limit == TGNumberEntry::kNELLimitMax) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), this->GetNumMin(), d);
		}
	}
}

void TNGMrbLabelEntry::SetDegree(Int_t Deg, Int_t Min, Int_t Sec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetDegree
// Purpose:        Set degree value
// Arguments:      Int_t Deg       -- degree
//                 Int_t Min       -- min
//                 Int_t Sec       -- sec
// Results:        --
// Exceptions:     
// Description:    Sets degree value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGNumberEntry::EStyle s = (TGNumberEntry::EStyle) this->GetNumStyle();
	if (this->CheckStyle(s, kGMrbEntryDegree) && this->CheckRange(Deg, -360, 360) && this->CheckRange(Min, 0, 60) && this->CheckRange(Sec, 0, 60)) {
		Int_t d = 3600 * Deg + 60 * Min + Sec;
		this->SetIntNumber(d);
	}
}

void TNGMrbLabelEntry::SetDegreeLimit(TGNumberEntry::ELimit Limit, Int_t Deg, Int_t Min, Int_t Sec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetDegreeLimit
// Purpose:        Set degree limit
// Arguments:      ELimit Limit    -- limit specifier (min or max)
//                 Int_t Deg       -- degree
//                 Int_t Min       -- min
//                 Int_t Sec       -- sec
// Results:        --
// Exceptions:     
// Description:    Sets degree limit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGNumberEntry::EStyle s = (TGNumberEntry::EStyle) this->GetNumStyle();
	if (this->CheckStyle(s, kGMrbEntryDegree) && this->CheckRange(Deg, -360, 360) && this->CheckRange(Min, 0, 60) && this->CheckRange(Sec, 0, 60)) {
		Double_t d = 3600 * Deg + 60 * Min + Sec;
		if (Limit == TGNumberEntry::kNELLimitMin) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), d, this->GetNumMax());
		} else if (Limit == TGNumberEntry::kNELLimitMax) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), this->GetNumMin(), d);
		}
	}
}

void TNGMrbLabelEntry::SetButtonBackground(Pixel_t Color) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetButtonBackground
// Purpose:        Change background color for buttons
// Arguments:      Pixel_t Color     -- color
// Results:        --
// Exceptions:     
// Description:    Changes background color.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntry) {
		fNumberEntry->GetButtonUp()->SetBackgroundColor(Color);
		fNumberEntry->GetButtonDown()->SetBackgroundColor(Color);
	}
	if (fButtonBegin) {
		fButtonBegin->SetBackgroundColor(Color);
		fButtonEnd->SetBackgroundColor(Color);
	}

	if (fAction) {
		fAction->SetBackgroundColor(Color);
	}
}

Bool_t TNGMrbLabelEntry::CheckRange(Int_t Value, Int_t LowerLim, Int_t UpperLim) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::CheckRange
// Purpose:        Check range
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Checks if value within range.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Value < LowerLim || Value > UpperLim) {
		gMrbLog->Err()	<< "Illegal range - " << Value
						<< " (should be in [" << LowerLim << "," << UpperLim << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckRange");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TNGMrbLabelEntry::CheckStyle(TGNumberEntry::EStyle Style, EGMrbEntryStyle StyleGroup) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::CheckStyle
// Purpose:        Check style
// Arguments:      EStyle Style                -- style
//                 EGMrbEntryStyle StyleGroup  -- style group
// Results:        --
// Exceptions:     
// Description:    Checks for appropriate style.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	EGMrbEntryStyle sg;
	TString s1, s2;

	switch (Style) {
		case TGNumberEntry::kNESInteger:
		case TGNumberEntry::kNESHex:
			sg = kGMrbEntryInt;
			s1 = "integer";
			break;
		case TGNumberEntry::kNESRealOne:
		case TGNumberEntry::kNESRealTwo:
		case TGNumberEntry::kNESRealThree:
		case TGNumberEntry::kNESRealFour:
		case TGNumberEntry::kNESReal:
			sg = kGMrbEntryReal;
			s1 = "real";
			break;
		case TGNumberEntry::kNESDegree:
			sg = kGMrbEntryDegree;
			s1 = "degree";
			break;
		case TGNumberEntry::kNESHourMinSec:
		case TGNumberEntry::kNESMinSec:
		case TGNumberEntry::kNESHourMin:
			sg = kGMrbEntryTime;
			s1 = "time";
			break;
		case TGNumberEntry::kNESDayMYear:
		case TGNumberEntry::kNESMDayYear:
			sg = kGMrbEntryDate;
			s1 = "date";
			break;
	}
	if (sg != StyleGroup) {
		switch (StyleGroup) {
			case kGMrbEntryInt: 	s2 = "integer"; break;
			case kGMrbEntryReal:	s2 = "real"; break;
			case kGMrbEntryDegree:	s2 = "degree"; break;
			case kGMrbEntryTime:	s2 = "time"; break;
			case kGMrbEntryDate:	s2 = "date"; break;
		}
		gMrbLog->Err()	<< "Illegal number style - " << s2
						<< " (should be \"" << s1 << "\")" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckStyle");
		return(kFALSE);
	}
	return(kTRUE);
}

void TNGMrbLabelEntryLayout::Layout() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntryLayout::Layout
// Purpose:        Layout manager
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Placement of widget elements
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fWidget) {
		UInt_t boxWidth = fWidget->GetWidth();			// width of labelled-entry box (label + entry + buttons)
		UInt_t boxHeight = fWidget->GetHeight() - 2 * TNGMrbLabelEntry::kGMrbEntryPadH;	// height
		UInt_t entryWidth = fWidget->GetEntryWidth(); 	// width of entry field (entry + buttons)

		UInt_t labelWidth = fWidget->HasLabel() ? fWidget->GetLabel()->GetWidth() : 0;	// label width

		if (entryWidth == 0) entryWidth = boxWidth - labelWidth;

		if (labelWidth + entryWidth > boxWidth) {
			boxWidth = labelWidth + entryWidth;
			fWidget->Resize(boxWidth, fWidget->GetHeight());
		}
			
		UInt_t btnWidth = 2 * boxHeight / 3;				// button width
		Int_t x = boxWidth;
		Int_t y = TNGMrbLabelEntry::kGMrbEntryPadH;
		Int_t delta = 0;

		if (fWidget->HasActionButton()) {
			UInt_t w = fWidget->GetActionButton()->GetWidth();
			x -= w; delta += w;
			fWidget->GetActionButton()->MoveResize(x, y, w, boxHeight);
		}

		if (fWidget->IsNumberEntry() && fWidget->HasBeginEndButtons()) {
			x -= btnWidth; delta += btnWidth;
			fWidget->GetButtonEnd()->MoveResize(x, y, btnWidth, boxHeight/2);
			fWidget->GetButtonBegin()->MoveResize(x, y + boxHeight/2, btnWidth, boxHeight/2);
		}

		x = boxWidth - entryWidth;		// entry starts 'entryWidth' from right end
		entryWidth -= delta;			// compensate for buttons

		if (fWidget->IsNumberEntry()) {
			if (fWidget->HasUpDownButtons()) {
				fWidget->GetNumberEntry()->MoveResize(x, y, entryWidth, boxHeight);
			} else {
				fWidget->GetNumberEntryField()->MoveResize(x, y, entryWidth, boxHeight);
			}
		} else {
			fWidget->GetTextEntry()->MoveResize(x, y, entryWidth, boxHeight);
		}

		if (fWidget->HasLabel()) {
			fWidget->GetLabel()->MoveResize(TNGMrbLabelEntry::kGMrbEntryPadW, y, x - TNGMrbLabelEntry::kGMrbEntryPadW, boxHeight);
		}
	}
}
