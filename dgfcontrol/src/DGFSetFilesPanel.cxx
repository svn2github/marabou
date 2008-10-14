//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFSetFilesPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Set Filenames
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFSetFilesPanel.cxx,v 1.9 2008-10-14 10:22:29 Marabou Exp $       
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

	TGMrbLayout * frameGC;
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
								gDGFControlData->fColorWhite);	HEAP(entryGC);

	this->ChangeBackground(gDGFControlData->fColorGreen);

//	Initialize button list
	fLofButtons.SetName("Buttons");
	fLofButtons.AddNamedX(kDGFSetFilesButtons);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);

//	user defs
	TGLayoutHints * userLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	HEAP(userLayout);
	fUserFrame = new TGGroupFrame(this, "User", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fUserFrame);
	this->AddFrame(fUserFrame, userLayout);

	TGLayoutHints * frameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 1);
	frameGC->SetLH(frameLayout);
	HEAP(frameLayout);
	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 3, 1);
	labelGC->SetLH(labelLayout);
	HEAP(labelLayout);
	TGLayoutHints * entryLayout = new TGLayoutHints(kLHintsRight, 5, 1, 3, 1);
	entryGC->SetLH(entryLayout);
	HEAP(entryLayout);

	fUserRunDataEntry = new TGMrbLabelEntry(fUserFrame, "Run Data File",
																200, kDGFSetFilesUserRunData,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																frameGC, labelGC, entryGC);
	HEAP(fUserRunDataEntry);
	fUserFrame->AddFrame(fUserRunDataEntry, frameGC->LH());
	fUserRunDataEntry->SetText(gDGFControlData->fRunDataFile.Data());

//	dgf defs
	TGLayoutHints * systemLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	gDGFControlData->SetLH(groupGC, frameGC, systemLayout);
	HEAP(systemLayout);
	fSystemDgfFrame = new TGGroupFrame(this, "System (DGF)", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSystemDgfFrame);
	this->AddFrame(fSystemDgfFrame, systemLayout);

	fSystemDSPCodeFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPCode;
	fSystemDSPCodeFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemDSPCodeEntry = new TGMrbFileEntry(fSystemDgfFrame, "DSP Code File",
																200, kDGFSetFilesSystemDSPCode,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemDSPCodeFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemDSPCodeEntry);
	fSystemDgfFrame->AddFrame(fSystemDSPCodeEntry, frameGC->LH());
	fSystemDSPCodeEntry->SetText(gDGFControlData->fDSPCodeFile.Data());

	fSystemDSPParamsFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPParams;
	fSystemDSPParamsFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemDSPParamsEntry = new TGMrbFileEntry(fSystemDgfFrame, "DSP Params Table",
																200, kDGFSetFilesSystemDSPParams,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemDSPParamsFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemUPSAParamsEntry);
	fSystemDgfFrame->AddFrame(fSystemDSPParamsEntry, frameGC->LH());
	fSystemDSPParamsEntry->SetText(gDGFControlData->fDSPParamsFile.Data());

	fSystemUPSAParamsFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPParams;
	fSystemUPSAParamsFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemUPSAParamsEntry = new TGMrbFileEntry(fSystemDgfFrame, "DSP Params Extension (User PSA)",
																200, kDGFSetFilesSystemUPSAParams,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemUPSAParamsFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemUPSAParamsEntry);
	fSystemDgfFrame->AddFrame(fSystemUPSAParamsEntry, frameGC->LH());
	fSystemUPSAParamsEntry->SetText(gDGFControlData->fUPSAParamsFile.Data());

	fSystemSystemFPGAFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemSystemFPGAFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemSystemFPGAConfigEntry = new TGMrbFileEntry(fSystemDgfFrame, "FPGA Configuration [System]",
																200, kDGFSetFilesSystemSystemFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemSystemFPGAFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemSystemFPGAConfigEntry);
	fSystemDgfFrame->AddFrame(fSystemSystemFPGAConfigEntry, frameGC->LH());
	fSystemSystemFPGAConfigEntry->SetText(gDGFControlData->fSystemFPGAConfigFile.Data());

	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD].fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD].fIniDir = StrDup(gDGFControlData->fLoadPath);

	fSystemFippiFPGAConfig[TMrbDGFData::kRevD] = new TGMrbFileEntry(fSystemDgfFrame, "FPGA Configuration [Fippi] RevD",
																200, kDGFSetFilesSystemFippiFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD], kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemFippiFPGAConfig[TMrbDGFData::kRevD]);
	fSystemDgfFrame->AddFrame(fSystemFippiFPGAConfig[TMrbDGFData::kRevD], frameGC->LH());
	fSystemFippiFPGAConfig[TMrbDGFData::kRevD]->SetText(gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevD].Data());

	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE].fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE].fIniDir = StrDup(gDGFControlData->fLoadPath);

	fSystemFippiFPGAConfig[TMrbDGFData::kRevE] = new TGMrbFileEntry(fSystemDgfFrame, "FPGA Configuration [Fippi] RevE",
																200, kDGFSetFilesSystemFippiFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE], kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemFippiFPGAConfig[TMrbDGFData::kRevE]);
	fSystemDgfFrame->AddFrame(fSystemFippiFPGAConfig[TMrbDGFData::kRevE], frameGC->LH());
	fSystemFippiFPGAConfig[TMrbDGFData::kRevE]->SetText(gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevE].Data());

	fSystemDgfSettingsPathInfo.fFileTypes = (const Char_t **) kDGFFileTypesAll;
	fSystemDgfSettingsPathInfo.fIniDir = StrDup(gDGFControlData->fDgfSettingsPath);
	fSystemDgfSettingsEntry = new TGMrbFileEntry(fSystemDgfFrame, "DGF Param Settings",
																200, kDGFSetFilesSystemDgfSettings,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemDgfSettingsPathInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemDgfSettingsEntry);
	fSystemDgfFrame->AddFrame(fSystemDgfSettingsEntry, frameGC->LH());
	fSystemDgfSettingsEntry->SetText(gDGFControlData->fDgfSettingsPath.Data());

//	cptm defs
	fSystemCptmFrame = new TGGroupFrame(this, "System (CPTM)", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSystemCptmFrame);
	this->AddFrame(fSystemCptmFrame, systemLayout);

	fSystemCptmCodeFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesCptmCode;
	TMrbSystem ux;
	TString cptmLoadPath;
	ux.GetDirName(cptmLoadPath, gDGFControlData->fCptmCodeFile);
	fSystemCptmCodeFileInfo.fIniDir = StrDup(cptmLoadPath);
	fSystemCptmCodeEntry = new TGMrbFileEntry(fSystemCptmFrame, "CPTM Code File",
																200, kDGFSetFilesSystemCptmCode,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemCptmCodeFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemCptmCodeEntry);
	fSystemCptmFrame->AddFrame(fSystemCptmCodeEntry, frameGC->LH());
	fSystemCptmCodeEntry->SetText(gDGFControlData->fCptmCodeFile.Data());

	fSystemCptmSettingsPathInfo.fFileTypes = (const Char_t **) kDGFFileTypesAll;
	fSystemCptmSettingsPathInfo.fIniDir = StrDup(gDGFControlData->fCptmSettingsPath);
	fSystemCptmSettingsEntry = new TGMrbFileEntry(fSystemCptmFrame, "CPTM Settings",
																200, kDGFSetFilesSystemCptmSettings,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemCptmSettingsPathInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemCptmSettingsEntry);
	fSystemCptmFrame->AddFrame(fSystemCptmSettingsEntry, frameGC->LH());
	fSystemCptmSettingsEntry->SetText(gDGFControlData->fCptmSettingsPath.Data());

//	buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);

	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	labelGC->SetLH(btnLayout);
	HEAP(btnLayout);

	fActionFrame = new TGGroupFrame(this, "Actions", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, groupGC->LH());
	
	fButtonFrame = new TGMrbTextButtonList(fActionFrame, NULL, &fLofButtons, -1, 1,
												kButtonWidth,
												kButtonHeight,
												frameGC, NULL, labelGC);
	HEAP(fButtonFrame);
	fActionFrame->AddFrame(fButtonFrame, labelGC->LH());
	((TGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);

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
