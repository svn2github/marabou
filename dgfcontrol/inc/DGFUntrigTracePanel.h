#ifndef __DGFUntrigTracePanel_h__
#define __DGFUntrigTracePanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFUntrigTracePanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

using namespace std;

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
#include "TGMrbLofKeyBindings.h"

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
													kDGFUntrigTraceSelectAll,
													kDGFUntrigTraceSelectNone,
													kDGFUntrigTraceSelectColumn
												};
				
	public:
		DGFUntrigTracePanel(TGCompositeFrame * TabFrame);
		virtual ~DGFUntrigTracePanel() { fHeap.Delete(); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		Bool_t StartTrace();

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TGGroupFrame * fXFrame;
		TGMrbLabelEntry * fXwait;							// wait states
		TGMrbCheckButtonGroup * fSelectChannel;		//	channel
		TGMrbTextButtonGroup * fActionFrame;

		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofChannels;				//! channel numbers

		TMrbLofNamedX fLofDGFModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

	ClassDef(DGFUntrigTracePanel, 0)		// [DGFControl] Untriggered trace
};

#endif
