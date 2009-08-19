//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFSaveModuleSettingsPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Restore settings
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFSaveModuleSettingsPanel.cxx,v 1.19 2009-08-19 12:52:49 Rudolf.Lutter Exp $
// Date:
// URL:
// Keywords:
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFSaveModuleSettingsPanel.gif>
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
#include "DGFSaveModuleSettingsPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

static Char_t * kDGFFileTypesSettings[]	=	{
												"All saves",			"*",
												NULL,					NULL
											};

const SMrbNamedX kDGFSaveModuleSettingsActions[] =
			{
				{DGFSaveModuleSettingsPanel::kDGFSaveModuleSettingsRestore,		"Save",		"Save module settings"	},
				{0, 									NULL,			NULL								}
			};

extern DGFControlData * gDGFControlData;

extern TMrbLogger * gMrbLog;

ClassImp(DGFSaveModuleSettingsPanel)

DGFSaveModuleSettingsPanel::DGFSaveModuleSettingsPanel(TGCompositeFrame * TabFrame) :
					TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSaveModuleSettingsPanel
// Purpose:        DGF Viewer: Setup Panel
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:
// Exceptions:
// Description:
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

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
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
								gDGFControlData->fColorWhite);	HEAP(entryGC);

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));

//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFSaveModuleSettingsSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFSaveModuleSettingsSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFSaveModuleSettingsSelectNone, "cbutton_none.xpm");


//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kDGFSaveModuleSettingsActions);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	this->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), -1, 1,
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], buttonGC->LH());
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}

	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight,
													kChildFrame, frameGC->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, frameGC->LH());

	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], -1, 1,
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		((TGMrbButtonFrame *) fGroupSelect[i])->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, -1, 1,
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	frameGC->LH()->GetPadLeft(),
																			frameGC->LH()->GetPadRight(),
																			frameGC->LH()->GetPadTop(),
																			frameGC->LH()->GetPadBottom()));
	((TGMrbButtonFrame *) fAllSelect)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");

// action buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);
	TGLayoutHints * aButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
	labelGC->SetLH(aButtonLayout);
	HEAP(aButtonLayout);

	fActionFrame = new TGGroupFrame(this, "Actions", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, groupGC->LH());

	fActionButtons = new TGMrbTextButtonList(fActionFrame, NULL, &fActions, -1, 1,
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fActionButtons);
	fActionFrame->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	((TGMrbButtonFrame *) fActionButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gDGFControlData->fColorGreen);

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFSaveModuleSettingsPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSaveModuleSettingsPanel::SelectModule
// Purpose:        Slot method: select module(s)
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFSaveModuleSettingsSelectColumn) {
		switch (Selection) {
			case kDGFSaveModuleSettingsSelectAll:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
				break;
			case kDGFSaveModuleSettingsSelectNone:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
		}
	} else {
		Selection -= kDGFSaveModuleSettingsSelectColumn;
		Bool_t select = ((Selection & 1) == 0);
		UInt_t bit = 0x1 << (Selection >> 1);
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			if (gDGFControlData->GetPatInUse(cl) & bit) {
				UInt_t act = fCluster[cl]->GetActive();
				UInt_t down = select ? (act | bit) : (act & ~bit);
				fCluster[cl]->SetState(down & 0xFFFF, kButtonDown);
			}
		}
	}
}

void DGFSaveModuleSettingsPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSaveModuleSettingsPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFSaveModuleSettingsRestore:
			this->SaveDatabase();
			break;
	}
}

Bool_t DGFSaveModuleSettingsPanel::SaveDatabase() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSaveModuleSettingsPanel::SaveDatabase
// Purpose:        Save DGF settings to file
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Saves DGF settings.
//                 Each individual module will be saved to <dgfName>.par,
//                 File "dgfControl.par" will contained some global settings.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGFileInfo fileInfoSave;
	TString saveDir, paramFile, valueFile;
	TMrbSystem uxSys;
	TMrbEnv env;
	Int_t nerr;
	TString errMsg;
	TString cmd;
	Int_t nofModules;
	DGFModule * dgfModule;
	TMrbDGF * dgf;
	UInt_t modIdx;
	Int_t cl, modNo;

	Bool_t verbose = gDGFControlData->IsVerbose();

	fileInfoSave.fFileTypes = (const Char_t **) kDGFFileTypesSettings;
	if (!gDGFControlData->CheckAccess(gDGFControlData->fDgfSettingsPath.Data(),
						(Int_t) DGFControlData::kDGFAccessDirectory | DGFControlData::kDGFAccessWrite, errMsg)) {
		fileInfoSave.fIniDir = StrDup(gSystem->WorkingDirectory());
		errMsg += "\nFalling back to \"";
		errMsg += gSystem->WorkingDirectory();
		errMsg += "\"";
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", errMsg.Data(), kMBIconExclamation);
	} else {
		fileInfoSave.fIniDir = StrDup(gDGFControlData->fDgfSettingsPath);
	}

	new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fileInfoSave);
	if (fileInfoSave.fFilename == NULL || *fileInfoSave.fFilename == '\0') return(kFALSE);
	saveDir = fileInfoSave.fFilename;

	TString baseName1, baseName2, dirName;				// check if user did a double click
	uxSys.GetBaseName(baseName1, saveDir.Data());		// as a result the last 2 parts of the returned path
	uxSys.GetDirName(dirName, saveDir.Data());			// will be identical
	uxSys.GetBaseName(baseName2, dirName.Data());		// example: single click returns /a/b/c, double click /a/b/c/c
	if (baseName1.CompareTo(baseName2.Data()) == 0) saveDir = dirName;	// double click: strip off last part

	if (!uxSys.Exists(saveDir.Data())) {
		cmd = "mkdir -p ";
		cmd += saveDir;
		gSystem->Exec(cmd);
		if (verbose) {
			gMrbLog->Out() << "Creating directory - " << saveDir << endl;
			gMrbLog->Flush(this->ClassName(), "SaveDatabase", setblue);
		}
	}

	if (!uxSys.IsDirectory(saveDir.Data())) {
		errMsg = "File \"";
		errMsg += saveDir.Data();
		errMsg += "\" is NOT a directory";
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", errMsg.Data(), kMBIconStop);
		return(kFALSE);
	}

	nerr = 0;

	dgfModule = gDGFControlData->FirstModule();
	nofModules = 0;
	if (dgfModule) {
		TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, "Saving module params ...", 400, "blue", NULL, kTRUE);
		pgb->SetRange(0, gDGFControlData->GetNofModules());
		while (dgfModule) {
			modIdx = gDGFControlData->GetIndex(dgfModule->GetName(), cl, modNo);
			modIdx &= 0xFFFF;
			if (gDGFControlData->ModuleInUse(dgfModule) && (fCluster[cl]->GetActive() & modIdx)) {
				dgf = dgfModule->GetAddr();
				paramFile = saveDir;
				paramFile += "/";
				paramFile += dgf->GetName();
				valueFile = paramFile;
				paramFile += ".par";
				valueFile += ".val";
				if (!dgf->SaveParams(paramFile.Data())) nerr++;
				if (!dgf->SaveValues(valueFile.Data())) nerr++;
				nofModules++;
				pgb->Increment(1, dgfModule->GetName());
			}
			gSystem->ProcessEvents();
			dgfModule = gDGFControlData->NextModule(dgfModule);
		}
		pgb->DeleteWindow();
	}

	if (nerr > 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Saving DGF settings failed", kMBIconStop);
	}

	if (nofModules > 0) {
		gMrbLog->Out()	<< nofModules << " DGF module(s) saved to directory " << saveDir << " ("
						<< nerr << " error(s))" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveDatabase", setblue);
	} else if (nerr == 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module or channel", kMBIconStop);
		return(kFALSE);
	}

	return(kTRUE);
}
