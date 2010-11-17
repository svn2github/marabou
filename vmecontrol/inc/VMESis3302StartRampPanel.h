#ifndef __VMESis3302StartRampPanel_h__
#define __VMESis3302StartRampPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          VMESis3302StartRampPanel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMESis3302StartRampPanel.h,v 1.1 2010-11-17 14:18:14 Marabou Exp $
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
// Name:           VMESis3302StartRampPanel
// Purpose:        Main frame to save & restore module settings
// Constructors:
// Description:    A dialog window to save & restore module settings
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class VMESis3302StartRampPanel : public TGMainFrame {

	public:

		// cmd ids to dispatch over X events in this panel
		enum EVMESis3302StartHistoId 			{
												kVMESis3302SelectModule,
												kVMESis3302SelectChanPatt,
												kVMESis3302StartStop,
												kVMESis3302WriteHisto,
												kVMESis3302DeleteClones,
												kVMESis3302Offset,
											};

		VMESis3302StartRampPanel(const TGWindow * Parent, TMrbLofNamedX * LofModules,
							UInt_t Width, UInt_t Height, UInt_t Options = kMainFrame | kVerticalFrame);
		virtual ~VMESis3302StartRampPanel() { fHeap.Delete(); };

		inline void CloseWindow() { TGMainFrame::CloseWindow(); };
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };

		void ModuleChanged(Int_t FrameId, Int_t Selection);			// slot methods
		void PerformAction(Int_t FrameId, Int_t Selection);
		void KeyPressed(Int_t FrameId, Int_t Action);

		void CloneHisto(TMrbNamedX * HistoDef);
		inline void DeleteClones() { fLofClones.Delete(); };

	protected:
		void StartGUI();
		void StartRamp();
		void StopRamp();
		Int_t InitializeHistos(UInt_t ChnPatt = kSis3302ChanPattern);
		void WriteHisto();

	protected:
		TList fHeap;								//! list of objects created on heap

		TGGroupFrame * fSelectFrame; 				// select
		TGMrbLabelCombo * fSelectModule;  			//		module
		TGMrbCheckButtonList * fSelectChanPatt;  		//		channel

		TGGroupFrame * fHistoFrame; 				// show histos
		TRootEmbeddedCanvas * fHistoCanvas;			//		canvas

		TGGroupFrame * fRampFrame;				// ramping
		TGTextButton * fStartStopButton;			// start/stop trace
		TGTextButton * fWriteHistoButton;			// write histogram
		TGTextButton * fWriteDeleteClonesButton;		// delete clones

		TGMrbLabelEntry * fOffset;				// ramping offset

		TMrbLofNamedX * fLofModules;
		TMrbLofNamedX fLofSelected;

		Bool_t fRampingInProgress;

		TGMrbFocusList fFocusList;

		TGMrbLofKeyBindings fKeyBindings; 		// key bindings

		TFile * fHistoFile;

		TMrbLofNamedX fLofHistos;			// list of histograms

		TObjArray fLofClones;				// list of canvas clones

	ClassDef(VMESis3302StartRampPanel, 0)		// [VMEControl] Panel to calibrate DACs for a SIS3302 module
};

#endif
