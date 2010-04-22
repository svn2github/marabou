//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel
// Purpose:        A GUI to control vme modules via tcp
// Description:    Connect to server
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302CopyPanel.cxx,v 1.5 2010-04-22 13:44:41 Rudolf.Lutter Exp $
// Date:
// URL:
// Keywords:
// Layout:
//////////////////////////////////////////////////////////////////////////////

#include "TObjString.h"
#include "TGMsgBox.h"
#include "TMath.h"

#include "TMrbLogger.h"
#include "TMrbC2Lynx.h"

#include "TGMrbProgressBar.h"

#include "TC2LSis3302.h"
#include "VMESis3302Panel.h"
#include "VMESis3302CopyPanel.h"

#include "SetColor.h"

extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

ClassImp(VMESis3302CopyPanel)

const SMrbNamedX kVMEActions[] =
			{
				{VMESis3302CopyPanel::kVMESis3302ActionCopy,	"Copy",		"Copy settings from source to dest"	},
				{VMESis3302CopyPanel::kVMESis3302ActionClose,	"Close",	"Close window"	},
				{0, 												NULL,			NULL								}
			};

static TC2LSis3302 * curModule = NULL;
static Int_t curChannel = 0;

VMESis3302CopyPanel::VMESis3302CopyPanel(const TGWindow * Window, TMrbLofNamedX * LofModules,
												UInt_t Width, UInt_t Height, UInt_t Options)
														: TGMainFrame(Window, Width, Height, Options) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel
// Purpose:        Connect to LynxOs server
// Arguments:
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

// geometry
	Int_t frameWidth = this->GetWidth();

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
	TGLayoutHints * loNormal = new TGLayoutHints(kLHintsTop, 10, 10, 1, 1);
	TGLayoutHints * loNormal2 = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	HEAP(loXpndX); HEAP(loXpndX2); HEAP(loNormal); HEAP(loNormal2);

	frameGC->SetLH(loNormal);
	comboGC->SetLH(loNormal2);
	labelGC->SetLH(loNormal);
	buttonGC->SetLH(loNormal);
	entryGC->SetLH(loNormal);
	groupGC->SetLH(loXpndX);

	TObjArray * lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));

//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kVMEActions);

// modules
	Int_t nofModules = 0;
	fLofModules = LofModules;
	fLofSelected.Delete();
	TIterator * iter = fLofModules->MakeIterator();
	TC2LSis3302 * module;
	Int_t idx = kVMESis3302SelectModuleTo;
	while (module = (TC2LSis3302 *) iter->Next()) {
		fLofSelected.AddNamedX(idx, module->GetName(), "", module);
		idx++;
		nofModules++;
	}

//	module / channel selection: source
	fCopyFromFrame = new TGGroupFrame(this, "Copy from", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCopyFromFrame);
	this->AddFrame(fCopyFromFrame, groupGC->LH());

	fSelectModuleFrom = new TGMrbLabelCombo(fCopyFromFrame, "Module",	fLofModules,
																kVMESis3302SelectModuleFrom, 1,
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, comboGC, labelGC);
	fCopyFromFrame->AddFrame(fSelectModuleFrom, frameGC->LH());
	fSelectModuleFrom->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ModuleFromChanged(Int_t, Int_t)");

	TMrbLofNamedX lofChannels;
	gVMEControlData->GetLofChannels(lofChannels, VMESis3302Panel::kVMENofSis3302Chans);

	fSelectChannelFrom = new TGMrbLabelCombo(fCopyFromFrame, "Channel",	&lofChannels,
																kVMESis3302SelectChannelFrom, 1,
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, comboGC, labelGC);
	fCopyFromFrame->AddFrame(fSelectChannelFrom, frameGC->LH());
	fSelectChannelFrom->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ChannelFromChanged(Int_t, Int_t)");

//	module / channel selection: destination
	fCopyToFrame = new TGGroupFrame(this, "Copy to", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCopyToFrame);
	this->AddFrame(fCopyToFrame, groupGC->LH());

	TMrbLofNamedX lofChannelBits;
	gVMEControlData->GetLofChannels(lofChannelBits, VMESis3302Panel::kVMENofSis3302Chans, "chn %d", kTRUE);
	Int_t bit = 0x1;

	fLofDests.Delete();
	for (Int_t i = 0; i < nofModules; i++) {
		TMrbNamedX * m = (TMrbNamedX *) fLofSelected.At(i);
		TGMrbCheckButtonList * cbl = new TGMrbCheckButtonList(fCopyToFrame, m->GetName(), &lofChannelBits,
													m->GetIndex(), 1,
													frameWidth, kLEHeight,
													frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(cbl);
		fCopyToFrame->AddFrame(cbl, groupGC->LH());
		fLofDests.AddNamedX(m->GetIndex(), m->GetName(), "", cbl);
	}

// action buttons
	fActionButtons = new TGMrbTextButtonGroup(this, "Actions", &fActions, -1, 1, groupGC, buttonGC);
	HEAP(fActionButtons);
	this->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	((TGMrbButtonFrame *) fActionButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gVMEControlData->fColorGreen);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(Width, Height);
	MapWindow();
}

void VMESis3302CopyPanel::StartGUI() {
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

	fSelectModuleFrom->Select(curModule->GetIndex());
	fSelectChannelFrom->Select(curChannel);
}

void VMESis3302CopyPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:
// Exceptions:
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case VMESis3302CopyPanel::kVMESis3302ActionCopy:
			this->CopySettings();
			break;
		case VMESis3302CopyPanel::kVMESis3302ActionClose:
			this->CloseWindow();
			break;
	}
}

void VMESis3302CopyPanel::ModuleFromChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel::ModuleFromChanged()
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

void VMESis3302CopyPanel::ChannelFromChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel::ChannelFromChanged
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

void VMESis3302CopyPanel::CopySettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302CopyPanel::CopySettings
// Purpose:        Copy module settings
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Copies settings from source module to destination(s)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TIterator * iter = fLofDests.MakeIterator();
	TMrbNamedX * nx;
	Int_t nofChans = 0;
	while (nx = (TMrbNamedX *) iter->Next()) {
		TString destModuleName = nx->GetName();
		TC2LSis3302 * destModule = (TC2LSis3302 *) fLofModules->FindByName(destModuleName.Data());
		Bool_t sameModule = destModuleName.CompareTo(curModule->GetName()) == 0;
		TGMrbCheckButtonList * cbl = (TGMrbCheckButtonList *) nx->GetAssignedObject();
		UInt_t pattern = cbl->GetActive();
		Int_t bit = 0x1;
		for (Int_t chn = 0; chn < VMESis3302Panel::kVMENofSis3302Chans; chn++, bit <<= 1) {
			if (pattern & bit) {
				nofChans++;
				if (sameModule && (chn == curChannel)) continue;

				if (gVMEControlData->IsVerbose()) {
					cout	<< "[Copysettings(): " << curModule->GetName() << ", chn " << curChannel
							<< " --> " << destModuleName << ", chn " << chn << "]" << endl;
				}

				if (!sameModule) {
					Int_t clockSource;
					curModule->GetClockSource(clockSource);
					destModule->SetClockSource(clockSource);

					Int_t lemo;
					curModule->GetLemoInMode(lemo);
					destModule->SetLemoInMode(lemo);
					curModule->GetLemoOutMode(lemo);
					destModule->SetLemoOutMode(lemo);
					curModule->GetLemoInEnableMask(lemo);
					destModule->SetLemoInEnableMask(lemo);
				}

				Int_t hdrBits;
				curModule->GetHeaderBits(hdrBits, curChannel);
				hdrBits &= 0x7ffc;
				destModule->SetHeaderBits(hdrBits, chn);

				Int_t trigger, gate;
				curModule->GetTriggerMode(trigger, curChannel);
				destModule->SetTriggerMode(trigger, chn);
				curModule->GetGateMode(gate, curChannel);
				destModule->SetGateMode(gate, chn);
				curModule->GetNextNeighborTriggerMode(trigger, curChannel);
				destModule->SetNextNeighborTriggerMode(trigger, chn);
				curModule->GetNextNeighborGateMode(gate, curChannel);
				destModule->SetNextNeighborGateMode(gate, chn);

				Bool_t invert;
				curModule->GetPolarity(invert, curChannel);
				destModule->SetPolarity(invert, chn);

				Int_t delay;
				curModule->ReadPreTrigDelay(delay, curChannel);
				destModule->WritePreTrigDelay(delay, chn);

				curModule->ReadTrigGateLength(gate, curChannel);
				destModule->WriteTrigGateLength(gate, chn);

				Int_t length;
				curModule->ReadRawDataSampleLength(length, curChannel);
				destModule->WriteRawDataSampleLength(length, chn);

				Int_t start;
				curModule->ReadRawDataStartIndex(start, curChannel);
				destModule->WriteRawDataStartIndex(start, chn);

				Int_t peak;
				Int_t gap;
				curModule->ReadTrigPeakAndGap(peak, gap, curChannel);
				destModule->WriteTrigPeakAndGap(peak, gap, chn);

				curModule->ReadTrigInternalGate(gate, curChannel);
				destModule->WriteTrigInternalGate(gate, chn);
				curModule->ReadTrigInternalDelay(delay, curChannel);
				destModule->WriteTrigInternalDelay(delay, chn);

				curModule->ReadTrigPulseLength(length, curChannel);
				destModule->WriteTrigPulseLength(length, chn);

				Int_t decim;
				curModule->GetTrigDecimation(decim, curChannel);
				destModule->SetTrigDecimation(decim, chn);

				Int_t thresh;
				curModule->ReadTrigThreshold(thresh, curChannel);
				destModule->WriteTrigThreshold(thresh, chn);

				Bool_t gt;
				curModule->GetTriggerGT(gt, curChannel);
				destModule->SetTriggerGT(gt, chn);

				Bool_t out;
				curModule->GetTriggerOut(out, curChannel);
				destModule->SetTriggerOut(out, chn);

				curModule->ReadEnergyPeakAndGap(peak, gap, curChannel);
				destModule->WriteEnergyPeakAndGap(peak, gap, chn);

				curModule->GetEnergyDecimation(decim, curChannel);
				destModule->SetEnergyDecimation(decim, chn);

				curModule->ReadEnergyGateLength(gate, curChannel);
				destModule->WriteEnergyGateLength(gate, chn);

				Int_t test;
				curModule->GetTestBits(test, curChannel);
				destModule->SetTestBits(test, chn);

				curModule->ReadEnergySampleLength(length, curChannel);
				destModule->WriteEnergySampleLength(length, chn);

				curModule->ReadStartIndex(start, 0, curChannel);
				destModule->WriteStartIndex(start, 0, chn);

				curModule->ReadStartIndex(start, 1, curChannel);
				destModule->WriteStartIndex(start, 1, chn);

				curModule->ReadStartIndex(start, 2, curChannel);
				destModule->WriteStartIndex(start, 2, chn);

				Int_t tau;
				curModule->ReadTauFactor(tau, curChannel);
				destModule->WriteTauFactor(tau, chn);
			}
		}
	}
	if (nofChans == 0) gVMEControlData->MsgBox(this, "CopySettings", "Error", "No channels selected");
}
