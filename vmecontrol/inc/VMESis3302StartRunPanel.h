#ifndef __VMESis3302StartRunPanel_h__
#define __VMESis3302StartRunPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRunPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          VMESis3302StartRunPanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302StartRunPanel.h,v 1.1 2008-10-27 12:26:07 Marabou Exp $       
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
#include "TGMenu.h"
#include "TGTextView.h"
#include "TRootEmbeddedCanvas.h"

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
// Name:           VMESis3302StartRunPanel
// Purpose:        Main frame to save & restore module settings
// Constructors:   
// Description:    A dialog window to save & restore module settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class VMESis3302StartRunPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EVMESis3302StartRunId 			{
												kVMESis3302SelectModule,
												kVMESis3302SelectChannel,
												kVMESis3302Timeout,
												kVMESis3302Start,
												kVMESis3302Close,
											};
				
	public:
		VMESis3302StartRunPanel(const TGWindow * Parent, TMrbLofNamedX * LofModules,
							UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~VMESis3302StartRunPanel() { fHeap.Delete(); };

		void ModuleChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void ChannelChanged(Int_t FrameId, Int_t Selection);
		void TimeoutChanged(Int_t FrameId, Int_t Selection);
		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		void StartGUI();
		void StartRun();
		inline void CloseWindow() { TGMainFrame::CloseWindow(); };

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule;  			//		module
		TGMrbLabelCombo * fSelectChannel;  			//		channel

		TGGroupFrame * fHistoFrame; 				// show histos
		TRootEmbeddedCanvas * fHistoCanvas;			//		canvas

		TGMrbTextButtonGroup * fActionButtons;	 	// actions
		
		TMrbLofNamedX fActions;						// text buttons:	actions to be taken
		
		TGMrbLabelEntry * fTimeout; 				// timeout

		TH1F * fHistoRaw;							// display raw and energy data
		TH1F * fHistoEnergy;

		TMrbLofNamedX * fLofModules;
		TMrbLofNamedX fLofSelected;

		TGMrbFocusList fFocusList;

	ClassDef(VMESis3302StartRunPanel, 0)		// [VMEControl] Panel to save/reswtore Sis3302 settings
};

#endif
