#ifndef __VMESis3302CopyPanel_h__
#define __VMESis3302CopyPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel.h
// Purpose:        Class defs for VME modules
// Class:          VMESis3302CopyPanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302CopyPanel.h,v 1.1 2008-10-18 17:09:14 Marabou Exp $       
// Date:           $Date: 2008-10-18 17:09:14 $
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"
#include "TGTextView.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbTail.h"

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbLofKeyBindings.h"

#include "M2L_CommonDefs.h"
#include "VMEControlData.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel
// Purpose:        Panel to control a Sis3302 adc
// Constructors:   
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class VMESis3302CopyPanel : public TGCompositeFrame {

	public:
		enum EVMESis3302CmdId 		{
										kVMESis3302SelectModuleFrom,
										kVMESis3302SelectChannelFrom,
										kVMESis3302ActionCopy,
										kVMESis3302SelectModuleTo		// has to be last entry in list!!
									};

	public:
		VMESis3302CopyPanel(TGCompositeFrame * TabFrame, TMrbLofNamedX * LofModules);
		virtual ~VMESis3302CopyPanel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		void StartGUI();

		void ModuleFromChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void ChannelFromChanged(Int_t FrameId, Int_t Selection);
		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		void CopySettings();

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fCopyFromFrame; 				// copy from
		TGMrbLabelCombo * fSelectModuleFrom;  		//		module
		TGMrbLabelCombo * fSelectChannelFrom;   	//		channel
		TGGroupFrame * fCopyToFrame; 				// copy to
		TGMrbCheckButtonList * fSelectModuleTo; 	//		modules and channels
		TGMrbTextButtonGroup * fActionButtons;	 	// actions
		
		TMrbLofNamedX fActions;

		TMrbLofNamedX * fLofModules;
		TMrbLofNamedX fLofSelected;
		TMrbLofNamedX fLofDests;

		TGMrbFocusList fFocusList;

	ClassDef(VMESis3302CopyPanel, 0) 	// [VMEControl] Panel to control a Sis3302 adc
};

#endif
