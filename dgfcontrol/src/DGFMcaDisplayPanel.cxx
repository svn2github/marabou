//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFMcaDisplayPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    MCA Display
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
				{DGFMcaDisplayPanel::kDGFMcaDisplayAbort,			"Abort",			"Abort accumulation"	},
				{DGFMcaDisplayPanel::kDGFMcaDisplayReset,			"Reset",			"Reset to default values"	},
				{0, 															NULL,				NULL						}
			};

const SMrbNamedXShort kDGFMcaTimeScaleButtons[] =
							{
								{DGFMcaDisplayPanel::kDGFMcaTimeScaleSecs, 		"secs"		},
								{DGFMcaDisplayPanel::kDGFMcaTimeScaleMins, 		"mins"		},
								{DGFMcaDisplayPanel::kDGFMcaTimeScaleHours, 	"hours" 	},
								{0, 											NULL		}
							};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

static Bool_t abortAccu = kFALSE;

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
							gDGFControlData->CopyKeyList(&fLofDGFModuleKeys[cl], cl, 1, kTRUE), 1, 
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
		fGroupSelect[i] = new TGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		fGroupSelect[i]->Associate(this);
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, 1, 
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

	fSelectChannel = new TGMrbCheckButtonGroup(fHFrame,  "Channel(s)", &fLofChannels, 1,
												groupGC, buttonGC, lofSpecialButtons);
	HEAP(fSelectChannel);
 	fSelectChannel->SetState(kDGFChannelMask, kButtonDown);
	fHFrame->AddFrame(fSelectChannel, frameGC->LH());

// accu settings
	TGLayoutHints * accuLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, accuLayout);
	HEAP(accuLayout);
	fAccuFrame = new TGGroupFrame(fHFrame, "Accu Settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fAccuFrame);
	fHFrame->AddFrame(fAccuFrame, frameGC->LH());

	TGLayoutHints * teLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	entryGC->SetLH(teLayout);
	HEAP(teLayout);
	fRunTimeEntry = new TGMrbLabelEntry(fAccuFrame, "Run time",
																200, kDGFMcaDisplayRunTime,
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

	fTimeScale = new TGMrbRadioButtonList(fAccuFrame,  NULL, &fMcaTimeScaleButtons, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, rbuttonGC);
	HEAP(fTimeScale);
	fAccuFrame->AddFrame(fTimeScale, frameGC->LH());
	fTimeScale->SetState(DGFMcaDisplayPanel::kDGFMcaTimeScaleSecs, kButtonDown);

//	buttons
	TGLayoutHints * btnLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 1);
	buttonGC->SetLH(btnLayout);
	HEAP(btnLayout);
	fButtonFrame = new TGMrbTextButtonGroup(this, "Actions", &fMcaActions, 1, groupGC, buttonGC);
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
								if (fIsRunning) {
									this->SetRunning(kFALSE);
									new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Warning", "Aborting histogram acquisition", kMBIconExclamation);
								} else {
									this->AcquireHistos();
								}
								break;
							case kDGFMcaDisplayAbort:
								abortAccu = kTRUE;
								break;
							case kDGFMcaDisplayReset:
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

	TMrbDGFHistogramBuffer histoBuffer;
	DGFModule * dgfModule;
	TMrbDGF * dgf;
	Int_t modNo, cl;
	Int_t nofModules, nofHistos, nofWords;
	TMrbString intStr;
	Int_t accuTime;
	TString timeScale;
										
	Bool_t verbose = (gDGFControlData->fStatus & DGFControlData::kDGFVerboseMode) != 0;
	Bool_t offlineMode = gDGFControlData->IsOffline();

	TMrbNamedX * tp = fMcaTimeScaleButtons.FindByIndex(fTimeScale->GetActive());
	intStr = fRunTimeEntry->GetEntry()->GetText();
	intStr.ToInteger(accuTime);
	Int_t waitInv;
	switch (tp->GetIndex()) {
		case kDGFMcaTimeScaleSecs:	waitInv = 1; break;
		case kDGFMcaTimeScaleMins:	waitInv = 60; break;
		case kDGFMcaTimeScaleHours: waitInv = 60 * 60; break;
	}
	Int_t secsToWait = accuTime * waitInv;

	UInt_t chnPattern = fSelectChannel->GetActive() >> 12;
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
				dgf->AccuHist_Start();
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

	cout << "[Accumulating " << accuTime << " " << tp->GetName() << "(s) - wait ... " << ends << flush;
	abortAccu = kFALSE;
	if (offlineMode) secsToWait = 1;
	for (Int_t i = 0; i < secsToWait; i++) {
		sleep(1);
		gSystem->ProcessEvents();
		if (abortAccu) {
			gMrbLog->Err() << "Aborted after " << i << " secs. Stopping current run." << endl;
			gMrbLog->Flush(this->ClassName(), "AccuHistograms");
			break;
		}	
	}
	if (!abortAccu) cout << "done]" << endl;

	nofHistos = 0;
	nofModules = 0;
	dgfModule = gDGFControlData->FirstModule();
	TFile * mcaFile = NULL;
	ofstream listFile;
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				histoBuffer.Reset();
				histoBuffer.SetModule(dgf);
				if (dgf->AccuHist_Stop(0)) {
					nofWords = dgf->ReadHistogramsViaRsh(histoBuffer, chnPattern);
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

void DGFMcaDisplayPanel::SetRunning(Bool_t RunFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFMcaDisplayPanel::SetRunning
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

	nx = fMcaActions.FindByIndex(kDGFMcaDisplayAcquire);
	btn = (TGTextButton *) fButtonFrame->GetButton(kDGFMcaDisplayAcquire);

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
