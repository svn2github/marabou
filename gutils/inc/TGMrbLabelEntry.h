#ifndef __TGMrbLabelEntry_h__
#define __TGMrbLabelEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbLabelEntry.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbLabelEntry              -- a frame containing a label and
//                                                 an entry
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbLabelEntry.h,v 1.6 2005-09-06 11:47:22 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGTextEntry.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TGMrbFocusList.h"
#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTextEntry
// Purpose:        
// Description:    
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbTextEntry: public TGTextEntry {

	public:
		TGMrbTextEntry(const TGWindow * Parent, TGTextBuffer * Text, Int_t Id,
								GContext_t Context, FontStruct_t Font, UInt_t Option, ULong_t Back) :
														TGTextEntry(Parent, Text, Id) {
			this->SetFont(Font);
			this->SetBackgroundColor(Back);
			fSendReturnPressed = kTRUE;
		};

		~TGMrbTextEntry() {};
		
		inline void SendReturnPressedOnButtonClick(Bool_t Flag) { fSendReturnPressed = Flag; };
		inline void SendSignal() {				// ReturnPressed() or TextChanged() depending on ReturnPressed flag
			if (fSendReturnPressed) TGTextEntry::ReturnPressed(); else TGTextEntry::TextChanged();
		};
		virtual void TextChanged(const Char_t * Text) {};	// no operation, action handled by SendSignal()
		
		virtual Bool_t HandleButton(Event_t * Event);

	protected:
		Bool_t fSendReturnPressed;

	ClassDef(TGMrbTextEntry, 1) 	// [GraphUtils] A text entry
};
			
//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelEntry
// Purpose:        Define a labelled text entry
// Description:    Defines a label together with a text entry field.
//                 an Up/Down button may be appended to increment entry data.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbLabelEntry: public TGCompositeFrame, public TGMrbObject {

	public:
		enum EGMrbEntryButton	{	kGMrbEntryButtonUp,
									kGMrbEntryButtonDown,
									kGMrbEntryButtonBegin,
									kGMrbEntryButtonEnd
								};

		enum EGMrbEntryType 	{	kGMrbEntryTypeChar,
									kGMrbEntryTypeInt,
									kGMrbEntryTypeDouble,
									kGMrbEntryTypeCharInt,
									kGMrbEntryTypeCharDouble
								};

	public:
		TGMrbLabelEntry(const TGWindow * Parent, const Char_t * Label,				// ctor
									Int_t BufferSize, Int_t EntryId,
									Int_t Width, Int_t Height,
									Int_t EntryWidth,
									TGMrbLayout * FrameGC,
									TGMrbLayout * LabelGC = NULL,
									TGMrbLayout * EntryGC = NULL,
									TGMrbLayout * UpDownBtnGC = NULL,
									Bool_t BeginEndBtns = kFALSE,
									TMrbNamedX * Action = NULL,
									TGMrbLayout * ActionGC = NULL,
									UInt_t FrameOptions = kHorizontalFrame,
									UInt_t EntryOptions = kSunkenFrame | kDoubleBorder);

		~TGMrbLabelEntry() {};				// default dtor

		void SetType(EGMrbEntryType EntryType, Int_t Width = 0, Char_t PadChar = ' ', Int_t BaseOrPrec = 0);
		inline void SetIncrement(Double_t Increment) { fIncrement = Increment; };
		Bool_t SetRange(Double_t LowerLimit, Double_t UpperLimit);
		Bool_t WithinRange() const;
		Bool_t CheckRange(Double_t Value, Bool_t Verbose = kFALSE, Bool_t Popup = kFALSE) const;
		Bool_t RangeToBeChecked() const;

		inline void AddToFocusList(TGMrbFocusList * FocusList) { fFocusList = FocusList; fFocusList->Add(fEntry); };
		inline TGMrbFocusList * GetFocusList() const { return(fFocusList); };

		inline TGMrbTextEntry * GetEntry() const { return(fEntry); };
		inline TGPictureButton * GetUpButton() const { return(fUp); };
		inline TGPictureButton * GetDownButton() const { return(fDown); };
		inline TGPictureButton * GetBeginButton() const { return(fBegin); };
		inline TGPictureButton * GetEndButton() const { return(fEnd); };
		inline TGTextButton * GetActionButton() const { return(fAction); };

		void UpDownButtonEnable(Bool_t Flag = kTRUE);			// enable/disable up/down buttons
		void ActionButtonEnable(Bool_t Flag = kTRUE);			// enable/disable action button

		const Char_t * GetText();								// get text, update tooltip if necessary
		Int_t GetText2Int();									// get text, convert to integer
		Double_t GetText2Double();								// ... convert to integer
		void SetText(const Char_t * Text);						// set text, update tooltip if necessary
		void SetText(Int_t Value);								// ... convert from integer
		void SetText(Double_t Value);							// ... from double

		inline TGLabel * GetLabel() { return(fLabel); };

		void ShowToolTip(Bool_t Flag = kTRUE, Bool_t ShowRange = kFALSE);

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Associate(const TGWindow * Window) { fEntry->Associate(Window); };	// where to go if text field data change

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbLabelEntry.html&"); };

	protected:
		void CreateToolTip();

	protected:
		TGLabel * fLabel;				//! label
		TGMrbTextEntry * fEntry;		//! entry widget
		TGPictureButton * fUp; 			//! button ">", increment
		TGPictureButton * fDown;		//! button "<", decrement
		TGPictureButton * fBegin; 		//! button "<<", begin
		TGPictureButton * fEnd; 		//! button ">>", end
		TGTextButton * fAction; 		//! textbutton

		EGMrbEntryType fType;			// entry type: char, char + int, ...
		Int_t fWidth;					// number of digits to be displayed
		Int_t fBase;					// numerical base if of type int or charint (2, 8, 10, 16)
		Int_t fPrecision;				// precision if type double
		Char_t fPadChar;				// character ot pad empty fields
		Double_t fLowerLimit;			// lower limit
		Double_t fUpperLimit;			// upper limit
		Double_t fIncrement;			// increment

		Bool_t fShowToolTip;			// kTRUE if tooltip has to be shown
		Bool_t fShowRange;				// kTRUE if range has to be shown in tooltip

	TGMrbFocusList * fFocusList;	//! list of focusable entries

	ClassDef(TGMrbLabelEntry, 1)		// [GraphUtils] An entry field with a label
};

#endif
