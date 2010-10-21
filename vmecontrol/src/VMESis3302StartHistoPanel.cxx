//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMESis3302StartHistoPanel
// Purpose:        A GUI to control a SIS 3302 adc
// Description:   Accumulate histograms
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302StartHistoPanel.cxx,v 1.4 2010-10-21 11:54:06 Marabou Exp $
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
#include "TFile.h"
#include "TApplication.h"

#include "TGFileDialog.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbLofDGFs.h"
#include "TMrbSystem.h"
#include "TMrbEnv.h"
#include "TGMrbProgressBar.h"

#include "TC2LSis3302.h"
#include "VMESis3302Panel.h"
#include "VMESis3302StartHistoPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

const SMrbNamedXShort kVMESis302StartHistoModes[] =
			{
				{VMESis3302StartHistoPanel::kVMESis3302ModeMAWD,		"MAWD"	},
				{VMESis3302StartHistoPanel::kVMESis3302ModeMAW,			"MAW"	},
				{VMESis3302StartHistoPanel::kVMESis3302ModeMAWFT,		"MAWFT"	},
				{0, 												NULL	}
			};

extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

static TC2LSis3302 * curModule = NULL;
static Int_t curChannel = 0;
static Int_t traceMode = 0;

ClassImp(VMESis3302StartHistoPanel)


VMESis3302StartHistoPanel::VMESis3302StartHistoPanel(const TGWindow * Window, TMrbLofNamedX * LofModules,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel
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

// histo buttons
	fTraceFrame = new TGGroupFrame(this, "Accu", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
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

	fWriteHistoButton = new TGTextButton(fTraceFrame, "Write histo", kVMESis3302WriteHisto);
	HEAP(fWriteHistoButton);
	fTraceFrame->AddFrame(fWriteHistoButton, groupGC->LH());
	fWriteHistoButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302WriteHisto));

	fNofEvtsBuf = new TGMrbLabelEntry(fTraceFrame, "Evts/buffer",	200, kVMESis3302NofEvtsBuf,
																frameWidth/5, kLEHeight, frameWidth/10,
																frameGC, labelGC);
	HEAP(fNofEvtsBuf);
	fNofEvtsBuf->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fNofEvtsBuf->SetText(0);
	fTraceFrame->AddFrame(fNofEvtsBuf, frameGC->LH());

// mode
	fLofTraceModes.AddNamedX(kVMESis302StartHistoModes);
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

	fMaxEnergy = new TGMrbLabelEntry(fDisplayFrame, "E-max",	200, kVMESis3302EnergyMax,
																frameWidth/5, kLEHeight, frameWidth/10,
																frameGC, labelGC);
	HEAP(fMaxEnergy);
	fMaxEnergy->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMaxEnergy->GetTextEntry()->SetEnabled(kFALSE);
	fDisplayFrame->AddFrame(fMaxEnergy, frameGC->LH());

	fHistoSize = new TGMrbLabelEntry(fDisplayFrame, "H-size",	200, kVMESis3302HistoSize,
																frameWidth/5, kLEHeight, frameWidth/15,
																frameGC, labelGC);
	HEAP(fHistoSize);
	fHistoSize->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fDisplayFrame->AddFrame(fHistoSize, frameGC->LH());

	fEnergyFactor = new TGMrbLabelEntry(fDisplayFrame, "E-fac",	200, kVMESis3302EnergyFactor,
																frameWidth/5, kLEHeight, frameWidth/15,
																frameGC, labelGC);
	HEAP(fEnergyFactor);
	fEnergyFactor->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fDisplayFrame->AddFrame(fEnergyFactor, frameGC->LH());

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

void VMESis3302StartHistoPanel::StartGUI() {
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
	fHistoFile = NULL;
	fHisto = NULL;
	fEmax = 0;
	fMaxEnergy->GetTextEntry()->SetEnabled(kTRUE);
	fMaxEnergy->SetText(fEmax);
	fMaxEnergy->GetTextEntry()->SetEnabled(kFALSE);
	fHsize = 16;
	fHistoSize->SetText(fHsize);
	fEfac = 1.0;
	fEnergyFactor->SetText(fEfac);
}

void VMESis3302StartHistoPanel::ModuleChanged(Int_t FrameId, Int_t Selection) {
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

void VMESis3302StartHistoPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case VMESis3302StartHistoPanel::kVMESis3302StartStop:
			if (fTraceCollection) this->StopHisto(); else this->StartHisto();
			break;
		case VMESis3302StartHistoPanel::kVMESis3302DumpTrace:
			curModule->DumpTrace();
			break;
		case VMESis3302StartHistoPanel::kVMESis3302WriteHisto:
			this->WriteHisto();
			break;
	}
}

void VMESis3302StartHistoPanel::TraceModeChanged(Int_t FrameId, Int_t Selection) {
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

void VMESis3302StartHistoPanel::ChannelChanged(Int_t FrameId, Int_t Selection) {
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

void VMESis3302StartHistoPanel::StartHisto() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::StartHisto
// Purpose:        Start data acquisition
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Start accumulation of histos
// Keywords:
//////////////////////////////////////////////////////////////////////////////

    Int_t chnPatt = fSelectChanPatt->GetActive();
	curModule->SetTestBits(traceMode);

	Int_t nofEvents = fNofEvtsBuf->GetText2Int();
	if (nofEvents <= 0) nofEvents = kSis3302MaxEvents;

	if (!curModule->StartTraceCollection(nofEvents, chnPatt)) {
		gVMEControlData->MsgBox(this, "StartHisto", "Error", "Couldn't get traces");
		return;
	}

	if (fHisto != NULL)  {
		fHisto->Delete();
		fHisto = NULL;
	}

	fTraceCollection = kTRUE;
	fStartStopButton->SetText("STOP");
	fStartStopButton->SetBackgroundColor(gVMEControlData->fColorRed);
	fStartStopButton->SetForegroundColor(gVMEControlData->fColorWhite);
	fStartStopButton->Layout();

	fEfac = fEnergyFactor->GetText2Double();
	fEnergyFactor->GetTextEntry()->SetEnabled(kFALSE);

	fHsize = fHistoSize->GetText2Int();
	fHistoSize->GetTextEntry()->SetEnabled(kFALSE);

	TArrayI evtData;
	Int_t traceNo = 0;
	for (;;) {
		gSystem->ProcessEvents();
		if (!fTraceCollection) {
			curModule->StopTraceCollection();
			break;
		}
		Int_t curEvent = kSis3302MaxEvents;
		Int_t wpt = 0;
		traceNo++;
		while (fTraceCollection && wpt == 0) {
			wpt = this->ReadData(evtData, curEvent, curChannel, traceNo);
			gSystem->ProcessEvents();
			if (wpt == 0) sleep(1);
		}
	}
}

void VMESis3302StartHistoPanel::StopHisto() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::StopHisto
// Purpose:        Stop data acquisition
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Stops accumulation of histos
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fTraceCollection = kFALSE;
	fStartStopButton->SetText("Start");
	fStartStopButton->SetBackgroundColor(gVMEControlData->fColorGreen);
	fStartStopButton->SetForegroundColor(gVMEControlData->fColorWhite);
	fStartStopButton->Layout();
	fHistoSize->GetTextEntry()->SetEnabled(kTRUE);
	fEnergyFactor->GetTextEntry()->SetEnabled(kTRUE);
}

Int_t VMESis3302StartHistoPanel::ReadData(TArrayI & EvtData, Int_t EventNo, Int_t Channel, Int_t TraceNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::ReadData
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

	EventNo = kSis3302MaxEvents;
 	Int_t n = wpt * ect;
	if (n < nxs) n = nxs;
	EvtData.Set(n + kSis3302EventPreHeader);

 	if (!curModule->GetTraceData(EvtData, EventNo, Channel)) {
		gVMEControlData->MsgBox(this, "ReadData", "Error", "Couldn't get data");
		return(-1);
	}

	Int_t k = kSis3302EventPreHeader;
	Int_t offset = kSis3302EventHeader + rdl + edl;
	for (Int_t evtNo = 0; evtNo < ect; evtNo++) {
		 k += offset;
		 Int_t deltaX = EvtData[k] - EvtData[k + 1];
		if (deltaX > fEmax) {
			fEmax = deltaX;
			fMaxEnergy->GetTextEntry()->SetEnabled(kTRUE);
			fMaxEnergy->SetText(fEmax);
			fMaxEnergy->GetTextEntry()->SetEnabled(kFALSE);
		}
		UInt_t trailer = EvtData[k + 3];
		if (trailer != 0xdeadbeef) {
			gMrbLog->Err() << "Wrong trailer - 0x" << setbase(16) << trailer << " (should be 0xdeadbeef). Giving up." << endl;
			gMrbLog->Flush(this->ClassName(), "ReadData");
			break;
		}
		k += 4;
	}

	Int_t hs = fHsize * 1024;
	Int_t xchans = (fEmax + 1023) / 1024 * 1024;
	Double_t fac = hs / (Double_t) xchans;

	TCanvas * c = fHistoCanvas->GetCanvas();
	if (fHisto == NULL) {
		fHisto = new TH1F(	Form("Hist-%s-chn%d", curModule->GetName(), Channel),
							Form("Histogram from module %s, channel %d", curModule->GetName(), Channel), hs, 0., (Axis_t) hs);
	}

	k = kSis3302EventPreHeader;
	for (Int_t evtNo = 0; evtNo < ect; evtNo++) {
		k += offset;
		Double_t x = (EvtData[k] - EvtData[k + 1]) * fac * fEfac;
		fHisto->Fill(x);
		k += 4;
	}
	fHisto->Draw();
	c->Update();
	return(wpt);
}

void VMESis3302StartHistoPanel::WriteHisto() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::WriteHisto
// Purpose:        Write histo to file
// Arguments:
// Results:
// Exceptions:
// Description:    (Re)opens root file and writes histo
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fHisto == NULL) {
		gVMEControlData->MsgBox(this, "WriteHisto", "Error", "No histo allocated yet");
		return;
	}
	const Char_t * mode = (fHistoFile == NULL) ? "RECREATE" : "UPDATE";
	fHistoFile = new TFile("histos.root", mode);
	fHisto->Write();
	gMrbLog->Out() << "Histo written to file histos.root" << endl;
	gMrbLog->Flush(this->ClassName(), "WriteHisto", setblue);
	fHistoFile->Close();
	delete fHistoFile;
}

void VMESis3302StartHistoPanel::KeyPressed(Int_t FrameId, Int_t Key) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::KeyPressed
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

