#ifndef __DGFRunControlPanel_h__
#define __DGFRunControlPanel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel.h
// Purpose:        Class defs for the XIA DGF-4C GUI
// Class:          DGFRunControlPanel
// Description:    A GUI to operate a XIA DGF-4C
// Author:         R. Lutter
// Revision:       
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
									kDGFRunControlTabSave,
									kDGFRunControlTabRestore,
									kDGFRunControlTabFiles
								};


	public:
		DGFRunControlPanel(const TGWindow * Window, UInt_t Width, UInt_t Height);
		virtual ~DGFRunControlPanel() { fHeap.Delete(); };

		DGFRunControlPanel(const DGFRunControlPanel & f) : TGMainFrame(f) {};	// default copy ctor

		virtual void CloseWindow();
		virtual Bool_t ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2);

		inline Bool_t HandleKey(Event_t * Event) { return(fKeyBindings.HandleKey(Event)); };
		
	protected:
		void PopupMessageViewer();
	
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
		TGCompositeFrame * fTauFit1Tab;
		TGCompositeFrame * fTauFit2Tab;
		TGCompositeFrame * fSaveTab;
		TGCompositeFrame * fRestoreTab;
		TGCompositeFrame * fFilesTab;

		Bool_t fSystemTabInit;
		Bool_t fModulesTabInit;
		Bool_t fParamsTabInit;
		Bool_t fTracesTabInit;
		Bool_t fUntrigTracesTabInit;
		Bool_t fOffsetsTabInit;
		Bool_t fMCATabInit;
		Bool_t fTauFit1TabInit;
		Bool_t fTauFit2TabInit;
		Bool_t fSaveTabInit;
		Bool_t fRestoreTabInit;
		Bool_t fFilesTabInit;

		TMrbLofMacros * fLofMacros; 				// list of macros

		TGMrbMessageViewer * fMsgViewer;
		
		TGMrbLofKeyBindings fKeyBindings; 		// key bindings
		
	ClassDef(DGFRunControlPanel, 0) 	// [DGFControl] Main control panel
};

#endif
