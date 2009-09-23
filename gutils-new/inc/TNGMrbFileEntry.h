#ifndef __TNGMrbFileEntry_h__
#define __TNGMrbFileEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbFileEntry.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TNGMrbFileEntry         -- a TNGMrbLabelEntry suitable to
//                                           browse files
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbFileEntry.h,v 1.2 2009-09-23 10:42:51 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGFileDialog.h"
#include "TGTextEntry.h"

#include "TNGMrbProfile.h"

#include "UseHeap.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileEntry
// Purpose:        Define a labelled text entry to browse files
// Description:    Defines a label together with a text entry field.
//                 A BROWSE button on right side opens a file dialog.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbFileEntry: public TGCompositeFrame {

	public:
		enum EGMrbEntryPad		{	kGMrbEntryPadW				= 5,
									kGMrbEntryPadH				= 1
								};

	public:
		TNGMrbFileEntry( 	const TGWindow * Parent,
							const Char_t * Label,
							TGFileInfo * FileInfo, EFileDialogMode DialogMode,
							TNGMrbProfile * Profile,
							Int_t EntryId, Int_t Width, Int_t Height, Int_t EntryWidth = 0,
							UInt_t EntryOptions = 0);

		~TNGMrbFileEntry() { CLEAR_HEAP(); };				// default dtor

		inline TGTextEntry * GetEntry() const { return(fEntry); };

		inline Int_t GetEntryWidth() { return(fEntryWidth); };
		inline const Char_t * GetText() const {return (fEntry->GetText());}

		inline TGLabel * GetLabel() const { return(fLabel); };
		inline void SetLabelText(const Char_t * Text) { if (fLabel) fLabel->SetText(Text); };
		inline const Char_t * GetLabelText() const { return(fLabel ? fLabel->GetText()->GetString() : "???"); };

		inline void SetText(const Char_t * Text) { fEntry->SetText(Text); };
		inline void SetFont(FontStruct_t Font) { fEntry->SetFont(Font); };

		inline void SetEntryBackground(Pixel_t Back) { fEntry->SetBackgroundColor(Back); };
		inline void SetLabelBackground(Pixel_t Back) { if (fLabel) fLabel->SetBackgroundColor(Back); };

		inline Bool_t HasLabel() const { return(fLabel != NULL); };

		void FileButtonEnable(Bool_t Flag = kTRUE);			// enable/disable file button

		inline TGPictureButton * GetFileButton() { return(fFileButton); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbFileEntry.html&"); };

	protected:
		USE_HEAP(); 						//!

		Int_t fEntryWidth;					// width of numeric entry field (+ button width)
		TGLabel * fLabel;					//! entry label
		TGTextEntry * fEntry;				//!
		TGPictureButton * fFileButton;	 	//!
		TGFileInfo fFileInfo;	 			//!
		EFileDialogMode fDialogMode;

	ClassDef(TNGMrbFileEntry, 1) 	// [GraphUtils] A labelled entry field to browse file names
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileEntryLayout
// Purpose:        Layout manager for TNGMrbFileEntry widget
// Description:    Defines widget layout
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbFileEntryLayout : public TGLayoutManager {

	public:
		TNGMrbFileEntryLayout(TNGMrbFileEntry * EntryBox) { fWidget = EntryBox; }
		virtual void Layout();
		inline TGDimension GetDefaultSize() const { return(fWidget->GetSize()); };

	protected:
		TNGMrbFileEntry * fWidget;        // pointer to main widget

};

#endif
