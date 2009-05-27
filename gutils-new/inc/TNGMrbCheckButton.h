#ifndef __TNGMrbCheckButton_h__
#define __TNGMrbCheckButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbCheckButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TNGMrbCheckButtonList   -- a composite frame containing
//                                           check buttons
//                 TNGMrbCheckButtonGroup  -- check buttons arranged in
//                                           group frame
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbCheckButton.h,v 1.2 2009-05-27 07:36:49 Marabou Exp $       
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
// Name:           TNGMrbCheckButtonList
// Purpose:        Define a list of check buttons and their behaviour
// Description:    Defines a list of check buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbCheckButtonList: public TGCompositeFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbCheckButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons, Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0,
													TObjArray * LofSpecialButtons = NULL);

		TNGMrbCheckButtonList(const TGWindow * Parent, const Char_t * Label,
													const Char_t * Buttons, Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0,
													TObjArray * LofSpecialButtons = NULL);

		~TNGMrbCheckButtonList() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbCheckButtonList.html&"); };

	ClassDef(TNGMrbCheckButtonList, 1)		// [GraphUtils] A list of check buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbCheckButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Description:    Defines a list of check buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbCheckButtonGroup: public TGGroupFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbCheckButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t ButtonWidth = 0,
													TObjArray * LofSpecialButtons = NULL);

		TNGMrbCheckButtonGroup(const TGWindow * Parent, const Char_t * Label,
													const Char_t * Buttons, Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t ButtonWidth = 0,
													TObjArray * LofSpecialButtons = NULL);

		~TNGMrbCheckButtonGroup() { CLEAR_HEAP(); };				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbRadioButtonGroup.html&"); };

	protected:
		USE_HEAP();

	ClassDef(TNGMrbCheckButtonGroup, 1)		// [GraphUtils] A group of check buttons
};

#endif
