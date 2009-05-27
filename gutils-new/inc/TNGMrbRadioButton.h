#ifndef __TNGMrbRadioButton_h__
#define __TNGMrbRadioButton_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbRadioButton.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbRadioButton.h,v 1.2 2009-05-27 07:36:49 Marabou Exp $       
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
// Name:           TNGMrbRadioButtonList
// Purpose:        Define a list of radio buttons and their behaviour
// Description:    Defines a list of radio buttons.
//                 Buttons will be arranged in a composite frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbRadioButtonList: public TGCompositeFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbRadioButtonList(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0);

		TNGMrbRadioButtonList(const TGWindow * Parent, const Char_t * Label,
													const Char_t * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0);

		~TNGMrbRadioButtonList() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {		// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbRadioButtonList.html&"); };

	ClassDef(TNGMrbRadioButtonList, 1)		// [GraphUtils] A list of radio buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbRadioButtonGroup
// Purpose:        Define a group of radio buttons and their behaviour
// Description:    Defines a list of radio buttons.
//                 Buttons will be arranged in a group frame.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbRadioButtonGroup: public TGGroupFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbRadioButtonGroup(const TGWindow * Parent, const Char_t * Label,
													TMrbLofNamedX * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t ButtonWidth = 0);

		TNGMrbRadioButtonGroup(const TGWindow * Parent, const Char_t * Label,
													const Char_t * Buttons,Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t NofRows = 0, Int_t NofCols = 0,
													Int_t ButtonWidth = 0);

		~TNGMrbRadioButtonGroup() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbRadioButtonGroup.html&"); };

	ClassDef(TNGMrbRadioButtonGroup, 1)		// [GraphUtils] A group of radio buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbYesNoButtonList
// Purpose:        Define a yes/no button and its behaviour
// Description:    Defines a yes/no button.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbYesNoButtonList: public TGCompositeFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbYesNoButtonList(const TGWindow * Parent, const Char_t * Label,			
													Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t Width = 0, Int_t Height = 0, Int_t ButtonWidth = 0);

		~TNGMrbYesNoButtonList() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {		// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbYesNoButtonList.html&"); };

	ClassDef(TNGMrbYesNoButtonList, 1)		// [GraphUtils] A list of radio buttons
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbYesNoButtonGroup
// Purpose:        Define a yes/no button in a group frame
// Description:    Defines a yes/no button in a group frame
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbYesNoButtonGroup: public TGGroupFrame, public TNGMrbButtonFrame {

	public:
		TNGMrbYesNoButtonGroup(const TGWindow * Parent, const Char_t * Label,
													Int_t FrameId,
													TNGMrbProfile * Profile,
													Int_t ButtonWidth = 0);

		~TNGMrbYesNoButtonGroup() {};				// default dtor

		/*virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {			// process mouse clicks
			return(ButtonFrameMessage(MsgId, Param1));
		};*/

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbYesNoButtonGroup.html&"); };

	ClassDef(TNGMrbYesNoButtonGroup, 1)		// [GraphUtils] A group of radio buttons
};
#endif
