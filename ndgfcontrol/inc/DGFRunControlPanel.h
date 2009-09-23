#ifndef __DGFRunControlPanel_h__
#define __DGFRunControlPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFRunControlPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       $Id: DGFRunControlPanel.h,v 1.1 2009-09-23 10:46:23 Marabou Exp $       
// Date:           
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
#include "TMrbDGF.h"

#include "TGMrbTextButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbMacroBrowser.h"
#include "TGMrbMessageViewer.h"
#include "TGMrbLofKeyBindings.h"

#include "DGFSetupPanel.h"
#include "DGFSetFilesPanel.h"
#include "DGFInstrumentPanel.h"
#include "DGFParamsPanel.h"
#include "DGFTauDisplayPanel.h"
#include "DGFTauFitPanel.h"
#include "DGFUntrigTracePanel.h"
#include "DGFTraceDisplayPanel.h"
#include "DGFOffsetsPanel.h"
#include "DGFMcaDisplayPanel.h"
#include "DGFRestoreModuleSettingsPanel.h"
#include "DGFSaveModuleSettingsPanel.h"
#include "DGFCopyModuleSettingsPanel.h"
#include "DGFMiscPanel.h"
#include "DGFCptmPanel.h"
#include "DGFControlCommon.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel
// Purpose:        Main frame for the DGF Control GUI
// Description:    Defines a main frame for the GUI
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class DGFRunControlPanel : public TGMainFrame {

	public:
		// cmd ids to dispatch over X events in main frame
		enum EDGFRunControlCmdId {	kDGFFileExit,					// pulldown menu "File"

									kDGFViewErrors, 				// pulldown menu "View"

									kDGFGeneralOutputNormal, 		// pulldown menu "General"
									kDGFGeneralOutputVerbose,
									kDGFGeneralOutputDebug,
									kDGFGeneralModNumLocal,
									kDGFGeneralModNumGlobal,
									kDGFGeneralOffline,
									kDGFGeneralOnline,
									kDGFGeneralEsoneSingleStep,
									kDGFGeneralEsoneNormal,

									kDGFHelpContents,				// pulldown menu "Help"
									kDGFHelpAbout,
								};

		enum EDGFRunControlTabId {
									kDGFRunControlTabSystem = 0,
									kDGFRunControlTabModules,
									kDGFRunControlTabParams,
									kDGFRunControlTabTrace,
									kDGFRunControlTabUntrigTrace,
									kDGFRunControlTabOffsets,
									kDGFRunControlTabMCA,
									kDGFRunControlTabTauFit1,
									kDGFRunControlTabTauFit2,
									kDGFRunControlTabMisc,
									kDGFRunControlTabSave,
									kDGFRunControlTabRestore,
									kDGFRunControlTabCopy,
									kDGFRunControlTabFiles,
									kDGFRunControlTabCptm
								};


	public:
		DGFRunControlPanel(const TGWindow * Window, UInt_t Width, UInt_t Height);
		virtual ~DGFRunControlPanel() { fHeap.Delete(); };

		virtual void CloseWindow();
		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		
		void Activate(Int_t Selection); 	// slot methods
		void SelectTab(Int_t Selection);
		void KeyPressed(Int_t FrameId, Int_t Action);

	protected:
		void PopupMessageViewer();
		void McaPause();
		void McaResume();
	
	protected:
		TList fHeap;								//! list of objects created on heap

		TGMenuBar * fMenuBar;						// main menu bar
		TGPopupMenu * fMenuFile;					//					pulldown menu: file
		TGPopupMenu * fMenuView;					//					pulldown menu: view
		TGPopupMenu * fMenuGeneral;					//					pulldown menu: general
		TGMrbMacroBrowserPopup * fMenuMacros;		//					pulldown menu: macros
		TGPopupMenu * fMenuHelp;					//					pulldown menu: help

													// run control panel
		TGTab * fRunControlTab; 					//					vert. frame to store different items

		TGCompositeFrame * fSystemTab;
		TGCompositeFrame * fModulesTab;
		TGCompositeFrame * fParamsTab;
		TGCompositeFrame * fTracesTab;
		TGCompositeFrame * fUntrigTracesTab;
		TGCompositeFrame * fOffsetsTab;
		TGCompositeFrame * fMCATab;
		TGCompositeFrame * fTauDisplayTab;
		TGCompositeFrame * fTauFitTab;
		TGCompositeFrame * fMiscTab;
		TGCompositeFrame * fSaveTab;
		TGCompositeFrame * fRestoreTab;
		TGCompositeFrame * fCopyTab;
		TGCompositeFrame * fFilesTab;
		TGCompositeFrame * fCptmTab;

		DGFSetupPanel * fSetupPanel;
		DGFInstrumentPanel * fInstrumentPanel;
		DGFParamsPanel * fParamsPanel;
		DGFTraceDisplayPanel * fTraceDisplayPanel;
		DGFUntrigTracePanel * fUntrigTracePanel;
		DGFOffsetsPanel * fOffsetsPanel;
		DGFMcaDisplayPanel * fMcaDisplayPanel;
		DGFTauDisplayPanel * fTauDisplayPanel;
		DGFTauFitPanel * fTauFitPanel;
		DGFMiscPanel * fMiscPanel;
		DGFSaveModuleSettingsPanel * fSaveModuleSettingsPanel;
		DGFRestoreModuleSettingsPanel * fRestoreModuleSettingsPanel;
		DGFCopyModuleSettingsPanel * fCopyModuleSettingsPanel;
		DGFSetFilesPanel * fSetFilesPanel;
		DGFCptmPanel * fCptmPanel;

		TMrbLofMacros * fLofMacros; 				// list of macros

		TGMrbMessageViewer * fMsgViewer;
		
		TGMrbLofKeyBindings fKeyBindings; 		// key bindings


		
	ClassDef(DGFRunControlPanel, 0) 	// [DGFControl] Main control panel
};

#endif
