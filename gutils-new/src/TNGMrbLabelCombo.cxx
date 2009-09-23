//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbLabelCombo.cxx
// Purpose:        MARaBOU graphic utilities: a labelled combo box
// Description:    Implements class methods to handle a labelled combo box
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbLabelCombo.cxx,v 1.4 2009-09-23 10:42:51 Marabou Exp $       
// Date:           
// Layout:         A labelled combo widget with up/down/begin/end buttons
//Begin_Html
/*
<img src=gutils/TNGMrbLabelCombo.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TMrbLogger.h"
#include "TNGMrbLabelCombo.h"

ClassImp(TNGMrbLabelCombo)

extern TMrbLogger * gMrbLog;		// access to message logging

TNGMrbLabelCombo::TNGMrbLabelCombo(	const TGWindow * Parent,
									const Char_t * Label,
									TMrbLofNamedX * Entries,
									TNGMrbProfile * Profile,
									Int_t ComboId, Int_t Selected,
									Int_t Width, Int_t Height, Int_t ComboWidth,
									UInt_t ComboOptions) :
							TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions() | kHorizontalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo
// Purpose:        Define a label together with a combo box
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 TMrbLofKeys * Entries       -- combo box entries
//                 TNGMrbProfile * Profile     -- graphics profile
//                 Int_t ComboId               -- id to be used in ProcessMessage
//                 Int_t Selected              -- id of item to be displayed at startup
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t ComboWidth            -- combo width
//                 UInt_t ComboOptions         -- options to configure the combo box
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	

// bei Label wird Combo verwendet, da das Label praktisch die ganze Flaeche des Combos ausmacht, somit ist es eg die Hintergrundfarbe des Combos
	TNGMrbGContext * labelGC = Profile->GetGC(TNGMrbGContext::kGMrbGCCombo);
	TNGMrbGContext * buttonGC = Profile->GetGC(TNGMrbGContext::kGMrbGCButton);
	TNGMrbGContext * comboGC = Profile->GetGC(TNGMrbGContext::kGMrbGCLBEntry);

	fClientWindow = NULL;

	fComboWidth = ComboWidth;
	fLabel = NULL;
	fButtonUp = NULL;
	fButtonDown = NULL;
	fButtonBegin = NULL;
	fButtonEnd = NULL;

	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCCombo)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCCombo)->BG());


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

	if (ComboOptions & kGMrbComboHasUpDownButtons) {
		fButtonUp = new TGPictureButton(this, fClient->GetPicture("arrow_up.xpm"), kGMrbComboButtonUp);
		TO_HEAP(fButtonUp);
		fButtonUp->ChangeBackground(buttonGC->BG());
		fButtonUp->SetToolTipText("StepUp", 500);
		this->AddFrame(fButtonUp);

		fButtonUp->Associate(this);
		fButtonDown = new TGPictureButton(this, fClient->GetPicture("arrow_down.xpm"), kGMrbComboButtonDown);
		TO_HEAP(fButtonDown);
		fButtonDown->ChangeBackground(buttonGC->BG());
		fButtonDown->SetToolTipText("StepDown", 500);
		this->AddFrame(fButtonDown);

		fButtonDown->Associate(this);
		if (ComboOptions & kGMrbComboHasBeginEndButtons) {
			fButtonBegin = new TGPictureButton(this, fClient->GetPicture("arrow_leftleft.xpm"), kGMrbComboButtonBegin);
			TO_HEAP(fButtonBegin);
			fButtonBegin->ChangeBackground(buttonGC->BG());
			fButtonBegin->SetToolTipText("ToBegin", 500);
			this->AddFrame(fButtonBegin);

			fButtonBegin->Associate(this);
			fButtonEnd = new TGPictureButton(this, fClient->GetPicture("arrow_rightright.xpm"), kGMrbComboButtonEnd);
			TO_HEAP(fButtonEnd);
			fButtonEnd->ChangeBackground(buttonGC->BG());
			fButtonEnd->SetToolTipText("ToEnd", 500);
			this->AddFrame(fButtonEnd);

			fButtonEnd->Associate(this);
		}
	}

	fCombo = new TGComboBox(this, ComboId);
	TO_HEAP(fCombo);
	this->AddFrame(fCombo);


	fLofEntries.Delete();
	if (Entries && Entries->GetEntriesFast() > 0) {
		Bool_t byName;
		if (Entries->IsSorted(byName)) fLofEntries.Sort(byName);
		this->AddEntry(Entries);
		fCombo->Select(((TMrbNamedX *) fLofEntries.First())->GetIndex());
	}

	fCombo->Connect("Selected(Int_t, Int_t)", this->ClassName(), this, "SelectionChanged(Int_t, Int_t)");

	this->Resize(Width, Height);
	this->SetLayoutManager(new TNGMrbLabelComboLayout(this));
}

void TNGMrbLabelCombo::AddEntry(TMrbLofNamedX * Entries) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo::AddEntry
// Purpose:        Add entries to the combo box
// Arguments:      TMrbLofNamedX * Entries   -- list of entries to be added
// Results:        
// Exceptions:     
// Description:    Fills the combo box.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fLofEntries.AddNamedX(Entries);
	fLofEntries.Sort();
	TIterator * iter = fLofEntries.MakeIterator();
	TMrbNamedX * entry;
	while (entry = (TMrbNamedX *) iter->Next()) fCombo->AddEntry(entry->GetName(), entry->GetIndex());
}

void TNGMrbLabelCombo::AddEntry(TMrbNamedX * Entry, Bool_t SelectFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo::AddEntry
// Purpose:        Add entries to the combo box
// Arguments:      TMrbNamedX * Entry   -- entry to be added
// Results:        
// Exceptions:     
// Description:    Fills the combo box.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fLofEntries.AddNamedX(Entry);
	Bool_t byName;
	if (fLofEntries.IsSorted(byName)) {
		fLofEntries.Sort(byName);
		TMrbNamedX * nx = fLofEntries.FindByIndex(Entry->GetIndex());
		fCombo->InsertEntry(nx->GetName(), nx->GetIndex(), ((TMrbNamedX *) fLofEntries.Before(nx))->GetIndex());
	} else {
		fCombo->AddEntry(Entry->GetName(), Entry->GetIndex());
	}
	if (SelectFlag)  fCombo->Select(Entry->GetIndex());
}

void TNGMrbLabelCombo::RemoveEntry(const Char_t * EntryName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo::RemoveEntry
// Purpose:        Add entries to the combo box
// Arguments:      Char_t * EntryName   -- entry to be removed
// Results:        
// Exceptions:     
// Description:    Fills the combo box.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofEntries.FindByName(EntryName);
	if (nx) fCombo->RemoveEntry(nx->GetIndex());
}

Bool_t TNGMrbLabelCombo::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo::ProcessMessage
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
						case TNGMrbLabelCombo::kGMrbComboButtonDown:
							idx = fCombo->GetSelected();
							nx = fLofEntries.FindByIndex(idx);
							if (nx) {
								if (nx == fLofEntries.First()) {
									nx = (TMrbNamedX *) fLofEntries.Last();
								} else {
									nx = (TMrbNamedX *) fLofEntries.At(fLofEntries.IndexOf(nx) - 1);
								}
								fCombo->Select(nx->GetIndex(),kTRUE);
								fCombo->GetListBox()->SetTopEntry(nx->GetIndex());
								if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							}
							break;
						case TNGMrbLabelCombo::kGMrbComboButtonUp:
							idx = fCombo->GetSelected();
							nx = fLofEntries.FindByIndex(idx);
							if (nx) {
								if (nx == fLofEntries.Last()) {
									nx = (TMrbNamedX *) fLofEntries.First();
								} else {
									nx = (TMrbNamedX *) fLofEntries.At(fLofEntries.IndexOf(nx) + 1);
								}
								fCombo->Select(nx->GetIndex(),kTRUE);
								fCombo->GetListBox()->SetTopEntry(nx->GetIndex());
								if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							}
							break;								
						case TNGMrbLabelCombo::kGMrbComboButtonBegin:
							nx = (TMrbNamedX *) fLofEntries.First();
							fCombo->Select(nx->GetIndex(),kTRUE);
							fCombo->GetListBox()->SetTopEntry(nx->GetIndex());
							if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							break;
						case TNGMrbLabelCombo::kGMrbComboButtonEnd:
							nx = (TMrbNamedX *) fLofEntries.Last();
							fCombo->Select(nx->GetIndex(),kTRUE);
							fCombo->GetListBox()->SetTopEntry(nx->GetIndex());
							if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							break;
					}
					break;
			}
			break;
	}
	return(kTRUE);
}

void TNGMrbLabelCombo::UpDownButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo::UpDownButtonEnable
// Purpose:        Enable/disable up/down buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables up/down buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (this->HasUpDownButtons()) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fButtonUp->SetState(btnState);
		fButtonDown->SetState(btnState);
	}
}

void TNGMrbLabelCombo::SetButtonBackground(Pixel_t Color) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo::SetButtonBackground
// Purpose:        Change background color for buttons
// Arguments:      Pixel_t Color     -- color
// Results:        --
// Exceptions:     
// Description:    Changes background color.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fButtonBegin) {
		fButtonBegin->SetBackgroundColor(Color);
		fButtonEnd->SetBackgroundColor(Color);
	}
}

void TNGMrbLabelCombo::BeginEndButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo::BeginEndButtonEnable
// Purpose:        Enable/disable begin/end buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables begin/end buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (this->HasBeginEndButtons()) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fButtonBegin->SetState(btnState);
		fButtonEnd->SetState(btnState);
	}
}

void TNGMrbLabelComboLayout::Layout() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelComboLayout::Layout
// Purpose:        Layout manager
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Placement of widget elements
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fWidget) {
		UInt_t boxWidth = fWidget->GetWidth();			// width of labelled-combo box (label + entry + buttons)
		UInt_t boxHeight = fWidget->GetHeight() - 2 * TNGMrbLabelCombo::kGMrbComboPadH;	// height
		UInt_t comboWidth = fWidget->GetComboWidth(); 	// width of combo field

		UInt_t labelWidth = fWidget->HasLabel() ? fWidget->GetLabel()->GetWidth() : 0;	// label width

		if (comboWidth == 0) comboWidth = boxWidth - labelWidth;

		if (labelWidth + comboWidth > boxWidth) {
			boxWidth = labelWidth + comboWidth;
			fWidget->Resize(boxWidth, fWidget->GetHeight());
		}
			
		UInt_t btnWidth = 2 * boxHeight / 3;				// button width
		Int_t x = boxWidth;
		Int_t y = TNGMrbLabelCombo::kGMrbComboPadH;
		Int_t delta = 0;

		if (fWidget->HasBeginEndButtons()) {
			x -= btnWidth; delta += btnWidth;
			fWidget->GetButtonEnd()->MoveResize(x, y, btnWidth, boxHeight/2);
			fWidget->GetButtonBegin()->MoveResize(x, y + boxHeight/2, btnWidth, boxHeight/2);
		}

		if (fWidget->HasUpDownButtons()) {
			x -= btnWidth; delta += btnWidth;
			fWidget->GetButtonUp()->MoveResize(x, y, btnWidth, boxHeight/2);
			fWidget->GetButtonDown()->MoveResize(x, y + boxHeight/2, btnWidth, boxHeight/2);
		}

		x = boxWidth - comboWidth;		// entry starts 'comboWidth' from right end
		comboWidth -= delta;			// compensate for buttons

		fWidget->GetComboBox()->MoveResize(x, y, comboWidth, boxHeight);

		if (fWidget->HasLabel()) {
			fWidget->GetLabel()->MoveResize(TNGMrbLabelCombo::kGMrbComboPadW, y, x - TNGMrbLabelCombo::kGMrbComboPadW, boxHeight);
		}
	}
}

void TNGMrbLabelCombo::SelectionChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo::SelectionChanged
// Purpose:        Signal handler
// Arguments:      Int_t FrameId    -- frame id
//                 Int_t Selection  -- selection index
// Results:        --
// Exceptions:     
// Description:    Emits signal on "selection changed" 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fCombo->GetSelectedEntry()->SetBackgroundColor(fBack);
   Long_t args[2];

   args[0] = FrameId;
   args[1] = Selection;

   this->Emit("SelectionChanged(Int_t, Int_t)", args);
}
void TNGMrbLabelCombo::SetEntryBackground(Pixel_t Back){ 
// 	fCombo->SetBackgroundColor(Back); 
// 	fCombo->GetListBox()->SetBackgroundColor(Back);
//  	fCombo->GetListBox()->GetEntry(id)->SetBackgroundColor(Back);
// 	fCombo->GetListBox()->GetEntry(id)->Update(fCombo->GetListBox()->GetEntry(id));
	fCombo->GetSelectedEntry()->SetBackgroundColor(Back);
	fBack=Back;

}