//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TGMrbFileEntry.cxx
// Purpose:        MARaBOU graphic utilities: a labelled file entry
// Description:    Implements class methods to handle a labelled file entry
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
// Layout:
//Begin_Html
/*
<img src=gutils/TGMrbFileEntry.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

#include "TGLabel.h"
#include "TGToolTip.h"
#include "TMrbSystem.h"

#include "TGMrbFileEntry.h"

ClassImp(TGMrbFileEntry)

TGMrbFileEntry::TGMrbFileEntry(const TGWindow * Parent,
												const Char_t * Label,
												Int_t BufferSize, Int_t EntryId,
												Int_t Width, Int_t Height,
												Int_t EntryWidth,
												TGFileInfo * FileInfo, EFileDialogMode DialogMode,
												TGMrbLayout * FrameGC,
												TGMrbLayout * LabelGC,
												TGMrbLayout * EntryGC,
												TGMrbLayout * BrowseGC,
												UInt_t FrameOptions,
												UInt_t EntryOptions) :
								TGCompositeFrame(Parent, Width, Height, FrameOptions, FrameGC->BG()) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileEntry
// Purpose:        Define a labelled entry suitable to browse files
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 Int_t BufferSize            -- size of text buffer in chars
//                 Int_t EntryId               -- id to be used in ProcessMessage
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- widht of the entry field
//                 TGFileInfo * FileInfo       -- file info
//                 EFileDialogMode DialogMode  -- mode: open or save
//                 TGMrbLayout * LabelGC       -- graphic context & layout (frame)
//                 TGMrbLayout * LabelGC       -- ... (label)
//                 TGMrbLayout * ButtonGC      -- ... (button)
//                 TGMrbLayout * BrowseGC      -- ... (browse)
//                 UInt_t FrameOptions         -- frame options
//                 UInt_t EntryOptions         -- options to configure the entry
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

	if (Label != NULL) {
		label = new TGLabel(this, new TGString(Label), LabelGC->GC(), LabelGC->Font(), kChildFrame, LabelGC->BG());
		fHeap.AddFirst((TObject *) label);
		this->AddFrame(label, LabelGC->LH());
		label->SetTextJustify(kTextLeft);
	}

	if (FileInfo) {
		fFileInfo.fFileTypes = FileInfo->fFileTypes;
		fFileInfo.fIniDir = StrDup(FileInfo->fIniDir);
		TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 0, 0);
		fHeap.AddFirst((TObject *) btnLayout);
		fBrowse = new TGPictureButton(this, fClient->GetPicture("ofolder_t.xpm"), 0);
		fHeap.AddFirst((TObject *) fBrowse);
		this->AddFrame(fBrowse, btnLayout);
		fBrowse->ChangeBackground(BrowseGC->BG());
		fBrowse->SetToolTipText("Browse files", 500);
		fBrowse->Associate(this);
		bSize = fBrowse->GetDefaultWidth();
	} else bSize = 0;

	fEntry = new TGTextEntry(this, new TGTextBuffer(BufferSize), EntryId,
										EntryGC->GC(), EntryGC->Font(), EntryOptions, EntryGC->BG());
	fHeap.AddFirst((TObject *) fEntry);
	this->AddFrame(fEntry, EntryGC->LH());
	fEntry->Resize(EntryWidth - bSize, Height);
}

Bool_t TGMrbFileEntry::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbFileEntry::ProcessMessage
// Purpose:        Message handler for browse button
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle message received from browse button
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbSystem ux;
	TGFileInfo fi;

	if (GET_MSG(MsgId) == kC_COMMAND) {
		switch (GET_SUBMSG(MsgId)) {
			case kCM_BUTTON:
				switch (Param1) {
					case 0:
						new TGFileDialog(fClient->GetRoot(), this, fDialogMode, &fFileInfo);
						if (fFileInfo.fFilename != NULL && *fFileInfo.fFilename != '\0') fEntry->SetText(fFileInfo.fFilename);
						break;
				}
		}
	}
	return(kTRUE);
}

void TGMrbFileEntry::FileButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:       TGMrbFileEntry::FileButtonEnableonEnable
// Purpose:        Enable/disable file button
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables file button.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fBrowse) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fBrowse->SetState(btnState);
	}
}

