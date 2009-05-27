#ifndef __TNGMrbTextButton_h__
#define __TNGMrbTextButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbTextButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TNGMrbTextButtonList       -- a composite frame containing
//                                              normal push buttons
//                 TNGMrbTextButtonGroup      -- a group frame containing
//                                              normal push buttons
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbTextButton.h,v 1.2 2009-05-27 07:36:49 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TGFrame.h"

#include "TMrbLofNamedX.h"
#include "TNGMrbButtonFrame.h"
#include "TNGMrbProfile.h"

#include "UseHeap.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbTextButtonList
// Purpose:        Define a list of text buttons and their behaviour
// Description:    Defines a list of text buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbTextButtonList: public TGCompositeFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbTextButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons,
													Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0);

		TNGMrbTextButtonList(const TGWindow * Parent, const Char_t * Label,
													const Char_t * Buttons,
													Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0);

		~TNGMrbTextButtonList() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t MsgParm1, Long_t MsgParam2) {	// process mouse clicks
			return(ButtonFrameMessage(MsgId, MsgParm1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbTextButtonList.html&"); };

	ClassDef(TNGMrbTextButtonList, 1)		// [GraphUtils] A list of text buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbTextButtonGroup
// Purpose:        Define a group of text buttons and their behaviour
// Description:    Defines a list of text buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbTextButtonGroup: public TGGroupFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbTextButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons,
													Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0, Int_t ButtonWidth = 0);

		TNGMrbTextButtonGroup(const TGWindow * Parent, const Char_t * Label,
													const Char_t * Buttons,
													Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0, Int_t ButtonWidth = 0);

		~TNGMrbTextButtonGroup() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {		// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbTextButtonGroup.html&"); };

	ClassDef(TNGMrbTextButtonGroup, 1)		// [GraphUtils] A group of text buttons
};

#endif
