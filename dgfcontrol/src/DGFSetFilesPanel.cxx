//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFSetFilesPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Set Filenames
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
		{DGFSetFilesPanel::kDGFSetFilesClose,		"Close",	"Close window"				},
		{0, 										NULL		}
	};

extern DGFControlData * gDGFControlData;

ClassImp(DGFSetFilesPanel)

DGFSetFilesPanel::DGFSetFilesPanel(const TGWindow * Window, UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFSetFilesPanel
// Purpose:        Define file names and paths
// Arguments:      TGWindow Window      -- connection to ROOT graphics
//                 TGWindow * MainFrame -- main frame
//                 UInt_t Width         -- window width in pixels
//                 UInt_t Height        -- window height in pixels
//                 UInt_t Options       -- options
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
																DGFSetFilesPanel::kLEWidth,
																DGFSetFilesPanel::kLEHeight,
																DGFSetFilesPanel::kEntryWidth,
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
																DGFSetFilesPanel::kLEWidth,
																DGFSetFilesPanel::kLEHeight,
																DGFSetFilesPanel::kEntryWidth,
																&fSystemDSPCodeFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemDSPCodeEntry);
	fSystemFrame->AddFrame(fSystemDSPCodeEntry, frameGC->LH());
	fSystemDSPCodeEntry->GetEntry()->SetText(gDGFControlData->fDSPCodeFile.Data());

	fSystemDSPParamsFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesDSPParams;
	fSystemDSPParamsFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemDSPParamsEntry = new TGMrbFileEntry(fSystemFrame, "DSP Params Table",
																200, kDGFSetFilesSystemDSPParams,
																DGFSetFilesPanel::kLEWidth,
																DGFSetFilesPanel::kLEHeight,
																DGFSetFilesPanel::kEntryWidth,
																&fSystemDSPParamsFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemDSPParamsEntry);
	fSystemFrame->AddFrame(fSystemDSPParamsEntry, frameGC->LH());
	fSystemDSPParamsEntry->GetEntry()->SetText(gDGFControlData->fDSPParamsFile.Data());

	fSystemSystemFPGAFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemSystemFPGAFileInfo.fIniDir = StrDup(gDGFControlData->fLoadPath);
	fSystemSystemFPGAConfigEntry = new TGMrbFileEntry(fSystemFrame, "FPGA Configuration [System]",
																200, kDGFSetFilesSystemSystemFPGAConfig,
																DGFSetFilesPanel::kLEWidth,
																DGFSetFilesPanel::kLEHeight,
																DGFSetFilesPanel::kEntryWidth,
																&fSystemSystemFPGAFileInfo, kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemSystemFPGAConfigEntry);
	fSystemFrame->AddFrame(fSystemSystemFPGAConfigEntry, frameGC->LH());
	fSystemSystemFPGAConfigEntry->GetEntry()->SetText(gDGFControlData->fSystemFPGAConfigFile.Data());

	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD].fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD].fIniDir = StrDup(gDGFControlData->fLoadPath);

	fSystemFippiFPGAConfig[TMrbDGFData::kRevD] = new TGMrbFileEntry(fSystemFrame, "FPGA Configuration [Fippi] RevD",
																200, kDGFSetFilesSystemFippiFPGAConfig,
																DGFSetFilesPanel::kLEWidth,
																DGFSetFilesPanel::kLEHeight,
																DGFSetFilesPanel::kEntryWidth,
																&fSystemFippiFPGAFileInfo[TMrbDGFData::kRevD], kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemFippiFPGAConfig[TMrbDGFData::kRevD]);
	fSystemFrame->AddFrame(fSystemFippiFPGAConfig[TMrbDGFData::kRevD], frameGC->LH());
	fSystemFippiFPGAConfig[TMrbDGFData::kRevD]->GetEntry()->SetText(gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevD].Data());

	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE].fFileTypes = (const Char_t **) kDGFFileTypesFPGACode;
	fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE].fIniDir = StrDup(gDGFControlData->fLoadPath);

	fSystemFippiFPGAConfig[TMrbDGFData::kRevE] = new TGMrbFileEntry(fSystemFrame, "FPGA Configuration [Fippi] RevE",
																200, kDGFSetFilesSystemFippiFPGAConfig,
																DGFSetFilesPanel::kLEWidth,
																DGFSetFilesPanel::kLEHeight,
																DGFSetFilesPanel::kEntryWidth,
																&fSystemFippiFPGAFileInfo[TMrbDGFData::kRevE], kFDOpen,
																frameGC, labelGC, entryGC);
	HEAP(fSystemFippiFPGAConfig[TMrbDGFData::kRevE]);
	fSystemFrame->AddFrame(fSystemFippiFPGAConfig[TMrbDGFData::kRevE], frameGC->LH());
	fSystemFippiFPGAConfig[TMrbDGFData::kRevE]->GetEntry()->SetText(gDGFControlData->fFippiFPGAConfigFile[TMrbDGFData::kRevE].Data());

//	buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	labelGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TGMrbTextButtonList(this, NULL, &fLofButtons, 1,
												DGFSetFilesPanel::kButtonWidth,
												DGFSetFilesPanel::kButtonHeight,
												frameGC, NULL, labelGC);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, labelGC->LH());
	fButtonFrame->Associate(this);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionClose);
	
	SetWindowName("DGFControl: SetFilesPanel");

	MapSubwindows();

	Resize(GetDefaultSize());
	Resize(Width, Height);

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
							this->CloseWindow();
							break;
						case kDGFSetFilesClose:
							this->CloseWindow();
							break;
						default:	break;
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
