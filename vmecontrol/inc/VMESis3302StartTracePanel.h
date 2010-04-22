#ifndef __VMESis3302StartTracePanel_h__
#define __VMESis3302StartTracePanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          VMESis3302StartTracePanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302StartTracePanel.h,v 1.1 2010-04-22 13:44:41 Rudolf.Lutter Exp $
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
#include "TGMrbCheckButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbLofKeyBindings.h"


#include "M2L_CommonDefs.h"
#include "VMEControlData.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel
// Purpose:        Main frame to save & restore module settings
// Constructors:
// Description:    A dialog window to save & restore module settings
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class VMESis3302StartTracePanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EVMESis3302StartTraceId 			{
												kVMESis3302SelectModule,
												kVMESis3302SelectChanPatt,
												kVMESis3302SelectMode,
												kVMESis3302NofEvents,
												kVMESis3302Timeout,
												kVMESis3302Start,
												kVMESis3302SelectChannel,
												kVMESis3302SelectEvent,
											};

		enum EVMESis3302TraceModes			{	kVMESis3302ModeMAWD		= 0,
												kVMESis3302ModeMAW		= BIT(0),
												kVMESis3302ModeMAWFT	= BIT(1)
											};

	public:
		VMESis3302StartTracePanel(const TGWindow * Parent, TMrbLofNamedX * LofModules,
							UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~VMESis3302StartTracePanel() { fHeap.Delete(); };

		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };

		void ModuleChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void ChannelChanged(Int_t FrameId, Int_t Selection);
		void TraceModeChanged(Int_t FrameId, Int_t Selection);
		void NofEventsChanged(Int_t FrameId, Int_t Selection);
		void EventNumberChanged(Int_t FrameId, Int_t Selection);
		void PerformAction(Int_t FrameId, Int_t Selection);
		void KeyPressed(Int_t FrameId, Int_t Action);

	protected:
		void StartGUI();
		void StartTrace();
		Bool_t ReadData(TArrayI & Data, Int_t EventNo, Int_t Channel);

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule;  			//		module
		TGMrbCheckButtonList * fSelectChanPatt;  	//		channel

		TGGroupFrame * fHistoFrame; 				// show histos
		TRootEmbeddedCanvas * fHistoCanvas;			//		canvas

		TGGroupFrame * fTraceFrame;					// traces
		TGMrbLabelCombo * fSelectMode;  			// trigger mode
		TGMrbLabelEntry * fNofEvents; 				// number of events
		TGTextButton * fStartButton;				// start trace

		TH1F * fHistoRaw;							// histos for raw and energy data
		TH1F * fHistoEnergy;

		TGGroupFrame * fDisplayFrame;				// display
		TGMrbLabelCombo * fSelectChannel;  			//		channel
		TGMrbLabelEntry * fSelectEvent; 			//		event

		TMrbLofNamedX * fLofModules;
		TMrbLofNamedX fLofSelected;
		TMrbLofNamedX fLofTraceModes;

		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings

	ClassDef(VMESis3302StartTracePanel, 0)		// [VMEControl] Panel to save/reswtore Sis3302 settings
};

#endif
