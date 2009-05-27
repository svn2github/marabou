#ifndef __TNGMrbPictureButton_h__
#define __TNGMrbPictureButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbPictureButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Classes:        TNGMrbPictureButtonList   -- a composite frame containing
//                                             check buttons
//                 TNGMrbPictureButtonGroup  -- check buttons arranged in
//                                             group frame
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbPictureButton.h,v 1.2 2009-05-27 07:36:49 Marabou Exp $       
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
// Name:           TNGMrbPictureButtonList
// Purpose:        Define a list of check buttons and their behaviour
// Description:    Defines a list of check buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbPictureButtonList: public TGCompositeFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbPictureButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0);

		TNGMrbPictureButtonList(const TGWindow * Parent, const Char_t * Label,
													const Char_t * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0);

		~TNGMrbPictureButtonList() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbPictureButtonList.html&"); };

	ClassDef(TNGMrbPictureButtonList, 1) 	// [GraphUtils] A list of picture buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbPictureButtonGroup
// Purpose:        Define a group of check buttons and their behaviour
// Description:    Defines a list of check buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbPictureButtonGroup: public TGGroupFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbPictureButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t ButtonWidth = 0);

		TNGMrbPictureButtonGroup(const TGWindow * Parent, const Char_t * Label,
													const Char_t * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t ButtonWidth = 0);

		~TNGMrbPictureButtonGroup() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbRadioButtonGroup.html&"); };

	ClassDef(TNGMrbPictureButtonGroup, 1) 	// [GraphUtils] A group of picture buttons
};

#endif
