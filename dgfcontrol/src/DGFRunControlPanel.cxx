//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFRunControlPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Run Control
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFRunControlPanel.cxx,v 1.18 2008-12-29 13:48:25 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFRunControlPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "TFile.h"
#include "TObjString.h"
#include "TApplication.h"
#include "TGFileDialog.h"

#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "DGFRunControlPanel.h"
#include "DGFControlData.h"
#include "DGFControlCommon.h"

#include "SetColor.h"

DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

ClassImp(DGFRunControlPanel)

DGFRunControlPanel::DGFRunControlPanel(const TGWindow * Window, UInt_t Width, UInt_t Height)
      																: TGMainFrame(Window, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel
// Purpose:        Main frame
// Arguments:      TGWindow Window      -- connection to ROOT graphics
//                 UInt_t Width         -- window width in pixels
//                 UInt_t Height        -- window height in pixels
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * buttonGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * frameGC;
	Bool_t hasMacros;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	fMsgViewer = NULL;
		
	buttonGC = new TGMrbLayout( gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGold);	HEAP(frameGC);
	
	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGold);	HEAP(groupGC);

//	Create popup menus.

//	File menu
	fMenuFile = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuFile);

	fMenuFile->AddEntry("Exit ... Ctrl-q", kDGFFileExit);
	fMenuFile->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

//	View menu
	fMenuView = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuView);

	fMenuView->AddEntry("&Errors", kDGFViewErrors);
	fMenuView->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

//	General menu
	fMenuGeneral = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuGeneral);

	fMenuGeneral->AddEntry("&Normal", kDGFGeneralOutputNormal);
	fMenuGeneral->AddEntry("&Verbose", kDGFGeneralOutputVerbose);
	fMenuGeneral->AddEntry("&Debug (very verbose)", kDGFGeneralOutputDebug);
	if (gDGFControlData->fStatus & DGFControlData::kDGFDebugMode) {
		fMenuGeneral->RCheckEntry(kDGFGeneralOutputDebug, kDGFGeneralOutputNormal, kDGFGeneralOutputDebug);
	} else if (gDGFControlData->fStatus & DGFControlData::kDGFVerboseMode) {
		fMenuGeneral->RCheckEntry(kDGFGeneralOutputVerbose, kDGFGeneralOutputNormal, kDGFGeneralOutputDebug);
	} else {
		fMenuGeneral->RCheckEntry(kDGFGeneralOutputNormal, kDGFGeneralOutputNormal, kDGFGeneralOutputDebug);
	}
	fMenuGeneral->AddSeparator();
	fMenuGeneral->AddEntry("Module Numbers &Global", kDGFGeneralModNumGlobal);
	fMenuGeneral->AddEntry("Module Numbers &Local", kDGFGeneralModNumLocal);
	if (gDGFControlData->fStatus & DGFControlData::kDGFModNumGlobal) {
		fMenuGeneral->RCheckEntry(kDGFGeneralModNumGlobal, kDGFGeneralModNumLocal, kDGFGeneralModNumGlobal);
	} else {
		fMenuGeneral->RCheckEntry(kDGFGeneralModNumLocal, kDGFGeneralModNumLocal, kDGFGeneralModNumGlobal);
	}
	fMenuGeneral->AddSeparator();
	fMenuGeneral->AddEntry("Offline", kDGFGeneralOffline);
	fMenuGeneral->AddEntry("Online", kDGFGeneralOnline);
	if (gDGFControlData->fStatus & DGFControlData::kDGFOfflineMode) {
		fMenuGeneral->RCheckEntry(kDGFGeneralOffline, kDGFGeneralOffline, kDGFGeneralOnline);
	} else {
		fMenuGeneral->RCheckEntry(kDGFGeneralOnline, kDGFGeneralOffline, kDGFGeneralOnline);
	}
	fMenuGeneral->AddSeparator();
	fMenuGeneral->AddEntry("&Esone Normal", kDGFGeneralEsoneNormal);
	fMenuGeneral->AddEntry("&Esone SingleStep", kDGFGeneralEsoneSingleStep);
	if (gDGFControlData->fStatus & DGFControlData::kDGFEsoneSingleStepMode) {
		fMenuGeneral->RCheckEntry(kDGFGeneralEsoneSingleStep, kDGFGeneralEsoneNormal, kDGFGeneralEsoneNormal);
	} else {
		fMenuGeneral->RCheckEntry(kDGFGeneralEsoneNormal, kDGFGeneralEsoneSingleStep, kDGFGeneralEsoneNormal);
	}
	fMenuGeneral->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

//	Macros menu
	fLofMacros = new TMrbLofMacros();
	HEAP(fLofMacros);
	fLofMacros->AddMacro("*.C", "DGF");
	hasMacros = (fLofMacros->GetLast() >= 0);

	if (hasMacros) {
		fMenuMacros = new TGMrbMacroBrowserPopup(fClient->GetRoot(), fLofMacros);
		HEAP(fMenuMacros);
		fMenuMacros->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");
	}

//	Help menu
	fMenuHelp = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuHelp);

	fMenuHelp->AddEntry("&Contents", kDGFHelpContents);
	fMenuHelp->AddSeparator();
	fMenuHelp->AddEntry("&About DGFControl", kDGFHelpAbout);
	fMenuHelp->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

	if (hasMacros) fMenuMacros->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

//	Main menu bar
	fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame | kSunkenFrame);
	HEAP(fMenuBar);

	TGLayoutHints * menuBarFileLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarFileLayout);
	fMenuBar->AddPopup("&File", fMenuFile, menuBarFileLayout);

	TGLayoutHints * menuBarViewLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarViewLayout);
	fMenuBar->AddPopup("&View", fMenuView, menuBarViewLayout);

	TGLayoutHints * menuBarGeneralLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarGeneralLayout);
	fMenuBar->AddPopup("&General", fMenuGeneral, menuBarGeneralLayout);

	if (hasMacros) {
		TGLayoutHints * menuBarMacrosLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 3);
		HEAP(menuBarMacrosLayout);
		fMenuBar->AddPopup("&Macros", fMenuMacros, menuBarMacrosLayout);
	}

	TGLayoutHints * menuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 2, 2, 3);
	HEAP(menuBarHelpLayout);
	fMenuBar->AddPopup("&Help", fMenuHelp, menuBarHelpLayout);

	TGLayoutHints * menuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 3);
	HEAP(menuBarLayout);
	AddFrame(fMenuBar, menuBarLayout);

	fMenuBar->ChangeBackground(gDGFControlData->fColorDarkBlue);

//	create main tab object
	fRunControlTab = new TGTab(this, kTabWidth, kTabHeight);
	HEAP(fRunControlTab);
	TGLayoutHints * tabLayout = new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
	HEAP(tabLayout);
	this->AddFrame(fRunControlTab, tabLayout);
	fRunControlTab->Connect("Selected(Int_t)", this->ClassName(), this, "SelectTab(Int_t)");

// add tabs
	fSetupPanel = NULL;
	fInstrumentPanel = NULL;
	fParamsPanel = NULL;
	fTraceDisplayPanel = NULL;
	fUntrigTracePanel = NULL;
	fOffsetsPanel = NULL;
	fMcaDisplayPanel = NULL;
	fTauDisplayPanel = NULL;
	fTauFitPanel = NULL;
	fMiscPanel = NULL;
	fSaveModuleSettingsPanel = NULL;
	fRestoreModuleSettingsPanel = NULL;
	fCopyModuleSettingsPanel = NULL;
	fSetFilesPanel = NULL;
	fCptmPanel = NULL;

	fSystemTab = fRunControlTab->AddTab("System");
	fModulesTab = fRunControlTab->AddTab("Modules");
	fParamsTab = fRunControlTab->AddTab("Params");
	fTracesTab = fRunControlTab->AddTab("Traces");
	fUntrigTracesTab = fRunControlTab->AddTab("Untrig Traces");
	fOffsetsTab = fRunControlTab->AddTab("Offsets");
	fMCATab = fRunControlTab->AddTab("MCA");
	fTauDisplayTab = fRunControlTab->AddTab("TauDisplay");
	fTauFitTab = fRunControlTab->AddTab("TauFit");
	fMiscTab = fRunControlTab->AddTab("Misc");
	fSaveTab = fRunControlTab->AddTab("Save");
	fRestoreTab = fRunControlTab->AddTab("Restore");
	fCopyTab = fRunControlTab->AddTab("Copy");
	fFilesTab = fRunControlTab->AddTab("Files");
	fCptmTab = fRunControlTab->AddTab("CPTM");

	fRunControlTab->SetTab(kDGFRunControlTabSystem);
	this->SelectTab(kDGFRunControlTabSystem);

//	create a message viewer window if wanted
	if (		gEnv->GetValue("DGFControl.ViewMessages", kFALSE)
			||	gEnv->GetValue("TGMrbMessageViewer.Popup", kFALSE)) {
					this->PopupMessageViewer();
	}

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-q", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.Connect("KeyPressed(Int_t, Int_t)", this->ClassName(), this, "KeyPressed(Int_t, Int_t)");
	
	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();

	SetWindowName("DGFControl - keep control over XIA DGF-4C modules");

	if (gDGFControlData->IsOffline()) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Info", "Running in OFFLINE mode", kMBIconExclamation);
		gMrbLog->Out()	<< "Running in OFFLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "", setblue);
	}
}

void DGFRunControlPanel::CloseWindow() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel::CloseWindow
// Purpose:        Close window and terminate application
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Got close message for this MainFrame.
//                 Calls parent's CloseWindow()
//                 (which destroys the window) and terminates the application.
//                 The close message is generated by the window manager when
//                 its close window menu item is selected.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

   TGMainFrame::CloseWindow();
   gApplication->Terminate(0);
}

void DGFRunControlPanel::PopupMessageViewer() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel::PopupMessageViewer
// Purpose:        Create a message viewer window
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fMsgViewer = new TGMrbMessageViewer(fClient->GetRoot(), this,
												"DGFControl: View (error) messages", 800, 300, gMrbLog, kTRUE, NULL);
	fMsgViewer->Disable(TMrbLogMessage::kMrbMsgMessage);
	fMsgViewer->Enable(TMrbLogMessage::kMrbMsgError);
	fMsgViewer->AddEntries();
}

void DGFRunControlPanel::Activate(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel::Activate
// Purpose:        Slot method: activate menu entry
// Arguments:      Int_t Selection    -- selected entry
// Results:        
// Exceptions:     
// Description:    Called if popup menu entry activated
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFViewErrors:
			if (fMsgViewer != NULL) delete fMsgViewer;
			this->PopupMessageViewer();
			break;
							
		case kDGFFileExit:
			this->CloseWindow();
			break;

		case kDGFGeneralOutputNormal:
			gDGFControlData->fStatus &= ~(DGFControlData::kDGFVerboseMode | DGFControlData::kDGFDebugMode);
			fMenuGeneral->RCheckEntry(kDGFGeneralOutputNormal, kDGFGeneralOutputNormal, kDGFGeneralOutputDebug);
			break;

		case kDGFGeneralOutputVerbose:
			gDGFControlData->fStatus &= ~DGFControlData::kDGFDebugMode;
			gDGFControlData->fStatus |= DGFControlData::kDGFVerboseMode;
			fMenuGeneral->RCheckEntry(kDGFGeneralOutputVerbose, kDGFGeneralOutputNormal, kDGFGeneralOutputDebug);
			break;

		case kDGFGeneralOutputDebug:
			gDGFControlData->fStatus |= DGFControlData::kDGFVerboseMode | DGFControlData::kDGFDebugMode;
			fMenuGeneral->RCheckEntry(kDGFGeneralOutputDebug, kDGFGeneralOutputNormal, kDGFGeneralOutputDebug);
			break;

		case kDGFGeneralModNumLocal:
			gDGFControlData->fStatus &= ~DGFControlData::kDGFModNumGlobal;
			fMenuGeneral->RCheckEntry(kDGFGeneralModNumLocal, kDGFGeneralModNumLocal, kDGFGeneralModNumGlobal);
			break;

		case kDGFGeneralModNumGlobal:
			gDGFControlData->fStatus |= DGFControlData::kDGFModNumGlobal;
			fMenuGeneral->RCheckEntry(kDGFGeneralModNumGlobal, kDGFGeneralModNumLocal, kDGFGeneralModNumGlobal);
			break;

		case kDGFGeneralOffline:
			gMrbLog->Out()	<< "Running in OFFLINE mode" << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessMessage", setblue);
			gDGFControlData->fStatus |= DGFControlData::kDGFOfflineMode;
			fMenuGeneral->RCheckEntry(kDGFGeneralOffline, kDGFGeneralOffline, kDGFGeneralOnline);
			break;

		case kDGFGeneralOnline:
			gMrbLog->Out()	<< "Running in ONLINE mode" << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessMessage", setblue);
			gDGFControlData->fStatus &= ~DGFControlData::kDGFOfflineMode;
			fMenuGeneral->RCheckEntry(kDGFGeneralOnline, kDGFGeneralOffline, kDGFGeneralOnline);
			break;

		case kDGFGeneralEsoneNormal:
			gMrbLog->Out()	<< "ESONE running in NORMAL mode" << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessMessage", setblue);
			gDGFControlData->fStatus &= ~DGFControlData::kDGFEsoneSingleStepMode;
			fMenuGeneral->RCheckEntry(kDGFGeneralEsoneNormal, kDGFGeneralEsoneSingleStep, kDGFGeneralEsoneNormal);
			break;

		case kDGFGeneralEsoneSingleStep:
			gMrbLog->Out()	<< "ESONE running in SINGLE STEP mode" << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessMessage", setblue);
			gDGFControlData->fStatus |= DGFControlData::kDGFEsoneSingleStepMode;
			fMenuGeneral->RCheckEntry(kDGFGeneralEsoneSingleStep, kDGFGeneralEsoneNormal, kDGFGeneralEsoneNormal);
			break;
	}
}

void DGFRunControlPanel::SelectTab(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel::SelectTab
// Purpose:        Slot method: select a tab
// Arguments:      Int_t Selection    -- selected tab
// Results:        
// Exceptions:     
// Description:    Called if a tab is being selected
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	for (Int_t i = kDGFRunControlTabSystem; i <= kDGFRunControlTabFiles; i++) {
		fRunControlTab->GetTabTab(i)->ChangeBackground(gDGFControlData->fColorGray);
	}
	fRunControlTab->GetTabTab(Selection)->ChangeBackground(gDGFControlData->fColorGold);

	if (fMcaDisplayPanel) {
		if (Selection == kDGFRunControlTabMCA || Selection == kDGFRunControlTabCptm) {
			fMcaDisplayPanel->McaResume();
		} else {
			fMcaDisplayPanel->McaPause();
		}
	}

	gDGFControlData->UpdateParamsAndFPGAs();

	switch (Selection) {

		case kDGFRunControlTabSystem:
			if (fSetupPanel == NULL) fSetupPanel = new DGFSetupPanel(fSystemTab);
			break;

		case kDGFRunControlTabFiles:
			if (fSetFilesPanel == NULL) fSetFilesPanel = new DGFSetFilesPanel(fFilesTab);
			break;

		case kDGFRunControlTabParams:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fParamsPanel == NULL) fParamsPanel = new DGFParamsPanel(fParamsTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabModules:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fInstrumentPanel == NULL) fInstrumentPanel = new DGFInstrumentPanel(fModulesTab);
				else fInstrumentPanel->InitializeValues(kFALSE);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabTrace:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fTraceDisplayPanel == NULL) fTraceDisplayPanel = new DGFTraceDisplayPanel(fTracesTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabUntrigTrace:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fUntrigTracePanel == NULL) fUntrigTracePanel = new DGFUntrigTracePanel(fUntrigTracesTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabOffsets:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fOffsetsPanel == NULL) fOffsetsPanel = new DGFOffsetsPanel(fOffsetsTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabMCA:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fMcaDisplayPanel == NULL) fMcaDisplayPanel = new DGFMcaDisplayPanel(fMCATab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabTauFit1:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fTauDisplayPanel == NULL) fTauDisplayPanel = new DGFTauDisplayPanel(fTauDisplayTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabTauFit2:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fTauFitPanel == NULL) fTauFitPanel = new DGFTauFitPanel(fTauFitTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabSave:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fSaveModuleSettingsPanel == NULL) fSaveModuleSettingsPanel = new DGFSaveModuleSettingsPanel(fSaveTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabRestore:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fRestoreModuleSettingsPanel == NULL) fRestoreModuleSettingsPanel = new DGFRestoreModuleSettingsPanel(fRestoreTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabCopy:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fCopyModuleSettingsPanel == NULL) fCopyModuleSettingsPanel = new DGFCopyModuleSettingsPanel(fCopyTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabMisc:
			if (gDGFControlData->IsOffline() || gDGFControlData->CheckIfStarted()) {
				if (fMiscPanel == NULL) fMiscPanel = new DGFMiscPanel(fMiscTab);
			} else {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
			}
			break;

		case kDGFRunControlTabCptm:
			if (fCptmPanel == NULL) fCptmPanel = new DGFCptmPanel(fCptmTab);
			if (fCptmPanel->GetNofCptmModules() == 0) {
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "No CPTM module found", kMBIconStop);
			} else {
				fCptmPanel->InitializeValues();
			}
			break;
	}
}

void DGFRunControlPanel::KeyPressed(Int_t FrameId, Int_t Key) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel::KeyPressed
// Purpose:        Slot method: handle special keys
// Arguments:      Int_t FrameId   -- frame issuing this signal
//                 Int_t Key       -- key code
// Results:        
// Exceptions:     
// Description:    Called if a special (=control) char has been typed
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Key) {
		case TGMrbLofKeyBindings::kGMrbKeyActionExit:
			gApplication->Terminate(0);
			break;
	}
}
