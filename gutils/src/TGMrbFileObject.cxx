//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbFileObject.cxx
// Purpose:        MARaBOU graphic utilities: a file entry and a combo box for ROOT objects
// Description:    Implements class methods to handle a file entry together with a combo box
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbFileObjectCombo.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <iostream>
#include <iomanip>

#include "TKey.h"
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

	fEntry = new TGTextEntry(fEB, new TGTextBuffer(BufferSize), EntryId,
										EntryGC->GC(), EntryGC->Font(), EntryOptions, EntryGC->BG());
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

	TFile * rootFile;
	TList * fileKeys;
	TKey * key;
	Int_t idx;
	TObject * obj;
	TH1 * h;
					
	if (GET_MSG(MsgId) == kC_COMMAND) {
		switch (GET_SUBMSG(MsgId)) {
			case kCM_BUTTON:
				switch (Param1) {
					case 0:
						new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fFileInfo);
						if (fFileInfo.fFilename != NULL && *fFileInfo.fFilename != '\0') {
							rootFile = new TFile(fFileInfo.fFilename);
							if (!rootFile->IsOpen()) {
								new TGMsgBox(fClient->GetRoot(), this, "TGMrbFileObjectCombo: Error", "Not a ROOT file", kMBIconStop);
								break;
							}
							this->SetFileEntry(fFileInfo.fFilename);
							fileKeys = rootFile->GetListOfKeys();
							fileKeys->Sort();
							key = (TKey *) fileKeys->First();
							idx = 0;
							Int_t nofEntries = fCombo->GetListBox()->GetNumberOfEntries();
							fCombo->RemoveEntries(0, nofEntries - 1);
							while (key) {
								TMrbString keyName = "(";
								keyName += key->GetClassName();
								keyName += " *) ";
								keyName += key->GetName();
								obj = key->ReadObj();
								if (obj) {
									if (obj->InheritsFrom("TH1")) {
										h = (TH1 *) obj;
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
								key = (TKey *) fileKeys->After(key);
							}
							fCombo->Select(0);
						}
						break;
				}
		}
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

const Char_t * TGMrbFileObjectCombo::GetFileEntry(TString & FileName, Bool_t FullPath) {
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

const Char_t * TGMrbFileObjectCombo::GetSelection(TString & SelItem, Bool_t FullPath) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileObjectCombo::GetSelection
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
