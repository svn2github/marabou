#ifndef __TGMrbButtonFrame_h__
#define __TGMrbButtonFrame_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbButtonFrame.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbButtonFrame    -- a composite frame containing buttons
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TString.h"
#include "TGWindow.h"
#include "TGButton.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbSpecialButton
// Purpose:        Declare a special button for check button lists
// Description:    Defines a special button (all, none, default)
//                 to be used in check button lists/groups.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbSpecialButton: public TMrbNamedX {

	public:
		TGMrbSpecialButton( Int_t Index, const Char_t * Name, const Char_t * Title,
							UInt_t Pattern, const Char_t * Picture = NULL) : TMrbNamedX(Index, Name, Title) {
			fPattern = Pattern;
			if (Picture) fPicture = Picture; else fPicture.Resize(0);
		}

		~TGMrbSpecialButton() {};			// dtor

		inline UInt_t GetPattern() { return(fPattern); };
		inline void SetPattern(UInt_t Pattern) { fPattern = Pattern; };
		inline const Char_t * GetPicture() { return(fPicture.Length() > 0 ? fPicture.Data() : NULL); };
		inline void SetPicture(const Char_t * Picture) { fPicture = Picture; };
		
	protected:
		UInt_t fPattern;
		TString fPicture;
		
	ClassDef(TGMrbSpecialButton, 0)				// [GraphUtils] A "special" button
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbButtonFrame
// Purpose:        Base class for lists of buttons
// Description:    A base class for lists of buttons. 
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbButtonFrame: public TGMrbObject {

	public:
		enum EGMrbButtonType		{	kGMrbTextButton 	= BIT(0),
										kGMrbRadioButton	= BIT(1),
										kGMrbCheckButton	= BIT(2),
										kGMrbPictureButton	= BIT(3)
									};

		enum EGMrbButtonFrameType 	{	kGMrbButtonList 	= BIT(4),
										kGMrbButtonGroup	= BIT(5)
									};


	public:
		TGMrbButtonFrame(const TGWindow * Parent, const Char_t * Label, UInt_t ButtonType,
											TMrbLofNamedX * Buttons, Int_t NofCL,
											Int_t Width, Int_t Height,
											TGMrbLayout * FrameGC, TGMrbLayout * LabelGC, TGMrbLayout * ButtonGC,
											UInt_t FrameOptions, UInt_t ButtonOptions);

		virtual ~TGMrbButtonFrame() {};			// dtor

		TGMrbButtonFrame(const TGMrbButtonFrame & f) {};	// default copy ctor

		void SetState(UInt_t Pattern, EButtonState State = kButtonDown);	// set button state
		UInt_t GetActive(); 												// return button state

		void FlipState(UInt_t Pattern);					// flip state
		void UpdateState(UInt_t Pattern);				// update state
		void ClearAll();								// clear all buttons

		void SetButtonWidth(Int_t Width, Int_t ButtonIndex = 0);	// set button width
		Int_t GetButtonWidth(Int_t ButtonIndex = 0);	// get button width
		void JustifyButton(ETextJustification Justify, Int_t ButtonIndex = 0);	// justify button text
		TGButton * GetButton(Int_t ButtonIndex);		// get button by index
		void AddButton(TGButton * Button, TMrbNamedX * ButtonSpecs);	// add a button

		void ChangeButtonBackground(ULong_t Color, Int_t Index = 0); 	// set background color

		Bool_t ButtonFrameMessage(Long_t, Long_t);		// process mouse clocks

		void Associate(const TGWindow * Window);		// redirect button events

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbButtonFrame.html&"); };

	protected:
		void PlaceButtons();						// place buttons in a subframe CxL
		TGMrbSpecialButton * FindSpecialButton(Int_t Index);

	protected:
		const TGWindow * fParent; 						// parent window
		UInt_t fType;								// button type: text, radio, or check, list or group
		TString fLabel; 							// label text
		TMrbLofNamedX fButtons;						// list of buttons: labels, indices, widgets
		Int_t fWidth;								// frame width
		Int_t fHeight;								// frame height
		Int_t fNofCL;								// button grid CxL: number of columns/lines
		TGMrbLayout * fFrameGC;						// frame layout
		TGMrbLayout * fLabelGC;						// label layout
		TGMrbLayout * fButtonGC;					// button layout
		UInt_t fFrameOptions;						// frame options
		UInt_t fButtonOptions;						// button options

		TObjArray * fLofSpecialButtons; 			//! list of special buttons
		
		TGCompositeFrame * fFrame;
		TGClient * fParentClient;
		
	ClassDef(TGMrbButtonFrame, 0)					// [GraphUtils] A frame of buttons
};

#endif
