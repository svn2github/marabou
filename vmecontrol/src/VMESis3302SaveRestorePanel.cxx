//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMESis3302SaveRestorePanel
// Purpose:        A GUI to control a SIS 3302 adc
// Description:    Save/restore settings
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302SaveRestorePanel.cxx,v 1.1 2008-10-16 08:29:33 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include "TEnv.h"
#include "TROOT.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "TGFileDialog.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbLofDGFs.h"
#include "TMrbSystem.h"
#include "TMrbEnv.h"
#include "TGMrbProgressBar.h"

#include "TC2LSis3302.h"
#include "VMESis3302SaveRestorePanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

static Char_t * kVMESis3302FileTypes[]	=	{
												"All saves",			"*",
												NULL,					NULL
											};

const SMrbNamedX kVMESis302SaveRestoreActions[] =
			{
				{VMESis3302SaveRestorePanel::kVMESis3302Save,		"Save",			"Save module settings"	},
				{VMESis3302SaveRestorePanel::kVMESis3302Restore,	"Restore",		"Restore module settings"	},
				{0, 											NULL,			NULL								}
			};

extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

ClassImp(VMESis3302SaveRestorePanel)


VMESis3302SaveRestorePanel::VMESis3302SaveRestorePanel(TGCompositeFrame * TabFrame, TMrbLofNamedX * LofModules) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302SaveRestorePanel
// Purpose:        VMEControl: Save & Restore
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * comboGC;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

// geometry
	Int_t frameWidth = this->GetWidth();

//	Clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gVMEControlData->SlantedFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGray);	HEAP(buttonGC);

	entryGC = new TGMrbLayout(	gVMEControlData->NormalFont(), 
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorWhite);	HEAP(entryGC);

	TGLayoutHints * loXpndX = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 10, 10, 1, 1);
	TGLayoutHints * loXpndX2 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	TGLayoutHints * loNormal = new TGLayoutHints(kLHintsTop, 10, 10, 1, 1);
	TGLayoutHints * loNormal2 = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	HEAP(loXpndX); HEAP(loXpndX2); HEAP(loNormal); HEAP(loNormal2);

	frameGC->SetLH(loNormal);
	comboGC->SetLH(loNormal2);
	labelGC->SetLH(loNormal);
	buttonGC->SetLH(loNormal);
	entryGC->SetLH(loNormal);
	groupGC->SetLH(loXpndX);

	TObjArray * lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kVMESis302SaveRestoreActions);

// modules
	fLofModules = LofModules;
	fLofSelected.Delete();
	TIterator * iter = fLofModules->MakeIterator();
	TC2LSis3302 * module;
	Int_t bit = 0x1;
	while (module = (TC2LSis3302 *) iter->Next()) {
		fLofSelected.AddNamedX(bit, module->GetName(), "", module);
		bit <<= 1;
	}

	fModules = new TGMrbCheckButtonGroup(this, "Modules", &fLofSelected,
													kVMESis3302Modules, 1, 
													frameGC, buttonGC, lofSpecialButtons);
	HEAP(fModules);
	this->AddFrame(fModules, groupGC->LH());

// action buttons
	fActionButtons = new TGMrbTextButtonGroup(this, "Actions", &fActions, -1, 1, frameGC, buttonGC);
	HEAP(fActionButtons);
	this->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	((TGMrbButtonFrame *) fActionButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gVMEControlData->fColorGreen);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void VMESis3302SaveRestorePanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302SaveRestorePanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case VMESis3302SaveRestorePanel::kVMESis3302Save:
			this->SaveSettings();
			break;
		case VMESis3302SaveRestorePanel::kVMESis3302Restore:
			this->RestoreSettings();
			break;
	}
}

Bool_t VMESis3302SaveRestorePanel::RestoreSettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302SaveRestorePanel::RestoreSettings
// Purpose:        Restore settings from file
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Restores settings.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString errMsg;
	TGFileInfo fileInfoRestore;

	Bool_t verbose = gVMEControlData->IsVerbose();

	UInt_t modSel = fModules->GetActive();
	if (modSel == 0) {
		gVMEControlData->MsgBox(this, "RestoreSettings", "Error", "No modules selected");
		return(kFALSE);
	}

	fileInfoRestore.fFileTypes = (const Char_t **) kVMESis3302FileTypes;
	if (!gVMEControlData->CheckAccess(gVMEControlData->fSettingsPath.Data(),
						(Int_t) VMEControlData::kVMEAccessDirectory | VMEControlData::kVMEAccessRead, errMsg)) {
		fileInfoRestore.fIniDir = StrDup(gSystem->WorkingDirectory());
		errMsg += "\nFalling back to \"";
		errMsg += gSystem->WorkingDirectory();
		errMsg += "\"";
		gVMEControlData->MsgBox(this, "RestoreSettings", "Warning", errMsg);
	} else {
		fileInfoRestore.fIniDir = StrDup(gVMEControlData->fSettingsPath);
	}

	new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fileInfoRestore);
	if (fileInfoRestore.fFilename == NULL || *fileInfoRestore.fFilename == '\0') return(kFALSE);
	TString loadDir = fileInfoRestore.fFilename;

	TString baseName1, baseName2, dirName;				// check if user did a double click
	TMrbSystem uxSys;
	uxSys.GetBaseName(baseName1, loadDir.Data());		// as a result the last 2 parts of the returned path
	uxSys.GetDirName(dirName, loadDir.Data());			// will be identical
	uxSys.GetBaseName(baseName2, dirName.Data());		// example: single click returns /a/b/c, double click /a/b/c/c
	if (baseName1.CompareTo(baseName2.Data()) == 0) loadDir = dirName;	// double click: strip off last part
	
	if (!uxSys.IsDirectory(loadDir.Data())) {
		errMsg = "No such directory - ";
		errMsg += loadDir;
		gVMEControlData->MsgBox(this, "RestoreSettings", "Error", errMsg);
		return(kFALSE);
	}
	
	Int_t nerr = 0;

	TIterator * iter = fLofSelected.MakeIterator();
	TMrbNamedX * module;
	Int_t bit = 0x1;
	Int_t nofModules = 0;
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, "Saving module settings ...", 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, fLofSelected.GetEntries());
	while (module = (TMrbNamedX *) iter->Next()) {
		if (modSel & bit) {
			TString settingsFile = loadDir;
			settingsFile += "/";
			settingsFile += module->GetName();
			settingsFile += ".cfg";
			TC2LSis3302 * sis3302 = (TC2LSis3302 *)	module->GetAssignedObject();
			if (!sis3302->RestoreSettings(settingsFile.Data())) nerr++;
			nofModules++;
			pgb->Increment(1, module->GetName());
			bit <<= 1;
		}
		gSystem->ProcessEvents();
	}
	delete pgb;
	
	if (nerr > 0) {
		gVMEControlData->MsgBox(this, "RestoreSettings", "Error", "Restoring module settings failed");
	}

	if (nofModules > 0) {
		gMrbLog->Out()	<< nofModules << " SIS3302 module(s) restored from directory " << loadDir << " ("
						<< nerr << " error(s))" << endl;
		gMrbLog->Flush(this->ClassName(), "RestoreSettings", setblue);
	}

}

Bool_t VMESis3302SaveRestorePanel::SaveSettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302SaveRestorePanel::SaveSettings
// Purpose:        Save settings to file
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Save settings.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGFileInfo fileInfoSave;
	TString errMsg;
			
	Bool_t verbose = gVMEControlData->IsVerbose();

	UInt_t modSel = fModules->GetActive();
	if (modSel == 0) {
		gVMEControlData->MsgBox(this, "SaveSettings", "Error", "No modules selected");
		return(kFALSE);
	}

	fileInfoSave.fFileTypes = (const Char_t **) kVMESis3302FileTypes;
	if (!gVMEControlData->CheckAccess(gVMEControlData->fSettingsPath.Data(),
						(Int_t) VMEControlData::kVMEAccessDirectory | VMEControlData::kVMEAccessWrite, errMsg)) {
		fileInfoSave.fIniDir = StrDup(gSystem->WorkingDirectory());
		errMsg += "\nFalling back to \"";
		errMsg += gSystem->WorkingDirectory();
		errMsg += "\"";
		gVMEControlData->MsgBox(this, "SaveSettings", "Warning", errMsg);
	} else {
		fileInfoSave.fIniDir = StrDup(gVMEControlData->fSettingsPath);
	}

	new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fileInfoSave);
	if (fileInfoSave.fFilename == NULL || *fileInfoSave.fFilename == '\0') return(kFALSE);
	TString saveDir = fileInfoSave.fFilename;

	TString baseName1, baseName2, dirName;				// check if user did a double click
	TMrbSystem uxSys;
	uxSys.GetBaseName(baseName1, saveDir.Data());		// as a result the last 2 parts of the returned path
	uxSys.GetDirName(dirName, saveDir.Data());			// will be identical
	uxSys.GetBaseName(baseName2, dirName.Data());		// example: single click returns /a/b/c, double click /a/b/c/c
	if (baseName1.CompareTo(baseName2.Data()) == 0) saveDir = dirName;	// double click: strip off last part
	
	if (!uxSys.Exists(saveDir.Data())) {
		TString cmd = "mkdir -p ";
		cmd += saveDir;
		gSystem->Exec(cmd);
		if (verbose) {
			gMrbLog->Out() << "Creating directory - " << saveDir << endl;
			gMrbLog->Flush(this->ClassName(), "SaveSettings", setblue);
		}
	}
	
	if (!uxSys.IsDirectory(saveDir.Data())) {
		errMsg = "File \"";
		errMsg += saveDir.Data();
		errMsg += "\" is NOT a directory";
		gVMEControlData->MsgBox(this, "SaveSettings", "Error", errMsg);
		return(kFALSE);
	}

	Int_t nerr = 0;

	TIterator * iter = fLofSelected.MakeIterator();
	TMrbNamedX * module;
	Int_t bit = 0x1;
	Int_t nofModules = 0;
	TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, "Saving module settings ...", 400, "blue", NULL, kTRUE);
	pgb->SetRange(0, fLofSelected.GetEntries());
	while (module = (TMrbNamedX *) iter->Next()) {
		if (modSel & bit) {
			TString settingsFile = saveDir;
			settingsFile += "/";
			settingsFile += module->GetName();
			settingsFile += ".cfg";
			TC2LSis3302 * sis3302 = (TC2LSis3302 *)	module->GetAssignedObject();
			if (!sis3302->SaveSettings(settingsFile.Data())) nerr++;
			nofModules++;
			pgb->Increment(1, module->GetName());
			bit <<= 1;
		}
		gSystem->ProcessEvents();
	}
	delete pgb;
	
	if (nerr > 0) {
		gVMEControlData->MsgBox(this, "SaveSettings", "Error", "Saving module settings failed");
	}

	if (nofModules > 0) {
		gMrbLog->Out()	<< nofModules << " SIS3302 module(s) saved to directory " << saveDir << " ("
						<< nerr << " error(s))" << endl;
		gMrbLog->Flush(this->ClassName(), "SaveSettings", setblue);
	}

	return(kTRUE);
}
