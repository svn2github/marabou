//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMESis3302StartRampPanel
// Purpose:        A GUI to control a SIS 3302 adc
// Description:    Start ramping
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302StartRampPanel.cxx,v 1.3 2011-07-26 08:41:50 Marabou Exp $
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
#include "VMESis3302StartRampPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;
extern TMrbC2Lynx * gMrbC2Lynx;

static TC2LSis3302 * curModule = NULL;

ClassImp(VMESis3302StartRampPanel)


VMESis3302StartRampPanel::VMESis3302StartRampPanel(const TGWindow * Window, TMrbLofNamedX * LofModules,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel
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
	fRampFrame = new TGGroupFrame(this, "Ramping", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fRampFrame);
	this->AddFrame(fRampFrame, groupGC->LH());

	fStartStopButton = new TGTextButton(fRampFrame, "Start", kVMESis3302StartStop);
	HEAP(fStartStopButton);
	fRampFrame->AddFrame(fStartStopButton, groupGC->LH());
	fStartStopButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302StartStop));

	fWriteHistoButton = new TGTextButton(fRampFrame, "Write histo", kVMESis3302WriteHisto);
	HEAP(fWriteHistoButton);
	fRampFrame->AddFrame(fWriteHistoButton, groupGC->LH());
	fWriteHistoButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302WriteHisto));

	fWriteDeleteClonesButton = new TGTextButton(fRampFrame, "Del clones", kVMESis3302DeleteClones);
	HEAP(fWriteDeleteClonesButton);
	fRampFrame->AddFrame(fWriteDeleteClonesButton, groupGC->LH());
	fWriteDeleteClonesButton->Connect("Clicked()", this->ClassName(), this, Form("PerformAction(Int_t=0, Int_t=%d)", kVMESis3302DeleteClones));

	fOffset = new TGMrbLabelEntry(fRampFrame, "Offset",	200, kVMESis3302Offset,
																frameWidth/5, kLEHeight, frameWidth/10,
																frameGC, labelGC);
	HEAP(fOffset);
	fOffset->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRampFrame->AddFrame(fOffset, frameGC->LH());

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

void VMESis3302StartRampPanel::StartGUI() {
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
	fHistoFile = NULL;
	fLofHistos.Delete();
	fLofClones.Delete();
	fRampingInProgress = kFALSE;
}

void VMESis3302StartRampPanel::ModuleChanged(Int_t FrameId, Int_t Selection) {
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

void VMESis3302StartRampPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case VMESis3302StartRampPanel::kVMESis3302StartStop:
			if (fRampingInProgress) this->StopRamp(); else this->StartRamp();
			break;
		case VMESis3302StartRampPanel::kVMESis3302WriteHisto:
			this->WriteHisto();
			break;
		case VMESis3302StartRampPanel::kVMESis3302DeleteClones:
			this->DeleteClones();
			break;
		case VMESis3302StartRampPanel::kVMESis3302Close:
			this->KeyPressed(0, TGMrbLofKeyBindings::kGMrbKeyActionClose);
			break;
	}
}

void VMESis3302StartRampPanel::StartRamp() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel::StartHisto
// Purpose:        Start data acquisition
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Start accumulation of histos
// Keywords:
//////////////////////////////////////////////////////////////////////////////

    	Int_t chnPatt = fSelectChanPatt->GetActive();

	this->InitializeHistos(chnPatt);

	fStartStopButton->SetText("STOP");
	fStartStopButton->SetBackgroundColor(gVMEControlData->fColorRed);
	fStartStopButton->SetForegroundColor(gVMEControlData->fColorWhite);
	fStartStopButton->Layout();
	fRampingInProgress = kTRUE;

	Int_t offset = fOffset->GetText2Int();
	if (offset <= 0) {
		gMrbLog->Wrn() << "Ramping offset not set - DACs won't be set" << endl;
		gMrbLog->Flush(this->ClassName(), "StartRamp");
	}

	TArrayI dacOffset(kSis3302NofChans);
	dacOffset.Reset(-1);

	TArrayI dacSaved(kSis3302NofChans);
	dacSaved.Reset(-1);
	curModule->ReadDac(dacSaved);

	TArrayI polarity(kSis3302NofChans);
	Bool_t invFlag;
	for (Int_t chn = 0; chn < kSis3302NofChans; chn++) polarity[chn] = curModule->GetPolarity(invFlag, chn) ? -1 : +1;

	TArrayI rampData(0x8000);
	TIterator * iter = fLofHistos.MakeIterator();
	TMrbNamedX * nx;
	TCanvas * c = fHistoCanvas->GetCanvas();
	while (nx = (TMrbNamedX *) iter->Next()) {
		Int_t chn = nx->GetIndex() & 0xF;
		rampData.Reset(0);
		curModule->RampDac(rampData, chn);
		Int_t subPad = (nx->GetIndex() >> 4) & 0xF;
		c->cd(subPad);
		TH1F * h = (TH1F *) nx->GetAssignedObject();
		Int_t dacVal = 0;
		Int_t last = 0x10000;
		for (Int_t idx = 0; idx < 0x8000; idx++) {
			UInt_t data = (UInt_t) rampData[idx];
			Int_t d = (data >> 16) & 0xFFFF;
			if (last < offset && d >= offset) dacOffset[chn] = dacVal;
			last = d;
			h->Fill(dacVal++, (Double_t) d);	
			d = data & 0xFFFF;
			if (last < offset && d >= offset) dacOffset[chn] = dacVal;
			last = d;
			h->Fill(dacVal++, (Double_t) d);	
		}
		h->Draw();
		c->Update();
		gSystem->ProcessEvents();
	}
	this->StopRamp();
	if (offset > 0) {
		for (Int_t chn = 0; chn < kSis3302NofChans; chn++) {
			if (dacOffset[chn] == -1) dacOffset[chn] = dacSaved[chn];
		}
		curModule->WriteDac(dacOffset);
	}
	VMESis3302Panel * panel = (VMESis3302Panel *) gVMEControlData->GetPanel(VMEControlData::kVMETabSis3302);
	panel->UpdateGUI();
}

void VMESis3302StartRampPanel::StopRamp() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel::StopHisto
// Purpose:        Stop data acquisition
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Stops accumulation of histos
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fStartStopButton->SetText("Start");
	fStartStopButton->SetBackgroundColor(gVMEControlData->fColorGreen);
	fStartStopButton->SetForegroundColor(gVMEControlData->fColorWhite);
	fStartStopButton->Layout();
	fRampingInProgress = kFALSE;
}

Int_t VMESis3302StartRampPanel::InitializeHistos(UInt_t ChannelPattern) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel::InitializeHistos
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
	Int_t hs = 0x10000;
	Int_t subPad = (nofChannels == 1) ? 0 : 1;
	for (Int_t chn = 0; chn < kSis3302NofChans; chn++, cp >>= 1) {
		if (cp & 1) {
			TString hName = Form("Ramp-%s-chn%d", curModule->GetName(), chn);
			TString hTitle = Form("Ramping data from module %s, channel %d", curModule->GetName(), chn);
			TH1F * h = new TH1F(hName.Data(), hTitle.Data(), hs, 0., (Axis_t) hs);
			Int_t index = (subPad << 4) | chn;
			TMrbNamedX * nx = fLofHistos.AddNamedX(index, hName.Data(), hTitle.Data(), h);
			if (nofChannels == 1) break;
			c->cd(subPad);
			TPad * p = (TPad *) gPad;
			p->AddExec("clone", Form("((VMESis3302StartRampPanel*)%#lx)->CloneHisto((TMrbNamedX *)%#lx)", this, nx));
			subPad++;
		}
	}

	return(nofChannels);
}

void VMESis3302StartRampPanel::CloneHisto(TMrbNamedX * HistoDef) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel::CloneHisto
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

void VMESis3302StartRampPanel::WriteHisto() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel::WriteHisto
// Purpose:        Write histo(s) to file
// Arguments:      TMrbNamedX * HistoDef  -- histo defs
// Results:
// Exceptions:
// Description:    (Re)opens root file and writes histo
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * iter = fLofHistos.MakeIterator();
	TMrbNamedX * nx;
	fHistoFile = new TFile("ramps.root", "RECREATE");
	while (nx = (TMrbNamedX *) iter->Next()) {
		TH1F * h = (TH1F *) nx->GetAssignedObject();
		h->Write();
	}
	fHistoFile->Close();
	delete fHistoFile;

	gMrbLog->Out() << "Ramping data written to file ramps.root" << endl;
	gMrbLog->Flush(this->ClassName(), "WriteHisto", setblue);
}

void VMESis3302StartRampPanel::KeyPressed(Int_t FrameId, Int_t Key) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRampPanel::KeyPressed
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
			if (gMrbC2Lynx) gMrbC2Lynx->Bye();
			gApplication->Terminate(0);
			break;
		case TGMrbLofKeyBindings::kGMrbKeyActionClose:
			this->CloseWindow();
			break;
	}
}
