//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFSetFilesPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Set Filenames
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFSetFilesPanel.cxx,v 1.5 2004-09-28 13:47:32 rudi Exp $       
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
	fUserRunDataEntry->GetEntry()->SetText(gDGFControlData->fRunDataFile.Data());

//	system defs
	TGLayoutHints * systemLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	gDGFControlData->SetLH(groupGC, frameGC, systemLayout);
	HEAP(systemLayout);
	fSystemFrame = new TGGroupFrame(this, "System", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSystemFrame);
	this->AddFrame(fSystemFrame, systemLayout);

	fSystemDSPCodeFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPCode;
	fSystemDSPCodeFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemDSPCodeEntry = new TGMrbFileEntry(fSystemFrame, "DSP Code File",
																200, kDGFSetFilesSystemDSPCode,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemDSPCodeFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemDSPCodeEntry);
	fSystemFrame->AddFrame(fSystemDSPCodeEntry, frameGC->LH());
	fSystemDSPCodeEntry->GetEntry()->SetText(gDGFControlData->fDSPCodeFile.Data());

	fSystemDSPParamsFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPParams;
	fSystemDSPParamsFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemDSPParamsEntry = new TGMrbFileEntry(fSystemFrame, "DSP Params Table",
																200, kDGFSetFilesSystemDSPParams,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemDSPParamsFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemDSPParamsEntry);
	fSystemFrame->AddFrame(fSystemDSPParamsEntry, frameGC->LH());
	fSystemDSPParamsEntry->GetEntry()->SetText(gDGFControlData->fDSPParamsFile.Data());

	fSystemSystemFPGAFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemSystemFPGAFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemSystemFPGAConfigEntry = new TGMrbFileEntry(fSystemFrame, "FPGA Configuration [System]",
																200, kDGFSetFilesSystemSystemFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemSystemFPGAFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemSystemFPGAConfigEntry);
	fSystemFrame->AddFrame(fSystemSystemFPGAConfigEntry, frameGC->LH());
	fSystemSystemFPGAConfigEntry->GetEntry()->SetText(gDGFControlData->fSystemFPGAConfigFile.Data());

	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD].fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD].fIniDir = StrDup(gDGFControlData->fLoadPath);

	fSystemFippiFPGAConfig[TMrbDGFData::kRevD] = new TGMrbFileEntry(fSystemFrame, "FPGA Configuration [Fippi] RevD",
																200, kDGFSetFilesSystemFippiFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD], kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemFippiFPGAConfig[TMrbDGFData::kRevD]);
	fSystemFrame->AddFrame(fSystemFippiFPGAConfig[TMrbDGFData::kRevD], frameGC->LH());
	fSystemFippiFPGAConfig[TMrbDGFData::kRevD]->GetEntry()->SetText(gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevD].Data());

	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE].fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE].fIniDir = StrDup(gDGFControlData->fLoadPath);

	fSystemFippiFPGAConfig[TMrbDGFData::kRevE] = new TGMrbFileEntry(fSystemFrame, "FPGA Configuration [Fippi] RevE",
																200, kDGFSetFilesSystemFippiFPGAConfig,
																kLEWidth,
																kLEHeight,
																5 * kEntryWidth,
																&fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE], kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemFippiFPGAConfig[TMrbDGFData::kRevE]);
	fSystemFrame->AddFrame(fSystemFippiFPGAConfig[TMrbDGFData::kRevE], frameGC->LH());
	fSystemFippiFPGAConfig[TMrbDGFData::kRevE]->GetEntry()->SetText(gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevE].Data());

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
	
	fButtonFrame = new TGMrbTextButtonList(fActionFrame, NULL, &fLofButtons, 1,
												kButtonWidth,
												kButtonHeight,
												frameGC, NULL, labelGC);
	HEAP(fButtonFrame);
	fActionFrame->AddFrame(fButtonFrame, labelGC->LH());
	fButtonFrame->Associate(this);

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t DGFSetFilesPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetFilesPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFSetFilesPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					switch (Param1) {
						case kDGFSetFilesApply:
							gDGFControlData->fDSPCodeFile = fSystemDSPCodeEntry->GetEntry()->GetText();
							gDGFControlData->fDSPParamsFile = fSystemDSPParamsEntry->GetEntry()->GetText();
							gDGFControlData->fSystemFPGAConfigFile = fSystemSystemFPGAConfigEntry->GetEntry()->GetText();
							gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevD] = fSystemFippiFPGAConfig[TMrbDGFData::kRevD]->GetEntry()->GetText();
							gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevE] = fSystemFippiFPGAConfig[TMrbDGFData::kRevE]->GetEntry()->GetText();
							gDGFControlData->fRunDataFile = fUserRunDataEntry->GetEntry()->GetText();
							break;
						default:	break;
					}
					break;
				default:	break;
			}
			break;

	}
	return(kTRUE);
}
