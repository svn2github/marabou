//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           snake/src/DDAControlPanel.cxx
// Purpose:        A GUI to control the DDA0816 device
// Description:    
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DDAControlPanel.cxx,v 1.2 2011-08-31 12:49:07 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "TFile.h"
#include "TGFileDialog.h"
#include "TGLabel.h"
#include "TGMsgBox.h"

#include "TMrbEnv.h"
#include "TMrbSystem.h"
#include "TMrbLogger.h"

#include "DDAControlPanel.h"

#include "TSnkDDA0816.h"
#include "TSnkDDACommon.h"

#include "SetColor.h"

ClassImp(DDAControlPanel)

extern TSnkDDA0816 * curDDA;

const SMrbNamedX kSnkLofSubdevs[] =
							{
								{kDDASubdevA,			"A (0-3)",	"Block A: magnetic deflection (X,Y,Hyst,Beam)"	 	},
								{kDDASubdevB, 			"B (4-7)",	"Block B: electrostatic deflection (X,Y,Hyst,Beam)"	},
								{kDDASubdevC,			"C (8-11)", "Block C: unused (8-11)"							},
								{kDDASubdevD,			"D (12-15)","Block D: unused (12-15)"						 	},
								{0, 					NULL,		NULL												}
							};

const SMrbNamedX kSnkLofControlButtons[] =
							{
								{kDDAControlCheck,		"Check",			"Check settings" 					},
								{kDDAControlConnect,	"Conn", 			"Connect to server"					},
								{kDDAControlStartScan,	"Start",			"Start a new scan"					},
								{kDDAControlStopScan,	"Stop", 	 		"Stop current scan" 				},
								{kDDAControlKillServer,	"Kill", 			"Stop server program" 				},
								{0, 					NULL,				NULL								}
							};

const SMrbNamedX kSnkLofSoftScaleButtons[] =
							{
								{kDDASoftScaleOff,		"Off",			"No fast channel defined"			},
								{kDDASoftScaleX,		"X",			"Fast channel X"					},
								{kDDASoftScaleY,		"Y",			"Fast channel Y"					},
								{0, 					NULL,			NULL								}
							};

const SMrbNamedX kSnkLofFileButtons[] =
							{
								{kDDAPictExit,			"exit.xpm", 	"Exit DDAControl" 	},
								{kDDAPictOpen,			"open.xpm", 	"Read previously saved settings from file" 	},
								{kDDAPictSave,			"save.xpm", 	"Save current settings to file" 			},
								{0, 					NULL,			NULL										}
							};

// file types
const char * FileTypes1[] = { 	"DDA0816 env saves",	"DDA*.dda",
								"DDA0816 root saves", 	"DDA*.root",
								"ROOT files",			"*.root",
								"All files",			"*",
                            	NULL,					NULL
							};
const char * FileTypes2[] = {	"Scan data", 			"*.scn",
								"GIF data", 			"*.gif",
								"All files",			"*",
                            	NULL,					NULL
							};

// color defs
static ULong_t cBlack;
static ULong_t cWhite;
static ULong_t cAwhite;
static ULong_t cBlue;
static ULong_t cGray;
static ULong_t cGreen;
static ULong_t cRed;
static ULong_t cYellow;

static const Char_t * sBlack = "black";
static const Char_t * sWhite = "white";
static const Char_t * sAwhite = "antiquewhite1";
static const Char_t * sBlue = "lightblue";
static const Char_t * sGray = "gray76";
static const Char_t * sGreen = "green";
static const Char_t * sRed = "red";
static const Char_t * sYellow = "yellow";

static TString offsetS0;

extern TMrbLogger * gMrbLog;		// access to MARaBOU's message logging

DDAControlPanel::DDAControlPanel(const TGWindow * Window, UInt_t Width, UInt_t Height)
      							: TGMainFrame(Window, Width, Height) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel
// Purpose:        Main frame
// Arguments:      TGWindow Window      -- connection to ROOT graphics
//                 UInt_t Width         -- window width in pixels
//                 UInt_t Height        -- window height in pixels
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t ddaChn;
	const Char_t * font;

//	Clear focus list
	fFocusList.Clear();

//	Initialize colors
	gClient->GetColorByName(sBlack, cBlack);
	gClient->GetColorByName(sWhite, cWhite);
	gClient->GetColorByName(sAwhite, cAwhite);
	gClient->GetColorByName(sBlue, cBlue);
	gClient->GetColorByName(sGray, cGray);
	gClient->GetColorByName(sGreen, cGreen);
	gClient->GetColorByName(sRed, cRed);
	gClient->GetColorByName(sYellow, cYellow);

//	Initialize graphic contexts
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	fFrameGC = new TGMrbLayout(font, sBlack, sGray);
	HEAP(fFrameGC);

	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGLayoutHints * entryLayout = new TGLayoutHints(kLHintsRight, 5, 1, 3, 1);
	HEAP(entryLayout);
	fWhiteEntryGC = new TGMrbLayout(font, sBlack, sWhite, entryLayout);
	HEAP(fWhiteEntryGC);

	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	fYellowEntryGC = new TGMrbLayout(font, sBlack, sYellow, entryLayout);
	HEAP(fYellowEntryGC);

	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 1);
	HEAP(labelLayout);
	fLabelGC = new TGMrbLayout(font, sBlack, sGray, labelLayout);
	HEAP(fLabelGC);

	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 1, 1);
	HEAP(buttonLayout);
	fGrayButtonGC = new TGMrbLayout(font, sBlack, sGray, buttonLayout);
	HEAP(fGrayButtonGC);
	
	font = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	fBlueButtonGC = new TGMrbLayout(font, sBlack, sBlue, buttonLayout);
	HEAP(fBlueButtonGC);
	
//	Initialize several lists
    for (ddaChn = 0; ddaChn <= 1; ddaChn++) {
        fLofScanProfiles[ddaChn].SetName("Scan Profiles");  // list of scan profiles
        fLofScanProfiles[ddaChn].AddNamedX(kSnkLofScanProfiles);
    }
	fLofSubdevs.SetName("DDA Subdevices"); 					// list of dac subdevs
	fLofSubdevs.AddNamedX(kSnkLofSubdevs);
	fLofPreScales.SetName("PreScaler Values");				// list of prescale values
	fLofPreScales.AddNamedX(kSnkLofPreScales);
	fLofScanModes.SetName("Scan Modes");					// list of scan modes
	fLofScanModes.AddNamedX(kSnkLofScanModes);
	fLofControlButtons.SetName("Control Buttons");			// list of control buttons
	fLofControlButtons.AddNamedX(kSnkLofControlButtons);
	fLofSoftScaleButtons.SetName("Soft Scaledown");			// soft scaledown buttons
	fLofSoftScaleButtons.AddNamedX(kSnkLofSoftScaleButtons);
	fLofFileButtons.SetName("File Buttons"); 				// list of file buttons
	fLofFileButtons.AddNamedX(kSnkLofFileButtons);

//	Create popup menus.

//	File menu
	fMenuFile = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuFile);
	fMenuFile->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

	fMenuFile->AddEntry("Open   Ctrl-o", kDDAFileOpen);
	fMenuFile->AddEntry("Save   Ctrl-s", kDDAFileSave);
	fMenuFile->AddEntry("Save&As", kDDAFileSaveAs);
	fMenuFile->AddEntry("Reset  Ctrl-r", kDDAFileReset);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Print  Ctrl-p", kDDAFilePrint);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Exit   Ctrl-q", kDDAFileExit);

//	Settings menu
	fMenuSettings = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuSettings);
	fMenuSettings->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

	fMenuSettings->AddEntry("Normal", kDDASettingsNormal);
	fMenuSettings->AddEntry("Verbose", kDDASettingsVerbose);
	fMenuSettings->AddEntry("Debug", kDDASettingsDebug);
	fMenuSettings->AddSeparator();
	fMenuSettings->AddEntry("Online", kDDASettingsOnline);
	fMenuSettings->AddEntry("Offline", kDDASettingsOffline);
	
	TString modeStr = gEnv->GetValue("DDAControl.ExecMode", "normal");
	TMrbLofNamedX lofExecModes;
	lofExecModes.AddNamedX(kSnkLofExecModes);
	lofExecModes.SetPatternMode();

	UInt_t eMode = lofExecModes.FindPattern(modeStr.Data());
	if (eMode & kSnkEMOffline) {
		fMenuSettings->RCheckEntry(kDDASettingsOffline, kDDASettingsOnline, kDDASettingsOffline);
		curDDA->SetOffline();
		gMrbLog->Out() << "Setting OFFLINE mode" << endl;
		gMrbLog->Flush("DDAControl", "", setred);
	} else {
		fMenuSettings->RCheckEntry(kDDASettingsOnline, kDDASettingsOnline, kDDASettingsOffline);
		curDDA->SetOnline();
	}

	if (eMode & kSnkEMDebug) {
		fMenuSettings->RCheckEntry(kDDASettingsDebug, kDDASettingsNormal, kDDASettingsDebug);
		curDDA->SetDebug();
		gMrbLog->Out() << "Setting DEBUG mode" << endl;
		gMrbLog->Flush("DDAControl", "", setred);
	} else if (eMode & kSnkEMVerbose) {
		fMenuSettings->RCheckEntry(kDDASettingsVerbose, kDDASettingsNormal, kDDASettingsDebug);
		curDDA->SetVerbose();
		gMrbLog->Out() << "Setting VERBOSE mode" << endl;
		gMrbLog->Flush("DDAControl", "", setred);
	} else {
		fMenuSettings->RCheckEntry(kDDASettingsNormal, kDDASettingsNormal, kDDASettingsDebug);
	}

//	Server menu
	fMenuServer = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuServer);
	fMenuServer->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

	fMenuServer->AddEntry("mamba (DDAExec)", kDDAServerOldDDA);
	fMenuServer->AddEntry("mamba2 (DDAExecME)", kDDAServerNewDDA);
	
	fServerName = gEnv->GetValue("DDAControl.Server", "localhost");
	fServerPort = gEnv->GetValue("DDAControl.Port", kSnkDefaultPort);
	fServerProg = gEnv->GetValue("DDAControl.ServerProg", "DDAExec");

	TMrbLofNamedX lofServers;
	lofServers.AddNamedX(kSnkLofServers);
	lofServers.SetPatternMode();

	UInt_t sMode = lofServers.FindPattern(fServerName.Data());
	if (sMode & kSnkSrvOldDDA) {
		fMenuServer->RCheckEntry(kDDAServerOldDDA, kDDAServerNewDDA, kDDAServerOldDDA);
		fServerName = "mamba";
		fServerProg = "DDAExec";
	} else {
		fMenuServer->RCheckEntry(kDDAServerNewDDA, kDDAServerNewDDA, kDDAServerOldDDA);
		fServerName = "mamba2";
		fServerProg = "DDAExecME";
	}

//	Help menu
	fMenuHelp = new TGPopupMenu(fClient->GetRoot());
	HEAP(fMenuHelp);
	fMenuHelp->Connect("Activated(Int_t)", this->ClassName(), this, "Activate(Int_t)");

	fMenuHelp->AddEntry("Contents", kDDAHelpContents);
	fMenuHelp->AddSeparator();
	fMenuHelp->AddEntry("About DDAControl", kDDAHelpAbout);

//	Main menu bar
	fMenuBar = new TGMenuBar(this, kAutoWidth, kAutoHeight, kHorizontalFrame | kSunkenFrame);
	HEAP(fMenuBar);

	TGLayoutHints * menuBarFileLayout = new TGLayoutHints(kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarFileLayout);
	fMenuBar->AddPopup("File", fMenuFile, menuBarFileLayout);

	TGLayoutHints * menuBarSettingsLayout = new TGLayoutHints(kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarSettingsLayout);
	fMenuBar->AddPopup("Settings", fMenuSettings, menuBarSettingsLayout);

	TGLayoutHints * menuBarServerLayout = new TGLayoutHints(kLHintsLeft, 2, 2, 2, 3);
	HEAP(menuBarServerLayout);
	fMenuBar->AddPopup("Server", fMenuServer, menuBarServerLayout);

	TGLayoutHints * menuBarHelpLayout = new TGLayoutHints(kLHintsRight, 2, 2, 2, 3);
	HEAP(menuBarHelpLayout);
	fMenuBar->AddPopup("Help", fMenuHelp, menuBarHelpLayout);

	TGLayoutHints * menuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 3);
	HEAP(menuBarLayout);
	AddFrame(fMenuBar, menuBarLayout);

	fMenuBar->ChangeBackground(cBlue);

//	Frame to show icons & control buttons
	TGLayoutHints * controlLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
	HEAP(controlLayout);
	fCFrame = new TGHorizontalFrame(this, kAutoWidth, kAutoHeight);
	HEAP(fCFrame);

//	Icon bar
	TGLayoutHints * pictLayout = new TGLayoutHints(0, 5, 5, 5, 5);
	HEAP(pictLayout);
	fFButtons = new TGMrbPictureButtonGroup(fCFrame, "Action", &fLofFileButtons, -1, 1,
												fFrameGC, fGrayButtonGC,
												kHorizontalFrame);
	HEAP(fFButtons);
	((TGMrbButtonFrame *) fFButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ButtonPressed(Int_t, Int_t)");

	fCFrame->AddFrame(fFButtons, pictLayout);

	TGLayoutHints * cbuttonLayout = new TGLayoutHints(kLHintsExpandX, 1, 1, 5, 5);
	HEAP(cbuttonLayout);
	fFrameGC->SetLH(cbuttonLayout);
	fCbuttons = new TGMrbTextButtonGroup(fCFrame, "Scan Control", &fLofControlButtons, -1, 1,
												fFrameGC, fBlueButtonGC,
												kHorizontalFrame);
	HEAP(fCbuttons);
	((TGMrbButtonFrame *) fCbuttons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "ButtonPressed(Int_t, Int_t)");
	
	fCFrame->AddFrame(fCbuttons, fFrameGC->LH());

	TGLayoutHints * spLayout = new TGLayoutHints(kLHintsTop, 5, 5, 5, 5);
	HEAP(spLayout);
	fFrameGC->SetLH(spLayout);
	fSFrame = new TGGroupFrame(fCFrame, "Scan Period / Cycles (0 = infin)", kHorizontalFrame);
	HEAP(fSFrame);
	fFrameGC->SetLH(spLayout);
	fScanPeriod = new TGMrbLabelEntry(fSFrame, "scan",
												10, kDDAEntryScanPeriod,
												kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
												fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fScanPeriod);
	fSFrame->AddFrame(fScanPeriod, fFrameGC->LH());
	fScanPeriod->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fScanPeriod->GetEntry()->SetText("0.");
	fScanPeriod->SetRange(0., 1000000.);
	fScanPeriod->SetIncrement(1.);
	fScanPeriod->AddToFocusList(&fFocusList);

	fCycles = new TGMrbLabelEntry(fSFrame, "cyc",
												10, kDDAEntryCycles,
												kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
												fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fCycles);
	fSFrame->AddFrame(fCycles, fFrameGC->LH());
	fCycles->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fCycles->GetEntry()->SetText("0");
	fCycles->SetRange(0, 1000000);
	fCycles->SetIncrement(1);
	fCycles->AddToFocusList(&fFocusList);

	fCFrame->AddFrame(fSFrame, fFrameGC->LH());

	TGLayoutHints * dfLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
	HEAP(dfLayout);
	fFrameGC->SetLH(dfLayout);
	fFFrame = new TGGroupFrame(fCFrame, "Scan Data File", kHorizontalFrame);
	HEAP(fFFrame);
	fFrameGC->SetLH(dfLayout);
	fDataFileInfo.fFileTypes = (const Char_t **) FileTypes2;
	fDataFile = new TGMrbFileEntry(fFFrame, NULL,	200, kDDAEntryDataFile,
															kDFEntryWidth,
															kDFEntryHeight,
															2*kDFEntryWidth,
															&fDataFileInfo, kFDOpen,
															fFrameGC, fLabelGC, fWhiteEntryGC);
	HEAP(fDataFile);
	fFFrame->AddFrame(fDataFile, fFrameGC->LH());

	fCFrame->AddFrame(fFFrame, fFrameGC->LH());

	this->AddFrame(fCFrame, controlLayout);
	
//	Horizontal group frame to choose module number
	TGLayoutHints * modulesLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
	HEAP(modulesLayout);
	fFrameGC->SetLH(modulesLayout);
	buttonLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX);
	HEAP(buttonLayout);
	fGrayButtonGC->SetLH(buttonLayout);
	fSubdevs = new TGMrbRadioButtonGroup(this, "Subdevice", &fLofSubdevs, -1, 1,
												fFrameGC, fGrayButtonGC, kHorizontalFrame);
	HEAP(fSubdevs);
	this->AddFrame(fSubdevs, fFrameGC->LH());

//	Horizontal frame to display scan mode
	TGLayoutHints * scanmodeLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
	HEAP(scanmodeLayout);
	fFrameGC->SetLH(scanmodeLayout);
	fLabelGC->SetLH(labelLayout);
	fScanModes = new TGMrbRadioButtonGroup(this, "Scan Mode", &fLofScanModes, -1, 1,
												fFrameGC, fLabelGC,
												kHorizontalFrame);
	HEAP(fScanModes);
	this->AddFrame(fScanModes, fFrameGC->LH());
	((TGMrbButtonFrame *) fScanModes)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SetScanMode(Int_t, Int_t)");

//	Horizontal frame to hold calibration data
	TGLayoutHints * calLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
	HEAP(calLayout);
	fFrameGC->SetLH(calLayout);
	fCalFrame = new TGGroupFrame(this, "Calibration", kVerticalFrame);
	HEAP(fCalFrame);
	this->AddFrame(fCalFrame, fFrameGC->LH());

	TGLayoutHints * rframeLayout = new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1);
	HEAP(rframeLayout);
	fCRow1Frame = new TGHorizontalFrame(fCalFrame, kAutoWidth, kHFrameHeight);
	HEAP(fCRow1Frame);
	fCalFrame->AddFrame(fCRow1Frame, rframeLayout);
	fCRow2Frame = new TGHorizontalFrame(fCalFrame, kAutoWidth, kHFrameHeight);
	HEAP(fCRow2Frame);
	fCalFrame->AddFrame(fCRow2Frame, rframeLayout);

	fChargeState = new TGMrbLabelEntry(fCRow1Frame,	"Charge state",
													10, kDDAEntryChargeState,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fChargeState);
	fCRow1Frame->AddFrame(fChargeState, fFrameGC->LH());
	fChargeState->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fChargeState->GetEntry()->SetText(Form("%d", curDDA->GetChargeState()));
	fChargeState->SetRange(0, 300);
	fChargeState->SetIncrement(1);
	fChargeState->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");
	fChargeState->AddToFocusList(&fFocusList);

	fCalib[0] = new TGMrbLabelEntry(fCRow1Frame,	"Calib X",
													10, kDDAEntryCalibX,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fCalib[0]);
	fCRow1Frame->AddFrame(fCalib[0], fFrameGC->LH());
	fCalib[0]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fCalib[0]->GetEntry()->SetText(Form("%g", curDDA->GetCalibration(0)));
	fCalib[0]->SetRange(0., 1000.);
	fCalib[0]->SetIncrement(.1);
	fCalib[0]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");
	fCalib[0]->AddToFocusList(&fFocusList);

	fVoltage[0] = new TGMrbLabelEntry(fCRow1Frame,	"Voltage X [kV]",
													10, kDDAEntryVoltageX,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fVoltage[0]);
	fCRow1Frame->AddFrame(fVoltage[0], fFrameGC->LH());
	fVoltage[0]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fVoltage[0]->GetEntry()->SetText(Form("%g", curDDA->GetVoltage(0)));
	fVoltage[0]->SetRange(0., 100.);
	fVoltage[0]->SetIncrement(.5);
	fVoltage[0]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");
	fVoltage[0]->AddToFocusList(&fFocusList);

	fStepWidth[0] = new TGMrbLabelEntry(fCRow1Frame,	"Step X [um]",
													10, kDDAEntryStepWidthX,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC);
	HEAP(fStepWidth[0]);
	fCRow1Frame->AddFrame(fStepWidth[0], fFrameGC->LH());
	fStepWidth[0]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	this->SetEntrySlave(fStepWidth[0], Form("%g", curDDA->GetStepWidth(0)));

	fRange[0] = new TGMrbLabelEntry(fCRow1Frame,	"Range X [um] +/-",
													10, kDDAEntryRangeX,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC);
	HEAP(fRange[0]);
	fCRow1Frame->AddFrame(fRange[0], fFrameGC->LH());
	fRange[0]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	this->SetEntrySlave(fRange[0], Form("%g", curDDA->GetRange(0)));

	fEnergy = new TGMrbLabelEntry(fCRow2Frame,	"Energy [MeV]",
												10, kDDAEntryEnergy,
												kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
												fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fEnergy);
	fCRow2Frame->AddFrame(fEnergy, fFrameGC->LH());

	fEnergy->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fEnergy->GetEntry()->SetText(Form("%g", curDDA->GetEnergy()));
	fEnergy->SetRange(0., 300.);
	fEnergy->SetIncrement(.5);
	fEnergy->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");
	fEnergy->AddToFocusList(&fFocusList);

	fCalib[1] = new TGMrbLabelEntry(fCRow2Frame,	"Calib Y",
													10, kDDAEntryCalibY,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fCalib[1]);
	fCRow2Frame->AddFrame(fCalib[1], fFrameGC->LH());
	fCalib[1]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fCalib[1]->GetEntry()->SetText(Form("%g", curDDA->GetCalibration(1)));
	fCalib[1]->SetRange(0., 1000.);
	fCalib[1]->SetIncrement(.1);
	fCalib[1]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");
	fCalib[1]->AddToFocusList(&fFocusList);

	fVoltage[1] = new TGMrbLabelEntry(fCRow2Frame,	"Voltage Y [kV]",
													10, kDDAEntryVoltageY,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fVoltage[1]);
	fCRow2Frame->AddFrame(fVoltage[1], fFrameGC->LH());
	fVoltage[1]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fVoltage[1]->GetEntry()->SetText(Form("%g", curDDA->GetVoltage(1)));
	fVoltage[1]->SetRange(0., 100.);
	fVoltage[1]->SetIncrement(.5);
	fVoltage[1]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");
	fVoltage[1]->AddToFocusList(&fFocusList);

	fStepWidth[1] = new TGMrbLabelEntry(fCRow2Frame,	"Step Y [um]",
													10, kDDAEntryStepWidthX,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC);
	HEAP(fStepWidth[1]);
	fCRow2Frame->AddFrame(fStepWidth[1], fFrameGC->LH());
	fStepWidth[1]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	this->SetEntrySlave(fStepWidth[1], Form("%g", curDDA->GetStepWidth(1)));

	fRange[1] = new TGMrbLabelEntry(fCRow2Frame,	"Range Y [um] +/-",
													10, kDDAEntryRangeX,
													kSPEntryWidth, kSPEntryHeight, kSPEntryWidth,
													fFrameGC, fLabelGC, fWhiteEntryGC);
	HEAP(fRange[1]);
	fCRow2Frame->AddFrame(fRange[1], fFrameGC->LH());
	fRange[1]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	this->SetEntrySlave(fRange[1], Form("%g", curDDA->GetRange(1)));

//	Horizontal frame to place X and Y panels
	TGLayoutHints * hframeLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
	HEAP(hframeLayout);
	fHFrame = new TGVerticalFrame(this, kAutoWidth, kHFrameHeight);
	HEAP(fHFrame);
	this->AddFrame(fHFrame, hframeLayout);

//	2 identical panels to show X und Y axes, resp.
	TString ddaLabel = "X Channel";
	for (ddaChn = 0; ddaChn <= 1; ddaChn++) {

//		panel to place entries, radios etc.
		fXYFrame[ddaChn] = new TGGroupFrame(fHFrame, ddaLabel.Data(), kHorizontalFrame);
		HEAP(fXYFrame[ddaChn]);
		TGLayoutHints * xyframeLayout = new TGLayoutHints(kLHintsLeft| kLHintsExpandX, 5, 5, 5, 5);
		HEAP(xyframeLayout);
		fHFrame->AddFrame(fXYFrame[ddaChn], xyframeLayout);

//		radiobuttons for scan profiles
		TGLayoutHints * scanprofilesLayout = new TGLayoutHints(kLHintsLeft, 5, 5, 5, 5);
		HEAP(scanprofilesLayout);
		fFrameGC->SetLH(scanprofilesLayout);
		fScanProfiles[ddaChn] = new TGMrbRadioButtonGroup(fXYFrame[ddaChn],
												"Scan Profile", &fLofScanProfiles[ddaChn], -1, 1,
												fFrameGC, fGrayButtonGC, kVerticalFrame);
		HEAP(fScanProfiles[ddaChn]);
		fXYFrame[ddaChn]->AddFrame(fScanProfiles[ddaChn], fFrameGC->LH());

//		distance in units of um
		fDistFrame[ddaChn] = new TGGroupFrame(fXYFrame[ddaChn], "Distance [um]");
		HEAP(fDistFrame[ddaChn]);
		fXYFrame[ddaChn]->AddFrame(fDistFrame[ddaChn], fFrameGC->LH());

//		amplitude & offset in steps
		fStepFrame[ddaChn] = new TGGroupFrame(fXYFrame[ddaChn], "Steps");
		HEAP(fStepFrame[ddaChn]);
		fXYFrame[ddaChn]->AddFrame(fStepFrame[ddaChn], fFrameGC->LH());

//		stop position
		fStopPosFrame[ddaChn] = new TGGroupFrame(fXYFrame[ddaChn], "Stop at");
		HEAP(fStopPosFrame[ddaChn]);
		fXYFrame[ddaChn]->AddFrame(fStopPosFrame[ddaChn], fFrameGC->LH());

//		labelled entry: offset
		TGLayoutHints * offsetLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 3, 1);
		HEAP(offsetLayout);
		fFrameGC->SetLH(offsetLayout);

		fOffsetZeroButton[ddaChn] = new TMrbNamedX(kDDAOffsetZero0 + ddaChn, "0", "PerformAction(Int_t)");
		fOffsetZeroButton[ddaChn]->AssignObject(this);
		fOffsetD[ddaChn] = new TGMrbLabelEntry(fDistFrame[ddaChn], "Offset",
												10, kDDAEntryOffset0 + ddaChn,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC,
												kFALSE, fOffsetZeroButton[ddaChn], fGrayButtonGC);
		HEAP(fOffsetD[ddaChn]);
		fDistFrame[ddaChn]->AddFrame(fOffsetD[ddaChn], fFrameGC->LH());

		fOffsetD[ddaChn]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
		fOffsetD[ddaChn]->AddToFocusList(&fFocusList);
		fOffsetD[ddaChn]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fOffsetS[ddaChn] = new TGMrbLabelEntry(fStepFrame[ddaChn], "Offset",
												10, -1,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC);
		HEAP(fOffsetS[ddaChn]);
		fStepFrame[ddaChn]->AddFrame(fOffsetS[ddaChn], fFrameGC->LH());

//		labelled entry: range
		fAmplitudeD[ddaChn] = new TGMrbLabelEntry(fDistFrame[ddaChn], "Amplitude +/-",
												10, kDDAEntryAmplitude0 + ddaChn,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
		HEAP(fAmplitudeD[ddaChn]);
		fDistFrame[ddaChn]->AddFrame(fAmplitudeD[ddaChn], fFrameGC->LH());

		fAmplitudeD[ddaChn]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
		fAmplitudeD[ddaChn]->AddToFocusList(&fFocusList);
		fAmplitudeD[ddaChn]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fAmplitudeS[ddaChn] = new TGMrbLabelEntry(fStepFrame[ddaChn], "Amplitude +/-",
												10, -1,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC);
		HEAP(fAmplitudeS[ddaChn]);
		fStepFrame[ddaChn]->AddFrame(fAmplitudeS[ddaChn], fFrameGC->LH());

// place an unvisible label to pad group frame vertically
		TGLayoutHints * dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 12, 0);
		HEAP(dmyLayout);
		TGLabel * dmyLabel = new TGLabel(fStepFrame[ddaChn], "", fFrameGC->GC(), fFrameGC->Font(), kChildFrame, fFrameGC->BG());
		HEAP(dmyLabel);
		fStepFrame[ddaChn]->AddFrame(dmyLabel, dmyLayout);

//		labelled entry: array size
		fNofPixels[ddaChn] = new TGMrbLabelEntry(fDistFrame[ddaChn], "# of pixels",
												10, kDDAEntrySize0 + ddaChn,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
		HEAP(fNofPixels[ddaChn]);
		fDistFrame[ddaChn]->AddFrame(fNofPixels[ddaChn], fFrameGC->LH());
		fNofPixels[ddaChn]->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
		fNofPixels[ddaChn]->AddToFocusList(&fFocusList);
		fNofPixels[ddaChn]->SetRange(0, 1000);
		fNofPixels[ddaChn]->SetIncrement(10);
		fNofPixels[ddaChn]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

//		labelled entry: scan limits
		fYellowEntryGC->SetLH(offsetLayout);
		fScanLimitsD[ddaChn] = new TGMrbLabelEntry(fDistFrame[ddaChn], NULL,
												10, -1,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC, fYellowEntryGC);
		HEAP(fScanLimitsD[ddaChn]);
		fDistFrame[ddaChn]->AddFrame(fScanLimitsD[ddaChn], fFrameGC->LH());
		fScanLimitsD[ddaChn]->AddToFocusList(&fFocusList);
		this->SetEntryDisabled(fScanLimitsD[ddaChn]);

		fScanLimitsS[ddaChn] = new TGMrbLabelEntry(fStepFrame[ddaChn], NULL,
												10, -1,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC, fYellowEntryGC);
		HEAP(fScanLimitsS[ddaChn]);
		fStepFrame[ddaChn]->AddFrame(fScanLimitsS[ddaChn], fFrameGC->LH());

//		labelled entry: stop position
		fFrameGC->SetLH(offsetLayout);

		fStopZeroButton[ddaChn] = new TMrbNamedX(kDDAStopZero0 + ddaChn, "0", "PerformAction(Int_t)");
		fStopZeroButton[ddaChn]->AssignObject(this);
		fStopPosD[ddaChn] = new TGMrbLabelEntry(fStopPosFrame[ddaChn], "Distance [um]",
												10, kDDAOffsetZero0 + ddaChn,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC,
												kFALSE, fStopZeroButton[ddaChn], fGrayButtonGC);
		fStopPosD[ddaChn]->SetRange(-1000., 1000.);
		fStopPosD[ddaChn]->SetIncrement(1.);
		HEAP(fStopPosD[ddaChn]);
		fStopPosFrame[ddaChn]->AddFrame(fStopPosD[ddaChn], fFrameGC->LH());

		fStopPosD[ddaChn]->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
		fStopPosD[ddaChn]->AddToFocusList(&fFocusList);
		fStopPosD[ddaChn]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

		fStopPosS[ddaChn] = new TGMrbLabelEntry(fStopPosFrame[ddaChn], "--> Steps",
												10, -1,
												kXYEntryWidth, kXYEntryHeight, kXYEntryWidth,
												fFrameGC, fLabelGC);
		HEAP(fStopPosS[ddaChn]);
		fStopPosFrame[ddaChn]->AddFrame(fStopPosS[ddaChn], fFrameGC->LH());


		ddaLabel = "Y Channel";

	}

//	Vertical frame to display frequencies and scaledowns
	fFQFrame = new TGGroupFrame(this, "Pixel Frequency & ScaleDown");
	HEAP(fFQFrame);
	TGLayoutHints * fqframeLayout = new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5);
	HEAP(fqframeLayout);
	this->AddFrame(fFQFrame, fqframeLayout);
	TGLayoutHints * prescaleLayout = new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1);
	HEAP(prescaleLayout);
	fFrameGC->SetLH(prescaleLayout);
	fPreScales = new TGMrbRadioButtonList(fFQFrame, NULL, &fLofPreScales, -1, 1,
												kAutoWidth, kAutoHeight,
												fFrameGC, fLabelGC, fGrayButtonGC,
												kHorizontalFrame);
	HEAP(fPreScales);
	fFQFrame->AddFrame(fPreScales, fFrameGC->LH());
//	Horizontal sub-frame to store scaledowns
	fSDFrame = new TGHorizontalFrame(fFQFrame, kAutoWidth, kAutoHeight);
	HEAP(fSDFrame);
	TGLayoutHints * sdframeLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	HEAP(sdframeLayout);
	fFQFrame->AddFrame(fSDFrame, sdframeLayout);
//	labelled entry: pacer clock
	TGLayoutHints * pacerLayout = new TGLayoutHints(kLHintsLeft, 10, 1, 3, 1);
	HEAP(pacerLayout);
	fFrameGC->SetLH(pacerLayout);
	fPacerClock = new TGMrbLabelEntry(fSDFrame, "Frequ Divider",
												5, kDDAEntryPacer,
												kSDEntryWidth, kSDEntryHeight, kSDEntryWidth,
												fFrameGC, fLabelGC, fWhiteEntryGC, fGrayButtonGC);
	HEAP(fPacerClock);
	fSDFrame->AddFrame(fPacerClock, fFrameGC->LH());
	fPacerClock->GetEntry()->SetText("1");
	fPacerClock->SetRange(1, 256);
	fPacerClock->SetIncrement(1);
	fPacerClock->AddToFocusList(&fFocusList);
//	radiobuttons: soft scaledown
	TGLayoutHints * softScaleLayout = new TGLayoutHints(kLHintsExpandX, 10, 1, 3, 1);
	HEAP(softScaleLayout);
	TGLayoutHints * sclabelLayout = new TGLayoutHints(kLHintsLeft, 10, 1, 3, 1);
	HEAP(sclabelLayout);
	fFrameGC->SetLH(softScaleLayout);
	fLabelGC->SetLH(sclabelLayout);
	fSoftScale = new TGMrbRadioButtonList(fSDFrame, "Fast Channel", &fLofSoftScaleButtons, -1, 1,
												kSDButtonWidth, kSDEntryHeight,
												fFrameGC, fLabelGC, fGrayButtonGC,
												kHorizontalFrame);
	HEAP(fSoftScale);
	fSDFrame->AddFrame(fSoftScale, softScaleLayout);
//	labelled entry: soft scale value
	TGLayoutHints * scaleValueLayout = new TGLayoutHints(kLHintsLeft, 10, 1, 3, 1);
	HEAP(scaleValueLayout);
	fFrameGC->SetLH(scaleValueLayout);
	fScaleValue = new TGMrbLabelEntry(fSDFrame, "Soft Scale",
												10, -1,
												kSDEntryWidth, kSDEntryHeight, kSDEntryWidth,
												fFrameGC, fLabelGC, fYellowEntryGC);
	HEAP(fScaleValue);
	fSDFrame->AddFrame(fScaleValue, fFrameGC->LH());
	fScaleValue->SetRange(0, 1000);
	fScaleValue->SetIncrement(1);
	this->SetEntryDisabled(fScaleValue, "0");
	fScaleValue->AddToFocusList(&fFocusList);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-c", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.BindKey("Ctrl-q", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.BindKey("Ctrl-o", TGMrbLofKeyBindings::kGMrbKeyActionOpen);
	fKeyBindings.BindKey("Ctrl-s", TGMrbLofKeyBindings::kGMrbKeyActionSave);
	fKeyBindings.BindKey("Ctrl-p", TGMrbLofKeyBindings::kGMrbKeyActionPrint);
	fKeyBindings.BindKey("Ctrl-r", (TGMrbLofKeyBindings::EGMrbKeyAction) kDDAKeyActionReset);
	fKeyBindings.Connect("KeyPressed(Int_t, Int_t)", this->ClassName(), this, "KeyPressed(Int_t, Int_t)");
	
//	Reset panel
	this->Reset();

	SetWindowName("DDAControl: DDA0816 Scan Control");

	MapSubwindows();

//	we need to use GetDefault...() to initialize the layout algorithm...
	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
}

Bool_t DDAControlPanel::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::Reset
// Purpose:        Reset control panel
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Resets any values in the control panel.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t ddaChn;
	TMrbNamedX *nx;
	
	TString subDevice = gEnv->GetValue("DDAControl.Subdevice", "B");
	if ((nx = fLofSubdevs.FindByName(subDevice.Data(), TMrbLofNamedX::kFindUnique)) == NULL) {
		TString msg = Form("Illegal subdevice - %s, defaulting to \"B\"", subDevice.Data());
		gMrbLog->Err() << msg << endl;
		gMrbLog->Flush(this->ClassName() , "Reset");
		fSubdevs->SetState(kDDASubdevB, kButtonDown);
		new TGMsgBox(fClient->GetRoot(), this, "DDAControl: Error", msg, kMBIconStop);
	} else {		
		fSubdevs->SetState(nx->GetIndex(), kButtonDown);
	}
	curDDA->SetSubdevice(fSubdevs->GetActive() - kDDASubdevA);
		
	offsetS0 = gEnv->GetValue("DDAControl.Offset0", kSnkOffset0);
		
//	set GUI values to "undef" or "0"
	for (ddaChn = 0; ddaChn <= 1; ddaChn++) {
		fScanProfiles[ddaChn]->SetState(kSnkSPUndef, kButtonDown);
		fNofPixels[ddaChn]->GetEntry()->SetText("0");
	}
	fPreScales->SetState(kSnkPSUndef, kButtonDown);
	fPacerClock->GetEntry()->SetText("1");
	fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
	this->SetEntryDisabled(fScaleValue, "0");
	fScanModes->SetState(kSnkSMUndef, kButtonDown);

	this->UpdateCalibration(curDDA);
	this->InitializeValues(curDDA, kSnkSMUndef);
	this->UpdateScanLimits(curDDA);
	
	return(kTRUE);
}

Bool_t DDAControlPanel::InitializeValues(TSnkDDA0816 * DDA, ESnkScanMode Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::InitializeValues
// Purpose:        Initialize GUI settings according to scan mode
// Arguments:      TSnkDDA0816 * DDA    -- DDA object
//                 ESnkScanMode Mode    -- scan mode
// Results:        
// Exceptions:     
// Description:    Initializes GUI settings.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString value;

	DDA->SetScanMode(Mode);

	if (Mode == kSnkSMUndef) {
		this->SetEntryDisabled(fStopPosS[0], offsetS0.Data());
		this->SetEntryDisabled(fStopPosD[0], "0.");
		this->SetEntryDisabled(fStopPosS[1], offsetS0.Data());
		this->SetEntryDisabled(fStopPosD[1], "0.");
	} else {
		this->SetEntrySlave(fStopPosS[0], offsetS0.Data());
		this->SetEntryEnabled(fStopPosD[0], "0.");
		this->SetEntrySlave(fStopPosS[1], offsetS0.Data());
		this->SetEntryEnabled(fStopPosD[1], "0.");
	}

	switch (Mode) {
		case kSnkSMUndef:
			for (Int_t i = 0; i <= 1; i++) {
				fScanProfiles[i]->SetState(kSnkSPUndef, kButtonDown);
				fScanProfiles[i]->SetState(~kSnkSPUndef, kButtonDisabled);
				this->SetEntryDisabled(fOffsetS[i], offsetS0.Data());
				this->SetEntryDisabled(fOffsetD[i], "0.");
				this->SetEntryDisabled(fAmplitudeS[i], "0");
				this->SetEntryDisabled(fAmplitudeD[i], "0.");
				this->SetEntryDisabled(fNofPixels[i], "0");
			}
			fPreScales->SetState(kSnkPSUndef, kButtonDown);
			fPreScales->SetState(~kSnkPSUndef, kButtonDisabled);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryDisabled(fPacerClock, "1");
			this->SetFileDisabled(fDataFile);
			break;

		case kSnkSMSymm:
			fScanProfiles[0]->SetState(kSnkAllBits, kButtonEngaged);
			fScanProfiles[0]->SetState(kSnkSPUndef, kButtonDown);
			this->SetEntryEnabled(fOffsetD[0]);
			this->SetEntrySlave(fOffsetS[0]);
			this->SetEntryEnabled(fAmplitudeD[0]);
			this->SetEntrySlave(fAmplitudeS[0]);
			this->SetEntryEnabled(fNofPixels[0]);
			fScanProfiles[1]->SetState(kSnkSPUndef, kButtonDown);
			fScanProfiles[1]->SetState(~kSnkSPUndef, kButtonDisabled);
			this->SetEntryEnabled(fOffsetD[1]);
			this->SetEntrySlave(fOffsetS[1]);
			value = fAmplitudeD[0]->GetEntry()->GetText();
			this->SetEntrySlave(fAmplitudeD[1], value.Data());
			value = Form("%d", DDA->Dist2Ampl(1, value.Atof()));
			this->SetEntrySlave(fAmplitudeS[1], value.Data());
			this->SetEntrySlave(fNofPixels[1], fNofPixels[0]->GetEntry()->GetText());
			fPreScales->SetState(kSnkAllBits, kButtonEngaged);
			fPreScales->SetState(kSnkPSUndef, kButtonDown);
			fSoftScale->SetState(kSnkAllBits, kButtonEngaged);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			this->SetEntryEnabled(fPacerClock, "1");
			this->SetFileDisabled(fDataFile);
			break;
			
		case kSnkSMAsymm:
			for (Int_t i = 0; i <= 1; i++) {
				fScanProfiles[i]->SetState(kSnkAllBits, kButtonEngaged);
				fScanProfiles[i]->SetState(kSnkSPUndef, kButtonDown);
				this->SetEntryEnabled(fOffsetD[i]);
				this->SetEntrySlave(fOffsetS[i]);
				this->SetEntryEnabled(fAmplitudeD[i]);
				this->SetEntrySlave(fAmplitudeS[i]);
				this->SetEntryEnabled(fNofPixels[i]);
			}
			fPreScales->SetState(kSnkAllBits, kButtonEngaged);
			fPreScales->SetState(kSnkPSUndef, kButtonDown);
			fSoftScale->SetState(kSnkAllBits, kButtonEngaged);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			this->SetEntryEnabled(fPacerClock, "1");
			this->SetFileDisabled(fDataFile);
			break;

		case kSnkSMLShape:
			for (Int_t i = 0; i <= 1; i++) {
				fScanProfiles[i]->SetState(kSnkAllBits, kButtonEngaged);
				fScanProfiles[i]->SetState(kSnkSPTriangle, kButtonDown);
				fScanProfiles[i]->SetState(~kSnkSPTriangle, kButtonDisabled);
				this->SetEntryEnabled(fOffsetD[i]);
				this->SetEntrySlave(fOffsetS[i]);
				this->SetEntryEnabled(fAmplitudeD[i]);
				this->SetEntrySlave(fAmplitudeS[i]);
				this->SetEntryEnabled(fNofPixels[i]);
			}
			fPreScales->SetState(kSnkAllBits, kButtonEngaged);
			fPreScales->SetState(kSnkPSUndef, kButtonDown);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock, "1");
			this->SetFileDisabled(fDataFile);
			break;

		case kSnkSMXOnly:
			fScanProfiles[0]->SetState(kSnkAllBits, kButtonEngaged);
			fScanProfiles[0]->SetState(kSnkSPUndef, kButtonDown);
			this->SetEntryEnabled(fOffsetD[0]);
			this->SetEntrySlave(fOffsetS[0]);
			this->SetEntryEnabled(fAmplitudeD[0]);
			this->SetEntrySlave(fAmplitudeS[0]);
			this->SetEntryEnabled(fNofPixels[0]);
			fScanProfiles[1]->SetState(kSnkSPConstant, kButtonDown);
			fScanProfiles[1]->SetState(~kSnkSPConstant, kButtonDisabled);
			this->SetEntryEnabled(fOffsetD[1]);
			this->SetEntrySlave(fOffsetS[1]);
			this->SetEntryDisabled(fAmplitudeD[1], "0.");
			this->SetEntryDisabled(fAmplitudeS[1], "0");
			this->SetEntryDisabled(fNofPixels[1], "1");
			fPreScales->SetState(kSnkAllBits, kButtonEngaged);
			fPreScales->SetState(kSnkPSUndef, kButtonDown);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock, "1");
			this->SetFileDisabled(fDataFile);
			break;
			
		case kSnkSMYOnly:
			fScanProfiles[0]->SetState(kSnkAllBits, kButtonEngaged);
			fScanProfiles[0]->SetState(kSnkSPConstant, kButtonDown);
			fScanProfiles[0]->SetState(~kSnkSPConstant, kButtonDisabled);
			this->SetEntryEnabled(fOffsetD[0]);
			this->SetEntrySlave(fOffsetS[0]);
			this->SetEntryDisabled(fAmplitudeD[0], "0.");
			this->SetEntryDisabled(fAmplitudeS[0], "0");
			this->SetEntryDisabled(fNofPixels[0], "1");
			fScanProfiles[1]->SetState(kSnkAllBits, kButtonEngaged);
			fScanProfiles[1]->SetState(kSnkSPUndef, kButtonDown);
			this->SetEntryEnabled(fOffsetD[1]);
			this->SetEntrySlave(fOffsetS[1]);
			this->SetEntryEnabled(fAmplitudeD[1]);
			this->SetEntrySlave(fAmplitudeS[1]);
			this->SetEntryEnabled(fNofPixels[1]);
			fPreScales->SetState(kSnkAllBits, kButtonEngaged);
			fPreScales->SetState(kSnkPSUndef, kButtonDown);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock, "1");
			this->SetFileDisabled(fDataFile);
			break;
			
		case kSnkSMXYConst:
			for (Int_t i = 0; i <= 1; i++) {
				fScanProfiles[i]->SetState(kSnkAllBits, kButtonEngaged);
				fScanProfiles[i]->SetState(kSnkSPConstant, kButtonDown);
				fScanProfiles[i]->SetState(~kSnkSPConstant, kButtonDisabled);
				this->SetEntryEnabled(fOffsetD[i]);
				this->SetEntrySlave(fOffsetS[i]);
				this->SetEntryDisabled(fAmplitudeD[i], "0.");
				this->SetEntryDisabled(fAmplitudeS[i], "0");
				this->SetEntryDisabled(fNofPixels[i], "1");
			}
			fPreScales->SetState(kSnkAllBits, kButtonEngaged);
			fPreScales->SetState(kSnkPS1, kButtonDown);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock, "1");
			this->SetFileDisabled(fDataFile);
			break;
			
		case kSnkSMFromFile:
			for (Int_t i = 0; i <= 1; i++) {
				fScanProfiles[i]->SetState(kSnkAllBits, kButtonEngaged);
				fScanProfiles[i]->SetState(kSnkSPUndef, kButtonDown);
				fScanProfiles[i]->SetState(~kSnkSPUndef, kButtonDisabled);
				this->SetEntryEnabled(fOffsetD[i]);
				this->SetEntrySlave(fOffsetS[i]);
				this->SetEntryDisabled(fAmplitudeD[i], "variable");
				this->SetEntryDisabled(fAmplitudeS[i], "variable");
				this->SetEntryDisabled(fNofPixels[i], "variable");
			}
			fPreScales->SetState(kSnkAllBits, kButtonEngaged);
//			fPreScales->SetState(kSnkPS1, kButtonDown);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock, "1");
			this->SetFileEnabled(fDataFile);
			break;
			
		default:
			break;			
	}
	return(kTRUE);
}

Bool_t DDAControlPanel::UpdateValues(TSnkDDA0816 * DDA, ESnkScanMode Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::UpdateValues
// Purpose:        Update GUI settings after check
// Arguments:      TSnkDDA0816 * DDA    -- DDA object
//                 ESnkScanMode Mode    -- scan mode
// Results:        
// Exceptions:     
// Description:    Update GUI settings.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t bit;
			
	DDA->SetScanMode(Mode);

	switch (Mode) {
		case kSnkSMUndef:
			this->InitializeValues(DDA, kSnkSMUndef);
			break;

		case kSnkSMSymm:
			this->SetEntryEnabled(fOffsetD[0]);
			this->SetEntrySlave(fOffsetS[0]);
			this->SetEntryEnabled(fAmplitudeD[0]);
			this->SetEntrySlave(fAmplitudeS[0]);
			this->SetEntryEnabled(fNofPixels[0]);
			bit = fScanProfiles[0]->GetActive();
			fScanProfiles[1]->SetState(kSnkAllBits, kButtonEngaged);
			fScanProfiles[1]->SetState(bit, kButtonDown);
			fScanProfiles[1]->SetState(~bit, kButtonDisabled);
			this->SetEntryEnabled(fOffsetD[1]);
			this->SetEntrySlave(fOffsetS[1]);
			this->SetEntrySlave(fAmplitudeD[1]);
			this->SetEntrySlave(fAmplitudeS[1]);
			fNofPixels[1]->GetEntry()->SetState(kTRUE);
			fNofPixels[1]->GetEntry()->SetText(fNofPixels[0]->GetEntry()->GetText());
			this->SetEntrySlave(fNofPixels[1]);
			break;
			
		case kSnkSMAsymm:
			for (Int_t i = 0; i <= 1; i++) {
				this->SetEntryEnabled(fOffsetD[i]);
				this->SetEntrySlave(fOffsetS[i]);
				this->SetEntryEnabled(fAmplitudeD[i]);
				this->SetEntrySlave(fAmplitudeS[i]);
				this->SetEntryEnabled(fNofPixels[i]);
			}
			this->SetEntryEnabled(fPacerClock);
			break;

		case kSnkSMLShape:
			for (Int_t i = 0; i <= 1; i++) {
				fScanProfiles[i]->SetState(kSnkAllBits, kButtonEngaged);
				fScanProfiles[i]->SetState(kSnkSPTriangle, kButtonDown);
				fScanProfiles[i]->SetState(~kSnkSPTriangle, kButtonDisabled);
				this->SetEntryEnabled(fOffsetD[i]);
				this->SetEntrySlave(fOffsetS[i]);
				this->SetEntryEnabled(fAmplitudeD[i]);
				this->SetEntrySlave(fAmplitudeS[i]);
				this->SetEntryEnabled(fNofPixels[i]);
			}
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock);
			break;

		case kSnkSMXOnly:
			this->SetEntryEnabled(fOffsetD[0]);
			this->SetEntrySlave(fOffsetS[0]);
			this->SetEntryEnabled(fAmplitudeD[0]);
			this->SetEntrySlave(fAmplitudeS[0]);
			this->SetEntryEnabled(fNofPixels[0]);
			fScanProfiles[1]->SetState(kSnkAllBits, kButtonEngaged);
			fScanProfiles[1]->SetState(kSnkSPConstant, kButtonDown);
			fScanProfiles[1]->SetState(~kSnkSPConstant, kButtonDisabled);
			this->SetEntryEnabled(fOffsetD[1]);
			this->SetEntrySlave(fOffsetS[1]);
			this->SetEntryDisabled(fAmplitudeD[1], "0.");
			this->SetEntryDisabled(fAmplitudeS[1], "0");
			this->SetEntryDisabled(fNofPixels[1], "1");
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock);
			break;
			
		case kSnkSMYOnly:
			fScanProfiles[0]->SetState(kSnkAllBits, kButtonEngaged);
			fScanProfiles[0]->SetState(kSnkSPConstant, kButtonDown);
			fScanProfiles[0]->SetState(~kSnkSPConstant, kButtonDisabled);
			this->SetEntryEnabled(fOffsetD[0]);
			this->SetEntrySlave(fOffsetS[0]);
			this->SetEntryDisabled(fAmplitudeD[0], "0.");
			this->SetEntryDisabled(fAmplitudeS[0], "0");
			this->SetEntryDisabled(fNofPixels[0], "1");
			this->SetEntryEnabled(fOffsetD[1]);
			this->SetEntrySlave(fOffsetS[1]);
			this->SetEntryEnabled(fAmplitudeD[1]);
			this->SetEntryEnabled(fNofPixels[1]);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock);
			break;
			
		case kSnkSMXYConst:
			for (Int_t i = 0; i <= 1; i++) {
				fScanProfiles[i]->SetState(kSnkAllBits, kButtonEngaged);
				fScanProfiles[i]->SetState(kSnkSPConstant, kButtonDown);
				fScanProfiles[i]->SetState(~kSnkSPConstant, kButtonDisabled);
				this->SetEntryEnabled(fOffsetD[i]);
				this->SetEntrySlave(fOffsetS[i]);
				this->SetEntryDisabled(fAmplitudeD[i], "0.");
				this->SetEntryDisabled(fAmplitudeS[i], "0");
				this->SetEntryDisabled(fNofPixels[i], "1");
			}
			fPreScales->SetState(kSnkAllBits, kButtonEngaged);
			fPreScales->SetState(kSnkPS1, kButtonDown);
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock);
			break;
			
		case kSnkSMFromFile:
			for (Int_t i = 0; i <= 1; i++) {
				fScanProfiles[i]->SetState(kSnkAllBits, kButtonEngaged);
				fScanProfiles[i]->SetState(kSnkSPUndef, kButtonDown);
				fScanProfiles[i]->SetState(~kSnkSPUndef, kButtonDisabled);
				this->SetEntryEnabled(fOffsetD[i]);
				this->SetEntrySlave(fOffsetS[i]);
				this->SetEntryDisabled(fAmplitudeD[i], "variable");
				this->SetEntryDisabled(fAmplitudeS[i], "variable");
				this->SetEntryDisabled(fNofPixels[i], "variable");
			}
			fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
			fSoftScale->SetState(~kDDASoftScaleOff, kButtonDisabled);
			this->SetEntryEnabled(fPacerClock);
			break;
			
		default:
			break;			
	}
	return(kTRUE);
}

Bool_t DDAControlPanel::UpdateCalibration(TSnkDDA0816 * DDA) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::UpdateCalibration
// Purpose:        Update calibration data
// Arguments:      TSnkDDA0816 * DDA   -- DDA object
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Update calibration.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString value;
	Int_t chargeState;
	Double_t energy, calibX, voltX, calibY, voltY, ampl;
			
	value = fChargeState->GetEntry()->GetText();
	chargeState = value.Atoi();
	value = fEnergy->GetEntry()->GetText();
	energy = value.Atof();

	if (energy <= 0) {
		DDA->ResetCalibration();
	} else {
		value = fCalib[0]->GetEntry()->GetText();
		calibX = value.Atof();
		value = fCalib[1]->GetEntry()->GetText();
		calibY = value.Atof();
		value = fVoltage[0]->GetEntry()->GetText();
		voltX = value.Atof();
		value = fVoltage[1]->GetEntry()->GetText();
		voltY = value.Atof();
		DDA->Calibrate(chargeState, energy, calibX, voltX, calibY, voltY);
	}

	this->SetEntrySlave(fStepWidth[0], Form("%g", DDA->GetStepWidth(0)));
	this->SetEntrySlave(fStepWidth[1], Form("%g", DDA->GetStepWidth(1)));
	this->SetEntrySlave(fRange[0], Form("%g", DDA->GetRange(0)));
	this->SetEntrySlave(fRange[1], Form("%g", DDA->GetRange(1)));

	ampl = DDA->Ampl2Dist(0, kSnkMaxAmplitude);
	fOffsetD[0]->SetRange(-ampl, ampl);
	fOffsetD[0]->SetIncrement(1.);
	fAmplitudeD[0]->SetRange(0., ampl);
	fAmplitudeD[0]->SetIncrement(1.);

	ampl = DDA->Ampl2Dist(1, kSnkMaxAmplitude);
	fOffsetD[1]->SetRange(-ampl, ampl);
	fOffsetD[1]->SetIncrement(1.);
	fAmplitudeD[1]->SetRange(0., ampl);
	fAmplitudeD[1]->SetIncrement(1.);

	return(kTRUE);
}
			
Bool_t DDAControlPanel::UpdateScanLimits(TSnkDDA0816 * DDA) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::UpdateScanLimits
// Purpose:        Update scan limits
// Arguments:      TSnkDDA0816 * DDA  -- DDA object
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Update scan range.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString valueD, valueS;
	TSnkDDAChannel * ddaAddr;
	Int_t intVal;
	Double_t dblVal;
	Int_t chn;
	Int_t offset, stopPos, ampl, npix;
	ESnkScanMode mode;

	chn = 0;

	for (Int_t ddaChn = 0; ddaChn <= 1; ddaChn++) {
		ddaAddr = DDA->GetDac(ddaChn);

		ddaAddr->SetScanProfile((ESnkScanProfile) fScanProfiles[ddaChn]->GetActive());

		valueD = fOffsetD[ddaChn]->GetEntry()->GetText();
		dblVal = valueD.Atof();
		offset = offsetS0.Atoi();
		offset += DDA->Dist2Ampl(ddaChn, dblVal);
		ddaAddr->SetOffset(offset);
		valueS = Form("%d", offset);
		this->SetEntryDisabled(fOffsetS[ddaChn], valueS.Data());
		valueD = fAmplitudeD[ddaChn]->GetEntry()->GetText();
		if (valueD.CompareTo("variable") != 0) {
			dblVal = valueD.Atof();
			ampl = DDA->Dist2Ampl(ddaChn, dblVal);
			ddaAddr->SetAmplitude(ampl);
			valueS = Form("%d", ampl);
			this->SetEntryDisabled(fAmplitudeS[ddaChn], valueS.Data());
		}
		valueD = fStopPosD[ddaChn]->GetEntry()->GetText();
		dblVal = valueD.Atof();
		stopPos = offset + DDA->Dist2Ampl(ddaChn, dblVal);
		offset = offsetS0.Atoi();
		ddaAddr->SetStopPos(dblVal, offset);
		valueS = Form("%d", stopPos);
		this->SetEntryDisabled(fStopPosS[ddaChn], valueS.Data());
		valueS = fNofPixels[ddaChn]->GetEntry()->GetText();
		if (valueS.CompareTo("variable") != 0) {
			npix = valueS.Atoi();
			ddaAddr->SetNofPixels(npix);
		}
		mode = DDA->GetScanMode();

		if (mode == kSnkSMUndef) {
			this->SetEntryDisabled(fStopPosD[ddaChn]);
			this->SetEntryDisabled(fStopPosS[ddaChn]);
		} else {
			this->SetEntryEnabled(fStopPosD[ddaChn]);
			this->SetEntrySlave(fStopPosS[ddaChn]);
		}

		switch (mode) {
			case kSnkSMUndef:
				this->SetEntryDisabled(fOffsetD[ddaChn]);
				this->SetEntryDisabled(fOffsetS[ddaChn]);
				this->SetEntryDisabled(fAmplitudeD[ddaChn]);
				this->SetEntryDisabled(fAmplitudeS[ddaChn]);
				this->SetEntryDisabled(fScanLimitsD[ddaChn], "No Scan");
				this->SetEntryDisabled(fScanLimitsS[ddaChn], "No Scan");
				continue;
			case kSnkSMSymm:
				if (ddaChn == 1) {
					this->SetEntryEnabled(fOffsetD[1]);
					this->SetEntrySlave(fOffsetS[1]);
					valueD = fAmplitudeD[0]->GetEntry()->GetText();
					this->SetEntrySlave(fAmplitudeD[1], valueD.Data());
					dblVal = valueD.Atof();
					ampl = DDA->Dist2Ampl(1, dblVal);
					ddaAddr->SetAmplitude(ampl);
					valueS = Form("%d", ampl);
					this->SetEntrySlave(fAmplitudeS[1], valueS.Data());
					valueS = fNofPixels[0]->GetEntry()->GetText();
					this->SetEntrySlave(fNofPixels[1], valueS.Data());
					intVal = valueS.Atoi();
					ddaAddr->SetNofPixels(intVal);
				}
				this->SetEntryEnabled(fOffsetD[0]);
				this->SetEntrySlave(fOffsetS[0]);
				this->SetEntryEnabled(fAmplitudeD[0]);
				this->SetEntrySlave(fAmplitudeS[0]);
				this->SetEntryEnabled(fNofPixels[0]);
				if (!DDA->AdjustSettings(ddaChn, DDA->IsVerbose())) {
					this->SetEntryDisabled(fScanLimitsD[ddaChn], "No Scan");
					this->SetEntryDisabled(fScanLimitsS[ddaChn], "No Scan");
				} else {
					valueS = ddaAddr->GetAxisName();
					valueS += " = ";
					valueS += Form("%d", ddaAddr->GetOffset() - ddaAddr->GetAmplitude());
					valueS += " ... ";
					valueS += Form("%d", ddaAddr->GetIncrement());
					valueS += " ... ";
					valueS += Form("%d", ddaAddr->GetOffset() + ddaAddr->GetAmplitude() - ddaAddr->GetIncrement());
					valueD = ddaAddr->GetAxisName();
					valueD += " = ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset() - ddaAddr->GetAmplitude()));
					valueD += " ... ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetIncrement()));
					valueD += " ... ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset() + ddaAddr->GetAmplitude() - ddaAddr->GetIncrement()));
					this->SetEntrySlave(fScanLimitsS[ddaChn], valueS.Data());
					this->SetEntrySlave(fScanLimitsD[ddaChn], valueD.Data());
				}
				break;
			case kSnkSMAsymm:
				this->SetEntryEnabled(fOffsetD[ddaChn]);
				this->SetEntrySlave(fOffsetS[ddaChn]);
				this->SetEntryEnabled(fAmplitudeD[ddaChn]);
				this->SetEntrySlave(fAmplitudeS[ddaChn]);
				this->SetEntryEnabled(fNofPixels[ddaChn]);
				if (!DDA->AdjustSettings(ddaChn, DDA->IsVerbose())) {
					this->SetEntryDisabled(fScanLimitsD[ddaChn], "No Scan");
					this->SetEntryDisabled(fScanLimitsS[ddaChn], "No Scan");
				} else {
					valueS = ddaAddr->GetAxisName();
					valueS += " = ";
					valueS += Form("%d", ddaAddr->GetOffset() - ddaAddr->GetAmplitude());
					valueS += " ... ";
					valueS += Form("%d", ddaAddr->GetIncrement());
					valueS += " ... ";
					valueS += Form("%d", ddaAddr->GetOffset() + ddaAddr->GetAmplitude() - ddaAddr->GetIncrement());
					valueD = ddaAddr->GetAxisName();
					valueD += " = ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset() - ddaAddr->GetAmplitude()));
					valueD += " ... ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetIncrement()));
					valueD += " ... ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset() + ddaAddr->GetAmplitude() - ddaAddr->GetIncrement()));
					this->SetEntrySlave(fScanLimitsS[ddaChn], valueS.Data());
					this->SetEntrySlave(fScanLimitsD[ddaChn], valueD.Data());
				}
				break;
			case kSnkSMXOnly:
			case kSnkSMYOnly:
				if (mode == kSnkSMXOnly) 		chn = 1;
				else if (mode == kSnkSMYOnly)	chn = 0;

				this->SetEntryEnabled(fOffsetD[ddaChn]);
				this->SetEntrySlave(fOffsetS[ddaChn]);
				if (ddaChn == chn) {
					this->SetEntryDisabled(fAmplitudeD[ddaChn]);
					this->SetEntryDisabled(fAmplitudeS[ddaChn]);
					fNofPixels[ddaChn]->GetEntry()->SetState(kTRUE);
					this->SetEntryDisabled(fNofPixels[ddaChn]);
					valueS = ddaAddr->GetAxisName();
					valueS += " = ";
					valueS += Form("%d", offset);
					valueS += " const";
					valueD = ddaAddr->GetAxisName();
					valueD += " = ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, offset));
					valueD += " const";
					this->SetEntrySlave(fScanLimitsS[ddaChn], valueS.Data());
					this->SetEntrySlave(fScanLimitsD[ddaChn], valueD.Data());
				} else {
					this->SetEntryEnabled(fAmplitudeD[ddaChn]);
					this->SetEntrySlave(fAmplitudeS[ddaChn]);
					this->SetEntryEnabled(fNofPixels[ddaChn]);
					if (!DDA->AdjustSettings(ddaChn, DDA->IsVerbose())) {
						this->SetEntryDisabled(fScanLimitsD[ddaChn], "No Scan");
						this->SetEntryDisabled(fScanLimitsS[ddaChn], "No Scan");
					} else {
						valueS = ddaAddr->GetAxisName();
						valueS += " = ";
						valueS += Form("%d", ddaAddr->GetOffset() - ddaAddr->GetAmplitude());
						valueS += " ... ";
						valueS += Form("%d", ddaAddr->GetIncrement());
						valueS += " ... ";
						valueS += Form("%d", ddaAddr->GetOffset() + ddaAddr->GetAmplitude() - ddaAddr->GetIncrement());
						valueD = ddaAddr->GetAxisName();
						valueD += " = ";
						valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset() - ddaAddr->GetAmplitude()));
						valueD += " ... ";
						valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetIncrement()));
						valueD += " ... ";
						valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset() + ddaAddr->GetAmplitude() - ddaAddr->GetIncrement()));
						this->SetEntrySlave(fScanLimitsS[ddaChn], valueS.Data());
						this->SetEntrySlave(fScanLimitsD[ddaChn], valueD.Data());
					}
				}
				break;
			case kSnkSMXYConst:
				this->SetEntryEnabled(fOffsetD[ddaChn]);
				this->SetEntrySlave(fOffsetS[ddaChn]);
				this->SetEntryDisabled(fAmplitudeD[ddaChn]);
				this->SetEntryDisabled(fAmplitudeS[ddaChn]);
				this->SetEntryDisabled(fNofPixels[ddaChn]);
				valueS = ddaAddr->GetAxisName();
				valueS += " = ";
				valueS += Form("%d", offset);
				valueS += " const";
				valueD = ddaAddr->GetAxisName();
				valueD += " = ";
				valueD += Form("%g", DDA->Ampl2Dist(ddaChn, offset));
				valueD += " const";
				this->SetEntrySlave(fScanLimitsS[ddaChn], valueS.Data());
				this->SetEntrySlave(fScanLimitsD[ddaChn], valueD.Data());
				break;
			case kSnkSMLShape:
				this->SetEntryEnabled(fOffsetD[ddaChn]);
				this->SetEntrySlave(fOffsetS[ddaChn]);
				this->SetEntryEnabled(fAmplitudeD[ddaChn]);
				this->SetEntrySlave(fAmplitudeS[ddaChn]);
				this->SetEntryEnabled(fNofPixels[ddaChn]);
				if (!DDA->AdjustSettings(ddaChn, DDA->IsVerbose())) {
					this->SetEntryDisabled(fScanLimitsD[ddaChn], "No Scan");
					this->SetEntryDisabled(fScanLimitsS[ddaChn], "No Scan");
				} else {
					valueS = ddaAddr->GetAxisName();
					valueS += " = ";
					valueS += Form("%d", ddaAddr->GetOffset());
					valueS += " ... ";
					valueS += Form("%d", ddaAddr->GetIncrement());
					valueS += " ... ";
					valueS += Form("%d", ddaAddr->GetOffset() + ddaAddr->GetAmplitude() - ddaAddr->GetIncrement());
					valueD = ddaAddr->GetAxisName();
					valueD += " = ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset()));
					valueD += " ... ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetIncrement()));
					valueD += " ... ";
					valueD += Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset() + ddaAddr->GetAmplitude() - ddaAddr->GetIncrement()));
					this->SetEntrySlave(fScanLimitsS[ddaChn], valueS.Data());
					this->SetEntrySlave(fScanLimitsD[ddaChn], valueD.Data());
				}
				break;
			case kSnkSMFromFile:
				this->SetEntryEnabled(fOffsetD[ddaChn]);
				this->SetEntrySlave(fOffsetS[ddaChn]);
				this->SetEntryDisabled(fAmplitudeD[ddaChn]);
				this->SetEntryDisabled(fAmplitudeS[ddaChn]);
				this->SetEntryDisabled(fNofPixels[ddaChn]);
				valueS = ddaAddr->GetAxisName();
				valueS += " variable";
				valueD = ddaAddr->GetAxisName();
				valueD += " variable";
				this->SetEntryDisabled(fScanLimitsS[ddaChn], valueS.Data());
				this->SetEntryDisabled(fScanLimitsD[ddaChn], valueD.Data());
				break;
		}
	}
	return(kTRUE);
}

Bool_t DDAControlPanel::ResetOffset(TSnkDDA0816 * DDA, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::ResetOffset
// Purpose:        Reset offset value
// Arguments:      TSnkDDA0816 * DDA  -- DDA object
//                 Int_t Channel      -- DDA channel
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Reset offset.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	ESnkScanMode mode;
	Int_t offset;
	TSnkDDAChannel * ddaAddr;

	ddaAddr = DDA->GetDac(Channel);

	offset = offsetS0.Atoi();
	ddaAddr->SetOffset(offset);

	switch (mode = DDA->GetScanMode()) {
		case kSnkSMUndef:
			this->SetEntryDisabled(fOffsetD[Channel], "0.");
			this->SetEntryDisabled(fOffsetS[Channel], offsetS0.Data());
			break;
		case kSnkSMSymm:
			if (Channel == 0) {
				this->SetEntryEnabled(fOffsetD[Channel], "0.");
				this->SetEntrySlave(fOffsetS[Channel], offsetS0.Data());
			} else {
				this->SetEntrySlave(fOffsetD[Channel], "0.");
				this->SetEntrySlave(fOffsetS[Channel], offsetS0.Data());
			}
			break;
		case kSnkSMAsymm:
		case kSnkSMXOnly:
		case kSnkSMYOnly:
		case kSnkSMXYConst:
		case kSnkSMLShape:
		case kSnkSMFromFile:
			this->SetEntryEnabled(fOffsetD[Channel], "0.");
			this->SetEntrySlave(fOffsetS[Channel], offsetS0.Data());
			break;
	}
	return(kTRUE);
}

Bool_t DDAControlPanel::ResetStopPos(TSnkDDA0816 * DDA, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::ResetStopPos
// Purpose:        Reset stop position
// Arguments:      TSnkDDA0816 * DDA  -- DDA object
//                 Int_t Channel      -- DDA channel
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Reset stop position.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	ESnkScanMode mode;
	TSnkDDAChannel * ddaAddr;

	ddaAddr = DDA->GetDac(Channel);

	mode = DDA->GetScanMode();
	if (mode == kSnkSMUndef) {
		this->SetEntryDisabled(fStopPosD[Channel], "0.");
		this->SetEntryDisabled(fStopPosS[Channel], offsetS0.Data());
	} else {
		this->SetEntryEnabled(fStopPosD[Channel], "0.");
		this->SetEntrySlave(fStopPosS[Channel], offsetS0.Data());
	}
	return(kTRUE);
}

Bool_t DDAControlPanel::DDA2Gui(TSnkDDA0816 * DDA) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::DDA2Gui
// Purpose:        Set panel values from current DDA
// Arguments:      TSnkDDA0816 * DDA  -- DDA object
// Results:        
// Exceptions:     
// Description:    Displays current DDA values on panel.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t ddaChn;
	TSnkDDAChannel * ddaAddr;
	TString value, intStr, dblStr;
	Int_t softScale;
	Int_t offset;

	fSubdevs->SetState(DDA->GetSubdevice() + kDDASubdevA, kButtonDown);
	
	for (ddaChn = 0; ddaChn <= 1; ddaChn++) {
		ddaAddr = DDA->GetDac(ddaChn);

		fScanProfiles[ddaChn]->SetState(kSnkAllBits, kButtonEngaged);
		fScanProfiles[ddaChn]->SetState(ddaAddr->GetScanProfile(), kButtonDown);

		value = Form("%d", ddaAddr->GetOffset());
		fOffsetS[ddaChn]->GetEntry()->SetState(kTRUE);
		fOffsetS[ddaChn]->GetEntry()->SetText(value.Data());
		fOffsetS[ddaChn]->GetEntry()->SetState(kFALSE);
		offset = offsetS0.Atoi();
		dblStr = Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetOffset() - offset));
		fOffsetD[ddaChn]->GetEntry()->SetState(kTRUE);
		fOffsetD[ddaChn]->GetEntry()->SetText(dblStr.Data());

		value = Form("%d", ddaAddr->GetAmplitude());
		fAmplitudeS[ddaChn]->GetEntry()->SetState(kTRUE);
		fAmplitudeS[ddaChn]->GetEntry()->SetText(value.Data());
		fAmplitudeS[ddaChn]->GetEntry()->SetState(kFALSE);
		dblStr = Form("%g", DDA->Ampl2Dist(ddaChn, ddaAddr->GetAmplitude()));
		fAmplitudeD[ddaChn]->GetEntry()->SetState(kTRUE);
		fAmplitudeD[ddaChn]->GetEntry()->SetText(dblStr.Data());

		dblStr = Form("%g", ddaAddr->GetStopPos());
		fStopPosD[ddaChn]->GetEntry()->SetState(kTRUE);
		fStopPosD[ddaChn]->GetEntry()->SetText(dblStr.Data());

		intStr = Form("%d", ddaAddr->GetPos0());
		fStopPosS[ddaChn]->GetEntry()->SetState(kTRUE);
		fStopPosS[ddaChn]->GetEntry()->SetText(intStr.Data());


		value = Form("%d", ddaAddr->GetNofPixels());
		fNofPixels[ddaChn]->GetEntry()->SetState(kTRUE);
		fNofPixels[ddaChn]->GetEntry()->SetText(value.Data());
	}

	this->UpdateScanLimits(DDA);

	Int_t cycles = DDA->GetCycleCount();
	value = Form("%d", cycles);
	fCycles->GetEntry()->SetText(value.Data());

	Int_t preScale = DDA->GetPreScaler()->GetIndex();
	fPreScales->SetState(kSnkAllBits, kButtonEngaged);
	fPreScales->SetState(preScale, kButtonDown);

	Int_t pacerClock = DDA->GetPacerClock();
	if (preScale == kSnkPS100k && pacerClock < 2) pacerClock = 2;
	value = Form("%d", pacerClock);
	fPacerClock->GetEntry()->SetText(value.Data());

	fSoftScale->SetState(kSnkAllBits, kButtonEngaged);
	fSoftScale->ClearAll();
	if ((softScale = DDA->GetSoftScale(0)) > 0) {
		fSoftScale->SetState(kDDASoftScaleY, kButtonDown);
	} else if ((softScale = DDA->GetSoftScale(1)) > 0) {
		fSoftScale->SetState(kDDASoftScaleX, kButtonDown);
	} else {
		fSoftScale->SetState(kDDASoftScaleOff, kButtonDown);
		softScale = 0;
	}
	value = Form("%d", softScale);
	this->SetEntrySlave(fScaleValue, value.Data());

	fScanModes->SetState(DDA->GetScanMode(), kButtonDown);

	intStr = Form("%d", DDA->GetChargeState());
	fChargeState->GetEntry()->SetText(intStr.Data());
	dblStr = Form("%g", DDA->GetEnergy());
	fEnergy->GetEntry()->SetText(dblStr.Data());
	dblStr = Form("%g", DDA->GetCalibration(0));
	fCalib[0]->GetEntry()->SetText(dblStr.Data());
	dblStr = Form("%g", DDA->GetCalibration(1));
	fCalib[1]->GetEntry()->SetText(dblStr.Data());
	dblStr = Form("%g", DDA->GetVoltage(0));
	fVoltage[0]->GetEntry()->SetText(dblStr.Data());
	dblStr = Form("%g", DDA->GetVoltage(1));
	fVoltage[1]->GetEntry()->SetText(dblStr.Data());

	TString dataFile = DDA->GetScanDataFile();
	if (DDA->GetScanMode() == kSnkSMFromFile && dataFile.Length() > 0) {
		this->SetFileEnabled(fDataFile, DDA->GetScanDataFile());
	} else {
		this->SetFileDisabled(fDataFile);
	}

	return(kTRUE);
}

Bool_t DDAControlPanel::Gui2DDA(TSnkDDA0816 * DDA, Bool_t SaveFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::Gui2DDA
// Purpose:        Set DDA from GUI values
// Arguments:      TSnkDDA0816 * DDA     -- DDA0816 object
//                 Bool_t SaveFlag       -- kTRUE if data should be saved      
// Results:        
// Exceptions:     
// Description:    Reads GUI values and sets DDA accordingly.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t ddaChn;
	TString offset;
	TString range;
	TString npoints;
	TString pacer;
	TString cycles;
	TString scaleY;
	TString stopPos;
	TString fileName;
		
	TSnkDDAChannel * ddaAddr;

	ESnkScanMode scanMode;

	for (ddaChn = 0; ddaChn <= 1; ddaChn++) {
		ddaAddr = DDA->GetDac(ddaChn);

		ddaAddr->SetScanProfile((ESnkScanProfile) fScanProfiles[ddaChn]->GetActive());

		offset = fOffsetS[ddaChn]->GetEntry()->GetText();
		ddaAddr->SetOffset(offset.Atoi());

		range = fAmplitudeS[ddaChn]->GetEntry()->GetText();
		if (range.CompareTo("variable") != 0) ddaAddr->SetAmplitude(range.Atoi());
		npoints = fNofPixels[ddaChn]->GetEntry()->GetText();
		if (npoints.CompareTo("variable") != 0) ddaAddr->SetNofPixels(npoints.Atoi());

		stopPos = fStopPosD[ddaChn]->GetEntry()->GetText();
		ddaAddr->SetStopPos(stopPos.Atof(), offsetS0.Atoi());
	}

	DDA->SetPreScaler((ESnkPreScale) fPreScales->GetActive());

	DDA->SetAmplYScale(1.0);		// obsolete
	
	cycles = fCycles->GetEntry()->GetText();
	DDA->SetCycleCount(cycles.Atoi());

	pacer = fPacerClock->GetEntry()->GetText();
	DDA->SetPacerClock(pacer.Atoi());

	DDA->ClearSoftScale();
	switch (fSoftScale->GetActive()) {
		case kDDASoftScaleX:	DDA->SetSoftScale(1); break;
		case kDDASoftScaleY:	DDA->SetSoftScale(0); break;
	}

	DDA->SetScanDataFile("");

	scanMode = (ESnkScanMode) fScanModes->GetActive();
	switch (scanMode) {
		case kSnkSMSymm:
			if (!DDA->SetCurveSymm(SaveFlag)) return(kFALSE);
			this->DDA2Gui(DDA);
			break;
		case kSnkSMAsymm:
			if (!DDA->SetCurveAsymm(SaveFlag)) return(kFALSE);
			this->DDA2Gui(DDA);
			break;
		case kSnkSMLShape:
			if (!DDA->SetCurveLShape(SaveFlag)) return(kFALSE);
			this->DDA2Gui(DDA);
			break;
		case kSnkSMXOnly:
			if (!DDA->SetCurveXOnly(SaveFlag)) return(kFALSE);
			this->DDA2Gui(DDA);
			break;
		case kSnkSMYOnly:
			if (!DDA->SetCurveYOnly(SaveFlag)) return(kFALSE);
			this->DDA2Gui(DDA);
			break;
		case kSnkSMXYConst:
			if (!DDA->SetCurveXYConst(SaveFlag)) return(kFALSE);
			this->DDA2Gui(DDA);
			break;
		case kSnkSMFromFile:
			fileName = fDataFile->GetEntry()->GetText();
			fileName = fileName.Strip(TString::kBoth);
			if (fileName.Length() == 0) {
				gMrbLog->Err() << "No file name given" << endl;
				gMrbLog->Flush(this->ClassName() , "Gui2DDA");
				new TGMsgBox(fClient->GetRoot(), this, "DDAControl: Error", "No file name given", kMBIconStop);
				return(kFALSE);
			}
			if (!DDA->SetCurveFromFile(fileName, SaveFlag)) return(kFALSE);
			DDA->SetScanDataFile(fileName.Data());
			this->DDA2Gui(DDA);
			break;
		default:
			gMrbLog->Err() << "No SCAN MODE defined" << endl;
			gMrbLog->Flush(this->ClassName() , "Gui2DDA");
			new TGMsgBox(fClient->GetRoot(), this, "DDAControl: Error", "No SCAN MODE defined", kMBIconStop);
			return(kFALSE);
	}
	return(kTRUE);
}

void DDAControlPanel::CloseWindow() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::CloseWindow
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

void DDAControlPanel::EntryChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::EntryChanged
// Purpose:        Slot method: update after entry changed
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (FrameId) {
		case kDDAEntryChargeState:
		case kDDAEntryEnergy:
		case kDDAEntryCalibX:
		case kDDAEntryCalibY:
		case kDDAEntryVoltageX:
		case kDDAEntryVoltageY:
			this->UpdateCalibration(curDDA);
			this->UpdateScanLimits(curDDA);
			break;
		case kDDAEntryOffset0:
		case kDDAEntryOffset1:
		case kDDAEntryAmplitude0:
		case kDDAEntryAmplitude1:
		case kDDAEntryStopPos0:
		case kDDAEntryStopPos1:
		case kDDAEntrySize0:
		case kDDAEntrySize1:
			this->UpdateScanLimits(curDDA);
			break;
		default:
			break;
	}
}

void DDAControlPanel::ButtonPressed(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::ButtonPressed
// Purpose:        Signal catcher for buttons
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:     
// Description:    Will be called on button events.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDDAControlCheck:
			curDDA->SetSubdevice(fSubdevs->GetActive() - kDDASubdevA);
			if (this->Gui2DDA(curDDA, kFALSE)) {
				curDDA->SaveDefault(kFALSE);
				curDDA->SaveDefault(kTRUE, "DDAControl");
				curDDA->Save("DDA_ScanData.root");
				curDDA->Save("DDA_ScanData.dda", "DDAControl");
			}
			this->UpdateValues(curDDA, curDDA->GetScanMode());
			break;

		case kDDAControlConnect:
			if (!curDDA->IsConnected()) {
				if (!curDDA->ConnectToServer(fServerName.Data(), fServerPort, fServerProg.Data())) break;
			}
			break;

		case kDDAControlStartScan:
			{
				curDDA->SetSubdevice(fSubdevs->GetActive() - kDDASubdevA);
				if (this->Gui2DDA(curDDA, kTRUE)) {
					if (!curDDA->IsConnected()) {
						if (!curDDA->ConnectToServer(fServerName.Data(), fServerPort, fServerProg.Data())) break;
					}
					curDDA->Save("DDA_ScanData.root");
					curDDA->Save("DDA_ScanData.dda", "DDAControl");
					TString secs = fScanPeriod->GetEntry()->GetText();
					Double_t dblSecs = secs.Atof();
					TString cycles = fCycles->GetEntry()->GetText();
					Int_t intCycles = cycles.Atoi();
					curDDA->StartScan(dblSecs, intCycles);
				}
			}
			break;

		case kDDAControlStopScan:
			if (curDDA && curDDA->IsConnected()) curDDA->StopScan();
			break;

		case kDDAControlKillServer:
			if (curDDA && curDDA->IsConnected()) {
				curDDA->StopScan();
				sleep(2);
				curDDA->KillServer();
			}
			break;

		case kDDAPictOpen:
			this->OpenScanFile();
			break;

		case kDDAPictSave:
			curDDA->SetSubdevice(fSubdevs->GetActive() - kDDASubdevA);
			if (this->Gui2DDA(curDDA, kFALSE)) {
				curDDA->SaveDefault(kFALSE);
				curDDA->SaveDefault(kTRUE, "DDAControl");
				curDDA->Save("DDA_ScanData.root");
				curDDA->Save("DDA_ScanData.dda", "DDAControl");
			}
			break;

		case kDDAPictExit:
			this->CloseWindow();
			break;

		default:	break;
	}
}

void DDAControlPanel::SetScanMode(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::SetScanMode
// Purpose:        Signal catcher for radio buttons (scan mode)
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:     
// Description:    Will be called when scan mode has changed.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fScanModes->SetState(Selection, kButtonDown);
	this->InitializeValues(curDDA, (ESnkScanMode) Selection);
	this->UpdateScanLimits(curDDA);
}

void DDAControlPanel::PerformAction(Int_t Action) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::PerformAction
// Purpose:        Signal catcher for action buttons
// Arguments:      Int_t Action     -- button id
// Results:        --
// Exceptions:     
// Description:    Will be called on action button pressed.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Action) {
		case kDDAOffsetZero0:
			this->ResetOffset(curDDA, 0);
			break;

		case kDDAOffsetZero1:
			this->ResetOffset(curDDA, 1);
			break;

		case kDDAStopZero0:
			this->ResetStopPos(curDDA, 0);
			break;

		case kDDAStopZero1:
			this->ResetStopPos(curDDA, 1);
			break;
	}
}

void DDAControlPanel::Activate(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::Activate
// Purpose:        Slot method: activate menu entry
// Arguments:      Int_t Selection    -- selected entry
// Results:        
// Exceptions:     
// Description:    Called if popup menu entry activated
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDDAFileOpen:
			this->OpenScanFile();
			break;

		case kDDAFileReset:
			this->Reset();
			break;

		case kDDAFilePrint:
			curDDA->Print();
			break;

		case kDDAFileSave:
			curDDA->SetSubdevice(fSubdevs->GetActive() - kDDASubdevA);
			if (this->Gui2DDA(curDDA, kFALSE)) {
				curDDA->SaveDefault(kFALSE);
				curDDA->SaveDefault(kTRUE, "DDAControl");
				curDDA->Save("DDA_ScanData.root");
				curDDA->Save("DDA_ScanData.dda", "DDAControl");
			}
			break;

		case kDDAFileSaveAs:
			{
				TGFileInfo fi;
				fi.fFileTypes = (const Char_t **) FileTypes1;
				new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);
				if (this->Gui2DDA(curDDA, kFALSE)) {
					curDDA->SetSubdevice(fSubdevs->GetActive() - kDDASubdevA);
					curDDA->Save(fi.fFilename, "DDAControl");
				}
			}
			break;

		case kDDAFileExit:
			this->CloseWindow();
			break;

		case kDDASettingsNormal:
			curDDA->SetNormalMode();
			fMenuSettings->RCheckEntry(kDDASettingsNormal, kDDASettingsNormal, kDDASettingsDebug);
			cout << "DDAControl: Resetting to NORMAL mode" << endl;
			break;

		case kDDASettingsVerbose:
			curDDA->SetVerbose();
			fMenuSettings->RCheckEntry(kDDASettingsVerbose, kDDASettingsNormal, kDDASettingsDebug);
			cout << "DDAControl: Setting VERBOSE mode" << endl;
			break;

		case kDDASettingsDebug:
			curDDA->SetDebug();
			fMenuSettings->RCheckEntry(kDDASettingsDebug, kDDASettingsNormal, kDDASettingsDebug);
			cout << "DDAControl: Setting DEBUG mode" << endl;
			break;

		case kDDASettingsOnline:
			curDDA->SetOnline();
			fMenuSettings->RCheckEntry(kDDASettingsOnline, kDDASettingsOnline, kDDASettingsOffline);
			cout << "DDAControl: Setting ONLINE mode" << endl;
			break;

		case kDDASettingsOffline:
			curDDA->SetOffline();
			fMenuSettings->RCheckEntry(kDDASettingsOffline, kDDASettingsOnline, kDDASettingsOffline);
			cout << "DDAControl: Setting OFFLINE mode" << endl;
			break;

		case kDDAServerOldDDA:
			fMenuSettings->RCheckEntry(kDDAServerOldDDA, kDDAServerNewDDA, kDDAServerOldDDA);
			cout << "DDAControl: Server program is now DDAExec @ mamba" << endl;
			fServerName = "mamba";
			fServerProg = "DDAExec";
			break;

		case kDDAServerNewDDA:
			fMenuSettings->RCheckEntry(kDDAServerNewDDA, kDDAServerNewDDA, kDDAServerOldDDA);
			cout << "DDAControl: Server program is now DDAExecME @ mamba2" << endl;
			fServerName = "mamba2";
			fServerProg = "DDAExecME";
			break;
	}
}

void DDAControlPanel::KeyPressed(Int_t FrameId, Int_t Key) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::KeyPressed
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
			this->CloseWindow();
			break;

		case TGMrbLofKeyBindings::kGMrbKeyActionSave:
			curDDA->SetSubdevice(fSubdevs->GetActive() - kDDASubdevA);
			if (this->Gui2DDA(curDDA, kFALSE)) {
				curDDA->SaveDefault(kFALSE);
				curDDA->SaveDefault(kTRUE, "DDAControl");
				curDDA->Save("DDA_ScanData.root");
				curDDA->Save("DDA_ScanData.dda", "DDAControl");
			}
			break;

		case TGMrbLofKeyBindings::kGMrbKeyActionOpen:
			this->OpenScanFile();
			break;

		case TGMrbLofKeyBindings::kGMrbKeyActionPrint:
			curDDA->Print();
			break;

		case kDDAKeyActionReset:
			this->Reset();
			break;
	}
}

Bool_t DDAControlPanel::OpenScanFile(const Char_t * ScanFileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::OpenScanFile
// Purpose:        Open a DDA0816 data file
// Arguments:      Char_t * ScanFileName   -- file name
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Opens a DDA0816 data file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString scanFileName;
	TString server;
	Int_t port;

	UInt_t execMode = kSnkEMNormal;
	TSocket * socket = NULL;

	if (ScanFileName == NULL || *ScanFileName == '\0') {
		TGFileInfo fi;
		fi.fFileTypes = (const Char_t **) FileTypes1;
		new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
		if (fi.fFilename == NULL || *fi.fFilename == '\0') return(kFALSE);
		scanFileName = fi.fFilename;
	} else {
		scanFileName = ScanFileName;
	}

	Bool_t readFromDDA = (scanFileName.Index(".dda", 0) == (scanFileName.Length() - 4));
	Bool_t readFromRoot = (scanFileName.Index(".root", 0) == (scanFileName.Length() - 5));

	if (curDDA != NULL) {
		execMode = curDDA->GetExecMode();
		socket = curDDA->GetConnection(server, port);
	}

	if (readFromDDA) {
		if (!this->OpenDDAFile(scanFileName.Data())) return(kFALSE);
	} else if (readFromRoot) {
		TFile * ddaFile = new TFile(scanFileName.Data());
		if (ddaFile->IsOpen()) {
			if (curDDA != NULL) delete curDDA;
			curDDA = (TSnkDDA0816 *) ddaFile->Get("DDA0816");
			ddaFile->Close();
			delete ddaFile;
		} else {
			TString msg = Form("Can't open scan file - %s", scanFileName.Data());
			gMrbLog->Err() << msg << endl;
			gMrbLog->Flush(this->ClassName() , "OpenScanFile");
			delete ddaFile;
			new TGMsgBox(fClient->GetRoot(), this, "DDAControl: Error", msg, kMBIconStop);
			return(kFALSE);
		}
	} else {
		TString msg = Form("Illegal file name - %s (ext has to be \".dda\" or \".root\")", scanFileName.Data());
		gMrbLog->Err() << msg << endl;
		gMrbLog->Flush(this->ClassName() , "OpenScanFile");
		new TGMsgBox(fClient->GetRoot(), this, "DDAControl: Error", msg, kMBIconStop);
		return(kFALSE);
	}

	if (this->DDA2Gui(curDDA)) {
		curDDA->SetExecMode(execMode);
		if (socket) curDDA->SetConnection(socket, server.Data(), port);
		this->UpdateValues(curDDA, curDDA->GetScanMode());
		this->UpdateCalibration(curDDA);
		return(kTRUE);
	} else {
		return(kFALSE);
	}
}

void DDAControlPanel::SetEntryEnabled(TGMrbLabelEntry * Entry, const Char_t * Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::SetEntryEnabled
// Purpose:        Enable an entry field
// Arguments:      TGMrbLabelEntry * Entry  -- entry
//                 Char_t * Value           -- value to be set
// Results:        --
// Exceptions:     
// Description:    Enables an entry:
//                     (a) set editable (state TRUE)
//                     (b) set background WHITE
//                     (c) enable up/down buttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Entry->GetEntry()->SetState(kTRUE);
	Entry->GetEntry()->ChangeBackground(cWhite);
	if (Value != NULL) Entry->GetEntry()->SetText(Value);
	Entry->UpDownButtonEnable(kTRUE);
}

void DDAControlPanel::SetEntryDisabled(TGMrbLabelEntry * Entry, const Char_t * Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::SetEntryDisabled
// Purpose:        Disable entry
// Arguments:      TGMrbLabelEntry * Entry  -- entry
//                 Char_t * Value           -- value to be set
// Results:        --
// Exceptions:     
// Description:    Disables an entry:
//                     (a) set ineditable (state FALSE)
//                     (b) set background GRAY
//                     (c) disable up/down buttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Value != NULL) {
		Entry->GetEntry()->SetState(kTRUE);
		Entry->GetEntry()->SetText(Value);
	}
	Entry->GetEntry()->SetState(kFALSE);
	Entry->GetEntry()->ChangeBackground(cGray);
	Entry->UpDownButtonEnable(kFALSE);
}

void DDAControlPanel::SetEntrySlave(TGMrbLabelEntry * Entry, const Char_t * Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::SetEntrySlave
// Purpose:        Set entry slave
// Arguments:      TGMrbLabelEntry * Entry  -- entry
//                 Char_t * Value           -- value to be set
// Results:        --
// Exceptions:     
// Description:    Defines an entry to be slave:
//                     (a) set ineditable (state FALSE)
//                     (b) set background YELLOW
//                     (c) disable up/down buttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Value != NULL) {
		Entry->GetEntry()->SetState(kTRUE);
		Entry->GetEntry()->SetText(Value);
	}
	Entry->GetEntry()->SetState(kFALSE);
	Entry->GetEntry()->ChangeBackground(cYellow);
	Entry->UpDownButtonEnable(kFALSE);
}

void DDAControlPanel::SetFileEnabled(TGMrbFileEntry * Entry, const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::SetFileEnabled
// Purpose:        Enable an entry field
// Arguments:      TGMrbFileEntry * Entry  -- entry
// Results:        --
// Exceptions:     
// Description:    Enables an entry:
//                     (a) set editable (state TRUE)
//                     (b) set background WHITE
//                     (c) enable file button
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Entry->GetEntry()->SetState(kTRUE);
	Entry->GetEntry()->ChangeBackground(cWhite);
	if (FileName != NULL && *FileName != '\0') Entry->GetEntry()->SetText(FileName);
	Entry->FileButtonEnable(kTRUE);
}

void DDAControlPanel::SetFileDisabled(TGMrbFileEntry * Entry, const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::SetFileDisabled
// Purpose:        Disable entry
// Arguments:      TGMrbFileEntry * Entry  -- entry
// Results:        --
// Exceptions:     
// Description:    Disables an entry:
//                     (a) set ineditable (state FALSE)
//                     (b) set background GRAY
//                     (c) disable file button
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (FileName != NULL && *FileName != '\0') {
		Entry->GetEntry()->SetState(kTRUE);
		Entry->GetEntry()->SetText(FileName);
	}
	Entry->GetEntry()->SetState(kFALSE);
	Entry->GetEntry()->ChangeBackground(cGray);
	Entry->FileButtonEnable(kFALSE);
}

Bool_t DDAControlPanel::OpenDDAFile(const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DDAControlPanel::OpenDDAFile
// Purpose:        Read DDA object from environment file
// Arguments:      const Char_t * FileName    -- file name (.dda)
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Reads DDA values from .dda file
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbSystem ux;
	TString ddaFile;

	ddaFile = FileName;
//	ux.GetBaseName(ddaFile, FileName);

	TMrbEnv * dda = new TMrbEnv(ddaFile.Data());
	if (dda->IsZombie()) {
		gMrbLog->Err() << "Can't open file - " << FileName << endl;
		gMrbLog->Flush(this->ClassName(), "OpenDDAFile");
		delete dda;
		return(kFALSE);
	}

	dda->SetPrefix("DDAControl");

	TString dacName;
	dda->Get(dacName, "DacX.Name", "");
	if (dacName.IsNull()) {
		gMrbLog->Err() << "DacX undefined - file probably corrupted" << endl;
		gMrbLog->Flush(this->ClassName(), "OpenDDAFile");
		delete dda;
		return(kFALSE);
	}
	dda->Get(dacName, "DacY.Name", "");
	if (dacName.IsNull()) {
		gMrbLog->Err() << "DacY undefined - file probably corrupted" << endl;
		gMrbLog->Flush(this->ClassName(), "OpenDDAFile");
		delete dda;
		return(kFALSE);
	}

	// common settings
	curDDA->SetSubdevice(dda->Get("Subdevice.Index", 0));
	curDDA->SetPacerClock(dda->Get("PacerClock", 0));

	TMrbNamedX namedVal;

	dda->Get(namedVal, "PreScale");
	TMrbNamedX * nx = curDDA->GetLofPreScales()->FindByName(namedVal.GetName());
	if (nx == NULL) {
		gMrbLog->Err()	<< "Wrong prescale value - "
						<< namedVal.GetName()
						<< "(" << namedVal.GetIndex() << ")"
						<< endl;
		gMrbLog->Flush(this->ClassName(), "OpenDDAFile");
		delete dda;
		return(kFALSE);
	}
	curDDA->SetPreScaler((ESnkPreScale) nx->GetIndex());

	dda->Get(namedVal, "ClockSource");
	nx = curDDA->GetLofClockSources()->FindByName(namedVal.GetName());
	if (nx == NULL) {
		gMrbLog->Err()	<< "Wrong clock source - "
						<< namedVal.GetName()
						<< "(" << namedVal.GetIndex() << ")"
						<< endl;
		gMrbLog->Flush(this->ClassName(), "OpenDDAFile");
		delete dda;
		return(kFALSE);
	}
	curDDA->SetClockSource((ESnkClockSource) nx->GetIndex());

	dda->Get(namedVal, "ScanMode");
	nx = curDDA->GetLofScanModes()->FindByName(namedVal.GetName());
	if (nx == NULL) {
		gMrbLog->Err()	<< "Wrong scan mode - "
						<< namedVal.GetName()
						<< "(" << namedVal.GetIndex() << ")"
						<< endl;
		gMrbLog->Flush(this->ClassName(), "OpenDDAFile");
		delete dda;
		return(kFALSE);
	}
	curDDA->SetScanMode((ESnkScanMode) nx->GetIndex());

	TString res;
	fScanPeriod->GetEntry()->SetText(dda->Get(res, "ScanPeriod", "0.0"));
	fCycles->GetEntry()->SetText(dda->Get(res, "Cycles", "0"));

// calibration
	curDDA->Calibrate(	dda->Get("ChargeState", 0),
						dda->Get("Energy", 0.0),
						dda->Get("DacX.Calibration", 1.0),
						dda->Get("DacX.Voltage", 0.0),
						dda->Get("DacY.Calibration", 1.0),
						dda->Get("DacY.Voltage", 0.0));

// channel data
	TString pf("XYHB");
	for (Int_t chn = 0; chn <= 1; chn++) {
		TString prefix = "DDAControl.Dac";
		prefix += pf(chn);
		dda->SetPrefix(prefix.Data());
		curDDA->SetOffset(chn, dda->Get("Offset", 0));
		curDDA->SetAmplitude(chn, dda->Get("Amplitude",0));
		curDDA->SetIncrement(chn, dda->Get("Increment", 0));
		curDDA->GetDac(chn)->SetNofPixels(dda->Get("NofPixels", 0));
		dda->Get(namedVal, "ScanProfile");
		nx = curDDA->GetLofScanProfiles()->FindByIndex(namedVal.GetIndex());
		if (nx == NULL) {
			gMrbLog->Err()	<< "Channel Dac" << pf(chn) << "(" << chn << "): Wrong scan profile - "
							<< namedVal.GetName()
							<< "(" << namedVal.GetIndex() << ")"
							<< endl;
			gMrbLog->Flush(this->ClassName(), "OpenDDAFile");
			delete dda;
			return(kFALSE);
		}
		curDDA->SetScanProfile(chn, (ESnkScanProfile) nx->GetIndex());
		Double_t stopAt = dda->Get("StopAt", 0.0);
		Int_t pos0 = dda->Get("Pos0", 0);
		curDDA->GetDac(chn)->SetStopPos(stopAt, pos0);
	}
	curDDA->ClearSoftScale();
	dda->SetPrefix("DDAControl");
	if (dda->Get("DacX.SoftScale", 0) > 0) curDDA->SetSoftScale(0);
	if (dda->Get("DacY.SoftScale", 0) > 0) curDDA->SetSoftScale(1);

	delete dda;
	return(kTRUE);
}
