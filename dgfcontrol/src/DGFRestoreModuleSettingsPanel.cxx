//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFRestoreModuleSettingsPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Restore settings
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
				{0, 									NULL,			NULL								}
			};

extern DGFControlData * gDGFControlData;

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
	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	this->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofModuleKeys[cl], cl, 1, kTRUE), 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], buttonGC->LH());
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}
	
	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, frameGC->LH());
	
	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		fGroupSelect[i]->Associate(this);
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	frameGC->LH()->GetPadLeft(),
																			frameGC->LH()->GetPadRight(),
																			frameGC->LH()->GetPadTop(),
																			frameGC->LH()->GetPadBottom()));
	fAllSelect->Associate(this);
			
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
	
	fActionButtons = new TGMrbTextButtonList(fActionFrame, NULL, &fActions, 1,
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fActionButtons);
	fActionFrame->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	fActionButtons->Associate(this);

	this->ChangeBackground(gDGFControlData->fColorGreen);

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t DGFRestoreModuleSettingsPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFRestoreModuleSettingsPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFRestoreModuleSettingsPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFRestoreModuleSettingsSelectColumn) {
						switch (Param1) {
							case kDGFRestoreModuleSettingsRestore:
								this->LoadDatabase(kTRUE);
								break;
							case kDGFRestoreModuleSettingsSelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
								break;
							case kDGFRestoreModuleSettingsSelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
								break;							
							default:	break;
						}
					} else {
						Param1 -= kDGFRestoreModuleSettingsSelectColumn;
						Bool_t select = ((Param1 & 1) == 0);
						UInt_t bit = 0x1 << (Param1 >> 1);
						for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
							if (gDGFControlData->GetPatInUse(cl) & bit) {
								if (select) fCluster[cl]->SetState(bit, kButtonDown);
								else		fCluster[cl]->SetState(bit, kButtonUp);
							}
						}
					}
					break;
				default:	break;
			}
			break;

	}
	return(kTRUE);
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
	TString sPath;
	UInt_t modIdx;
	Int_t cl, modNo;
				
	fileInfoRestore.fFileTypes = (const Char_t **) kDGFFileTypesSettings;
	env.Find(sPath, "DGFControl:TMrbDGF", "SettingsPath", "../dgfSettings");
	fileInfoRestore.fIniDir = StrDup(sPath);

	new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fileInfoRestore);
	if (fileInfoRestore.fFilename == NULL || *fileInfoRestore.fFilename == '\0') return(kFALSE);
	loadDir = fileInfoRestore.fFilename;
	TString base;
	uxSys.GetBaseName(base, loadDir.Data());
	TString dir;
	uxSys.GetDirName(dir, loadDir.Data());
	loadDir = dir;
	loadDir += "/data/";
	loadDir += base;
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
				h.FromInteger(dgf->GetClusterHexNum(), 0, ' ', 16, kFALSE);
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
						if (dgf->Data()->ReadNameTable(gDGFControlData->fDSPParamsFile) <= 0) nerr++;
						param = dgf->Data()->FirstParam();
						while (param) {
							if (param->GetIndex() < TMrbDGFData::kNofDSPInputParams) {
								paramName = param->GetName();
								dgfResource = paramName;
								parval = dgfEnv->GetValue(dgfResource.Data(), 0);
								dgf->SetParValue(paramName.Data(), parval, kFALSE);
							}
							param = dgf->Data()->NextParam(param);
						}
						dgf->SetParamValuesRead();
						dgf->WriteParamMemory(kFALSE);
						gDGFControlData->fDeltaT = dgf->GetDeltaT();
						nofRestored++;
					}
				} else nerr++;
				if (LoadPSA) {
					psaFile = loadDir;
					psaFile += "/";
					psaFile += dgfName;
					psaFile += ".psa";
					altParamFile = loadDir;
					altParamFile += "/dgfCommon.psa";
					if (!dgf->LoadPsaParams(psaFile.Data(), altParamFile.Data())) nerr++;
				}
			}
			pgb->Increment(1, module->GetName());
			gSystem->ProcessEvents();
			module = gDGFControlData->NextModule(module);
		}
		delete pgb;
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
