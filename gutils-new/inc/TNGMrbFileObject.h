#ifndef __TNGMrbFileObject_h__
#define __TNGMrbFileObject_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbFileObject.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TNGMrbFileObjectCombo   -- a file entry together with
//                                           a combo box containing file objects
//                 TNGMrbFileObjectList    -- a file entry together with
//                                           a list box containing file objects
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbFileObject.h,v 1.1 2009-03-27 09:39:35 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TGFileDialog.h"
#include "TGComboBox.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TNGMrbProfile.h"

#include "UseHeap.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbFileObjectCombo
// Purpose:        Define a labelled file entry and an associated combo box
// Description:    Defines a labelled file entry (TNGMrbFileEntry) together
//                 with a combo box showing ROOT objects from selected file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbFileObjectCombo: public TGCompositeFrame {

	public:
		enum EGMrbComboButton	{	kGMrbComboButtonUp,
									kGMrbComboButtonDown,
									kGMrbComboButtonBegin,
									kGMrbComboButtonEnd,
									kGMrbComboButtonBrowse
								};

		enum EGMrbComboOption	{	kGMrbComboHasUpDownButtons	 	= 0x1 << 0,
									kGMrbComboHasBeginEndButtons 	= 0x1 << 1
								};

		enum EGMrbComboPad		{	kGMrbComboPadW  			= 5,
									kGMrbComboPadH				= 1
								};

	public:
		TNGMrbFileObjectCombo(	const TGWindow * Parent,				// ctor
								const Char_t * Label,
								TNGMrbProfile * Profile,
								Int_t EntryId, Int_t ComboId,
								Int_t Width, Int_t Height,
								Int_t EntryWidth = 0, Int_t ComboWidth = 0,
								UInt_t ComboOptions = 0);

		~TNGMrbFileObjectCombo() { CLEAR_HEAP(); };				// default dtor

		inline TGTextEntry * GetEntry() const { return(fEntry); };
		inline TGComboBox * GetComboBox() const { return(fCombo); };

		void SetFileEntry(const Char_t * File);
		const Char_t * GetFileEntry(TString & FileName, Bool_t FullPath = kTRUE) const;	// get selected file
		const Char_t * GetSelection(TString & SelItem, Bool_t FullPath = kTRUE) const;	// get selected file/object

		inline TGLabel * GetLabel() const { return(fLabel); };
		inline void SetLabelText(const Char_t * Text) { if (fLabel) fLabel->SetText(Text); };
		inline const Char_t * GetLabelText() const { return(fLabel ? fLabel->GetText()->GetString() : "???"); };

		inline void SetFont(FontStruct_t Font) { fEntry->SetFont(Font); };

		inline void SetEntryBackground(Pixel_t Back) { fEntry->SetBackgroundColor(Back); };
		inline void SetComboBackground(Pixel_t Back) { fCombo->SetBackgroundColor(Back); };
		inline void SetBackground(Pixel_t Back) {
			this->SetEntryBackground(Back);
			this->SetComboBackground(Back);
		};
		inline void SetLabelBackground(Pixel_t Back) { if (fLabel) fLabel->SetBackgroundColor(Back); };

		inline TGPictureButton * GetButtonUp() const { return(fButtonUp); };
		inline TGPictureButton * GetButtonDown() const { return(fButtonDown); };
		inline TGPictureButton * GetButtonBegin() const { return(fButtonBegin); };
		inline TGPictureButton * GetButtonEnd() const { return(fButtonEnd); };
		inline TGPictureButton * GetBrowseButton() { return(fBrowse); };

		inline Bool_t HasUpDownButtons() const { return(fButtonUp != NULL); };
		inline Bool_t HasBeginEndButtons() const { return(fButtonBegin != NULL); };
		inline Bool_t HasLabel() const { return(fLabel != NULL); };

		inline Int_t GetEntryWidth() { return(fEntryWidth); };
		inline Int_t GetComboWidth() { return(fComboWidth); };

		inline const Char_t * GetText() const { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field
		
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbFileObjectCombo.html&"); };

	protected:
		Bool_t OpenFile(const Char_t * File);

	protected:
		USE_HEAP();

		Int_t fEntryWidth;
		Int_t fComboWidth;
		TGLabel * fLabel;					//! entry label

		TGTextEntry * fEntry;				//!
		TGComboBox * fCombo;				//!

		TGPictureButton * fBrowse;	 		//!
		TGPictureButton * fButtonUp; 		//! button up
		TGPictureButton * fButtonDown;		//! button down
		TGPictureButton * fButtonBegin; 	//! button begin
		TGPictureButton * fButtonEnd; 		//! button end

		TMrbLofNamedX fLofEntries;			//! entries

		TString fText;

		TGWindow * fClientWindow;

		TGFileInfo fFileInfo; 				//!

	ClassDef(TNGMrbFileObjectCombo, 1) 	// [GraphUtils] A file browser with a combo box showing ROOT objects
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelComboLayout
// Purpose:        Layout manager for TNGMrbLabelCombo widget
// Description:    Defines widget layout
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbFileObjectComboLayout : public TGLayoutManager {

	public:
		TNGMrbFileObjectComboLayout(TNGMrbFileObjectCombo * Widget) { fWidget = Widget; };
		virtual void Layout();
		inline TGDimension GetDefaultSize() const { return(fWidget->GetSize()); };

	protected:
		TNGMrbFileObjectCombo * fWidget;        // pointer to main widget

};

#endif
