//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMESis3302StartTracePanel
// Purpose:        A GUI to control a SIS 3302 adc
// Description:    Collect traces
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302StartTracePanel.cxx,v 1.14 2012-01-18 11:11:32 Marabou Exp $
// Date:
// URL:
// Keywords:
// Layout:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include "TEnv.h"
#include "TROOT.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TFile.h"

#include "TGFileDialog.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbLofDGFs.h"
#include "TMrbSystem.h"
#include "TMrbEnv.h"
#include "TGMrbProgressBar.h"

#include "TC2LSis3302.h"
#include "VMESis3302Panel.h"
#include "VMESis3302StartTracePanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

const SMrbNamedXShort kVMESis302StartTraceModes[] =
			{
				{VMESis3302StartTracePanel::kVMESis3302ModeMAWD,		"MAWD"	},
				{VMESis3302StartTracePanel::kVMESis3302ModeMAW,			"MAW"	},
				{VMESis3302StartTracePanel::kVMESis3302ModeMAWFT,		"MAWFT"	},
				{0, 								NULL	}
			};

extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;
extern TMrbC2Lynx * gMrbC2Lynx;

static TC2LSis3302 * curModule = NULL;
static Int_t curChannel = 0;
static Int_t traceMode = 0;

ClassImp(VMESis3302StartTracePanel)


VMESis3302StartTracePanel::VMESis3302StartTracePanel(const TGWindow * Window, TMrbLofNamedX * LofModules,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel
// Purpose:        VMEControl: Save & Restore
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * chkbtnGC;
	TGMrbLayout * comboGC;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	fTraceCollection = kFALSE;

// geometry
	Int_t frameWidth = this->GetWidth();
	Int_t frameHeight = this->GetHeight();

//	Clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGold);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gVMEControlData->SlantedFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGold);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGold);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGold);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGray);	HEAP(buttonGC);

	chkbtnGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGold);	HEAP(chkbtnGC);

	entryGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorWhite);	HEAP(entryGC);

	TGLayoutHints * loXpndX = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 10, 10, 1, 1);
	TGLayoutHints * loXpndX2 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	TGLayoutHints * loNormal = new TGLayoutHints(kLHintsTop, 10, 10, 1, 1);
	TGLayoutHints * loNormal2 = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	HEAP(loXpndX); HEAP(loXpndX2); HEAP(loNormal); HEAP(loNormal2);

	frameGC->SetLH(loNormal);
	comboGC->SetLH(loNormal2);
	labelGC->SetLH(loNormal);
	buttonGC->SetLH(loNormal);
	entryGC->SetLH(loNormal);
	groupGC->SetLH(loXpndX);

// special buttons
	TObjArray * lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));

// modules
	fLofModules = LofModules;

	fSelectFrame = new TGGroupFrame(this, "Select module", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, groupGC->LH());

	fSelectModule = new TGMrbLabelCombo(fSelectFrame, "Module",	fLofModules,
																kVMESis3302SelectModule, 1,
																frameWidth/5, kLEHeight, frameWidth/5,
																frameGC, labelGC, comboGC, labelGC, kTRUE);
	fSelectFrame->AddFrame(fSelectModule, frameGC->LH());
	fSelectModule->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ModuleChanged(Int_t, Int_t)");

	TMrbLofNamedX lofChanPatterns;
	gVMEControlData->GetLofChannels(lofChanPatterns, VMESis3302Panel::kVMENofSis3302Chans, "%d", kTRUE);

	fSelectChanPatt = new TGMrbCheckButtonList(fSelectFrame, "Channel", &lofChanPatterns,
																kVMESis3302SelectChanPatt, 1,
																frameWidth, kLEHeight,
																frameGC, labelGC, chkbtnGC, lofSpecialButtons);
	fSelectFrame->AddFrame(fSelectChanPatt, frameGC->LH());
	fSelectChanPatt->ChangeBackground(gVMEControlData->fColorGold);
	fSelectChanPatt->SetState(0x1);

	fCloseButton = new TGTextButton(fSelectFrame, "Close", kVMESis3302Close);
	HEAP(fCloseButton);
	fSelectFrame->AddFrame(fCloseButton, groupGC->LH());
	fCloseButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302Close));

// canvas
	fHistoFrame = new TGGroupFrame(this, "Histograms", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fHistoFrame);
	this->AddFrame(fHistoFrame, groupGC->LH());
	fHistoCanvas = new TRootEmbeddedCanvas("HistoCanvas", fHistoFrame, frameWidth - 100, frameHeight*3/4);
	fHistoFrame->AddFrame(fHistoCanvas, frameGC->LH());
	fHistoCanvas->GetCanvas()->Divide(1, 2);

// trace buttons
	fTraceFrame = new TGGroupFrame(this, "Trace", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTraceFrame);
	this->AddFrame(fTraceFrame, groupGC->LH());

	fStartStopButton = new TGTextButton(fTraceFrame, "Start", kVMESis3302StartStop);
	HEAP(fStartStopButton);
	fTraceFrame->AddFrame(fStartStopButton, groupGC->LH());
	fStartStopButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302StartStop));

	fDumpTrace = kFALSE;
	fDumpTraceButton = new TGTextButton(fTraceFrame, "Dump trace ON", kVMESis3302DumpTrace);
	HEAP(fDumpTraceButton);
	fTraceFrame->AddFrame(fDumpTraceButton, groupGC->LH());
	fDumpTraceButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302DumpTrace));

	fWriteTraceButton = new TGTextButton(fTraceFrame, "Write traces", kVMESis3302WriteTrace);
	HEAP(fWriteTraceButton);
	fTraceFrame->AddFrame(fWriteTraceButton, groupGC->LH());
	fWriteTraceButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302WriteTrace));

	fWriteDeleteClonesButton = new TGTextButton(fTraceFrame, "Del clones", kVMESis3302DeleteClones);
	HEAP(fWriteDeleteClonesButton);
	fTraceFrame->AddFrame(fWriteDeleteClonesButton, groupGC->LH());
	fWriteDeleteClonesButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302DeleteClones));

	fMaxTraces = new TGMrbLabelEntry(fTraceFrame, "#traces",	200, kVMESis3302MaxTraces,
																frameWidth/5, kLEHeight, frameWidth/15,
																frameGC, labelGC);
	HEAP(fMaxTraces);
	fMaxTraces->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMaxTraces->SetText(0);
	fTraceFrame->AddFrame(fMaxTraces, frameGC->LH());

// mode
	fLofTraceModes.AddNamedX(kVMESis302StartTraceModes);
	fSelectTrigMode = new TGMrbLabelCombo(fTraceFrame, "Trigger",	&fLofTraceModes,
															kVMESis3302SelectTrigMode, 1,
															frameWidth/5, kLEHeight, frameWidth/8,
															frameGC, labelGC, comboGC, labelGC);
	fTraceFrame->AddFrame(fSelectTrigMode, frameGC->LH());
	fSelectTrigMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TraceModeChanged(Int_t, Int_t)");
	fSelectTrigMode->Select(kVMESis3302ModeMAWFT);
	this->TraceModeChanged(0, kVMESis3302ModeMAWFT);
	HEAP(fSelectTrigMode);

//	display
	fDisplayFrame = new TGGroupFrame(this, "Display", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDisplayFrame);
	this->AddFrame(fDisplayFrame, groupGC->LH());

	fTimeStamp = new TGMrbLabelEntry(fDisplayFrame, "TimeStamp",	200, kVMESis3302TimeStamp,
															frameWidth/5, kLEHeight, frameWidth/5,
															frameGC, labelGC);
	HEAP(fTimeStamp);
	fTimeStamp->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTimeStamp->SetText(0);
	fTimeStamp->GetTextEntry()->SetEnabled(kFALSE);
	fDisplayFrame->AddFrame(fTimeStamp, frameGC->LH());

	fTracesNo = new TGMrbLabelEntry(fDisplayFrame, "Trace#",	200, kVMESis3302TracesNo,
															frameWidth/5, kLEHeight, frameWidth/12,
															frameGC, labelGC);
	HEAP(fTracesNo);
	fTracesNo->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTracesNo->SetText(0);
	fTracesNo->GetTextEntry()->SetEnabled(kFALSE);
	fDisplayFrame->AddFrame(fTracesNo, frameGC->LH());

	fTracesPerSec = new TGMrbLabelEntry(fDisplayFrame, "Traces/s",	200, kVMESis3302TracesPerSecond,
															frameWidth/5, kLEHeight, frameWidth/12,
															frameGC, labelGC);
	HEAP(fTracesPerSec);
	fTracesPerSec->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTracesPerSec->SetText(0.0);
	fTracesPerSec->GetTextEntry()->SetEnabled(kFALSE);
	fDisplayFrame->AddFrame(fTracesPerSec, frameGC->LH());

	this->ChangeBackground(gVMEControlData->fColorGold);

//	key bindings
	fKeyBindings.SetParent(this);
	fKeyBindings.BindKey("Ctrl-q", TGMrbLofKeyBindings::kGMrbKeyActionExit);
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionClose);
	fKeyBindings.Connect("KeyPressed(Int_t, Int_t)", this->ClassName(), this, "KeyPressed(Int_t, Int_t)");

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(Width, Height);
	MapWindow();

	this->StartGUI();
}

void VMESis3302StartTracePanel::StartGUI() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel::StartGUI
// Purpose:        GUI starters
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Initializes GUI
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (curModule == NULL) {
		curModule = (TC2LSis3302 *) fLofModules->At(0);
		if (curModule == NULL) return;
		curModule->SetVerbose(gVMEControlData->IsVerbose());
		curModule->SetOffline(gVMEControlData->IsOffline());
	}

	fSelectModule->Select(curModule->GetIndex());
	fTraceCollection = kFALSE;
	fTraceFile = NULL;
	fLofRhistos.Delete();
	fLofEhistos.Delete();
	fLofClones.Delete();
}

void VMESis3302StartTracePanel::ModuleChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::ModuleChanged()
// Purpose:        Slot method: module selection changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fTraceCollection) {
		gVMEControlData->MsgBox(this, "ModuleChanged", "Error", "Stop trace collection first");
		return;
	}
	curModule = (TC2LSis3302 *) fLofModules->FindByIndex(Selection);
	curModule->SetVerbose(gVMEControlData->IsVerbose());
	curModule->SetOffline(gVMEControlData->IsOffline());
}

void VMESis3302StartTracePanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case VMESis3302StartTracePanel::kVMESis3302StartStop:
			if (fTraceCollection) this->StopTrace(); else this->StartTrace();
			break;
		case VMESis3302StartTracePanel::kVMESis3302DumpTrace:
			if (fDumpTrace) {
				fDumpTraceButton->SetText("Dump trace ON");
				fDumpTraceButton->SetBackgroundColor(gVMEControlData->fColorGreen);
				fDumpTraceButton->SetForegroundColor(gVMEControlData->fColorWhite);
				fDumpTrace = kFALSE;
			} else {
				fDumpTraceButton->SetText("Dump trace OFF");
				fDumpTraceButton->SetBackgroundColor(gVMEControlData->fColorRed);
				fDumpTraceButton->SetForegroundColor(gVMEControlData->fColorWhite);
				fDumpTrace = kTRUE;
			}
			fDumpTraceButton->Layout();
			curModule->DumpTrace();
			break;
		case VMESis3302StartTracePanel::kVMESis3302WriteTrace:
			this->WriteTrace();
			break;
		case VMESis3302StartTracePanel::kVMESis3302DeleteClones:
			this->DeleteClones();
			break;
		case VMESis3302StartTracePanel::kVMESis3302Close:
			this->KeyPressed(0, TGMrbLofKeyBindings::kGMrbKeyActionClose);
			break;
	}
}

void VMESis3302StartTracePanel::TraceModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TraceModeChanged
// Purpose:        Slot method: mode selection changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fTraceCollection) {
		gVMEControlData->MsgBox(this, "TraceModeChanged", "Error", "Stop trace collection first");
		return;
	}
	traceMode = ((TMrbNamedX *) fLofTraceModes[Selection])->GetIndex();
}

void VMESis3302StartTracePanel::ChannelChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::ChannelChanged()
// Purpose:        Slot method: channel selection changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fTraceCollection) {
		gVMEControlData->MsgBox(this, "ChannelChanged", "Error", "Stop trace collection first");
		return;
	}
	curChannel = Selection;
}

void VMESis3302StartTracePanel::StartTrace() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::StartTrace
// Purpose:        Start data acquisition
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Start run and collect traces
// Keywords:
//////////////////////////////////////////////////////////////////////////////

    	Int_t chnPatt = fSelectChanPatt->GetActive();
	curModule->SetTestBits(traceMode);

	Int_t nofEvents = 1;

	if (!curModule->StartTraceCollection(nofEvents, chnPatt)) {
		gVMEControlData->MsgBox(this, "StartTrace", "Error", "Couldn't get traces");
		return;
	}

	this->InitializeHistos(chnPatt);

	fTraceCollection = kTRUE;
	fStartStopButton->SetText("STOP");
	fStartStopButton->SetBackgroundColor(gVMEControlData->fColorRed);
	fStartStopButton->SetForegroundColor(gVMEControlData->fColorWhite);
	fStartStopButton->Layout();

	TArrayI evtData;
	Int_t traceNo = 0;
	Int_t maxTraces = fMaxTraces->GetText2Int();
	for (;;) {
		gSystem->ProcessEvents();
		if (!fTraceCollection) {
			curModule->StopTraceCollection();
			break;
		}
		if (maxTraces > 0 && traceNo >= maxTraces) {
			this->StopTrace();
			if (fTraceFile != NULL) this->WriteTrace();
			break;
		}

		TArrayI traceData(kSis3302NofChans * kSis3302EventPreHeader);
		traceNo++;
		traceData.Reset(0);
		if (!curModule->GetTraceLength(traceData, chnPatt)) {
			gVMEControlData->MsgBox(this, "StartTrace", "Error", "Couldn't get trace data");
			return;
		}
		TIterator * rIter = fLofRhistos.MakeIterator();
		TIterator * eIter = fLofEhistos.MakeIterator();
		TMrbNamedX * rnx;
		TMrbNamedX * enx;
		while (rnx = (TMrbNamedX *) rIter->Next()) {
			enx = (TMrbNamedX *) eIter->Next();
			Int_t chn = rnx->GetIndex() & 0xF;
			if (this->ReadData(evtData, rnx, enx, &traceData[chn * kSis3302EventPreHeader], traceNo) == -1) return;
			gSystem->ProcessEvents();
		}
	}
}

void VMESis3302StartTracePanel::StopTrace() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::StopTrace
// Purpose:        Stop data acquisition
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Stops run and collect traces
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fTraceCollection = kFALSE;
	fStartStopButton->SetText("Start");
	fStartStopButton->SetBackgroundColor(gVMEControlData->fColorGreen);
	fStartStopButton->SetForegroundColor(gVMEControlData->fColorWhite);
	fStartStopButton->Layout();
}

Int_t VMESis3302StartTracePanel::ReadData(TArrayI & EvtData, TMrbNamedX * RhistoDef, TMrbNamedX * EhistoDef, Int_t TraceData[], Int_t TraceNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::ReadData
// Purpose:        Read data from SIS3302
// Arguments:      TArrayI EvtData        -- where to store event data
//                 Int_t EventNo          -- event number
//                 TMrbNamedX * RhistoDef -- raw data: channel number, pad position, histo addr
//                 TMrbNamedX * EhistoDef -- energy data: ...
// Results:        NofData                -- word count
// Exceptions:
// Description:    Get event data from SIS3302 module(s)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t chn = RhistoDef->GetIndex() & 0xF;

	Int_t rdl = TraceData[0];
	Int_t edl = TraceData[1];
	Int_t wpt = TraceData[2];
	Int_t ect = TraceData[3];
	Int_t nxs = TraceData[4];
	
	Int_t wpt2 = wpt - rdl/2;
	if (nxs == 0 || wpt == 0 || wpt2 > nxs) return(0);

	Int_t nofEvents = nxs / wpt;
	if (nofEvents > ect) nofEvents = ect;

	Int_t e = 0;
	EvtData.Set(wpt + kSis3302EventPreHeader);
	if (!curModule->GetTraceData(EvtData, e, chn)) {
		gVMEControlData->MsgBox(this, "ReadData", "Error", "Couldn't get data");
		return(-1);
	}
	Int_t k = kSis3302EventPreHeader;

	ULong64_t ts = (UInt_t) EvtData[k + 1];
	UInt_t ts48 = ((UInt_t) EvtData[k] >> 16) & 0xFFFF;
	ts += ts48 * 0x100000000LL;
	fTimeStamp->GetTextEntry()->SetEnabled(kTRUE);
	fTimeStamp->SetText(Form("%lld", ts));
	fTimeStamp->GetTextEntry()->SetEnabled(kFALSE);

	fTracesNo->GetTextEntry()->SetEnabled(kTRUE);
	fTracesNo->SetText(TraceNumber);
	fTracesNo->GetTextEntry()->SetEnabled(kFALSE);

	Double_t secs = ts / 100000000.;
	Double_t tracesPerSec = TraceNumber / secs;
	fTracesPerSec->GetTextEntry()->SetEnabled(kTRUE);
	fTracesPerSec->SetText(tracesPerSec);
	fTracesPerSec->GetTextEntry()->SetEnabled(kFALSE);

	k = kSis3302EventPreHeader + kSis3302EventHeader;

	Int_t ksave = k;
	Int_t min = 1000000;
	Int_t max = 0;
	for (Int_t i = 0; i < rdl; i++, k++) {
		if (EvtData[k] < min) min = EvtData[k];
		if (EvtData[k] > max) max = EvtData[k];
	}
	if (max == 0) return(-1);

	TCanvas * c = fHistoCanvas->GetCanvas();

	Int_t subPad = (RhistoDef->GetIndex() >> 4) & 0xF;
	c->cd(subPad);
	TH1F * h = (TH1F *) RhistoDef->GetAssignedObject();

	k = ksave;
	h->SetName(Form("%s-%d", RhistoDef->GetName(), TraceNumber));
	h->SetTitle(Form("%s, trace %d", RhistoDef->GetTitle(), TraceNumber));
	h->Reset("ICE");
//	h->ResetStats();
	h->SetEntries(0);
	for (Int_t i = 0; i < rdl; i++, k++) h->Fill(i, EvtData[k]);
	h->Draw();
	if (fTraceFile) {
		h->Write();
		fNofTracesWritten++;
	}

	ksave = k;
	min = 1000000;
	max = 0;
	for (Int_t i = 0; i < edl; i++, k++) {
		if (EvtData[k] < min) min = EvtData[k];
		if (EvtData[k] > max) max = EvtData[k];
	}
	if (max == 0) return(-1);

	subPad = (EhistoDef->GetIndex() >> 4) & 0xF;
	c->cd(subPad);
	h = (TH1F *) EhistoDef->GetAssignedObject();

	k = ksave;
	h->SetName(Form("%s-%d", EhistoDef->GetName(), TraceNumber));
	h->SetTitle(Form("%s, trace %d", EhistoDef->GetTitle(), TraceNumber));
	h->Reset("ICE");
//	h->ResetStats();
	h->SetEntries(0);
	for (Int_t i = 0; i < edl; i++, k++) h->Fill(i, EvtData[k]);
	h->Draw();
	if (fTraceFile) h->Write();

	c->Update();
	c->cd();

	return(wpt);
}

Int_t VMESis3302StartTracePanel::InitializeHistos(UInt_t ChannelPattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::InitializeTraces
// Purpose:        Initialize traces
// Arguments:      UInt_t ChannelPattern   -- channel pattern
// Results:
// Exceptions:
// Description:    Fills histo database
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Int_t padX[] =		{ 1, 1, 1, 2, 2, 2, 3, 3 };
	const Int_t padY[] =		{ 2, 4, 6, 4, 6, 6, 6, 6 };

	TIterator * rIter = fLofRhistos.MakeIterator();
	TMrbNamedX * rnx;
	while (rnx = (TMrbNamedX *) rIter->Next()) {
		TH1F * h = (TH1F *) rnx->GetAssignedObject();
		if (h) delete h;
	}
	fLofRhistos.Delete();

	TIterator * eIter = fLofEhistos.MakeIterator();
	TMrbNamedX * enx;
	while (enx = (TMrbNamedX *) eIter->Next()) {
		TH1F * h = (TH1F *) enx->GetAssignedObject();
		if (h) delete h;
	}
	fLofEhistos.Delete();

	Int_t nofChannels = 0;
	UInt_t cp = ChannelPattern;
	for (Int_t chn = 0; chn < kSis3302NofChans; chn++, cp >>= 1) { if (cp & 1) nofChannels++; }

	TCanvas * c = fHistoCanvas->GetCanvas();
	c->Clear();
	c->Divide(padX[nofChannels - 1], padY[nofChannels - 1]);

	cp = ChannelPattern;
	Int_t padPos = 0;
	for (Int_t chn = 0; chn < kSis3302NofChans; chn++, cp >>= 1) {
		if (cp & 1) {
			Int_t rdl;
			curModule->ReadRawDataSampleLength(rdl, chn);
			TString hName = Form("Raw-%s-chn%d", curModule->GetName(), chn);
			TString hTitle = Form("Raw data from module %s, channel %d", curModule->GetName(), chn);
			TH1F * h = new TH1F(hName.Data(), hTitle.Data(), rdl, 0., (Axis_t) rdl);
			Int_t px = padX[nofChannels - 1];
			Int_t subPad = padPos + 1 + px * (padPos/px);
			Int_t index = (subPad << 4) | chn;
			TMrbNamedX * rnx = fLofRhistos.AddNamedX(index, hName.Data(), hTitle.Data(), h);
			c->cd(subPad);
			TPad * p = (TPad *) gPad;
			p->AddExec("clone", Form("((VMESis3302StartTracePanel*)%#lx)->CloneHisto((TMrbNamedX *)%#lx)", this, rnx));

			Int_t edl;
			curModule->ReadEnergySampleLength(edl, chn);
			hName = Form("Energy-%s-chn%d", curModule->GetName(), chn);
			hTitle = Form("Energy data from module %s, channel %d", curModule->GetName(), chn);
			h = new TH1F(hName.Data(), hTitle.Data(), edl, 0., (Axis_t) edl);
			subPad += px;
			index = (subPad << 4) | chn;
			TMrbNamedX * enx = fLofEhistos.AddNamedX(index, hName.Data(), hTitle.Data(), h);
			c->cd(subPad);
			p = (TPad *) gPad;
			p->AddExec("clone", Form("((VMESis3302StartTracePanel*)%#lx)->CloneHisto((TMrbNamedX *)%#lx)", this, enx));
			padPos++;
			if (nofChannels == 1) break;
		}
	}

	return(nofChannels);
}


void VMESis3302StartTracePanel::CloneHisto(TMrbNamedX * HistoDef) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::CloneHisto
// Purpose:        Show histogram as clone
// Arguments:      TMrbNameX * HistoDef  -- histogram defs
// Results:        --
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t event = gPad->GetEvent();
	if (event != kButton1Down) return;

	TH1F * h = (TH1F *) HistoDef->GetAssignedObject();
	TH1F * hclone = (TH1F *) h->Clone();
	TString hName = h->GetName();
	hclone->SetName(Form("%s_clone", hName.Data()));
	TCanvas * c = new TCanvas();
	fLofClones.Add(c);
	hclone->Draw();
}

void VMESis3302StartTracePanel::WriteTrace() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::WriteTrace
// Purpose:        Write traces to file
// Arguments:
// Results:
// Exceptions:
// Description:    Open root file and write traces
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fTraceCollection) {
		gVMEControlData->MsgBox(this, "WriteTrace", "Error", "Stop trace collection first");
		return;
	} else if (fTraceFile == NULL) {
		fTraceFile = new TFile("traces.root", "RECREATE");
		fNofTracesWritten = 0;
		fWriteTraceButton->SetText("Close trace file");
		fWriteTraceButton->SetBackgroundColor(gVMEControlData->fColorRed);
		fWriteTraceButton->SetForegroundColor(gVMEControlData->fColorWhite);
	} else {
		fTraceFile->Close();
		gMrbLog->Out() << fNofTracesWritten << " trace(s) written to file traces.root" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteTrace", setblue);
		delete fTraceFile;
		fTraceFile = NULL;
		fWriteTraceButton->SetText("Write traces");
		fWriteTraceButton->SetBackgroundColor(gVMEControlData->fColorGray);
		fWriteTraceButton->SetForegroundColor(gVMEControlData->fColorBlack);
	}
	fWriteTraceButton->Layout();
	
}

void VMESis3302StartTracePanel::KeyPressed(Int_t FrameId, Int_t Key) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::KeyPressed
// Purpose:        Slot method: handle special keys
// Arguments:      Int_t FrameId   -- frame issuing this signal
//                 Int_t Key       -- key code
// Results:
// Exceptions:
// Description:    Called if a special (=control) char has been typed
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Key) {
		case TGMrbLofKeyBindings::kGMrbKeyActionExit:
			if (fTraceCollection) {
				gVMEControlData->MsgBox(this, "KeyPressed", "Error", "Stop trace collection first");
				return;
			}
			if (gMrbC2Lynx) gMrbC2Lynx->Bye();
			gApplication->Terminate(0);
			break;
		case TGMrbLofKeyBindings::kGMrbKeyActionClose:
			if (fTraceCollection) {
				gVMEControlData->MsgBox(this, "KeyPressed", "Error", "Stop trace collection first");
				return;
			}
			this->CloseWindow();
			break;
	}
}

