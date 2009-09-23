//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFRestoreModuleSettingsPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Restore settings
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFRestoreModuleSettingsPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $
// Date:
// URL:
// Keywords:
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFRestoreModuleSettingsPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include "TEnv.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TOrdCollection.h"

#include "TGFileDialog.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbLofDGFs.h"
#include "TMrbSystem.h"
#include "TMrbEnv.h"
#include "TGMrbProgressBar.h"

#include "DGFControlData.h"
#include "DGFRestoreModuleSettingsPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

static Char_t * kDGFFileTypesSettings[]	=	{
												"All saves",			"*",
												NULL,					NULL
											};

const SMrbNamedX kDGFRestoreModuleSettingsActions[] =
			{
				{DGFRestoreModuleSettingsPanel::kDGFRestoreModuleSettingsRestore,		"Restore",			"Restore module params"	},
				{DGFRestoreModuleSettingsPanel::kDGFRestoreModuleSettingsRestorePSA,	"Restore (+PSA)",	"Restore params + *OLD STYLE* PSA params"	},
				{0, 									NULL,			NULL								}
			};

extern DGFControlData * gDGFControlData;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

extern TMrbLogger * gMrbLog;

ClassImp(DGFRestoreModuleSettingsPanel)

DGFRestoreModuleSettingsPanel::DGFRestoreModuleSettingsPanel(TGCompositeFrame * TabFrame) :
						TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel
// Purpose:        DGF Viewer: Setup Panel
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////// Profiles Laden////////////////////////////////////////

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
// 	stdProfile = gMrbLofProfiles->GetDefault();
	stdProfile = gMrbLofProfiles->FindProfile("green", kTRUE);
	//yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	//blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);

	gMrbLofProfiles->Print();

////////////////////////////////////////////////////////////////////////////////////////////////

/*	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;*/

	TMrbString camacHost;
	TMrbString intStr;

	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	clear focus list
	fFocusList.Clear();

/*	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);

	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(labelGC);

	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);*/

	TGLayoutHints * expandXLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,2,5,2);
	HEAP(expandXLayout);

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));

//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFRestoreModuleSettingsSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFRestoreModuleSettingsSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFRestoreModuleSettingsSelectNone, "cbutton_none.xpm");


//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kDGFRestoreModuleSettingsActions);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fModules);
	this->AddFrame(fModules, dgfFrameLayout);

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TNGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), -1,stdProfile, 1,0,
							kTabWidth, kLEHeight,0, lofSpecialButtons, kTRUE);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], expandXLayout);
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}

	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight, kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame,dgfFrameLayout);

	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TNGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], -1,stdProfile, 1,0,
							kTabWidth, kLEHeight);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], dgfFrameLayout);
		((TNGMrbButtonFrame *) fGroupSelect[i])->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");
	}
	fAllSelect = new TNGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, -1,stdProfile, 1,0,
							kTabWidth, kLEHeight);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	dgfFrameLayout->GetPadLeft(),
																			dgfFrameLayout->GetPadRight(),
																			dgfFrameLayout->GetPadTop(),
																			dgfFrameLayout->GetPadBottom()));
	((TNGMrbButtonFrame *) fAllSelect)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");

// action buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);
	TGLayoutHints * aButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
// 	labelGC->SetLH(aButtonLayout);
	HEAP(aButtonLayout);

	fActionFrame = new TGGroupFrame(this, "Actions", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, aFrameLayout);

	fActionButtons = new TNGMrbTextButtonList(fActionFrame, NULL, &fActions, -1,stdProfile, 1,0,
							kTabWidth, kLEHeight);
	HEAP(fActionButtons);
	fActionFrame->AddFrame(fActionButtons,aFrameLayout);
	fActionButtons->JustifyButton(kTextCenterX);
	((TNGMrbButtonFrame *) fActionButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gDGFControlData->fColorGreen);

	dgfFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);
	this->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	TabFrame->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFRestoreModuleSettingsPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel::SelectModule
// Purpose:        Slot method: select module(s)
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFRestoreModuleSettingsSelectColumn) {
		switch (Selection) {
			case kDGFRestoreModuleSettingsSelectAll:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
				break;
			case kDGFRestoreModuleSettingsSelectNone:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
				break;
		}
	} else {
		Selection -= kDGFRestoreModuleSettingsSelectColumn;
		Bool_t select = ((Selection & 1) == 0);
		UInt_t bit = 0x1 << (Selection >> 1);
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			if (gDGFControlData->GetPatInUse(cl) & bit) {
				UInt_t act = fCluster[cl]->GetActive();
				UInt_t down = select ? (act | bit) : (act & ~bit);
				fCluster[cl]->SetState( 0xFFFF, kButtonUp);
				fCluster[cl]->SetState(down & 0xFFFF, kButtonDown);
			}
		}
	}
}

void DGFRestoreModuleSettingsPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFRestoreModuleSettingsRestore:
			this->LoadDatabase(kFALSE);
			break;
		case kDGFRestoreModuleSettingsRestorePSA:
			this->LoadDatabase(kTRUE);
			break;
	}
}

Bool_t DGFRestoreModuleSettingsPanel::LoadDatabase(Bool_t LoadPSA) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel::LoadDatabase
// Purpose:        Restore DGF settings from file
// Arguments:      Bool_t LoadPSA     -- load PSA values if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Restores DGF settings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGFileInfo fileInfoRestore;

	TMrbSystem uxSys;
	TMrbEnv env;
	TString errMsg;
	TString dgfFile;

	TString dgfName, paramFile, psaFile, loadDir;
	TMrbString altParamFile;
	TEnv * dgfEnv;
	TString fileType;
	DGFModule * module;
	TMrbDGF * dgf;
	TMrbString paramName;
	Bool_t offlineMode;
	Int_t nerr;
	Int_t nofRestored;
	TString dgfResource;
	TMrbNamedX * param;
	Int_t parval = 0;
	Bool_t verbose;
	UInt_t modIdx;
	Int_t cl, modNo;

	fileInfoRestore.fFileTypes = (const Char_t **) kDGFFileTypesSettings;
	if (!gDGFControlData->CheckAccess(gDGFControlData->fDgfSettingsPath.Data(),
						(Int_t) DGFControlData::kDGFAccessDirectory | DGFControlData::kDGFAccessRead, errMsg)) {
		fileInfoRestore.fIniDir = StrDup(gSystem->WorkingDirectory());
		errMsg += "\nFalling back to \"";
		errMsg += gSystem->WorkingDirectory();
		errMsg += "\"";
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", errMsg.Data(), kMBIconExclamation);
	} else {
		fileInfoRestore.fIniDir = StrDup(gDGFControlData->fDgfSettingsPath);
	}

	new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fileInfoRestore);
	if (fileInfoRestore.fFilename == NULL || *fileInfoRestore.fFilename == '\0') return(kFALSE);
	loadDir = fileInfoRestore.fFilename;

	TString baseName1, baseName2, dirName;				// check if user did a double click
	uxSys.GetBaseName(baseName1, loadDir.Data());		// as a result the last 2 parts of the returned path
	uxSys.GetDirName(dirName, loadDir.Data());			// will be identical
	uxSys.GetBaseName(baseName2, dirName.Data());		// example: single click returns /a/b/c, double click /a/b/c/c
	if (baseName1.CompareTo(baseName2.Data()) == 0) loadDir = dirName;	// double click: strip off last part

	if (!uxSys.IsDirectory(loadDir.Data())) {
		errMsg = "No such directory - ";
		errMsg += loadDir;
		gMrbLog->Err() << errMsg << endl;
		gMrbLog->Flush(this->ClassName(), "LoadDatabase");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", errMsg.Data(), kMBIconStop);
		return(kFALSE);
	}

	offlineMode = gDGFControlData->IsOffline();

	nerr = 0;

	nofRestored = 0;
	module = gDGFControlData->FirstModule();
	verbose = gDGFControlData->IsVerbose();
	if (module) {
		TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, "Restoring module params ...", 400, "blue", NULL, kTRUE);
		pgb->SetRange(0, gDGFControlData->GetNofModules());
		while (module) {
			modIdx = gDGFControlData->GetIndex(module->GetName(), cl, modNo);
			modIdx &= 0xFFFF;
			if (gDGFControlData->ModuleInUse(module) && (fCluster[cl]->GetActive() & modIdx)) {
				dgfName = module->GetName();
				dgf = module->GetAddr();
				paramFile = loadDir;
				paramFile += "/";
				paramFile += dgfName;
				paramFile += ".par";
				altParamFile = loadDir;
				altParamFile += "/dgf_";
				altParamFile += dgf->GetClusterSerial();
				altParamFile += "_";
				TMrbString h;
				h.FromInteger(dgf->GetClusterHexNum(), 0, 16, kFALSE, kFALSE);
				altParamFile += h;
				altParamFile += "_";
				TString s = dgf->GetClusterSegments();
				if (s.Index("c", 0) != -1) altParamFile += "0"; else altParamFile += "3";
				altParamFile += ".par";
				dgfEnv = new TEnv(".dgfParams");
				if (dgf->LoadParamsToEnv(dgfEnv, paramFile.Data(), altParamFile.Data())) {
					if (dgf->IsConnected()) {
						dgf->SetVerboseMode(gDGFControlData->IsVerbose());
						dgf->Camac()->SetVerboseMode(gDGFControlData->IsDebug());
						param = dgf->Data()->FirstParam();
						while (param) {
							if (param->GetIndex() >= 0 && param->GetIndex() < TMrbDGFData::kNofDSPInputParams) {
								paramName = param->GetName();
								dgfResource = paramName;
								parval = dgfEnv->GetValue(dgfResource.Data(), 0);
								dgf->SetParValue(paramName.Data(), parval, kFALSE);
							}
							param = dgf->Data()->NextParam(param);
						}
						dgf->SetParamValuesRead();
						gDGFControlData->fDeltaT = dgf->GetDeltaT();
						nofRestored++;
						if (LoadPSA) {
							psaFile = loadDir;
							psaFile += "/";
							psaFile += dgfName;
							psaFile += ".psa";
							altParamFile = loadDir;
							altParamFile += "/dgfCommon.psa";
							if (!dgf->LoadPsaParams(psaFile.Data(), altParamFile.Data())) nerr++;
						}
						dgf->WriteParamMemory(kTRUE);
					}
				} else nerr++;
				pgb->Increment(1, module->GetName());
			}
			gSystem->ProcessEvents();
			module = gDGFControlData->NextModule(module);
		}
		pgb->DeleteWindow();
	}

	if (nerr > 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Restoring DGF modules failed", kMBIconStop);
	}

	if (nofRestored > 0) {
		gMrbLog->Out()	<< "Settings of " << nofRestored << " DGF module(s) restored from file ("
						<< nerr << " error(s))" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadDataBase", setblue);
		return(kTRUE);
	} else if (nerr == 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module or channel", kMBIconStop);
		return(kFALSE);
	}

	return(kTRUE);
}
