#ifndef __VMESis3302Panel_h__
#define __VMESis3302Panel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel.h
// Purpose:        Class defs for VME modules
// Class:          VMESis3302Panel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302Panel.h,v 1.3 2008-10-18 17:09:14 Marabou Exp $       
// Date:           $Date: 2008-10-18 17:09:14 $
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"
#include "TGTab.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbTail.h"

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbLofKeyBindings.h"

#include "VMESis3302SettingsPanel.h"
#include "VMESis3302SaveRestorePanel.h"
#include "VMESis3302CopyPanel.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel
// Purpose:        Panel to control a Sis3302 adc
// Constructors:   
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class VMESis3302Panel : public TGCompositeFrame {

	public:
		enum EVMEMainFrameTabId {
									kVMETabSettings = 0,
									kVMETabCopy,
									kVMETabSaveRestore,
									kVMELastTab 				// init of tabs has to be done in same order!!
								};

		enum					{	kVMENofSis3302Chans =	8	};

	public:
		VMESis3302Panel(TGCompositeFrame * TabFrame);
		virtual ~VMESis3302Panel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		inline TMrbLofNamedX * GetLofModules() { return(&fLofModules); };
		inline Int_t GetNofModules() { return(fLofModules.GetEntries()); };

		Bool_t SetupModuleList();

		void TabChanged(Int_t Selection);			// slot methods

	protected:
		TList fHeap;								//! list of objects created on heap

		TGTab * fTabFrame;
		TGCompositeFrame * fSettingsTab;
		VMESis3302SettingsPanel * fSettingsPanel;
		TGCompositeFrame * fSaveRestoreTab;
		VMESis3302SaveRestorePanel * fSaveRestorePanel;
		TGCompositeFrame * fCopyTab;
		VMESis3302CopyPanel * fCopyPanel;

		TMrbLofNamedX fLofModules;	// list of SIS 3302 modules

		TGMrbFocusList fFocusList;

	ClassDef(VMESis3302Panel, 0) 	// [VMEControl] Panel to control a Sis3302 adc
};

#endif
