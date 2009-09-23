#ifndef __DGFTraceDisplayPanel_h__
#define __DGFTraceDisplayPanel_h__


//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFTraceDisplayPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFTraceDisplayPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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
// Name:           DGFTraceDisplayPanel
// Purpose:        Main frame to take untriggered traces
// Constructors:   
// Description:    A dialog window to take untriggered traces
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFTraceDisplayPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFTraceDisplayActionId 	{
														kDGFTraceDisplayTraceLength,
														kDGFTraceDisplayXwait,
														kDGFTraceDisplayNormal,
														kDGFTraceDisplayAutoTrig,
														kDGFTraceDisplayStop,
														kDGFTraceDisplaySelectAll,
														kDGFTraceDisplaySelectNone,
														kDGFTraceDisplaySelectColumn
										};
				
	public:
		DGFTraceDisplayPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFTraceDisplayPanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection); 	// slot methods
		void SelectModule(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t StartTrace(Bool_t AutoTrigFlag);
		Bool_t Update(Int_t EntryId);											// update program state on X events
		void MoveFocus(Int_t EntryId);											// move focus to next entry

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TNGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TNGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TNGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TGGroupFrame * fTFrame;
		TNGMrbLabelEntry * fTraceLength;						// trace length
		TGGroupFrame * fXFrame;
		TNGMrbLabelEntry * fXwait;							// wait states
		TNGMrbCheckButtonGroup * fSelectChannel;		//	channel
		TNGMrbTextButtonGroup * fActionFrame;

		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofChannels;				//! channel numbers

		TMrbLofNamedX fLofDGFModuleKeys[kNofClusters];
		
		TNGMrbFocusList fFocusList;

	ClassDef(DGFTraceDisplayPanel, 0)		// [DGFControl] Collect a trace
};

#endif

