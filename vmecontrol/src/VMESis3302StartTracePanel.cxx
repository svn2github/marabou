//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMESis3302StartTracePanel
// Purpose:        A GUI to control a SIS 3302 adc
// Description:    Collect traces
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302StartTracePanel.cxx,v 1.7 2010-10-27 11:02:31 Marabou Exp $
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

	fDumpTraceButton = new TGTextButton(fTraceFrame, "Dump trace", kVMESis3302DumpTrace);
	HEAP(fDumpTraceButton);
	fTraceFrame->AddFrame(fDumpTraceButton, groupGC->LH());
	fDumpTraceButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302DumpTrace));

	fWriteTraceButton = new TGTextButton(fTraceFrame, "Write traces", kVMESis3302WriteTrace);
	HEAP(fWriteTraceButton);
	fTraceFrame->AddFrame(fWriteTraceButton, groupGC->LH());
	fWriteTraceButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302WriteTrace));

	fMaxTraces = new TGMrbLabelEntry(fTraceFrame, "# traces",	200, kVMESis3302MaxTraces,
																frameWidth/5, kLEHeight, frameWidth/10,
																frameGC, labelGC);
	HEAP(fMaxTraces);
	fMaxTraces->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMaxTraces->SetText(0);
	fTraceFrame->AddFrame(fMaxTraces, frameGC->LH());

// mode
	fLofTraceModes.AddNamedX(kVMESis302StartTraceModes);
	fSelectTrigMode = new TGMrbLabelCombo(fTraceFrame, "TriggerMode",	&fLofTraceModes,
															kVMESis3302SelectTrigMode, 1,
															frameWidth/5, kLEHeight, frameWidth/6,
															frameGC, labelGC, comboGC, labelGC);
	fTraceFrame->AddFrame(fSelectTrigMode, frameGC->LH());
	fSelectTrigMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TraceModeChanged(Int_t, Int_t)");
	fSelectTrigMode->Select(0);
	HEAP(fSelectTrigMode);

//	display
	fDisplayFrame = new TGGroupFrame(this, "Display", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDisplayFrame);
	this->AddFrame(fDisplayFrame, groupGC->LH());

	TMrbLofNamedX lofChannels;
	gVMEControlData->GetLofChannels(lofChannels, VMESis3302Panel::kVMENofSis3302Chans, "chn %d");

	fSelectChannel = new TGMrbLabelCombo(fDisplayFrame, "Channel",	&lofChannels,
																kVMESis3302SelectChannel, 1,
																frameWidth/5, kLEHeight, frameWidth/5,
																frameGC, labelGC, comboGC, labelGC, kTRUE);
	fDisplayFrame->AddFrame(fSelectChannel, frameGC->LH());
	fSelectChannel->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ChannelChanged(Int_t, Int_t)");
	fSelectChannel->Select(0);

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
	fHistoRaw = NULL;
	fHistoEnergy = NULL;
	fTraceFile = NULL;
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
			curModule->DumpTrace();
			break;
		case VMESis3302StartTracePanel::kVMESis3302WriteTrace:
			this->WriteTrace();
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
		Int_t curEvent = 0;
		Int_t wpt = 0;
		traceNo++;
		while (fTraceCollection && wpt == 0) {
			wpt = this->ReadData(evtData, curEvent, curChannel, traceNo);
			gSystem->ProcessEvents();
			if (wpt == 0) sleep(1);
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

Int_t VMESis3302StartTracePanel::ReadData(TArrayI & EvtData, Int_t EventNo, Int_t Channel, Int_t TraceNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::ReadData
// Purpose:        Read data from SIS3302
// Arguments:      TArrayI EvtData       -- where to store event data
//                 Int_t EventNo         -- event number
//                 Int_t Channel         -- channel number
//                 Int_t TraceNumber     -- trace number
// Results:        NofData               -- word count
// Exceptions:
// Description:    Get event data from SIS3302 module(s)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TArrayI traceLength(kSis3302EventPreHeader);
	if (!curModule->GetTraceLength(traceLength, Channel)) {
		gVMEControlData->MsgBox(this, "ReadData", "Error", "Couldn't get data length");
		return(-1);
	}

	Int_t rdl = traceLength[0];
	Int_t edl = traceLength[1];
	Int_t wpt = traceLength[2];
	Int_t ect = traceLength[3];
	Int_t nxs = traceLength[4];

	if (nxs == 0) return(0);

	EvtData.Set(wpt + kSis3302EventPreHeader);

 	if (!curModule->GetTraceData(EvtData, EventNo, Channel)) {
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

	c->cd(1);
	k = ksave;
	if (fHistoRaw == NULL) fHistoRaw = new TH1F("Raw", "Raw data", rdl, 0, rdl);
	fHistoRaw->SetName(Form("Raw-%s-chn%d-%d", curModule->GetName(), Channel, TraceNumber));
	fHistoRaw->SetTitle(Form("Raw data from module %s, channel %d, trace %d", curModule->GetName(), Channel, TraceNumber));
	fHistoRaw->Reset("ICE");
	fHistoRaw->ResetStats();
	fHistoRaw->SetEntries(0);
	for (Int_t i = 0; i < rdl; i++, k++) fHistoRaw->Fill(i, EvtData[k]);
	fHistoRaw->Draw();
	if (fTraceFile) {
		fHistoRaw->Write();
		fNofTracesWritten++;
	}
	c->Update();
	c->cd();

	ksave = k;
	min = 1000000;
	max = 0;
	for (Int_t i = 0; i < edl; i++, k++) {
		if (EvtData[k] < min) min = EvtData[k];
		if (EvtData[k] > max) max = EvtData[k];
	}
	if (max == 0) return(-1);

	c->cd(2);
	k = ksave;
	if (fHistoEnergy == NULL) fHistoEnergy = new TH1F("Energy", "Energy data", edl, 0, edl);
	fHistoEnergy->SetName(Form("Energy-%s-chn%d-%d", curModule->GetName(), Channel, TraceNumber));
	fHistoEnergy->SetTitle(Form("Energy data from module %s, channel %d, trace %d", curModule->GetName(), Channel, TraceNumber));
	fHistoEnergy->Reset("ICE");
	fHistoEnergy->ResetStats();
	fHistoEnergy->SetEntries(0);
	for (Int_t i = 0; i < edl; i++, k++) fHistoEnergy->Fill(i, EvtData[k]);
	fHistoEnergy->Draw();
	if (fTraceFile) fHistoEnergy->Write();
	c->Update();
	c->cd();
	return(wpt);
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
			gApplication->Terminate(0);
			break;
	}
}

