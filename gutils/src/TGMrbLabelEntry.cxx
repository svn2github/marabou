//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbLabelEntry.cxx
// Purpose:        MARaBOU graphic utilities: a labelled entry
// Description:    Implements class methods to handle a labelled entry
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbLabelEntry.cxx,v 1.12 2006-09-12 08:02:47 Marabou Exp $       
// Date:           
// Layout: A plain entry
//Begin_Html
/*
<img src=gutils/TGMrbLabelEntry1.gif>
<img src=gutils/TGMrbLabelEntry2.gif>
<img src=gutils/TGMrbLabelEntry3.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TGMsgBox.h"
#include "TGButton.h"
#include "TGToolTip.h"
#include "TMrbString.h"
#include "TMrbLogger.h"
#include "TGMrbLabelEntry.h"

#include "SetColor.h"

ClassImp(TGMrbTextEntry)
ClassImp(TGMrbLabelEntry)

extern TMrbLogger * gMrbLog;		// access to message logging

TGMrbLabelEntry::TGMrbLabelEntry(const TGWindow * Parent,
												const Char_t * Label,
												Int_t BufferSize, Int_t EntryId,
												Int_t Width, Int_t Height,
												Int_t EntryWidth,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * EntryGC,
												TGMrbLayout * UpDownBtnGC,
												Bool_t BeginEndBtns,
												TMrbNamedX * Action, TGMrbLayout * ActionGC,
												UInt_t FrameOptions,
												UInt_t EntryOptions) :
										TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry
// Purpose:        Define a label together with an text entry field
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 Int_t BufferSize            -- size of text buffer in chars
//                 Int_t EntryId               -- id to be used in ProcessMessage
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- widht of the entry field
//                 Bool_t UpDownBtn            -- kTRUE: append up/down buttons
//                 TGMrbLayout * FrameGC       -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC       -- ... (label)
//                 TGMrbLayout * EntryGC       -- ... (entry)
//                 TGMrbLayout * UpDownBtnGC   -- ... (up/down btns) - adds buttons "<" and ">"
//                 Bool_t BeginEndBtns         -- if kTRUE add two additional buttons "<<" and ">>"
//                 TMrbNamedX * Action         -- adds an "action" button
//                 TGMrbLayout * ActionGC      -- graphic context & layout (action)
//                 UInt_t FrameOptions         -- frame options
//                 UInt_t EntryOptions         -- options to configure the entry
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bSize;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	LabelGC = this->SetupGC(LabelGC, FrameOptions);
	EntryGC = this->SetupGC(EntryGC, FrameOptions);

	fUp = NULL;
	fDown = NULL;
	fBegin = NULL;
	fEnd = NULL;

	fLowerLimit = 0;
	fUpperLimit = 0;
	fIncrement = 1;
	fBase = TMrbString::kDefaultBase;
	fPrecision = TMrbString::kDefaultPrecision;
	fType = TGMrbLabelEntry::kGMrbEntryTypeChar;
	fLabel = NULL;

	fShowToolTip = kFALSE;
	fShowRange = kFALSE;

	if (Label != NULL) {
		fLabel = new TGLabel(this, new TGString(Label), LabelGC->GC(), LabelGC->Font(), kChildFrame, LabelGC->BG());
		fHeap.AddFirst((TObject *) fLabel);
		this->AddFrame(fLabel, LabelGC->LH());
		fLabel->SetTextJustify(kTextLeft);
	}

	bSize = 0;

	if (Action && Action->GetAssignedObject()) {
		TGLayoutHints * actLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
		fHeap.AddFirst((TObject *) actLayout);
		fAction = new TGTextButton(this, Action->GetName(), Action->GetIndex(), ActionGC->GC(), ActionGC->Font());			fHeap.AddFirst((TObject *) fAction);
		fAction->ChangeBackground(ActionGC->BG());
		this->AddFrame(fAction, actLayout);			
		fAction->Associate((const TGWindow *) Action->GetAssignedObject());
		bSize += fAction->GetDefaultWidth();
	}

	if (UpDownBtnGC) {
		TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
		fHeap.AddFirst((TObject *) btnLayout);
		if (BeginEndBtns) {
			fEnd = new TGPictureButton(this, fClient->GetPicture("arrow_rightright.xpm"), kGMrbEntryButtonEnd, UpDownBtnGC->GC());
			fHeap.AddFirst((TObject *) fEnd);
			fEnd->ChangeBackground(UpDownBtnGC->BG());
			fEnd->SetToolTipText("->End", 500);
			bSize += fEnd->GetDefaultWidth();
			this->AddFrame(fEnd, btnLayout);
		}
		fUp = new TGPictureButton(this, fClient->GetPicture("arrow_right.xpm"), kGMrbEntryButtonUp, UpDownBtnGC->GC());
		fHeap.AddFirst((TObject *) fUp);
		fUp->ChangeBackground(UpDownBtnGC->BG());
		fUp->SetToolTipText("Increment", 500);
		bSize += fUp->GetDefaultWidth();
		this->AddFrame(fUp, btnLayout);
		fUp->Associate(this);
		fDown = new TGPictureButton(this, fClient->GetPicture("arrow_left.xpm"), kGMrbEntryButtonDown, UpDownBtnGC->GC());
		fHeap.AddFirst((TObject *) fDown);
		fDown->ChangeBackground(UpDownBtnGC->BG());
		fDown->SetToolTipText("Decrement", 500);
		bSize += fDown->GetDefaultWidth();
		this->AddFrame(fDown, btnLayout);
		fDown->Associate(this);
		if (BeginEndBtns) {
			fBegin = new TGPictureButton(this, fClient->GetPicture("arrow_leftleft.xpm"), kGMrbEntryButtonBegin, UpDownBtnGC->GC());
			fHeap.AddFirst((TObject *) fBegin);
			fBegin->ChangeBackground(UpDownBtnGC->BG());
			fBegin->SetToolTipText("->Start", 500);
			bSize += fBegin->GetDefaultWidth();
			this->AddFrame(fBegin, btnLayout);
		}
		fType = TGMrbLabelEntry::kGMrbEntryTypeCharInt;
	}

	fEntry = new TGMrbTextEntry(this, new TGTextBuffer(BufferSize), EntryId,
											EntryGC->GC(), EntryGC->Font(), EntryOptions, EntryGC->BG());
	fHeap.AddFirst((TObject *) fEntry);
	this->AddFrame(fEntry, EntryGC->LH());
	fEntry->Resize(EntryWidth - bSize, Height);
	fEntry->Associate(this);
}

Bool_t TGMrbLabelEntry::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::ProcessMessage
// Purpose:        Message handler for up/down buttons
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages reveived from up/down buttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString s;
	TString prefix;
	Int_t intVal;
	Double_t dblVal;

	switch (GET_MSG(MsgId)) {
		case kC_COMMAND:

			switch (GET_SUBMSG(MsgId)) {

				case kCM_BUTTON:
					switch (Param1) {
						case TGMrbLabelEntry::kGMrbEntryButtonUp:
							s = this->GetText();
							if (fType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
								s.ToInteger(intVal, fBase);
								intVal += (Int_t) fIncrement;
								if (!this->CheckRange((Double_t) intVal)) break;
								s.FromInteger(intVal, fWidth, fBase, kTRUE);
								this->SetText(s.Data());
							} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeCharInt) {
								s.SplitOffInteger(prefix, intVal, fBase);
								intVal += (Int_t) fIncrement;
								if (!this->CheckRange((Double_t) intVal)) break;
								s = prefix; s.AppendInteger(intVal, fWidth, fBase, kTRUE);
								this->SetText(s.Data());
							} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
								s.ToDouble(dblVal);
								dblVal += fIncrement;
								if (!this->CheckRange(dblVal)) break;
								s.FromDouble(dblVal, fWidth, fPrecision);
								this->SetText(s.Data());
							}
							fEntry->SendSignal();
							break;
						case TGMrbLabelEntry::kGMrbEntryButtonDown:
							s = this->GetText();
							if (fType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
								s.ToInteger(intVal, fBase);
								intVal -= (Int_t) fIncrement;
								if (!this->CheckRange((Double_t) intVal)) break;
								s.FromInteger(intVal, fWidth, fBase, kTRUE);
								this->SetText(s.Data());
							} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeCharInt) {
								s.SplitOffInteger(prefix, intVal, fBase);
								intVal -= (Int_t) fIncrement;
								if (!this->CheckRange((Double_t) intVal)) break;
								s = prefix; s.AppendInteger(intVal, fWidth, fBase, kTRUE);
								this->SetText(s.Data());
							} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
								s.ToDouble(dblVal);
								dblVal -= fIncrement;
								if (!this->CheckRange(dblVal)) break;
								s.FromDouble(dblVal, fWidth, fPrecision);
								this->SetText(s.Data());
							}
							fEntry->SendSignal();
							break;
						case TGMrbLabelEntry::kGMrbEntryButtonBegin:
							s = this->GetText();
							if (fType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
								Int_t dmy;
								s.ToInteger(dmy, fBase);
								intVal = (Int_t) fLowerLimit;
								s.FromInteger(intVal, fWidth, fBase, kTRUE);
								this->SetText(s.Data());
							} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeCharInt) {
								Int_t dmy;
								s.SplitOffInteger(prefix, dmy, fBase);
								intVal = (Int_t) fLowerLimit;
								s = prefix; s.AppendInteger(intVal, fWidth, fBase, kTRUE);
								this->SetText(s.Data());
							} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
								Double_t dmy;
								s.ToDouble(dmy);
								dblVal = fLowerLimit;
								s.FromDouble(dblVal, fWidth, fPrecision);
								this->SetText(s.Data());
							}
							fEntry->SendSignal();
							break;
						case TGMrbLabelEntry::kGMrbEntryButtonEnd:
							s = this->GetText();
							if (fType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
								Int_t dmy;
								s.ToInteger(dmy, fBase);
								intVal = (Int_t) fUpperLimit;
								s.FromInteger(intVal, fWidth, fBase, kTRUE);
								this->SetText(s.Data());
							} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeCharInt) {
								Int_t dmy;
								s.SplitOffInteger(prefix, dmy, fBase);
								intVal = (Int_t) fUpperLimit;
								s = prefix; s.AppendInteger(intVal, fWidth, fBase, kTRUE);
								this->SetText(s.Data());
							} else if (fType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
								Double_t dmy;
								s.ToDouble(dmy);
								dblVal = fUpperLimit;
								s.FromDouble(dblVal, fWidth, fPrecision);
								this->SetText(s.Data());
							}
							fEntry->SendSignal();
							break;
					}
			}
			break;

		case kC_TEXTENTRY:

			switch (GET_SUBMSG(MsgId)) {

				case kTE_ENTER:
					s = this->GetText();
					break;

				case kTE_TAB:
					if (fFocusList) fFocusList->FocusForward(fEntry);
					break;
			}
			break;
	}
	return(kTRUE);
}

void TGMrbLabelEntry::SetType(EGMrbEntryType EntryType, Int_t Width, Int_t BaseOrPrec, Bool_t PadZero) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetType
// Purpose:        Define entry type
// Arguments:      EGMrbEntryType EntryType   -- type
//                 Int_t Width                -- number width
//                 Int_t BaseOrPrec           -- numerical base (int) or precision (double)
//                 Bool_t PadZero             -- pad with 0 if kTRUE
// Results:        --
// Exceptions:     
// Description:    Defines entry type.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fType = EntryType;
	fWidth = Width;
	fPadZero = PadZero;
	if (EntryType == TGMrbLabelEntry::kGMrbEntryTypeInt) {
		if (BaseOrPrec == -1) {
			fBase = TMrbString::kDefaultBase;
		} else {
			fBase = BaseOrPrec;
		}
	} else if (EntryType == TGMrbLabelEntry::kGMrbEntryTypeDouble) {
		if (BaseOrPrec == -1) {
			fPrecision = TMrbString::kDefaultPrecision;
		} else {
			fPrecision = BaseOrPrec;
		}
	}
}

void TGMrbLabelEntry::SetText(const Char_t * Text) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetText
// Purpose:        Write text entry and create tooltip
// Arguments:      Char_t * Text    -- text to be written
// Results:        --
// Exceptions:     
// Description:    Writes entry text and updates tooltip.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fEntry->SetText(Text);
	this->CreateToolTip();
}

void TGMrbLabelEntry::SetText(Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetText
// Purpose:        Write text entry and create tooltip
// Arguments:      Int_t Value    -- integer to be converted to text
// Results:        --
// Exceptions:     
// Description:    Writes entry text and updates tooltip.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString v;
	v.FromInteger(Value, fWidth, fBase, fPadZero);
	fEntry->SetText(v.Data());
	this->CreateToolTip();
}

void TGMrbLabelEntry::SetText(Double_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetText
// Purpose:        Write text entry and create tooltip
// Arguments:      Double_t Value    -- double to be converted to text
// Results:        --
// Exceptions:     
// Description:    Writes entry text and updates tooltip.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString v;
	v.FromDouble(Value, fWidth, fPrecision, fPadZero);
	fEntry->SetText(v.Data());
	this->CreateToolTip();
}

const Char_t * TGMrbLabelEntry::GetText() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::GetText
// Purpose:        Read text entry and update tooltip
// Arguments:      --
// Results:        Char_t * Text    -- text
// Exceptions:     
// Description:    Reads entry contents and updates tooltip
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->CreateToolTip();
	return(fEntry->GetText());
}

Int_t TGMrbLabelEntry::GetText2Int() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::GetText2Int
// Purpose:        Read text entry, convert to integer, update tooltip
// Arguments:      --
// Results:        Int_t Value   -- integer value
// Exceptions:     
// Description:    Reads entry contents and converts to integer
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal = 0;
	if (fType == kGMrbEntryTypeInt) {
		TMrbString intStr = fEntry->GetText();
		Int_t idx = intStr.Index("0x", 0);
		if (idx >= 0) {
			intStr = intStr(idx + 2, intStr.Length());
			intStr.ToInteger(intVal, 16);
		} else {
			intStr.ToInteger(intVal);
		}
		this->CreateToolTip();
	} else {
		if (fLabel) gMrbLog->Err() << fLabel << ": ";
		gMrbLog->Err() << "Not a INTEGER entry" << endl;
		gMrbLog->Flush(this->ClassName(), "GetText2Int");
		new TGMsgBox(fClient->GetRoot(), this, "TGMrbLabelEntry: Error", gMrbLog->GetLast()->GetText(), kMBIconStop);
	}
	return(intVal);
}

Double_t TGMrbLabelEntry::GetText2Double() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::GetText2Double
// Purpose:        Read text entry, convert to double, update tooltip
// Arguments:      --
// Results:        Double_t Value   -- double value
// Exceptions:     
// Description:    Reads entry contents and converts to double
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Double_t dblVal = 0.0;
	if (fType == kGMrbEntryTypeDouble) {
		TMrbString dblStr = fEntry->GetText();
		dblStr.ToDouble(dblVal);
		this->CreateToolTip();
	} else {
		if (fLabel) gMrbLog->Err() << fLabel << ": ";
		gMrbLog->Err() << "Not a DOUBLE entry" << endl;
		gMrbLog->Flush(this->ClassName(), "GetText2Double");
		new TGMsgBox(fClient->GetRoot(), this, "TGMrbLabelEntry: Error", gMrbLog->GetLast()->GetText(), kMBIconStop);
	}
	return(dblVal);
}

void TGMrbLabelEntry::ShowToolTip(Bool_t ShowFlag, Bool_t ShowRange) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::ShowToolTip
// Purpose:        Turn tooltips on/off
// Arguments:      Bool_t ShowFlag    -- kTRUE if tooltips have to be shown
//                 Bool_t ShowRange   -- kTRUE if range has to be shown
// Results:        --
// Exceptions:     
// Description:    Shows number as binary, octal, dec, and hex in tooltip.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fShowToolTip = ShowFlag;
	fShowRange = ShowRange;
	this->CreateToolTip();
}

void TGMrbLabelEntry::CreateToolTip() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::CreateToolTip
// Purpose:        Create tooltip showing integer value in different formats
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Shows number as binary, octal, dec, and hex in tooltip.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fShowToolTip) {
		Bool_t showRange = fShowRange;
		if (fLowerLimit >= fUpperLimit || fIncrement == 0) showRange = kFALSE;

		if (fType == kGMrbEntryTypeInt) {
			TMrbString v = fEntry->GetText();
			Int_t value;
			v.ToInteger(value);
			TMrbString ttStr = "";
			if (showRange) {
				ttStr += "[";
				ttStr.AppendInteger((Int_t) fLowerLimit, 0, 10);
				ttStr += "...";
				ttStr.AppendInteger((Int_t) fIncrement, 0, 10);
				ttStr += "...";
				ttStr.AppendInteger((Int_t) fUpperLimit, 0, 10);
				ttStr += "] ";
			}
			TMrbString m;
			if (value < 0) {
				value = -value;
				m = "-";
			} else {
				m = "";
			}
			ttStr += m;
			ttStr.AppendInteger(value, 0, 2);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 8);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 10);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 16);
			fEntry->SetToolTipText(ttStr.Data());
		} else if (fType == kGMrbEntryTypeDouble) {
			TMrbString v = fEntry->GetText();
			Double_t value;
			v.ToDouble(value);
			TMrbString ttStr = "";
			if (showRange) {
				ttStr += "[";
				ttStr.AppendDouble(fLowerLimit);
				ttStr += "...";
				ttStr.AppendDouble(fIncrement);
				ttStr += "...";
				ttStr.AppendDouble(fUpperLimit);
				ttStr += "] ";
			}
			ttStr.AppendDouble(value);
			fEntry->SetToolTipText(ttStr.Data());
		}
	}
}

Bool_t TGMrbLabelEntry::SetRange(Double_t LowerLimit, Double_t UpperLimit) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::SetRange
// Purpose:        Define a range for numerical input
// Arguments:      Double_t LowerLimit   -- lower limit
//                 Double_t UpperLimit   -- upper limit 
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines a numerical range.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (LowerLimit == 0 && UpperLimit == 0) {
		fLowerLimit = 0;
		fUpperLimit = 0;
		return(kTRUE);
	}
	if (LowerLimit > UpperLimit) {
		gMrbLog->Err() << "Illegal range - [" << LowerLimit << "," << UpperLimit << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "SetRange");
		return(kFALSE);
	}
	fLowerLimit = LowerLimit;
	fUpperLimit = UpperLimit;
	return(kTRUE);
}

Bool_t TGMrbLabelEntry::WithinRange() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::WithinRange
// Purpose:        Check range
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks if entry text is a numeric value within range.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t intVal;
	Double_t dblVal;
	TMrbString numStr;
	TString prefix;

	if (!this->RangeToBeChecked()) return(kTRUE);

	switch (fType) {
		case TGMrbLabelEntry::kGMrbEntryTypeInt:		numStr = fEntry->GetText();
														if (!numStr.ToInteger(intVal)) return(kFALSE);
														return(intVal >= fLowerLimit && intVal <= fUpperLimit);
		case TGMrbLabelEntry::kGMrbEntryTypeDouble:		numStr = fEntry->GetText();
														if (!numStr.ToDouble(dblVal)) return(kFALSE);
														return(dblVal >= fLowerLimit && dblVal <= fUpperLimit);
		case TGMrbLabelEntry::kGMrbEntryTypeCharInt:	numStr = fEntry->GetText();
														numStr.SplitOffInteger(prefix, intVal, fBase);
														return(intVal >= fLowerLimit && intVal <= fUpperLimit);
		case TGMrbLabelEntry::kGMrbEntryTypeCharDouble:	numStr = fEntry->GetText();
														numStr.SplitOffDouble(prefix, dblVal);
														return(dblVal >= fLowerLimit && dblVal <= fUpperLimit);
	}
	return(kTRUE);
}

Bool_t TGMrbLabelEntry::CheckRange(Double_t Value, Bool_t Verbose, Bool_t Popup) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::CheckRange
// Purpose:        Check range
// Arguments:      Double_t Value    -- value to be tested
//                 Bool_t Verbose    -- ouput message to stderr
//                 Bool_t Popup      -- pop up TGMsgBox
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks if value is within range.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (!this->RangeToBeChecked()) return(kTRUE);
	Bool_t withinRange = (Value >= fLowerLimit && Value <= fUpperLimit);
	if (withinRange) return(kTRUE);
	if (Verbose || Popup) {
		if (fLabel) gMrbLog->Err() << fLabel << ": ";
		gMrbLog->Err() << "Value out of range - " << Value << " should be in [" << fLowerLimit << "," << fUpperLimit << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckRange");
		if (Popup) new TGMsgBox(fClient->GetRoot(), this, "TGMrbLabelEntry: Error", gMrbLog->GetLast()->GetText(), kMBIconStop);
	}
	return(kFALSE);
}

Bool_t TGMrbLabelEntry::RangeToBeChecked() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::RangeToBeChecked
// Purpose:        Test if range to be checked
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks if range has to be checked.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fType == TGMrbLabelEntry::kGMrbEntryTypeChar) return(kFALSE);
	if (fLowerLimit == 0 && fUpperLimit == 0) return(kFALSE);
	return(kTRUE);
}

void TGMrbLabelEntry::UpDownButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::UpDownButtonEnable
// Purpose:        Enable/disable up/down buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables up/down buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fUp && fDown) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fUp->SetState(btnState);
		fDown->SetState(btnState);
	}
}

void TGMrbLabelEntry::ActionButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::ActionButtonEnable
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

Bool_t TGMrbTextEntry::HandleButton(Event_t * Event) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry::HandleButton
// Purpose:        Handle button events
// Arguments:      Event_t * Event   -- button event
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Adds private actions to button handling.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsEnabled()) return(kTRUE);
	if (Event->fType == kButtonPress && Event->fCode == kButton3) {
		cerr	<< setred
				<< this->ClassName() << "::HandleButton(): Mouse button 333 not yet implemented"
				<< setblack << endl;
	} else {
		return(TGTextEntry::HandleButton(Event));
	}
	return(kTRUE);
}

