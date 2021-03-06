//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFSetupPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFSetupPanel.cxx,v 1.42 2011-02-11 08:06:08 Marabou Exp $
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
#include "TMrbSystem.h"
#include "TGMrbProgressBar.h"

#include "TGMsgBox.h"

#include "DGFControlData.h"
#include "DGFSetupPanel.h"

#include "SetColor.h"

const SMrbNamedXShort kDGFSetupDGFModes[] =
							{
								{DGFControlData::kDGFSimulStartStop,		"Simultaneously start/stop modules" },
								{DGFControlData::kDGFSyncClocks,			"Synchronize clocks with run"		},
								{DGFControlData::kDGFIndivSwitchBusTerm,	"Terminate switchbus individually"	},
								{DGFControlData::kDGFUserPSA,				"Activate user PSA code"			},
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
				{DGFSetupPanel::kDGFSetupConnectToEsone,		"Connect",		"Connect to DGF modules w/o downloading code" },
				{DGFSetupPanel::kDGFSetupReloadDGFs,			"Reload DGFs",	"Download FPGA and DSP code"			},
				{DGFSetupPanel::kDGFSetupAbortBusySync,			"Abort Busy-Sync","Force return from busy-sync-loop"			},
				{DGFSetupPanel::kDGFSetupRestartEsone,			"Restart ESONE","Restart ESONE server"			},
				{DGFSetupPanel::kDGFSetupAbortEsone, 			"Abort ESONE Restart","Abort ESONE restart procedure"			},
				{0, 											NULL,			NULL							}
			};

TMrbEsone * esoneCold = NULL;

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

ClassImp(DGFSetupPanel)

DGFSetupPanel::DGFSetupPanel(TGCompositeFrame * TabFrame) :
					TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel
// Purpose:        DGF Viewer: Setup Panel
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
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
	TMrbLofNamedX colSelect[kNofModulesPerCluster];
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

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));

//	create buttons to select/deselect groups of modules
	Int_t idx = kDGFSetupModuleSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		colSelect[i].Delete();							// (de)select columns
		colSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		colSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
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

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// DGF defs
	TGLayoutHints * dgfButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 5, 1, 10, 1);
	labelGC->SetLH(dgfButtonLayout);
	HEAP(dgfButtonLayout);
	fDGFFrame = new TGMrbCheckButtonGroup(this, "DGF General", &fSetupDGFModes, -1, 1, groupGC, labelGC, NULL, kVerticalFrame);
	HEAP(fDGFFrame);
	this->AddFrame(fDGFFrame, groupGC->LH());
//	fDGFFrame->SetState(gDGFControlData->fStatus);

	fDGFFrame->SetState(DGFControlData::kDGFSimulStartStop, gDGFControlData->fSimulStartStop ? kButtonDown : kButtonUp);
	fDGFFrame->SetState(DGFControlData::kDGFSyncClocks, gDGFControlData->fSyncClocks ? kButtonDown : kButtonUp);
	fDGFFrame->SetState(DGFControlData::kDGFIndivSwitchBusTerm, gDGFControlData->fIndivSwitchBusTerm ? kButtonDown : kButtonUp);
	fDGFFrame->SetState(DGFControlData::kDGFUserPSA, gDGFControlData->fUserPSA ? kButtonDown : kButtonUp);

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
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, labelGC);
	fCAMACFrame->AddFrame(fCAMACHostEntry, frameGC->LH());
	camacHost = gDGFControlData->fCAMACHost.Data();
#ifdef PPC_NEW_ADDRESS
	if (camacHost.Length() == 0) env.Find(camacHost, "DGFControl:TMrbDGF:TMrbEsone", "HostName", "gar-ex-ppc01");
#else
	if (camacHost.Length() == 0) env.Find(camacHost, "DGFControl:TMrbDGF:TMrbEsone", "HostName", "ppc-0");
#endif
	fCAMACHostEntry->SetText(camacHost.Data());
	fCAMACHostEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeCharInt);
	fCAMACHostEntry->SetRange(0, DGFSetupPanel::kNofPPCs - 1);
	gDGFControlData->fCAMACHost = camacHost;
	fCAMACHostEntry->AddToFocusList(&fFocusList);

// download codes
	fCodeFrame = new TGGroupFrame(this, "Download", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCodeFrame);
	this->AddFrame(fCodeFrame, groupGC->LH());

	fCodes = new TGMrbCheckButtonList(fCodeFrame, NULL, &fSetupDGFCodes, -1, 1,
											TabFrame->GetWidth(), kLEHeight,
											frameGC, labelGC, buttonGC, lofSpecialButtons);
	HEAP(fCodes);
	fCodeFrame->AddFrame(fCodes, groupGC->LH());
	fCodes->SetState(0xffffffff, kButtonDown);


// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	this->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), -1, 1,
							TabFrame->GetWidth(), kLEHeight,
							frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], buttonGC->LH());
		fCluster[cl]->SetState(~gDGFControlData->GetPatEnabled(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatEnabled(cl), kButtonDown);
	}


	fSelectFrame = new TGHorizontalFrame(fModules, TabFrame->GetWidth(), TabFrame->GetHeight(), kChildFrame, frameGC->BG());
	HEAP(fSelectFrame);
	fModules->AddFrame(fSelectFrame, frameGC->LH());

	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fColSelect[i] = new TGMrbPictureButtonList(fSelectFrame,  NULL, &colSelect[i], -1, 1,
							TabFrame->GetWidth(), kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fColSelect[i]);
		fSelectFrame->AddFrame(fColSelect[i], frameGC->LH());
		((TGMrbButtonFrame *) fColSelect[i])->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");
	}
	fAllSelect = new TGMrbPictureButtonList(fSelectFrame,  NULL, &allSelect, -1, 1,
							TabFrame->GetWidth(), kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fSelectFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	frameGC->LH()->GetPadLeft(),
																			frameGC->LH()->GetPadRight(),
																			frameGC->LH()->GetPadTop(),
																			frameGC->LH()->GetPadBottom()));
	((TGMrbButtonFrame *) fAllSelect)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");

// Connect buttons
	fActionFrame = new TGMrbTextButtonGroup(this, "Actions", &fSetupConnect, -1, 1, groupGC, labelGC);
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, groupGC->LH());
	fActionFrame->JustifyButton(kTextCenterX);
	((TGMrbButtonFrame *) fActionFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gDGFControlData->fColorBlue);

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);

	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());

	MapSubwindows();
	MapWindow();
}

void DGFSetupPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::SelectModule
// Purpose:        Slot method: select module(s)
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFSetupModuleSelectColumn) {
		switch (Selection) {
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
		}
	} else {
		Selection -= kDGFSetupModuleSelectColumn;
		Bool_t select = ((Selection & 1) == 0);
		UInt_t bit = 0x1 << (Selection >> 1);
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			if (gDGFControlData->GetPatEnabled(cl) & bit) {
				UInt_t act = fCluster[cl]->GetActive();
				UInt_t down = select ? (act | bit) : (act & ~bit);
				fCluster[cl]->SetState(down & 0xFFFF, kButtonDown);
			}
		}
	}
}

void DGFSetupPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFSetupConnectToEsone:
			if (this->ConnectToEsone()) {
				this->SetupGeneral();
				gDGFControlData->fStatus |= fDGFFrame->GetActive();
			}
			break;
		case kDGFSetupReloadDGFs:
			if (this->ReloadDGFs()) {
				this->SetupGeneral();
				gDGFControlData->fStatus |= fDGFFrame->GetActive();
			}
			break;
		case kDGFSetupAbortBusySync:
			if (this->AbortDGFs()) {
				gDGFControlData->fStatus |= fDGFFrame->GetActive();
			}
			break;
		case kDGFSetupRestartEsone:
			if (this->RestartEsone()) {
				gDGFControlData->fStatus |= fDGFFrame->GetActive();
			}
			break;
		case kDGFSetupAbortEsone:
			if (esoneCold) esoneCold->Abort();
			break;
	}
}

Bool_t DGFSetupPanel::ConnectToEsone() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::ConnectToEsone
// Purpose:        Connect modules to esone server
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Connects to esone server
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray errLog;
	TMrbLofDGFs lofDGFs[kNofCrates];

	Bool_t offlineMode = gDGFControlData->IsOffline();

	Int_t errCnt = gMrbLog->GetErrors(errLog);

	if (this->DaqIsRunning()) {
		gMrbLog->Err()	<< "DAQ seems to be running - can't access DGF modules" << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToEsone");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DAQ seems to be running", kMBIconStop);
		return(kFALSE);
	}

	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToEsone");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	if (esoneCold == NULL) {
		esoneCold = new TMrbEsone(offlineMode);
		if (esoneCold->IsZombie()) {
			gMrbLog->Err()	<< "ESONE not running" << endl;
			gMrbLog->Flush(this->ClassName(), "ConnectToEsone");
			new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "ESONE not running", kMBIconStop);
			delete esoneCold;
			esoneCold = NULL;
			return(kFALSE);
		}
	}

	TString camacHost = fCAMACHostEntry->GetText();
	gDGFControlData->fCAMACHost = camacHost;

	esoneCold->SetVerboseMode(gDGFControlData->IsDebug());
	esoneCold->SetSingleStep(gDGFControlData->IsSingleStep());

	DGFModule * dgfModule = gDGFControlData->FirstModule();
	while (dgfModule) {
		TMrbDGF * dgf = dgfModule->GetAddr();
		if (dgf) delete dgf;
		dgfModule->SetAddr(NULL);
		dgfModule = gDGFControlData->NextModule(dgfModule);
	}

	Int_t nerr = 0;
	UInt_t selFlag = 0;

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		gDGFControlData->SetPatInUse(cl, fCluster[cl]->GetActive());
		selFlag |= fCluster[cl]->GetActive();
	}
	if (selFlag == 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}

	if (nerr == 0) {
		esoneCold->UseBroadCast(kTRUE);
		Bool_t broadCast = esoneCold->HasBroadCast();
		if (!broadCast) {
			gMrbLog->Err()	<< "Need broadcast mode - controller isn't capable of" << endl;
			gMrbLog->Flush(this->ClassName(), "ConnectToEsone");
			new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Camac controller doesn't have BROADCAST mode", kMBIconStop);
			return(kFALSE);
		}
		if (!offlineMode) {
			for (Int_t cr = 0; cr < kNofCrates; cr++) lofDGFs[cr].Delete();
			DGFModule * dgfModule = gDGFControlData->FirstModule();
			TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, "Connecting ...", 400, "blue", NULL, kTRUE);
			pgb->SetRange(0, gDGFControlData->GetNofModules());
			Bool_t upsaLoaded = kFALSE;
			while (dgfModule) {
				if (gDGFControlData->ModuleInUse(dgfModule)) {
					TMrbDGF * dgf = dgfModule->GetAddr();
					if (dgf == NULL) {
						dgf = new TMrbDGF(dgfModule->GetName(), camacHost.Data(),
											dgfModule->GetCrate(), dgfModule->GetStation(),
											kTRUE, offlineMode);
						dgfModule->SetHost(camacHost.Data());
						dgfModule->SetAddr(dgf);
						dgf->SetClusterID(	dgfModule->GetClusterSerial(),
											dgfModule->GetClusterColor(),
											dgfModule->GetClusterSegments(),
											dgfModule->GetClusterHexNum());
						if (dgf->IsZombie()) dgfModule->SetActive(kFALSE);
					}
					if (!dgf->IsZombie() && dgf->IsConnected()) {
						dgf->Camac()->UseBroadCast(kTRUE);
						lofDGFs[dgf->GetCrate() - 1].AddModule(dgf);
						dgf->SetVerboseMode(gDGFControlData->IsVerbose());
						dgfModule->SetTitle(dgf->GetTitle());
						dgfModule->SetAddr(dgf);
						if (dgf->Data()->ReadNameTable(gDGFControlData->fDSPParamsFile) <= 0) nerr++;
						if (!upsaLoaded) {
							if (dgf->Data()->AddToNameTable(gDGFControlData->fUPSAParamsFile, "UserPSA") > 0) upsaLoaded = kTRUE;
						}
						if(!dgf->ReadParamMemory(kTRUE, kTRUE)) nerr++;
						if (!dgf->SetSwitchBusDefault(gDGFControlData->fIndivSwitchBusTerm, "DGFControl", gDGFControlData->Dgfrc()->Env())) nerr++;
						if (!this->TurnUserPSAOnOff(dgfModule, gDGFControlData->fUserPSA)) nerr++;
						gDGFControlData->fDeltaT = dgf->GetDeltaT();
						Bool_t synchWait = ((fDGFFrame->GetActive() & DGFControlData::kDGFSimulStartStop) != 0);
						if (!this->SetSynchWait(dgfModule, synchWait)) nerr++;
						Bool_t inSynch = ((fDGFFrame->GetActive() & DGFControlData::kDGFSyncClocks) != 0);
						if (!this->SetInSynch(dgfModule, inSynch)) nerr++;
						dgf->Camac()->SetVerboseMode(gDGFControlData->IsDebug());
						dgf->Camac()->SetSingleStep(gDGFControlData->IsSingleStep());
						gROOT->Append(dgf);
					} else nerr++;
				}
				pgb->Increment(1, dgfModule->GetName());
				gSystem->ProcessEvents();
				dgfModule = gDGFControlData->NextModule(dgfModule);
			}
//			pgb->DeleteWindow();
			delete pgb;
			gDGFControlData->Dgfrc()->Write();
		}
	}

	if (nerr > 0 || gMrbLog->GetErrors(errLog) > errCnt) {
		gMrbLog->Err()	<< "Connecting DGF module(s) to esone server failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToEsone");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Connecting DGF module(s) to esone server failed", kMBIconStop);
		return(kFALSE);
	} else {
		gMrbLog->Out()	<< "DGF module(s) connected" << endl;
		gMrbLog->Flush(this->ClassName(), "ConnectToEsone", setblue);
		return(kTRUE);
	}
	return(kTRUE);
}

Bool_t DGFSetupPanel::ReloadDGFs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::ReloadDGFs
// Purpose:        Reload DGF code
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Downloads code file to fpgas and dsp.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TObjArray errLog;
	TMrbLofDGFs lofDGFs[kNofCrates];

	Bool_t offlineMode = gDGFControlData->IsOffline();

	Int_t errCnt = gMrbLog->GetErrors(errLog);

	if (this->DaqIsRunning()) {
		gMrbLog->Err()	<< "DAQ seems to be running - can't access DGF modules" << endl;
		gMrbLog->Flush(this->ClassName(), "ReloadDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DAQ seems to be running", kMBIconStop);
		return(kFALSE);
	}

	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "ReloadDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	if (esoneCold == NULL) {
		esoneCold = new TMrbEsone(offlineMode);
		if (esoneCold->IsZombie()) {
			gMrbLog->Err()	<< "ESONE not running" << endl;
			gMrbLog->Flush(this->ClassName(), "ReloadDGFs");
			new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "ESONE not running", kMBIconStop);
			delete esoneCold;
			esoneCold = NULL;
			return(kFALSE);
		}
	}

	TString camacHost = fCAMACHostEntry->GetText();
	gDGFControlData->fCAMACHost = camacHost;

	Int_t hostAddr = esoneCold->ConnectToHost(camacHost.Data(), kTRUE);
	if (hostAddr == 0) {
			gMrbLog->Err()	<< "ESONE not running" << endl;
			gMrbLog->Flush(this->ClassName(), "ReloadDGFs");
			new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "ESONE not running", kMBIconStop);
			delete esoneCold;
			esoneCold = NULL;
			return(kFALSE);
	}

	esoneCold->SetVerboseMode(gDGFControlData->IsDebug());
	esoneCold->SetSingleStep(gDGFControlData->IsSingleStep());

	DGFModule * dgfModule = gDGFControlData->FirstModule();
	while (dgfModule) {
		TMrbDGF * dgf = dgfModule->GetAddr();
		if (dgf) delete dgf;
		dgfModule->SetAddr(NULL);
		dgfModule->SetActive();
		dgfModule = gDGFControlData->NextModule(dgfModule);
	}

	Int_t nerr = 0;
	UInt_t selFlag = 0;

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		gDGFControlData->SetPatInUse(cl, fCluster[cl]->GetActive());
		selFlag |= fCluster[cl]->GetActive();
	}
	if (selFlag == 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}

	if (nerr == 0) {
		esoneCold->UseBroadCast(kTRUE);
		Bool_t broadCast = esoneCold->HasBroadCast();
		if (!broadCast) {
			gMrbLog->Err()	<< "Need broadcast mode - controller isn't capable of" << endl;
			gMrbLog->Flush(this->ClassName(), "ReloadDGFs");
			new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Camac controller doesn't have BROADCAST mode", kMBIconStop);
			return(kFALSE);
		}
		if (!offlineMode) {
			UInt_t toBeLoaded = fCodes->GetActive();
			for (Int_t cr = 0; cr < kNofCrates; cr++) lofDGFs[cr].Delete();
			DGFModule * dgfModule = gDGFControlData->FirstModule();
			Bool_t upsaLoaded = kFALSE;
			while (dgfModule) {
				if (gDGFControlData->ModuleInUse(dgfModule)) {
					TMrbDGF * dgf = dgfModule->GetAddr();
					if (dgf == NULL) {
						dgf = new TMrbDGF(dgfModule->GetName(), camacHost.Data(),
											dgfModule->GetCrate(), dgfModule->GetStation(),
											kTRUE, offlineMode);
						dgfModule->SetHost(camacHost.Data());
						dgfModule->SetAddr(dgf);
						dgf->SetClusterID(	dgfModule->GetClusterSerial(),
											dgfModule->GetClusterColor(),
											dgfModule->GetClusterSegments(),
											dgfModule->GetClusterHexNum());
						if (dgf->IsZombie()) dgfModule->SetActive(kFALSE);
					}
					if (!dgf->IsZombie() && dgf->IsConnected()) {
						dgf->Camac()->UseBroadCast(kTRUE);
						lofDGFs[dgf->GetCrate() - 1].AddModule(dgf);
						dgf->SetVerboseMode(gDGFControlData->IsVerbose());
						dgf->Camac()->SetVerboseMode(gDGFControlData->IsDebug());
						dgf->Camac()->SetSingleStep(gDGFControlData->IsSingleStep());
						dgfModule->SetTitle(dgf->GetTitle());
						dgfModule->SetAddr(dgf);
						if (dgf->Data()->ReadNameTable(gDGFControlData->fDSPParamsFile) <= 0) nerr++;
						if (!upsaLoaded) {
							if (dgf->Data()->AddToNameTable(gDGFControlData->fUPSAParamsFile, "UserPSA") > 0) upsaLoaded = kTRUE;
						}
					} else nerr++;
				}
				dgfModule = gDGFControlData->NextModule(dgfModule);
			}

			if (nerr == 0) {
				TString dlOpt = "";
				if (toBeLoaded & kDGFSetupCodeSystemFPGA)	dlOpt += "S";
				if (toBeLoaded & kDGFSetupCodeFippiFPGA)	dlOpt += "F";
				if (toBeLoaded & kDGFSetupCodeDSP)			dlOpt += "D";
				if (dlOpt.Length() == 0) {
					gMrbLog->Err()	<< "Download option missing - neither SystemFPGA, nor FippiFPGA, nor DSP selected" << endl;
					gMrbLog->Flush(this->ClassName(), "ReloadDGFs");
					new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You didn't select a download option", kMBIconStop);
					return(kFALSE);
				}
				TString errMsg;
				ofstream dlf(".DgfDownload.rc", ios::out);
				dlf << "DownloadOptions:	" << dlOpt << endl;
				TString relPath;
				TMrbSystem ux;
				dlf << "LoadPath: 	" << gDGFControlData->fLoadPath << endl;
				relPath = gDGFControlData->fSystemFPGAConfigFile;
				if (!gDGFControlData->CheckAccess(relPath.Data(), DGFControlData::kDGFAccessRead, errMsg)) nerr++;
				dlf << "SystemFPGACode: 	" << ux.GetRelPath(relPath, gDGFControlData->fLoadPath) << endl;
				relPath = gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevD];
				if (!gDGFControlData->CheckAccess(relPath.Data(), DGFControlData::kDGFAccessRead, errMsg)) nerr++;
				dlf << "FippiFPGACode.RevD: 	" <<  ux.GetRelPath(relPath, gDGFControlData->fLoadPath) << endl;
				relPath = gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevE];
				if (!gDGFControlData->CheckAccess(relPath.Data(), DGFControlData::kDGFAccessRead, errMsg)) nerr++;
				dlf << "FippiFPGACode.RevE: 	" << ux.GetRelPath(relPath, gDGFControlData->fLoadPath) << endl;
				relPath = gDGFControlData->fDSPCodeFile;
				if (!gDGFControlData->CheckAccess(relPath.Data(), DGFControlData::kDGFAccessRead, errMsg)) nerr++;
				dlf << "DspCode:		" << ux.GetRelPath(relPath, gDGFControlData->fLoadPath) << endl;
				Int_t nofCrates = 0;
				for (Int_t cr = 0; cr < kNofCrates; cr++) {
					if (lofDGFs[cr].GetLast() >= 0) {
						nofCrates++;
						dlf << "StationMask.Crate" << cr + 1 << ":			0x"
							<< setbase(16) << lofDGFs[cr].GetStationMask() << setbase(10) << endl;
					}
				}
				dlf << "NofCrates:			" << nofCrates << endl;
				dlf.close();

				if (nerr == 0) {
					TString dlPgm = gEnv->GetValue("TMrbDGF.ProgramToDownLoadCode", "$MARABOU/powerpc/bin/v22/dgfdown");
					gSystem->ExpandPathName(dlPgm);
					gMrbLog->Out()	<< "Calling program \"" << camacHost << ":" << dlPgm << "\" via rsh" << endl;
					gMrbLog->Flush(this->ClassName(), "ReloadDGFs", setblue);

					TString dlPath = gSystem->Getenv("PWD");
					if (dlPath.IsNull()) dlPath = gSystem->WorkingDirectory();

					gSystem->Exec(Form("rsh %s 'cd %s; %s %s'", camacHost.Data(), dlPath.Data(), dlPgm.Data(), ".DgfDownload.rc"));
					for (Int_t i = 0; i < 100; i++) {
						if (!gSystem->AccessPathName(".dgfdown.ok", (EAccessMode) F_OK)) {
							TEnv * e = new TEnv(".dgfdown.ok");
							nerr += e->GetValue("Download.Errors", 0);
							delete e;
							break;
						}
						usleep(1000);
					}
				}

				if (nerr == 0) {

					dgfModule = gDGFControlData->FirstModule();
					while (dgfModule) {
						if (gDGFControlData->ModuleInUse(dgfModule)) {
							TMrbDGF * dgf = dgfModule->GetAddr();
							if (dgf->IsConnected()) {
								if (!dgf->ReadParamMemory(kTRUE, kTRUE)) nerr++;
								if (!dgf->SetSwitchBusDefault(gDGFControlData->fIndivSwitchBusTerm, "DGFControl", gDGFControlData->Dgfrc()->Env())) nerr++;
								if (!this->TurnUserPSAOnOff(dgfModule, gDGFControlData->fUserPSA)) nerr++;
								gDGFControlData->fDeltaT = dgf->GetDeltaT();
								Bool_t synchWait = ((fDGFFrame->GetActive() & DGFControlData::kDGFSimulStartStop) != 0);
								if (!this->SetSynchWait(dgfModule, synchWait)) nerr++;
								Bool_t inSynch = ((fDGFFrame->GetActive() & DGFControlData::kDGFSyncClocks) != 0);
								if (!this->SetInSynch(dgfModule, inSynch)) nerr++;
								gROOT->Append(dgf);
							} else nerr++;
						}
						dgfModule = gDGFControlData->NextModule(dgfModule);
					}
					gDGFControlData->Dgfrc()->Write();
				}
			}
		}
	}

	if (nerr > 0 || gMrbLog->GetErrors(errLog) > errCnt) {
		gMrbLog->Err()	<< "Downloading DGF code failed" << endl;
		gMrbLog->Flush(this->ClassName(), "ReloadDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Downloading DGF code failed", kMBIconStop);
		return(kFALSE);
	} else {
		gMrbLog->Out()	<< "Download of DGF code finished" << endl;
		gMrbLog->Flush(this->ClassName(), "ReloadDGFs", setblue);
		return(kTRUE);
	}
	return(kTRUE);
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

	if (this->DaqIsRunning()) {
		gMrbLog->Err()	<< "DAQ seems to be running - can't access DGF modules" << endl;
		gMrbLog->Flush(this->ClassName(), "AbortDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DAQ seems to be running", kMBIconStop);
		return(kFALSE);
	}

	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "AbortDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	nerr = 0;
	dgfModule = gDGFControlData->FirstModule();
	Int_t nofModules = 0;
	isAborted = kFALSE;
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, "Aborting Busy/Sync ...", 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, gDGFControlData->GetNofModules());
	while (dgfModule) {
		Int_t cl = nofModules / kNofModulesPerCluster;
		Int_t modNo = nofModules - cl * kNofModulesPerCluster;
		UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
		if (((fCluster[cl]->GetActive() & bits) == bits ) && dgfModule->IsActive()) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				isAborted = kTRUE;
				dgf->SetParValue("SYNCHDONE", 1, kTRUE);
				if (!dgf->StopRun()) nerr++;
			}
			pgb->Increment(1, dgfModule->GetName());
			gSystem->ProcessEvents();
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}
//	pgb->DeleteWindow();
	delete pgb;

	if (nerr > 0) {
		gMrbLog->Err()	<< "Aborting busy-sync loop failed" << endl;
		gMrbLog->Flush(this->ClassName(), "AbortDGFs");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Aborting busy-sync loop failed", kMBIconStop);
		return(kFALSE);
	} else if (offlineMode || isAborted) {
		gMrbLog->Out()	<< "Busy-sync loop terminated properly" << endl;
		gMrbLog->Flush(this->ClassName(), "AbortDGFs", setblue);
		return(kTRUE);
	} else {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
}

Bool_t DGFSetupPanel::RestartEsone() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::RestartEsone
// Purpose:        Restart esone server
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Restarts esone server
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t offlineMode = gDGFControlData->IsOffline();

	if (this->DaqIsRunning()) {
		gMrbLog->Err()	<< "DAQ seems to be running - can't access DGF modules" << endl;
		gMrbLog->Flush(this->ClassName(), "RestartEsone");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DAQ seems to be running", kMBIconStop);
		return(kFALSE);
	}

	esoneCold = new TMrbEsone(offlineMode);
	if (esoneCold->IsZombie()) {
		gMrbLog->Err()	<< "ESONE not running" << endl;
		gMrbLog->Flush(this->ClassName(), "RestartEsone");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "ESONE not running", kMBIconStop);
		delete esoneCold;
		esoneCold = NULL;
		return(kFALSE);
	}

	TString camacHost = fCAMACHostEntry->GetText();
	gDGFControlData->fCAMACHost = camacHost;

	if (!offlineMode) {
		cout	<< setblue
				<< "[DGFSetupPanel::StartDGFs(): Cold start for ESONE server / MBS @ " << camacHost << "]"
				<< setblack << endl;
		if (!esoneCold->StartServer(camacHost.Data())) {
			gMrbLog->Err()	<< "Restarting ESONE server failed" << endl;
			gMrbLog->Flush(this->ClassName(), "RestartEsone");
			new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Restarting ESONE server failed", kMBIconStop);
			delete esoneCold;
			esoneCold = NULL;
			return(kFALSE);
		}
	}

	esoneCold->SetVerboseMode(gDGFControlData->IsDebug());
	esoneCold->SetSingleStep(gDGFControlData->IsSingleStep());
	return(kTRUE);
}

Bool_t DGFSetupPanel::TurnUserPSAOnOff(Bool_t ActivateFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::TurnUserPSAOnOff
// Purpose:        Turn user PSA on/off
// Arguments:      Bool_t ActivateFlag   -- kTRUE if user PSA code is to be activated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Controls the PSA feature.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t offlineMode = gDGFControlData->IsOffline();

	if (this->DaqIsRunning()) {
		gMrbLog->Err()	<< "DAQ seems to be running - can't access DGF modules" << endl;
		gMrbLog->Flush(this->ClassName(), "TurnUserPSAOnOff");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DAQ seems to be running", kMBIconStop);
		return(kFALSE);
	}

	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "TurnUserPSAOnOff");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	Int_t nerr = 0;
	DGFModule * dgfModule = gDGFControlData->FirstModule();
	Int_t nofModules = 0;
	Bool_t found = kFALSE;
	TString onoff = ActivateFlag ? "Turning UserPSA ON ..." : "Turning UserPSA OFF ...";
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, onoff, 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, gDGFControlData->GetNofModules());
	while (dgfModule) {
		Int_t cl = nofModules / kNofModulesPerCluster;
		Int_t modNo = nofModules - cl * kNofModulesPerCluster;
		UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
		if (((fCluster[cl]->GetActive() & bits) == bits ) && dgfModule->IsActive()) {
			this->TurnUserPSAOnOff(dgfModule, ActivateFlag);
			found = kTRUE;
			pgb->Increment(1, dgfModule->GetName());
			gSystem->ProcessEvents();
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}
//	pgb->DeleteWindow();
	delete pgb;
	if (nerr > 0) {
		gMrbLog->Err()	<< "Turning PSA code on/off failed" << endl;
		gMrbLog->Flush(this->ClassName(), "TurnUserPSAOnOff");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Turning PSA code on/off failed", kMBIconStop);
		return(kFALSE);
	} else if (offlineMode || found) {
		onoff = ActivateFlag ? "ON" : "OFF";
		gMrbLog->Out()	<< "User PSA code turned " << onoff << endl;
		gMrbLog->Flush(this->ClassName(), "TurnUserPSAOnOff", setblue);
		gDGFControlData->Dgfrc()->Write();
		return(kTRUE);
	} else {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
}

Bool_t DGFSetupPanel::TurnUserPSAOnOff(DGFModule * Module, Bool_t ActivateFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::TurnUserPSAOnOff
// Purpose:        Turn user PSA on/off
// Arguments:      DGFModule * Module    -- module
//                 Bool_t ActivateFlag   -- kTRUE if user PSA code is to be activated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Turns PSA mode on/off for specified module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t offlineMode = gDGFControlData->IsOffline();

	TString trueFalse = ActivateFlag ? "TRUE" : "FALSE";
	TMrbDGF * dgf = Module->GetAddr();
	if (dgf && !offlineMode) dgf->ActivateUserPSACode(ActivateFlag);
	gDGFControlData->Dgfrc()->Set(".Module", Module->GetName(), "ActivateUserPSACode", trueFalse);
	return(kTRUE);
}

Bool_t DGFSetupPanel::SetSwitchBus(Bool_t IndivFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::SetSwitchBus
// Purpose:        Set switchbus register
// Arguments:      Bool_t IndivFlag   -- kTRUE if user PSA code is to be activated
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets switchbus register.
// Keywords:
//////////////////////////////////////////////////////////////////////////////
// Arguments:      DGFModule * Module    -- module

	Bool_t offlineMode = gDGFControlData->IsOffline();

	if (this->DaqIsRunning()) {
		gMrbLog->Err()	<< "DAQ seems to be running - can't access DGF modules" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSwitchBus");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DAQ seems to be running", kMBIconStop);
		return(kFALSE);
	}

	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSwitchBus");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	Int_t nerr = 0;
	DGFModule * dgfModule = gDGFControlData->FirstModule();
	Int_t nofModules = 0;
	Bool_t found = kFALSE;
	TString indiv = IndivFlag ? "Terminating switchbus individually ..." : "Terminating switchbus for cores only ...";
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, indiv, 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, gDGFControlData->GetNofModules());
	while (dgfModule) {
		Int_t cl = nofModules / kNofModulesPerCluster;
		Int_t modNo = nofModules - cl * kNofModulesPerCluster;
		UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
		if (((fCluster[cl]->GetActive() & bits) == bits ) && dgfModule->IsActive()) {
			if (!offlineMode) {
				TMrbDGF * dgf = dgfModule->GetAddr();
				if (dgf && !dgf->SetSwitchBusDefault(gDGFControlData->fIndivSwitchBusTerm, "DGFControl", gDGFControlData->Dgfrc()->Env())) nerr++;
			}
			found = kTRUE;
			pgb->Increment(1, dgfModule->GetName());
			gSystem->ProcessEvents();
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}
//	pgb->DeleteWindow();
	delete pgb;
	if (nerr > 0) {
		gMrbLog->Err()	<< "Setting switchbus register failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSwitchBus");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Setting switchbus register failed", kMBIconStop);
		return(kFALSE);
	} else if (offlineMode || found) {
		indiv = IndivFlag ? "individually" : "for cores only";
		gMrbLog->Out()	<< "Switchbus terminated " << indiv << endl;
		gMrbLog->Flush(this->ClassName(), "SetSwitchBus", setblue);
		gDGFControlData->Dgfrc()->Write();
		return(kTRUE);
	} else {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
}

Bool_t DGFSetupPanel::SetSynchWait(Bool_t SyncFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::SetSynchWait
// Purpose:        Turn busy/sync loop on/off
// Arguments:      Bool_t SyncFlag   -- kTRUE busy/sync loop
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    (De)activates the busy/sync loop
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t offlineMode = gDGFControlData->IsOffline();

	if (this->DaqIsRunning()) {
		gMrbLog->Err()	<< "DAQ seems to be running - can't access DGF modules" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSynchWait");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DAQ seems to be running", kMBIconStop);
		return(kFALSE);
	}

	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSynchWait");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	Int_t nerr = 0;
	DGFModule * dgfModule = gDGFControlData->FirstModule();
	Int_t nofModules = 0;
	Bool_t found = kFALSE;
	TString onoff = SyncFlag ? "Turning busy/sync loop ON ..." : "Turning busy/sync loop OFF ...";
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, onoff, 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, gDGFControlData->GetNofModules());
	while (dgfModule) {
		Int_t cl = nofModules / kNofModulesPerCluster;
		Int_t modNo = nofModules - cl * kNofModulesPerCluster;
		UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
		if (((fCluster[cl]->GetActive() & bits) == bits ) && dgfModule->IsActive()) {
			this->SetSynchWait(dgfModule, SyncFlag);
			found = kTRUE;
			pgb->Increment(1, dgfModule->GetName());
			gSystem->ProcessEvents();
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}
//	pgb->DeleteWindow();
	delete pgb;
	if (nerr > 0) {
		gMrbLog->Err()	<< "Turning busy/sync loop on/off failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetSynchWait");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Turning busy/sync loop on/off failed", kMBIconStop);
		return(kFALSE);
	} else if (offlineMode || found) {
		onoff = SyncFlag ? "ON" : "OFF";
		gMrbLog->Out()	<< "Busy/sync loop turned " << onoff << endl;
		gMrbLog->Flush(this->ClassName(), "SetSynchWait", setblue);
		gDGFControlData->Dgfrc()->Write();
		return(kTRUE);
	} else {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
}

Bool_t DGFSetupPanel::SetSynchWait(DGFModule * Module, Bool_t SyncFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::SetSynchWait
// Purpose:        Turn busy/sync loop on/off
// Arguments:      DGFModule * Module    -- module
//                 Bool_t SyncFlag   -- kTRUE busy/sync loop
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    (De)activates the busy/sync loop
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t offlineMode = gDGFControlData->IsOffline();

	Int_t onoff = SyncFlag ? 1 : 0;
	TMrbDGF * dgf = Module->GetAddr();
	if (dgf && !offlineMode) dgf->SetSynchWait(SyncFlag, kTRUE);
	gDGFControlData->Dgfrc()->Set(".Module", Module->GetName(), "SynchWait", onoff);
	return(kTRUE);
}

Bool_t DGFSetupPanel::SetInSynch(Bool_t SyncFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::SetInSynch
// Purpose:        Synchronize clock with run
// Arguments:      Bool_t SyncFlag   -- kTRUE if to be synchronized
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Synchronizes clock with run
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t offlineMode = gDGFControlData->IsOffline();

	if (this->DaqIsRunning()) {
		gMrbLog->Err()	<< "DAQ seems to be running - can't access DGF modules" << endl;
		gMrbLog->Flush(this->ClassName(), "SetInSynch");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "DAQ seems to be running", kMBIconStop);
		return(kFALSE);
	}

	if (gDGFControlData->GetNofModules() == 0) {
		gMrbLog->Err()	<< "Number of DGF modules = 0" << endl;
		gMrbLog->Flush(this->ClassName(), "SetInSynch");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Number of DGF modules = 0", kMBIconStop);
		return(kFALSE);
	}

	Int_t nerr = 0;
	DGFModule * dgfModule = gDGFControlData->FirstModule();
	Int_t nofModules = 0;
	Bool_t found = kFALSE;
	TString onoff = SyncFlag ? "Synchronizing clock with run ..." : "Clock is running free ...";
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, onoff, 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, gDGFControlData->GetNofModules());
	while (dgfModule) {
		Int_t cl = nofModules / kNofModulesPerCluster;
		Int_t modNo = nofModules - cl * kNofModulesPerCluster;
		UInt_t bits = (UInt_t) gDGFControlData->ModuleIndex(cl, modNo);
		if (((fCluster[cl]->GetActive() & bits) == bits ) && dgfModule->IsActive()) {
			this->SetInSynch(dgfModule, SyncFlag);
			found = kTRUE;
			pgb->Increment(1, dgfModule->GetName());
			gSystem->ProcessEvents();
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}
//	pgb->DeleteWindow();
	delete pgb;
	if (nerr > 0) {
		gMrbLog->Err()	<< "Synchronizing clock failed" << endl;
		gMrbLog->Flush(this->ClassName(), "SetInSynch");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Synchronizing clock failed", kMBIconStop);
		return(kFALSE);
	} else if (offlineMode || found) {
		onoff = SyncFlag ? "ON" : "OFF";
		gMrbLog->Out()	<< "Synchronizing clock with run turned " << onoff << endl;
		gMrbLog->Flush(this->ClassName(), "SetInSynch", setblue);
		gDGFControlData->Dgfrc()->Write();
		return(kTRUE);
	} else {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}
}

Bool_t DGFSetupPanel::SetInSynch(DGFModule * Module, Bool_t SyncFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::SetInSynch
// Purpose:        Synchronize clock with run
// Arguments:      DGFModule * Module    -- module
//                 Bool_t SyncFlag       -- kTRUE if to be synchronized
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Synchronizes clock with run
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t offlineMode = gDGFControlData->IsOffline();

	Int_t onoff = SyncFlag ? 0 : 1;
	TMrbDGF * dgf = Module->GetAddr();
	if (dgf && !offlineMode) dgf->SetSynchWait(onoff, kTRUE);
	gDGFControlData->Dgfrc()->Set(".Module", Module->GetName(), "InSynch", onoff);
	return(kTRUE);
}

Bool_t DGFSetupPanel::DaqIsRunning() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::DaqIsRunning
// Purpose:        Check if a DAQ is running
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks for a running daq.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gDGFControlData->IsOffline()) return(kFALSE);		// don't worry about if offline

	TString pidFile = "/tmp/M_analyze_";
	pidFile += gSystem->Getenv("USER");
	pidFile += ".9090";
	if (gSystem->AccessPathName(pidFile.Data())) return(kFALSE);	// no pid file, therefore no active daq

	ifstream p;
	p.open(pidFile.Data(), ios::in);
	Int_t pid;
	p >> pid;
	TString proc = "/proc/";
	proc += pid;
	if (gSystem->AccessPathName(proc.Data())) return(kFALSE);		// pid exists but process has gone

	return(kTRUE);
}

void DGFSetupPanel::SetupGeneral() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetupPanel::SetupGeneral
// Purpose:        Set global options
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Reads checkbuttons and performs actions
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gDGFControlData->fSimulStartStop = (fDGFFrame->GetActive() & DGFControlData::kDGFSimulStartStop) != 0;
	this->SetSynchWait(gDGFControlData->fSimulStartStop);

	gDGFControlData->fSyncClocks = (fDGFFrame->GetActive() & DGFControlData::kDGFSyncClocks) != 0;
	this->SetInSynch(gDGFControlData->fSyncClocks);

	gDGFControlData->fIndivSwitchBusTerm = (fDGFFrame->GetActive() & DGFControlData::kDGFIndivSwitchBusTerm) != 0;
	this->SetSwitchBus(gDGFControlData->fIndivSwitchBusTerm);

	gDGFControlData->fUserPSA = (fDGFFrame->GetActive() & DGFControlData::kDGFUserPSA) != 0;
	this->TurnUserPSAOnOff(gDGFControlData->fUserPSA);
}
