//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbLabelCombo.cxx
// Purpose:        MARaBOU graphic utilities: a labelled combo box
// Description:    Implements class methods to handle a labelled combo box
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbLabelCombo.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TGMrbLabelCombo.h"

ClassImp(TGMrbLabelCombo)

TGMrbLabelCombo::TGMrbLabelCombo(const TGWindow * Parent,
												const Char_t * Label,
												TMrbLofNamedX * Entries,
												Int_t ComboId, Int_t Selected,
												Int_t Width, Int_t Height, Int_t ComboWidth,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * ComboGC,
												TGMrbLayout * UpDownBtnGC,
												Bool_t BeginEndBtns,
												UInt_t FrameOptions,
												UInt_t ComboOptions) :
										TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo
// Purpose:        Define a label together with a combo box
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 TMrbLofKeys * Entries       -- combo box entries
//                 Int_t ComboId               -- id to be used in ProcessMessage
//                 Int_t Selected              -- id of item to be displayed at startup
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t ComboWidth            -- combo width
//                 TGMrbLayout * FrameGC       -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC       -- ... (label)
//                 TGMrbLayout * ComboGC       -- ... (combo box)
//                 TGMrbLayout * UpDownBtnGC   -- ... (up/down btns) - adds buttons "<" and ">"
//                 Bool_t BeginEndBtns         -- if kTRUE add two additional buttons "<<" and ">>"
//                 UInt_t FrameOptions         -- frame options
//                 UInt_t ComboOptions         -- options to configure the combo box
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGLabel * label;
	Int_t bSize;

	fClientWindow = NULL;

	LabelGC = this->SetupGC(LabelGC, FrameOptions);
	ComboGC = this->SetupGC(ComboGC, FrameOptions);

	fUp = NULL;
	fDown = NULL;
	fBegin = NULL;
	fEnd = NULL;

	if (Label != NULL) {
		label = new TGLabel(this, new TGString(Label), LabelGC->GC(), LabelGC->Font(), kChildFrame, LabelGC->BG());
		fHeap.AddFirst((TObject *) label);
		this->AddFrame(label, LabelGC->LH());
		label->SetTextJustify(kTextLeft);
	}

	if (UpDownBtnGC) {
		TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 1, 0);
		fHeap.AddFirst((TObject *) btnLayout);
		if (BeginEndBtns) {
			fEnd = new TGPictureButton(this, fClient->GetPicture("arrow_rightright.xpm"), kGMrbComboButtonEnd, UpDownBtnGC->GC());
			fHeap.AddFirst((TObject *) fEnd);
			fEnd->ChangeBackground(UpDownBtnGC->BG());
			fEnd->SetToolTipText("->End", 500);
			bSize = fEnd->GetDefaultWidth();
			this->AddFrame(fEnd, btnLayout);
			fEnd->Associate(this);
		}
		fUp = new TGPictureButton(this, fClient->GetPicture("arrow_right.xpm"), kGMrbComboButtonUp, UpDownBtnGC->GC());
		fHeap.AddFirst((TObject *) fUp);
		fUp->ChangeBackground(UpDownBtnGC->BG());
		fUp->SetToolTipText("Increment", 500);
		bSize += fUp->GetDefaultWidth();
		this->AddFrame(fUp, btnLayout);
		fUp->Associate(this);
		fDown = new TGPictureButton(this, fClient->GetPicture("arrow_left.xpm"), kGMrbComboButtonDown, UpDownBtnGC->GC());
		fHeap.AddFirst((TObject *) fDown);
		fDown->ChangeBackground(UpDownBtnGC->BG());
		fDown->SetToolTipText("Decrement", 500);
		bSize += fDown->GetDefaultWidth();
		this->AddFrame(fDown, btnLayout);
		fDown->Associate(this);
		if (BeginEndBtns) {
			fBegin = new TGPictureButton(this, fClient->GetPicture("arrow_leftleft.xpm"), kGMrbComboButtonBegin, UpDownBtnGC->GC());
			fHeap.AddFirst((TObject *) fBegin);
			fBegin->ChangeBackground(UpDownBtnGC->BG());
			fBegin->SetToolTipText("->Start", 500);
			bSize += fBegin->GetDefaultWidth();
			this->AddFrame(fBegin, btnLayout);
			fBegin->Associate(this);
		}
	}

	fCombo = new TGComboBox(this, ComboId, ComboOptions, ComboGC->BG());
	fHeap.AddFirst((TObject *) fCombo);
	this->AddFrame(fCombo, ComboGC->LH());
	fEntries.Delete();
	this->AddEntries(Entries);
	fCombo->Resize(ComboWidth, Height);
	fCombo->Associate(this);
}

Bool_t TGMrbLabelCombo::AddEntries(TMrbLofNamedX * Entries) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo::AddEntries
// Purpose:        Add entries to the combo box
// Arguments:      TMrbLofNamedX * Entries   -- list of entries to be added
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Fills the combo box.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * entry;
	entry = (TMrbNamedX *) Entries->First();
	while (entry) {
		fEntries.AddNamedX(entry);
		fCombo->AddEntry(entry->GetName(), entry->GetIndex());
		entry = (TMrbNamedX *) Entries->After(entry);
	}
	return(kTRUE);
}

Bool_t TGMrbLabelCombo::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo::ProcessMessage
// Purpose:        Message handler for combo boxes
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle combo box messages
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t idx;
	TMrbNamedX * nx;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_COMBOBOX:
					fText = ((TGTextLBEntry *) fCombo->GetSelectedEntry())->GetText()->GetString();
					break;
				case kCM_BUTTON:
					switch (Param1) {
						case TGMrbLabelCombo::kGMrbComboButtonDown:
							idx = fCombo->GetSelected();
							nx = fEntries.FindByIndex(idx);
							if (nx == fEntries.First()) {
								nx = (TMrbNamedX *) fEntries.Last();
							} else {
								nx = (TMrbNamedX *) fEntries.At(fEntries.IndexOf(nx) - 1);
							}
							fCombo->Select(nx->GetIndex());
							if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							break;
						case TGMrbLabelCombo::kGMrbComboButtonUp:
							idx = fCombo->GetSelected();
							nx = fEntries.FindByIndex(idx);
							if (nx == fEntries.Last()) {
								nx = (TMrbNamedX *) fEntries.First();
							} else {
								nx = (TMrbNamedX *) fEntries.At(fEntries.IndexOf(nx) + 1);
							}
							fCombo->Select(nx->GetIndex());
							if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							break;								
						case TGMrbLabelCombo::kGMrbComboButtonBegin:
							nx = (TMrbNamedX *) fEntries.First();
							fCombo->Select(nx->GetIndex());
							if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							break;
						case TGMrbLabelCombo::kGMrbComboButtonEnd:
							nx = (TMrbNamedX *) fEntries.Last();
							fCombo->Select(nx->GetIndex());
							if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							break;
					}
					break;
			}
			break;
	}
	return(kTRUE);
}

void TGMrbLabelCombo::UpDownButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo::UpDownButtonEnable
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

