#ifndef __VMESis3302StartHistoPanel_h__
#define __VMESis3302StartHistoPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          VMESis3302StartHistoPanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302StartHistoPanel.h,v 1.7 2011-07-26 08:41:49 Marabou Exp $
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
// Name:           VMESis3302StartHistoPanel
// Purpose:        Main frame to save & restore module settings
// Constructors:
// Description:    A dialog window to save & restore module settings
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class VMESis3302StartHistoPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EVMESis3302StartHistoId 			{
												kVMESis3302SelectModule,
												kVMESis3302SelectChanPatt,
												kVMESis3302SelectTrigMode,
												kVMESis3302StartStop,
												kVMESis3302Pause,
												kVMESis3302DumpTrace,
												kVMESis3302WriteHisto,
												kVMESis3302DeleteClones,
												kVMESis3302NofBufsWrite,
												kVMESis3302NofEvtsBuf,
												kVMESis3302EnergyMax,
												kVMESis3302HistoSize,
												kVMESis3302EnergyFactor,
												kVMESis3302Close
		};

		enum EVMESis3302TraceModes			{				kVMESis3302ModeMAWD		= 0,
												kVMESis3302ModeMAW		= BIT(0),
												kVMESis3302ModeMAWFT		= BIT(1)
											};

	public:
		VMESis3302StartHistoPanel(const TGWindow * Parent, TMrbLofNamedX * LofModules,
							UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~VMESis3302StartHistoPanel() { fHeap.Delete(); };

		inline void CloseWindow() { TGMainFrame::CloseWindow(); };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };

		void ModuleChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void TraceModeChanged(Int_t FrameId, Int_t Selection);
		void PerformAction(Int_t FrameId, Int_t Selection);
		void KeyPressed(Int_t FrameId, Int_t Action);

		void CloneHisto(TMrbNamedX * HistoDef);
		inline void DeleteClones() { fLofClones.Delete(); };

	protected:
		void StartGUI();
		void StartHisto();
		void StopHisto();
		void PauseHisto();
		Int_t InitializeHistos(UInt_t ChnPatt = kSis3302ChanPattern);
		void WriteHisto(TMrbNamedX * HistoDef = NULL);
		Int_t ReadData(TArrayI & Data, TMrbNamedX * HistoDef, Int_t TraceData[], Int_t TraceNumber);

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule;  			//		module
		TGMrbCheckButtonList * fSelectChanPatt;  		//		channel

		TGGroupFrame * fHistoFrame; 				// show histos
		TRootEmbeddedCanvas * fHistoCanvas;			//		canvas

		TGGroupFrame * fTraceFrame;					// traces
		TGMrbLabelCombo * fSelectTrigMode;  			// trigger mode
		TGTextButton * fStartStopButton;			// start/stop trace
		TGTextButton * fPauseButton;				// pause
		TGTextButton * fDumpTraceButton;			// dump trace
		TGTextButton * fWriteHistoButton;			// write histogram
		TGTextButton * fWriteDeleteClonesButton;		// delete clones
		TGTextButton * fCloseButton;				// close window

		TGGroupFrame * fDisplayFrame;				// display
		TGMrbLabelCombo * fSelectChannel;  			//		channel
		TGMrbLabelEntry * fMaxEnergy;				// max energy
		TGMrbLabelEntry * fHistoSize;				// size of histogram in kChannels
		TGMrbLabelEntry * fEnergyFactor;			// energy scale factor
		TGMrbLabelEntry * fNofBufsWrite;			// buffers per write
		TGMrbLabelEntry * fNofEvtsBuf;				// events per buffer

		Int_t fEmax;
		Int_t fHsize;
		Double_t fEfac;

		TMrbLofNamedX * fLofModules;
		TMrbLofNamedX fLofSelected;
		TMrbLofNamedX fLofTraceModes;

		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings

		Bool_t fTraceCollection;			// kTRUE if started
		Bool_t fPausePressed;				// pause button
		TFile * fHistoFile;
		Int_t fNofHistosWritten;

		TMrbLofNamedX fLofHistos;			// list of histograms

		TObjArray fLofClones;				// list of canvas clones

	ClassDef(VMESis3302StartHistoPanel, 0)		// [VMEControl] Panel to start histogramming for a SIS3302 module
};

#endif
