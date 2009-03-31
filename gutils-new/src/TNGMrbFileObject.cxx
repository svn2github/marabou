//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbFileObject.cxx
// Purpose:        MARaBOU graphic utilities: a file entry and a combo box for ROOT objects
// Description:    Implements class methods to handle a file entry together with a combo box
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbFileObject.cxx,v 1.3 2009-03-31 14:34:32 Rudolf.Lutter Exp $       
// Date:           
// Layout:         A labelled entry widget with a file button
//                 together with a combo widget showing objects of selected (root) file
//Begin_Html
/*
<img src=gutils/TNGMrbFileObjectCombo.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TKey.h"
#include "TNamed.h"
#include "TFile.h"
#include "TH1.h"
#include "TGLabel.h"
#include "TGToolTip.h"
#include "TGMsgBox.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"
#include "TMrbString.h"
#include "TNGMrbFileObject.h"

// file types
static Char_t * rootFileTypes[] =	{	"ROOT files", 	"*.root",
										"All files",	"*",
                            			NULL,			NULL
									};

ClassImp(TNGMrbFileObjectCombo)

extern TMrbLogger * gMrbLog;		// access to message logging

TNGMrbFileObjectCombo::TNGMrbFileObjectCombo(	const TGWindow * Parent,				// ctor
											const Char_t * Label,
											TNGMrbProfile * Profile,
											Int_t EntryId, Int_t ComboId,
											Int_t Width, Int_t Height,
											Int_t EntryWidth, Int_t ComboWidth,
											UInt_t ComboOptions) :
								TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions() | kHorizontalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileObjectCombo
// Purpose:        Define a file entry together with a combo box
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 TNGMrbProfile * Profile      -- graphics profile
//                 Int_t EntryId               -- id to be used in ProcessMessage
//                 Int_t ComboId               -- ...
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- width of the entry field
//                 Int_t ComboWidth            -- width of the combo box
//                 UInt_t ComboOptions         -- options
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	TNGMrbGContext * labelGC = Profile->GetGC(TNGMrbGContext::kGMrbGCLabel);
	TNGMrbGContext * entryGC = Profile->GetGC(TNGMrbGContext::kGMrbGCTextEntry);
	TNGMrbGContext * buttonGC = Profile->GetGC(TNGMrbGContext::kGMrbGCButton);
	TNGMrbGContext * comboGC = Profile->GetGC(TNGMrbGContext::kGMrbGCLBEntry);

	fClientWindow = NULL;

	if (Label != NULL) {
		fLabel = new TGLabel(this, new TGString(Label));
		fLabel->SetTextFont(labelGC->Font());
		fLabel->SetForegroundColor(labelGC->FG());
		fLabel->SetBackgroundColor(labelGC->BG());
		fLabel->ChangeOptions(labelGC->GetOptions());
		TO_HEAP(fLabel);
		this->AddFrame(fLabel);
		fLabel->SetTextJustify(kTextLeft);
	} else {
		fLabel = NULL;
	}

	fFileInfo.fFileTypes = (const Char_t **) rootFileTypes;
	fFileInfo.fIniDir = StrDup(gSystem->WorkingDirectory());

	fBrowse = new TGPictureButton(this, fClient->GetPicture("ofolder_t.xpm"), kGMrbComboButtonBrowse);
	TO_HEAP(fBrowse);
	this->AddFrame(fBrowse);
	fBrowse->ChangeBackground(buttonGC->BG());
	fBrowse->SetToolTipText("Browse files", 500);
	fBrowse->Associate(this);

	fEntry = new TGTextEntry(this, new TGTextBuffer(), EntryId);
	TO_HEAP(fEntry);
	this->AddFrame(fEntry);
	
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
		if (ComboOptions & kGMrbComboHasUpDownButtons) {
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
	((TGListBox *) fCombo->GetListBox())->SetHeight(Height * 10);
	
	fEntryWidth = EntryWidth;
	fComboWidth = ComboWidth;
	if (fComboWidth <= 0) fComboWidth = fEntryWidth;

	this->SetFont(entryGC->Font());
	this->SetEntryBackground(entryGC->BG());
	this->SetComboBackground(comboGC->BG());
	this->Resize(Width, Height);
	this->SetLayoutManager(new TNGMrbFileObjectComboLayout(this));
}

Bool_t TNGMrbFileObjectCombo::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileObjectCombo::ProcessMessage
// Purpose:        Message handler for browse button
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle message received from browse button
// Keywords:       
//////////////////////////////////////////////////////////////////////////////
					
	Int_t idx;
	TMrbNamedX * nx;

	if (GET_MSG(MsgId) == kC_COMMAND) {
		switch (GET_SUBMSG(MsgId)) {
				case kCM_COMBOBOX:
					fText = ((TGTextLBEntry *) fCombo->GetSelectedEntry())->GetText()->GetString();
					break;
				case kCM_BUTTON:
					switch (Param1) {
						case TNGMrbFileObjectCombo::kGMrbComboButtonDown:
							idx = fCombo->GetSelected();
							nx = fLofEntries.FindByIndex(idx);
							if (nx) {
								if (nx == fLofEntries.First()) {
									nx = (TMrbNamedX *) fLofEntries.Last();
								} else {
									nx = (TMrbNamedX *) fLofEntries.At(fLofEntries.IndexOf(nx) - 1);
								}
								fCombo->Select(nx->GetIndex());
								if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							}
							break;
						case TNGMrbFileObjectCombo::kGMrbComboButtonUp:
							idx = fCombo->GetSelected();
							nx = fLofEntries.FindByIndex(idx);
							if (nx) {
								if (nx == fLofEntries.Last()) {
									nx = (TMrbNamedX *) fLofEntries.First();
								} else {
									nx = (TMrbNamedX *) fLofEntries.At(fLofEntries.IndexOf(nx) + 1);
								}
								fCombo->Select(nx->GetIndex());
								if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							}
							break;								
						case TNGMrbFileObjectCombo::kGMrbComboButtonBegin:
							nx = (TMrbNamedX *) fLofEntries.First();
							fCombo->Select(nx->GetIndex());
							if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							break;
						case TNGMrbFileObjectCombo::kGMrbComboButtonEnd:
							nx = (TMrbNamedX *) fLofEntries.Last();
							fCombo->Select(nx->GetIndex());
							if (fClientWindow) this->SendMessage(fClientWindow, MK_MSG(kC_COMMAND, kCM_COMBOBOX), 0, nx->GetIndex());
							break;
						case TNGMrbFileObjectCombo::kGMrbComboButtonBrowse:
							new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fFileInfo);
							if (fFileInfo.fFilename != NULL && *fFileInfo.fFilename != '\0') {
								this->OpenFile(fFileInfo.fFilename);
							}
							break;
					}
			}
	} else if (GET_MSG(MsgId) == kC_TEXTENTRY) {
		switch (GET_SUBMSG(MsgId)) {
			case kTE_ENTER:
			case kTE_TAB:
				this->OpenFile(fEntry->GetText());
				break;
		}
	}


	return(kTRUE);
}

Bool_t TNGMrbFileObjectCombo::OpenFile(const Char_t * File) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileObjectCombo::OpenFile
// Purpose:        Open ROOT file
// Arguments:      Char_t * File   -- root file to be read
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Opens root file and reads objects.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TFile * rootFile = new TFile(File);
	if (!rootFile->IsOpen()) {
		TString msg = "Not a ROOT file - ";
		msg += File;
		new TGMsgBox(fClient->GetRoot(), this, "TNGMrbFileObjectCombo: Error", msg.Data(), kMBIconStop);
		return(kFALSE);
	}
	this->SetFileEntry(File);
	TList * fileKeys = rootFile->GetListOfKeys();
	fileKeys->Sort();
	TIterator * iter = fileKeys->MakeIterator();
	TKey * key;
	Int_t idx = 0;
	Int_t nofEntries = fCombo->GetListBox()->GetNumberOfEntries();
	fCombo->RemoveEntries(0, nofEntries - 1);
	while (key = (TKey *) iter->Next()) {
		TMrbString keyName = "(";
		keyName += key->GetClassName();
		keyName += " *) ";
		keyName += key->GetName();
		TObject * obj = key->ReadObj();
		if (obj) {
			if (obj->InheritsFrom("TH1")) {
				TH1 * h = (TH1 *) obj;
				keyName += " [";
				keyName += h->GetNbinsX();
				if (h->GetDimension() == 2) {
					keyName += ",";
					keyName += h->GetNbinsY();
				}
				keyName += "]";
			}
			fCombo->AddEntry(keyName.Data(), idx);
			fLofEntries.AddNamedX(idx, keyName.Data());
			idx++;
		}
	}
	fCombo->Select(0);
	return(kTRUE);
}

void TNGMrbFileObjectCombo::SetFileEntry(const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileObjectCombo::SetFileEntry
// Purpose:        Fill text entry with file name
// Arguments:      Char_t * FileName     -- file name to be filled in
// Results:        
// Exceptions:     
// Description:    Inserts file name with respect to path spec in
//                 fFileInfo.fIniDir.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbSystem ux;
	TString fileName;

	fileName = FileName;
	fEntry->SetText(ux.GetRelPath(fileName, fFileInfo.fIniDir));
}

const Char_t * TNGMrbFileObjectCombo::GetFileEntry(TString & FileName, Bool_t FullPath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileObjectCombo::GetFileEntry
// Purpose:        Return file name
// Arguments:      Bool_t FullPath  -- return full path
// Results:        
// Exceptions:     
// Description:    Returns entry text prepended by fFileInfo.fIniDir if needed.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	FileName = fEntry->GetText();
	FileName = FileName.Strip(TString::kBoth);
	if (FullPath && FileName(0) != '/') {
		FileName.Prepend("/");
		FileName.Prepend(fFileInfo.fIniDir);
	}
	return(FileName.Data());
}

const Char_t * TNGMrbFileObjectCombo::GetSelection(TString & SelItem, Bool_t FullPath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileObjectCombo::GetSelection
// Purpose:        Return selected file/object
// Arguments:      TString SelItem         -- where to store selection data
//                 Bool_t FullPath         -- prepend full file path if kTRUE
// Results:        Char_t * SelItem        -- selected item
// Exceptions:     
// Description:    Returns selected item "file:object"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString itemString = ((TGTextLBEntry *) fCombo->GetSelectedEntry())->GetText()->GetString();
	this->GetFileEntry(SelItem, FullPath);
	SelItem += ":";
	Int_t idx = itemString.Index(")", 0) + 1;
	if (idx < 0) idx = 0;
	itemString = itemString(idx, itemString.Length());
	idx = itemString.Index("[", 0) - 1;
	if (idx < 0) idx = itemString.Length();
	itemString.Resize(idx);
	itemString = itemString.Strip(TString::kBoth);
	SelItem += itemString; 
	return(SelItem.Data());
}

void TNGMrbFileObjectComboLayout::Layout() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileObjectComboLayout::Layout
// Purpose:        Layout manager
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Placement of widget elements
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fWidget) {
		UInt_t boxWidth = fWidget->GetWidth();			// width of labelled-entry box (label + entry + buttons)
		UInt_t boxHeight = fWidget->GetHeight() / 2 - 2 * TNGMrbFileObjectCombo::kGMrbComboPadH;	// height
		UInt_t entryWidth = fWidget->GetEntryWidth(); 	// width of entry field (entry + buttons)
		UInt_t comboWidth = fWidget->GetComboWidth(); 	// width of combo field (entry + buttons)

		UInt_t labelWidth = fWidget->HasLabel() ? fWidget->GetLabel()->GetWidth() : 0;	// label width

		if (entryWidth == 0) entryWidth = boxWidth - labelWidth;

		if (labelWidth + entryWidth > boxWidth) {
			boxWidth = labelWidth + entryWidth;
			fWidget->Resize(boxWidth, fWidget->GetHeight());
		}
			
		Int_t x = boxWidth;
		Int_t y = TNGMrbFileObjectCombo::kGMrbComboPadH;
		Int_t delta = 0;

		TGPictureButton * browse = fWidget->GetBrowseButton();
		UInt_t w = browse->GetWidth();
		x -= w; delta += w;
		browse->MoveResize(x, y, w, boxHeight);

		x = boxWidth - entryWidth;		// entry starts 'entryWidth' from right end
		entryWidth -= delta;			// compensate for buttons

		fWidget->GetEntry()->MoveResize(x, y, entryWidth, boxHeight);

		if (fWidget->HasLabel()) {
			fWidget->GetLabel()->MoveResize(TNGMrbFileObjectCombo::kGMrbComboPadW, y, x - TNGMrbFileObjectCombo::kGMrbComboPadW, boxHeight);
		}

		x = boxWidth;
		y += boxHeight + 2 * TNGMrbFileObjectCombo::kGMrbComboPadH;
		delta = 0;

		UInt_t btnWidth = 2 * boxHeight / 3;				// button width

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
	}
}
