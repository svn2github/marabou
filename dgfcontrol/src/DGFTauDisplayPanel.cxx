//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFTauDisplayPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Tau Display
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFTauDisplayPanel.cxx,v 1.15 2008-10-14 17:27:06 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFTauDisplayPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"

#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbHistory.h"

#include "TMrbDGFEventBuffer.h"

#include "DGFControlData.h"
#include "DGFTauDisplayPanel.h"

#include "SetColor.h"

static Char_t * kDGFFileTypesTraces[]	=	{	"Trace files", 			"dgfTrace.*.root",
												"ROOT files",			"*.root",
												"All files",			"*",
												NULL,					NULL
											};


const SMrbNamedX kDGFTauButtons[] =
			{
				{DGFTauDisplayPanel::kDGFTauButtonAcquire, 		"Acquire trace(s)", "Start run to collect traces"	},
				{DGFTauDisplayPanel::kDGFTauButtonSaveTrace,	"Save trace",		"Save trace data to file"	},
				{DGFTauDisplayPanel::kDGFTauButtonTauOK, 		"Current tau OK",	"Use current tau for corrections" },
				{DGFTauDisplayPanel::kDGFTauButtonBestTau, 		"Best tau OK",		"Use result of tau fit for corrections" },
				{DGFTauDisplayPanel::kDGFTauButtonRemoveTraces,	"Remove all traces",	"Remove trace files"	},
				{DGFTauDisplayPanel::kDGFTauButtonReset,		"Reset",			"Reset to default values"	},
				{0, 											NULL,				NULL						}
			};

const SMrbNamedXShort kDGFTauFitTraceButtons[] =
							{
								{DGFTauDisplayPanel::kDGFTauFitTraceYes, 		"yes" 	},
								{DGFTauDisplayPanel::kDGFTauFitTraceNo, 		"no" 	},
								{0, 											NULL				}
							};

const SMrbNamedXShort kDGFTauDispStatBoxButtons[] =
							{
								{DGFTauDisplayPanel::kDGFTauDispStatBoxYes, 	"yes" 	},
								{DGFTauDisplayPanel::kDGFTauDispStatBoxNo, 		"no" 	},
								{0, 											NULL				}
							};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

static TString btnText;

ClassImp(DGFTauDisplayPanel)

DGFTauDisplayPanel::DGFTauDisplayPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
///__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel
// Purpose:        DGF Viewer: Calculate tau value
// Arguments:      TGWindow Window      -- connection to ROOT graphics
//                 TGWindow * MainFrame -- main frame
//                 UInt_t Width         -- window width in pixels
//                 UInt_t Height        -- window height in pixels
//                 UInt_t Options       -- options
// Results:        
// Exceptions:     
// Description:    Implements DGF Viewer's TauDisplayPanel
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * rbuttonGC;
	TGMrbLayout * comboGC;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
//	Clear focus list
	fFocusList.Clear();

// graphic layout
	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGold);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGold);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGold);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	rbuttonGC = new TGMrbLayout(gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGold);	HEAP(rbuttonGC);

	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);

//	Initialize several lists
	fTauActions.SetName("Actions");
	fTauActions.AddNamedX(kDGFTauButtons);

	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	fFitTraceButtons.SetName("Fit buttons");
	fFitTraceButtons.AddNamedX(kDGFTauFitTraceButtons);
	fFitTraceButtons.SetPatternMode();

	fDispStatBoxButtons.SetName("Stat box buttons");
	fDispStatBoxButtons.AddNamedX(kDGFTauDispStatBoxButtons);
	fDispStatBoxButtons.SetPatternMode();

	this->ChangeBackground(gDGFControlData->fColorGold);

//	module / channel selection
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, selectLayout);
	HEAP(selectLayout);
	fSelectFrame = new TGGroupFrame(this, "DGF Selection", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, frameGC->LH());

	TGLayoutHints * smfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 10, 1, 10, 1);
	frameGC->SetLH(smfLayout);
	HEAP(smfLayout);
	TGLayoutHints * smlLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	labelGC->SetLH(smlLayout);
	HEAP(smlLayout);
	TGLayoutHints * smcLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
	comboGC->SetLH(smcLayout);
	HEAP(smcLayout);

	Bool_t clearList = kTRUE;
	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		gDGFControlData->CopyKeyList(&fLofModuleKeys, cl, gDGFControlData->GetPatInUse(cl), clearList);
		clearList = kFALSE;
	}
	gDGFControlData->SetSelectedModule(fLofModuleKeys.First()->GetName());	// dgf to start with
	gDGFControlData->SetSelectedChannel(0); 								// channel 0

	fSelectModule = new TGMrbLabelCombo(fSelectFrame,  "Module",
											&fLofModuleKeys,
											DGFTauDisplayPanel::kDGFTauSelectModule, 2,
											kTabWidth, kLEHeight,
											kComboWidth,
											frameGC, labelGC, comboGC, buttonGC, kTRUE);
	HEAP(fSelectModule);
	fSelectFrame->AddFrame(fSelectModule, frameGC->LH());
	fSelectModule->GetComboBox()->Select(gDGFControlData->GetSelectedModuleIndex());
	fSelectModule->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "SelectModule(Int_t, Int_t)");

	TGLayoutHints * scfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 10, 1, 10, 1);
	frameGC->SetLH(scfLayout);
	HEAP(scfLayout);
	TGLayoutHints * sclLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	labelGC->SetLH(sclLayout);
	HEAP(sclLayout);
	TGLayoutHints * scbLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	buttonGC->SetLH(scbLayout);
	HEAP(scbLayout);
	fSelectChannel = new TGMrbRadioButtonList(fSelectFrame,  "Channel", &fLofChannels,
													kDGFTauSelectChannel, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, rbuttonGC);
	HEAP(fSelectChannel);
	fSelectChannel->SetState(gDGFControlData->GetSelectedChannelIndex());
	((TGMrbButtonFrame *) fSelectChannel)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "RadioButtonPressed(Int_t, Int_t)");
	fSelectFrame->AddFrame(fSelectChannel, frameGC->LH());

// trace settings
	TGLayoutHints * traceLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, traceLayout);
	HEAP(traceLayout);
	fTraceFrame = new TGGroupFrame(this, "Trace Settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTraceFrame);
	this->AddFrame(fTraceFrame, frameGC->LH());

	TGLayoutHints * teLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	entryGC->SetLH(teLayout);
	HEAP(teLayout);
	fTraceLengthEntry = new TGMrbLabelEntry(fTraceFrame, "Trace length [samples]",
																200, kDGFTauTraceLength,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fTraceLengthEntry);
	fTraceFrame->AddFrame(fTraceLengthEntry, frameGC->LH());
	fTraceLengthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceLengthEntry->SetText("8001");
	fTraceLengthEntry->SetRange(100, 8001);
	fTraceLengthEntry->SetIncrement(100);
	fTraceLengthEntry->AddToFocusList(&fFocusList);
	fTraceLengthEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fXwaitEntry = new TGMrbLabelEntry(fTraceFrame, "Xwait states",
																200, kDGFTauXwait,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fXwaitEntry);
	fTraceFrame->AddFrame(fXwaitEntry, frameGC->LH());
	fXwaitEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fXwaitEntry->SetText("10");
	fXwaitEntry->SetRange(0, 1000);
	fXwaitEntry->SetIncrement(1);
	fXwaitEntry->AddToFocusList(&fFocusList);

	fNofTracesEntry = new TGMrbLabelEntry(fTraceFrame, "Number of traces",
																200, kDGFTauTraceNofTraces,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fNofTracesEntry);
	fTraceFrame->AddFrame(fNofTracesEntry, frameGC->LH());
	fNofTracesEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fNofTracesEntry->SetText("1");
	fNofTracesEntry->SetRange(0, 99);
	fNofTracesEntry->SetIncrement(1);
	fNofTracesEntry->AddToFocusList(&fFocusList);
	fNofTracesEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

// fit settings
	fHFrame = new TGHorizontalFrame(this, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
	HEAP(fHFrame);
	this->AddFrame(fHFrame, frameGC->LH());
	
	TGLayoutHints * fitLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, fitLayout);
	HEAP(fitLayout);
	fFitFrame = new TGGroupFrame(fHFrame, "Fit Settings", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fFitFrame);
	fHFrame->AddFrame(fFitFrame, frameGC->LH());

	TGLayoutHints * rbLayout = new TGLayoutHints(kLHintsRight, 1, 1, 1, 1);
	rbuttonGC->SetLH(rbLayout);
	HEAP(rbLayout);
	fFitTraceYesNo = new TGMrbRadioButtonList(fFitFrame,  "Fit trace", &fFitTraceButtons, -1, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, rbuttonGC);
	HEAP(fFitTraceYesNo);
	fFitFrame->AddFrame(fFitTraceYesNo, frameGC->LH());

	frameGC->SetLH(fitLayout);
	TGLayoutHints * feLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	entryGC->SetLH(feLayout);
	HEAP(feLayout);
	fFitFromEntry = new TGMrbLabelEntry(fFitFrame, "From sample",
																200, kDGFTauFitFrom,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fFitFromEntry);
	fFitFrame->AddFrame(fFitFromEntry, frameGC->LH());
	fFitFromEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fFitFromEntry->SetText("0");
	fFitFromEntry->SetRange(0, 8001);
	fFitFromEntry->SetIncrement(100);
	fFitFromEntry->AddToFocusList(&fFocusList);
	fFitFromEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fFitToEntry = new TGMrbLabelEntry(fFitFrame, "To sample",
																200, kDGFTauFitTo,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fFitToEntry);
	fFitFrame->AddFrame(fFitToEntry, frameGC->LH());
	fFitToEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fFitToEntry->SetRange(0, 4095);
	fFitToEntry->SetIncrement(100);
	fFitToEntry->AddToFocusList(&fFocusList);
	fFitToEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fFitA0Entry = new TGMrbLabelEntry(fFitFrame, "A0 (const)",
																200, kDGFTauFitParamA0,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fFitA0Entry);
	fFitFrame->AddFrame(fFitA0Entry, frameGC->LH());
	fFitA0Entry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fFitA0Entry->SetRange(0, 10000);
	fFitA0Entry->SetIncrement(100);
	fFitA0Entry->AddToFocusList(&fFocusList);
	fFitA0Entry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fFitA1Entry = new TGMrbLabelEntry(fFitFrame, "A1 (ampl)",
																200, kDGFTauFitParamA1,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fFitA1Entry);
	fFitFrame->AddFrame(fFitA1Entry, frameGC->LH());
	fFitA1Entry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fFitA1Entry->SetRange(0, 20000);
	fFitA1Entry->SetIncrement(100);
	fFitA1Entry->AddToFocusList(&fFocusList);
	fFitA1Entry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fFitA2Entry = new TGMrbLabelEntry(fFitFrame, "A2 (tau)",
																200, kDGFTauFitParamA2,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fFitA2Entry);
	fFitFrame->AddFrame(fFitA2Entry, frameGC->LH());
	fFitA2Entry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fFitA2Entry->SetRange(0, 10000);
	fFitA2Entry->SetIncrement(100);
	fFitA2Entry->AddToFocusList(&fFocusList);
	fFitA2Entry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fFitErrorEntry = new TGMrbLabelEntry(fFitFrame, "Error",
																200, kDGFTauFitError,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fFitErrorEntry);
	fFitFrame->AddFrame(fFitErrorEntry, frameGC->LH());
	fFitErrorEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fFitErrorEntry->SetRange(0, 20);
	fFitErrorEntry->SetIncrement(.5);
	fFitErrorEntry->AddToFocusList(&fFocusList);
	fFitErrorEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fFitChiSquareEntry = new TGMrbLabelEntry(fFitFrame, "Chi square",
																200, kDGFTauFitChiSquare,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fFitChiSquareEntry);
	fFitFrame->AddFrame(fFitChiSquareEntry, frameGC->LH());
	fFitChiSquareEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fFitChiSquareEntry->SetRange(0, 20);
	fFitChiSquareEntry->SetIncrement(.5);
	fFitChiSquareEntry->AddToFocusList(&fFocusList);
	fFitChiSquareEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

// display
	TGLayoutHints * dispLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dispLayout);
	HEAP(dispLayout);
	fDisplayFrame = new TGGroupFrame(fHFrame, "Display", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDisplayFrame);
	fHFrame->AddFrame(fDisplayFrame, frameGC->LH());

	TGLayoutHints * deLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	entryGC->SetLH(deLayout);
	HEAP(deLayout);
	fDispTraceNoEntry = new TGMrbLabelEntry(fDisplayFrame, "Trace number",
																200, kDGFTauDispTraceNo,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fDispTraceNoEntry);
	fDisplayFrame->AddFrame(fDispTraceNoEntry, frameGC->LH());
	fDispTraceNoEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fDispTraceNoEntry->SetText("0");
	fDispTraceNoEntry->SetRange(0, 99);
	fDispTraceNoEntry->SetIncrement(1);
	fDispTraceNoEntry->AddToFocusList(&fFocusList);
	fDispTraceNoEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fDispStatBox = new TGMrbRadioButtonList(fDisplayFrame,  "Stat box", &fDispStatBoxButtons, -1, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, rbuttonGC);
	HEAP(fDispStatBox);
	fDisplayFrame->AddFrame(fDispStatBox, frameGC->LH());

	fTraceFileInfo.fFileTypes = (const Char_t **) kDGFFileTypesTraces;
	fTraceFileInfo.fIniDir = StrDup(gDGFControlData->fDataPath);
	fDispFileEntry = new TGMrbFileEntry(fDisplayFrame, "Trace file",
																200, kDGFTauDispFile,
																kLEWidth,
																kLEHeight,
																kFileEntryWidth,
																&fTraceFileInfo, kFDOpen,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fDispFileEntry);
	fDisplayFrame->AddFrame(fDispFileEntry, frameGC->LH());

	fDispCurTauEntry = new TGMrbLabelEntry(fDisplayFrame, "Tau value",
																200, kDGFTauDispCurTau,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fDispCurTauEntry);
	fDisplayFrame->AddFrame(fDispCurTauEntry, frameGC->LH());
	fDispCurTauEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fDispCurTauEntry->SetText("0.0");
	fDispCurTauEntry->AddToFocusList(&fFocusList);
	fDispCurTauEntry->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EntryChanged(Int_t, Int_t)");

	fDispBestTauEntry = new TGMrbLabelEntry(fDisplayFrame, "Best tau",
																200, kDGFTauDispBestTau,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC);
	HEAP(fDispBestTauEntry);
	fDisplayFrame->AddFrame(fDispBestTauEntry, frameGC->LH());
	fDispBestTauEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fDispBestTauEntry->SetText("0.0");
	fDispBestTauEntry->GetEntry()->SetState(kFALSE);

//	buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TGMrbTextButtonGroup(this, "Actions", &fTauActions, -1, 1, groupGC, buttonGC);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, buttonGC->LH());
	((TGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

// no trace acquisition active
	fIsRunning = kFALSE;
	
// reset trace file
	fTraceFile = NULL;

// initialize histogram presenter
	fFitTrace = NULL;
	fFitTau = NULL;
	
	this->ResetValues();

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFTauDisplayPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Double_t tau;
		
	TMrbDGF * dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	Int_t chn = gDGFControlData->GetSelectedChannel();

	switch (Selection) {
		case kDGFTauButtonAcquire:
			if (fIsRunning) {
				dgf->AbortRun();
				this->SetRunning(kFALSE);
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", "Aborting trace acquisition", kMBIconExclamation);
			} else {
				this->AcquireTraces();
			}
			break;
		case kDGFTauButtonTauOK:
			tau = fDispCurTauEntry->GetText2Double();
			dgf->SetTau(chn, tau);
			break;
		case kDGFTauButtonBestTau:
			tau = fDispBestTauEntry->GetText2Double();
			dgf->SetTau(chn, tau);
			break;
		case kDGFTauButtonRemoveTraces:
			this->RemoveTrace(-1, -1);
			break;
		case kDGFTauButtonSaveTrace:
			new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fTraceFileInfo);
			if (fTraceFileInfo.fFilename != NULL && *fTraceFileInfo.fFilename != '\0') this->SaveTrace(fTraceFileInfo.fFilename);
			break;
		case kDGFTauButtonReset:
			break;
	}
}

void DGFTauDisplayPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::SelectModule
// Purpose:        Slot method: select module
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	gDGFControlData->SetSelectedModuleIndex(Selection);
	TMrbDGF * dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	for (Int_t i = 0; i < TMrbDGFData::kNofChannels; i++) {
		if (dgf->HasTriggerBitSet(i)) {
			gDGFControlData->SetSelectedChannel(i);
			break;
		}
	}
	fSelectChannel->SetState(gDGFControlData->GetSelectedChannelIndex());
}

void DGFTauDisplayPanel::EntryChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::EntryChanged
// Purpose:        Slot method: update after entry changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->Update(Selection);
}

void DGFTauDisplayPanel::RadioButtonPressed(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::RadioButtonPressed
// Purpose:        Signal catcher for radio buttons
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:     
// Description:    Will be called on radio button events.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	gDGFControlData->SetSelectedChannelIndex(Selection);
}

Bool_t DGFTauDisplayPanel::AcquireTraces() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::AcquireTraces
// Purpose:        Start tracing
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Starts accumulation of traces
//                 Writes results (histos + fits) to root file.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Int_t chn, nofWords;
	TMrbString intStr;
	TMrbString dblStr;

	Int_t nofBuffers;
	Int_t nofTraces;
	Int_t traceLength, xwait;
	Int_t nt, nw, bs, tpb;
	Int_t event0;
	Int_t fitFrom, fitTo;
	Double_t fitError, fitA0, fitA1, fitA2;
	Bool_t fitFlag;
	UInt_t chnPattern;

	Int_t nofEvents, nofGoodTaus, totalEvents;
	Double_t tau, tauSum, tauMean;
	
	TMrbString fileName;

	TMrbDGFEventBuffer buffer;
		
	dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	chn = gDGFControlData->GetSelectedChannel();
	chnPattern = gDGFControlData->GetSelectedChannelIndex();

	fileName = this->TraceFileName();

	traceLength = fTraceLengthEntry->GetText2Int();
	xwait = fXwaitEntry->GetText2Int();
	nofTraces = fNofTracesEntry->GetText2Int();

	nw = traceLength + TMrbDGFEventBuffer::kTotalHeaderLength;
	bs = dgf->GetParValue("LOUTBUFFER", kTRUE);
	tpb = bs / nw;
	if (tpb == 0) {
		gMrbLog->Err()	<< "Something went wrong - bufferSize=" << bs
						<< ", traceLength=" << traceLength << " -> eventLength=" << nw
						<< ", tracesPerBuffer=" << tpb << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireTraces");
		return(kFALSE);
	}

	nofBuffers = (nofTraces + tpb - 1) / tpb;
	fDispTraceNoEntry->SetRange(0, nofTraces - 1);

	fFitFromEntry->SetRange(0, traceLength);
	fFitToEntry->SetRange(0, traceLength);

	fitFrom = fFitFromEntry->GetText2Int();
	fitTo = fFitToEntry->GetText2Int();
	if (fitTo > traceLength) {
		fitTo = traceLength;
		fFitToEntry->SetText(fitTo);
	}
	if (fitFrom < 0 || fitTo < 0 || fitFrom >= fitTo) {
		gMrbLog->Err() << "Illegal fit region - [" << fitFrom << "," << fitTo << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireTraces");
		return(kFALSE);
	}
	fFitFromEntry->SetRange(0, traceLength);

	fitError = fFitErrorEntry->GetText2Double();
	fitA0 = fFitA0Entry->GetText2Double();
	fitA1 = fFitA1Entry->GetText2Double();
	fitA2 = fFitA2Entry->GetText2Double();

	cout	<< setblue
			<< this->ClassName() << "::AcquireTraces(): Acquiring " << nofTraces
			<< " trace(s) from " << dgf->GetName() << " (chn " << chn << "), output to file " << this->TraceFileName()
			<< setblack << endl;

	if (fTraceFile) {
		fTraceFile->Close();
		delete fTraceFile;
	}
	fTraceFile = new TFile(fileName.Data(), "RECREATE");
	if (!fTraceFile->IsOpen()) {
		gMrbLog->Err() << "Can't open file - " << fileName << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireTraces");
		delete fTraceFile;
		fTraceFile = NULL;
		return(kFALSE);
	}
	this->SetRunning(kTRUE);
	
	dgf->SaveParams(TMrbDGF::kSaveTrace);			// save current settings

	dgf->ClearChannelMask();
	dgf->ClearTriggerMask();

	fTauDistr = new TH1F("hTauDistr", "Tau distribution", 100, 0., 100.);
	
	nt = nofTraces;
	event0 = 0;
	nofGoodTaus = 0;
	totalEvents = 0;
	tauSum = 0.;
	for (Int_t i = 0; i < nofBuffers; i++) {
		buffer.Reset();
		buffer.SetTraceError(fitError);
		buffer.SetGlobalIndex(i);
		buffer.SetGlobalEventNo(event0);
		nofEvents = 0;
		dgf->SetParValue("MAXEVENTS", (nt > tpb) ? tpb : nt, kTRUE);

		dgf->GetTrace_Init(traceLength, chnPattern, xwait, kTRUE);
		dgf->GetTrace_Start();
		nofWords = dgf->GetTrace_Stop(buffer, 20);

		if (gDGFControlData->IsDebug()) {
			TMrbString fn = dgf->GetName();
			fn += ".chn";
			fn += chn;
			fn += ".evtbuf";
			fn += i;
			fn += ".dat";
			buffer.WriteToFile(fn.Data(), dgf->GetTitle());
		}

		if (nofWords > TMrbDGFEventBuffer::kTotalHeaderLength) {
			nofEvents = buffer.GetNofEvents();
			if (nofEvents > 0) {
				for (Int_t evtNo = 0; evtNo < nofEvents; evtNo++, totalEvents++) {
					fitFlag = (fFitTraceYesNo->GetActive() == kDGFTauFitTraceYes);
					if (fitFlag) {
						tau = buffer.CalcTau(evtNo, chn, 0, fitA0, fitA1, fitA2, fitFrom, fitTo, kFALSE, kTRUE);
//						if (tau > 0 && buffer.Fit()->GetChisquare() < fitChiSquare) 
						if (tau > 0) {
							nofGoodTaus++;
							tauSum += tau;
							fTauDistr->Fill((Axis_t) tau, 1.);
						}
					} else {
						buffer.FillHistogram(evtNo, chn, kFALSE);
					}
					fHistogram = buffer.Histogram();
					if (fHistogram != NULL) {
						this->HistogramName(event0 + evtNo);
						fHistogram->SetName(fHistoName.Data());
						fHistogram->SetTitle(fHistoTitle.Data());
						fHistogram->Write();
					}
				}
				this->BufferName(event0);
				buffer.SetName(fEbuName.Data());
				buffer.Write();
			}
		} else if (dgf->GetWaitStatus() != TMrbDGF::kWaitOk) {
			gMrbLog->Err()	<< dgf->GetName() << " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
							<< ": Unable to acquire traces" << endl;
			gMrbLog->Flush(this->ClassName(), "AcquireTraces");
			this->SetRunning(kFALSE);
			dgf->RestoreParams(TMrbDGF::kSaveTrace);
			return(kFALSE);
		}
					
		event0 += nofEvents;
		nt -= nofEvents;
	}
	
	if (totalEvents == 0) {
		gMrbLog->Err()	<< dgf->GetName() << " in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
						<< ": Unable to acquire traces (all buffers empty)" << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireTraces");
		this->SetRunning(kFALSE);
		dgf->RestoreParams(TMrbDGF::kSaveTrace);
		return(kFALSE);
	}

	if (nofGoodTaus > 0) {
		tauMean = tauSum / nofGoodTaus;
		fGaussian = new TF1("fTauDistr", "gaus", tauMean - 15, tauMean + 15);
		fGaussian->SetParameters(tauSum, tauMean, 3);
		fGaussian->SetParName(0,"gaus_const");
		fGaussian->SetParName(1,"gaus_mean");
		fGaussian->SetParName(2,"gaus_sigma");
		fGaussian->SetLineColor(4);
		fGaussian->SetLineWidth(3);
		fTauDistr->Fit("fTauDistr", "R+", "SAME");
		fTauDistr->Write();
	}
	
	fTraceFile->Close();
	gROOT->cd();
	delete fTraceFile;
	fTraceFile = NULL;
	fHistogram = NULL;
	fTauDistr = NULL;

	dgf->RestoreParams(TMrbDGF::kSaveTrace);			// restore settings

	fDispTraceNoEntry->SetText("0");
	this->Update(kDGFTauDispTraceNo);

	this->SetRunning(kFALSE);
	return(kTRUE);
}

Bool_t DGFTauDisplayPanel::Update(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::Update
// Purpose:        Update program state on X events
// Arguments:      Int_t EntryId      -- entry id
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Updates variables on X events and starts action.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t chn;
	TMrbString intStr, dblStr;
	Int_t traceNo;
	TMrbString fileName;
	TString fhName;
	
	Int_t bufNo;
		
	chn = gDGFControlData->GetSelectedChannel();

	switch (EntryId) {
		case kDGFTauDispTraceNo:
			traceNo = fDispTraceNoEntry->GetText2Int();
			if (fTraceFile == NULL) {
				fileName = fDispFileEntry->GetText();
				fTraceFile = new TFile(fileName.Data());
				if (!fTraceFile->IsOpen()) {
					gMrbLog->Err() << "Can't open file - " << fileName << endl;
					gMrbLog->Flush(this->ClassName(), "Update");
					delete fTraceFile;
					fTraceFile = NULL;
					fHistogram = NULL;
					fTauDistr = NULL;
					return(kFALSE);
				}
				fLofBuffers.Delete();
				bufNo = 0;
				for (;;) {
					fBuffer = (TMrbDGFEventBuffer *) fTraceFile->Get(this->BufferName(bufNo));
					if (fBuffer == NULL) break;
					fLofBuffers.Add(fBuffer);
					bufNo += fBuffer->GetNofEvents();
				}
				
				fTauDistr = (TH1F *) fTraceFile->Get("hTauDistr");
				if (fTauDistr) {
					TString canvas = "fhTauDistr";
					fFitTau = (FitHist *) gROOT->FindObject(canvas.Data());
					if (fFitTau) {
						fFitTau->GetCanvas()->cd();
						fFitTau->SetHist(fTauDistr);
					} else {
						gROOT->cd();
						Window_t wdum;
						Int_t ax, ay, w, h;
						w = this->GetWidth();
						h = this->GetHeight();
						gVirtualX->TranslateCoordinates(	this->GetId(), this->GetParent()->GetId(),
															(Int_t) (w * .125), h / 2,
															ax, ay, wdum);
						fFitTau = new FitHist(				canvas.Data(), fTauDistr->GetTitle(),
															fTauDistr,
															fTauDistr->GetName(),
															ax, ay, (Int_t) (w * .75), h / 2);
						HEAP(fFitTau);
						fTraceFile->cd();
					}
					fGaussian = fTauDistr->GetFunction("fTauDistr");
					if (fGaussian) {
						dblStr = fGaussian->GetParameter(1);
					} else {
						dblStr = "0.0";
					}
					fDispBestTauEntry->GetEntry()->SetState(kTRUE);
					fDispBestTauEntry->SetText(dblStr.Data());
					fDispBestTauEntry->GetEntry()->SetState(kFALSE);
				}
			}

			fBuffer = this->FindBuffer(traceNo);
			
			this->HistogramName(traceNo);

			fHistogram = (TH1F *) fTraceFile->Get(fHistoName.Data());
			if (fHistogram) {
				fHistogram->SetStats(fDispStatBox->GetActive() == kDGFTauDispStatBoxYes);
				fHistogram->SetName("hTrace");
				TString canvas = "fhFitTrace";
				fFitTrace = (FitHist *) gROOT->FindObject(canvas.Data());
				if (fFitTrace) {
					fFitTrace->GetCanvas()->cd();
					fFitTrace->SetHist(fHistogram);
				} else {
					gROOT->cd();
					Window_t wdum;
					Int_t ax, ay, w, h;
					w = this->GetWidth();
					h = this->GetHeight();
					gVirtualX->TranslateCoordinates(	this->GetId(), this->GetParent()->GetId(),
														(Int_t) (w * .125), h / 2,
														ax, ay, wdum);
					fhName = fHistogram->GetName();
					fhName.Prepend("f");
					fFitTrace = new FitHist(			fhName.Data(), fHistogram->GetTitle(),
														fHistogram,
														fHistogram->GetName(),
														ax, ay, (Int_t) (w * .75), h / 2);
					HEAP(fFitTrace);
					fTraceFile->cd();
				}
				if (fFitTraceYesNo->GetActive() == kDGFTauFitTraceYes && fBuffer != NULL) {
					dblStr = fBuffer->GetTau(traceNo - fBuffer->GetGlobalEventNo(), chn);
				} else {
					dblStr = "0.0";
				}
				fDispCurTauEntry->SetText(dblStr.Data());
			} else {
				gMrbLog->Err() << "No such histogram -  " << fHistoName << endl;
				gMrbLog->Flush(this->ClassName(), "Update");
			}
			fTraceFile->Close();
			delete fTraceFile;
			fTraceFile = NULL;
			break;
		case kDGFTauDispFile:
			if (fTraceFile) {
				fTraceFile->Close();
				gROOT->cd();
				delete fTraceFile;
				fTraceFile = NULL;
				fHistogram = NULL;
			}
			break;
	}
	return(kTRUE);
}

Bool_t DGFTauDisplayPanel::ResetValues() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::ResetValues
// Purpose:        Clear values in tau display panel
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Clears entry fields in tau display panel.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fTraceLengthEntry->SetText("8001");
	fXwaitEntry->SetText("10");
	fNofTracesEntry->SetText("1");
	fFitFromEntry->SetText("0");
	fFitToEntry->SetText("200");
	fFitA0Entry->SetText("1000");
	fFitA1Entry->SetText("1000");
	fFitA2Entry->SetText("2000");
	fFitErrorEntry->SetText("2.0");
	fFitChiSquareEntry->SetText("5.0");
	fDispTraceNoEntry->SetText("0");
	fDispCurTauEntry->SetText("0.0");
	fFitTraceYesNo->SetState(kDGFTauFitTraceNo);
	fDispStatBox->SetState(kDGFTauDispStatBoxNo);
	return(kTRUE);
}

Bool_t DGFTauDisplayPanel::RemoveTrace(Int_t ModuleId, Int_t ChannelId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::RemoveTrace
// Purpose:        Remove root file containing trace data
// Arguments:      Int_t ModuleId     -- module id
//                 Int_t ChannelId    -- channel id
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Removes file "dgfTrace.<module>.<channel>.root
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Int_t chn;
	Int_t clearIt;
	TMrbString cmd;

	if (ModuleId == -1) 	// clear all trace files
	{
		new TGMsgBox(fClient->GetRoot(), this,	"DGFControl: Confirm",
												"Remove ALL trace sets (files \"dgfTrace.*.root\")?",
												kMBIconQuestion, kMBCancel | kMBOk, &clearIt);
		if (clearIt != 0) {
			gSystem->Exec("rm -f dgfTrace.*.root");
			gMrbLog->Out() << "File(s) \"dgfTrace.*.root\" removed" << endl;
			gMrbLog->Flush(this->ClassName(), "RemoveTrace", setblue);
			return(kTRUE);
		}
		return(kFALSE);
	}

	dgf = gDGFControlData->GetModule(ModuleId)->GetAddr();

	if (ChannelId == -1) {
		chn = -1;
		cout	<< setblue
			<< "DGFTauDisplayPanel::RemoveTrace(): Removing trace file(s) for module "
			<< dgf->GetName()
			<< setblack << endl;
	} else {
		chn = gDGFControlData->GetChannel(ChannelId);
		cout	<< setblue
			<< "DGFTauDisplayPanel::RemoveTrace(): Removing trace file for module "
			<< dgf->GetName() << " (Channel " << chn << ")"
			<< setblack << endl;
	}

	for (Int_t i = 0; i < 3; i++) {
		if (ChannelId == -1) chn = i;
		if (chn == i) {
			cmd = "rm -rf dgfTrace.";
			cmd += dgf->GetName();
			cmd += ".chn";
			cmd += chn;
			cmd += ".root";
			gSystem->Exec(cmd.Data());
		}
	}
	return(kTRUE);
}

Bool_t DGFTauDisplayPanel::SaveTrace(const Char_t * FileName, Int_t ModuleId, Int_t ChannelId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::RemoveTrace
// Purpose:        Remove root file containing trace data
// Arguments:      const Char_t * FileName   -- file name
//                 Int_t ModuleId            -- module id
//                 Int_t ChannelId           -- channel id
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Removes file "dgfTrace.<module>.<channel>.root
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString cmd;

	this->TraceFileName();
	cmd = "cp ";
	cmd += fTraceFileName;
	cmd += " ";
	cmd += FileName;
	gSystem->Exec(cmd.Data());
	cout	<< setblue
			<< this->ClassName() << "::SaveTrace(): Copied file " << fTraceFileName << " to " << FileName
			<< setblack
			<< endl;
	return(kTRUE);
}
	
void DGFTauDisplayPanel::MoveFocus(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::MoveFocus
// Purpose:        Move focus to next entry field
// Arguments:      Int_t EntryId     -- entry id
// Results:        --
// Exceptions:     
// Description:    Moves focus to next entry field in ring buffer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

}

const Char_t  * DGFTauDisplayPanel::TraceFileName(const Char_t * Text, Int_t ModuleId, Int_t ChannelId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::TraceFileName
// Purpose:        Create a file name from module & channel
// Arguments:      const Char_t * Text         -- text to be embedded in file name
//                 Int_t ModuleId              -- module index
//                 Int_t ChannelId             -- channel index
// Results:        const Char_t * FileName     -- resulting file name
// Exceptions:     
// Description:    Creates a file name "dgfTrace.<module>.<channel>-<text>.root"
//                 and outputs result to GUI entry field.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Int_t chn;

	if (ModuleId == -1) 	dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	else					dgf = gDGFControlData->GetModule(ModuleId)->GetAddr();

	if (ChannelId == -1)	chn = gDGFControlData->GetSelectedChannel();
	else					chn = gDGFControlData->GetChannel(ChannelId);

	fTraceFileName = "dgfTrace.";
	fTraceFileName += dgf->GetName();
	fTraceFileName += ".chn";
	fTraceFileName += chn;
	if (Text != NULL && *Text != '\0') {
		fTraceFileName += "-";
		fTraceFileName += Text;
	}
	fTraceFileName += ".root";
	fDispFileEntry->GetEntry()->SetText(fTraceFileName.Data());
	return(fTraceFileName.Data());
}

const Char_t  * DGFTauDisplayPanel::HistogramName(Int_t TraceNo, Int_t ModuleId, Int_t ChannelId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::HistogramName
// Purpose:        Create a histogram name from module & channel
// Arguments:      Int_t TraceNo               -- trace number
//                 Int_t ModuleId              -- module index
//                 Int_t ChannelId             -- channel index
// Results:        const Char_t * HistoName    -- resulting name
// Exceptions:     
// Description:    Creates a histogram name "ht<trace>c<channel>"
//                 and corresponding title.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Int_t chn;

	if (ModuleId == -1) 	dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	else					dgf = gDGFControlData->GetModule(ModuleId)->GetAddr();

	if (ChannelId == -1)	chn = gDGFControlData->GetSelectedChannel();
	else					chn = gDGFControlData->GetChannel(ChannelId);

	fHistoName = "ht";
	fHistoName += TraceNo;
	fHistoName += "c";
	fHistoName += chn;

	fHistoTitle = "Trace data: ";
	fHistoTitle += dgf->GetName();
	fHistoTitle += ", chn";
	fHistoTitle += chn;
	fHistoTitle += " (trace ";
	fHistoTitle += TraceNo;
	fHistoTitle += ")";

	return(fHistoName.Data());
}

TMrbDGFEventBuffer * DGFTauDisplayPanel::FindBuffer(Int_t TraceNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::FindBuffer
// Purpose:        Find event buffer by global trace number
// Arguments:      Int_t TraceNo               -- trace number
// Results:        TMrbDGFEventBuffer * Buffer -- buffer address
// Exceptions:     
// Description:    Searches for a buffer containing a given trace/event.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFEventBuffer * ebu;
	Int_t event0;
		
	ebu = (TMrbDGFEventBuffer *) fLofBuffers.First();
	while (ebu) {
		event0 = ebu->GetGlobalEventNo();
		if (TraceNo >= event0 && TraceNo <= event0 + ebu->GetNofEvents() - 1) return(ebu);
		ebu = (TMrbDGFEventBuffer *) fLofBuffers.After(ebu);
	}
	return(NULL);
}
			
const Char_t  * DGFTauDisplayPanel::BufferName(Int_t Index, Int_t ModuleId, Int_t ChannelId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::BufferName
// Purpose:        Create a buffer name from module & channel
// Arguments:      Int_t TraceNo               -- trace number
//                 Int_t ModuleId              -- module index
//                 Int_t ChannelId             -- channel index
// Results:        const Char_t * BufferName   -- resulting name
// Exceptions:     
// Description:    Creates a buffer name "dgfEbuf<index>c<channel>"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t chn;

	if (ChannelId == -1)	chn = gDGFControlData->GetSelectedChannel();
	else					chn = gDGFControlData->GetChannel(ChannelId);

	fEbuName = "eb";
	fEbuName += Index;
	fHistoName += "c";
	fHistoName += chn;

	return(fEbuName.Data());
}

void DGFTauDisplayPanel::SetRunning(Bool_t RunFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauDisplayPanel::SetRunning
// Purpose:        Reflect run status
// Arguments:      Bool_t RunFlag     -- run status
// Results:        --
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TGTextButton * btn;
		
	nx = fTauActions.FindByIndex(kDGFTauButtonAcquire);
	btn = (TGTextButton *) fButtonFrame->GetButton(kDGFTauButtonAcquire);

	if (RunFlag) {
		btnText = "Stop acquisition";
		btn->SetText(btnText);
		btn->ChangeBackground(gDGFControlData->fColorRed);
		fIsRunning = kTRUE;
	} else {
		btnText = nx->GetName();
		btn->SetText(btnText);
		btn->ChangeBackground(gDGFControlData->fColorGray);
		fIsRunning = kFALSE;
	}
	btn->Layout();
}
