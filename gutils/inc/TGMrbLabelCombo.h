#ifndef __TGMrbLabelCombo_h__
#define __TGMrbLabelCombo_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbLabelCombo.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbLabelCombo     -- a frame containing a label and
//                                        a combo box
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGComboBox.h"

#include "TMrbLofNamedX.h"
#include "TGMrbObject.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLabelCombo
// Purpose:        Define a labelled combo box
// Description:    Defines a label together with a combo box
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbLabelCombo: public TGCompositeFrame, public TGMrbObject {

	public:
		enum EGMrbEntryButton	{	kGMrbComboButtonUp,
									kGMrbComboButtonDown,
									kGMrbComboButtonBegin,
									kGMrbComboButtonEnd
								};

	public:
		TGMrbLabelCombo(const TGWindow * Parent, const Char_t * Label,					// ctor
							TMrbLofNamedX * Entries,
							Int_t ComboId, Int_t Selected,
							Int_t Width, Int_t Height, Int_t ComboWidth,
							TGMrbLayout * FrameGC,
							TGMrbLayout * LabelGC = NULL,
							TGMrbLayout * ComboGC = NULL,
							TGMrbLayout * UpDownBtnGC = NULL,
							Bool_t BeginEndBtns = kFALSE,
							UInt_t FrameOptions = kHorizontalFrame,
							UInt_t ComboOptions = kHorizontalFrame|kSunkenFrame|kDoubleBorder);

		~TGMrbLabelCombo() {};														// default dtor

		Bool_t AddEntries(TMrbLofNamedX * Entries); 								// add entries to the box
		
		inline TGComboBox * GetComboBox() { return(fCombo); };

		inline TGPictureButton * GetUpButton() { return(fUp); };
		inline TGPictureButton * GetDownButton() { return(fDown); };
		inline TGPictureButton * GetBeginButton() { return(fBegin); };
		inline TGPictureButton * GetEndButton() { return(fEnd); };

		void UpDownButtonEnable(Bool_t Flag = kTRUE);			// enable/disable up/down buttons

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline void Associate(const TGWindow * Window) { fCombo->Associate(Window); };	// where to go if combobox
																					// selection changes
		inline const Char_t * GetText() { return(fText.Data()); };					// return text field data
		inline void SetText(const Char_t * Text) { fText = Text; }; 				// set text field

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbLabelCombo.html&"); };

	protected:
		TGComboBox * fCombo;			//!
		TString fText;					//!

		TMrbLofNamedX fEntries;			//! entries
		TGPictureButton * fUp; 			//! button ">", increment
		TGPictureButton * fDown;		//! button "<", decrement
		TGPictureButton * fBegin; 		//! button "<<", begin
		TGPictureButton * fEnd; 		//! button ">>", end

	ClassDef(TGMrbLabelCombo, 1)		// [GraphUtils] A labelled combo box
};

#endif
