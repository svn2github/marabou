#ifndef __DGFRestoreModuleSettingsPanel_h__
#define __DGFRestoreModuleSettingsPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFRestoreModuleSettingsPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFRestoreModuleSettingsPanel.h,v 1.11 2008-12-29 13:48:25 Rudolf.Lutter Exp $       
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
		TGMrbCheckButtonList * fCluster[kNofClusters];
		TGHorizontalFrame * fGroupFrame;
		TGMrbPictureButtonList * fGroupSelect[kNofModulesPerCluster];
		TGMrbPictureButtonList * fAllSelect;
		TGGroupFrame * fActionFrame;
		TGMrbTextButtonList * fActionButtons;
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX fLofModuleKeys[kNofClusters];
		
		TGMrbFocusList fFocusList;

	ClassDef(DGFRestoreModuleSettingsPanel, 0)		// [DGFControl] Restore settings
};

#endif
