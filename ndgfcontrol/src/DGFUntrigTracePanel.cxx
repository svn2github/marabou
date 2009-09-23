//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFUntrigTracePanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFUntrigTracePanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include "TEnv.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TOrdCollection.h"

#include "TMrbLofDGFs.h"
#include "TMrbLogger.h"
#include "TMrbHistory.h"

#include "TGMsgBox.h"

#include "DGFControlData.h"
#include "DGFUntrigTracePanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

const SMrbNamedX kDGFUntrigTraceActions[] =
			{
				{DGFUntrigTracePanel::kDGFUntrigTraceStart,		"Untrig trace",	"Take untriggered trace (control task 4)"	},
				{DGFUntrigTracePanel::kDGFUntrigTraceAbort,		"Abort",	"Abort trace taking"	},
				{0, 											NULL,					NULL		}
			};

enum	{	kNofTraceBuffers	=	1000	};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

static TArrayI * lofTraceBuffers[kNofTraceBuffers];
static TMrbLofDGFs lofDgfs;

ClassImp(DGFUntrigTracePanel)

DGFUntrigTracePanel::DGFUntrigTracePanel(TGCompositeFrame * TabFrame) :
					TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel
// Purpose:        DGF Viewer: Setup Panel
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:        
// Exceptions:     
// Description:    Implements DGF Viewer's Untrig Trace Panel
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////// Profiles Laden////////////////////////////////////////

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
// 	stdProfile = gMrbLofProfiles->GetDefault();
	stdProfile = gMrbLofProfiles->FindProfile("green", kTRUE);
	//yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	//blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);

	gMrbLofProfiles->Print();

////////////////////////////////////////////////////////////////////////////////////////////////

	/*TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;*/

	TMrbString camacHost;
	TMrbString intStr;
			
	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;
	
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	for (Int_t i = 0; i < kNofTraceBuffers; i++) lofTraceBuffers[i] = NULL;

	TGLayoutHints * expandXLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,2,5,2);
	HEAP(expandXLayout);


//	clear focus list
	fFocusList.Clear();

	/*frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);
	
	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);
	
	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(labelGC);
	
	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);*/
	
	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TNGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFUntrigTraceSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFUntrigTraceSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFUntrigTraceSelectNone, "cbutton_none.xpm");
	
	
//	Initialize several lists
	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	fActions.SetName("Actions");
	fActions.AddNamedX(kDGFUntrigTraceActions);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

// modules
	fModules = new TGGroupFrame(this, "Modules", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fModules);
	this->AddFrame(fModules,dgfFrameLayout);

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TNGMrbCheckButtonList(fModules,  NULL,
							gDGFControlData->CopyKeyList(&fLofDGFModuleKeys[cl], cl, 1, kTRUE), -1,stdProfile, 1,0,
							kTabWidth, kLEHeight,0,
							lofSpecialButtons,kTRUE);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl],expandXLayout);
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled, kFALSE);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown, kFALSE);
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

	fSelectChannel = new TNGMrbCheckButtonGroup(fHFrame,  "Channel(s)", &fLofChannels, -1,stdProfile, 1,0,0,lofSpecialButtons,kTRUE);
	HEAP(fSelectChannel);
 	fSelectChannel->SetState(kDGFChannelMask, kButtonDown, kFALSE);
	fHFrame->AddFrame(fSelectChannel, dgfFrameLayout);

	fXFrame = new TGGroupFrame(fHFrame, "WaitStates", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fXFrame);
	fHFrame->AddFrame(fXFrame, dgfFrameLayout);

	TGLayoutHints * dgfLabelEntryLayout = new TGLayoutHints(kLHintsRight | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfLabelEntryLayout);

	fXwait = new TNGMrbLabelEntry(fXFrame, "XWAIT",
																kDGFUntrigTraceXwait,stdProfile,
																kLEWidth,
																kLEHeight,
																150,TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fXwait);
	fXFrame->AddFrame(fXwait, dgfLabelEntryLayout);
// 	fXwait->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fXwait->SetFormat(TGNumberEntry::kNESInteger);
	fXwait->SetIntNumber(0);
// 	fXwait->SetRange(0, 1000);
	fXwait->SetLimits(TGNumberEntry::kNELLimitMinMax,0,1000);
	fXwait->SetIncrement(1);
	fXwait->AddToFocusList(&fFocusList);

// action buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
// 	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);
	TGLayoutHints * aButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
// 	labelGC->SetLH(aButtonLayout);
	HEAP(aButtonLayout);
	fActionFrame = new TNGMrbTextButtonGroup(this, "Actions", &fActions, -1,stdProfile, 1,0);
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, aFrameLayout);
	fActionFrame->JustifyButton(kTextCenterX);
	((TNGMrbButtonFrame *) fActionFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gDGFControlData->fColorGreen);

	dgfFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);
	this->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	TabFrame->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFUntrigTracePanel::SelectModule(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel::SelectModule
// Purpose:        Slot method: select module
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFUntrigTraceSelectColumn) {
		switch (Selection) {
			case kDGFUntrigTraceSelectAll:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
				break;
			case kDGFUntrigTraceSelectNone:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
				break;							
		}
	} else {
		Selection -= kDGFUntrigTraceSelectColumn;
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

void DGFUntrigTracePanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFUntrigTraceStart:
			this->StartTrace();
			break;
		case kDGFUntrigTraceAbort:
			lofDgfs.Abort();
			break;
	}
}

Bool_t DGFUntrigTracePanel::StartTrace() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel::StartTrace()
// Purpose:        Start untriggered trace
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TFile * traceFile = NULL;
	TArrayI * traceBuffer;
	DGFModule * dgfModule;
	Int_t modNo, cl;
	TMrbDGF * dgf;
	TH1F * h;
	Bool_t selectFlag;
	Int_t nofWords;
	TString hTitle;
	Int_t nofModules;
	Int_t nofTraces = 0;
	ofstream hl;
	UInt_t chnPattern;
	Int_t xwait;
	TMrbString intStr;
										
	TArrayI wpc(TMrbDGFData::kNofChannels * kNofTraceBuffers);
	wpc.Reset();

	Bool_t verbose = gDGFControlData->IsVerbose();
	Bool_t offlineMode = gDGFControlData->IsOffline();

	chnPattern = (fSelectChannel->GetActive()) & 0xF;
	if (chnPattern == 0) {
		gMrbLog->Err()	<< "No channels selected" << endl;
		gMrbLog->Flush(this->ClassName(), "StartTrace");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one channel", kMBIconStop);
		return(kFALSE);
	}

	xwait = fXwait->GetIntNumber();
	dgfModule = gDGFControlData->FirstModule();
	selectFlag = kFALSE;
	nofModules = 0;
	Int_t nentries = 0;
	lofDgfs.Clear();
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			selectFlag = kTRUE;
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				if (gDGFControlData->IsDebug()) {
					TMrbString fn = dgf->GetName();
					fn += ".param.dat";
					dgf->WriteParamsToFile(fn.Data());
				}
				traceBuffer = lofTraceBuffers[nentries];
				if (traceBuffer == NULL) {
					traceBuffer = new TArrayI();
					lofTraceBuffers[nentries] = traceBuffer;
				}
				traceBuffer->Set(TMrbDGFData::kUntrigTraceLength * TMrbDGFData::kNofChannels);
				traceBuffer->Reset();
				dgf->GetUntrigTrace_Init(*traceBuffer, chnPattern, xwait);
				lofDgfs.AddModule(dgf, kTRUE);
				nentries++;
			}
		}
		nofModules++;
		dgfModule = gDGFControlData->NextModule(dgfModule);
	}				
	if (!selectFlag) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}

	UInt_t chnp = chnPattern;
	nofWords = 0;
	lofDgfs.ResetAbort();
	if (!verbose) cout << "[Taking untrig traces for chn " << ends << flush;
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (chnp & 1) {
			if (!verbose) cout << chn << " " << ends << flush;
			dgf = (TMrbDGF *) lofDgfs.First();
			while (dgf) {
				dgf->GetUntrigTrace_Start(chn);
				dgf = (TMrbDGF *) lofDgfs.After(dgf);
			}

			if (!offlineMode) lofDgfs.WaitActive(5);

			dgf = (TMrbDGF *) lofDgfs.First();
			nofModules = 0;
			while (dgf) {
				traceBuffer = lofTraceBuffers[nofModules];
				Int_t n = dgf->GetUntrigTrace_Stop(chn, *traceBuffer, 0);
				wpc[chn * kNofTraceBuffers + nofModules] = n;
				dgf = (TMrbDGF *) lofDgfs.After(dgf);
				nofModules++;
				nofWords += n;
			}
		}
		chnp >>= 1;
	}
	if (!verbose) cout << "done]" << endl;

	dgf = (TMrbDGF *) lofDgfs.First();
	while (dgf) {
		dgf->GetUntrigTrace_Restore();
		dgf = (TMrbDGF *) lofDgfs.After(dgf);
	}

	if (nofWords > 0) {
		traceFile = new TFile("untrigTrace.root", "RECREATE");
		hl.open("untrigTrace.histlist", ios::out);
		nofTraces = 0;
		dgf = (TMrbDGF *) lofDgfs.First();
		nofModules = 0;
		while (dgf) {
			Int_t td = 0;
			for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
				td += wpc[chn * kNofTraceBuffers + nofModules];
			}
			if (td > 0) {
				hTitle = "Untrig traces for module ";
				hTitle += dgf->GetName();
				h = new TH1F(dgf->GetName(), hTitle.Data(), td, 0., td);
				TMrbString chnList = "chn ";
				Int_t n = 0;
				traceBuffer = lofTraceBuffers[nofModules];
				for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
					Int_t offset = chn * TMrbDGFData::kUntrigTraceLength;
					Int_t wc = wpc[chn * kNofTraceBuffers + nofModules];
					if (wc > 0) {
						chnList += chn;
						chnList += " ";
						for (Int_t i = 0; i < wc; i++, n++, offset++) {
							h->Fill((Axis_t) n, traceBuffer->At(offset));
						}
					}
				}
				hTitle += ": ";
				hTitle += chnList.Data();
				h->SetTitle(hTitle.Data());
				h->Write();
				hl << dgf->GetName() << endl;
				nofTraces++;
				gMrbLog->Out()	<< "StartTrace(): [" << dgf->GetName() << "] "
								<< td << " untrig trace data written" << endl;
				gMrbLog->Flush(this->ClassName(), "StartTrace", setblue);
			}
			dgf = (TMrbDGF *) lofDgfs.After(dgf);
			nofModules++;
		}
	}				
	if (nofTraces > 0) {
		traceFile->Close();
		hl.close();
	}
	if (offlineMode || (nofTraces > 0)) {
		gMrbLog->Out()	<< "StartTrace(): " << nofTraces << " traces written to file \"untrigTrace.root\"" << endl;
		gMrbLog->Flush(this->ClassName(), "StartTrace", setblue);
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "StartTrace(): Couldn't get any traces" << endl;
		gMrbLog->Flush(this->ClassName(), "StartTrace");
		return(kTRUE);
	}
}
