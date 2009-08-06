//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel
// Purpose:        A GUI to control vme modules via tcp
// Description:    Connect to server
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEVulomTBPanel.cxx,v 1.1 2009-08-06 08:32:34 Rudolf.Lutter Exp $
// Date:
// URL:
// Keywords:
// Layout:
//////////////////////////////////////////////////////////////////////////////

#include "TObjString.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TMrbC2Lynx.h"
#include "TC2LVulomTB.h"

#include "TGMrbProgressBar.h"

#include "VMEControlData.h"
#include "VMEVulomTBPanel.h"

#include "SetColor.h"

extern TMrbC2Lynx * gMrbC2Lynx;
extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

static TC2LVulomTB * curModule = NULL;
static Int_t curChannel = 0;

static Int_t frameWidth;
static Int_t frameHeight;

const SMrbNamedX kVMEActions[] =
			{
				{VMEVulomTBPanel::kVMEVulomTBActionEnableAll,		"Enable all",		"Enable all channels"				},
				{VMEVulomTBPanel::kVMEVulomTBActionDisableAll,		"Disable all",		"Disable all channels"				},
				{VMEVulomTBPanel::kVMEVulomTBActionClearScaler,		"Clear scaler",		"Clear scaler contents"				},
				{VMEVulomTBPanel::kVMEVulomTBActionReset,			"Reset scaledown",	"Reset all scale down values"		},
				{VMEVulomTBPanel::kVMEVulomTBActionStartTimer,		"Start timer",		"Start timer with given interval"	},
				{VMEVulomTBPanel::kVMEVulomTBActionStopTimer,		"Stop timer",		"Stop timer"						},
				{0, 												NULL,				NULL								}
			};

ClassImp(VMEVulomTBPanel)

VMEVulomTBPanel::VMEVulomTBPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel
// Purpose:        Connect to LynxOs server
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * comboGC;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	Clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gVMEControlData->SlantedFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGreen);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorGray);	HEAP(buttonGC);

	entryGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
								gVMEControlData->fColorBlack,
								gVMEControlData->fColorWhite);	HEAP(entryGC);

	TGLayoutHints * loXpndX = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 10, 10, 1, 1);
	TGLayoutHints * loXpndX2 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	TGLayoutHints * loXpndX3 = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
	TGLayoutHints * loNormal = new TGLayoutHints(kLHintsTop, 10, 10, 1, 1);
	TGLayoutHints * loNormal2 = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	TGLayoutHints * loNormal3 = new TGLayoutHints(kLHintsTop | kLHintsExpandY, 1, 1, 1, 1);
	HEAP(loXpndX); HEAP(loXpndX2); HEAP(loNormal); HEAP(loNormal2);

	frameGC->SetLH(loNormal3);
	comboGC->SetLH(loNormal2);
	labelGC->SetLH(loNormal);
	buttonGC->SetLH(loNormal);
	entryGC->SetLH(loNormal);
	groupGC->SetLH(loXpndX2);

// geometry
	frameWidth = this->GetWidth();
	frameHeight = this->GetHeight();

//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kVMEActions);

//	module / channel selection
	fSelectFrame = new TGGroupFrame(this, "Select module", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, groupGC->LH());

	this->SetupModuleList();

	fSelectModule = new TGMrbLabelCombo(fSelectFrame, "Module",	&fLofModules,
																kVMEVulomTBSelectModule, 1,
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, comboGC, labelGC);
	fSelectFrame->AddFrame(fSelectModule, frameGC->LH());
	fSelectModule->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ModuleChanged(Int_t, Int_t)");

	fSettingsFrame = new TGGroupFrame(this, "Triggerbox settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSettingsFrame);
	this->AddFrame(fSettingsFrame, groupGC->LH());

	groupGC->SetLH(loNormal3);
	fEnableFrame = new TGGroupFrame(fSettingsFrame, "Enable/disable", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fEnableFrame);
	fSettingsFrame->AddFrame(fEnableFrame, groupGC->LH());

	for (Int_t ch = 0; ch < kVMENofVulomTBChans; ch++) {
	  fChannelEnable[ch] = new TGCheckButton(fEnableFrame, Form("chn%02d", ch), ch);
	  HEAP(fChannelEnable[ch]);
	  fEnableFrame->AddFrame(fChannelEnable[ch], frameGC->LH());
	  fChannelEnable[ch]->Connect("Clicked()", this->ClassName(), this, Form("ChannelEnableChanged(Int_t=%d)", ch));
	  fChannelEnable[ch]->SetToolTipText(Form("chn %d", ch));
	  fChannelEnable[ch]->ChangeBackground(gVMEControlData->fColorGreen);
	}

	groupGC->SetLH(loNormal3);
	fScaleDownFrame = new TGGroupFrame(fSettingsFrame, "Scale down", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fScaleDownFrame);
	fSettingsFrame->AddFrame(fScaleDownFrame, groupGC->LH());

	for (Int_t ch = 0; ch < kVMENofVulomTBChans; ch++) {
	  fScaleDownValue[ch] = new TGMrbLabelEntry(fScaleDownFrame, Form("chn%02d", ch),	100, ch,
																frameWidth/6, kLEHeight, frameWidth/8,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	  HEAP(fScaleDownValue[ch]);
	  fScaleDownValue[ch]->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	  fScaleDownValue[ch]->SetText(0);
	  fScaleDownValue[ch]->SetRange(0, 0xF);
	  fScaleDownValue[ch]->SetIncrement(1);
	  fScaleDownValue[ch]->ShowToolTip(kTRUE, kTRUE);
	  fScaleDownFrame->AddFrame(fScaleDownValue[ch], frameGC->LH());
	  fScaleDownValue[ch]->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "ScaleDownChanged(Int_t, Int_t)");
	  fScaleDownValue[ch]->GetLabel()->SetTextJustify(kTextCenterY);
	}

	groupGC->SetLH(loXpndX3);
	fScalerFrame = new TGGroupFrame(fSettingsFrame, "Scaler contents", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fScalerFrame);
	fSettingsFrame->AddFrame(fScalerFrame, groupGC->LH());

	for (Int_t ch = 0; ch < kVMENofVulomTBChans; ch++) {
	  TGHorizontalFrame * hf = new TGHorizontalFrame(fScalerFrame);
	  HEAP(hf);
	  fScalerFrame->AddFrame(hf, groupGC->LH());
	  hf->ChangeBackground(gVMEControlData->fColorGreen);

	  fScalerFree[ch] = new TGMrbLabelEntry(hf,  Form("free%02d", ch), 100, ch,
																frameWidth/8, kLEHeight, frameWidth/12,
																frameGC, labelGC, entryGC);
	  HEAP(fScalerFree[ch]);
	  hf->AddFrame(fScalerFree[ch], frameGC->LH());
	  fScalerFree[ch]->GetLabel()->SetTextJustify(kTextCenterY);

	  fScalerInhibit[ch] = new TGMrbLabelEntry(hf, Form("inh%02d", ch), 100, ch,
																frameWidth/8, kLEHeight, frameWidth/12,
																frameGC, labelGC, entryGC);
	  HEAP(fScalerInhibit[ch]);
	  hf->AddFrame(fScalerInhibit[ch], frameGC->LH());
	  fScalerInhibit[ch]->GetLabel()->SetTextJustify(kTextCenterY);

	  fScalerScaledDown[ch] = new TGMrbLabelEntry(hf, Form("scd%02d", ch), 100, ch,
																frameWidth/5, kLEHeight, frameWidth/16,
																frameGC, labelGC, entryGC);
	  HEAP(fScalerScaledDown[ch]);
	  hf->AddFrame(fScalerScaledDown[ch], frameGC->LH());
	  fScalerScaledDown[ch]->GetLabel()->SetTextJustify(kTextCenterY);
	}

// action buttons
	TGVerticalFrame * hf = new TGVerticalFrame(this);
	hf->ChangeBackground(gVMEControlData->fColorGreen);
	HEAP(hf);
	this->AddFrame(hf, frameGC->LH());

	groupGC->SetLH(loXpndX2);

	fActionButtons = new TGMrbTextButtonGroup(hf, "Actions", &fActions, -1, 1, groupGC, buttonGC);
	HEAP(fActionButtons);
	hf->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	((TGMrbButtonFrame *) fActionButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

// setup timer
	TGGroupFrame * gf = new TGGroupFrame(hf, "Timer");
	gf->ChangeBackground(gVMEControlData->fColorGreen);
	hf->AddFrame(gf, groupGC->LH());

	fTimer = new TTimer(1000);
	fTimer->Connect("Timeout()", "VMEVulomTBPanel", this, "UpdateGUI()");

	fTimerInterval = new TGMrbLabelEntry(gf, "Interval [ms]",	100, -1,
																frameWidth/6, kLEHeight, frameWidth/8,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTimerInterval);
	fTimerInterval->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTimerInterval->SetText(0);
	fTimerInterval->SetRange(0, 100000);
	fTimerInterval->SetIncrement(1000);
	fTimerInterval->ShowToolTip(kTRUE, kTRUE);
	gf->AddFrame(fTimerInterval, groupGC->LH());
	fTimerInterval->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TimerIntervalChanged(Int_t, Int_t)");
	fTimerInterval->GetLabel()->SetTextJustify(kTextCenterY);

// start GUI

	this->ChangeBackground(gVMEControlData->fColorGreen);
	this->UpdateGUI();

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t VMEVulomTBPanel::SetupModuleList() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel::SetupModuleList
// Purpose:        Fill list of modules
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbC2Lynx) {
		if (gVMEControlData->SetupModuleList(fLofModules, "TMrbVulomTB")) return(kTRUE);
		if (this->GetNofModules() == 0) {
			gVMEControlData->MsgBox(this, "SetupModuleList", "No modules", "No Vulom/TB modules found");
			return(kFALSE);
		}
	} else {
		gVMEControlData->MsgBox(this, "SetupModuleList", "Not connected", "No connection to LynxOs server");
	}
	return(kFALSE);
}

void VMEVulomTBPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (curModule) {
	  switch (Selection) {
		case kVMEVulomTBActionEnableAll:
		  {
			UInt_t bits = 0xFFFF;
			curModule->SetEnableMask(bits);
		  }
		  break;
		case kVMEVulomTBActionDisableAll:
		  {
			UInt_t bits = 0x0;
			curModule->SetEnableMask(bits);
		  }
		  break;
		case kVMEVulomTBActionClearScaler:
		  {
			curModule->ClearScaler();
		  }
		  break;
		case kVMEVulomTBActionReset:
		  {
			Int_t scd = 0;
			curModule->SetScaleDown(scd);
		  }
		  break;
		case kVMEVulomTBActionStartTimer:
		  {
			Int_t interval = fTimerInterval->GetText2Int();
			if (interval > 0) {
			  fTimer->SetTime(interval);
			  fTimer->TurnOn();
			} else {
			  gVMEControlData->MsgBox(this, "PerformAction", "No timer", "Timer interval not set");
			}
		  }
		  break;
		case kVMEVulomTBActionStopTimer:
		  {
			fTimer->SetTime(0);
			fTimer->TurnOff();
		  }
		  break;
	  }
	}
	this->UpdateGUI();
}

void VMEVulomTBPanel::UpdateGUI() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel::UpdateGUI
// Purpose:        Update values
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Update GUI.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (curModule == NULL) {
		curModule = (TC2LVulomTB *) fLofModules.At(0);
		if (curModule == NULL) return;
		curModule->SetVerbose(gVMEControlData->IsVerbose());
		curModule->SetOffline(gVMEControlData->IsOffline());
	}

	fSelectModule->Select(curModule->GetIndex());

	TString moduleName = curModule->GetName();
	fSettingsFrame->SetTitle(Form("Settings for module %s", moduleName.Data()));

	UInt_t enable;
	curModule->GetEnableMask(enable);
	UInt_t bit = 0x1;
	for (Int_t ch = 0; ch < kVMENofVulomTBChans; ch++) {
		fChannelEnable[ch]->SetDown((enable & bit) != 0);
		bit <<= 1;
	}

	TArrayI scd(kVMENofVulomTBChans);
 	curModule->ReadScaleDown(scd);
 	for (Int_t ch = 0; ch < kVMENofVulomTBChans; ch++) fScaleDownValue[ch]->SetText(scd[ch]);

	TArrayI sca(kVMENofVulomTBChans);
	curModule->ReadScaler(sca, 0);
	for (Int_t ch = 0; ch < kVMENofVulomTBChans; ch++) fScalerFree[ch]->SetText(sca[ch]);

	curModule->ReadScaler(sca, 1);
	for (Int_t ch = 0; ch < kVMENofVulomTBChans; ch++) fScalerInhibit[ch]->SetText(sca[ch]);

	curModule->ReadScaler(sca, 2);
	for (Int_t ch = 0; ch < kVMENofVulomTBChans; ch++) fScalerScaledDown[ch]->SetText(sca[ch]);
}

void VMEVulomTBPanel::ModuleChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel::ModuleChanged()
// Purpose:        Slot method: module selection changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	curModule = (TC2LVulomTB *) fLofModules.FindByIndex(Selection);
	curModule->SetVerbose(gVMEControlData->IsVerbose());
	curModule->SetOffline(gVMEControlData->IsOffline());
	this->UpdateGUI();
}

void VMEVulomTBPanel::ScaleDownChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel::ScaleDownChanged
// Purpose:        Slot method: scale down value changed
// Arguments:      Int_t FrameId     -- frame id
//                 Int_t Selection   -- selection (ignored)
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	curChannel = FrameId;
	if (curModule) {
	  Int_t scd = fScaleDownValue[curChannel]->GetText2Int();
	  curModule->SetScaleDown(scd, curChannel);
	}
	this->UpdateGUI();
}

void VMEVulomTBPanel::TimerIntervalChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel::TimerIntervalChanged
// Purpose:        Slot method: timer interval changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection (ignored)
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t interval = fTimerInterval->GetText2Int();
	fTimer->SetTime(interval);
}

void VMEVulomTBPanel::ChannelEnableChanged(Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEVulomTBPanel::ChannelEnableChanged
// Purpose:        Slot method: set/clear channel enable bit
// Arguments:      Int_t Channel     -- channel number
// Results:        --
// Exceptions:
// Description:    Called on TGCheckButton::Clicked()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	curChannel = Channel;
	if (curModule) {
	  if (fChannelEnable[curChannel]->IsDown()) {
		curModule->EnableChannel(curChannel);
	  } else {
		curModule->DisableChannel(curChannel);
	  }
	}
	this->UpdateGUI();
}

