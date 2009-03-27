#ifndef __TNGMrbLabelCombo_h__
#define __TNGMrbLabelCombo_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbLabelCombo.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TNGMrbLabelCombo     -- a frame containing a label and
//                                        a combo box
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbLabelCombo.h,v 1.1 2009-03-27 09:39:35 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGComboBox.h"

#include "TMrbLofNamedX.h"
#include "TNGMrbProfile.h"

#include "UseHeap.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelCombo
// Purpose:        Define a labelled combo box
// Description:    Defines a label together with a combo box
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbLabelCombo: public TGCompositeFrame {

	public:
		enum EGMrbComboButton	{	kGMrbComboButtonUp,
									kGMrbComboButtonDown,
									kGMrbComboButtonBegin,
									kGMrbComboButtonEnd
								};

		enum EGMrbComboOption	{	kGMrbComboHasUpDownButtons	 	= 0x1 << 0,
									kGMrbComboHasBeginEndButtons 	= 0x1 << 1
								};

		enum EGMrbComboPad		{	kGMrbComboPadW				= 5,
									kGMrbComboPadH				= 1
								};

	public:
		TNGMrbLabelCombo(	const TGWindow * Parent,
							const Char_t * Label,
							TMrbLofNamedX * Entries,
							TNGMrbProfile * Profile,
							Int_t ComboId, Int_t Selected,
							Int_t Width, Int_t Height, Int_t ComboWidth = 0,
							UInt_t ComboOptions = 0);

		~TNGMrbLabelCombo() { CLEAR_HEAP(); };				// default dtor

		void AddEntry(TMrbLofNamedX * Entries); 		// add one or more entries to the box
		void AddEntry(TMrbNamedX * Entry, Bool_t SelectFlag = kFALSE);
		inline void RemoveEntry(Int_t EntryId) { fCombo->RemoveEntry(EntryId); };
		void RemoveEntry(const Char_t * EntryName);
		inline TMrbLofNamedX * GetLofEntries() { return(&fLofEntries); };
		
		inline TGComboBox * GetComboBox() const { return(fCombo); };

		inline TGPictureButton * GetButtonUp() const { return(fButtonUp); };
		inline TGPictureButton * GetButtonDown() const { return(fButtonDown); };
		inline TGPictureButton * GetButtonBegin() const { return(fButtonBegin); };
		inline TGPictureButton * GetButtonEnd() const { return(fButtonEnd); };

		inline Int_t GetComboWidth() { return(fComboWidth); };

		void UpDownButtonEnable(Bool_t Flag = kTRUE);			// enable/disable up/down button
		void BeginEndButtonEnable(Bool_t Flag = kTRUE);			// enable/disable begin/end button

		inline Bool_t HasUpDownButtons() const { return(fButtonUp != NULL); };
		inline Bool_t HasBeginEndButtons() const { return(fButtonBegin != NULL); };
		inline Bool_t HasLabel() const { return(fLabel != NULL); };

		inline TGLabel * GetLabel() const { return(fLabel); };
		inline void SetLabelText(const Char_t * Text) { if (fLabel) fLabel->SetText(Text); };
		inline const Char_t * GetLabelText() const { return(fLabel ? fLabel->GetText()->GetString() : "???"); };

		inline void SetEntryBackground(Pixel_t Back) { fCombo->SetBackgroundColor(Back); };
		inline void SetLabelBackground(Pixel_t Back) { if (fLabel) fLabel->SetBackgroundColor(Back); };
		void SetButtonBackground(Pixel_t Back);

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline const Char_t * GetText() const { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbLabelCombo.html&"); };

	protected:
		USE_HEAP();

		Int_t fComboWidth;

		TGComboBox * fCombo;			//!
		TGLabel * fLabel;				//!
		TString fText;					//!

		TGWindow * fClientWindow;

		TMrbLofNamedX fLofEntries;		//! entries

		TGPictureButton * fButtonUp; 	//! button up
		TGPictureButton * fButtonDown;	//! button down
		TGPictureButton * fButtonBegin; //! button begin
		TGPictureButton * fButtonEnd; 	//! button end

	ClassDef(TNGMrbLabelCombo, 1)		// [GraphUtils] A labelled combo box
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelComboLayout
// Purpose:        Layout manager for TNGMrbLabelCombo widget
// Description:    Defines widget layout
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbLabelComboLayout : public TGLayoutManager {

	public:
		TNGMrbLabelComboLayout(TNGMrbLabelCombo * Widget) { fWidget = Widget; }
		virtual void Layout();
		inline TGDimension GetDefaultSize() const { return(fWidget->GetSize()); };

	protected:
		TNGMrbLabelCombo * fWidget;        // pointer to main widget

};

#endif
