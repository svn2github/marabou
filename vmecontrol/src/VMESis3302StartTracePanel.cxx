//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMESis3302StartTracePanel
// Purpose:        A GUI to control a SIS 3302 adc
// Description:    Save/restore settings
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302StartTracePanel.cxx,v 1.1 2010-04-22 13:44:41 Rudolf.Lutter Exp $
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

const SMrbNamedX kVMESis302StartTraceActions[] =
			{
				{VMESis3302StartTracePanel::kVMESis3302Start,		"Start",		"Start trace collection"	},
				{0, 											NULL,			NULL			}
			};

const SMrbNamedXShort kVMESis302StartTraceModes[] =
			{
				{VMESis3302StartTracePanel::kVMESis3302ModeMAWD,		"MAWD - decay corrected moving average window"	},
				{VMESis3302StartTracePanel::kVMESis3302ModeMAW,		"MAW - moving average window"	},
				{VMESis3302StartTracePanel::kVMESis3302ModeMAWFT,		"MAWFT - FIR trigger moving average window"	},
				{0, 												NULL	}
			};

extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

static TC2LSis3302 * curModule = NULL;
static Int_t curChannel = 0;
static Int_t curEvent = 0;
static Int_t traceMode = 0;
static Int_t nofEvents = 1;

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

	fHistoRaw = NULL;
	fHistoEnergy = NULL;

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
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, comboGC, labelGC);
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

	fStartButton = new TGTextButton(fTraceFrame, "Start", kVMESis3302Start);
	HEAP(fStartButton);
	fTraceFrame->AddFrame(fStartButton, groupGC->LH());
	fStartButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302Start));

// mode & number of events
	fLofTraceModes.AddNamedX(kVMESis302StartTraceModes);
	fSelectMode = new TGMrbLabelCombo(fTraceFrame, "Mode",	&fLofTraceModes,
															kVMESis3302SelectMode, 1,
															frameWidth/5, kLEHeight, frameWidth/3 + 100,
															frameGC, labelGC, comboGC, labelGC);
	fTraceFrame->AddFrame(fSelectMode, frameGC->LH());
	fSelectMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TraceModeChanged(Int_t, Int_t)");
	fSelectMode->Select(0);
	HEAP(fSelectMode);

	fNofEvents = new TGMrbLabelEntry(fTraceFrame, "Events",	200, kVMESis3302NofEvents,
															frameWidth/5, kLEHeight, frameWidth/8,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fNofEvents);
	fNofEvents->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fNofEvents->SetText(nofEvents);
	fNofEvents->SetRange(0, 1000000);
	fNofEvents->SetIncrement(10);
	fNofEvents->ShowToolTip(kTRUE, kTRUE);
	fTraceFrame->AddFrame(fNofEvents, frameGC->LH());
	fNofEvents->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "NofEventsChanged(Int_t, Int_t)");

//	display
	fDisplayFrame = new TGGroupFrame(this, "Display", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDisplayFrame);
	this->AddFrame(fDisplayFrame, groupGC->LH());

	TMrbLofNamedX lofChannels;
	gVMEControlData->GetLofChannels(lofChannels, VMESis3302Panel::kVMENofSis3302Chans, "chn %d");

	fSelectChannel = new TGMrbLabelCombo(fDisplayFrame, "Channel",	&lofChannels,
																kVMESis3302SelectChannel, 1,
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, comboGC, labelGC);
	fDisplayFrame->AddFrame(fSelectChannel, frameGC->LH());
	fSelectChannel->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ChannelChanged(Int_t, Int_t)");
	fSelectChannel->Select(0);

	fSelectEvent = new TGMrbLabelEntry(fDisplayFrame, "Event",	200, kVMESis3302SelectEvent,
															frameWidth/5, kLEHeight, frameWidth/8,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fSelectEvent);
	fSelectEvent->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fSelectEvent->SetText(0);
	fSelectEvent->SetRange(0, nofEvents - 1);
	fSelectEvent->SetIncrement(1);
	fSelectEvent->ShowToolTip(kTRUE, kTRUE);
	fDisplayFrame->AddFrame(fSelectEvent, frameGC->LH());
	fSelectEvent->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EventNumberChanged(Int_t, Int_t)");

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

	Int_t timeout = gVMEControlData->Vctrlrc()->Get(".Timeout", 10);
	curModule->SetTimeout(timeout);
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
		case VMESis3302StartTracePanel::kVMESis3302Start:
			this->StartTrace();
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

void VMESis3302StartTracePanel::NofEventsChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::NofEventsChanged
// Purpose:        Slot method: number of events changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	nofEvents = fNofEvents->GetText2Int();
	fSelectEvent->SetRange(0, nofEvents - 1);
	if (curEvent > nofEvents - 1) curEvent = nofEvents - 1;
	fSelectEvent->SetText(curEvent);
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

void VMESis3302StartTracePanel::EventNumberChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EventNumberChanged
// Purpose:        Slot method: event number changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	curEvent = fSelectEvent->GetText2Int();
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

	if (!curModule->CollectTraces(nofEvents, chnPatt)) {
		gVMEControlData->MsgBox(this, "StartTrace", "Error", "Couldn't get traces");
		return;
	}

	TArrayI evtData;
	this->ReadData(evtData, curEvent, curChannel);
}

Bool_t VMESis3302StartTracePanel::ReadData(TArrayI & EvtData, Int_t EventNo, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartTracePanel::ReadData
// Purpose:        Read data from SIS3302
// Arguments:      TArrayI EvtData       -- where to store event data
//                 Int_t EventNo         -- event number
//                 Int_t Channel         -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Get event(s) from SIS3302 module(s)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TArrayI lengthArr(3);
	if (!curModule->GetDataLength(lengthArr, Channel)) {
		gVMEControlData->MsgBox(this, "ReadData", "Error", "Couldn't get data length");
		return(kFALSE);
	}

	Int_t rdl = lengthArr[0];
	Int_t edl = lengthArr[1];
	Int_t wpe = lengthArr[2];
	EvtData.Set(wpe + kSis3302EventPreHeader);

 	if (!curModule->GetEvent(EvtData, EventNo, Channel)) {
		gVMEControlData->MsgBox(this, "ReadData", "Error", "Couldn't get data");
		return(kFALSE);
	}

	Int_t k = kSis3302EventPreHeader + kSis3302EventHeader;

	Int_t ksave = k;
	Int_t min = 1000000;
	Int_t max = 0;
	for (Int_t i = 0; i < rdl; i++, k++) {
		if (EvtData[k] < min) min = EvtData[k];
		if (EvtData[k] > max) max = EvtData[k];
	}
	if (max == 0) {
		gMrbLog->Err()	<< "Raw data buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadData");
		return(kFALSE);
	}

	TCanvas * c = fHistoCanvas->GetCanvas();

	c->cd(1);
	k = ksave;
	if (fHistoRaw) fHistoRaw->Delete();
	fHistoRaw = new TH1F(	Form("Raw-%s-chn%d-%d", curModule->GetName(), Channel, EventNo),
							Form("Raw data from module %s, channel %d, evt #%d", curModule->GetName(), Channel, EventNo),
							rdl, 0, rdl);
	for (Int_t i = 0; i < rdl; i++, k++) fHistoRaw->Fill(i, EvtData[k]);
	fHistoRaw->Draw();
	c->Update();
	c->cd();

	ksave = k;
	min = 1000000;
	max = 0;
	for (Int_t i = 0; i < edl; i++, k++) {
		if (EvtData[k] < min) min = EvtData[k];
		if (EvtData[k] > max) max = EvtData[k];
	}
	if (max == 0) {
		gMrbLog->Err()	<< "Event data buffer is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadData");
		return(kFALSE);
	}

	c->cd(2);
	k = ksave;
	if (fHistoEnergy) fHistoEnergy->Delete();
	fHistoEnergy = new TH1F(	Form("Energy-%s-chn%d-%d", curModule->GetName(), Channel, EventNo),
							Form("Energy data from module %s, channel %d, evt #%d", curModule->GetName(), Channel, EventNo),
							edl, 0, edl);
	for (Int_t i = 0; i < edl; i++, k++) fHistoEnergy->Fill(i, EvtData[k]);
	fHistoEnergy->Draw();
	c->Update();
	c->cd();
	return(kTRUE);
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

