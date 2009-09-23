//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFMcaDisplayPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    MCA Display
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFMcaDisplayPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $
// Date:
// URL:
// Keywords:
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFMcaDisplayPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TFile.h"
#include "TH1.h"

#include "TGMsgBox.h"
#include "TGLayout.h"
#include "TGTableLayout.h"

#include "TMrbDGFData.h"
#include "TMrbDGFHistogramBuffer.h"

#include "DGFControlData.h"
#include "DGFMcaDisplayPanel.h"

#include "SetColor.h"

const SMrbNamedX kDGFTauButtons[] =
			{
				{DGFMcaDisplayPanel::kDGFMcaDisplayAcquire, 		"Start", "Start run to accumulate histograms"	},
				{DGFMcaDisplayPanel::kDGFMcaDisplayHisto,			"Display",	"Display histogram"	},
				{DGFMcaDisplayPanel::kDGFMcaDisplayHistoClear,		"Display + Clear",	"Display histogram, then reset dgf"	},
				{DGFMcaDisplayPanel::kDGFMcaDisplayStop,			"Stop",		"Stop accumulation"	},
				{0, 															NULL,				NULL						}
			};

const SMrbNamedXShort kDGFMcaTimeScaleButtons[] =
							{
								{DGFMcaDisplayPanel::kDGFMcaTimeScaleSecs, 		"secs"		},
								{DGFMcaDisplayPanel::kDGFMcaTimeScaleMins, 		"mins"		},
								{DGFMcaDisplayPanel::kDGFMcaTimeScaleHours, 	"hours" 	},
								{DGFMcaDisplayPanel::kDGFMcaTimeScaleInfin, 	"infin" 	},
								{0, 											NULL		}
							};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

static TString btnText;

ClassImp(DGFMcaDisplayPanel)

DGFMcaDisplayPanel::DGFMcaDisplayPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel
// Purpose:        DGF Viewer: Calculate tau value
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:
// Exceptions:
// Description:    Implements DGF Viewer's McaDisplay
// Keywords:
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////// Profiles Laden////////////////////////////////////////

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
// 	stdProfile = gMrbLofProfiles->GetDefault();
	stdProfile = gMrbLofProfiles->FindProfile("gold", kTRUE);
	//yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	//blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);

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

	TGLayoutHints * expandXLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,2,5,2);
	HEAP(expandXLayout);

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));

//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFMcaDisplaySelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFMcaDisplaySelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFMcaDisplaySelectNone, "cbutton_none.xpm");

//	Initialize several lists
	fMcaActions.SetName("Actions");
	fMcaActions.AddNamedX(kDGFTauButtons);

	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	fMcaTimeScaleButtons.SetName("Time scale");
	fMcaTimeScaleButtons.AddNamedX(kDGFMcaTimeScaleButtons);
	fMcaTimeScaleButtons.SetPatternMode();

	this->ChangeBackground(gDGFControlData->fColorGold);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fModules);
	this->AddFrame(fModules,dgfFrameLayout);

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {

		fCluster[cl] = new TNGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofDGFModuleKeys[cl], cl, 1, kTRUE), -1,stdProfile ,1,0,
							kTabWidth, kLEHeight,0,lofSpecialButtons, kTRUE);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], expandXLayout);
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled,kFALSE);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown,kFALSE);
	}

	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight,
													kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, dgfFrameLayout);

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
	this->AddFrame(fHFrame,dgfFrameLayout);

	fSelectChannel = new TNGMrbCheckButtonGroup(fHFrame,  "Channel(s)", &fLofChannels, -1,stdProfile, 1,0,0,lofSpecialButtons, kTRUE);
	HEAP(fSelectChannel);
 	fSelectChannel->SetState(kDGFChannelMask, kButtonDown, kFALSE);
	fHFrame->AddFrame(fSelectChannel, dgfFrameLayout);

// run time
	TGLayoutHints * accuLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, accuLayout);
	HEAP(accuLayout);
	fAccuFrame = new TGGroupFrame(fHFrame, "Run Time", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fAccuFrame);
	fHFrame->AddFrame(fAccuFrame,accuLayout);

	TGLayoutHints * teLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
// 	entryGC->SetLH(teLayout);
	HEAP(teLayout);
	fRunTimeEntry = new TNGMrbLabelEntry(fAccuFrame,"Run Time", 	kDGFMcaDisplayRunTime,stdProfile,
																kLEWidth,
																kLEHeight,
																150,TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fRunTimeEntry);
	fAccuFrame->AddFrame(fRunTimeEntry,accuLayout);
// 	fRunTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRunTimeEntry->SetFormat(TGNumberEntry::kNESInteger);
	fRunTimeEntry->SetIntNumber(10);
// 	fRunTimeEntry->SetRange(0, 1000);
	fRunTimeEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,1000);
	fRunTimeEntry->SetIncrement(1);
	fRunTimeEntry->AddToFocusList(&fFocusList);
	fRunTimeEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fTimeScale = new TNGMrbRadioButtonList(fAccuFrame,  NULL, &fMcaTimeScaleButtons,
													-1,stdProfile, 1,0,
													kTabWidth, kLEHeight);
	HEAP(fTimeScale);
	fAccuFrame->AddFrame(fTimeScale, accuLayout);
	fTimeScale->SetState(DGFMcaDisplayPanel::kDGFMcaTimeScaleSecs, kButtonDown,kFALSE);





//	display: module/channel
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, selectLayout);
	HEAP(selectLayout);
	fDisplayFrame = new TGGroupFrame(this, "Display", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fDisplayFrame);
	this->AddFrame(fDisplayFrame, selectLayout);


//	fDisplayFrame->SetLayoutManager(new TGTableLayout(fDisplayFrame, 1,3,kTRUE,10));

	TGLayoutHints * smfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 10, 1, 10, 1);
// 	frameGC->SetLH(smfLayout);
	HEAP(smfLayout);
	TGLayoutHints * smlLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
// 	labelGC->SetLH(smlLayout);
	HEAP(smlLayout);
	TGLayoutHints * smcLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
// 	comboGC->SetLH(smcLayout);
	HEAP(smcLayout);

	Bool_t clearList = kTRUE;
	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		gDGFControlData->CopyKeyList(&fLofModuleKeys, cl, gDGFControlData->GetPatInUse(cl), clearList);
		clearList = kFALSE;
	}

	fDisplayModule = new TNGMrbLabelCombo(fDisplayFrame,  "Module",
											&fLofModuleKeys,stdProfile,
											DGFMcaDisplayPanel::kDGFMcaDisplaySelectDisplay, 2,
											kTabWidth, kLEHeight,
											kComboWidth,TNGMrbLabelCombo::kGMrbComboHasUpDownButtons|TNGMrbLabelCombo::kGMrbComboHasBeginEndButtons);
	HEAP(fDisplayModule);
	fDisplayFrame->AddFrame(fDisplayModule,new TGLayoutHints(kLHintsLeft|kLHintsExpandX,0,50,5,5));
	fDisplayModule->GetComboBox()->Select(((TMrbNamedX *) fLofModuleKeys.First())->GetIndex());
	fDisplayModule->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "SelectDisplay(Int_t, Int_t)");

	TGLayoutHints * scfLayout = new TGLayoutHints(kLHintsLeft, 80, 1, 10, 1);
// 	frameGC->SetLH(scfLayout);
	HEAP(scfLayout);
	TGLayoutHints * sclLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
// 	labelGC->SetLH(sclLayout);
	HEAP(sclLayout);
	TGLayoutHints * scbLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
// 	buttonGC->SetLH(scbLayout);
	HEAP(scbLayout);
	fDisplayChannel = new TNGMrbRadioButtonList(fDisplayFrame, "Channel", &fLofChannels,
													kDGFMcaDisplaySelectChannel,stdProfile, 1,0,
													kTabWidth, kLEHeight);
	HEAP(fDisplayChannel);
	fDisplayChannel->SetState(1,kButtonDown, kFALSE);
	((TNGMrbButtonFrame *) fDisplayChannel)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "RadioButtonPressed(Int_t, Int_t)");

	fDisplayFrame->AddFrame(fDisplayChannel, new TGLayoutHints(kLHintsLeft|kLHintsExpandX,50,50,5,5));

	fRefreshTimeEntry = new TNGMrbLabelEntry(fDisplayFrame, "Refresh (s)",	kDGFMcaDisplayRefreshDisplay,stdProfile,
																kLEWidth,
																kLEHeight,
																150,TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fRefreshTimeEntry);
// 	fDisplayFrame->AddFrame(fRefreshTimeEntry/*, scfLayout*/);
	fDisplayFrame->AddFrame(fRefreshTimeEntry, new TGLayoutHints(kLHintsLeft|kLHintsExpandX,50,0,5,5));
// 	fRefreshTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRefreshTimeEntry->SetFormat(TGNumberEntry::kNESInteger);
	fRefreshTimeEntry->SetIntNumber(0);
// 	fRefreshTimeEntry->SetRange(0, 60);
	fRefreshTimeEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,60);
	fRefreshTimeEntry->SetIncrement(10);
	fRefreshTimeEntry->AddToFocusList(&fFocusList);
	fRefreshTimeEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

//	buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
// 	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TNGMrbTextButtonGroup(this, "Actions", &fMcaActions, -1,stdProfile, 1,0);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, btnLayout);
	((TNGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ResetValues();
	fAccuTimer = NULL;
	fRefreshTimer = NULL;

	dgfFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);
	this->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	TabFrame->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());

	TabFrame->AddFrame(this, dgfFrameLayout);
// 	
// 	MapSubwindows();


// 	fClient->NeedRedraw(this);
// 	fClient->NeedRedraw(fDisplayFrame);
	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();




}

void DGFMcaDisplayPanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::SelectModule
// Purpose:        Slot method: select module(s)
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFMcaDisplaySelectColumn) {
		switch (Selection) {
			case kDGFMcaDisplaySelectAll:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
				}
				break;
			case kDGFMcaDisplaySelectNone:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
					break;
		}
	} else {
		Selection -= kDGFMcaDisplaySelectColumn;
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

void DGFMcaDisplayPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCptmPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFMcaDisplayAcquire:
			this->AcquireHistos();
			break;
		case kDGFMcaDisplayHisto:
			this->DisplayHisto(kFALSE);
			break;
		case kDGFMcaDisplayHistoClear:
			this->DisplayHisto(kTRUE);
			break;
		case kDGFMcaDisplayStop:
			fStopAccu = kTRUE;
			break;
	}
}

void DGFMcaDisplayPanel::SelectDisplay(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::SelectDisplay
// Purpose:        Slot method: select module to be displayed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fModuleToBeDisplayed = gDGFControlData->GetModule(Selection);
}

void DGFMcaDisplayPanel::EntryChanged(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::EntryChanged
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

void DGFMcaDisplayPanel::RadioButtonPressed(Int_t FrameId, Int_t Selection) {
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

	UInt_t chn = Selection;
	UInt_t chnPattern = fSelectChannel->GetActive();
	if ((chnPattern & chn) == 0) {
		gMrbLog->Err()	<< "Display channel not active - 0x" << chn << endl;
		gMrbLog->Flush(this->ClassName(), "ProcessMessage");
	}
}

Bool_t DGFMcaDisplayPanel::AcquireHistos() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::AcquireHistos
// Purpose:        Start accumulation
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Starts accumulation of histograms
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	DGFModule * dgfModule;
	TMrbDGF * dgf;
	Int_t modNo, cl;
	Int_t nofModules;
	TString timeScale;

	Bool_t offlineMode = gDGFControlData->IsOffline();

	if (fRunState == kDGFMcaIsRunning) {
		gMrbLog->Err()	<< "Accu in progress - press \"Stop\" first" << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireHistos");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", "Accu in progress - press \"Stop\" first", kMBIconExclamation);
		return(kFALSE);
	}

	TMrbNamedX * tp = fMcaTimeScaleButtons.FindByIndex(fTimeScale->GetActive());
	Int_t accuTime = fRunTimeEntry->GetIntNumber();
	Int_t waitInv = 0;
	switch (tp->GetIndex()) {
		case kDGFMcaTimeScaleSecs:	waitInv = 1; break;
		case kDGFMcaTimeScaleMins:	waitInv = 60; break;
		case kDGFMcaTimeScaleHours: waitInv = 60 * 60; break;
		case kDGFMcaTimeScaleInfin: waitInv = 0; break;
	}
	fSecsToWait = accuTime * waitInv;

	UInt_t chnPattern = fSelectChannel->GetActive();
	if (chnPattern == 0) {
		gMrbLog->Err()	<< "No channels selected" << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireHistos");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one channel", kMBIconStop);
		return(kFALSE);
	}

	dgfModule = gDGFControlData->FirstModule();
	nofModules = 0;
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				dgf->AccuHist_Init(chnPattern);
				dgf->AccuHist_Start(kTRUE);
			}
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}
	if (nofModules == 0) {
		gMrbLog->Err()	<< "No modules selected" << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireHistos");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "No modules selected", kMBIconExclamation);
		return(kFALSE);
	}

	fProgressBar = new TGMrbProgressBar(fClient->GetRoot(), this, "Accumulating ...", 400, "blue", NULL, kTRUE);
	fProgressBar->SetRange(0, (fSecsToWait == 0) ? 60 : fSecsToWait);

	if (offlineMode) fSecsToWait = 1;
	fRunState = kDGFMcaIsRunning;
	fStopAccu = kFALSE;
	fStopWatch = 0;
	if (fAccuTimer == NULL) {
		fAccuTimer = new TTimer(this, 1000, kFALSE);
		fAccuTimer->SetTimerID(kDGFAccuTimerID);
	}
	fAccuTimer->Start(1000);
	return(kTRUE);
}

Bool_t DGFMcaDisplayPanel::HandleTimer(TTimer * Timer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::HandleTimer
// Purpose:        Handle timer requests
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Method will be called on timer requests.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Timer->GetTimerID() == kDGFAccuTimerID) {
		Bool_t stopIt = kFALSE;
		fStopWatch++;			// count seconds

		if (fStopAccu) {
			gMrbLog->Out() << "Accu stopped after " << fStopWatch << " secs." << endl;
			gMrbLog->Flush(this->ClassName(), "HandleTimer", setblue);
			stopIt = kTRUE;
		}

		if (fSecsToWait != 0 && fStopWatch > fSecsToWait) stopIt = kTRUE;

		if (stopIt) {
			Timer->Stop();
			fRunState = kDGFMcaRunStopped;
			if (fRefreshTimer) fRefreshTimer->Stop();
			fProgressBar->DeleteWindow();
			fProgressBar = NULL;
			this->StoreHistos();
		} else if (fProgressBar) {
			if (fSecsToWait == 0) {
				if ((fStopWatch % 60) == 0) fProgressBar->Reset();
				fProgressBar->Increment(1, Form("%d secs", fStopWatch));
			} else {
				fProgressBar->Increment(1, Form("%d of %d sec(s)", fStopWatch, fSecsToWait));
			}
		}
	} else if (Timer->GetTimerID() == kDGFRefreshTimerID) {
		this->DisplayHisto(kFALSE);
	}
	return(kTRUE);
}

Bool_t DGFMcaDisplayPanel::StoreHistos() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::StoreHistos
// Purpose:        Store resulting histograms
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Saves histograms to file
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFHistogramBuffer histoBuffer;
	TMrbString intStr;
	TString timeScale;

	Bool_t verbose = gDGFControlData->IsVerbose();
	Bool_t offlineMode = gDGFControlData->IsOffline();

	Int_t nofHistos = 0;
	Int_t nofModules = 0;
	UInt_t chnPattern = fSelectChannel->GetActive();

	DGFModule * dgfModule = gDGFControlData->FirstModule();
	TFile * mcaFile = NULL;
	ofstream listFile;
	while (dgfModule) {
		Int_t cl = nofModules / kNofModulesPerCluster;
		Int_t modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				TMrbDGF * dgf = dgfModule->GetAddr();
				histoBuffer.Reset();
				histoBuffer.SetModule(dgf);
				if (dgf->AccuHist_Stop(0)) {
					Int_t nofWords = dgf->ReadHistogramsViaRsh(histoBuffer, chnPattern);
					if (nofWords > 0) {
						if (verbose) histoBuffer.Print();
						for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
							if (histoBuffer.IsActive(chn)) {
								if (mcaFile == NULL) {
									listFile.open("mca.histlist", ios::out);
									mcaFile = new TFile("mca.root", "RECREATE");
								}
								histoBuffer.FillHistogram(chn, kFALSE);
								TH1F * h = histoBuffer.Histogram(chn);
								h->Write();
								listFile << h->GetName() << endl;
								nofHistos++;
							}
						}
					} else {
						gMrbLog->Err()	<< "DGF in C" << dgf->GetCrate() << ".N" << dgf->GetStation()
											<< ": Histogram buffer is empty" << endl;
						gMrbLog->Flush(this->ClassName(), "AcquireHistos");
					}
				}
			}
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}
	if (nofHistos > 0) {
		listFile.close();
		mcaFile->Close();
	}
	if (offlineMode || (nofHistos > 0)) {
		gMrbLog->Out()	<< nofHistos << " histogram(s) written to file mca.root" << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireHistos", setblue);
	} else {
		gMrbLog->Err()  << "No histograms at all" << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireHistos");
	}
	return(kTRUE);
}

Bool_t DGFMcaDisplayPanel::DisplayHisto(Bool_t ClearMCA) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::DisplayHisto
// Purpose:        Display selected histogram
// Arguments:      Bool_t ClearMCA   -- clear mca if kTRUE
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Displays selected histogram.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFHistogramBuffer histoBuffer;

	Bool_t verbose = gDGFControlData->IsVerbose();
	Bool_t offlineMode = gDGFControlData->IsOffline();

	Int_t refresh = fRefreshTimeEntry->GetIntNumber();
	if (refresh > 0) {
		if (fRefreshTimer == NULL) {
			fRefreshTimer = new TTimer(this, 1000 * refresh, kFALSE);
			fRefreshTimer->SetTimerID(kDGFRefreshTimerID);
		}
		fRefreshTimer->Start(1000 * refresh, kTRUE);
	} else if (fRefreshTimer) {
		fRefreshTimer->Stop();
	}

	DGFModule * dgfModule = fModuleToBeDisplayed;
	if (dgfModule == NULL) {
		gMrbLog->Err()  << "No module selected for display" << endl;
		gMrbLog->Flush(this->ClassName(), "DisplayHisto");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "No module selected", kMBIconExclamation);
		return(kFALSE);
	}

	UInt_t displPattern = fDisplayChannel->GetActive();
	if (displPattern == 0) {
		gMrbLog->Err()  << "No channel selected for display" << endl;
		gMrbLog->Flush(this->ClassName(), "DisplayHisto");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "No channel selected", kMBIconExclamation);
		return(kFALSE);
	}

	if (!offlineMode) {
		TMrbDGF * dgf = dgfModule->GetAddr();
		if (fRunState == kDGFMcaIsRunning) dgf->AccuHist_Stop(0);
		histoBuffer.Reset();
		histoBuffer.SetModule(dgf);
		Int_t nofWords = dgf->ReadHistogramsViaRsh(histoBuffer, displPattern);
		if (nofWords > 0) {
			if (verbose) histoBuffer.Print();
			Int_t chn = 0;
			for (Int_t i = 0; i < TMrbDGFData::kNofChannels; i++, chn++) {
				if (displPattern & 1)break;
				displPattern >>= 1;
			}
			if (histoBuffer.IsActive(chn)) {
				histoBuffer.FillHistogram(chn, kFALSE);
				TH1F * hist = histoBuffer.Histogram(chn);
				hist->SetName("hMca");
				TString canvas = "fhMcaCanvas";
				fFitHist = (FitHist *) gROOT->FindObject(canvas.Data());
				if (fFitHist) {
					fFitHist->GetCanvas()->cd();
					fFitHist->SetHist(hist);
				} else {
					gROOT->cd();
					Window_t wdum;
					Int_t ax, ay, w, h;
					w = this->GetWidth();
					h = this->GetHeight();
					gVirtualX->TranslateCoordinates(	this->GetId(), this->GetParent()->GetId(),
														0, h / 2,
														ax, ay, wdum);
					fFitHist = new FitHist(				canvas.Data(), hist->GetTitle(),
														hist,
														hist->GetName(),
														ax, ay, w / 2, h / 2);
					HEAP(fFitHist);
				}
			}
		}
		if (fRunState == kDGFMcaIsRunning) {
			UInt_t chnPattern = fSelectChannel->GetActive();
			if (ClearMCA) {
				dgf->AccuHist_Init(chnPattern);
				dgf->AccuHist_Start(kTRUE);
			} else {
				dgf->InhibitNewRun(kTRUE);
				dgf->AccuHist_Init(chnPattern);
				dgf->AccuHist_Start(kFALSE);
				dgf->InhibitNewRun(kFALSE);
			}
		}
	}
	return(kTRUE);
}

Bool_t DGFMcaDisplayPanel::McaPause() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::McaPause
// Purpose:        Stop MCA run temporarily
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    If MCA is running it will be stopped.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fRunState == kDGFMcaIsRunning) {
		Bool_t verbose = gDGFControlData->IsVerbose();
		Bool_t offlineMode = gDGFControlData->IsOffline();
		if (fAccuTimer) fAccuTimer->Stop();
		if (fRefreshTimer) fRefreshTimer->Stop();
		Int_t nofModules = 0;
		DGFModule * dgfModule = gDGFControlData->FirstModule();
		while (dgfModule) {
			Int_t cl = nofModules / kNofModulesPerCluster;
			Int_t modNo = nofModules - cl * kNofModulesPerCluster;
			if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
				if (!offlineMode) {
					TMrbDGF * dgf = dgfModule->GetAddr();
					dgf->AccuHist_Stop(0);
					dgf->InhibitNewRun();				}
			}
			dgfModule = gDGFControlData->NextModule(dgfModule);
			nofModules++;
		}
		if (verbose) {
			gMrbLog->Out()  << "MCA run pausing" << endl;
			gMrbLog->Flush(this->ClassName(), "McaPause", setmagenta);
		}
		fRunState = kDGFMcaRunPausing;
	}
	return(kTRUE);
}

Bool_t DGFMcaDisplayPanel::McaResume() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::McaResume
// Purpose:        Resume MCA run
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    If MCA is running accumulation will continue.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fRunState == kDGFMcaRunPausing) {
		Bool_t verbose = gDGFControlData->IsVerbose();
		Bool_t offlineMode = gDGFControlData->IsOffline();
		DGFModule * dgfModule = gDGFControlData->FirstModule();
		Int_t nofModules = 0;
		while (dgfModule) {
			Int_t cl = nofModules / kNofModulesPerCluster;
			Int_t modNo = nofModules - cl * kNofModulesPerCluster;
			if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
				if (!offlineMode) {
					UInt_t chnPattern = fSelectChannel->GetActive();
					TMrbDGF * dgf = dgfModule->GetAddr();
					dgf->AccuHist_Init(chnPattern);
					dgf->AccuHist_Start(kFALSE);
					dgf->InhibitNewRun(kFALSE);
				}
			}
			dgfModule = gDGFControlData->NextModule(dgfModule);
			nofModules++;
		}
		if (fAccuTimer) fAccuTimer->Start();
		if (fRefreshTimer) fRefreshTimer->Start(-1, kTRUE);
		if (verbose) {
			gMrbLog->Out()  << "MCA resumes running" << endl;
			gMrbLog->Flush(this->ClassName(), "McaResume", setmagenta);
		}
		fRunState = kDGFMcaIsRunning;
	}
	return(kTRUE);
}

Bool_t DGFMcaDisplayPanel::Update(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::Update
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

Bool_t DGFMcaDisplayPanel::ResetValues() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::ResetValues
// Purpose:        Clear values in tau display panel
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Clears entry fields in tau display panel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fRunTimeEntry->SetNumber(10);
	fRefreshTimeEntry->SetIntNumber(0);
	fRunState = kDGFMcaRunStopped;
	fStopAccu = kFALSE;
	fStopWatch = 0;
	fSecsToWait = 0;
	return(kTRUE);
}

void DGFMcaDisplayPanel::MoveFocus(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::MoveFocus
// Purpose:        Move focus to next entry field
// Arguments:      Int_t EntryId     -- entry id
// Results:        --
// Exceptions:
// Description:    Moves focus to next entry field in ring buffer.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

}
