#ifndef __VMEVulomTBPanel_h__
#define __VMEVulomTBPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel.h
// Purpose:        Class defs for VME modules
// Class:          VMEVulomTBPanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMEVulomTBPanel.h,v 1.1 2009-08-06 08:32:34 Rudolf.Lutter Exp $
// Date:           $Date: 2009-08-06 08:32:34 $
// URL:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"
#include "TGTextView.h"

#include "TTimer.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbTail.h"

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbLofKeyBindings.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel
// Purpose:        Panel to control a Caen V785 adc
// Constructors:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class VMEVulomTBPanel : public TGCompositeFrame {

	public:
		enum EVMEVulomTBCmdId 		{
										kVMEVulomTBSelectModule,
										kVMEVulomTBActionEnableAll,
										kVMEVulomTBActionDisableAll,
										kVMEVulomTBActionClearScaler,
										kVMEVulomTBActionReset,
										kVMEVulomTBActionStartTimer,
										kVMEVulomTBActionStopTimer
									};

		enum						{	kVMENofVulomTBChans =	16	};

	public:
		VMEVulomTBPanel(TGCompositeFrame * TabFrame);
		virtual ~VMEVulomTBPanel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		void UpdateGUI();

		inline Int_t GetNofModules() { return(fLofModules.GetEntries()); };
		inline TMrbLofNamedX * GetLofModules() { return(&fLofModules); };

		void ModuleChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void ChannelEnableChanged(Int_t Channel);
		void ScaleDownChanged(Int_t FrameId, Int_t Selection);
		void TimerIntervalChanged(Int_t FrameId, Int_t Selection);

		void PerformAction(Int_t FrameId, Int_t Selection);

	protected:
		Bool_t SetupModuleList();

	protected:
		TList fHeap;										//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 								// select
		TGMrbLabelCombo * fSelectModule;  							//		module

		TGGroupFrame * fSettingsFrame; 								// trigger box

		TGGroupFrame * fEnableFrame; 								// 		enable/diable channel
		TGGroupFrame * fScaleDownFrame; 							//		scale down values
		TGGroupFrame * fScalerFrame; 								//		scaler contents

		TGCheckButton * fChannelEnable[kVMENofVulomTBChans];		// enable bits
		TGMrbLabelEntry * fScaleDownValue[kVMENofVulomTBChans];		// scale down values

		TGMrbLabelEntry * fScalerFree[kVMENofVulomTBChans];			// scaler contents
		TGMrbLabelEntry * fScalerInhibit[kVMENofVulomTBChans];		// ... after inhibit
		TGMrbLabelEntry * fScalerScaledDown[kVMENofVulomTBChans];	// ... after scale down

		TGMrbTextButtonGroup * fActionButtons;	 	// actions
		TMrbLofNamedX fActions;

		TTimer * fTimer;											// timer
		TGMrbLabelEntry * fTimerInterval;							// ... interval

		TMrbLofNamedX fLofModules;

		TGMrbFocusList fFocusList;


	ClassDef(VMEVulomTBPanel, 0) 	// [VMEControl] Panel to control a Caen V785 adc
};

#endif
