//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMEMainFrame
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Run Control
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEMainFrame.cxx,v 1.1 2008-08-28 07:16:48 Rudolf.Lutter Exp $       
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

#include "VMEControlData.h"
#include "VMEMainFrame.h"

#include "SetColor.h"

VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

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

//	View menu
	fMenuView = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuView);

	fMenuView->AddEntry("&Errors", kVMEViewErrors);

//	General menu
	fMenuGeneral = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuGeneral);

	fMenuGeneral->AddEntry("&Normal", kVMEGeneralOutputNormal);
	fMenuGeneral->AddEntry("&Verbose", kVMEGeneralOutputVerbose);
	fMenuGeneral->AddEntry("&Debug (very verbose)", kVMEGeneralOutputDebug);
	fMenuGeneral->AddSeparator();

	fMenuGeneral->AddEntry("Offline", kVMEGeneralOffline);
	fMenuGeneral->AddEntry("Online", kVMEGeneralOnline);

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

//	Menu button messages are handled by the main frame (i.e. "this")
//	ProcessMessage() method.
//	(fMenuMacros will be handled by its own ProcessMessage())
	fMenuFile->Associate(this);
	fMenuView->Associate(this);
	fMenuGeneral->Associate(this);
	fMenuHelp->Associate(this);
	if (hasMacros) fMenuMacros->Associate(fMenuMacros);

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

	fMenuBar->ChangeBackground(gVMEControlData->fColorDarkBlue);

//	create main tab object
	fSystemTab = new TGTab(this, kTabWidth, kTabHeight);
	HEAP(fSystemTab);
	TGLayoutHints * tabLayout = new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);
	HEAP(tabLayout);
	this->AddFrame(fSystemTab, tabLayout);

// add tabs
	fServerPanel = NULL;
//	fModulesPanel = NULL;
//	fSaveSettingsPanel = NULL;
//	fRestoreSettingsPanel = NULL;
//	fCopySettingsPanel = NULL;

	fServerTab = fSystemTab->AddTab("Server");
//	fModulesTab = fSystemTab->AddTab("Modules");
//	fSaveTab = fSystemTab->AddTab("Save");
//	fRestoreTab = fSystemTab->AddTab("Restore");
//	fCopyTab = fSystemTab->AddTab("Copy");

	fSystemTab->SetTab(kVMEMainFrameTabServer);
	this->SendMessage(this, MK_MSG(kC_COMMAND, kCM_TAB), kVMEMainFrameTabServer, 0);

//	create a message viewer window if wanted
	if (		gEnv->GetValue("VMEControl.ViewMessages", kFALSE)
			||	gEnv->GetValue("TGMrbMessageViewer.Popup", kFALSE)) {
					this->PopupMessageViewer();
	}

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

Bool_t VMEMainFrame::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEMainFrame::ProcessMessage
// Purpose:        Message handler for the main frame widget
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to VMEMainFrame.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_MENU:
					switch (Param1) {
						case kVMEViewErrors:
							if (fMsgViewer != NULL) delete fMsgViewer;
							this->PopupMessageViewer();
							break;
							
						case kVMEFileExit:
							this->CloseWindow();
							break;

						case kVMEGeneralOutputNormal:
							gVMEControlData->fStatus &= ~(VMEControlData::kVMEVerboseMode | VMEControlData::kVMEDebugMode);
							fMenuGeneral->RCheckEntry(kVMEGeneralOutputNormal, kVMEGeneralOutputNormal, kVMEGeneralOutputDebug);
							break;

						case kVMEGeneralOutputVerbose:
							gVMEControlData->fStatus &= ~VMEControlData::kVMEDebugMode;
							gVMEControlData->fStatus |= VMEControlData::kVMEVerboseMode;
							fMenuGeneral->RCheckEntry(kVMEGeneralOutputVerbose, kVMEGeneralOutputNormal, kVMEGeneralOutputDebug);
							break;

						case kVMEGeneralOutputDebug:
							gVMEControlData->fStatus |= VMEControlData::kVMEVerboseMode | VMEControlData::kVMEDebugMode;
							fMenuGeneral->RCheckEntry(kVMEGeneralOutputDebug, kVMEGeneralOutputNormal, kVMEGeneralOutputDebug);
							break;

						case kVMEGeneralOffline:
							gMrbLog->Out()	<< "Running in OFFLINE mode" << endl;
							gMrbLog->Flush(this->ClassName(), "ProcessMessage", setblue);
							gVMEControlData->fStatus |= VMEControlData::kVMEOfflineMode;
							fMenuGeneral->RCheckEntry(kVMEGeneralOffline, kVMEGeneralOffline, kVMEGeneralOnline);
							break;

						case kVMEGeneralOnline:
							gMrbLog->Out()	<< "Running in ONLINE mode" << endl;
							gMrbLog->Flush(this->ClassName(), "ProcessMessage", setblue);
							gVMEControlData->fStatus &= ~VMEControlData::kVMEOfflineMode;
							fMenuGeneral->RCheckEntry(kVMEGeneralOnline, kVMEGeneralOffline, kVMEGeneralOnline);
							break;

						default:
							break;
					}
					break;

				case kCM_TAB:
					for (Int_t i = kVMEMainFrameTabServer; i <= kVMEMainFrameTabServer; i++) {
						fSystemTab->GetTabTab(i)->ChangeBackground(gVMEControlData->fColorGray);
					}
					fSystemTab->GetTabTab(Param1)->ChangeBackground(gVMEControlData->fColorGold);

					switch (Param1) {

						case kVMEMainFrameTabServer:
                    		if (fServerPanel == NULL) fServerPanel = new VMEServerPanel(fServerTab);
							break;

						default:
							break;
					}
					break;
			}
			break;
			
		case kC_KEY:
			switch (Param1) {
				case TGMrbLofKeyBindings::kGMrbKeyActionExit:
					gApplication->Terminate(0);
					break;
				default:
					break;
			}
			break;
	}
	return(kTRUE);
}
