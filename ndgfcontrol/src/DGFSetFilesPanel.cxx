//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFSetFilesPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Set Filenames
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFSetFilesPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFSetFilesPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"

#include "TMrbSystem.h"

#include "DGFControlData.h"
#include "DGFSetFilesPanel.h"

static Char_t * kDGFFileTypesDSPParams[]	=	{
												"DSP params", 			"dgf*.var",
												"All files",			"*",
												NULL,					NULL
											};

static Char_t * kDGFFileTypesFPGACode[]	=	{
												"FPGA code (BIN)", 		"*dgf*.bin",
												"FPGA code (ASCII)", 	"*dgf*.fip",
												"All files",			"*",
												NULL,					NULL
											};

static Char_t * kDGFFileTypesDSPCode[]	=	{
												"DSP code (BIN)", 		"dgf*.bin",
												"DSP code (ASCII)", 	"dgf*.exe",
												"All files",			"*",
												NULL,					NULL
											};

static Char_t * kDGFFileTypesCptmCode[]	=	{
												"CPTM code", 			"*.rbf",
												"All files",			"*",
												NULL,					NULL
											};

static Char_t * kDGFFileTypesAll[]	=	{
												"All files",			"*",
												NULL,					NULL
											};

const SMrbNamedX kDGFSetFilesButtons[] =
	{
		{DGFSetFilesPanel::kDGFSetFilesApply,		"Apply",	"Apply current settings"	},
		{0, 										NULL		}
	};

extern DGFControlData * gDGFControlData;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

ClassImp(DGFSetFilesPanel)

DGFSetFilesPanel::DGFSetFilesPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetFilesPanel
// Purpose:        Define file names and paths
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////// Profiles Laden////////////////////////////////////////

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
// 	stdProfile = gMrbLofProfiles->GetDefault();
	//yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	//blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);
	stdProfile = gMrbLofProfiles->FindProfile("green", kTRUE);

	gMrbLofProfiles->Print();

////////////////////////////////////////////////////////////////////////////////////////////////
/*	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * entryGC;


	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(labelGC);

	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);*/

// 	this->ChangeBackground(gDGFControlData->fColorGreen);

//	Initialize button list
	fLofButtons.SetName("Buttons");
	fLofButtons.AddNamedX(kDGFSetFilesButtons);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);
//  	TabFrame->AddFrame(this, dgfFrameLayout);

//	user defs
	TGLayoutHints * userLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	HEAP(userLayout);
	fUserFrame = new TGGroupFrame(this, "User", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fUserFrame);
	this->AddFrame(fUserFrame, userLayout);

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 1);
// 	frameGC->SetLH(frameLayout);
	HEAP(frameLayout);
	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 1);
// 	labelGC->SetLH(labelLayout);
	HEAP(labelLayout);
	TGLayoutHints * entryLayout = new TGLayoutHints(kLHintsRight, 5, 1, 3, 1);
// 	entryGC->SetLH(entryLayout);
	HEAP(entryLayout);

	fUserRunDataEntry = new TNGMrbLabelEntry(fUserFrame, "Run Data File",
																kDGFSetFilesUserRunData,stdProfile,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,TNGMrbLabelEntry::kGMrbEntryIsText);
	HEAP(fUserRunDataEntry);
	fUserFrame->AddFrame(fUserRunDataEntry, frameLayout);
	fUserRunDataEntry->SetText(gDGFControlData->fRunDataFile.Data());

//	dgf defs
	TGLayoutHints * systemLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, systemLayout);
	HEAP(systemLayout);
	fSystemDgfFrame = new TGGroupFrame(this, "System (DGF)", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fSystemDgfFrame);
	this->AddFrame(fSystemDgfFrame, systemLayout);

	fSystemDSPCodeFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPCode;
	fSystemDSPCodeFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemDSPCodeEntry = new TNGMrbFileEntry(fSystemDgfFrame, "DSP Code File",
																&fSystemDSPCodeFileInfo,kFDOpen,stdProfile, kDGFSetFilesSystemDSPCode,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemDSPCodeEntry);
	fSystemDgfFrame->AddFrame(fSystemDSPCodeEntry, systemLayout);
	fSystemDSPCodeEntry->SetText(gDGFControlData->fDSPCodeFile.Data());

	fSystemDSPParamsFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPParams;
	fSystemDSPParamsFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemDSPParamsEntry = new TNGMrbFileEntry(fSystemDgfFrame, "DSP Params Table",
																&fSystemDSPParamsFileInfo, kFDOpen, stdProfile,kDGFSetFilesSystemDSPParams,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemUPSAParamsEntry);
	fSystemDgfFrame->AddFrame(fSystemDSPParamsEntry, systemLayout);
	fSystemDSPParamsEntry->SetText(gDGFControlData->fDSPParamsFile.Data());

	fSystemUPSAParamsFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPParams;
	fSystemUPSAParamsFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemUPSAParamsEntry = new TNGMrbFileEntry(fSystemDgfFrame, "DSP Params Extension (User PSA)",
																&fSystemUPSAParamsFileInfo, kFDOpen,stdProfile,kDGFSetFilesSystemUPSAParams,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemUPSAParamsEntry);
	fSystemDgfFrame->AddFrame(fSystemUPSAParamsEntry, systemLayout);
	fSystemUPSAParamsEntry->SetText(gDGFControlData->fUPSAParamsFile.Data());

	fSystemSystemFPGAFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemSystemFPGAFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemSystemFPGAConfigEntry = new TNGMrbFileEntry(fSystemDgfFrame, "FPGA Configuration [System]",
																&fSystemSystemFPGAFileInfo,kFDOpen,stdProfile, kDGFSetFilesSystemSystemFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemSystemFPGAConfigEntry);
	fSystemDgfFrame->AddFrame(fSystemSystemFPGAConfigEntry, systemLayout);
	fSystemSystemFPGAConfigEntry->SetText(gDGFControlData->fSystemFPGAConfigFile.Data());

	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD].fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD].fIniDir = StrDup(gDGFControlData->fLoadPath);

	fSystemFippiFPGAConfig[TMrbDGFData::kRevD] = new TNGMrbFileEntry(fSystemDgfFrame, "FPGA Configuration [Fippi] RevD",
																&fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD],kFDOpen,stdProfile, kDGFSetFilesSystemFippiFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemFippiFPGAConfig[TMrbDGFData::kRevD]);
	fSystemDgfFrame->AddFrame(fSystemFippiFPGAConfig[TMrbDGFData::kRevD], systemLayout);
	fSystemFippiFPGAConfig[TMrbDGFData::kRevD]->SetText(gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevD].Data());

	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE].fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE].fIniDir = StrDup(gDGFControlData->fLoadPath);

	fSystemFippiFPGAConfig[TMrbDGFData::kRevE] = new TNGMrbFileEntry(fSystemDgfFrame, "FPGA Configuration [Fippi] RevE",
																&fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE],kFDOpen,stdProfile, kDGFSetFilesSystemFippiFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemFippiFPGAConfig[TMrbDGFData::kRevE]);
	fSystemDgfFrame->AddFrame(fSystemFippiFPGAConfig[TMrbDGFData::kRevE], systemLayout);
	fSystemFippiFPGAConfig[TMrbDGFData::kRevE]->SetText(gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevE].Data());

	fSystemDgfSettingsPathInfo.fFileTypes = (const Char_t **) kDGFFileTypesAll;
	fSystemDgfSettingsPathInfo.fIniDir = StrDup(gDGFControlData->fDgfSettingsPath);
	fSystemDgfSettingsEntry = new TNGMrbFileEntry(fSystemDgfFrame, "DGF Param Settings",
																&fSystemDgfSettingsPathInfo,kFDOpen,stdProfile, kDGFSetFilesSystemDgfSettings,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemDgfSettingsEntry);
	fSystemDgfFrame->AddFrame(fSystemDgfSettingsEntry, systemLayout);
	fSystemDgfSettingsEntry->SetText(gDGFControlData->fDgfSettingsPath.Data());

//	cptm defs
	fSystemCptmFrame = new TGGroupFrame(this, "System (CPTM)", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fSystemCptmFrame);
	this->AddFrame(fSystemCptmFrame, systemLayout);

	fSystemCptmCodeFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesCptmCode;
	TMrbSystem ux;
	TString cptmLoadPath;
	ux.GetDirName(cptmLoadPath, gDGFControlData->fCptmCodeFile);
	fSystemCptmCodeFileInfo.fIniDir = StrDup(cptmLoadPath);
	fSystemCptmCodeEntry = new TNGMrbFileEntry(fSystemCptmFrame, "CPTM Code File",
																&fSystemCptmCodeFileInfo,kFDOpen,stdProfile, kDGFSetFilesSystemCptmCode,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemCptmCodeEntry);
	fSystemCptmFrame->AddFrame(fSystemCptmCodeEntry, systemLayout);
	fSystemCptmCodeEntry->SetText(gDGFControlData->fCptmCodeFile.Data());

	fSystemCptmSettingsPathInfo.fFileTypes = (const Char_t **) kDGFFileTypesAll;
	fSystemCptmSettingsPathInfo.fIniDir = StrDup(gDGFControlData->fCptmSettingsPath);
	fSystemCptmSettingsEntry = new TNGMrbFileEntry(fSystemCptmFrame, "CPTM Settings",
																&fSystemCptmSettingsPathInfo, kFDOpen,stdProfile,kDGFSetFilesSystemCptmSettings,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth);
	HEAP(fSystemCptmSettingsEntry);
	fSystemCptmFrame->AddFrame(fSystemCptmSettingsEntry, systemLayout);
	fSystemCptmSettingsEntry->SetText(gDGFControlData->fCptmSettingsPath.Data());

//	buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);

	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
// 	labelGC->SetLH(btnLayout);
	HEAP(btnLayout);

	fActionFrame = new TGGroupFrame(this, "Actions", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, aFrameLayout);
	
	fButtonFrame = new TNGMrbTextButtonList(fActionFrame, NULL, &fLofButtons, -1,stdProfile, 1,0,
												kButtonWidth,
												kButtonHeight);
	HEAP(fButtonFrame);
	fActionFrame->AddFrame(fButtonFrame, btnLayout);
	((TNGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	dgfFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);
	this->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	TabFrame->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFSetFilesPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetFilesPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFSetFilesApply:
			gDGFControlData->fDSPCodeFile = fSystemDSPCodeEntry->GetText();
			gDGFControlData->fDSPParamsFile = fSystemDSPParamsEntry->GetText();
			gDGFControlData->fSystemFPGAConfigFile = fSystemSystemFPGAConfigEntry->GetText();
			gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevD] = fSystemFippiFPGAConfig[TMrbDGFData::kRevD]->GetText();
			gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevE] = fSystemFippiFPGAConfig[TMrbDGFData::kRevE]->GetText();
			gDGFControlData->fDgfSettingsPath = fSystemDgfSettingsEntry->GetText();
			gDGFControlData->fCptmSettingsPath = fSystemCptmSettingsEntry->GetText();
			gDGFControlData->fCptmCodeFile = fSystemCptmCodeEntry->GetText();
			gDGFControlData->fRunDataFile = fUserRunDataEntry->GetText();
			break;
	}
}
		
