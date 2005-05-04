//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFTauFitPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Tau fit
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFTauFitPanel.cxx,v 1.7 2005-05-04 13:36:57 rudi Exp $       
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

#include "DGFControlData.h"
#include "DGFTauFitPanel.h"

#include "SetColor.h"

const SMrbNamedX kDGFTauButtons[] =
			{
				{DGFTauFitPanel::kDGFTauFitStartFit, 	"Start",	"Start tau fit"	},
				{DGFTauFitPanel::kDGFTauFitAbort,		"Abort",	"Abort accumulation"	},
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

// tau range
	TGLayoutHints * tauLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, tauLayout);
	HEAP(tauLayout);
	fTauFrame = new TGGroupFrame(fHFrame, "Tau Range", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTauFrame);
	fHFrame->AddFrame(fTauFrame, frameGC->LH());

	TGLayoutHints * teLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	entryGC->SetLH(teLayout);
	HEAP(teLayout);
	fTauStartEntry = new TGMrbLabelEntry(fTauFrame, "Start",
																200, kDGFTauFitRangeStart,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fTauStartEntry);
	fTauFrame->AddFrame(fTauStartEntry, frameGC->LH());
	fTauStartEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTauStartEntry->GetEntry()->SetText("50.");
	fTauStartEntry->SetRange(1, 100);
	fTauStartEntry->SetIncrement(.5);
	fTauStartEntry->AddToFocusList(&fFocusList);
	fTauStartEntry->Associate(this);

	fTauStepEntry = new TGMrbLabelEntry(fTauFrame, "Step",
																200, kDGFTauFitRangeStep,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fTauStepEntry);
	fTauFrame->AddFrame(fTauStepEntry, frameGC->LH());
	fTauStepEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTauStepEntry->GetEntry()->SetText(".1");
	fTauStepEntry->SetRange(.1, 5);
	fTauStepEntry->SetIncrement(.1);
	fTauStepEntry->AddToFocusList(&fFocusList);
	fTauStepEntry->Associate(this);

	fTauStopEntry = new TGMrbLabelEntry(fTauFrame, "Stop",
																200, kDGFTauFitRangeStop,
																kLEWidth,
																kLEHeight,
																(Int_t) (1.5 * kEntryWidth),
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fTauStopEntry);
	fTauFrame->AddFrame(fTauStopEntry, frameGC->LH());
	fTauStopEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTauStopEntry->GetEntry()->SetText("50.");
	fTauStopEntry->SetRange(1, 100);
	fTauStopEntry->SetIncrement(.5);
	fTauStopEntry->AddToFocusList(&fFocusList);
	fTauStopEntry->Associate(this);

// accu settings
	TGLayoutHints * accuLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, accuLayout);
	HEAP(accuLayout);
	fAccuFrame = new TGGroupFrame(fHFrame, "Accu Settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fAccuFrame);
	fHFrame->AddFrame(fAccuFrame, frameGC->LH());

	fRunTimeEntry = new TGMrbLabelEntry(fAccuFrame, "Run time",
																200, kDGFTauFitRunTime,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fRunTimeEntry);
	fAccuFrame->AddFrame(fRunTimeEntry, frameGC->LH());
	fRunTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRunTimeEntry->GetEntry()->SetText("10");
	fRunTimeEntry->SetRange(1, 1000);
	fRunTimeEntry->SetIncrement(1);
	fRunTimeEntry->AddToFocusList(&fFocusList);
	fRunTimeEntry->Associate(this);

	fTimeScale = new TGMrbRadioButtonList(fAccuFrame,  NULL, &fTauFitTimeScaleButtons, -1, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, rbuttonGC);
	HEAP(fTimeScale);
	fAccuFrame->AddFrame(fTimeScale, frameGC->LH());
	fTimeScale->SetState(DGFTauFitPanel::kDGFTauFitTimeScaleSecs, kButtonDown);

//	buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TGMrbTextButtonGroup(this, "Actions", &fTauFitActions, -1, 1, groupGC, buttonGC);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, buttonGC->LH());
	fButtonFrame->Associate(this);

//	no accu running
	fIsRunning = kFALSE;
	
	this->ResetValues();

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t DGFTauFitPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTauFitPanel::ProcessMessage
// Purpose:        Message handler for the instrument panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFTauFitPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;
	TMrbString dblStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFTauFitSelectColumn) {
						switch (Param1) {
							case kDGFTauFitStartFit:
								if (fIsRunning) {
									this->SetRunning(kFALSE);
									new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", "Aborting histogram acquisition", kMBIconExclamation);
								} else {
									this->TauFit();
								}
								break;
							case kDGFTauFitAbort:
								abortAccu = kTRUE;
								break;
							case kDGFTauFitReset:
								break;
							case kDGFTauFitSelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
								}
								break;
							case kDGFTauFitSelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
								break;							
						}
					} else {
						Param1 -= kDGFTauFitSelectColumn;
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
	intStr = fRunTimeEntry->GetEntry()->GetText();
	intStr.ToInteger(accuTime);
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

	TMrbString dblStr = fTauStartEntry->GetEntry()->GetText();
	Double_t tauStart;
	dblStr.ToDouble(tauStart);
	dblStr = fTauStepEntry->GetEntry()->GetText();
	Double_t tauStep;
	dblStr.ToDouble(tauStep);
	dblStr = fTauStopEntry->GetEntry()->GetText();
	Double_t tauStop;
	dblStr.ToDouble(tauStop);
	if (tauStart >= tauStop) {
		gMrbLog->Err()	<< "Illegal tau range - " << tauStart << " ... " << tauStop << endl;
		gMrbLog->Flush(this->ClassName(), "TauFit");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Illegal tau range", kMBIconExclamation);
		return(kFALSE);
	}

	nofHistos = 0;
	TFile * taufitFile = NULL;
	Double_t tau = tauStart;
	Bool_t initFlag = kTRUE;
	while (tau <= tauStop) {
		dgfModule = gDGFControlData->FirstModule();
		nofModules = 0;
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

		cout << "[Tau: " << tau << " - accumulating " << accuTime << " " << tp->GetName() << "(s) - wait ... " << ends << flush;
		abortAccu = kFALSE;
		if (offlineMode) secsToWait = 1;
		for (Int_t i = 0; i < secsToWait; i++) {
			sleep(1);
			gSystem->ProcessEvents();
			if (abortAccu) {
				gMrbLog->Err() << "Aborted after " << i << " secs. Stopping current run." << endl;
				gMrbLog->Flush(this->ClassName(), "TauFit");
				break;
			}	
		}
		if (abortAccu) break; else cout << "done]" << endl;

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
									TMrbString hName = h->GetName();
									hName += ".tau";
									hName += tau;
									TMrbString hTitle = h->GetTitle();
									hTitle += "; tau = ";
									hTitle += tau;
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
		tau += tauStep;
	}

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

	fRunTimeEntry->GetEntry()->SetText("10");
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
