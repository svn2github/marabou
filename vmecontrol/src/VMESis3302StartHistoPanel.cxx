//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMESis3302StartHistoPanel
// Purpose:        A GUI to control a SIS 3302 adc
// Description:   Accumulate histograms
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302StartHistoPanel.cxx,v 1.9 2011-07-26 08:41:50 Marabou Exp $
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
extern TMrbC2Lynx * gMrbC2Lynx;

static TC2LSis3302 * curModule = NULL;
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

// histo buttons
	fTraceFrame = new TGGroupFrame(this, "Accu", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTraceFrame);
	this->AddFrame(fTraceFrame, groupGC->LH());

	fStartStopButton = new TGTextButton(fTraceFrame, "Start", kVMESis3302StartStop);
	HEAP(fStartStopButton);
	fTraceFrame->AddFrame(fStartStopButton, groupGC->LH());
	fStartStopButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302StartStop));

	fPauseButton = new TGTextButton(fTraceFrame, "Pause", kVMESis3302StartStop);
	HEAP(fPauseButton);
	fTraceFrame->AddFrame(fPauseButton, groupGC->LH());
	fPauseButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302Pause));

	fDumpTraceButton = new TGTextButton(fTraceFrame, "Dump buffer", kVMESis3302DumpTrace);
	HEAP(fDumpTraceButton);
	fTraceFrame->AddFrame(fDumpTraceButton, groupGC->LH());
	fDumpTraceButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302DumpTrace));

	fWriteHistoButton = new TGTextButton(fTraceFrame, "Write histo", kVMESis3302WriteHisto);
	HEAP(fWriteHistoButton);
	fTraceFrame->AddFrame(fWriteHistoButton, groupGC->LH());
	fWriteHistoButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302WriteHisto));

	fWriteDeleteClonesButton = new TGTextButton(fTraceFrame, "Del clones", kVMESis3302DeleteClones);
	HEAP(fWriteDeleteClonesButton);
	fTraceFrame->AddFrame(fWriteDeleteClonesButton, groupGC->LH());
	fWriteDeleteClonesButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302DeleteClones));

	fNofBufsWrite = new TGMrbLabelEntry(fTraceFrame, "Bufs/write",	200, kVMESis3302NofBufsWrite,
																frameWidth/5, kLEHeight, frameWidth/15,
																frameGC, labelGC);
	HEAP(fNofBufsWrite);
	fNofBufsWrite->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fNofBufsWrite->SetText(0);
	fTraceFrame->AddFrame(fNofBufsWrite, frameGC->LH());

	fNofEvtsBuf = new TGMrbLabelEntry(fTraceFrame, "Evts/buf",	200, kVMESis3302NofEvtsBuf,
																frameWidth/5, kLEHeight, frameWidth/15,
																frameGC, labelGC);
	HEAP(fNofEvtsBuf);
	fNofEvtsBuf->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fNofEvtsBuf->SetText(0);
	fTraceFrame->AddFrame(fNofEvtsBuf, frameGC->LH());

// mode
	fLofTraceModes.AddNamedX(kVMESis302StartHistoModes);
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
	fKeyBindings.BindKey("Ctrl-w", TGMrbLofKeyBindings::kGMrbKeyActionClose);
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
	fPausePressed = kFALSE;
	fHistoFile = NULL;
	fNofHistosWritten = 0;
	fEmax = 0;
	fMaxEnergy->GetTextEntry()->SetEnabled(kTRUE);
	fMaxEnergy->SetText(fEmax);
	fMaxEnergy->GetTextEntry()->SetEnabled(kFALSE);
	fHsize = 16;
	fHistoSize->SetText(fHsize);
	fEfac = 1.0;
	fEnergyFactor->SetText(fEfac);

	fLofHistos.Delete();
	fLofClones.Delete();
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
		case VMESis3302StartHistoPanel::kVMESis3302Pause:
			if (fTraceCollection) this->PauseHisto();
			break;
		case VMESis3302StartHistoPanel::kVMESis3302DumpTrace:
			curModule->DumpTrace();
			break;
		case VMESis3302StartHistoPanel::kVMESis3302WriteHisto:
			this->WriteHisto(NULL);
			break;
		case VMESis3302StartHistoPanel::kVMESis3302DeleteClones:
			this->DeleteClones();
			break;
		case VMESis3302StartHistoPanel::kVMESis3302Close:
			this->KeyPressed(0, TGMrbLofKeyBindings::kGMrbKeyActionExit);
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

	Int_t nofBufsWrite = fNofBufsWrite->GetText2Int();

	Int_t nofEvents = fNofEvtsBuf->GetText2Int();
	if (nofEvents <= 0) nofEvents = kSis3302MaxEvents;

	if (!curModule->StartTraceCollection(nofEvents, chnPatt)) {
		gVMEControlData->MsgBox(this, "StartHisto", "Error", "Couldn't get traces");
		return;
	}

	this->InitializeHistos(chnPatt);

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

		TArrayI traceData(kSis3302NofChans * kSis3302EventPreHeader);
		traceNo++;
		Bool_t writeFlag = (nofBufsWrite > 0 && (traceNo % nofBufsWrite) == 0);
		while (fTraceCollection) {
			traceData.Reset(0);
			if (!curModule->GetTraceLength(traceData, chnPatt)) {
				gVMEControlData->MsgBox(this, "StartHisto", "Error", "Couldn't get trace data");
				return;
			}
			TIterator * iter = fLofHistos.MakeIterator();
			TMrbNamedX * nx;
			while (nx = (TMrbNamedX *) iter->Next()) {
				while (fPausePressed) gSystem->ProcessEvents();
				Int_t chn = nx->GetIndex() & 0xF;
				Int_t wpt = this->ReadData(evtData, nx, &traceData[chn * kSis3302EventPreHeader], traceNo);
				if (wpt == -1) return;
				if (writeFlag && wpt > 0) {
					this->WriteHisto(nx);
					writeFlag = kFALSE;
				}
				gSystem->ProcessEvents();
			}
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
	fPausePressed = kFALSE;
	fPauseButton->SetText("Pause");
	fPauseButton->SetBackgroundColor(gVMEControlData->fColorGray);
	fPauseButton->SetForegroundColor(gVMEControlData->fColorBlack);
	fPauseButton->Layout();
	fHistoSize->GetTextEntry()->SetEnabled(kTRUE);
	fEnergyFactor->GetTextEntry()->SetEnabled(kTRUE);
}

void VMESis3302StartHistoPanel::PauseHisto() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::PauseHisto
// Purpose:        Pause accumulation
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Pauses accumulation of histos
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fPausePressed) {
		fPausePressed = kFALSE;
		fPauseButton->SetText("Pause");
		fPauseButton->SetBackgroundColor(gVMEControlData->fColorGray);
		fPauseButton->SetForegroundColor(gVMEControlData->fColorBlack);
	} else {
		fPausePressed = kTRUE;
		fPauseButton->SetText("Continue");
		fPauseButton->SetBackgroundColor(gVMEControlData->fColorRed);
		fPauseButton->SetForegroundColor(gVMEControlData->fColorWhite);
	}
	fPauseButton->Layout();
}

Int_t VMESis3302StartHistoPanel::ReadData(TArrayI & EvtData, TMrbNamedX * HistoDef, Int_t TraceData[], Int_t TraceNumber) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::ReadData
// Purpose:        Read data from SIS3302
// Arguments:      TArrayI EvtData       -- where to store event data
//                 Int_t EventNo         -- event number
//                 TMrbNamedX * HistoDef -- channel number, pad position, histo addr
// Results:        NofData               -- word count
// Exceptions:
// Description:    Get event data from SIS3302 module(s)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t chn = HistoDef->GetIndex() & 0xF;

	Int_t rdl = TraceData[0];
	Int_t edl = TraceData[1];
	Int_t wpt = TraceData[2];
	Int_t ect = TraceData[3];
	Int_t nxs = TraceData[4];

	Int_t wpt2 = wpt - rdl/2;
	if (nxs == 0 || wpt == 0 || wpt2 > nxs) return(0);

	Int_t nofEvents = nxs / wpt;
	if (nofEvents > ect) nofEvents = ect;

	Int_t e = kSis3302MaxEvents;
 	Int_t n = wpt * nofEvents;
	if (n < nxs) n = nxs;
	EvtData.Set(n + kSis3302EventPreHeader);
	if (!curModule->GetTraceData(EvtData, e, chn)) {
		gVMEControlData->MsgBox(this, "ReadData", "Error", "Couldn't get data");
		return(-1);
	}

	fEmax = 0;
	Int_t k = kSis3302EventPreHeader;
	Int_t offset = kSis3302EventHeader + rdl + edl;
	for (Int_t i = 0; i < nofEvents; i++) {
		 k += offset;
		 Int_t deltaX = EvtData[k];
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

	Int_t subPad = (HistoDef->GetIndex() >> 4) & 0xF;
	c->cd(subPad);

	TH1F * h = (TH1F *) HistoDef->GetAssignedObject();

	k = kSis3302EventPreHeader;
	for (Int_t evtNo = 0; evtNo < nofEvents; evtNo++) {
		k += offset;
		Double_t x = EvtData[k] * fac * fEfac;
		h->Fill(x);
		k += 4;
	}
	h->Draw();
	c->Update();
	return(wpt);
}

Int_t VMESis3302StartHistoPanel::InitializeHistos(UInt_t ChannelPattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::InitializeHistos
// Purpose:        Initialize histograms
// Arguments:      UInt_t ChannelPattern   -- channel pattern
// Results:
// Exceptions:
// Description:    Fills histo database
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const Int_t padX[] =		{ 0, 1, 1, 2, 2, 2, 3, 3 };
	const Int_t padY[] =		{ 0, 2, 3, 2, 3, 3, 3, 3 };

	TIterator * iter = fLofHistos.MakeIterator();
	TMrbNamedX * nx;
	while (nx = (TMrbNamedX *) iter->Next()) {
		TH1F * h = (TH1F *) nx->GetAssignedObject();
		if (h) delete h;
	}
	fLofHistos.Delete();

	Int_t nofChannels = 0;
	UInt_t cp = ChannelPattern;
	for (Int_t chn = 0; chn < kSis3302NofChans; chn++, cp >>= 1) { if (cp & 1) nofChannels++; }

	TCanvas * c = fHistoCanvas->GetCanvas();
	c->Clear();
	c->Divide(padX[nofChannels - 1], padY[nofChannels - 1]);

	cp = ChannelPattern;
	Int_t hs = fHsize * 1024;
	Int_t subPad = (nofChannels == 1) ? 0 : 1;
	for (Int_t chn = 0; chn < kSis3302NofChans; chn++, cp >>= 1) {
		if (cp & 1) {
			TString hName = Form("Hist-%s-chn%d", curModule->GetName(), chn);
			TString hTitle = Form("Histogram from module %s, channel %d", curModule->GetName(), chn);
			TH1F * h = new TH1F(hName.Data(), hTitle.Data(), hs, 0., (Axis_t) hs);
			Int_t index = (subPad << 4) | chn;
			TMrbNamedX * nx = fLofHistos.AddNamedX(index, hName.Data(), hTitle.Data(), h);
			if (nofChannels == 1) break;
			c->cd(subPad);
			TPad * p = (TPad *) gPad;
			p->AddExec("clone", Form("((VMESis3302StartHistoPanel*)%#lx)->CloneHisto((TMrbNamedX *)%#lx)", this, nx));
			subPad++;
		}
	}

	return(nofChannels);
}

void VMESis3302StartHistoPanel::CloneHisto(TMrbNamedX * HistoDef) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::CloneHisto
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

void VMESis3302StartHistoPanel::WriteHisto(TMrbNamedX * HistoDef) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartHistoPanel::WriteHisto
// Purpose:        Write histo(s) to file
// Arguments:      TMrbNamedX * HistoDef  -- histo defs
// Results:
// Exceptions:
// Description:    (Re)opens root file and writes histo
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t start;
	Int_t end;

	if (HistoDef == NULL) {
		start = 0;
		end = kSis3302NofChans - 1;
	} else {
		start = HistoDef->GetIndex() & 0xF;
		end = start;
	}

	TIterator * iter = fLofHistos.MakeIterator();
	TMrbNamedX * nx;
	Int_t hwcnt = 0;
	while (nx = (TMrbNamedX *) iter->Next()) {
		Int_t chn = nx->GetIndex() & 0xF;
		if (chn >= start && chn <= end) {
			TH1F * h = (TH1F *) nx->GetAssignedObject();
			const Char_t * mode;
			if (hwcnt == 0) {
				if (fHistoFile == NULL) {
					mode = "RECREATE";
					fNofHistosWritten = 0;
				} else {
					mode = "UPDATE";
				}
				fHistoFile = new TFile("histos.root", mode);
				fNofHistosWritten++;
			}
			h->Write();
			hwcnt++;
			if (HistoDef != NULL) break;
		}
	}
	if (hwcnt > 0) {
		fHistoFile->Close();
		delete fHistoFile;
	}

	if (HistoDef == NULL)	gMrbLog->Out() << hwcnt << " histo(s) written to file histos.root (set #" << fNofHistosWritten << ")" << endl;
	else			gMrbLog->Out() << "Histogram for chn " << start << " written to file histos.root (set #" << fNofHistosWritten << ")" << endl;
	gMrbLog->Flush(this->ClassName(), "WriteHisto", setblue);
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
			if (fTraceCollection) {
				gMrbLog->Err() << "Accumulation of histos in progress - press STOP first" << endl;
				gMrbLog->Flush(this->ClassName(), "KeyPressed");
				return;
			}
			if (gMrbC2Lynx) gMrbC2Lynx->Bye();
			gApplication->Terminate(0);
			break;
		case TGMrbLofKeyBindings::kGMrbKeyActionClose:
			this->CloseWindow();
			break;
	}
}
