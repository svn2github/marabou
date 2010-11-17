#ifndef __VMESis3302StartTracePanel_h__
#define __VMESis3302StartTracePanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          VMESis3302StartTracePanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302StartTracePanel.h,v 1.8 2010-11-17 12:25:11 Marabou Exp $
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
												kVMESis3302SelectTrigMode,
												kVMESis3302StartStop,
												kVMESis3302DumpTrace,
												kVMESis3302WriteTrace,
												kVMESis3302DeleteClones,
												kVMESis3302MaxTraces,
												kVMESis3302TimeStamp,
												kVMESis3302TracesNo,
												kVMESis3302TracesPerSecond,
											};

		enum EVMESis3302TraceModes			{	kVMESis3302ModeMAWD		= 0,
												kVMESis3302ModeMAW		= BIT(0),
												kVMESis3302ModeMAWFT	= BIT(1)
											};

	public:
		VMESis3302StartTracePanel(const TGWindow * Parent, TMrbLofNamedX * LofModules,
							UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~VMESis3302StartTracePanel() { fHeap.Delete(); };

		inline void CloseWindow() { TGMainFrame::CloseWindow(); };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };

		void ModuleChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void ChannelChanged(Int_t FrameId, Int_t Selection);
		void TraceModeChanged(Int_t FrameId, Int_t Selection);
		void PerformAction(Int_t FrameId, Int_t Selection);
		void KeyPressed(Int_t FrameId, Int_t Action);

		void CloneHisto(TMrbNamedX * HistoDef);
		inline void DeleteClones() { fLofClones.Delete(); };

	protected:
		void StartGUI();
		void StartTrace();
		void StopTrace();
		Int_t InitializeHistos(UInt_t ChnPatt = kSis3302ChanPattern);
		void WriteTrace();
		Int_t ReadData(TArrayI & Data, TMrbNamedX * RhistoDef, TMrbNamedX * EhistoDef, Int_t TraceData[], Int_t TraceNumber);

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule;  			//		module
		TGMrbCheckButtonList * fSelectChanPatt;  		//		channel

		TGGroupFrame * fHistoFrame; 				// show histos
		TRootEmbeddedCanvas * fHistoCanvas;			//		canvas

		TGGroupFrame * fTraceFrame;				// traces
		TGMrbLabelCombo * fSelectTrigMode;  			// trigger mode
		TGTextButton * fStartStopButton;			// start/stop trace
		TGTextButton * fDumpTraceButton;			// dump trace
		TGTextButton * fWriteTraceButton;			// write trace
		TGTextButton * fWriteDeleteClonesButton;		// delete clones

		TGGroupFrame * fDisplayFrame;				// display
		TGMrbLabelEntry * fTimeStamp;				// timestamp
		TGMrbLabelEntry * fTracesNo;				// traces number
		TGMrbLabelEntry * fTracesPerSec;			// traces per second
		TGMrbLabelEntry * fMaxTraces;				// number of traces

		TMrbLofNamedX * fLofModules;
		TMrbLofNamedX fLofSelected;
		TMrbLofNamedX fLofTraceModes;

		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 			// key bindings

		Bool_t fTraceCollection;				// kTRUE if started
		TFile * fTraceFile;					// root file to store traces
		Int_t fNofTracesWritten;

		TMrbLofNamedX fLofRhistos;				// list of histograms
		TMrbLofNamedX fLofEhistos;

		TObjArray fLofClones;					// list of canvas clones

	ClassDef(VMESis3302StartTracePanel, 0)		// [VMEControl] Panel to collect traces for a SIS3302 module
};

#endif
