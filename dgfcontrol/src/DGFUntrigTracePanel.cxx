//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFUntrigTracePanel.cxx,v 1.24 2005-09-08 13:56:38 Rudolf.Lutter Exp $       
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

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;

	TMrbString camacHost;
	TMrbString intStr;
			
	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;
	TMrbLofNamedX lofModuleKeys;
	
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	for (Int_t i = 0; i < kNofTraceBuffers; i++) lofTraceBuffers[i] = NULL;

//	clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
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
								gDGFControlData->fColorWhite);	HEAP(entryGC);
	
	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
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

	fXFrame = new TGGroupFrame(fHFrame, "WaitStates", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fXFrame);
	fHFrame->AddFrame(fXFrame, groupGC->LH());

	fXwait = new TGMrbLabelEntry(fXFrame, "XWAIT",
																200, kDGFUntrigTraceXwait,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fXwait);
	fXFrame->AddFrame(fXwait, frameGC->LH());
	fXwait->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fXwait->SetText(0);
	fXwait->SetRange(0, 1000);
	fXwait->SetIncrement(1);
	fXwait->AddToFocusList(&fFocusList);

// action buttons
	TGLayoutHints * aFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	gDGFControlData->SetLH(groupGC, frameGC, aFrameLayout);
	HEAP(aFrameLayout);
	TGLayoutHints * aButtonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
	labelGC->SetLH(aButtonLayout);
	HEAP(aButtonLayout);
	fActionFrame = new TGMrbTextButtonGroup(this, "Actions", &fActions, -1, 1, groupGC, labelGC);
	HEAP(fActionFrame);
	this->AddFrame(fActionFrame, groupGC->LH());
	fActionFrame->JustifyButton(kTextCenterX);
	fActionFrame->Associate(this);

	this->ChangeBackground(gDGFControlData->fColorGreen);

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t DGFUntrigTracePanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFUntrigTracePanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFUntrigTracePanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFUntrigTraceSelectColumn) {
						switch (Param1) {
							case kDGFUntrigTraceStart:
								this->StartTrace();
								break;
							case kDGFUntrigTraceAbort:
								lofDgfs.Abort();
								break;
							case kDGFUntrigTraceSelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
								break;
							case kDGFUntrigTraceSelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
								break;							
							default:	break;
						}
					} else {
						Param1 -= kDGFUntrigTraceSelectColumn;
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
				default:	break;
			}
			break;

	}
	return(kTRUE);
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

	xwait = fXwait->GetText2Int();
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
