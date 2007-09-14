//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbFileObject.cxx
// Purpose:        MARaBOU graphic utilities: a file entry and a combo box for ROOT objects
// Description:    Implements class methods to handle a file entry together with a combo box
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TGMrbFileObject.cxx,v 1.19 2007-09-14 13:37:41 Rudolf.Lutter Exp $       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbFileObject.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TKey.h"
#include "TObjString.h"
#include "TNamed.h"
#include "TFile.h"
#include "TH1.h"
#include "TGLabel.h"
#include "TGToolTip.h"
#include "TGMsgBox.h"
#include "TMrbSystem.h"
#include "TMrbString.h"
#include "TGMrbFileObject.h"

// file types
static Char_t * rootFileTypes[] =	{	"ROOT files", 	"*.root",
										"All files",	"*",
                            			NULL,			NULL
									};

ClassImp(TGMrbFileObjectCombo)
ClassImp(TGMrbFileObjectListBox)

TGMrbFileObjectCombo::TGMrbFileObjectCombo(const TGWindow * Parent,
												const Char_t * Label,
												Int_t BufferSize,
												Int_t EntryId, Int_t ComboId,
												Int_t Width, Int_t Height,
												Int_t EntryWidth, Int_t ComboWidth,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * EntryGC,
												TGMrbLayout * BrowseGC,
												TGMrbLayout * ComboGC,
												UInt_t FrameOptions,
												UInt_t EntryOptions,
												UInt_t ComboOptions) :
								TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo
// Purpose:        Define a file entry together with a combo box
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 Int_t BufferSize            -- buffer size
//                 Int_t EntryId               -- id to be used in ProcessMessage
//                 Int_t ComboId               -- ...
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- width of the entry field
//                 Int_t ComboWidth            -- width of the combo box
//                 TGMrbLayout * FrameGC       -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC       -- ... (label)
//                 TGMrbLayout * EntryGC       -- ... (entry)
//                 TGMrbLayout * BrowseGC      -- ... (browse)
//                 TGMrbLayout * ComboGC       -- ... (combo box)
//                 UInt_t FrameOptions         -- frame options
//                 UInt_t EntryOptions         -- options to configure the entry
//                 UInt_t ComboOptions         -- options to configure the combo box
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGLabel * label;
	Int_t bSize;

	LabelGC = this->SetupGC(LabelGC, FrameOptions);
	EntryGC = this->SetupGC(EntryGC, FrameOptions);
	BrowseGC = this->SetupGC(BrowseGC, FrameOptions);
	ComboGC = this->SetupGC(ComboGC, FrameOptions);

	if (Label != NULL) {
		label = new TGLabel(this, new TGString(Label), LabelGC->GC(), LabelGC->Font(), kChildFrame, LabelGC->BG());
		fHeap.AddFirst((TObject *) label);
		this->AddFrame(label, LabelGC->LH());
		label->SetTextJustify(kTextLeft);
	}

	fFileInfo.fFileTypes = (const Char_t **) rootFileTypes;
	fFileInfo.fIniDir = StrDup(gSystem->WorkingDirectory());

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 0, 0);
	fHeap.AddFirst((TObject *) frameLayout);
	fEC = new TGVerticalFrame(this, ComboWidth, Height, kChildFrame, FrameGC->BG());
	fHeap.AddFirst((TObject *) fEC);
	this->AddFrame(fEC, frameLayout);
		
	fEB = new TGHorizontalFrame(fEC, ComboWidth, Height, kChildFrame, FrameGC->BG());
	fHeap.AddFirst((TObject *) fEB);
	fEC->AddFrame(fEB, frameLayout);
		
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 0);
	fHeap.AddFirst((TObject *) btnLayout);
	fBrowse = new TGPictureButton(fEB, fClient->GetPicture("ofolder_t.xpm"), 0);
	fHeap.AddFirst((TObject *) fBrowse);
	fEB->AddFrame(fBrowse, btnLayout);
	fBrowse->ChangeBackground(BrowseGC->BG());
	fBrowse->SetToolTipText("Browse files", 500);
	fBrowse->Associate(this);
	bSize = fBrowse->GetDefaultWidth();

	fEntry = new TGTextEntry(fEB, new TGTextBuffer(BufferSize), EntryId);
	fEntry->SetFont(EntryGC->Font());
	fEntry->SetBackgroundColor(EntryGC->BG());
	fEntry->Associate(this);

	fHeap.AddFirst((TObject *) fEntry);
	fEB->AddFrame(fEntry, EntryGC->LH());
	fEntry->Resize(EntryWidth - bSize, Height);
	
	fCombo = new TGComboBox(fEC, ComboId, ComboOptions, ComboGC->BG());
	fHeap.AddFirst((TObject *) fCombo);
	fEC->AddFrame(fCombo, ComboGC->LH());
	((TGListBox *) fCombo->GetListBox())->SetHeight(Height * 10);
	fCombo->Resize(ComboWidth, Height);
	
}

Bool_t TGMrbFileObjectCombo::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo::ProcessMessage
// Purpose:        Message handler for browse button
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle message received from browse button
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (GET_MSG(MsgId)) {
		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					switch (Param1) {
						case 0:
							new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fFileInfo);
							if (fFileInfo.fFilename != NULL && *fFileInfo.fFilename != '\0') {
								this->OpenFile(fFileInfo.fFilename);
								fCombo->Select(0);
							}
							break;
					}
					break;
			}
			break;
		case kC_TEXTENTRY:
			switch (GET_SUBMSG(MsgId)) {
				case kTE_ENTER:
					{
						TString fName;
						this->GetFileEntry(fName, kTRUE);
						this->OpenFile(fName.Data());
						fCombo->Select(0);
					}
					break;
			}
			break;
	}
	return(kTRUE);
}

void TGMrbFileObjectCombo::SetFileEntry(const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo::SetFileEntry
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

const Char_t * TGMrbFileObjectCombo::GetFileEntry(TString & FileName, Bool_t FullPath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo::GetFileEntry
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

Int_t TGMrbFileObjectCombo::GetSelectionAsString(TString & SelItem, Bool_t FullPath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo::GetSelectionAsString
// Purpose:        Return selected file/object as string
// Arguments:      TString SelItem         -- where to store selection data
//                 Bool_t FullPath         -- prepend full file path if kTRUE
// Results:        Int_t NofItems          -- number of selected items (+1 for filename)
// Exceptions:     
// Description:    Returns selected item as string: "file:object"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	SelItem = "";
	TGTextLBEntry * lbEntry = (TGTextLBEntry *) fCombo->GetSelectedEntry();
	if (lbEntry) {
		this->GetFileEntry(SelItem, FullPath);
		SelItem = SelItem.Strip(TString::kBoth);
		if (SelItem.IsNull()) return(0);
		TString item = lbEntry->GetText()->GetString();
		Int_t idx = item.Index(")", 0) + 1;
		if (idx < 0) idx = 0;
		item = item(idx, item.Length());
		idx = item.Index("[", 0) - 1;
		if (idx < 0) idx = item.Length();
		item.Resize(idx);
		item = item.Strip(TString::kBoth);
		if (item.IsNull()) return(1);
		SelItem += ":";
		SelItem += item; 
		return(2);
	}
	return(0);
}

Int_t TGMrbFileObjectCombo::GetSelection(TObjArray & SelArr, Bool_t FullPath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo::GetSelection
// Purpose:        Return selected file/object
// Arguments:      TObjArray SelArr        -- where to store selection data
//                 Bool_t FullPath         -- prepend full file path if kTRUE
// Results:        Int_t NofItems          -- number of selected items (0/1)
// Exceptions:     
// Description:    Returns selected item as TObjArray
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	SelArr.Delete();
	TGTextLBEntry * lbEntry = (TGTextLBEntry *) fCombo->GetSelectedEntry();
	if (lbEntry) {
		TString file;
		this->GetFileEntry(file, FullPath);
		SelArr.Add(new TObjString(file.Data()));
		TString item = lbEntry->GetText()->GetString();
		Int_t idx = item.Index(")", 0);
		if (idx > 0) item = item(idx + 1, item.Length());
		idx = item.Index("[", 0);
		if (idx > 0) item.Resize(idx - 1);
		item = item.Strip(TString::kBoth);
		if (item.IsNull()) return(1);
		SelArr.Add(new TObjString(item.Data()));
		return(2);
	}
	return(0);
}

void TGMrbFileObjectCombo::SetSelectionFromString(TString & SelString, Bool_t IsNewFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::SetSelectionFromString
// Purpose:        Fill file entry and listbox from array
// Arguments:      TString & SelString     -- string containing file name and file items
//                 Bool_t IsNewFile        -- open new file anyway
// Results:        
// Exceptions:     
// Description:    Inserts file name into file entry and file items into listbox.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->ClearList();
	Int_t from;
	TString item;
	Int_t id = -1;
	while (SelString.Tokenize(item, from, ":")) {
		if (id == -1) {
			if (IsNewFile || fFileName.CompareTo(item.Data()) != 0) this->OpenFile(item.Data());
		} else {
			fCombo->AddEntry(item.Data(), id);
		}
		id++;
	}
}

void TGMrbFileObjectCombo::SetSelection(TObjArray & SelArr, Bool_t IsNewFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::SetSelection
// Purpose:        Fill file entry and listbox from array
// Arguments:      TObjArray & SelArr     -- array containing file name and file items
//                 Bool_t IsNewFile        -- open new file anyway
// Results:        
// Exceptions:     
// Description:    Inserts file name into file entry and file items into listbox.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->ClearList();
	TIterator * iter = SelArr.MakeIterator();
	TObjString * o;
	Int_t id = -1;
	while (o = (TObjString *) iter->Next()) {
		if (id == -1) {
			if (IsNewFile || fFileName.CompareTo(o->GetString()) != 0) this->OpenFile(o->GetString());
		} else {
			fCombo->AddEntry(o->GetString(), id);
		}
		id++;
	}
}

Bool_t TGMrbFileObjectCombo::OpenFile(const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo::OpenFile
// Purpose:        Open root file
// Arguments:      Char_t * FileName  -- file name
// Results:        kTRUE/kFALSE
// Description:    Open file given by selection
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TFile * rootFile = new TFile(FileName);
	if (!rootFile->IsOpen()) {
		TString err = "Not a ROOT file:\n";
		err += FileName;
		new TGMsgBox(fClient->GetRoot(), this, "TGMrbFileObjectCombo: Error", err.Data(), kMBIconStop);
		return(kFALSE);
	}
	fFileName = FileName;
	this->SetFileEntry(FileName);
	TList * fileKeys = rootFile->GetListOfKeys();
	Int_t idx = 0;
	this->ClearList();
	TKey * key;
	TIterator * keyIter = fileKeys->MakeIterator();
	while (key = (TKey *) keyIter->Next()) {
		TMrbString keyName = "(";
		keyName += key->GetClassName();
		keyName += " *) ";
		keyName += key->GetName();
		TObject * obj = key->ReadObj();
		if (obj) {
			if (obj->InheritsFrom("TH1")) {
				TH1 *h = (TH1 *) obj;
				keyName += " [";
				keyName += h->GetNbinsX();
				if (h->GetDimension() == 2) {
					keyName += ",";
					keyName += h->GetNbinsY();
				}
				keyName += "]";
			}
			fCombo->AddEntry(keyName.Data(), idx);
			idx++;
		}
	}
	return(kTRUE);
}

TGMrbFileObjectListBox::TGMrbFileObjectListBox(const TGWindow * Parent,
												const Char_t * Label,
												Int_t BufferSize,
												Int_t EntryId, Int_t ListBoxId,
												Int_t Width, Int_t Height,
												Int_t EntryWidth, Int_t ListBoxWidth,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * EntryGC,
												TGMrbLayout * BrowseGC,
												TGMrbLayout * ListBoxGC,
												UInt_t FrameOptions,
												UInt_t EntryOptions,
												UInt_t ListBoxOptions) :
								TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox
// Purpose:        Define a file entry together with a listbox
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 Int_t BufferSize            -- buffer size
//                 Int_t EntryId               -- id to be used in ProcessMessage
//                 Int_t ListBoxId             -- ...
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- width of the entry field
//                 Int_t ListBoxWidth          -- width of the combo box
//                 TGMrbLayout * FrameGC       -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC       -- ... (label)
//                 TGMrbLayout * EntryGC       -- ... (entry)
//                 TGMrbLayout * BrowseGC      -- ... (browse)
//                 TGMrbLayout * ListBoxGC     -- ... (listbox)
//                 UInt_t FrameOptions         -- frame options
//                 UInt_t EntryOptions         -- options to configure the entry
//                 UInt_t ListBoxOptions       -- options to configure the listbox
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGLabel * label;
	Int_t bSize;

	LabelGC = this->SetupGC(LabelGC, FrameOptions);
	EntryGC = this->SetupGC(EntryGC, FrameOptions);
	BrowseGC = this->SetupGC(BrowseGC, FrameOptions);
	ListBoxGC = this->SetupGC(ListBoxGC, FrameOptions);

	if (Label != NULL) {
		label = new TGLabel(this, new TGString(Label), LabelGC->GC(), LabelGC->Font(), kChildFrame, LabelGC->BG());
		fHeap.AddFirst((TObject *) label);
		this->AddFrame(label, LabelGC->LH());
		label->SetTextJustify(kTextLeft);
	}

	fFileInfo.fFileTypes = (const Char_t **) rootFileTypes;
	fFileInfo.fIniDir = StrDup(gSystem->WorkingDirectory());

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsCenterY, 0, 0, 0, 0);
	fHeap.AddFirst((TObject *) frameLayout);
	fEC = new TGVerticalFrame(this, ListBoxWidth, Height, kChildFrame, FrameGC->BG());
	fHeap.AddFirst((TObject *) fEC);
	this->AddFrame(fEC, frameLayout);
		
	fEB1 = new TGHorizontalFrame(fEC, ListBoxWidth, Height, kChildFrame, FrameGC->BG());
	fHeap.AddFirst((TObject *) fEB1);
	fEC->AddFrame(fEB1, frameLayout);
		
	fEB2 = new TGHorizontalFrame(fEC, ListBoxWidth, Height, kChildFrame, FrameGC->BG());
	fHeap.AddFirst((TObject *) fEB2);
	fEC->AddFrame(fEB2, frameLayout);
		
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 0, 0);
	fHeap.AddFirst((TObject *) btnLayout);
	fBrowse = new TGPictureButton(fEB1, fClient->GetPicture("ofolder_t.xpm"), kBtnBrowse);
	fHeap.AddFirst((TObject *) fBrowse);
	fEB1->AddFrame(fBrowse, btnLayout);
	fBrowse->ChangeBackground(BrowseGC->BG());
	fBrowse->SetToolTipText("Browse files", 500);
	fBrowse->Associate(this);
	bSize = fBrowse->GetDefaultWidth();

	fEntry = new TGTextEntry(fEB1, new TGTextBuffer(BufferSize), EntryId);
	fEntry->SetFont(EntryGC->Font());
	fEntry->SetBackgroundColor(EntryGC->BG());
	fEntry->Associate(this);

	fHeap.AddFirst((TObject *) fEntry);
	fEB1->AddFrame(fEntry, EntryGC->LH());
	fEntry->Resize(EntryWidth - bSize, Height);
	
	TGLayoutHints * rbLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 0, 0);
	fHeap.AddFirst((TObject *) rbLayout);
	fRBSingle = new TGRadioButton(fEB2, "single", kBtnSingle);
	fHeap.AddFirst((TObject *) fRBSingle);
	fEB2->AddFrame(fRBSingle, rbLayout);
	fRBSingle->SetToolTipText("Select single objects", 500);
	fRBSingle->SetState(kButtonUp);
	fRBSingle->Associate(this);

	fRBRange = new TGRadioButton(fEB2, "range", kBtnRange);
	fHeap.AddFirst((TObject *) fRBRange);
	fEB2->AddFrame(fRBRange, rbLayout);
	fRBRange->SetToolTipText("Select range of objects", 500);
	fRBRange->SetState(kButtonDown);
	fRBRange->Associate(this);
	fStartIndex = -1;

	TGLayoutHints * tbLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 10, 10, 0, 0);
	fHeap.AddFirst((TObject *) tbLayout);

	fTBClear = new TGTextButton(fEB2, "clear", kBtnClear);
	fHeap.AddFirst((TObject *) fTBClear);
	fEB2->AddFrame(fTBClear, tbLayout);
	fTBClear->SetToolTipText("Clear selection", 500);
	fTBClear->Associate(this);

	fTBApply = new TGTextButton(fEB2, "apply", kBtnApply);
	fHeap.AddFirst((TObject *) fTBApply);
	fEB2->AddFrame(fTBApply, tbLayout);
	fTBApply->SetToolTipText("Apply selection", 500);
	fTBApply->Associate(this);

	fListBox = new TGListBox(fEC, ListBoxId, ListBoxOptions, ListBoxGC->BG());
	fHeap.AddFirst((TObject *) fListBox);
	fEC->AddFrame(fListBox, ListBoxGC->LH());
	fListBox->SetHeight(Height * 10);
	fListBox->Resize(ListBoxWidth, fListBox->GetHeight());
	fListBox->SetMultipleSelections(kTRUE);
	fListBox->Associate(this);
}

Bool_t TGMrbFileObjectListBox::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::ProcessMessage
// Purpose:        Message handler for browse button
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle message received from browse button
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (GET_MSG(MsgId)) {
		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					switch (Param1) {
						case kBtnBrowse:
							new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fFileInfo);
							if (fFileInfo.fFilename != NULL && *fFileInfo.fFilename != '\0') {
								this->OpenFile(fFileInfo.fFilename);
							}
							break;

						case kBtnClear:
							{
  								this->ClearList();
								TIterator * iter = fLofListItems.MakeIterator();
								TMrbNamedX * nx;
								while (nx = (TMrbNamedX *) iter->Next()) {
									fListBox->AddEntry(nx->GetTitle(), nx->GetIndex());
								}
								fListBox->Layout();
								fStartIndex = -1;
							}
							break;
						case kBtnApply:
							{
								TList selected;
								fListBox->GetSelectedEntries(&selected);
								TMrbLofNamedX lofSelected;
								TIterator * iter = selected.MakeIterator();
								TGTextLBEntry * lbe;
								Int_t idx = 0;
								while (lbe = (TGTextLBEntry *) iter->Next()) {
									lofSelected.AddNamedX(idx, lbe->GetText()->GetString());
									idx++;
								}
  								this->ClearList();
								iter = lofSelected.MakeIterator();
								TMrbNamedX * nx;
								while (nx = (TMrbNamedX *) iter->Next()) {
									fListBox->AddEntry(nx->GetName(), nx->GetIndex());
								}
								fListBox->Layout();
								fStartIndex = -1;
							}
							break;
					}
					break;

				case kCM_RADIOBUTTON:
					switch (Param1) {
						case kBtnSingle:
							fRBSingle->SetState(kButtonDown);
							fRBRange->SetState(kButtonUp);
							break;
						case kBtnRange:
							fRBRange->SetState(kButtonDown);
							fRBSingle->SetState(kButtonUp);
							break;
					}
					break;

				case kCM_LISTBOX:
					if (fRBRange->GetState() == kButtonDown) {
						if (fStartIndex == -1) {
							fStartIndex = Param2;
						} else {
							if (fStartIndex <= Param2) {
								for (Int_t i = fStartIndex; i <= Param2; i++) fListBox->Select(i, kTRUE);
							} else {
								for (Int_t i = Param2; i < fStartIndex; i++) fListBox->Select(i, kTRUE);
							}
							fStartIndex = -1;
						}
					}
					break;
			}
			break;
		case kC_TEXTENTRY:
			switch (GET_SUBMSG(MsgId)) {
				case kTE_ENTER:
					{
						TString fName;
						this->GetFileEntry(fName, kTRUE);
						this->OpenFile(fName.Data());
					}
					break;
			}
			break;
	}
	return(kTRUE);
}

void TGMrbFileObjectListBox::SetList(TObjArray & LofEntries) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::SetList
// Purpose:        Set list entries
// Arguments:      TObjArray & LofEntries  -- list of entries
// Results:        
// Exceptions:     
// Description:    Inserts names into list
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,10,0)
  	fListBox->RemoveAll();
#else
    fListBox->RemoveEntries(0, 1000);
#endif
	TIterator * iter = LofEntries.MakeIterator();
	TObjString * str;
	Int_t idx = 0;
	while (str = (TObjString *) iter->Next()) {
		fListBox->AddEntry(str->GetString(), idx);
		idx++;
	}
	fListBox->Layout();
	fStartIndex = -1;
}

void TGMrbFileObjectListBox::SetFileEntry(const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::SetFileEntry
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

const Char_t * TGMrbFileObjectListBox::GetFileEntry(TString & FileName, Bool_t FullPath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::GetFileEntry
// Purpose:        Return file name
// Arguments:      Bool_t FullPath  -- return full path
// Results:        
// Exceptions:     
// Description:    Returns entry text prepended by fFileInfo.fIniDir if needed.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	FileName = fEntry->GetText();
	FileName = FileName.Strip(TString::kBoth);
	if (FileName.IsNull()) return("");

	if (FullPath && FileName(0) != '/') {
		FileName.Prepend("/");
		FileName.Prepend(fFileInfo.fIniDir);
	}
	return(FileName.Data());
}

Int_t TGMrbFileObjectListBox::GetSelectionAsString(TString & SelString, Bool_t FullPath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::GetSelectionAsString
// Purpose:        Return selected file/objects as string
// Arguments:      TString SelString       -- where to store selection data
//                 Bool_t FullPath         -- prepend full file path if kTRUE
// Results:        Int_t NofItems          -- number of selected items (+1 for filename)
// Exceptions:     
// Description:    Returns selected item "file:obj1:obj2:...:objN"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	SelString = "";
	Int_t n = fListBox->GetNumberOfEntries();
	if (n == 0) return(0);

	this->GetFileEntry(SelString, FullPath);
	SelString.Strip(TString::kBoth);
	if (SelString.IsNull()) return(0);

	Int_t nofSelected = 1;
	for (Int_t i = 0; i < n; i++) {
		TGTextLBEntry * lbEntry = (TGTextLBEntry *) fListBox->GetEntry(i);
		if (lbEntry) {
			TString item = lbEntry->GetText()->GetString();
			Int_t idx = item.Index(")", 0) + 1;
			if (idx < 0) idx = 0;
			item = item(idx, item.Length());
			idx = item.Index("[", 0) - 1;
			if (idx < 0) idx = item.Length();
			item.Resize(idx);
			item = item.Strip(TString::kBoth);
			if (!item.IsNull()) {
				SelString += ":";
				SelString += item;
				nofSelected++;
			}
		} 
	}
	return(nofSelected);
}

Int_t TGMrbFileObjectListBox::GetSelection(TObjArray & SelArr, Bool_t FullPath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::GetSelection
// Purpose:        Return selected file/objects
// Arguments:      TString SelItem         -- where to store selection data
//                 Bool_t FullPath         -- prepend full file path if kTRUE
// Results:        Int_t NofItems          -- number of selected items
// Exceptions:     
// Description:    Returns selected item as TObjArray
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	SelArr.Delete();
	Int_t n = fListBox->GetNumberOfEntries();
	if (n == 0) return(0);

	TString file;
	this->GetFileEntry(file, FullPath);
	file.Strip(TString::kBoth);
	if (file.IsNull()) return(0);

	SelArr.Add(new TObjString(file.Data()));

	Int_t nofSelected = 1;
	for (Int_t i = 0; i < n; i++) {
		TGTextLBEntry * lbEntry = (TGTextLBEntry *) fListBox->GetEntry(i);
		if (lbEntry) {
			TString item = lbEntry->GetText()->GetString();
			Int_t idx = item.Index(")", 0);
			if (idx > 0) item = item(idx + 1, item.Length());
			idx = item.Index("[", 0);
			if (idx > 0) item.Resize(idx - 1);
			item = item.Strip(TString::kBoth);
			if (!item.IsNull()) {
				SelArr.Add(new TObjString(item.Data()));
				nofSelected++;
			}
		} 
	}
	return(nofSelected);
}

void TGMrbFileObjectListBox::SetSelectionFromString(TString & SelString, Bool_t IsNewFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::SetSelectionFromString
// Purpose:        Fill file entry and listbox from array
// Arguments:      TString & SelString     -- string containing file name and file items
//                 Bool_t IsNewFile        -- open new file anyway
// Results:        
// Exceptions:     
// Description:    Inserts file name into file entry and file items into listbox.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->ClearList();
	Int_t from;
	TString item;
	Int_t id = -1;
	while (SelString.Tokenize(item, from, ":")) {
		if (id == -1) {
			if (IsNewFile || fFileName.CompareTo(item.Data()) != 0) this->OpenFile(item.Data());
			this->ClearList();
		} else {
			fListBox->AddEntry(item.Data(), id);
		}
		id++;
	}
}

void TGMrbFileObjectListBox::SetSelection(TObjArray & SelArr, Bool_t IsNewFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::SetSelection
// Purpose:        Fill file entry and listbox from array
// Arguments:      TObjArray & SelArr     -- array containing file name and file items
//                 Bool_t IsNewFile        -- open new file anyway
// Results:        
// Exceptions:     
// Description:    Inserts file name into file entry and file items into listbox.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->ClearList();
	TIterator * iter = SelArr.MakeIterator();
	TObjString * o;
	Int_t id = -1;
	while (o = (TObjString *) iter->Next()) {
		if (id == -1) {
			if (IsNewFile || fFileName.CompareTo(o->GetString()) != 0) this->OpenFile(o->GetString());
			this->ClearList();
		} else {
			fListBox->AddEntry(o->GetString(), id);
		}
		id++;
	}
}

Bool_t TGMrbFileObjectListBox::OpenFile(const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectListBox::OpenFile
// Purpose:        Open root file
// Arguments:      Char_t * FileName  -- file name
// Results:        kTRUE/kFALSE
// Description:    Open file given by selection
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TFile * rootFile = new TFile(FileName);
	if (!rootFile->IsOpen()) {
		TString err = "Not a ROOT file:\n";
		err += FileName;
		new TGMsgBox(fClient->GetRoot(), this, "TGMrbFileObjectListBox: Error", err.Data(), kMBIconStop);
		return(kFALSE);
	}
	fFileName = FileName;
	this->SetFileEntry(FileName);
	TList * fileKeys = rootFile->GetListOfKeys();
	Int_t idx = 0;
	this->ClearList();
	fLofListItems.Delete();
	TKey * key;
	TIterator * keyIter = fileKeys->MakeIterator();
	while (key = (TKey *) keyIter->Next()) {
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
			fListBox->AddEntry(keyName.Data(), idx);
			fLofListItems.AddNamedX(idx, key->GetName(), keyName.Data());
			idx++;
		}
		key = (TKey *) fileKeys->After(key);
	}
	fListBox->Layout();
	return(kTRUE);
}

