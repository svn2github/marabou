#ifndef __TGMrbTextButton_h__
#define __TGMrbTextButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbTextButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TGMrbTextButtonList       -- a composite frame containing
//                                              normal push buttons
//                 TGMrbTextButtonGroup      -- a group frame containing
//                                              normal push buttons
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbTextButton.h,v 1.3 2004-09-28 13:47:33 rudi Exp $       
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
// Name:           TGMrbTextButtonList
// Purpose:        Define a list of text buttons and their behaviour
// Description:    Defines a list of text buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbTextButtonList: public TGCompositeFrame, public TGMrbButtonFrame {

	public:
		TGMrbTextButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t NofCL,
													Int_t Width, Int_t Height,
													TGMrbLayout * FrameGC,
													TGMrbLayout * LabelGC = NULL,
													TGMrbLayout * ButtonGC = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = kRaisedFrame | kDoubleBorder);

		~TGMrbTextButtonList() {};				// default dtor

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t MsgParm1, Long_t MsgParam2) {	// process mouse clicks
			return(ButtonFrameMessage(MsgId, MsgParm1));
		};

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbTextButtonList.html&"); };

	ClassDef(TGMrbTextButtonList, 1)		// [GraphUtils] A list of text buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbTextButtonGroup
// Purpose:        Define a group of text buttons and their behaviour
// Description:    Defines a list of text buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbTextButtonGroup: public TGGroupFrame, public TGMrbButtonFrame {

	public:
		TGMrbTextButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t NofCL,
													TGMrbLayout * FrameGC,
													TGMrbLayout * ButtonGC = NULL,
													UInt_t FrameOptions = kHorizontalFrame,
													UInt_t ButtonOptions = kRaisedFrame | kDoubleBorder);

		~TGMrbTextButtonGroup() {};				// default dtor

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {		// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbTextButtonGroup.html&"); };

	ClassDef(TGMrbTextButtonGroup, 1)		// [GraphUtils] A group of text buttons
};

#endif
