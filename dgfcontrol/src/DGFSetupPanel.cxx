//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFSetupPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
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
<img src=dgfcontrol/DGFSetupPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TROOT.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TOrdCollection.h"

#include "TMrbLofDGFs.h"
#include "TMrbLogger.h"
#include "TMrbEnv.h"

#include "TGMsgBox.h"

#include "DGFControlData.h"
#include "DGFSetupPanel.h"

#include "SetColor.h"

const SMrbNamedXShort kDGFSetupDGFModes[] =
							{
								{DGFControlData::kDGFSimulStartStop,		"Simultaneously start/stop modules" },
								{DGFControlData::kDGFSyncClocks,			"Synchronize clocks with run"		},
								{DGFControlData::kDGFIndivSwitchBusTerm,	"Terminate switchbus individually"	},
								{0, 										NULL								}
							};

const SMrbNamedXShort kDGFSetupDGFCodes[] =
							{
								{DGFSetupPanel::kDGFSetupCodeSystemFPGA,	"System FPGA"	},
								{DGFSetupPanel::kDGFSetupCodeFippiFPGA, 	"Fippi FPGA"	},
								{DGFSetupPanel::kDGFSetupCodeDSP,		 	"DSP"			},
								{0, 										NULL			}
							};

const SMrbNamedX kDGFSetupConnect[] =
			{
				{DGFSetupPanel::kDGFSetupConnectConnect,		"Connect",		"Connect to DGF modules w/o downloading code" },
				{DGFSetupPanel::kDGFSetupConnectReloadDGFs,		"Reload DGFs",	"Download FPGA and DSP code"			},
				{DGFSetupPanel::kDGFSetupConnectAbortBusySync,	"Abort Busy-Sync","Force return from busy-sync-loop"			},
				{DGFSetupPanel::kDGFSetupConnectRestartEsone,	"Restart ESONE","Restart ESONE server"			},
				{DGFSetupPanel::kDGFSetupConnectAbortEsone, 	"Abort ESONE Restart","Abort ESONE restart procedure"			},
				{DGFSetupPanel::kDGFSetupConnectClose,			"Close",		"Close window"					},
				{0, 											NULL,			NULL							}
			};

const SMrbNamedXShort kDGFSetupAccessMode[] =
			{
				{DGFSetupPanel::kDGFSetupBroadCast,			"broadcast" 				},
				{DGFSetupPanel::kDGFSetupSingleDGF,			"single"		 			},
				{DGFSetupPanel::kDGFSetupRemoteShell,		"rsh"			 			},
				{0, 										NULL						}
			};

TMrbEsone * esoneCold = NULL;

Bool_t firstCall = kTRUE;

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

ClassImp(DGFSetupPanel)

DGFSetupPanel::DGFSetupPanel(const TGWindow * Window, const TGWindow * MainFrame, UInt_t Width, UInt_t Height, UInt_t Options)
														: TGTransientFrame(Window, MainFrame, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel
// Purpose:        DGF Viewer: Setup Panel
// Arguments:      TGWindow Window      -- connection to ROOT graphics
//                 TGWindow * MainFrame -- main frame
//                 UInt_t Width         -- window width in pixels
//                 UInt_t Height        -- window height in pixels
//                 UInt_t Options       -- options
// Results:        
// Exceptions:     
// Description:    Implements DGF Viewer's Setup Panel
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;

	TMrbString camacHost;
	TMrbString intStr;
			
	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;
	
	TMrbEnv env;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorBlue);	HEAP(frameGC);
	
	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorBlue);	HEAP(groupGC);
	
	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(labelGC);
	
	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);
	
	TGLayoutHints * cnFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	gDGFControlData->SetLH(groupGC, frameGC, cnFrameLayout);
	HEAP(cnFrameLayout);
	TGLayoutHints * cnLabelLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
	labelGC->SetLH(cnLabelLayout);
	HEAP(cnLabelLayout);
//	TGLayoutHints * cnButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
//	buttonGC->SetLH(cnButtonLayout);
//	HEAP(cnButtonLayout);

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFSetupModuleSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFSetupModuleSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFSetupModuleSelectNone, "cbutton_none.xpm");
		
//	Initialize several lists
	fSetupDGFModes.SetName("DGF Modes");				// checkbutton list:	DGF modes
	fSetupDGFModes.AddNamedX(kDGFSetupDGFModes);
	fSetupDGFModes.SetPatternMode();
	fSetupDGFCodes.SetName("DGF Codes");				// checkbutton list:	DGF codes
	fSetupDGFCodes.AddNamedX(kDGFSetupDGFCodes);
	fSetupDGFCodes.SetPatternMode();
	fSetupConnect.SetName("Connect to CAMAC");			// button list: 		connect to CAMAC
	fSetupConnect.AddNamedX(kDGFSetupConnect);
	fSetupBroadCast.SetName("AccessMode");				// button list: 		single/broadcast/remote
	fSetupBroadCast.AddNamedX(kDGFSetupAccessMode);

// DGF defs
	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);
	TGLayoutHints * dgfButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 10, 1);
	labelGC->SetLH(dgfButtonLayout);
	HEAP(dgfButtonLayout);
	fDGFFrame = new TGMrbCheckButtonGroup(this, "DGF General", &fSetupDGFModes, 1, groupGC, labelGC, NULL, kVerticalFrame);
	HEAP(fDGFFrame);
	this->AddFrame(fDGFFrame, groupGC->LH());
	fDGFFrame->Associate(this);
	fDGFFrame->SetState(gDGFControlData->fStatus);

	if (firstCall) {
		gDGFControlData->fSimulStartStop = gEnv->GetValue("DGFControl.StartStopSimultaneously", kTRUE) ? kButtonDown : kButtonUp;
		gDGFControlData->fSyncClocks = gEnv->GetValue("DGFControl.SynchronizeClocks", kTRUE) ? kButtonDown : kButtonUp;
		gDGFControlData->fIndivSwitchBusTerm = gEnv->GetValue("DGFControl.TerminateSwitchBusIndividually", kFALSE) ? kButtonDown : kButtonUp;
	}
	fDGFFrame->SetState(DGFControlData::kDGFSimulStartStop, gDGFControlData->fSimulStartStop ? kButtonDown : kButtonUp);
	fDGFFrame->SetState(DGFControlData::kDGFSyncClocks, gDGFControlData->fSyncClocks ? kButtonDown : kButtonUp);
	fDGFFrame->SetState(DGFControlData::kDGFIndivSwitchBusTerm, gDGFControlData->fIndivSwitchBusTerm ? kButtonDown : kButtonUp);

//	CAMAC defs
	TGLayoutHints * camacLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 10, 1);
	HEAP(camacLayout);
	fCAMACFrame = new TGGroupFrame(this, "CAMAC", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCAMACFrame);
	this->AddFrame(fCAMACFrame, camacLayout);

	TGLayoutHints * hFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 1);
	frameGC->SetLH(hFrameLayout);
	HEAP(hFrameLayout);
	TGLayoutHints * hLabelLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 1);
	labelGC->SetLH(hLabelLayout);
	HEAP(hLabelLayout);
	TGLayoutHints * hEntryLayout = new TGLayoutHints(kLHintsRight, 5, 1, 3, 1);
	entryGC->SetLH(hEntryLayout);
	HEAP(hEntryLayout);
	fCAMACHostEntry = new TGMrbLabelEntry(fCAMACFrame, "CAMAC Host",
																10, kDGFSetupCamacHost,
																DGFSetupPanel::kLEWidth,
																DGFSetupPanel::kLEHeight,
																DGFSetupPanel::kEntryWidth,
																frameGC, labelGC, entryGC, labelGC);
	fCAMACFrame->AddFrame(fCAMACHostEntry, frameGC->LH());
	camacHost = gDGFControlData->fCAMACHost.Data();
	if (camacHost.Length() == 0) env.Find(camacHost, "DGFControl:TMrbDGF:TMrbEsone", "HostName", "ppc-0");
	fCAMACHostEntry->GetEntry()->SetText(camacHost.Data());
	fCAMACHostEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeCharInt);
	fCAMACHostEntry->SetRange(0, DGFSetupPanel::kNofPPCs - 1);
	gDGFControlData->fCAMACHost = camacHost;
	fCAMACHostEntry->AddToFocusList(&fFocusList);

// download codes
	fCodeFrame = new TGGroupFrame(this, "Download", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCodeFrame);
	this->AddFrame(fCodeFrame, groupGC->LH());
	
	fCodes = new TGMrbCheckButtonList(fCodeFrame, NULL, &fSetupDGFCodes, 1,
											DGFSetupPanel::kFrameWidth, DGFSetupPanel::kLEHeight,
											frameGC, labelGC, buttonGC, lofSpecialButtons);
	HEAP(fCodes);
	fCodeFrame->AddFrame(fCodes, groupGC->LH());
	fCodes->SetState(0xffffffff, kButtonDown);

	fBroadCast = new TGMrbRadioButtonList(fCodeFrame, NULL, &fSetupBroadCast, 1, 
											DGFSetupPanel::kFrameWidth, DGFSetupPanel::kLEHeight,
											frameGC, labelGC, buttonGC);
	fCodeFrame->AddFrame(fBroadCast, groupGC->LH());
	fBroadCast->SetState(kDGFSetupRemoteShell, kButtonDown);
	HEAP(fBroadCast);
	esoneCold = new TMrbEsone();
	Bool_t hasBroadCast = esoneCold->HasBroadCast();
	delete esoneCold;
	if (!hasBroadCast) {
		fBroadCast->SetState(DGFSetupPanel::kDGFSetupBroadCast, kButtonDisabled);
		fBroadCast->SetState(DGFSetupPanel::kDGFSetupRemoteShell, kButtonDisabled);
	}
	
// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	this->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), 1, 
							DGFSetupPanel::kFrameWidth, DGFSetupPanel::kLEHeight,
							frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], buttonGC->LH());
		fCluster[cl]->SetState(~gDGFControlData->GetPatEnabled(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatEnabled(cl), kButtonDown);
	}

//	TGLayoutHints * sFrameLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
//	frameGC->SetLH(sFrameLayout);
//	HEAP(sFrameLayout);
//	TGLayoutHints * sButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
//	buttonGC->SetLH(sButtonLayout);
//	HEAP(sButtonLayout);
	
	fSelectFrame = new TGHorizontalFrame(fModules, DGFSetupPanel::kFrameWidth, DGFSetupPanel::kFrameHeight,
													kChildFrame, frameGC->BG());
	HEAP(fSelectFrame);
	fModules->AddFrame(fSelectFrame, frameGC->LH());
	
	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TGMrbPictureButtonList(fSelectFrame,  NULL, &gSelect[i], 1, 
							DGFSetupPanel::kFrameWidth, DGFSetupPanel::kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fSelectFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		fGroupSelect[i]->Associate(this);
	}
	fAllSelect = new TGMrbPictureButtonList(fSelectFrame,  NULL, &allSelect, 1, 
							DGFSetupPanel::kFrameWidth, DGFSetupPanel::kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fSelectFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	frameGC->LH()->GetPadLeft(),
																			frameGC->LH()->GetPadRight(),
																			frameGC->LH()->GetPadTop(),
																			frameGC->LH()->GetPadBottom()));
	fAllSelect->Associate(this);

// Connect buttons
	fActionFrame = new TGMrbTextButtonGroup(this, "Actions", &fSetupConnect, 1, groupGC, labelGC);
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, groupGC->LH());
	fActionFrame->JustifyButton(kTextCenterX);
	fActionFrame->Associate(this);

	this->ChangeBackground(gDGFControlData->fColorBlue);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionClose);
	
	Window_t wdum;
	Int_t ax, ay;
	gVirtualX->TranslateCoordinates(MainFrame->GetId(), this->GetParent()->GetId(),
								(((TGFrame *) MainFrame)->GetWidth() + 10), 0,
								ax, ay, wdum);
	Move(ax, ay);

	SetWindowName("DGFControl: SetupPanel");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

	MapWindow();
//	gClient->WaitFor(this);

	firstCall = kFALSE;
}

Bool_t DGFSetupPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFSetupPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFSetupModuleSelectColumn) {
						switch (Param1) {
							case kDGFSetupConnectConnect:
								if (this->StartDGFs(kDGFSetupConnectConnect)) {
									gDGFControlData->fStatus |= fDGFFrame->GetActive();
									this->CloseWindow();
								}
								break;
							case kDGFSetupConnectReloadDGFs:
								if (this->StartDGFs(kDGFSetupConnectReloadDGFs)) {
									gDGFControlData->fStatus |= fDGFFrame->GetActive();
								}
								break;
							case kDGFSetupConnectAbortBusySync:
								if (this->AbortDGFs()) {
									gDGFControlData->fStatus |= fDGFFrame->GetActive();
								}
								break;
							case kDGFSetupConnectRestartEsone:
								if (this->StartDGFs(kDGFSetupConnectRestartEsone)) {
									gDGFControlData->fStatus |= fDGFFrame->GetActive();
								}
								break;
							case kDGFSetupConnectAbortEsone:
								if (esoneCold) esoneCold->Abort();
								break;
							case kDGFSetupConnectClose:
								gDGFControlData->fStatus |= fDGFFrame->GetActive();
								this->CloseWindow();
								break;
							case kDGFSetupModuleSelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
									fCluster[cl]->SetState(gDGFControlData->GetPatEnabled(cl), kButtonDown);
								}
								break;
							case kDGFSetupModuleSelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
									fCluster[cl]->SetState(gDGFControlData->GetPatEnabled(cl), kButtonUp);
								}
								break;							
							default:	break;
						}
					} else {
						Param1 -= kDGFSetupModuleSelectColumn;
						Bool_t select = ((Param1 & 1) == 0);
						UInt_t bit = 0x1 << (Param1 >> 1);
						for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
							if (gDGFControlData->GetPatEnabled(cl) & bit) {
								if (select) fCluster[cl]->SetState(bit, kButtonDown);
								else		fCluster[cl]->SetState(bit, kButtonUp);
							}
						}
					}
					break;
				default:	break;
			}
			break;

		case kC_KEY:
			switch (Param1) {
				case TGMrbLofKeyBindings::kGMrbKeyActionClose:
					this->CloseWindow();
					break;
			}
			break;
	}
	return(kTRUE);
}

Bool_t DGFSetupPanel::StartDGFs(EDGFSetupCmdId Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::StartDGFs
// Purpose:        Initialize DGF modules
// Arguments:      EDGFSetupCmdId Mode    -- connect only, warm / cold start?
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    (1) if cold start restart MBS and the ESONE server,
//                 (2) connect to it
//                 (3) if warm or cold start download FPGA and DSP codes
//                 (4) initialize DGF modules
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * dgfModule;
	TMrbDGF * dgf;
	Int_t nerr;
	Int_t broadCast;
	Bool_t offlineMode;
	TMrbLofDGFs lofDGFs[kNofCrates];
	UInt_t toBeLoaded;
						
	TString camacHost;

	TMrbNamedX * clID;

	TObjArray errLog;
	Int_t errCnt;

	Bool_t systemFPGAok;
	Bool_t fippiFPGAok;
	Bool_t DSPok;
	
	Int_t cl;

	offlineMode = gDGFControlData->IsOffline();
	
	errCnt = gMrbLog->GetErrors(errLog);

	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "StartDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	esoneCold = new TMrbEsone(offlineMode);
	if (esoneCold->IsZombie()) {
		gMrbLog->Err()	<< "ESONE not running" << endl;
		gMrbLog->Flush(this->ClassName(), "StartDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "ESONE not running", kMBIconStop);
		delete esoneCold;
		esoneCold = NULL;
		return(kFALSE);
	}
	
	camacHost = fCAMACHostEntry->GetEntry()->GetText();
	gDGFControlData->fCAMACHost = camacHost;

	toBeLoaded = fCodes->GetActive();
	if (Mode == kDGFSetupConnectReloadDGFs) {
		if ((toBeLoaded & (kDGFSetupCodeSystemFPGA | kDGFSetupCodeFippiFPGA | kDGFSetupCodeDSP)) == 0) {
			new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DOWNLOAD option", kMBIconStop);
			return(kFALSE);
		}

		systemFPGAok = kFALSE;
		fippiFPGAok = kFALSE;
		DSPok = kFALSE;
		if (toBeLoaded & kDGFSetupCodeSystemFPGA) {
			systemFPGAok = kFALSE;
			fippiFPGAok = kFALSE;
			DSPok = kFALSE;
		} else {
			systemFPGAok = kTRUE;
		}
		if (systemFPGAok) {
			if (toBeLoaded & kDGFSetupCodeFippiFPGA) {
				fippiFPGAok = kFALSE;
				DSPok = kFALSE;
			} else {
				fippiFPGAok = kTRUE;
			}
		}
		if (systemFPGAok && fippiFPGAok) {
			if (toBeLoaded & kDGFSetupCodeDSP)	DSPok = kFALSE;
			else								DSPok = kTRUE;
		}
	} else {
		systemFPGAok = kTRUE;
		fippiFPGAok = kTRUE;
		DSPok = kTRUE;
	}
	
	nerr = 0;
	if (Mode == kDGFSetupConnectRestartEsone) {
		if (!offlineMode) {
			cout	<< setblue
				<< "[DGFSetupPanel::StartDGFs(): Cold start for ESONE server / MBS @ " << camacHost << "]"
				<< setblack << endl;
			if (!esoneCold->StartServer(camacHost.Data())) {
				gMrbLog->Err()	<< "Restarting ESONE server failed" << endl;
				gMrbLog->Flush(this->ClassName(), "StartDGFs");
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Restarting ESONE server failed", kMBIconStop);
				delete esoneCold;
				esoneCold = NULL;
				return(kFALSE);
			} else {
				dgfModule = gDGFControlData->FirstModule();
				while (dgfModule) {
					dgf = dgfModule->GetAddr();
					if (dgf) delete dgf;
					dgfModule->SetAddr(NULL);
					dgfModule = gDGFControlData->NextModule(dgfModule);
				}
			}
		}
		return(kTRUE);
	}

	esoneCold->SetVerboseMode((gDGFControlData->fStatus & DGFControlData::kDGFDebugMode) != 0);
		
	UInt_t selFlag = 0;
	for (cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		gDGFControlData->SetPatInUse(cl, fCluster[cl]->GetActive());
		selFlag |= fCluster[cl]->GetActive();
	}
	if (selFlag == 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}

	if (nerr == 0) {
		esoneCold->UseBroadCast(fBroadCast->GetActive() == kDGFSetupBroadCast);
		broadCast = esoneCold->HasBroadCast();
		if (broadCast && !offlineMode) {
			gMrbLog->Out() << "DGFSetupPanel::StartDGFs(): Using broadcast mode" << endl;
			gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
			for (Int_t cr = 0; cr < kNofCrates; cr++) lofDGFs[cr].Delete();
			dgfModule = gDGFControlData->FirstModule();
			while (dgfModule) {
				if (gDGFControlData->ModuleInUse(dgfModule)) {
					dgf = dgfModule->GetAddr();
					if (dgf == NULL) {
						dgf = new TMrbDGF(dgfModule->GetName(), camacHost.Data(),
											dgfModule->GetCrate(), dgfModule->GetStation(),
											kTRUE, offlineMode);
						dgfModule->SetHost(camacHost.Data());
						dgfModule->SetAddr(dgf);
						clID = dgfModule->GetClusterID();
						dgf->SetClusterID(clID->GetIndex(), clID->GetName(), clID->GetTitle());
						if (dgf->IsZombie()) dgfModule->SetActive(kFALSE);
					}
					if (!dgf->IsZombie() && dgf->IsConnected()) {
						dgf->Camac()->UseBroadCast(kTRUE);
						lofDGFs[dgf->GetCrate() - 1].AddModule(dgf);
						dgf->SetVerboseMode((gDGFControlData->fStatus & DGFControlData::kDGFVerboseMode) != 0);
						dgfModule->SetTitle(dgf->GetTitle());
						dgfModule->SetAddr(dgf);
						if (dgf->Data()->ReadNameTable(gDGFControlData->fDSPParamsFile) <= 0) nerr++;
						if (dgf->Data()->ReadFPGACode(TMrbDGFData::kSystemFPGA, gDGFControlData->fSystemFPGAConfigFile) <= 0) nerr++;
						if (dgf->Data()->ReadFPGACode(TMrbDGFData::kFippiFPGA, gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevD], TMrbDGFData::kRevD) <= 0) nerr++;
						if (dgf->Data()->ReadFPGACode(TMrbDGFData::kFippiFPGA, gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevE], TMrbDGFData::kRevE) <= 0) nerr++;
						if (dgf->Data()->ReadDSPCode(gDGFControlData->fDSPCodeFile) <= 0) nerr++;
					} else nerr++;
				}
				dgfModule = gDGFControlData->NextModule(dgfModule);
			}

			for (Int_t cr = 0; cr < kNofCrates; cr++) {
				if (lofDGFs[cr].GetLast() >= 0) {
					if (toBeLoaded & kDGFSetupCodeSystemFPGA) {
						gMrbLog->Out()	<< "Downloading System FPGA for modules in C"
										<< (cr + 1) << " (broadcast pattern = 0x"
										<< setbase(16) << lofDGFs[cr].GetStationMask() << setbase(10) << ")" << endl;
						gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
						if (lofDGFs[cr].DownloadFPGACode(TMrbDGFData::kSystemFPGA)) {
							systemFPGAok = kTRUE;
						} else {
							systemFPGAok = kFALSE;
							nerr++;
						}
					}
					if (toBeLoaded & kDGFSetupCodeFippiFPGA) {
						gMrbLog->Out()	<< "Downloading Fippi FPGA for modules in C"
										<< (cr + 1) << " (broadcast pattern = 0x"
										<< setbase(16) << lofDGFs[cr].GetStationMask() << setbase(10) << ")" << endl;
						gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
						if (lofDGFs[cr].DownloadFPGACode(TMrbDGFData::kFippiFPGA)) {
							fippiFPGAok = kTRUE;
						} else {
							fippiFPGAok = kFALSE;
							nerr++;
						}
					}
				}
			}

			if (systemFPGAok && fippiFPGAok) {
				dgfModule = gDGFControlData->FirstModule();
				while (dgfModule) {
					if (gDGFControlData->ModuleInUse(dgfModule)) {
						dgf = dgfModule->GetAddr();
						if (dgf->IsConnected()) {
							if (Mode == kDGFSetupConnectReloadDGFs) {
								if (!dgf->SetSwitchBusDefault(gDGFControlData->fIndivSwitchBusTerm, "DGFControl")) nerr++;
							}
						}
					}
					dgfModule = gDGFControlData->NextModule(dgfModule);
				}
			}

			for (Int_t cr = 0; cr < kNofCrates; cr++) {
				if (lofDGFs[cr].GetLast() >= 0) {
					if (toBeLoaded & kDGFSetupCodeDSP) {
						gMrbLog->Out()	<< "Downloading DSP for modules in C"
										<< (cr + 1) << " (broadcast pattern = 0x"
										<< setbase(16) << lofDGFs[cr].GetStationMask() << setbase(10) << ")" << endl;
						gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
						if (lofDGFs[cr].DownloadDSPCode()) {
							DSPok = kTRUE;
						} else {
							DSPok = kFALSE;
							nerr++;
						}
					}
				}
			}

			dgfModule = gDGFControlData->FirstModule();
			while (dgfModule) {
				if (gDGFControlData->ModuleInUse(dgfModule)) {
					dgf = dgfModule->GetAddr();
					if (dgf->IsConnected()) {
						if (systemFPGAok && fippiFPGAok && DSPok) {
							if(!dgf->ReadParamMemory(kTRUE, kTRUE)) nerr++;
							dgf->ClearChannelMask();
							dgf->ClearTriggerMask();
							gDGFControlData->fDeltaT = dgf->GetDeltaT();
							Bool_t synchWait = ((fDGFFrame->GetActive() & DGFControlData::kDGFSimulStartStop) != 0);
							dgf->SetSynchWait(synchWait, kTRUE);
							Bool_t inSynch = ((fDGFFrame->GetActive() & DGFControlData::kDGFSyncClocks) != 0);
							dgf->SetInSynch(inSynch, kTRUE);
							gROOT->Append(dgf);
						}
					} else nerr++;
				}
				dgfModule = gDGFControlData->NextModule(dgfModule);
			}
		} else {
			dgfModule = gDGFControlData->FirstModule();
			while (dgfModule) {
				if (gDGFControlData->ModuleInUse(dgfModule)) {
					dgf = dgfModule->GetAddr();
					if (dgf == NULL) {
						dgf = new TMrbDGF(dgfModule->GetName(), camacHost.Data(),
											dgfModule->GetCrate(), dgfModule->GetStation(),
											kTRUE, offlineMode);
						dgfModule->SetHost(camacHost.Data());
						dgfModule->SetAddr(dgf);
						clID = dgfModule->GetClusterID();
						dgf->SetClusterID(clID->GetIndex(), clID->GetName(), clID->GetTitle());
						if (dgf->IsZombie()) dgfModule->SetActive(kFALSE);
					}
					if (!dgf->IsZombie() && dgf->IsConnected()) {
						dgf->SetVerboseMode((gDGFControlData->fStatus & DGFControlData::kDGFVerboseMode) != 0);
						dgfModule->SetTitle(dgf->GetTitle());
						if (dgf->Data()->ReadNameTable(gDGFControlData->fDSPParamsFile) <= 0) nerr++;
						if (Mode == kDGFSetupConnectReloadDGFs) {
							if (!offlineMode) {
								if (toBeLoaded & kDGFSetupCodeSystemFPGA) {
									if (dgf->Data()->ReadFPGACode(TMrbDGFData::kSystemFPGA, gDGFControlData->fSystemFPGAConfigFile) <= 0) nerr++;
									if (dgf->DownloadFPGACode(TMrbDGFData::kSystemFPGA)) {
										systemFPGAok = kTRUE;
										if (!dgf->Data()->IsVerbose()) {
											gMrbLog->Out()	<< "[System FPGA] " << dgf->GetName()
																<< " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
																<< ": Download successful" << endl;
											gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
										}
									} else {
										systemFPGAok = kFALSE;
										nerr++;
									}
								}
							}
						}
					} else nerr++;
				}
				dgfModule = gDGFControlData->NextModule(dgfModule);
			}

			dgfModule = gDGFControlData->FirstModule();
			while (dgfModule) {
				if (gDGFControlData->ModuleInUse(dgfModule)) {
					dgf = dgfModule->GetAddr();
					if (dgf->IsConnected()) {
						if (Mode == kDGFSetupConnectReloadDGFs) {
							if (!offlineMode) {
								if (systemFPGAok && toBeLoaded & kDGFSetupCodeFippiFPGA) {
									TMrbDGFData::EMrbDGFRevision rev = (TMrbDGFData::EMrbDGFRevision) dgf->GetRevision()->GetIndex();
									if (dgf->Data()->ReadFPGACode(TMrbDGFData::kFippiFPGA, gDGFControlData->fFippiFPGAConfigFile[rev], rev) <= 0) nerr++;
									if (dgf->DownloadFPGACode(TMrbDGFData::kFippiFPGA)) {
										fippiFPGAok = kTRUE;
										if (!dgf->Data()->IsVerbose()) {
											gMrbLog->Out()	<< "[Fippi FPGA] " << dgf->GetName()
																<< " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
																<< ": Download successful" << endl;
											gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
										}
									} else {
										fippiFPGAok = kFALSE;
										nerr++;
									}
								}

								if (systemFPGAok && fippiFPGAok) {
									if (!dgf->SetSwitchBusDefault(gDGFControlData->fIndivSwitchBusTerm, "DGFControl")) nerr++;
									if (toBeLoaded & kDGFSetupCodeDSP) {
										if (dgf->Data()->ReadDSPCode(gDGFControlData->fDSPCodeFile) <= 0) nerr++;
										if (dgf->DownloadDSPCode()) {
											DSPok = kTRUE;
											if (!dgf->Data()->IsVerbose()) {
												gMrbLog->Out()	<< dgf->GetName()
																	<< " C" << dgf->GetCrate() << ".N" << dgf->GetStation()
																	<< ": DSP download successful" << endl;
												gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
											}
										} else {
											DSPok = kFALSE;
											nerr++;
										}
									}
								}
							}
						}
						if (!offlineMode) {
							if (systemFPGAok && fippiFPGAok && DSPok) {
								if(!dgf->ReadParamMemory(kTRUE, kTRUE)) nerr++;
								gDGFControlData->fDeltaT = dgf->GetDeltaT();
								Bool_t synchWait = ((fDGFFrame->GetActive() & DGFControlData::kDGFSimulStartStop) != 0);
								dgf->SetSynchWait(synchWait, kTRUE);
								Bool_t inSynch = ((fDGFFrame->GetActive() & DGFControlData::kDGFSyncClocks) != 0);
								dgf->SetInSynch(inSynch, kTRUE);
							}
						}
						gROOT->Append(dgf);
					} else nerr++;
					if (offlineMode) {
						cout	<< setgreen
								<< "[DGFSetupPanel::StartDGFs(): Running " << dgf->GetTitle()
								<< " in OFFLINE mode" << setblack << endl;
					}
				}
				dgfModule = gDGFControlData->NextModule(dgfModule);
			}
		}
	}

	if (nerr > 0 || gMrbLog->GetErrors(errLog) > errCnt) {
		gMrbLog->Err()	<< "Initialization of DGF modules failed" << endl;
		gMrbLog->Flush(this->ClassName(), "StartDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Initialization of DGF modules failed", kMBIconStop);
		return(kFALSE);
	} else if (Mode == kDGFSetupConnectReloadDGFs) {
		gMrbLog->Out()	<< "DGF download ok" << endl;
		gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
		return(kTRUE);
	} else {
		gMrbLog->Out()	<< "DGF module(s) connected" << endl;
		gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
		return(kTRUE);
	}
	return(kFALSE);
}

Bool_t DGFSetupPanel::AbortDGFs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::AbortDGFs
// Purpose:        Initialize DGF modules
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Force DGFs to return from infinite busy-sync loop
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * dgfModule;
	TMrbDGF * dgf;
	Int_t nerr;
	Bool_t offlineMode;
	Bool_t isAborted;
						
	offlineMode = gDGFControlData->IsOffline();
	
	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "StartDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	nerr = 0;
	dgfModule = gDGFControlData->FirstModule();
	Int_t nofModules = 0;
	isAborted = kFALSE;
	cout << endl << "Wait " << flush;
	while (dgfModule) {
		Int_t cl = nofModules / kNofModulesPerCluster;
		Int_t modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() == (UInt_t) gDGFControlData->ModuleIndex(cl, modNo)) && dgfModule->IsActive()) {
			dgf = dgfModule->GetAddr();
			isAborted = kTRUE;
			dgf->SetParValue("SYNCHDONE", 1, kTRUE);
			if (dgf->StopRun()) {
				cout << "." << flush;
			} else {
				nerr++;
			}
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}
	cout << " done" << endl;
	if (nerr > 0) {
		gMrbLog->Err()	<< "Aborting busy-sync loop failed" << endl;
		gMrbLog->Flush(this->ClassName(), "StartDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Aborting busy-sync loop failed", kMBIconStop);
		return(kFALSE);
	} else if (isAborted) {
		gMrbLog->Out()	<< "Busy-sync loop terminated properly" << endl;
		gMrbLog->Flush(this->ClassName(), "StartDGFs", setblue);
		return(kTRUE);
	} else {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
}
