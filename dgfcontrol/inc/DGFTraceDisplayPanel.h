#ifndef __DGFTraceDisplayPanel_h__
#define __DGFTraceDisplayPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFTraceDisplayPanel
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
														kDGFTraceDisplayXwait,
														kDGFTraceDisplayNormal,
														kDGFTraceDisplayAutoTrig,
														kDGFTraceDisplayAbort,
														kDGFTraceDisplaySelectAll,
														kDGFTraceDisplaySelectNone,
														kDGFTraceDisplaySelectColumn
										};
				
	public:
		DGFTraceDisplayPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFTraceDisplayPanel() { fHeap.Delete(); };

		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

	protected:
		Bool_t StartTrace(Bool_t AutoTrigFlag);
		Bool_t Update(Int_t EntryId);											// update program state on X events
		void MoveFocus(Int_t EntryId);											// move focus to next entry

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGHorizontalFrame * fHFrame;
		TGGroupFrame * fTFrame;
		TGMrbLabelEntry * fTraceLength;						// trace length
		TGGroupFrame * fXFrame;
		TGMrbLabelEntry * fXwait;							// wait states
		TGMrbCheckButtonGroup * fSelectChannel;		//	channel
		TGMrbTextButtonGroup * fActionFrame;

		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofChannels;				//! channel numbers

		TMrbLofNamedX fLofDGFModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

	ClassDef(DGFTraceDisplayPanel, 0)		// [DGFControl] Collect a trace
};

#endif
