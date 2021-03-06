#ifndef __DGFSaveModuleSettingsPanel_h__
#define __DGFSaveModuleSettingsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSaveModuleSettingsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFSaveModuleSettingsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFSaveModuleSettingsPanel.h,v 1.8 2008-12-29 13:48:25 Rudolf.Lutter Exp $       
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
// Name:           DGFSaveModuleSettingsPanel
// Purpose:        Main frame to save module settings
// Constructors:   
// Description:    A dialog window to save module settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFSaveModuleSettingsPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFSaveModuleSettingsActionsId 	{	kDGFSaveModuleSettingsRestore,
													kDGFSaveModuleSettingsSelectAll,
													kDGFSaveModuleSettingsSelectNone,
													kDGFSaveModuleSettingsSelectColumn
												};
				
	public:
		DGFSaveModuleSettingsPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFSaveModuleSettingsPanel() { fHeap.Delete(); };

		void SelectModule(Int_t FrameId, Int_t Selection);		// slot methods
		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t SaveDatabase();						// save dgf settings to file

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGGroupFrame * fActionFrame;
		TGMrbTextButtonList * fActionButtons;
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

	ClassDef(DGFSaveModuleSettingsPanel, 0)		// [DGFControl] Save settings
};

#endif
