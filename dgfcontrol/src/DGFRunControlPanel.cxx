//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFRunControlPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Run Control
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
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

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

#include "TEnv.h"
#include "TFile.h"
#include "TObjString.h"
#include "TApplication.h"
#include "TGFileDialog.h"

#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "DGFRunControlPanel.h"
#include "DGFSetupPanel.h"
#include "DGFSetFilesPanel.h"
#include "DGFInstrumentPanel.h"
#include "DGFParamsPanel.h"
#include "DGFTauDisplayPanel.h"
#include "DGFUntrigTracePanel.h"
#include "DGFTraceDisplayPanel.h"
#include "DGFOffsetsPanel.h"
#include "DGFMcaDisplayPanel.h"
#include "DGFRestoreModuleSettingsPanel.h"
#include "DGFSaveModuleSettingsPanel.h"
#include "DGFControlData.h"
#include "DGFControlCommon.h"

#include "SetColor.h"

const SMrbNamedX kDGFRunControlSettingsButtons[] =
			{
				{DGFRunControlPanel::kDGFRunControlSettingsSystem,		"System",	"Define and initialize DGF modules" },
				{DGFRunControlPanel::kDGFRunControlSettingsFiles,		"Files",	"Define path names and files"	},
				{DGFRunControlPanel::kDGFRunControlSettingsModules,		"Module",	"Set module parameters" 		},
				{DGFRunControlPanel::kDGFRunControlSettingsParams,		"Params",	"Set parameters"		 		},
				{0, 													NULL,		NULL							}
			};

const SMrbNamedX kDGFRunControlCalibrateButtons[] =
			{
				{DGFRunControlPanel::kDGFRunControlCalibrateTauTrace,		"Tau",		"Calculate tau value"				},
				{DGFRunControlPanel::kDGFRunControlCalibrateTrace,			"Trace",	"Take trace"				},
				{DGFRunControlPanel::kDGFRunControlCalibrateUntrigTrace,	"Untrig Trace",		"Take untriggered trace"	},
				{DGFRunControlPanel::kDGFRunControlCalibrateOffsets,		"Offsets",	"Modify offsets"					},
				{0, 														NULL,		NULL								}
			};

const SMrbNamedX kDGFRunControlRunButtons[] =
			{
				{DGFRunControlPanel::kDGFRunControlRunEvents,			"Events",		"Start a LISTMODE run"		},
				{DGFRunControlPanel::kDGFRunControlRunMCA,				"MCA",			"Start a MCA run"			},
				{DGFRunControlPanel::kDGFRunControlRunStatistics,		"Statistics",	"Display run statistics"	},
				{0, 													NULL,			NULL						}
			};

const SMrbNamedX kDGFRunControlDatabaseButtons[] =
			{
				{DGFRunControlPanel::kDGFRunControlDatabaseSave,		"Save", 	"Save current settings to ROOT file" },
				{DGFRunControlPanel::kDGFRunControlDatabaseRestore,		"Restore",	"Restore previously saved settings" },
				{0, 													NULL,		NULL							}
			};

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

	//	Initialize several lists
	fRunControlSettings.SetName("Edit Settings");		// button list: edit settings
	fRunControlSettings.AddNamedX(kDGFRunControlSettingsButtons);
	fRunControlDatabase.SetName("Database");			//				database
	fRunControlDatabase.AddNamedX(kDGFRunControlDatabaseButtons);
	fRunControlCalibrate.SetName("Calibrate");			//				calibrate
	fRunControlCalibrate.AddNamedX(kDGFRunControlCalibrateButtons);
	fRunControlRun.SetName("Run");						//				run
	fRunControlRun.AddNamedX(kDGFRunControlRunButtons);

//	Create popup menus.

//	File menu
	fMenuFile = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuFile);

	fMenuFile->AddEntry("Save ... Ctrl-s", kDGFFileSave);
	fMenuFile->AddEntry("Exit ... Ctrl-q", kDGFFileExit);

//	View menu
	fMenuView = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuView);

	fMenuView->AddEntry("&Errors", kDGFViewErrors);

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

//	Macros menu
	fLofMacros = new TMrbLofMacros();
	HEAP(fLofMacros);
	fLofMacros->AddMacro("*.C");
	hasMacros = (fLofMacros->GetLast() >= 0);

	if (hasMacros) {
		fMenuMacros = new TGMrbMacroBrowserPopup(fClient->GetRoot(), fLofMacros);
		HEAP(fMenuMacros);
	}

//	Help menu
	fMenuHelp = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuHelp);

	fMenuHelp->AddEntry("&Contents", kDGFHelpContents);
	fMenuHelp->AddSeparator();
	fMenuHelp->AddEntry("&About DGFControl", kDGFHelpAbout);

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

	fMenuBar->ChangeBackground(gDGFControlData->fColorDarkBlue);

//	Vertical frame to place run control buttons
	fRunControlFrame = new TGVerticalFrame(this, kFrameWidth, kAutoHeight);
	HEAP(fRunControlFrame);
	TGLayoutHints * vframeLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5);
	HEAP(vframeLayout);
	this->AddFrame(fRunControlFrame, vframeLayout);
	fRunControlFrame->ChangeBackground(gDGFControlData->fColorGold);


//	Run control: edit settings
	TGLayoutHints * sFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 10, 1);
	gDGFControlData->SetLH(groupGC, frameGC, sFrameLayout);
	HEAP(sFrameLayout);
	TGLayoutHints * sButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 10, 1);
	buttonGC->SetLH(sButtonLayout);
	HEAP(sButtonLayout);
	fSettingsFrame = new TGMrbTextButtonGroup(fRunControlFrame, "Edit Settings", &fRunControlSettings, 1, groupGC, buttonGC);
	HEAP(fSettingsFrame);
	fRunControlFrame->AddFrame(fSettingsFrame, groupGC->LH());
	fSettingsFrame->JustifyButton(kTextCenterX);
	fSettingsFrame->Associate(this);

//	Run control: calibrate
	TGLayoutHints * cFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 10, 1);
	gDGFControlData->SetLH(groupGC, frameGC, cFrameLayout);
	HEAP(cFrameLayout);
	TGLayoutHints * cButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 10, 1);
	buttonGC->SetLH(cButtonLayout);
	HEAP(cButtonLayout);
	fCalibrateFrame = new TGMrbTextButtonGroup(fRunControlFrame, "Calibrate", &fRunControlCalibrate, 1, groupGC, buttonGC);
	HEAP(fCalibrateFrame);
	fRunControlFrame->AddFrame(fCalibrateFrame, groupGC->LH());
	fCalibrateFrame->JustifyButton(kTextCenterX);
	fCalibrateFrame->Associate(this);

//	Run control: run
	TGLayoutHints * rFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 10, 1);
	gDGFControlData->SetLH(groupGC, frameGC, rFrameLayout);
	HEAP(rFrameLayout);
	TGLayoutHints * rButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 10, 1);
	buttonGC->SetLH(rButtonLayout);
	HEAP(rButtonLayout);
	fRunFrame = new TGMrbTextButtonGroup(fRunControlFrame, "Run", &fRunControlRun, 1, groupGC, buttonGC);
	HEAP(fRunFrame);
	fRunControlFrame->AddFrame(fRunFrame, groupGC->LH());
	fRunFrame->JustifyButton(kTextCenterX);
	fRunFrame->Associate(this);

//	Run control: database
	TGLayoutHints * dbFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 10, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dbFrameLayout);
	HEAP(dbFrameLayout);
	TGLayoutHints * dbButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 10, 1);
	buttonGC->SetLH(dbButtonLayout);
	HEAP(dbButtonLayout);

	fDatabaseFrame = new TGMrbTextButtonGroup(fRunControlFrame, "Database", &fRunControlDatabase, 1, groupGC, buttonGC);
	HEAP(fDatabaseFrame);
	fRunControlFrame->AddFrame(fDatabaseFrame, groupGC->LH());
	fDatabaseFrame->JustifyButton(kTextCenterX);
	fDatabaseFrame->Associate(this);

	this->ChangeBackground(gDGFControlData->fColorGold);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-q", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.BindKey("Ctrl-s", TGMrbLofKeyBindings::kGMrbKeyActionSave);
	
	SetWindowName("DGFControl: XIA DGF-4C Control");

//	create a message viewer window if wanted
	if (		gEnv->GetValue("DGFControl.ViewMessages", kFALSE)
			||	gEnv->GetValue("TGMrbMessageViewer.Popup", kFALSE)) {
					this->PopupMessageViewer();
	}

	MapSubwindows();

//	we need to use GetDefault...() to initialize the layout algorithm...
	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();

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

Bool_t DGFRunControlPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRunControlPanel::ProcessMessage
// Purpose:        Message handler for the main frame widget
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFRunControlPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_MENU:
					switch (Param1) {
						case kDGFViewErrors:
							if (fMsgViewer != NULL) delete fMsgViewer;
							this->PopupMessageViewer();
							break;
							
						case kDGFFileExit:
							this->CloseWindow();
							break;

						case kDGFFileSave:
							if (gDGFControlData->CheckIfStarted()) {
								new DGFSaveModuleSettingsPanel(fClient->GetRoot(), this, DGFSaveModuleSettingsPanel::kFrameWidth, DGFSaveModuleSettingsPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
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

						default:
							break;
					}
					break;

				case kCM_BUTTON:
					switch (Param1) {

						// edit settings
						case kDGFRunControlSettingsSystem:
                    		new DGFSetupPanel(fClient->GetRoot(), this, DGFSetupPanel::kFrameWidth, DGFSetupPanel::kFrameHeight);
							break;
						case kDGFRunControlSettingsFiles:
                    		new DGFSetFilesPanel(fClient->GetRoot(), this, DGFSetFilesPanel::kFrameWidth, DGFSetFilesPanel::kFrameHeight);
							break;
						case kDGFRunControlSettingsParams:
							if (gDGFControlData->CheckIfStarted()) {
                    			new DGFParamsPanel(fClient->GetRoot(), this, DGFParamsPanel::kFrameWidth, DGFParamsPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;
						case kDGFRunControlSettingsModules:
							if (gDGFControlData->CheckIfStarted()) {
                    			new DGFInstrumentPanel(fClient->GetRoot(), this, DGFInstrumentPanel::kFrameWidth, DGFInstrumentPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;

						// calibrate
						case kDGFRunControlCalibrateTauTrace:
							if (gDGFControlData->CheckIfStarted()) {
	                   			new DGFTauDisplayPanel(fClient->GetRoot(), this, DGFTauDisplayPanel::kFrameWidth, DGFTauDisplayPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;
						case kDGFRunControlCalibrateTrace:
							if (gDGFControlData->CheckIfStarted()) {
	                   			new DGFTraceDisplayPanel(fClient->GetRoot(), this, DGFTraceDisplayPanel::kFrameWidth, DGFTraceDisplayPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;
						case kDGFRunControlCalibrateUntrigTrace:
							if (gDGFControlData->CheckIfStarted()) {
	                   			new DGFUntrigTracePanel(fClient->GetRoot(), this, DGFUntrigTracePanel::kFrameWidth, DGFUntrigTracePanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;
						case kDGFRunControlCalibrateOffsets:
							if (gDGFControlData->CheckIfStarted()) {
	                   			new DGFOffsetsPanel(fClient->GetRoot(), this, DGFOffsetsPanel::kFrameWidth, DGFOffsetsPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;

						// run
						case kDGFRunControlRunEvents:
							break;
						case kDGFRunControlRunMCA:
							if (gDGFControlData->CheckIfStarted()) {
	                   			new DGFMcaDisplayPanel(fClient->GetRoot(), this, DGFMcaDisplayPanel::kFrameWidth, DGFMcaDisplayPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;
						case kDGFRunControlRunStatistics:
							break;

						// database
						case kDGFRunControlDatabaseSave:
							if (gDGFControlData->CheckIfStarted()) {
	                   			new DGFSaveModuleSettingsPanel(fClient->GetRoot(), this, DGFSaveModuleSettingsPanel::kFrameWidth, DGFSaveModuleSettingsPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;
						case kDGFRunControlDatabaseRestore:
							if (gDGFControlData->CheckIfStarted()) {
	                   			new DGFRestoreModuleSettingsPanel(fClient->GetRoot(), this, DGFRestoreModuleSettingsPanel::kFrameWidth, DGFRestoreModuleSettingsPanel::kFrameHeight);
							} else {
								new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
							}
							break;
						// close
						case kDGFRunControlClose:
							this->CloseWindow();
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
				case TGMrbLofKeyBindings::kGMrbKeyActionSave:
					if (gDGFControlData->CheckIfStarted()) {
						new DGFSaveModuleSettingsPanel(fClient->GetRoot(), this, DGFSaveModuleSettingsPanel::kFrameWidth, DGFSaveModuleSettingsPanel::kFrameHeight);
					} else {
						new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DGF module(s) not started", kMBIconStop);
					}
					break;
				default:
					break;
			}
			break;
	}
	return(kTRUE);
}
