#ifndef __TNGMrbButtonFrame_h__
#define __TNGMrbButtonFrame_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbButtonFrame.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TNGMrbButtonFrame    -- a composite frame containing buttons
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbButtonFrame.h,v 1.3 2009-05-27 07:36:48 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TArrayI.h"
#include "TString.h"
#include "TGWindow.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGTableLayout.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TNGMrbProfile.h"

#include "UseHeap.h"

#include "RQ_OBJECT.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbSpecialButton
// Purpose:        Declare a special button for check button lists
// Description:    Defines a special button (all, none, default)
//                 to be used in check button lists/groups.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbSpecialButton: public TMrbNamedX {

	

	public:
		TNGMrbSpecialButton( Int_t Index, const Char_t * Name, const Char_t * Title,
							UInt_t Pattern, const Char_t * Picture = NULL) : TMrbNamedX(Index, Name, Title) {
			fPattern = Pattern;
			if (Picture) fPicture = Picture; else fPicture.Resize(0);
		}

		~TNGMrbSpecialButton() { CLEAR_HEAP(); };			// dtor

		inline UInt_t GetPattern() const { return(fPattern); };
		inline void SetPattern(UInt_t Pattern) { fPattern = Pattern; };
		inline const Char_t * GetPicture() const { return(fPicture.Length() > 0 ? fPicture.Data() : NULL); };
		inline void SetPicture(const Char_t * Picture) { fPicture = Picture; };
		
	protected:
		USE_HEAP();

		UInt_t fPattern;
		TString fPicture;
		
	ClassDef(TNGMrbSpecialButton, 0)				// [GraphUtils] A "special" button
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrame
// Purpose:        Base class for lists of buttons
// Description:    A base class for lists of buttons. 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbButtonFrame {

RQ_OBJECT("TNGMrbButtonFrame")

	public:
		enum EGMrbButtonPad			{	kGMrbButtonPadW  			= 2,
										kGMrbButtonPadH				= 2
									};
		

	public:
		TNGMrbButtonFrame(const TGWindow * Parent, UInt_t ButtonType,
											TMrbLofNamedX * Buttons,Int_t FrameId, TNGMrbProfile * Profile, Int_t NofRows, 
											Int_t NofCols, Int_t Width, Int_t Height, Int_t ButtonWidth);

		TNGMrbButtonFrame(const TGWindow * Parent, UInt_t ButtonType,
											const Char_t * Buttons,Int_t FrameId, TNGMrbProfile * Profile, Int_t NofRows, Int_t 												NofCols, Int_t Width, Int_t Height, Int_t ButtonWidth);

		virtual ~TNGMrbButtonFrame() { CLEAR_HEAP(); };			// dtor

		TNGMrbButtonFrame(const TNGMrbButtonFrame & f) {};	// default copy ctor

		//void SetState(UInt_t Pattern, EButtonState State = kButtonDown);	// set button state
		void SetState(UInt_t Pattern, EButtonState State = kButtonDown, Bool_t Emit=kTRUE);
		UInt_t GetActive() const; 												// return button state

		void FlipState(UInt_t Pattern);					// flip state
		void UpdateState(UInt_t Pattern);				// update state
		void ClearAll();								// clear all buttons

		inline TGCompositeFrame * GetFrame() { return(fFrame); };

		inline Int_t GetWidth() { return(fWidth); };
		inline Int_t GetHeight() { return(fHeight); };
		inline Int_t GetButtonWidth() { return(fButtonWidth); };
		inline Int_t GetButtonHeight() { return(fButtonHeight); };
		inline Int_t GetNofRows() { return(fNofRows); };
		inline Int_t GetNofCols() { return(fNofCols); };

		inline Bool_t HasLabel() { return(fLabel != NULL); };
		inline TGLabel * GetLabel() { return(fLabel); };
		inline const Char_t * GetText() { return(fLabelText.Data()); };

		void SetButtonWidth(Int_t Width, Int_t ButtonIndex = -1);	// set button width
		Int_t GetButtonWidth(Int_t ButtonIndex = -1) const; 		// get button width
		void JustifyButton(ETextJustification Justify, Int_t ButtonIndex);	// justify button text
		TGButton * GetButton(Int_t ButtonIndex) const;		// get button by index
		//void AddButton(TGButton * Button, TMrbNamedX * ButtonSpecs);	// add a button

		inline Int_t GetNofButtons() { return(fNofButtons); };
		inline Int_t GetNofSpecialButtons() { return(fNofSpecialButtons); };

		inline UInt_t GetButtonType() const { return(fType); };
		inline UInt_t GetButtonOptions() const { return(fButtonOptions); };
		inline UInt_t GetFrameOptions() const { return(fFrameOptions); };

		inline Bool_t HasSpecialButtons() { return(fNofSpecialButtons > 0); };

		void ChangeButtonBackground(ULong_t Color, Int_t Index = 0); 	// set background color

		//Bool_t ButtonFrameMessage(Long_t, Long_t);		// process mouse clocks

		//void Associate(const TGWindow * Window);		// redirect button events

		inline TGCompositeFrame * GetFrame() const { return(fFrame); };
		inline const TGWindow * GetParent() const { return(fParent); };
		inline TMrbLofNamedX * GetLofButtons() { return(&fLofButtons); };
		inline TObjArray * GetLofSpecialButtons() { return(fLofSpecialButtons); };

		void SetIndivColumnWidth(Bool_t Flag = kTRUE);
		inline Bool_t HasIndivColumnWidth() { return(fIndivColumnWidth); };
		TArrayI * GetColWidth() { return(&fColWidth); };

		inline void SetPadLeft(Int_t Points) { fPadLeft = Points; fPadLeftFrac = -1.; };
		inline void SetPadLeftFrac(Double_t Frac) { fPadLeft = -1; fPadLeftFrac = Frac; };
		Int_t GetPadLeft(Int_t Points);

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbButtonFrame.html&"); };

		
		void ButtonPressed(Int_t Button);	// slot methods	
		void ButtonPressed(Int_t FrameId, Int_t Button); 	//*SIGNAL*

		inline void CheckButtonClicked(Int_t Button) { this->UpdateState((UInt_t) Button); }; //slotmethods
		inline void RadioButtonClicked(Int_t Button) { this->SetState((UInt_t) Button, kButtonDown, kTRUE); };
		inline void TextButtonClicked(Int_t Button)  { this->UpdateState((UInt_t) Button); };
		inline void PictureButtonClicked(Int_t Button) { this->UpdateState((UInt_t) Button); };
		

		

	protected:
		void CreateButtons();
		TNGMrbSpecialButton * FindSpecialButton(Int_t Index);
		

	protected:
		USE_HEAP();
		const TGWindow * fParent; 					// parent window
		UInt_t fType;								// button type: text, radio, or check, list or group
		TGLabel * fLabel;							// label (button list only)
		TString fLabelText; 						// text: label (button list) or tab (button group)
		TMrbLofNamedX fLofButtons;						// list of buttons: labels, indices, widgets
		TNGMrbGContext * fLabelGC;					// graphics context (label)
		TNGMrbGContext * fButtonGC;					// ... (button)
		UInt_t fFrameOptions;						// frame options
		UInt_t fButtonOptions;						// button options

		Int_t fWidth;								// frame width
		Int_t fHeight;								// frame height
		Int_t fButtonWidth; 						// (max) button width
		Int_t fButtonHeight; 						// ... height
		Int_t fNofRows;								// number of rows
		Int_t fNofCols;								// ... columns
		Int_t fNofButtons;							// number of (normal) buttons
		Int_t fNofSpecialButtons;					// ... special buttons
		Int_t fFrameId;
		Bool_t fIndivColumnWidth;					// kTRUE if column widhts to be calculated individually
		Int_t fPadLeft; 							// padding from left (points)
		Double_t fPadLeftFrac;						// padding left:right (fraction)

		TObjArray * fLofSpecialButtons; 			//! list of special buttons
		
		TArrayI fColWidth;							// individual column widths
		TArrayI fLofButtonWidths;					// list of button widths

		TGCompositeFrame * fFrame;
		TGClient * fFrameClient;
		
		TNGMrbProfile * fProfile;					// graphics profile
		TGTableLayout * tabLayout;

	ClassDef(TNGMrbButtonFrame, 0)					// [GraphUtils] A frame of buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbButtonFrameLayout
// Purpose:        Layout manager for TNGMrbButtonFrame widget
// Description:    Defines widget layout
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbButtonFrameLayout : public TGLayoutManager {

	public:
		TNGMrbButtonFrameLayout(TNGMrbButtonFrame * Widget) {
			fWidget = Widget;
		}
		virtual void Layout();
		inline TGDimension GetDefaultSize() const { return(fWidget->GetFrame()->GetSize()); };

	protected:
		TNGMrbButtonFrame * fWidget; 		// pointer to main widget
};

#endif
