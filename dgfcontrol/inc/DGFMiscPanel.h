#ifndef __DGFMiscPanel_h__
#define __DGFMiscPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMiscPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFMiscPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFMiscPanel.h,v 1.6 2008-12-29 13:48:24 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>
#include <iomanip>

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"

#include "TMrbLofNamedX.h"
#include "TMrbDGF.h"

#include "TGMrbCheckButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbTextButton.h"
#include "TGMrbPictureButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFocusList.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMiscPanel
// Purpose:        Main frame to take untriggered traces
// Constructors:   
// Description:    A dialog window to take untriggered traces
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFMiscPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFOffsetsActionId 	{	kDGFMiscSetGFLT,
										kDGFMiscClearGFLT,
										kDGFMiscSetCoincWait,
										kDGFMiscSelectAll,
										kDGFMiscSelectNone,
										kDGFMiscSelectColumn
									};
				
	public:
		DGFMiscPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFMiscPanel() { fHeap.Delete(); };

		void SelectModule(Int_t FrameId, Int_t Selection);		// slot methods
		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t StartRamp();

	protected:
		Bool_t SetGFLT(Bool_t SetFlag = kTRUE);
		Bool_t SetCoincWait();
	
	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TGGroupFrame * fActionFrame;
		TGMrbTextButtonList * fActionButtons;
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

	ClassDef(DGFMiscPanel, 0)		// [DGFControl] Miscellaneous
};

#endif
