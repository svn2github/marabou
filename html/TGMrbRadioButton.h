#ifndef __TGMrbRadioButton_h__
#define __TGMrbRadioButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbRadioButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbRadioButtonList    -- a composite frame contaoning
//                                            radio buttons
//                 TGMrbRadioButtonGroup   -- a group frame containing
//                                            radio buttons
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

#include "TMrbLofNamedX.h"
#include "TGMrbButtonFrame.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbRadioButtonList
// Purpose:        Define a list of radio buttons and their behaviour
// Description:    Defines a list of radio buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbRadioButtonList: public TGCompositeFrame, public TGMrbButtonFrame {

	public:
		TGMrbRadioButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t NofCL,
													Int_t Width, Int_t Height,
													TGMrbLayout * FrameGC,
													TGMrbLayout * LabelGC = NULL,
													TGMrbLayout * ButtonGC = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = 0);

		~TGMrbRadioButtonList() {};				// default dtor

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {		// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbRadioButtonList.html&"); };

	ClassDef(TGMrbRadioButtonList, 1)		// [GraphUtils] A list of radio buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbRadioButtonGroup
// Purpose:        Define a group of radio buttons and their behaviour
// Description:    Defines a list of radio buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbRadioButtonGroup: public TGGroupFrame, public TGMrbButtonFrame {

	public:
		TGMrbRadioButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t NofCL,
													TGMrbLayout * FrameGC,
													TGMrbLayout * ButtonGC = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = 0);

		~TGMrbRadioButtonGroup() {};				// default dtor

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbRadioButtonGroup.html&"); };

	ClassDef(TGMrbRadioButtonGroup, 1)		// [GraphUtils] A group of radio buttons
};

#endif
