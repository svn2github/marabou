//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFTraceDisplayPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Setup
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
<img src=dgfcontrol/DGFTraceDisplayPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include "TEnv.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TOrdCollection.h"

#include "TMrbLofDGFs.h"
#include "TMrbLogger.h"

#include "TGMsgBox.h"

#include "DGFControlData.h"
#include "DGFTraceDisplayPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

const SMrbNamedX kDGFTraceDisplayActions[] =
			{
				{DGFTraceDisplayPanel::kDGFTraceDisplayNormal,		"Normal trace",		"Take trace, leave trigger bits unchanged"		},
				{DGFTraceDisplayPanel::kDGFTraceDisplayAutoTrig,	"AutoTrig trace",	"Take trace, enable trigger for each channel"		},
				{0, 												NULL,				NULL						}
			};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

ClassImp(DGFTraceDisplayPanel)

DGFTraceDisplayPanel::DGFTraceDisplayPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel
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
	Int_t idx = kDGFTraceDisplaySelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFTraceDisplaySelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFTraceDisplaySelectNone, "cbutton_none.xpm");
		
//	Initialize several lists
	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	fActions.SetName("Actions");
	fActions.AddNamedX(kDGFTraceDisplayActions);

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

	fTFrame = new TGGroupFrame(fHFrame, "TraceLength", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTFrame);
	fHFrame->AddFrame(fTFrame, groupGC->LH());

	fTraceLength = new TGMrbLabelEntry(fTFrame, NULL,
																200, kDGFTraceDisplayXwait,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fTraceLength);
	fTFrame->AddFrame(fTraceLength, frameGC->LH());
	fTraceLength->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceLength->GetEntry()->SetText("0");
	fTraceLength->SetRange(0, 8000);
	fTraceLength->SetIncrement(100);
	fTraceLength->AddToFocusList(&fFocusList);

	fXFrame = new TGGroupFrame(fHFrame, "WaitStates", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fXFrame);
	fHFrame->AddFrame(fXFrame, groupGC->LH());

	fXwait = new TGMrbLabelEntry(fXFrame, NULL,
																200, kDGFTraceDisplayXwait,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fXwait);
	fXFrame->AddFrame(fXwait, frameGC->LH());
	fXwait->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fXwait->GetEntry()->SetText("0");
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
	fActionFrame = new TGMrbTextButtonGroup(this, "Actions", &fActions, 1, groupGC, labelGC);
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

Bool_t DGFTraceDisplayPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel::ProcessMessage
// Purpose:        Message handler for the setup panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFTraceDisplayPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString intStr;

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					if (Param1 < kDGFTraceDisplaySelectColumn) {
						switch (Param1) {
							case kDGFTraceDisplayNormal:
								this->StartTrace(kFALSE);
								break;
							case kDGFTraceDisplayAutoTrig:
								this->StartTrace(kTRUE);
								break;
							case kDGFTraceDisplaySelectAll:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
								}
								break;
							case kDGFTraceDisplaySelectNone:
								for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
									fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
								break;							
							default:	break;
						}
					} else {
						Param1 -= kDGFTraceDisplaySelectColumn;
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
			
		default:	break;
	}
	return(kTRUE);
}

Bool_t DGFTraceDisplayPanel::Update(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel::Update
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

void DGFTraceDisplayPanel::MoveFocus(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel::MoveFocus
// Purpose:        Move focus to next entry field
// Arguments:      Int_t EntryId     -- entry id
// Results:        --
// Exceptions:     
// Description:    Moves focus to next entry field in ring buffer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

}

Bool_t DGFTraceDisplayPanel::StartTrace(Bool_t AutoTrigFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFTraceDisplayPanel::StartTrace()
// Purpose:        Start untriggered trace
// Arguments:      Bool_t AutoTrigFlag    -- kTRUE: enable trigger for each channel
//                                        -- kFALSE: take trigger bits "as is"
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGFEventBuffer traceBuffer;
	TFile * traceFile = NULL;
	DGFModule * dgfModule;
	TMrbDGF * dgf;
	TH1F * h;
	Bool_t selectFlag, dataOkFlag;
	Int_t nofWords;
	Int_t modNo, cl;
	TString hTitle;
	Int_t nofModules, nofTraces, nofChannels;
	ofstream hl;
	TMrbString hName;
	UInt_t chnPattern;
	TMrbString intStr;
	Int_t xwait;
	Int_t traceLength;
										
	Bool_t offlineMode = gDGFControlData->IsOffline();

	traceBuffer.Set(4 * 8192);
	
	nofTraces = 0;
	chnPattern = fSelectChannel->GetActive() >> 12;
	nofChannels = 0;
	for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (chnPattern & (1 << chn)) nofChannels++;
	}
 	if (nofChannels == 0) {
		gMrbLog->Err()	<< "No channels selected" << endl;
		gMrbLog->Flush(this->ClassName(), "StartTrace");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one channel", kMBIconStop);
		return(kFALSE);
	}

	intStr = fTraceLength->GetEntry()->GetText();
	intStr.ToInteger(traceLength);

	Int_t tLength;
	switch (nofChannels) {
		case 1: tLength = 8000; break;
		case 2: tLength = 4000; break;
		case 3: tLength = 2000; break;
		case 4: tLength = 2000; break;
	}
	if (traceLength == 0) traceLength = tLength;

	if (traceLength > tLength) {
		gMrbLog->Err()	<< "StartTrace(): Tracelength too long - " << traceLength
						<< " (should be " << tLength << " max for " << nofChannels 
						<< " channels)" << endl;
		gMrbLog->Flush(this->ClassName(), "StartTrace");
		return(kTRUE);
	}

	intStr = fXwait->GetEntry()->GetText();
	intStr.ToInteger(xwait);
	selectFlag = kFALSE;
	dgfModule = gDGFControlData->FirstModule();
	nofModules = 0;
	TMrbLofDGFs lofDgfs;
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			selectFlag = kTRUE;
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				selectFlag = kTRUE;
				lofDgfs.AddModule(dgf, kTRUE);
				dgf->GetTrace_Init(traceLength, chnPattern, xwait, AutoTrigFlag);
				dgf->GetTrace_Start();
			}
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}				

	if (!selectFlag) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module", kMBIconStop);
		return(kFALSE);
	}

	if (!offlineMode) lofDgfs.WaitActive(5);

	dgfModule = gDGFControlData->FirstModule();
	nofModules = 0;
	dataOkFlag = kFALSE;
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				if (!dgf->ActiveBit()) { 
					traceBuffer.Reset();
					nofWords = dgf->GetTrace_Stop(traceBuffer, 0);
					if (gDGFControlData->IsDebug()) {
						TMrbString fn = dgf->GetName();
						fn += ".param.dat";
						dgf->PrintParamsToFile(fn.Data());
						fn = dgf->GetName();
						fn += ".evtbuf.dat";
						traceBuffer.PrintToFile(fn.Data(), dgf->GetTitle());
					}
					if (nofWords > 0) {
						if (!dataOkFlag) {
							traceFile = new TFile("trace.root", "RECREATE");
							hl.open("trace.histlist", ios::out);
						}
						dataOkFlag = kTRUE;
						if (traceBuffer.GetNofEvents() > 0) {
							for (Int_t chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
								if ( (h = traceBuffer.FillHistogram(0, chn, kFALSE)) ) {
									hl << dgfModule->GetName() << chn << endl;
									hName = dgfModule->GetName();
									hName += chn;
									h->SetName(hName.Data());
									TString title = h->GetTitle();
									title.Prepend(": ");
									title.Prepend(dgfModule->GetTitle());
									h->SetTitle(title.Data());
									h->Write();
									if (gDGFControlData->IsVerbose()) {
										cout	<< "[" << dgfModule->GetTitle()
												<< "(chn" << chn
												<< "): trace" << nofTraces
												<< " -> histo " << hName
												<< ", " << traceLength << " data points]" << endl;
									}
									nofTraces++;
								}
							}
							gMrbLog->Out()	<< "[" << dgfModule->GetName() << "] "
											<< nofWords << " trace data written" << endl;
							gMrbLog->Flush(this->ClassName(), "StartTrace", setblue);
						}
					}
					if (traceBuffer.GetNofEvents() == 0) {
						gMrbLog->Err()	<< "[" << dgfModule->GetName() << "] DGF in C"
										<< dgf->GetCrate() << ".N" << dgf->GetStation()
										<< ": Unable to acquire traces (buffer empty)" << endl;
						gMrbLog->Flush(this->ClassName(), "StartTrace");
					}
				}
			}
		}
		dgfModule = gDGFControlData->NextModule(dgfModule);
		nofModules++;
	}				

	if (dataOkFlag) {
		traceFile->Close();
		hl.close();
	}
	if (offlineMode || dataOkFlag) {
		gMrbLog->Out()	<< "StartTrace(): " << nofTraces << " traces written to file \"trace.root\"";
		if (AutoTrigFlag) gMrbLog->Out() << " (AutoTrig mode ON)";
		gMrbLog->Out() << endl;
		gMrbLog->Flush(this->ClassName(), "StartTrace", setblue);
		return(kTRUE);
	} else {
		gMrbLog->Err()	<< "StartTrace(): Couldn't get any traces" << endl;
		gMrbLog->Flush(this->ClassName(), "StartTrace");
		return(kTRUE);
	}
}
