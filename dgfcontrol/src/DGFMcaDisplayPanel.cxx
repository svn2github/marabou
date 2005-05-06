//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFMcaDisplayPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    MCA Display
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFMcaDisplayPanel.cxx,v 1.18 2005-05-06 08:43:43 rudi Exp $       
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

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * rbuttonGC;
	TGMrbLayout * comboGC;

	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;
	
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
//	clear focus list
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

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
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
	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	this->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofDGFModuleKeys[cl], cl, 1, kTRUE), -1, 1, 
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
		fGroupSelect[i]->Associate(this);
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, -1, 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, 	frameGC->LH()->GetPadLeft(),
																			frameGC->LH()->GetPadRight(),
																			frameGC->LH()->GetPadTop(),
																			frameGC->LH()->GetPadBottom()));
	fAllSelect->Associate(this);
			
	fHFrame = new TGHorizontalFrame(this, kTabWidth, kTabHeight,
													kChildFrame, frameGC->BG());
	HEAP(fHFrame);
	this->AddFrame(fHFrame, frameGC->LH());

	fSelectChannel = new TGMrbCheckButtonGroup(fHFrame,  "Channel(s)", &fLofChannels, -1, 1,
												groupGC, buttonGC, lofSpecialButtons);
	HEAP(fSelectChannel);
 	fSelectChannel->SetState(kDGFChannelMask, kButtonDown);
	fHFrame->AddFrame(fSelectChannel, frameGC->LH());

// run time
	TGLayoutHints * accuLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, accuLayout);
	HEAP(accuLayout);
	fAccuFrame = new TGGroupFrame(fHFrame, "Run Time", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fAccuFrame);
	fHFrame->AddFrame(fAccuFrame, frameGC->LH());

	TGLayoutHints * teLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	entryGC->SetLH(teLayout);
	HEAP(teLayout);
	fRunTimeEntry = new TGMrbLabelEntry(fAccuFrame, NULL, 200,	kDGFMcaDisplayRunTime,
																kLEWidth,
																kLEHeight,
																200,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fRunTimeEntry);
	fAccuFrame->AddFrame(fRunTimeEntry, frameGC->LH());
	fRunTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRunTimeEntry->GetEntry()->SetText("10");
	fRunTimeEntry->SetRange(0, 1000);
	fRunTimeEntry->SetIncrement(1);
	fRunTimeEntry->AddToFocusList(&fFocusList);
	fRunTimeEntry->Associate(this);

	fTimeScale = new TGMrbRadioButtonList(fAccuFrame,  NULL, &fMcaTimeScaleButtons,
													-1, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, rbuttonGC);
	HEAP(fTimeScale);
	fAccuFrame->AddFrame(fTimeScale, frameGC->LH());
	fTimeScale->SetState(DGFMcaDisplayPanel::kDGFMcaTimeScaleSecs, kButtonDown);

//	display: module/channel
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, selectLayout);
	HEAP(selectLayout);
	fDisplayFrame = new TGGroupFrame(this, "Display", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDisplayFrame);
	this->AddFrame(fDisplayFrame, groupGC->LH());

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

	fDisplayModule = new TGMrbLabelCombo(fDisplayFrame,  "Module",
											&fLofModuleKeys,
											DGFMcaDisplayPanel::kDGFMcaDisplaySelectDisplay, 2,
											kTabWidth, kLEHeight,
											kEntryWidth,
											frameGC, labelGC, comboGC, buttonGC, kTRUE);
	HEAP(fDisplayModule);
	fDisplayFrame->AddFrame(fDisplayModule, frameGC->LH());
	fDisplayModule->GetComboBox()->Select(((TMrbNamedX *) fLofModuleKeys.First())->GetIndex());
	fDisplayModule->Associate(this); 	// get informed if module selection changes

	TGLayoutHints * scfLayout = new TGLayoutHints(kLHintsLeft, 80, 1, 10, 1);
	frameGC->SetLH(scfLayout);
	HEAP(scfLayout);
	TGLayoutHints * sclLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	labelGC->SetLH(sclLayout);
	HEAP(sclLayout);
	TGLayoutHints * scbLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	buttonGC->SetLH(scbLayout);
	HEAP(scbLayout);
	fDisplayChannel = new TGMrbRadioButtonList(fDisplayFrame, "Channel", &fLofChannels,
													kDGFMcaDisplaySelectChannel, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, rbuttonGC);
	HEAP(fDisplayChannel);
	fDisplayChannel->SetState(1);
	fDisplayChannel->Associate(this);	// get informed if channel number changes
	fDisplayFrame->AddFrame(fDisplayChannel, frameGC->LH());

	fRefreshTimeEntry = new TGMrbLabelEntry(fDisplayFrame, "Refresh (s)", 200,	kDGFMcaDisplayRefreshDisplay,
																kLEWidth,
																kLEHeight,
																100,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fRefreshTimeEntry);
	fDisplayFrame->AddFrame(fRefreshTimeEntry, frameGC->LH());
	fRefreshTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRefreshTimeEntry->GetEntry()->SetText("0");
	fRefreshTimeEntry->SetRange(0, 60);
	fRefreshTimeEntry->SetIncrement(10);
	fRefreshTimeEntry->AddToFocusList(&fFocusList);
	fRefreshTimeEntry->Associate(this);

//	buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TGMrbTextButtonGroup(this, "Actions", &fMcaActions, -1, 1, groupGC, buttonGC);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, buttonGC->LH());
	fButtonFrame->Associate(this);

	this->ResetValues();
	fAccuTimer = NULL;
	fRefreshTimer = NULL;

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t DGFMcaDisplayPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::ProcessMessage
// Purpose:        Message handler for the instrument panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFMcaDisplayPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;
	TMrbString dblStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFMcaDisplaySelectColumn) {
						switch (Param1) {
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
						Param1 -= kDGFMcaDisplaySelectColumn;
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
				case kCM_RADIOBUTTON:
					switch (Param1) {
						case kDGFMcaDisplaySelectChannel:
							{
								UInt_t chn = fDisplayChannel->GetActive();
								UInt_t chnPattern = fSelectChannel->GetActive();
								if ((chnPattern & chn) == 0) {
									gMrbLog->Err()	<< "Display channel not active - 0x" << chn << endl;
									gMrbLog->Flush(this->ClassName(), "ProcessMessage");
									fDisplayChannel->SetState(0);
								} else {
									fDisplayChannel->SetState(chn);
								}
							}
							break;
					}
					break;
				case kCM_COMBOBOX:
					fModuleToBeDisplayed = gDGFControlData->GetModule(Param2);
					break;
			}
			break;

		case kC_TEXTENTRY:
			switch (GET_SUBMSG(MsgId)) {
				case kTE_ENTER:
					this->Update(Param1);
					break;
				case kTE_TAB:
					this->Update(Param1);
					this->MoveFocus(Param1);
					break;
			}
			break;
			
	}
	return(kTRUE);
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
	Int_t accuTime;
	TString timeScale;
										
	Bool_t offlineMode = gDGFControlData->IsOffline();

	if (fRunState == kDGFMcaIsRunning) {
		gMrbLog->Err()	<< "Accu in progress - press \"Stop\" first" << endl;
		gMrbLog->Flush(this->ClassName(), "AcquireHistos");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", "Accu in progress - press \"Stop\" first", kMBIconExclamation);
		return(kFALSE);
	}

	TMrbNamedX * tp = fMcaTimeScaleButtons.FindByIndex(fTimeScale->GetActive());
	TMrbString intStr = fRunTimeEntry->GetEntry()->GetText();
	intStr.ToInteger(accuTime);
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
			delete fProgressBar;
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

	TMrbString intStr = fRefreshTimeEntry->GetEntry()->GetText();
	Int_t refresh;
	intStr.ToInteger(refresh);
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

	fRunTimeEntry->GetEntry()->SetText("10");
	fRefreshTimeEntry->GetEntry()->SetText("0");
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
