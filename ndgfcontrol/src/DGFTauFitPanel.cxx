//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFTauFitPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Tau fit
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFTauFitPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $
// Date:
// URL:
// Keywords:
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFTauFitPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TFile.h"
#include "TH1.h"

#include "TGMsgBox.h"

#include "TMrbDGFData.h"
#include "TMrbDGFHistogramBuffer.h"
#include "TGMrbProgressBar.h"

#include "DGFControlData.h"
#include "DGFTauFitPanel.h"

#include "SetColor.h"

const SMrbNamedX kDGFTauButtons[] =
			{
				{DGFTauFitPanel::kDGFTauFitStartFit, 	"Start",	"Start tau fit"	},
				{DGFTauFitPanel::kDGFTauFitStop,		"Stop", 	"Stop accumulation"	},
				{DGFTauFitPanel::kDGFTauFitReset,		"Reset",	"Reset to default values"	},
				{0, 									NULL,		NULL						}
			};

const SMrbNamedXShort kDGFTauFitTimeScaleButtons[] =
							{
								{DGFTauFitPanel::kDGFTauFitTimeScaleSecs, 		"secs"		},
								{DGFTauFitPanel::kDGFTauFitTimeScaleMins, 		"mins"		},
								{DGFTauFitPanel::kDGFTauFitTimeScaleHours, 		"hours" 	},
								{0, 											NULL		}
							};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

static Bool_t abortAccu = kFALSE;

static TString btnText;

ClassImp(DGFTauFitPanel)

DGFTauFitPanel::DGFTauFitPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel
// Purpose:        DGF Viewer: Calculate tau value
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:
// Exceptions:
// Description:    Implements DGF Viewer's TauFit
// Keywords:
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////// Profiles Laden////////////////////////////////////////

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
// 	stdProfile = gMrbLofProfiles->GetDefault();
	//yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	//blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);
	stdProfile = gMrbLofProfiles->FindProfile("gold", kTRUE);

	gMrbLofProfiles->Print();

////////////////////////////////////////////////////////////////////////////////////////////////

/*	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * rbuttonGC;
	TGMrbLayout * comboGC;*/

	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	clear focus list
	fFocusList.Clear();

// graphic layout
/*	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
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
								gDGFControlData->fColorWhite);	HEAP(entryGC);*/



	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));

//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFTauFitSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFTauFitSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFTauFitSelectNone, "cbutton_none.xpm");

//	Initialize several lists
	fTauFitActions.SetName("Actions");
	fTauFitActions.AddNamedX(kDGFTauButtons);

	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	fTauFitTimeScaleButtons.SetName("Time scale");
	fTauFitTimeScaleButtons.AddNamedX(kDGFTauFitTimeScaleButtons);
	fTauFitTimeScaleButtons.SetPatternMode();

	this->ChangeBackground(gDGFControlData->fColorGold);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules

	TGLayoutHints * expandXLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,2,5,2);
	HEAP(expandXLayout);

	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fModules);
	this->AddFrame(fModules, dgfFrameLayout);

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TNGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofDGFModuleKeys[cl], cl, 1, kTRUE), -1,stdProfile, 1,0,
							kTabWidth, kLEHeight,0,lofSpecialButtons,kTRUE);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], expandXLayout);
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}

	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight,
													kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
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

	fHFrame = new TGHorizontalFrame(this, kTabWidth, kTabHeight,
													kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fHFrame);
	this->AddFrame(fHFrame, dgfFrameLayout);

	fSelectChannel = new TNGMrbCheckButtonGroup(fHFrame,  "Channel(s)", &fLofChannels, -1,stdProfile, 1,0);
	HEAP(fSelectChannel);
 	fSelectChannel->SetState(kDGFChannelMask, kButtonDown);
	fHFrame->AddFrame(fSelectChannel, dgfFrameLayout);

// tau range
	TGLayoutHints * tauLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, tauLayout);
	HEAP(tauLayout);
	fTauFrame = new TGGroupFrame(fHFrame, "Tau Range", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fTauFrame);
	fHFrame->AddFrame(fTauFrame, tauLayout);

	TGLayoutHints * teLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
// 	entryGC->SetLH(teLayout);
	HEAP(teLayout);
	fTauStartEntry = new TNGMrbLabelEntry(fTauFrame, "Start",
																kDGFTauFitRangeStart,stdProfile,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fTauStartEntry);
	fTauFrame->AddFrame(fTauStartEntry, tauLayout);
// 	fTauStartEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTauStartEntry->SetFormat(TGNumberEntry::kNESRealTwo);
	fTauStartEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,1,100);
	fTauStartEntry->SetNumber(50);
// 	fTauStartEntry->SetRange(1, 100);
	fTauStartEntry->SetIncrement(.5);
	fTauStartEntry->AddToFocusList(&fFocusList);
	fTauStartEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fTauStepEntry = new TNGMrbLabelEntry(fTauFrame, "Step",
																kDGFTauFitRangeStep,stdProfile,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fTauStepEntry);
	fTauFrame->AddFrame(fTauStepEntry, tauLayout);
// 	fTauStepEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTauStepEntry->SetFormat(TGNumberEntry::kNESRealTwo);
	fTauStepEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0.1,5);
	fTauStepEntry->SetNumber(.1);
// 	fTauStepEntry->SetRange(.1, 5);
	fTauStepEntry->SetIncrement(.1);
	fTauStepEntry->AddToFocusList(&fFocusList);
	fTauStepEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fTauStopEntry = new TNGMrbLabelEntry(fTauFrame, "Stop",
																kDGFTauFitRangeStop,stdProfile,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fTauStopEntry);
	fTauFrame->AddFrame(fTauStopEntry, tauLayout);
// 	fTauStopEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTauStopEntry->SetFormat(TGNumberEntry::kNESRealTwo);
	fTauStopEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,1,100);
	fTauStopEntry->SetNumber(50.);
// 	fTauStopEntry->SetRange(1, 100);
	fTauStopEntry->SetIncrement(.5);
	fTauStopEntry->AddToFocusList(&fFocusList);
	fTauStopEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

// accu settings
	TGLayoutHints * accuLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, accuLayout);
	HEAP(accuLayout);
	fAccuFrame = new TGGroupFrame(fHFrame, "Run Time", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fAccuFrame);
	fHFrame->AddFrame(fAccuFrame,accuLayout);

	fRunTimeEntry = new TNGMrbLabelEntry(fAccuFrame,"Run Time",
																kDGFTauFitRunTime,stdProfile,
																kLEWidth,
																kLEHeight,
																150,
																TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fRunTimeEntry);
	fAccuFrame->AddFrame(fRunTimeEntry, accuLayout);
// 	fRunTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRunTimeEntry->SetFormat(TGNumberEntry::kNESRealTwo);
	fRunTimeEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,1,1000);
	fRunTimeEntry->SetNumber(10);
// 	fRunTimeEntry->SetRange(1, 1000);
	fRunTimeEntry->SetIncrement(1);
	fRunTimeEntry->AddToFocusList(&fFocusList);
	fRunTimeEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fTimeScale = new TNGMrbRadioButtonList(fAccuFrame,  NULL, &fTauFitTimeScaleButtons, -1,stdProfile, 1,0,
													kTabWidth, kLEHeight);
	HEAP(fTimeScale);
	fAccuFrame->AddFrame(fTimeScale, accuLayout);
	fTimeScale->SetState(DGFTauFitPanel::kDGFTauFitTimeScaleSecs, kButtonDown);

//	buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
// 	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TNGMrbTextButtonGroup(this, "Actions", &fTauFitActions, -1,stdProfile, 1,0);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame,btnLayout);
	((TNGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

//	no accu running
	fIsRunning = kFALSE;

	this->ResetValues();

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

void DGFTauFitPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::SelectModule
// Purpose:        Slot method: select destination module(s)
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFTauFitSelectColumn) {
		switch (Selection) {
			case kDGFTauFitSelectAll:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
				break;
			case kDGFTauFitSelectNone:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
				break;
		}
	} else {
		Selection -= kDGFTauFitSelectColumn;
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
}void DGFTauFitPanel::EntryChanged(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::EntryChanged
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

void DGFTauFitPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFTauFitStartFit:
			if (fIsRunning) {
				this->SetRunning(kFALSE);
				new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", "Aborting histogram acquisition", kMBIconExclamation);
			} else {
				this->TauFit();
			}
			break;
		case kDGFTauFitStop:
			abortAccu = kTRUE;
			break;
		case kDGFTauFitReset:
			break;
	}
}

Bool_t DGFTauFitPanel::TauFit() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::TauFit
// Purpose:        Start tau fit
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Starts accumulation of histograms
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFHistogramBuffer histoBuffer;
	DGFModule * dgfModule;
	TMrbDGF * dgf;
	Int_t modNo, cl;
	Int_t nofModules, nofHistos, nofWords;
	TMrbString intStr;
	Int_t accuTime;
	TString timeScale;

	Bool_t verbose = gDGFControlData->IsVerbose();
	Bool_t offlineMode = gDGFControlData->IsOffline();

	TMrbNamedX * tp = fTauFitTimeScaleButtons.FindByIndex(fTimeScale->GetActive());
	accuTime = fRunTimeEntry->GetIntNumber();
	Int_t waitInv = 0;
	switch (tp->GetIndex()) {
		case kDGFTauFitTimeScaleSecs:	waitInv = 1; break;
		case kDGFTauFitTimeScaleMins:	waitInv = 60; break;
		case kDGFTauFitTimeScaleHours: waitInv = 60 * 60; break;
	}
	Int_t secsToWait = accuTime * waitInv;

	UInt_t chnPattern = fSelectChannel->GetActive();
	if (chnPattern == 0) {
		gMrbLog->Err()	<< "No channels selected" << endl;
		gMrbLog->Flush(this->ClassName(), "TauFit");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one channel", kMBIconStop);
		return(kFALSE);
	}

	Double_t tauStart = fTauStartEntry->GetNumber();
	Double_t tauStep = fTauStepEntry->GetNumber();
	Double_t tauStop = fTauStopEntry->GetNumber();
	if (tauStart >= tauStop) {
		gMrbLog->Err()	<< "Illegal tau range - " << tauStart << " ... " << tauStop << endl;
		gMrbLog->Flush(this->ClassName(), "TauFit");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Illegal tau range", kMBIconExclamation);
		return(kFALSE);
	}

	nofHistos = 0;
	TFile * taufitFile = NULL;
	Double_t tauUp = tauStart;
	Double_t tauDown = tauStop;
	Int_t nofTaus = (Int_t) ((tauStop - tauStart + .5) / tauStep) + 1;
	TArrayD tauList(nofTaus);
	Int_t n = 0;
	while (tauUp < tauDown) {
		tauList[n] = tauUp;
		tauList[n + 1] = tauDown;
		tauUp += tauStep;
		tauDown -= tauStep;
		n += 2;
	}
	nofTaus = n;

	TGMrbProgressBar * pgb1;
	TGMrbProgressBar * pgb2;

	pgb1 = new TGMrbProgressBar(fClient->GetRoot(), this, "Starting TAU fit ...", 400, "blue", NULL, kTRUE);
	pgb2 = new TGMrbProgressBar(fClient->GetRoot(), this, "Starting TAU fit ...", 400, "blue", NULL, kTRUE);
	pgb1->SetRange(0, nofTaus);
 	Bool_t initFlag = kTRUE;
	for (Int_t i = 0; i < nofTaus; i++) {
		Double_t tau = tauList[i];
		dgfModule = gDGFControlData->FirstModule();
		nofModules = 0;
		TString tauVal = Form("tau=%5.2f (%d out of %d)", tau, i + 1, nofTaus);
		pgb1->Increment(1, tauVal.Data());
		while (dgfModule) {
			cl = nofModules / kNofModulesPerCluster;
			modNo = nofModules - cl * kNofModulesPerCluster;
			if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
				if (!offlineMode) {
					dgf = dgfModule->GetAddr();
					for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
						if (chnPattern & (1 << chn)) dgf->SetTau(chn, tau, kTRUE);
					}
					if (initFlag) dgf->AccuHist_Init(chnPattern);
					dgf->AccuHist_Start();
				}
			}
			dgfModule = gDGFControlData->NextModule(dgfModule);
			nofModules++;
		}
		if (nofModules == 0) {
			gMrbLog->Err()	<< "No modules selected" << endl;
			gMrbLog->Flush(this->ClassName(), "TauFit");
			new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "No modules selected", kMBIconExclamation);
			break;
		}

		initFlag = kFALSE;

		abortAccu = kFALSE;
		if (offlineMode) secsToWait = 1;
		TString accuMsg = Form("Starting accu for tau=%5.2f", tau);
		pgb2->SetWindowName(accuMsg.Data());
		pgb2->SetRange(0, secsToWait);
		pgb2->Reset();
		for (Int_t i = 0; i < secsToWait; i++) {
			TString timMsg = Form("%d out of %d secs", i + 1, secsToWait);
			pgb2->Increment(1, timMsg.Data());
			sleep(1);
			gSystem->ProcessEvents();
			if (abortAccu) {
				gMrbLog->Err() << "Aborted after " << i << " secs. Stopping current run." << endl;
				gMrbLog->Flush(this->ClassName(), "TauFit");
				break;
			}
		}
		if (abortAccu) break;

		nofModules = 0;
		dgfModule = gDGFControlData->FirstModule();
		while (dgfModule) {
			cl = nofModules / kNofModulesPerCluster;
			modNo = nofModules - cl * kNofModulesPerCluster;
			if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
				if (!offlineMode) {
					dgf = dgfModule->GetAddr();
					histoBuffer.Reset();
					histoBuffer.SetModule(dgf);
					if (dgf->StopRun()) {
						nofWords = dgf->ReadHistogramsViaRsh(histoBuffer, chnPattern);
						if (nofWords > 0) {
							if (verbose) histoBuffer.Print();
							for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
								if (histoBuffer.IsActive(chn)) {
									if (taufitFile == NULL) taufitFile = new TFile("tau.root", "RECREATE");
									histoBuffer.FillHistogram(chn);
									TH1F * h = histoBuffer.Histogram(chn);
									TString hName = Form("%s.tau_%5.2f", h->GetName(), tau);
									TString hTitle = Form("%s; tau = %5.2f", h->GetTitle(), tau);
									hName.ReplaceAll(".", "_");
									h->SetName(hName.Data());
									h->SetTitle(hTitle.Data());
									h->Fill((Axis_t) 0, tau);
									h->Write();
									nofHistos++;
								}
							}
						} else {
							gMrbLog->Err()	<< "DGF in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
												<< ": Histogram buffer is empty" << endl;
							gMrbLog->Flush(this->ClassName(), "TauFit");
						}
					}
				}
			}
			dgfModule = gDGFControlData->NextModule(dgfModule);
			nofModules++;
		}
	}
//	pgb1->DeleteWindow();
//	pgb2->DeleteWindow();
	delete pgb1;
	delete pgb2;


	nofModules = 0;
	dgfModule = gDGFControlData->FirstModule();
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				dgf->RestoreParams(TMrbDGF::kSaveAccuHist);
			}
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}

	if (nofHistos > 0) taufitFile->Close();

	if (offlineMode || (nofHistos > 0)) {
		gMrbLog->Out()	<< nofHistos << " histogram(s) written to file tau.root (tau = " << tauStart
						<< " ... " << tauStep << " ... " << tauStop << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "TauFit", setblue);
	} else {
		gMrbLog->Err()  << "No histograms at all" << endl;
		gMrbLog->Flush(this->ClassName(), "TauFit");
	}
	return(kTRUE);
}

Bool_t DGFTauFitPanel::Update(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::Update
// Purpose:        Update program state on X events
// Arguments:      Int_t EntryId      -- entry id
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Updates variables on X events and starts action.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr, dblStr;

	switch (EntryId) {
	}
	return(kTRUE);
}

Bool_t DGFTauFitPanel::ResetValues() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::ResetValues
// Purpose:        Clear values in tau display panel
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Clears entry fields in tau display panel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fRunTimeEntry->SetNumber(10);
	return(kTRUE);
}

void DGFTauFitPanel::MoveFocus(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::MoveFocus
// Purpose:        Move focus to next entry field
// Arguments:      Int_t EntryId     -- entry id
// Results:        --
// Exceptions:
// Description:    Moves focus to next entry field in ring buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

}

void DGFTauFitPanel::SetRunning(Bool_t RunFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::SetRunning
// Purpose:        Reflect run status
// Arguments:      Bool_t RunFlag     -- run status
// Results:        --
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	TGTextButton * btn;

	TMrbDGF * dgf;

	dgf = gDGFControlData->GetSelectedModule()->GetAddr();

	nx = fTauFitActions.FindByIndex(kDGFTauFitStartFit);
	btn = (TGTextButton *) fButtonFrame->GetButton(kDGFTauFitStartFit);

	if (RunFlag) {
		btnText = "Stop tau fit";
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
