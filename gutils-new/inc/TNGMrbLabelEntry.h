#ifndef __TNGMrbLabelEntry_h__
#define __TNGMrbLabelEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbLabelEntry.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TNGMrbLabelEntry              -- a frame containing a label and
//                                                 an entry
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbLabelEntry.h,v 1.1 2009-03-27 09:39:35 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGLayout.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TGNumberEntry.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TNGMrbFocusList.h"
#include "TNGMrbProfile.h"

#include "UseHeap.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry
// Purpose:        Define a labelled text entry
// Description:    Defines a label together with a text entry field.
//                 an Up/Down button may be appended to increment entry data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbLabelEntry: public TGCompositeFrame {

	public:
		enum EGMrbEntryButton	{	kGMrbEntryButtonUp = 1,
									kGMrbEntryButtonDown,
									kGMrbEntryButtonEnd,
									kGMrbEntryButtonBegin,
									kGMrbEntryButtonAction
								};

		enum EGMrbEntryOption	{	kGMrbEntryIsText				= 0,
									kGMrbEntryIsNumber				= 0x1 << 0,
									kGMrbEntryHasUpDownButtons	 	= 0x1 << 1,
									kGMrbEntryHasBeginEndButtons 	= 0x1 << 2
								};

		enum EGMrbEntryPad		{	kGMrbEntryPadW				= 5,
									kGMrbEntryPadH				= 1
								};

		enum EGMrbEntryStyle	{	kGMrbEntryInt,
									kGMrbEntryReal,
									kGMrbEntryTime,
									kGMrbEntryDate,
									kGMrbEntryDegree
								};

	public:
		TNGMrbLabelEntry(	const TGWindow * Parent,
							const Char_t * Label,
							TNGMrbProfile * Profile,
							Int_t EntryId, Int_t Width, Int_t Height, Int_t EntryWidth = 0,
							UInt_t EntryOptions = kGMrbEntryIsNumber,
							TMrbNamedX * Action = NULL);


		~TNGMrbLabelEntry() { CLEAR_HEAP(); };		// default dtor

		inline void SetNumber(Double_t Value) { if (this->IsNumberEntry()) fNumberEntryField->SetNumber(Value); };
		inline void SetIntNumber(Int_t Value) { if (this->IsNumberEntry()) fNumberEntryField->SetNumber((Long_t) Value); };
		inline void SetTime(Int_t Hour, Int_t Min, Int_t Sec) { if (this->IsNumberEntry()) fNumberEntryField->SetTime(Hour, Min, Sec); };
		inline void SetDate(Int_t Year, Int_t Month, Int_t Day) { if (this->IsNumberEntry()) fNumberEntryField->SetDate(Year, Month, Day); };
		inline void SetText(const Char_t * Text) { if (this->IsNumberEntry()) fNumberEntryField->SetText(Text); else fTextEntry->SetText(Text); };
		inline void SetTextForced(const Char_t * Text) { fTextEntry->SetText(Text); };
		inline void SetFont(FontStruct_t Font) { if (this->IsNumberEntry()) fNumberEntryField->SetFont(Font); else fTextEntry->SetFont(Font); };
		void SetHexNumber(UInt_t Value);
		void SetOctNumber();
		void SetBinNumber();

		void SetTimeLimit(TGNumberEntry::ELimit Limit = TGNumberEntry::kNELLimitMin, Int_t Hour = 0, Int_t Min = 0, Int_t Sec = 0);
		void SetDateLimit(TGNumberEntry::ELimit Limit = TGNumberEntry::kNELLimitMin, Int_t Year = 0, Int_t Month = 0, Int_t Day = 0);
		void SetDegree(Int_t Deg = 0, Int_t Min = 0, Int_t Sec = 0);
		void SetDegreeLimit(TGNumberEntry::ELimit Limit = TGNumberEntry::kNELLimitMin, Int_t Deg = 0, Int_t Min = 0, Int_t Sec = 0);

		inline void SetEntryBackground(Pixel_t Back) { if (this->IsNumberEntry()) fNumberEntryField->SetBackgroundColor(Back); fTextEntry->SetBackgroundColor(Back); };
		inline void SetLabelBackground(Pixel_t Back) { if (fLabel) fLabel->SetBackgroundColor(Back); };
		void SetButtonBackground(Pixel_t Back);

		inline Double_t GetNumber() const { return(this->IsNumberEntry() ? fNumberEntryField->GetNumber() : 0.); }; 
		inline Int_t GetIntNumber() const { return(this->IsNumberEntry() ? (Int_t) fNumberEntryField->GetIntNumber() : 0); };
		inline void GetTime(Int_t & Hour, Int_t & Min, Int_t & Sec) const { if (this->IsNumberEntry()) fNumberEntryField->GetTime(Hour, Min, Sec); }; 
		inline void GetDate(Int_t & Year, Int_t & Month, Int_t & Day) const{ if (this->IsNumberEntry()) fNumberEntryField->GetDate(Year, Month, Day); };
		inline UInt_t GetHexNumber() const { return(this->IsNumberEntry() ? (UInt_t) fNumberEntryField->GetHexNumber() : 0); }; 
		inline const Char_t * GetText() { return(fTextEntry->GetText()); };

		void SetFormat(TGNumberEntry::EStyle Style, TGNumberEntry::EAttribute Attr = TGNumberEntry::kNEAAnyNumber);
		void SetLimits(TGNumberEntry::ELimit Limits = TGNumberEntry::kNELNoLimits, Double_t Min = 0, Double_t Max = 1);
		inline void SetState(Bool_t State) { if (this->IsNumberEntry()) { if (fNumberEntry) fNumberEntry->SetState(State); else fNumberEntryField->SetState(State); }; };
		inline void SetLogStep(Bool_t On = kTRUE) { if (this->IsNumberEntry()) fNumberEntryField->SetLogStep(On); };
		inline UInt_t GetNumStyle() const { return(this->IsNumberEntry() ? fNumberEntryField->GetNumStyle() : 0xffffffff); };
		inline UInt_t GetNumAttr() const { return(this->IsNumberEntry() ? (UInt_t) fNumberEntryField->GetNumAttr() : 0xffffffff); };
		inline UInt_t GetNumLimits() const { return(this->IsNumberEntry() ? (UInt_t) fNumberEntryField->GetNumLimits() : 0xffffffff); };
		inline Double_t GetNumMin() const { return(this->IsNumberEntry() ? fNumberEntryField->GetNumMin() : 0.); };
		inline Double_t GetNumMax() const { return(this->IsNumberEntry() ? fNumberEntryField->GetNumMax() : 0.); };
		inline Bool_t IsLogStep() const { return(this->IsNumberEntry() ? fNumberEntryField->IsLogStep() : kFALSE); };

		inline Int_t GetEntryWidth() { return(fEntryWidth); };

		void UpDownButtonEnable(Bool_t Flag = kTRUE);			// enable/disable up/down button
		void BeginEndButtonEnable(Bool_t Flag = kTRUE);			// enable/disable begin/end button
		void ActionButtonEnable(Bool_t Flag = kTRUE);			// enable/disable action button

		inline Bool_t IsNumberEntry() const { return(fNumberEntryField != NULL); };
		inline Bool_t HasUpDownButtons() const { return(fNumberEntry != NULL); };
		inline Bool_t HasBeginEndButtons() const { return(fButtonBegin != NULL); };
		inline Bool_t HasActionButton() const { return(fAction != NULL); };
		inline Bool_t HasLabel() const { return(fLabel != NULL); };

		inline TGLabel * GetLabel() const { return(fLabel); };
		inline void SetLabelText(const Char_t * Text) { if (fLabel) fLabel->SetText(Text); };
		inline const Char_t * GetLabelText() const { return(fLabel ? fLabel->GetText()->GetString() : "???"); };

		inline TGButton * GetButtonUp() { return(this->IsNumberEntry() ? fNumberEntry->GetButtonUp() : NULL); };
		inline TGButton * GetButtonDown() { return(this->IsNumberEntry() ? fNumberEntry->GetButtonDown() : NULL); };
		inline TGPictureButton * GetButtonBegin() { return(fButtonBegin); };
		inline TGPictureButton * GetButtonEnd() { return(fButtonEnd); };
		inline TGTextButton * GetActionButton() { return(fAction); };

		inline TGNumberEntry * GetNumberEntry() { return(fNumberEntry); };
		inline TGNumberEntryField * GetNumberEntryField() { return(fNumberEntryField); };
		inline TGTextEntry * GetTextEntry() { return(fTextEntry); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Associate(const TGWindow * Window) {		// where to go if text field data change
			this->IsNumberEntry() ?	fNumberEntryField->Associate(Window) : fTextEntry->Associate(Window);
		};
		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbLabelEntry.html&"); };

		inline void AddToFocusList(TNGMrbFocusList * FocusList) {
			fFocusList = FocusList;
			if (this->IsNumberEntry()) fFocusList->Add(fNumberEntryField); else fFocusList->Add(fTextEntry);
		};
		inline TNGMrbFocusList * GetFocusList() const { return(fFocusList); };

	protected:
		Bool_t CheckRange(Int_t Value, Int_t LowerLim, Int_t UpperLim) const;
		Bool_t CheckStyle(TGNumberEntry::EStyle Style, EGMrbEntryStyle StyleGroup) const;

	protected:
		USE_HEAP();

		Int_t fEntryWidth;				// width of numeric entry field (+ button width)
		TGLabel * fLabel;				//! entry label
		TGNumberEntry * fNumberEntry;			//! entry widget (numbers + buttons)
		TGNumberEntryField * fNumberEntryField; //! ... (numbers)
		TGTextEntry * fTextEntry;				//! ... (text)
		TGTextButton * fAction; 		//! textbutton
		TNGMrbFocusList * fFocusList;	//! list of focusable entries

		TGPictureButton * fButtonBegin;	//! begin/end buttons
		TGPictureButton * fButtonEnd; 	//!

	ClassDef(TNGMrbLabelEntry, 1)		// [GraphUtils] An entry field with a label
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntryLayout
// Purpose:        Layout manager for TNGMrbLabelEntry widget
// Description:    Defines widget layout
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbLabelEntryLayout : public TGLayoutManager {

	public:
		TNGMrbLabelEntryLayout(TNGMrbLabelEntry * Widget) { fWidget = Widget; }
		virtual void Layout();
		inline TGDimension GetDefaultSize() const { return(fWidget->GetSize()); };

	protected:
		TNGMrbLabelEntry * fWidget;        // pointer to main widget

};

#endif
