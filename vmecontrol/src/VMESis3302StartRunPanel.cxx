//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMESis3302StartRunPanel
// Purpose:        A GUI to control a SIS 3302 adc
// Description:    Save/restore settings
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302StartRunPanel.cxx,v 1.1 2008-10-27 12:26:07 Marabou Exp $       
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

#include "TGFileDialog.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbLofDGFs.h"
#include "TMrbSystem.h"
#include "TMrbEnv.h"
#include "TGMrbProgressBar.h"

#include "TC2LSis3302.h"
#include "VMESis3302Panel.h"
#include "VMESis3302StartRunPanel.h"

#include "SetColor.h"

#include <iostream>
#include <fstream>

const SMrbNamedX kVMESis302StartRunActions[] =
			{
				{VMESis3302StartRunPanel::kVMESis3302Start,		"Start",		"Start acquisition"	},
				{VMESis3302StartRunPanel::kVMESis3302Close,		"Close",		"Close window"	},
				{0, 											NULL,			NULL								}
			};

extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

static TC2LSis3302 * curModule = NULL;
static Int_t curChannel = 0;

ClassImp(VMESis3302StartRunPanel)


VMESis3302StartRunPanel::VMESis3302StartRunPanel(const TGWindow * Window, TMrbLofNamedX * LofModules,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRunPanel
// Purpose:        VMEControl: Save & Restore
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
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

//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kVMESis302StartRunActions);

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

	TMrbLofNamedX lofChannels;
	for (Int_t i = 0; i < VMESis3302Panel::kVMENofSis3302Chans; i++) lofChannels.AddNamedX(i, Form("chn %d", i));
	fSelectChannel = new TGMrbLabelCombo(fSelectFrame, "Channel",	&lofChannels,
																kVMESis3302SelectChannel, 1,
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, comboGC, labelGC);
	fSelectFrame->AddFrame(fSelectChannel, frameGC->LH());
	fSelectChannel->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ChannelChanged(Int_t, Int_t)");

// timeout
	fTimeout = new TGMrbLabelEntry(fSelectFrame, "Timeout",	200, kVMESis3302Timeout,
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTimeout);
	fTimeout->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTimeout->SetText(0);
	fTimeout->SetRange(0, 1000000);
	fTimeout->SetIncrement(100);
	fTimeout->ShowToolTip(kTRUE, kTRUE);
	fSelectFrame->AddFrame(fTimeout, frameGC->LH());
	fTimeout->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TimeoutChanged(Int_t, Int_t)");

// canvas
	fHistoFrame = new TGGroupFrame(this, "Histograms", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fHistoFrame);
	this->AddFrame(fHistoFrame, groupGC->LH());
	fHistoCanvas = new TRootEmbeddedCanvas("HistoCanvas", fHistoFrame, frameWidth - 100, frameHeight*3/4);
	fHistoFrame->AddFrame(fHistoCanvas, frameGC->LH());
	fHistoCanvas->GetCanvas()->Divide(1, 2);

// action buttons
	fActionButtons = new TGMrbTextButtonGroup(this, "Actions", &fActions, -1, 1, groupGC, buttonGC);
	HEAP(fActionButtons);
	this->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	((TGMrbButtonFrame *) fActionButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gVMEControlData->fColorGold);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(Width, Height);
	MapWindow();

	this->StartGUI();
}

void VMESis3302StartRunPanel::StartGUI() {
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
	fSelectChannel->Select(curChannel);

	Int_t timeout;
	curModule->SetTimeout(timeout);
}

void VMESis3302StartRunPanel::ModuleChanged(Int_t FrameId, Int_t Selection) {
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

void VMESis3302StartRunPanel::ChannelChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::ChannelChanged
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

void VMESis3302StartRunPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRunPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case VMESis3302StartRunPanel::kVMESis3302Start:
			this->StartRun();
			break;
		case VMESis3302StartRunPanel::kVMESis3302Close:
			this->CloseWindow();
			break;
	}
}

void VMESis3302StartRunPanel::TimeoutChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TimeoutChanged
// Purpose:        Slot method: timeout changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t timeout = fTimeout->GetText2Int();
	curModule->SetTimeout(timeout);
}

void VMESis3302StartRunPanel::StartRun() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302StartRunPanel::StartRun
// Purpose:        Start data acquisition
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Get event(s) from SIS3302 module(s)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI evtData;
	Int_t rawDataLength;
	Int_t energyDataLength;

	curModule->GetSingleEvent(evtData, curChannel);

	Int_t k = kSis3302EventPreHeader + kSis3302EventHeader;

	if (curModule->IsOffline()) {
		rawDataLength = 1024;
		energyDataLength = 512;
 		Int_t nofWords = kSis3302EventPreHeader
					+ kSis3302EventHeader
					+ rawDataLength
					+ energyDataLength
					+ kSis3302EventMinMax
					+ kSis3302EventTrailer;
		evtData.Set(nofWords);
		for (Int_t i = 0; i < rawDataLength + energyDataLength; i++)
					evtData[i + kSis3302EventPreHeader + kSis3302EventHeader] = (Int_t) 100 * sin(i * 0.02);
	} else {
		cout << "@@@ " << evtData.GetSize() << endl;
		return;
		rawDataLength = evtData[0];
		energyDataLength = evtData[1];
	}

	TCanvas * c = fHistoCanvas->GetCanvas();

	c->cd(1);
	if (fHistoRaw) delete fHistoRaw;
	fHistoRaw = new TH1F(	Form("Raw-%s-chn%d", curModule->GetName(), curChannel),
							Form("Raw data from module %s, channel %d", curModule->GetName(), curChannel),
							rawDataLength, 0, rawDataLength);
	for (Int_t i = 0; i < rawDataLength; i++, k++) fHistoRaw->Fill(i, evtData[k]);
	fHistoRaw->Draw();

	c->cd(2);
	if (fHistoEnergy) delete fHistoEnergy;
	fHistoEnergy = new TH1F(	Form("Energy-%s-chn%d", curModule->GetName(), curChannel),
							Form("Energy data from module %s, channel %d", curModule->GetName(), curChannel),
							energyDataLength, 0, energyDataLength);
	for (Int_t i = 0; i < energyDataLength; i++, k++) fHistoEnergy->Fill(i, evtData[k]);
	fHistoEnergy->Draw();
	c->Update();
	c->cd();
}


