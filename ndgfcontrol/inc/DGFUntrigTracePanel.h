#ifndef __DGFUntrigTracePanel_h__
#define __DGFUntrigTracePanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFUntrigTracePanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFUntrigTracePanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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

#include "TNGMrbCheckButton.h"
#include "TNGMrbRadioButton.h"
#include "TNGMrbTextButton.h"
#include "TNGMrbPictureButton.h"
#include "TNGMrbLabelEntry.h"
#include "TNGMrbFocusList.h"

#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel
// Purpose:        Main frame to take untriggered traces
// Constructors:   
// Description:    A dialog window to take untriggered traces
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFUntrigTracePanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFUntrigTraceActionId 	{
													kDGFUntrigTraceXwait,
													kDGFUntrigTraceStart,
													kDGFUntrigTraceAbort,
													kDGFUntrigTraceSelectAll,
													kDGFUntrigTraceSelectNone,
													kDGFUntrigTraceSelectColumn
												};
				
	public:
		DGFUntrigTracePanel(TGCompositeFrame * TabFrame);
		virtual ~DGFUntrigTracePanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot methods
		void SelectModule(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t StartTrace();

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TNGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TNGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TNGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TGGroupFrame * fXFrame;
		TNGMrbLabelEntry * fXwait;							// wait states
		TNGMrbCheckButtonGroup * fSelectChannel;		//	channel
		TNGMrbTextButtonGroup * fActionFrame;

		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofChannels;				//! channel numbers

		TMrbLofNamedX fLofDGFModuleKeys[kNofClusters];
		
		TNGMrbFocusList fFocusList;

	ClassDef(DGFUntrigTracePanel, 0)		// [DGFControl] Untriggered trace
};

#endif
