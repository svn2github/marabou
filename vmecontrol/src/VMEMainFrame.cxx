//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMEMainFrame
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Run Control
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEMainFrame.cxx,v 1.11 2011-07-26 08:41:49 Marabou Exp $
// Date:
// URL:
// Keywords:
// Layout:
//Begin_Html
/*
<img src=VMEControl/VMEMainFrame.gif>
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
#include "TMrbC2Lynx.h"

#include "VMEControlData.h"
#include "VMEMainFrame.h"

#include "SetColor.h"

VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;
extern TMrbC2Lynx * gMrbC2Lynx;

ClassImp(VMEMainFrame)

VMEMainFrame::VMEMainFrame(const TGWindow * Window, UInt_t Width, UInt_t Height)
      																: TGMainFrame(Window, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame
// Purpose:        Main frame
// Arguments:      TGWindow Window      -- connection to ROOT graphics
//                 UInt_t Width         -- window width in pixels
//                 UInt_t Height        -- window height in pixels
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	fMsgViewer = NULL;

	TGMrbLayout * buttonGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGray);	HEAP(buttonGC);

	TGMrbLayout * frameGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGold);	HEAP(frameGC);

	TGMrbLayout * groupGC = new TGMrbLayout(	gVMEControlData->SlantedFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGold);	HEAP(groupGC);

//	Create popup menus.

//	File menu
	fMenuFile = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuFile);

	fMenuFile->AddEntry("&Exit ... Ctrl-q", kVMEFileExit);
	fMenuFile->Connect("Activated(Int_t)", this->ClassName(), this, "MenuSelect(Int_t)");

//	View menu
	fMenuView = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuView);

	fMenuView->AddEntry("&Errors", kVMEViewErrors);
	fMenuView->Connect("Activated(Int_t)", this->ClassName(), this, "MenuSelect(Int_t)");

//	Settings menu
	fMenuSettings = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuSettings);

	fMenuSettings->AddEntry("&Normal", kVMESettingsOutputNormal);
	fMenuSettings->AddEntry("&Verbose", kVMESettingsOutputVerbose);
	fMenuSettings->AddEntry("&Debug", kVMESettingsOutputDebug);
	fMenuSettings->AddEntry("&Debug and Stop", kVMESettingsOutputDebugStop);

	if (gVMEControlData->fStatus & VMEControlData::kVMEVerboseMode) {
		fMenuSettings->RCheckEntry(kVMESettingsOutputVerbose, kVMESettingsOutputNormal, kVMESettingsOutputDebugStop);
	} else if (gVMEControlData->fStatus & VMEControlData::kVMEDebugMode) {
		fMenuSettings->RCheckEntry(kVMESettingsOutputDebug, kVMESettingsOutputNormal, kVMESettingsOutputDebugStop);
	} else if (gVMEControlData->fStatus & VMEControlData::kVMEDebugStopMode) {
		fMenuSettings->RCheckEntry(kVMESettingsOutputDebugStop, kVMESettingsOutputNormal, kVMESettingsOutputDebugStop);
	} else {
		fMenuSettings->RCheckEntry(kVMESettingsOutputNormal, kVMESettingsOutputNormal, kVMESettingsOutputDebugStop);
	}

	fMenuSettings->AddSeparator();

	fMenuSettings->AddEntry("Offline", kVMESettingsOffline);
	fMenuSettings->AddEntry("Online", kVMESettingsOnline);
	fMenuSettings->Connect("Activated(Int_t)", this->ClassName(), this, "MenuSelect(Int_t)");

	if (gVMEControlData->fStatus & VMEControlData::kVMEOfflineMode) {
		fMenuSettings->RCheckEntry(kVMESettingsOffline, kVMESettingsOffline, kVMESettingsOnline);
	} else {
		fMenuSettings->RCheckEntry(kVMESettingsOnline, kVMESettingsOffline, kVMESettingsOnline);
	}

			//	Macros menu
	fLofMacros = new TMrbLofMacros();
	HEAP(fLofMacros);
	fLofMacros->AddMacro("*.C", "VME");
	Bool_t hasMacros = (fLofMacros->GetLast() >= 0);

	if (hasMacros) {
		fMenuMacros = new TGMrbMacroBrowserPopup(fClient->GetRoot(), fLofMacros);
		HEAP(fMenuMacros);
	}

//	Help menu
	fMenuHelp = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuHelp);

	fMenuHelp->AddEntry("&Contents", kVMEHelpContents);
	fMenuHelp->AddSeparator();
	fMenuHelp->AddEntry("&About VMEControl", kVMEHelpAbout);
	fMenuHelp->Connect("Activated(Int_t)", this->ClassName(), this, "MenuSelect(Int_t)");

//	Main menu bar
	fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame | kSunkenFrame);
	HEAP(fMenuBar);

	TGLayoutHints * menuBarFileLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarFileLayout);
	fMenuBar->AddPopup("&File", fMenuFile, menuBarFileLayout);

	TGLayoutHints * menuBarViewLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarViewLayout);
	fMenuBar->AddPopup("&View", fMenuView, menuBarViewLayout);

	TGLayoutHints * menuBarSettingsLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarSettingsLayout);
	fMenuBar->AddPopup("&Settings", fMenuSettings, menuBarSettingsLayout);

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

	fMenuBar->ChangeBackground(gVMEControlData->fColorBlue);

//	create main tab object
	fTabFrame = new TGTab(this, gVMEControlData->GetFrameWidth(), gVMEControlData->GetFrameHeight());
	HEAP(fTabFrame);
	TGLayoutHints * tabLayout = new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
	HEAP(tabLayout);
	this->AddFrame(fTabFrame, tabLayout);
	fTabFrame->Connect("Selected(Int_t)", this->ClassName(), this, "TabChanged(Int_t)");

// add tabs
	fServerPanel = NULL;
	fSis3302Panel = NULL;
	fCaen785Panel = NULL;

	fServerTab = fTabFrame->AddTab("Server");
	fSis3302Tab = fTabFrame->AddTab("Sis 3302");
	fCaen785Tab = fTabFrame->AddTab("Caen V785");
	fVulomTBTab = fTabFrame->AddTab("Vulom/TB");

	fTabFrame->SetTab(VMEControlData::kVMETabServer);
	this->TabChanged(VMEControlData::kVMETabServer);

//	create a message viewer window if wanted
	if (		gEnv->GetValue("VMEControl.ViewMessages", kFALSE)
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

	SetWindowName("VMEControl - keep control over VME modules");

	if (gVMEControlData->IsOffline()) {
		new TGMsgBox(fClient->GetRoot(), this, "VMEControl: Info", "Running in OFFLINE mode", kMBIconExclamation);
		gMrbLog->Out()	<< "Running in OFFLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "", setblue);
	}
}

void VMEMainFrame::CloseWindow() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame::CloseWindow
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

void VMEMainFrame::PopupMessageViewer() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame::PopupMessageViewer
// Purpose:        Create a message viewer window
// Arguments:
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fMsgViewer = new TGMrbMessageViewer(fClient->GetRoot(), this,
												"VMEControl: View (error) messages", 800, 300, gMrbLog, kTRUE, NULL);
	fMsgViewer->Disable(TMrbLogMessage::kMrbMsgMessage);
	fMsgViewer->Enable(TMrbLogMessage::kMrbMsgError);
	fMsgViewer->AddEntries();
}

void VMEMainFrame::MenuSelect(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame::MenuSelect
// Purpose:        Slot method
// Arguments:      Int_t Selection    -- selection index
// Results:        --
// Exceptions:
// Description:    Called upon 'menu select' events
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kVMEViewErrors:
			if (fMsgViewer != NULL) delete fMsgViewer;
			this->PopupMessageViewer();
			break;

		case kVMEFileExit:
			if (gMrbC2Lynx) gMrbC2Lynx->Bye();
			this->CloseWindow();
			break;

		case kVMESettingsOutputNormal:
			gVMEControlData->fStatus &= ~(VMEControlData::kVMEVerboseMode | VMEControlData::kVMEDebugMode | VMEControlData::kVMEDebugStopMode);
			fMenuSettings->RCheckEntry(kVMESettingsOutputNormal, kVMESettingsOutputNormal, kVMESettingsOutputDebugStop);
			break;

		case kVMESettingsOutputVerbose:
			gVMEControlData->fStatus &= ~(VMEControlData::kVMEDebugMode | VMEControlData::kVMEDebugStopMode);
			gVMEControlData->fStatus |= VMEControlData::kVMEVerboseMode;
			fMenuSettings->RCheckEntry(kVMESettingsOutputVerbose, kVMESettingsOutputNormal, kVMESettingsOutputDebugStop);
			break;

		case kVMESettingsOutputDebug:
			gVMEControlData->fStatus &= ~VMEControlData::kVMEDebugStopMode;
			gVMEControlData->fStatus |= VMEControlData::kVMEVerboseMode | VMEControlData::kVMEDebugMode;
			fMenuSettings->RCheckEntry(kVMESettingsOutputDebug, kVMESettingsOutputNormal, kVMESettingsOutputDebugStop);
			break;

		case kVMESettingsOutputDebugStop:
			gVMEControlData->fStatus |= VMEControlData::kVMEVerboseMode | VMEControlData::kVMEDebugMode | VMEControlData::kVMEDebugStopMode;
			fMenuSettings->RCheckEntry(kVMESettingsOutputDebugStop, kVMESettingsOutputNormal, kVMESettingsOutputDebugStop);
			break;

		case kVMESettingsOffline:
			gMrbLog->Out()	<< "Running in OFFLINE mode" << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessMessage", setblue);
			gVMEControlData->fStatus |= VMEControlData::kVMEOfflineMode;
			fMenuSettings->RCheckEntry(kVMESettingsOffline, kVMESettingsOffline, kVMESettingsOnline);
			break;

		case kVMESettingsOnline:
			gMrbLog->Out()	<< "Running in ONLINE mode" << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessMessage", setblue);
			gVMEControlData->fStatus &= ~VMEControlData::kVMEOfflineMode;
			fMenuSettings->RCheckEntry(kVMESettingsOnline, kVMESettingsOffline, kVMESettingsOnline);
			break;
	}
}

void VMEMainFrame::TabChanged(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame::TabChanged
// Purpose:        Slot method
// Arguments:      Int_t Selection    -- selection index
// Results:        --
// Exceptions:
// Description:    Called upon 'tab select' events
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	for (Int_t i = VMEControlData::kVMETabServer; i < VMEControlData::kVMELastTab; i++) {
		fTabFrame->GetTabTab(i)->ChangeBackground(gVMEControlData->fColorGray);
	}
	fTabFrame->GetTabTab(Selection)->ChangeBackground(gVMEControlData->fColorGold);

	switch (Selection) {
	  case VMEControlData::kVMETabServer:
			if (fServerPanel == NULL) {
				fServerPanel = new VMEServerPanel(fServerTab);
				gVMEControlData->AddToLofPanels(fServerPanel, VMEControlData::kVMETabServer);
			}
			break;

		case VMEControlData::kVMETabSis3302:
			if (fSis3302Panel == NULL) {
				fSis3302Panel = new VMESis3302Panel(fSis3302Tab);
				gVMEControlData->AddToLofPanels(fSis3302Panel, VMEControlData::kVMETabSis3302);
			}
			fSis3302Panel->UpdateGUI();
			break;

		case VMEControlData::kVMETabCaen785:
			if (fCaen785Panel == NULL) {
				fCaen785Panel = new VMECaen785Panel(fCaen785Tab);
				gVMEControlData->AddToLofPanels(fCaen785Panel, VMEControlData::kVMETabCaen785);
			}
			break;

		case VMEControlData::kVMETabVulomTB:
			if (fVulomTBPanel == NULL) {
				fVulomTBPanel = new VMEVulomTBPanel(fVulomTBTab);
				gVMEControlData->AddToLofPanels(fVulomTBPanel, VMEControlData::kVMETabVulomTB);
			}
			break;
	}
}

void VMEMainFrame::KeyPressed(Int_t FrameId, Int_t Key) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame::KeyPressed
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
			if (gMrbC2Lynx) gMrbC2Lynx->Bye();
			gApplication->Terminate(0);
			break;
	}
}
