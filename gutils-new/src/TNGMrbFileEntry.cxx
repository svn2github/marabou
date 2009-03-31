//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbFileEntry.cxx
// Purpose:        MARaBOU graphic utilities: a labelled file entry
// Description:    Implements class methods to handle a labelled file entry
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbFileEntry.cxx,v 1.3 2009-03-31 14:34:32 Rudolf.Lutter Exp $       
// Date:           
// Layout:         A labelled entry widget with a file button
//Begin_Html
/*
<img src=gutils/TNGMrbFileEntry.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TGToolTip.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "TNGMrbFileEntry.h"

ClassImp(TNGMrbFileEntry)

extern TMrbLogger * gMrbLog;		// access to message logging

TNGMrbFileEntry::TNGMrbFileEntry( const TGWindow * Parent,
								const Char_t * Label,
								TGFileInfo * FileInfo, EFileDialogMode DialogMode,
								TNGMrbProfile * Profile,
								Int_t EntryId, Int_t Width, Int_t Height, Int_t EntryWidth,
								UInt_t EntryOptions) :
						TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions() | kHorizontalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileEntry
// Purpose:        Define a labelled entry suitable to browse files
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 TNGMrbProfile * Profile      -- graphics profile
//                 TGFileInfo * FileInfo       -- file info
//                 EFileDialogMode DialogMode  -- mode: open or save
//                 Int_t EntryId               -- id to be used in ProcessMessage
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- widht of the entry field
//                 UInt_t EntryOptions         -- options to configure the entry
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	TNGMrbGContext * labelGC = Profile->GetGC(TNGMrbGContext::kGMrbGCLabel);
	TNGMrbGContext * entryGC = Profile->GetGC(TNGMrbGContext::kGMrbGCTextEntry);
	TNGMrbGContext * buttonGC = Profile->GetGC(TNGMrbGContext::kGMrbGCButton);

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

	if (FileInfo) {
		fDialogMode = DialogMode;
		fFileInfo.fFileTypes = FileInfo->fFileTypes;
		fFileInfo.fIniDir = StrDup(FileInfo->fIniDir);
		fFileButton = new TGPictureButton(this, fClient->GetPicture("ofolder_t.xpm"), 0);
		TO_HEAP(fFileButton);
		this->AddFrame(fFileButton);
		fFileButton->ChangeBackground(buttonGC->BG());
		fFileButton->SetToolTipText("Browse files", 500);
		fFileButton->Associate(this);
	} else {
		fFileButton = NULL;
	}

	fEntry = new TGTextEntry(this, new TGTextBuffer(), EntryId);
	TO_HEAP(fEntry);
	this->AddFrame(fEntry);

	fEntryWidth = EntryWidth;

	this->SetFont(entryGC->Font());
	this->SetEntryBackground(entryGC->BG());
	this->Resize(Width, Height);
	this->SetLayoutManager(new TNGMrbFileEntryLayout(this));
}

Bool_t TNGMrbFileEntry::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileEntry::ProcessMessage
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
						if (fFileInfo.fFilename != NULL && *fFileInfo.fFilename != '\0') {
							fEntry->SetText(fFileInfo.fFilename);
							fEntry->SetAlignment(kTextRight);
						}
						break;
				}
		}
	}
	return(kTRUE);
}

void TNGMrbFileEntry::FileButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:       TNGMrbFileEntry::FileButtonEnableonEnable
// Purpose:        Enable/disable file button
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables file button.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fFileButton) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fFileButton->SetState(btnState);
	}
}

void TNGMrbFileEntryLayout::Layout() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileEntryLayout::Layout
// Purpose:        Layout manager
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Placement of widget elements
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fWidget) {
		UInt_t boxWidth = fWidget->GetWidth();			// width of labelled-entry box (label + entry + buttons)
		UInt_t boxHeight = fWidget->GetHeight() - 2 * TNGMrbFileEntry::kGMrbEntryPadH;	// height
		UInt_t entryWidth = fWidget->GetEntryWidth(); 	// width of entry field (entry + buttons)

		UInt_t labelWidth = fWidget->HasLabel() ? fWidget->GetLabel()->GetWidth() : 0;	// label width

		if (entryWidth == 0) entryWidth = boxWidth - labelWidth;

		if (labelWidth + entryWidth > boxWidth) {
			boxWidth = labelWidth + entryWidth;
			fWidget->Resize(boxWidth, fWidget->GetHeight());
		}
			
		Int_t x = boxWidth;
		Int_t y = TNGMrbFileEntry::kGMrbEntryPadH;
		Int_t delta = 0;

		TGPictureButton * fileButton = fWidget->GetFileButton();
		if (fileButton) {
			UInt_t w = fileButton->GetWidth();
			x -= w; delta += w;
			fileButton->MoveResize(x, y, w, boxHeight);
		}

		x = boxWidth - entryWidth;		// entry starts 'entryWidth' from right end
		entryWidth -= delta;			// compensate for buttons

		fWidget->GetEntry()->MoveResize(x, y, entryWidth, boxHeight);

		if (fWidget->HasLabel()) {
			fWidget->GetLabel()->MoveResize(TNGMrbFileEntry::kGMrbEntryPadW, y, x - TNGMrbFileEntry::kGMrbEntryPadW, boxHeight);
		}
	}
}
