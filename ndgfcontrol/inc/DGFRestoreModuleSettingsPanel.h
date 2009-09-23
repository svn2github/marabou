#ifndef __DGFRestoreModuleSettingsPanel_h__
#define __DGFRestoreModuleSettingsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFRestoreModuleSettingsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFRestoreModuleSettingsPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
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
// Name:           DGFRestoreModuleSettingsPanel
// Purpose:        Main frame to restore module settings
// Constructors:   
// Description:    A dialog window to restore module settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFRestoreModuleSettingsPanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EDGFRestoreModuleSettingsActionsId 	{	kDGFRestoreModuleSettingsRestore,
														kDGFRestoreModuleSettingsRestorePSA,
														kDGFRestoreModuleSettingsSelectAll,
														kDGFRestoreModuleSettingsSelectNone,
														kDGFRestoreModuleSettingsSelectColumn
													};
				
	public:
		DGFRestoreModuleSettingsPanel(TGCompositeFrame * TabFrame);
		virtual ~DGFRestoreModuleSettingsPanel() { fHeap.Delete(); };

		void SelectModule(Int_t FrameId, Int_t Selection);		// slot methods
		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t LoadDatabase(Bool_t LoadPSA);		// restore dgf settings from file

	protected:
		TList fHeap;								//! list of objects created on heap
		TGGroupFrame * fModules;			 		// module list
		TNGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TNGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TNGMrbPictureButtonList * fAllSelect;
		TGGroupFrame * fActionFrame;
		TNGMrbTextButtonList * fActionButtons;
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TNGMrbFocusList fFocusList;

	ClassDef(DGFRestoreModuleSettingsPanel, 0)		// [DGFControl] Restore settings
};

#endif
