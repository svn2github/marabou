#ifndef __VMESis3302SaveRestorePanel_h__
#define __VMESis3302SaveRestorePanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302SaveRestorePanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          VMESis3302SaveRestorePanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302SaveRestorePanel.h,v 1.1 2008-10-16 08:29:33 Marabou Exp $       
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
#include "TGMrbLofKeyBindings.h"

#include "M2L_CommonDefs.h"
#include "VMEControlData.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302SaveRestorePanel
// Purpose:        Main frame to save & restore module settings
// Constructors:   
// Description:    A dialog window to save & restore module settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class VMESis3302SaveRestorePanel : public TGCompositeFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EVMESis3302SaveRestoreId 		{	kVMESis3302Save,
												kVMESis3302Restore,
												kVMESis3302Modules,
											};
				
	public:
		VMESis3302SaveRestorePanel(TGCompositeFrame * TabFrame, TMrbLofNamedX * LofModules);
		virtual ~VMESis3302SaveRestorePanel() { fHeap.Delete(); };

		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t SaveSettings();			// save settings to file
		Bool_t RestoreSettings();		// restore settings from file

	protected:
		TList fHeap;								//! list of objects created on heap
		TGMrbCheckButtonGroup * fModules;
		TGMrbTextButtonGroup * fActionButtons;
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TMrbLofNamedX * fLofModules;
		TMrbLofNamedX fLofSelected;

		TGMrbFocusList fFocusList;

	ClassDef(VMESis3302SaveRestorePanel, 0)		// [VMEControl] Panel to save/reswtore Sis3302 settings
};

#endif
