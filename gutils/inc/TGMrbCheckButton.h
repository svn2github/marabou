#ifndef __TGMrbCheckButton_h__
#define __TGMrbCheckButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbCheckButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbCheckButtonList   -- a composite frame containing
//                                           check buttons
//                 TGMrbCheckButtonGroup  -- check buttons arranged in
//                                           group frame
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
// Name:           TGMrbCheckButtonList
// Purpose:        Define a list of check buttons and their behaviour
// Description:    Defines a list of check buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbCheckButtonList: public TGCompositeFrame, public TGMrbButtonFrame {

	public:
		TGMrbCheckButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t NofCL,
													Int_t Width, Int_t Height,
													TGMrbLayout * FrameGC,
													TGMrbLayout * LabelGC = NULL,
													TGMrbLayout * ButtonGC = NULL,
													TObjArray * LofSpecialButtons = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = 0);

		~TGMrbCheckButtonList() {};				// default dtor

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbCheckButtonList.html&"); };

	ClassDef(TGMrbCheckButtonList, 1)		// [GraphUtils] A list of check buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbCheckButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Description:    Defines a list of check buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbCheckButtonGroup: public TGGroupFrame, public TGMrbButtonFrame {

	public:
		TGMrbCheckButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t NofCL,
													TGMrbLayout * FrameGC,
													TGMrbLayout * ButtonGC = NULL,
													TObjArray * LofSpecialButtons = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = 0);

		~TGMrbCheckButtonGroup() {};				// default dtor

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbRadioButtonGroup.html&"); };

	ClassDef(TGMrbCheckButtonGroup, 1)		// [GraphUtils] A group of check buttons
};

#endif
