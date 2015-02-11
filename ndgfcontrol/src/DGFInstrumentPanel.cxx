//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Instrument Panel
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFInstrumentPanel.cxx,v 1.1 2009-09-23 10:46:24 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFInstrumentPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"
#include "TGMrbProgressBar.h"

#include "DGFControlData.h"
#include "DGFInstrumentPanel.h"
#include "DGFEditModICSRPanel.h"
#include "DGFEditChanCSRAPanel.h"
#include "DGFEditUserPsaCSRPanel.h"
#include "DGFEditRunTaskPanel.h"
#include "DGFEditCoincPatternPanel.h"
#include "DGFCopyModuleSettingsPanel.h"

#include "SetColor.h"

const SMrbNamedX kDGFInstrumentModuleButtons[] =
			{
				{DGFInstrumentPanel::kDGFInstrButtonShowParams,		"Show params", 	"Show actual param settings"		},
				{DGFInstrumentPanel::kDGFInstrButtonUpdateFPGAs,	"Update FPGAs", "Write params and update FPGAs"	},
				{0, 												NULL,		NULL									}
			};

const SMrbNamedX kDGFInstrCFDOnOff[] =
			{
				{DGFInstrumentPanel::kDGFInstrCFDOn,	"on",	"Enable hardware CFD"},
				{DGFInstrumentPanel::kDGFInstrCFDOff,	"off",	"Disable hardware CFD"},
				{0, 			NULL,	NULL	}
			};

const SMrbNamedX kDGFInstrCFDFraction[] =
			{
				{DGFInstrumentPanel::kDGFInstrCFDFract00,	"0.5",		"Attenuation 0.5"},
				{DGFInstrumentPanel::kDGFInstrCFDFract01,	"0.25",		"Attenuation 0.25"},
				{DGFInstrumentPanel::kDGFInstrCFDFract10,	"0.125",	"Attenuation 0.125"},
				{0, 			NULL,	NULL	}
			};

const SMrbNamedX kDGFInstrUPSAOnOff[] =
			{
				{DGFInstrumentPanel::kDGFInstrUPSAOn,		"on",		"Enable UserPSA"},
				{DGFInstrumentPanel::kDGFInstrUPSAOff,		"off",		"Disable UserPSA"},
				{0, 			NULL,	NULL	}
			};

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

extern TNGMrbLofProfiles * gMrbLofProfiles;
static TNGMrbProfile * stdProfile;

ClassImp(DGFInstrumentPanel)

DGFInstrumentPanel::DGFInstrumentPanel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel
// Purpose:        DGF Viewer: Instrument Panel
// Arguments:      TGCompositeFrame * TabFrame   -- pointer to tab object
// Results:        
// Exceptions:     
// Description:    Implements DGF Viewer's Instrument Panel
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////// Profiles Laden////////////////////////////////////////

	if (gMrbLofProfiles == NULL) gMrbLofProfiles = new TNGMrbLofProfiles();
	//stdProfile = gMrbLofProfiles->GetDefault();
	stdProfile=gMrbLofProfiles->FindProfile("green", kTRUE);
	//yellowProfile = gMrbLofProfiles->FindProfile("yellow", kTRUE);
	//blueProfile = gMrbLofProfiles->FindProfile("blue", kTRUE);

	gMrbLofProfiles->Print();

////////////////////////////////////////////////////////////////////////////////////////////////

//	TGMrbLayout * frameGC;
//	TGMrbLayout * groupGC;
//	TGMrbLayout * entryGC;
//	TGMrbLayout * labelGC;
//	TGMrbLayout * buttonGC;
//	TGMrbLayout * comboGC;
    

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	Clear focus list
	fFocusList.Clear();

/*	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);

	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);

	comboGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(comboGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(labelGC);

	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);
*/

//	Initialize several lists
	fInstrModuleActions.SetName("Instrument module actions");
	fInstrModuleActions.AddNamedX(kDGFInstrumentModuleButtons);

	fInstrCFDOnOff.SetName("CFD on/off");
	fInstrCFDOnOff.AddNamedX(kDGFInstrCFDOnOff);
	fInstrCFDOnOff.SetPatternMode();

	fInstrCFDFraction.SetName("CFD fraction");
	fInstrCFDFraction.AddNamedX(kDGFInstrCFDFraction);
	fInstrCFDFraction.SetPatternMode();

	fInstrUPSAOnOff.SetName("UserPSA on/off");
	fInstrUPSAOnOff.AddNamedX(kDGFInstrUPSAOnOff);
	fInstrUPSAOnOff.SetPatternMode();

	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	this->ChangeBackground(gDGFControlData->fColorGreen);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

//	module / channel selection
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, selectLayout);
	HEAP(selectLayout);
	fSelectFrame = new TGGroupFrame(this, "DGF Selection", kHorizontalFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, selectLayout);

	TGLayoutHints * smfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 10, 1, 10, 1);
//	frameGC->SetLH(smfLayout);
	HEAP(smfLayout);
	TGLayoutHints * smlLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	labelGC->SetLH(smlLayout);
	HEAP(smlLayout);
	TGLayoutHints * smcLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
//	comboGC->SetLH(smcLayout);
	HEAP(smcLayout);

	Bool_t clearList = kTRUE;
	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		gDGFControlData->CopyKeyList(&fLofModuleKeys, cl, gDGFControlData->GetPatInUse(cl), clearList);
		clearList = kFALSE;
	}			
	gDGFControlData->SetSelectedModule(fLofModuleKeys.First()->GetName());		// dgf to start with
	gDGFControlData->SetSelectedChannel(0); 									// channel 0

	fSelectModule = new TNGMrbLabelCombo(fSelectFrame,  "Module",
											&fLofModuleKeys,
											stdProfile, -1,
											DGFInstrumentPanel::kDGFInstrSelectModule, //2,
											kTabWidth, kLEHeight,
											kComboWidth,
											TNGMrbLabelCombo::kGMrbComboHasUpDownButtons|
											TNGMrbLabelCombo::kGMrbComboHasBeginEndButtons);
	HEAP(fSelectModule);
	fSelectFrame->AddFrame(fSelectModule, smfLayout);
	fSelectModule->GetComboBox()->Select(gDGFControlData->GetSelectedModuleIndex());
	fSelectModule->GetComboBox()->Connect("Selected(Int_t)", this->ClassName(), this, "SelectModule(Int_t)");

	TGLayoutHints * scfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 80, 1, 10, 1);
//	frameGC->SetLH(scfLayout);
	HEAP(scfLayout);
	TGLayoutHints * sclLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	labelGC->SetLH(sclLayout);
	HEAP(sclLayout);
	TGLayoutHints * scbLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	buttonGC->SetLH(scbLayout);
	HEAP(scbLayout);
	fSelectChannel = new TNGMrbRadioButtonList(fSelectFrame, "Channel", &fLofChannels,
													kDGFInstrSelectChannel,stdProfile,1,0,
													kTabWidth, kLEHeight);
	HEAP(fSelectChannel);
	fSelectChannel->SetState(gDGFControlData->GetSelectedChannelIndex());
	((TNGMrbButtonFrame *) fSelectChannel)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "RadioButtonPressed(Int_t, Int_t)");
	fSelectFrame->AddFrame(fSelectChannel, scfLayout);

// 2 vertical frames, left and right
	TGLayoutHints * hLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
//	frameGC->SetLH(hLayout);
	HEAP(hLayout);
	fInstrFrame = new TGHorizontalFrame(this, kTabWidth, kTabHeight, kChildFrame,stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fInstrFrame);
	this->AddFrame(fInstrFrame, hLayout);

	TGLayoutHints * vLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	frameGC->SetLH(vLayout);
	HEAP(vLayout);
	fLeftFrame = new TGVerticalFrame(fInstrFrame, kVFrameWidth, kVFrameHeight, kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fLeftFrame);
	fInstrFrame->AddFrame(fLeftFrame, vLayout);

	fRightFrame = new TGVerticalFrame(fInstrFrame, kVFrameWidth, kVFrameHeight, kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fRightFrame);
	fInstrFrame->AddFrame(fRightFrame, vLayout);

// left: filter
	TGLayoutHints * filterLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, filterLayout);
	HEAP(filterLayout);
	fFilterFrame = new TGGroupFrame(fLeftFrame, "Filter Settings", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fFilterFrame);
	fLeftFrame->AddFrame(fFilterFrame, filterLayout);

// left/left: energy
	TGLayoutHints * efLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, efLayout);
	HEAP(efLayout);
	fEnergyFilterFrame = new TGGroupFrame(fFilterFrame, "Energy", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fEnergyFilterFrame);
	fFilterFrame->AddFrame(fEnergyFilterFrame, efLayout);

	TGLayoutHints * eeLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
//	entryGC->SetLH(eeLayout);
	HEAP(eeLayout);
	fEnergyPeakTimeEntry = new TNGMrbLabelEntry(fEnergyFilterFrame, "Peaking [us]",
																kDGFInstrEnergyPeakTimeEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fEnergyPeakTimeEntry);
	fEnergyFilterFrame->AddFrame(fEnergyPeakTimeEntry, efLayout);
//	fEnergyPeakTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 1);
	fEnergyPeakTimeEntry->SetFormat(TGNumberEntry::kNESRealOne);
	fEnergyPeakTimeEntry->SetNumber(0.0);
//	fEnergyPeakTimeEntry->SetRange(0, 100000);
	fEnergyPeakTimeEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,100000);
//	fEnergyPeakTimeEntry->SetIncrement(gDGFControlData->fDeltaT);
	fEnergyPeakTimeEntry->SetIncrement(gDGFControlData->fDeltaT);
	fEnergyPeakTimeEntry->ShowToolTip(kTRUE, kTRUE);
	fEnergyPeakTimeEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fEnergyPeakTimeEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrEnergyPeakTimeEntry));
	fEnergyPeakTimeEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fEnergyGapTimeEntry = new TNGMrbLabelEntry(fEnergyFilterFrame, "Gap [us]",
																kDGFInstrEnergyGapTimeEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fEnergyGapTimeEntry);
	fEnergyFilterFrame->AddFrame(fEnergyGapTimeEntry, efLayout);
//	fEnergyGapTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 1);
	fEnergyGapTimeEntry->SetFormat(TGNumberEntry::kNESRealOne);
	fEnergyGapTimeEntry->SetNumber(0.0);
//	fEnergyGapTimeEntry->SetRange(0, 100000);
	fEnergyGapTimeEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,100000);
	fEnergyGapTimeEntry->SetIncrement(gDGFControlData->fDeltaT);
	fEnergyGapTimeEntry->ShowToolTip(kTRUE, kTRUE);
	fEnergyGapTimeEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fEnergyGapTimeEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrEnergyGapTimeEntry));
	fEnergyGapTimeEntry->Connect("EntryChanged(Int_t", this->ClassName(), this, "EntryChanged(Int_t)");

	fEnergyAveragingEntry = new TNGMrbLabelEntry(fEnergyFilterFrame, "Averaging 2^",
																kDGFInstrEnergyAveragingEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fEnergyAveragingEntry);
	fEnergyFilterFrame->AddFrame(fEnergyAveragingEntry, efLayout);
//	fEnergyAveragingEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyAveragingEntry->SetFormat(TGNumberEntry::kNESInteger);
	fEnergyAveragingEntry->SetIntNumber(2);
//	fEnergyAveragingEntry->SetRange(-10, -1);
	fEnergyAveragingEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,-10,-1);
	fEnergyAveragingEntry->ShowToolTip(kTRUE, kTRUE);
	fEnergyAveragingEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fEnergyAveragingEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrEnergyAveragingEntry));
	fEnergyAveragingEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fEnergyTauEntry = new TNGMrbLabelEntry(fEnergyFilterFrame, "Tau value",
																kDGFInstrEnergyTauEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fEnergyTauEntry);
	fEnergyFilterFrame->AddFrame(fEnergyTauEntry, efLayout);
//	fEnergyTauEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 1);
	fEnergyTauEntry->SetFormat(TGNumberEntry::kNESRealOne);
	fEnergyTauEntry->SetNumber(0.0);
//	fEnergyTauEntry->SetRange(0, 99);
	fEnergyTauEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,99);
	fEnergyTauEntry->SetIncrement(0.2);
	fEnergyTauEntry->ShowToolTip(kTRUE, kTRUE);
	fEnergyTauEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fEnergyTauEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrEnergyTauEntry));
	fEnergyTauEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

// left/right: trigger
	TGLayoutHints * tfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, tfLayout);
	HEAP(tfLayout);
	fTriggerFilterFrame = new TGGroupFrame(fFilterFrame, "Trigger", kVerticalFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fTriggerFilterFrame);
	fFilterFrame->AddFrame(fTriggerFilterFrame, tfLayout);

	TGLayoutHints * teLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
//	entryGC->SetLH(teLayout);
	HEAP(teLayout);
	fTriggerPeakTimeEntry = new TNGMrbLabelEntry(fTriggerFilterFrame, "Peaking [us]",
																kDGFInstrTriggerPeakTimeEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTriggerPeakTimeEntry);
	fTriggerFilterFrame->AddFrame(fTriggerPeakTimeEntry, tfLayout);
//	fTriggerPeakTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble,0, 3);
	fTriggerPeakTimeEntry->SetFormat(TGNumberEntry::kNESRealThree);
	fTriggerPeakTimeEntry->SetNumber(0.025);
//	fTriggerPeakTimeEntry->SetRange(.025, .1);
	fTriggerPeakTimeEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0.025,0.1);
	fTriggerPeakTimeEntry->SetIncrement(.025);
	fTriggerPeakTimeEntry->ShowToolTip(kTRUE, kTRUE);
	fTriggerPeakTimeEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTriggerPeakTimeEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTriggerPeakTimeEntry));
	fTriggerPeakTimeEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fTriggerGapTimeEntry = new TNGMrbLabelEntry(fTriggerFilterFrame, "Gap [us]",
																kDGFInstrTriggerGapTimeEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTriggerGapTimeEntry);
	fTriggerFilterFrame->AddFrame(fTriggerGapTimeEntry, tfLayout);
//	fTriggerGapTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 3);
	fTriggerGapTimeEntry->SetFormat(TGNumberEntry::kNESRealThree);
	fTriggerGapTimeEntry->SetNumber(0.0);
//	fTriggerGapTimeEntry->SetRange(0, .75);
	fTriggerGapTimeEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0.75);
	fTriggerGapTimeEntry->SetIncrement(.025);
	fTriggerGapTimeEntry->ShowToolTip(kTRUE, kTRUE);
	fTriggerGapTimeEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTriggerGapTimeEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTriggerGapTimeEntry));
	fTriggerGapTimeEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fTriggerThresholdEntry = new TNGMrbLabelEntry(fTriggerFilterFrame, "Threshold",
																kDGFInstrTriggerThresholdEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTriggerThresholdEntry);
	fTriggerFilterFrame->AddFrame(fTriggerThresholdEntry, tfLayout);
//	fTriggerThresholdEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTriggerThresholdEntry->SetFormat(TGNumberEntry::kNESInteger);
	fTriggerThresholdEntry->SetIntNumber(0);
//	fTriggerThresholdEntry->SetRange(0, 1000);
	fTriggerThresholdEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,100);
	fTriggerThresholdEntry->ShowToolTip(kTRUE, kTRUE);
	fTriggerThresholdEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTriggerThresholdEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTriggerThresholdEntry));
	fTriggerThresholdEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

// place an unvisible label to pad group frame vertically
	TGLayoutHints * dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 12, 0);
//	frameGC->SetLH(dmyLayout);
	HEAP(dmyLayout);
	fDummyLabel = new TGLabel(fTriggerFilterFrame, "", stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->GC(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->Font(), kChildFrame,stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->BG());
	HEAP(fDummyLabel);
	fTriggerFilterFrame->AddFrame(fDummyLabel, dmyLayout);

// right: DAC Settings
	TGLayoutHints * dacLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, dacLayout);
	HEAP(dacLayout);
	fDACFrame = new TGGroupFrame(fRightFrame, "DAC Settings", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fDACFrame);
	fRightFrame->AddFrame(fDACFrame, dacLayout);

// right/left: gain
	TGLayoutHints * dgLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, dgLayout);
	HEAP(dgLayout);
	fDACGainFrame = new TGGroupFrame(fDACFrame, "Gain", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fDACGainFrame);
	fDACFrame->AddFrame(fDACGainFrame, dgLayout);

	fDACGainEntry = new TNGMrbLabelEntry(fDACGainFrame, "DAC",
																kDGFInstrDACGainEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fDACGainEntry);
	fDACGainFrame->AddFrame(fDACGainEntry, dgLayout);
//	fDACGainEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fDACGainEntry->SetFormat(TGNumberEntry::kNESInteger);
	fDACGainEntry->SetIntNumber(0);
//	fDACGainEntry->SetRange(0, 65535);
	fDACGainEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,65535);
	fDACGainEntry->SetIncrement(1);
	fDACGainEntry->ShowToolTip(kTRUE, kTRUE);
	fDACGainEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fDACGainEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrDACGainEntry));
	fDACGainEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fDACVVEntry = new TNGMrbLabelEntry(fDACGainFrame, "V/V",
																kDGFInstrDACVVEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fDACVVEntry);
	fDACGainFrame->AddFrame(fDACVVEntry, dgLayout);
//	fDACVVEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 4);
	fDACVVEntry->SetFormat(TGNumberEntry::kNESRealFour);
	fDACVVEntry->SetNumber(0.0);
//	fDACVVEntry->SetRange(0, 20)
	fDACVVEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,20);;
	fDACVVEntry->SetIncrement(.1);
	fDACVVEntry->ShowToolTip(kTRUE, kTRUE);
	fDACVVEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fDACVVEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrDACVVEntry));
	fDACVVEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

// place an unvisible label to pad group frame vertically
	dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 36, 0);
//	frameGC->SetLH(dmyLayout);
	HEAP(dmyLayout);
	fDummyLabel = new TGLabel(fDACGainFrame, "",  stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->GC(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->Font(), kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->BG());
	HEAP(fDummyLabel);
	fDACGainFrame->AddFrame(fDummyLabel, dmyLayout);

// right/right: offset
	TGLayoutHints * doLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, doLayout);
	HEAP(doLayout);
	fDACOffsetFrame = new TGGroupFrame(fDACFrame, "Offset", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fDACOffsetFrame);
	fDACFrame->AddFrame(fDACOffsetFrame, doLayout);

	fDACOffsetEntry = new TNGMrbLabelEntry(fDACOffsetFrame, "DAC",
																kDGFInstrDACOffsetEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fDACOffsetEntry);
	fDACOffsetFrame->AddFrame(fDACOffsetEntry, doLayout);
//	fDACOffsetEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fDACOffsetEntry->SetFormat(TGNumberEntry::kNESInteger);
	fDACOffsetEntry->SetIntNumber(0);
//	fDACOffsetEntry->SetRange(0, 65535);
	fDACOffsetEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,65535);
	fDACOffsetEntry->SetIncrement(1);
	fDACOffsetEntry->ShowToolTip(kTRUE, kTRUE);
	fDACOffsetEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fDACOffsetEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrDACOffsetEntry));
	fDACOffsetEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fDACVoltEntry = new TNGMrbLabelEntry(fDACOffsetFrame, "Volt",
																kDGFInstrDACVoltEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fDACVoltEntry);
	fDACOffsetFrame->AddFrame(fDACVoltEntry, doLayout);
//	fDACVoltEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 4);
	fDACVoltEntry->SetFormat(TGNumberEntry::kNESRealFour);
	fDACVoltEntry->SetNumber(0);
//	fDACVoltEntry->SetRange(-3, 3);
	fDACVoltEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,-3,3);
	fDACVoltEntry->SetIncrement(.1);
	fDACVoltEntry->ShowToolTip(kTRUE, kTRUE);
	fDACVoltEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fDACVoltEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrDACVoltEntry));
	fDACVoltEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t,)");

// place an unvisible label to pad group frame vertically
//	frameGC->SetLH(dmyLayout);
	fDummyLabel = new TGLabel(fDACOffsetFrame, "", stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->GC(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->Font(), kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->BG());
	HEAP(fDummyLabel);
	fDACOffsetFrame->AddFrame(fDummyLabel, dmyLayout);

// left: trace
	TGLayoutHints * traceLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, traceLayout);
	HEAP(traceLayout);
	fTraceFrame = new TGGroupFrame(fLeftFrame, "Trace Settings", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fTraceFrame);
	fLeftFrame->AddFrame(fTraceFrame, traceLayout);

	fTraceLeftFrame = new TGVerticalFrame(fTraceFrame, kTabWidth, kTabHeight, kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fTraceLeftFrame);
	fTraceFrame->AddFrame(fTraceLeftFrame, traceLayout);

	fTraceLengthDelayFrame = new TGGroupFrame(fTraceLeftFrame, "Trace", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fTraceLengthDelayFrame);
	fTraceLeftFrame->AddFrame(fTraceLengthDelayFrame, traceLayout);
	TGLayoutHints * tlLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(tlLayout);
	HEAP(tlLayout);
	fTraceLengthEntry = new TNGMrbLabelEntry(fTraceLengthDelayFrame, "Length [us]",
																kDGFInstrTraceLengthEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceLengthEntry);
	fTraceLengthDelayFrame->AddFrame(fTraceLengthEntry, traceLayout);
//	fTraceLengthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 1);
	fTraceLengthEntry->SetFormat(TGNumberEntry::kNESRealOne);
	fTraceLengthEntry->SetNumber(0.0);
//	fTraceLengthEntry->SetRange(0, 25.6);
	fTraceLengthEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,25.6);
	fTraceLengthEntry->SetIncrement(.5);
	fTraceLengthEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceLengthEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceLengthEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceLengthEntry));
	fTraceLengthEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	TGLayoutHints * tdLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(tdLayout);
	HEAP(tdLayout);
	fTraceDelayEntry = new TNGMrbLabelEntry(fTraceLengthDelayFrame, "Delay [us]",
																kDGFInstrTraceDelayEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceDelayEntry);
	fTraceLengthDelayFrame->AddFrame(fTraceDelayEntry,traceLayout);
//	fTraceDelayEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 1);
	fTraceDelayEntry->SetFormat(TGNumberEntry::kNESRealOne);
	fTraceDelayEntry->SetNumber(0.0);
//	fTraceDelayEntry->SetRange(0, 100000);
	fTraceDelayEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,100000);
	fTraceDelayEntry->SetIncrement(.1);
	fTraceDelayEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceDelayEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceDelayEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceDelayEntry));
	fTraceDelayEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fTraceXPSAFrame = new TGGroupFrame(fTraceLeftFrame, "XIA PSA", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fTraceXPSAFrame);
	fTraceLeftFrame->AddFrame(fTraceXPSAFrame, traceLayout);

	TGLayoutHints * psaLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(psaLayout);
	HEAP(psaLayout);
	fTraceXPSALengthEntry = new TNGMrbLabelEntry(fTraceXPSAFrame, "Length [us]",
																kDGFInstrTraceXPSALengthEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceXPSALengthEntry);
	fTraceXPSAFrame->AddFrame(fTraceXPSALengthEntry, traceLayout);
//	fTraceXPSALengthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 1);
	fTraceXPSALengthEntry->SetFormat(TGNumberEntry::kNESRealOne);
	fTraceXPSALengthEntry->SetNumber(0.0);
//	fTraceXPSALengthEntry->SetRange(0, 25);
	fTraceXPSALengthEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,25);
	fTraceXPSALengthEntry->SetIncrement(.5);
	fTraceXPSALengthEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceXPSALengthEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceXPSALengthEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceXPSALengthEntry));
	fTraceXPSALengthEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

//	entryGC->SetLH(psaLayout);
	fTraceXPSAOffsetEntry = new TNGMrbLabelEntry(fTraceXPSAFrame, "Offset [us]",
																kDGFInstrTraceXPSAOffsetEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceXPSAOffsetEntry);
	fTraceXPSAFrame->AddFrame(fTraceXPSAOffsetEntry, traceLayout);
//	fTraceXPSAOffsetEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble, 0, 1);
	fTraceXPSAOffsetEntry->SetFormat(TGNumberEntry::kNESRealOne);
	fTraceXPSAOffsetEntry->SetNumber(0.0);
//	fTraceXPSAOffsetEntry->SetRange(0, 25);
	fTraceXPSAOffsetEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,25);
	fTraceXPSAOffsetEntry->SetIncrement(.1);
	fTraceXPSAOffsetEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceXPSAOffsetEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceXPSAOffsetEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceXPSAOffsetEntry));
	fTraceXPSAOffsetEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fTraceRightFrame = new TGVerticalFrame(fTraceFrame, kTabWidth, kTabHeight, kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	HEAP(fTraceRightFrame);
	fTraceFrame->AddFrame(fTraceRightFrame, traceLayout);

	fTraceUPSAFrame = new TGGroupFrame(fTraceRightFrame, "User PSA", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fTraceUPSAFrame);
	fTraceRightFrame->AddFrame(fTraceUPSAFrame, traceLayout);

#if 0
	fTraceUPSAOnOffButton = new TGMrbRadioButtonList(fTraceUPSAFrame, "PSA enable", &fInstrUPSAOnOff,
													kDGFInstrTraceUPSAOnOffButton, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, comboGC);
	HEAP(fTraceUPSAOnOffButton);
	fTraceUPSAOnOffButton->SetState(DGFInstrumentPanel::kDGFInstrUPSAOff);
	((TGMrbButtonFrame *) fTraceUPSAOnOffButton)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "RadioButtonPressed(Int_t, Int_t)");
	fTraceUPSAFrame->AddFrame(fTraceUPSAOnOffButton, frameGC->);
#endif

//	entryGC->SetLH(psaLayout);
	fTraceUPSABaselineEntry = new TNGMrbLabelEntry(fTraceUPSAFrame, "Baseline",
																kDGFInstrTraceUPSABaselineEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceUPSABaselineEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSABaselineEntry,traceLayout);
//	fTraceUPSABaselineEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSABaselineEntry->SetFormat(TGNumberEntry::kNESInteger);
	fTraceUPSABaselineEntry->SetNumber(0);
//	fTraceUPSABaselineEntry->SetRange(0, 0xF);
	fTraceUPSABaselineEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xF);
	fTraceUPSABaselineEntry->SetIncrement(1);
	fTraceUPSABaselineEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSABaselineEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceUPSABaselineEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceUPSABaselineEntry));
	fTraceUPSABaselineEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

//	entryGC->SetLH(psaLayout);
	fTraceUPSAEnergyCutoffEntry = new TNGMrbLabelEntry(fTraceUPSAFrame, "Energy cutoff",
																kDGFInstrTraceUPSAEnergyCutoffEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceUPSAEnergyCutoffEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSAEnergyCutoffEntry, traceLayout);
//	fTraceUPSAEnergyCutoffEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSAEnergyCutoffEntry->SetFormat(TGNumberEntry::kNESInteger);
	fTraceUPSAEnergyCutoffEntry->SetIntNumber(0);
//	fTraceUPSAEnergyCutoffEntry->SetRange(0, 0xFF);
	fTraceUPSAEnergyCutoffEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xFF);
	fTraceUPSAEnergyCutoffEntry->SetIncrement(1);
	fTraceUPSAEnergyCutoffEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSAEnergyCutoffEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceUPSAEnergyCutoffEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceUPSAEnergyCutoffEntry));
	fTraceUPSAEnergyCutoffEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

//	entryGC->SetLH(psaLayout);
	fTraceUPSATriggerThreshEntry = new TNGMrbLabelEntry(fTraceUPSAFrame, "Trigger thres",
																kDGFInstrTraceUPSATriggerThreshEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceUPSATriggerThreshEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSATriggerThreshEntry, traceLayout);
//	fTraceUPSATriggerThreshEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSATriggerThreshEntry->SetFormat(TGNumberEntry::kNESInteger);
	fTraceUPSATriggerThreshEntry->SetIntNumber(0);
//	fTraceUPSATriggerThreshEntry->SetRange(0, 0xFF);
	fTraceUPSATriggerThreshEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xFF);
	fTraceUPSATriggerThreshEntry->SetIncrement(1);
	fTraceUPSATriggerThreshEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSATriggerThreshEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceUPSATriggerThreshEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceUPSATriggerThreshEntry));
	fTraceUPSATriggerThreshEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

//	entryGC->SetLH(psaLayout);
	fTraceUPSAT90ThreshEntry = new TNGMrbLabelEntry(fTraceUPSAFrame, "T90 thresh",
																kDGFInstrTraceUPSAT90ThreshEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceUPSAT90ThreshEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSAT90ThreshEntry, traceLayout);
//	fTraceUPSAT90ThreshEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSAT90ThreshEntry->SetFormat(TGNumberEntry::kNESInteger);
	fTraceUPSAT90ThreshEntry->SetIntNumber(0);
//	fTraceUPSAT90ThreshEntry->SetRange(0, 0xF);
	fTraceUPSAT90ThreshEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xF);
	fTraceUPSAT90ThreshEntry->SetIncrement(1);
	fTraceUPSAT90ThreshEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSAT90ThreshEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceUPSAT90ThreshEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceUPSAT90ThreshEntry));
	fTraceUPSAT90ThreshEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

//	entryGC->SetLH(psaLayout);
	fUserPsaCSREditButton = new TMrbNamedX(kDGFInstrStatRegUserPsaCSREditButton, "Edit", "EditBits(Int_t)");
	fUserPsaCSREditButton->AssignObject(this);
	fStatRegUserPsaCSREntry = new TNGMrbLabelEntry(fTraceUPSAFrame, "PSA CSR",
																kDGFInstrStatRegUserPsaCSREntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth, TNGMrbLabelEntry::kGMrbEntryIsNumber,
																fUserPsaCSREditButton);
	HEAP(fStatRegUserPsaCSREntry);
	fTraceUPSAFrame->AddFrame(fStatRegUserPsaCSREntry, traceLayout);
//	fStatRegUserPsaCSREntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 6, 16);
	fStatRegUserPsaCSREntry->SetFormat(TGNumberEntry::kNESHex);
	fStatRegUserPsaCSREntry->SetHexNumber(0);
//	fStatRegUserPsaCSREntry->SetRange(0, 0xffff);
	fStatRegUserPsaCSREntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xffff);
	fStatRegUserPsaCSREntry->ShowToolTip(kTRUE, kTRUE);
	fStatRegUserPsaCSREntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fStatRegUserPsaCSREntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrStatRegUserPsaCSREntry));
	fStatRegUserPsaCSREntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");
	fStatRegUserPsaCSREntry->Connect("LabelButtonPressed(Int_t, Int_t)", this->ClassName(), this, "EditBits(Int_t, Int_t)");

//	entryGC->SetLH(psaLayout);
	fTraceUPSAOffsetEntry = new TNGMrbLabelEntry(fTraceUPSAFrame, "Offset",
																kDGFInstrTraceUPSAOffsetEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceUPSAOffsetEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSAOffsetEntry,traceLayout);
//	fTraceUPSAOffsetEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSAOffsetEntry->SetFormat(TGNumberEntry::kNESInteger);
	fTraceUPSAOffsetEntry->SetIntNumber(0);
//	fTraceUPSAOffsetEntry->SetRange(0, 0xFF);
	fTraceUPSAOffsetEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xFF);
	fTraceUPSAOffsetEntry->SetIncrement(1);
	fTraceUPSAOffsetEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSAOffsetEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceUPSAOffsetEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceUPSAOffsetEntry));
	fTraceUPSAOffsetEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

//	entryGC->SetLH(psaLayout);
	fTraceUPSALengthEntry = new TNGMrbLabelEntry(fTraceUPSAFrame, "Length",
																kDGFInstrTraceUPSALengthEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceUPSALengthEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSALengthEntry,traceLayout);
//	fTraceUPSALengthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSALengthEntry->SetFormat(TGNumberEntry::kNESInteger);
	fTraceUPSALengthEntry->SetIntNumber(0);
//	fTraceUPSALengthEntry->SetRange(0, 0xFF);
	fTraceUPSALengthEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xFF);
	fTraceUPSALengthEntry->SetIncrement(1);
	fTraceUPSALengthEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSALengthEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceUPSALengthEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceUPSALengthEntry));
	fTraceUPSALengthEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

//	entryGC->SetLH(psaLayout);
	fTraceUPSATFAEnergyCutoffEntry = new TNGMrbLabelEntry(fTraceUPSAFrame, "TFA cutoff",
																kDGFInstrTraceUPSATFAEnergyCutoffEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fTraceUPSATFAEnergyCutoffEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSATFAEnergyCutoffEntry, traceLayout);
//	fTraceUPSATFAEnergyCutoffEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSATFAEnergyCutoffEntry->SetFormat(TGNumberEntry::kNESInteger);
	fTraceUPSATFAEnergyCutoffEntry->SetIntNumber(0);
//	fTraceUPSATFAEnergyCutoffEntry->SetRange(0, 0xFF);
	fTraceUPSATFAEnergyCutoffEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xFF);
	fTraceUPSATFAEnergyCutoffEntry->SetIncrement(1);
	fTraceUPSATFAEnergyCutoffEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSATFAEnergyCutoffEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fTraceUPSATFAEnergyCutoffEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrTraceUPSATFAEnergyCutoffEntry));
	fTraceUPSATFAEnergyCutoffEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

// right: cfd
	TGLayoutHints * cfdLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, cfdLayout);
	HEAP(cfdLayout);
	fCFDFrame = new TGGroupFrame(fRightFrame, "CFD Settings", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fCFDFrame);
	fRightFrame->AddFrame(fCFDFrame, cfdLayout);

	TGGroupFrame * fCFDDataFrame = new TGGroupFrame(fCFDFrame, "-", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fCFDDataFrame);
	fCFDFrame->AddFrame(fCFDDataFrame,cfdLayout);
	TGLayoutHints * cfrLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(cfrLayout);
	HEAP(cfrLayout);

	fCFDRegEntry = new TNGMrbLabelEntry(fCFDDataFrame, "CFDREG",
																kDGFInstrCFDRegEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fCFDRegEntry);
	fCFDDataFrame->AddFrame(fCFDRegEntry, cfdLayout);
//	fCFDRegEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 0, 16);
	fCFDRegEntry->SetFormat(TGNumberEntry::kNESHex);
	fCFDRegEntry->SetHexNumber(0);
//	fCFDRegEntry->SetRange(0, 0x1 << 16);
	fCFDRegEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0x1<<16);
	fCFDRegEntry->ShowToolTip(kTRUE, kTRUE);
	fCFDRegEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fCFDRegEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrCFDRegEntry));
	fCFDRegEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fCFDOnOffButton = new TNGMrbRadioButtonList(fCFDDataFrame, "CFD enable", &fInstrCFDOnOff,
													kDGFInstrCFDOnOffButton, stdProfile,1,0, 
													kTabWidth, kLEHeight);
	HEAP(fCFDOnOffButton);
	fCFDOnOffButton->SetState(DGFInstrumentPanel::kDGFInstrCFDOff);
	((TNGMrbButtonFrame *) fCFDOnOffButton)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "RadioButtonPressed(Int_t, Int_t)");
	fCFDDataFrame->AddFrame(fCFDOnOffButton, cfdLayout);

	fCFDDelayBeforeLEEntry = new TNGMrbLabelEntry(fCFDDataFrame, "Delay before LE [ns]",
																kDGFInstrCFDDelayBeforeLEEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																2 * kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fCFDDelayBeforeLEEntry);
	fCFDDataFrame->AddFrame(fCFDDelayBeforeLEEntry, cfdLayout);
//	fCFDDelayBeforeLEEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fCFDDelayBeforeLEEntry->SetFormat(TGNumberEntry::kNESInteger);
	fCFDDelayBeforeLEEntry->SetIntNumber(0);
//	fCFDDelayBeforeLEEntry->SetRange(2 * 25, 17 * 25);
	fCFDDelayBeforeLEEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,2*25,17*25);
	fCFDDelayBeforeLEEntry->SetIncrement(25);
	fCFDDelayBeforeLEEntry->ShowToolTip(kTRUE, kTRUE);
	fCFDDelayBeforeLEEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fCFDDelayBeforeLEEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrCFDDelayBeforeLEEntry));
	fCFDDelayBeforeLEEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fCFDDelayBipolarEntry = new TNGMrbLabelEntry(fCFDDataFrame, "Delay bipolar signal [ns]",
																kDGFInstrCFDDelayBipolarEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																2 * kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fCFDDelayBipolarEntry);
	fCFDDataFrame->AddFrame(fCFDDelayBipolarEntry, cfdLayout);
//	fCFDDelayBipolarEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fCFDDelayBipolarEntry->SetFormat(TGNumberEntry::kNESInteger);
	fCFDDelayBipolarEntry->SetIntNumber(0);
//	fCFDDelayBipolarEntry->SetRange(2 * 25, 17 * 25);
	fCFDDelayBipolarEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,2*25,17*25);
	fCFDDelayBipolarEntry->SetIncrement(25);
	fCFDDelayBipolarEntry->ShowToolTip(kTRUE, kTRUE);
	fCFDDelayBipolarEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fCFDDelayBipolarEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrCFDDelayBipolarEntry));
	fCFDDelayBipolarEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fCFDWalkEntry = new TNGMrbLabelEntry(fCFDDataFrame, "Walk",
																kDGFInstrCFDWalkEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																2 * kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryIsNumber|
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryHasBeginEndButtons);
	HEAP(fCFDWalkEntry);
	fCFDDataFrame->AddFrame(fCFDWalkEntry, cfdLayout);
//	fCFDWalkEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fCFDWalkEntry->SetFormat(TGNumberEntry::kNESInteger);
	fCFDWalkEntry->SetIntNumber(0);
//	fCFDWalkEntry->SetRange(0, 31 * 8);
	fCFDWalkEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,31*8);
	fCFDWalkEntry->SetIncrement(8);
	fCFDWalkEntry->ShowToolTip(kTRUE, kTRUE);
	fCFDWalkEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fCFDWalkEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrCFDWalkEntry));
	fCFDWalkEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	fCFDFractionButton = new TNGMrbRadioButtonList(fCFDDataFrame, "Fraction", &fInstrCFDFraction,
													kDGFInstrCFDFractionButton,stdProfile,1, 0, 
													kTabWidth, kLEHeight);
	HEAP(fCFDFractionButton);
	fCFDFractionButton->SetState(DGFInstrumentPanel::kDGFInstrCFDFract00);
	((TNGMrbButtonFrame *) fCFDFractionButton)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "RadioButtonPressed(Int_t, Int_t)");
	fCFDDataFrame->AddFrame(fCFDFractionButton, cfdLayout);

// place an unvisible label to pad group frame vertically
	dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 12, 0);
//	frameGC->SetLH(dmyLayout);
	fDummyLabel = new TGLabel(fCFDFrame, "", stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->GC(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->Font(), kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->BG());
	HEAP(fDummyLabel);
	fCFDFrame->AddFrame(fDummyLabel, dmyLayout);

// left: csra
	TGLayoutHints * csraLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, csraLayout);
	HEAP(csraLayout);
	fStatRegFrame = new TGGroupFrame(fLeftFrame, "Registers", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fStatRegFrame);
	fLeftFrame->AddFrame(fStatRegFrame, csraLayout);

	fStatRegEntryFrame = new TGGroupFrame(fStatRegFrame, "-", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fStatRegEntryFrame);
	fStatRegFrame->AddFrame(fStatRegEntryFrame, csraLayout);

	TGLayoutHints * micsrLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(micsrLayout);
	HEAP(micsrLayout);
	fModICSREditButton = new TMrbNamedX(kDGFInstrStatRegModICSREditButton, "Edit", "EditBits(Int_t)");
	fModICSREditButton->AssignObject(this);
	fStatRegModICSREntry = new TNGMrbLabelEntry(fStatRegEntryFrame, "SwitchBus Register",
																kDGFInstrStatRegModICSREntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fStatRegModICSREntry);
	fStatRegEntryFrame->AddFrame(fStatRegModICSREntry, csraLayout);
//	fStatRegModICSREntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 6, 16);
	fStatRegModICSREntry->SetFormat(TGNumberEntry::kNESHex);
	fStatRegModICSREntry->SetHexNumber(0);
//	fStatRegModICSREntry->SetRange(0, 0x2400);
	fStatRegModICSREntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0x2400);
	fStatRegModICSREntry->SetIncrement(0x2400);
	fStatRegModICSREntry->ShowToolTip(kTRUE, kTRUE);
	fStatRegModICSREntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fStatRegModICSREntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrStatRegModICSREntry));
	fStatRegModICSREntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	TGLayoutHints * ccsraLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(ccsraLayout);
 	HEAP(ccsraLayout);
	fChanCSRAEditButton = new TMrbNamedX(kDGFInstrStatRegChanCSRAEditButton, "Edit", "EditBits(Int_t)");
	fChanCSRAEditButton->AssignObject(this);
 	fStatRegChanCSRAEntry = new TNGMrbLabelEntry(fStatRegEntryFrame, "Channel CSRA",
																kDGFInstrStatRegChanCSRAEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryIsNumber,
																fChanCSRAEditButton);
	HEAP(fStatRegChanCSRAEntry);
	fStatRegEntryFrame->AddFrame(fStatRegChanCSRAEntry, csraLayout);
//	fStatRegChanCSRAEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 6, 16, kTRUE);

	fStatRegChanCSRAEntry->SetFormat(TGNumberEntry::kNESHex);
	fStatRegChanCSRAEntry->SetHexNumber(0);
//	fStatRegChanCSRAEntry->SetRange(0, 0xffff);
	fStatRegChanCSRAEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xffff);
	fStatRegChanCSRAEntry->SetHexNumber(0);
	fStatRegChanCSRAEntry->ShowToolTip(kTRUE, kTRUE);
	fStatRegChanCSRAEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fStatRegChanCSRAEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrStatRegChanCSRAEntry));
	fStatRegChanCSRAEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");
	fStatRegChanCSRAEntry->Connect("LabelButtonPressed(Int_t, Int_t)", this->ClassName(), this, "EditBits(Int_t, Int_t)");

	TGLayoutHints * coincLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(coincLayout);
	HEAP(coincLayout);
	fCoincPatternEditButton = new TMrbNamedX(kDGFInstrStatCoincPatternEditButton, "Edit", "EditBits(Int_t)");
	fCoincPatternEditButton->AssignObject(this);
	fStatCoincPatternEntry = new TNGMrbLabelEntry(fStatRegEntryFrame, "Coinc Pattern",
																kDGFInstrStatCoincPatternEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryIsNumber,
																fCoincPatternEditButton);
	HEAP(fStatCoincPatternEntry);
	fStatRegEntryFrame->AddFrame(fStatCoincPatternEntry,csraLayout);
//	fStatCoincPatternEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 6, 16,kTRUE);
	fStatCoincPatternEntry->SetFormat(TGNumberEntry::kNESHex);
	fStatCoincPatternEntry->SetHexNumber(0);
//	fStatCoincPatternEntry->SetRange(0, 0xf);
	fStatCoincPatternEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,0xf);
	fStatCoincPatternEntry->ShowToolTip(kTRUE, kTRUE);
	fStatCoincPatternEntry->AddToFocusList(&fFocusList);
	fStatCoincPatternEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");
	fStatCoincPatternEntry->Connect("LabelButtonPressed(Int_t, Int_t)", this->ClassName(), this, "EditBits(Int_t, Int_t)");
	((TGTextEntry*)fStatCoincPatternEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrStatCoincPatternEntry));


	TGLayoutHints * runtaskLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(runtaskLayout);
	HEAP(runtaskLayout);
	fRunTaskEditButton = new TMrbNamedX(kDGFInstrStatRunTaskEditButton, "Edit", "EditBits(Int_t)");
	fRunTaskEditButton->AssignObject(this);
	fStatRunTaskEntry = new TNGMrbLabelEntry(fStatRegEntryFrame, "RUNTASK",
																kDGFInstrStatRunTaskEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryIsNumber,
																fRunTaskEditButton);
	HEAP(fStatRunTaskEntry);
	fStatRegEntryFrame->AddFrame(fStatRunTaskEntry, csraLayout);
//	fStatRunTaskEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 5, 16, kTRUE);
	fStatRunTaskEntry->SetFormat(TGNumberEntry::kNESHex);
	fStatRunTaskEntry->SetHexNumber((Int_t)0x100);
//	fStatRunTaskEntry->SetRange(0x100, 0x301);
	fStatRunTaskEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0x100,0x301);
	fStatRunTaskEntry->ShowToolTip(kTRUE, kTRUE);
	fStatRunTaskEntry->AddToFocusList(&fFocusList);
	fStatRunTaskEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");
	fStatRunTaskEntry->Connect("LabelButtonPressed(Int_t, Int_t)", this->ClassName(), this, "EditBits(Int_t, Int_t)");
	((TGTextEntry*)fStatRunTaskEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrStatRunTaskEntry));


// right: mca
	TGLayoutHints * mcaLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, mcaLayout);
	HEAP(mcaLayout);
	fMCAFrame = new TGGroupFrame(fRightFrame, "MCA Settings", kHorizontalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fMCAFrame);
	fRightFrame->AddFrame(fMCAFrame, mcaLayout);

	TGLayoutHints * mcaeLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, mcaeLayout);
	HEAP(mcaeLayout);
	fMCAEnergyFrame = new TGGroupFrame(fMCAFrame, "Energy", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fMCAEnergyFrame);
	fMCAFrame->AddFrame(fMCAEnergyFrame, mcaeLayout);

	TGLayoutHints * mcabLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	gDGFControlData->SetLH(groupGC, frameGC, mcabLayout);
	HEAP(mcabLayout);
	fMCABaselineFrame = new TGGroupFrame(fMCAFrame, "Baseline", kVerticalFrame,  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->GC(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->Font(),  stdProfile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());
	HEAP(fMCABaselineFrame);
	fMCAFrame->AddFrame(fMCABaselineFrame, mcabLayout);

// right/left: mca energy
	TGLayoutHints * mcaeeLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(mcaeeLayout);
	HEAP(mcaeeLayout);
	fMCAEnergyEntry = new TNGMrbLabelEntry(fMCAEnergyFrame, "Energy",
																kDGFInstrMCAEnergyEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fMCAEnergyEntry);
	fMCAEnergyFrame->AddFrame(fMCAEnergyEntry, mcabLayout);
//	fMCAEnergyEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMCAEnergyEntry->SetFormat(TGNumberEntry::kNESInteger);
	fMCAEnergyEntry->SetIntNumber(0);
//	fMCAEnergyEntry->SetRange(0, 8192);
	fMCAEnergyEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,8192);
	fMCAEnergyEntry->SetIncrement(100);
	fMCAEnergyEntry->ShowToolTip(kTRUE, kTRUE);
	fMCAEnergyEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fMCAEnergyEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrMCAEnergyEntry));
	fMCAEnergyEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	TGLayoutHints * mcaebLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(mcaebLayout);
	HEAP(mcaebLayout);
	fMCAEnergyBinsEntry = new TNGMrbLabelEntry(fMCAEnergyFrame, "Binning 2^",
																kDGFInstrMCAEnergyBinsEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fMCAEnergyBinsEntry);
	fMCAEnergyFrame->AddFrame(fMCAEnergyBinsEntry, mcabLayout);
//	fMCAEnergyBinsEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMCAEnergyBinsEntry->SetFormat(TGNumberEntry::kNESInteger);
	fMCAEnergyBinsEntry->SetIntNumber(0);
//	fMCAEnergyBinsEntry->SetRange(-10, -1);
	fMCAEnergyBinsEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,-10,-1);
	fMCAEnergyBinsEntry->ShowToolTip(kTRUE, kTRUE);
	fMCAEnergyBinsEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fMCAEnergyBinsEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrMCAEnergyBinsEntry));
	fMCAEnergyBinsEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

// place an unvisible label to pad group frame vertically
	dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 35, 0);
//	frameGC->SetLH(dmyLayout);
	HEAP(dmyLayout);
	fDummyLabel = new TGLabel(fMCAEnergyFrame, "", stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->GC(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->Font(), kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->BG());
	HEAP(fDummyLabel);
	fMCAEnergyFrame->AddFrame(fDummyLabel, dmyLayout);

// right/right: mca baseline
	TGLayoutHints * mcafLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
//	frameGC->SetLH(mcafLayout);
	HEAP(mcafLayout);
	TGLayoutHints * mcabdLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(mcabdLayout);
	HEAP(mcabdLayout);
	fMCABaselineDCEntry = new TNGMrbLabelEntry(fMCABaselineFrame, "DC",
																kDGFInstrMCABaselineDCEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fMCABaselineDCEntry);
	fMCABaselineFrame->AddFrame(fMCABaselineDCEntry, mcafLayout);
//	fMCABaselineDCEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMCABaselineDCEntry->SetFormat(TGNumberEntry::kNESInteger);
	fMCABaselineDCEntry->SetIntNumber(0);
//	fMCABaselineDCEntry->SetRange(0, 1024);
	fMCABaselineDCEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,1024);
	fMCABaselineDCEntry->ShowToolTip(kTRUE, kTRUE);
	fMCABaselineDCEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fMCABaselineDCEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrMCABaselineDCEntry));
	fMCABaselineDCEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

	TGLayoutHints * mcabbLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
//	entryGC->SetLH(mcabbLayout);
	HEAP(mcabbLayout);
	fMCABaselineBinsEntry = new TNGMrbLabelEntry(fMCABaselineFrame, "Binning 2^",
																kDGFInstrMCABaselineBinsEntry,stdProfile,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																TNGMrbLabelEntry::kGMrbEntryHasUpDownButtons|
																TNGMrbLabelEntry::kGMrbEntryIsNumber);
	HEAP(fMCABaselineBinsEntry);
	fMCABaselineFrame->AddFrame(fMCABaselineBinsEntry, mcafLayout);
//	fMCABaselineBinsEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMCABaselineBinsEntry->SetFormat(TGNumberEntry::kNESInteger);
	fMCABaselineBinsEntry->SetIntNumber(0);
//	fMCABaselineBinsEntry->SetRange(0, 16);
	fMCABaselineBinsEntry->SetLimits(TGNumberEntry::kNELLimitMinMax,0,16);
	fMCABaselineBinsEntry->ShowToolTip(kTRUE, kTRUE);
	fMCABaselineBinsEntry->AddToFocusList(&fFocusList);
	((TGTextEntry*)fMCABaselineBinsEntry->GetNumberEntryField())->Connect("TabPressed()", this->ClassName(), this,Form("MoveFocus(= %d)", kDGFInstrMCABaselineBinsEntry));
	fMCABaselineBinsEntry->Connect("EntryChanged(Int_t)", this->ClassName(), this, "EntryChanged(Int_t)");

// place an unvisible label to pad group frame vertically
//	frameGC->SetLH(dmyLayout);
	fDummyLabel = new TGLabel(fMCABaselineFrame, "", stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->GC(), stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->Font(), kChildFrame, stdProfile->GetGC(TNGMrbGContext::kGMrbGCLabel)->BG());
	HEAP(fDummyLabel);
	fMCABaselineFrame->AddFrame(fDummyLabel,dmyLayout);

//	buttons
	fModuleButtonFrame = new TNGMrbTextButtonGroup(this, "Actions", &fInstrModuleActions, -1,stdProfile,1, 0);
	HEAP(fModuleButtonFrame);
	this->AddFrame(fModuleButtonFrame, scbLayout);
 	((TNGMrbButtonFrame *) fModuleButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

// initialize data fields
	this->InitializeValues(kTRUE);

	dgfFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);
	this->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());
	TabFrame->ChangeBackground(stdProfile->GetGC(TNGMrbGContext::kGMrbGCFrame)->BG());



	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFInstrumentPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////


	gDGFControlData->AddToUpdateList(gDGFControlData->GetSelectedModule());

	switch (Selection) {
		case kDGFInstrButtonUpdateFPGAs:
			gDGFControlData->UpdateParamsAndFPGAs();
			break;
		case kDGFInstrButtonShowParams:
			this->ShowModuleSettings();
			break;
		case kDGFInstrDACGainEntry:														//wo wird das aufgerufen?
			this->UpdateValue(kDGFInstrDACGainEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
		
			break;	
		case kDGFInstrDACVVEntry:
			this->UpdateValue(kDGFInstrDACVVEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
		
			break;	
		case kDGFInstrDACOffsetEntry:
			this->UpdateValue(kDGFInstrDACOffsetEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
		
			break;	
		case kDGFInstrDACVoltEntry:
			this->UpdateValue(kDGFInstrDACVoltEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
		
			break;	
		case kDGFInstrCFDRegEntry:
			this->UpdateValue(kDGFInstrCFDRegEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
		
			break;	
		case kDGFInstrCFDDelayBeforeLEEntry:
			this->UpdateValue(kDGFInstrCFDDelayBeforeLEEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
		
			break;	
		case kDGFInstrCFDDelayBipolarEntry:
			this->UpdateValue(kDGFInstrCFDDelayBipolarEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
		
			break;	
		case kDGFInstrCFDWalkEntry:
			this->UpdateValue(kDGFInstrCFDWalkEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
		
			break;
	}
}
	
void DGFInstrumentPanel::SelectModule(Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::SelectModule
// Purpose:        Slot method: select module
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	gDGFControlData->SetSelectedModuleIndex(Selection);
	this->InitializeValues(kFALSE);
}


void DGFInstrumentPanel::EntryChanged(Int_t FrameId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::EntryChanged
// Purpose:        Slot method: update after entry changed
// Arguments:      Int_t FrameId     -- frame id

// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->UpdateValue(FrameId,	gDGFControlData->GetSelectedModuleIndex(),
									gDGFControlData->GetSelectedChannelIndex());

}

void DGFInstrumentPanel::RadioButtonPressed(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::RadioButtonPressed
// Purpose:        Signal catcher for radio buttons
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:     
// Description:    Will be called on radio button events.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (FrameId) {
		case kDGFInstrSelectChannel:
			gDGFControlData->SetSelectedChannelIndex(Selection);
			this->InitializeValues(kFALSE);
			break;
		case kDGFInstrCFDOnOffButton:
			this->InitializeCFD(Selection, -1);
			break;
		case kDGFInstrCFDFractionButton:
			this->InitializeCFD(-1, Selection);
			break;
	}
}

void DGFInstrumentPanel::EditBits(Int_t ButtonId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::EditBits
// Purpose:        Slot method: call bit editor
// Arguments:      Int_t ButtonId    -- button id
// Results:        
// Exceptions:     
// Description:    Called if action button pressed for TGMrbLabelEntry
// Keywords:       
//////////////////////////////////////////////////////////////////////////////



	switch(ButtonId) {
		case kDGFInstrStatRegChanCSRAEntry:
			new DGFEditChanCSRAPanel(fClient->GetRoot(), fStatRegChanCSRAEntry,
											DGFEditChanCSRAPanel::kFrameWidth, DGFEditChanCSRAPanel::kFrameHeight);
			this->UpdateValue(kDGFInstrStatRegChanCSRAEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
			break;
		case kDGFInstrStatRegUserPsaCSREntry:
			new DGFEditUserPsaCSRPanel(fClient->GetRoot(), fStatRegUserPsaCSREntry,
											DGFEditUserPsaCSRPanel::kFrameWidth, DGFEditUserPsaCSRPanel::kFrameHeight);
			this->UpdateValue(kDGFInstrStatRegUserPsaCSREntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
			break;
		case kDGFInstrStatCoincPatternEntry:
			new DGFEditCoincPatternPanel(fClient->GetRoot(), fStatCoincPatternEntry,
											DGFEditCoincPatternPanel::kFrameWidth, DGFEditCoincPatternPanel::kFrameHeight);
			this->UpdateValue(kDGFInstrStatCoincPatternEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
			break;
		case kDGFInstrStatRunTaskEntry:
			new DGFEditRunTaskPanel(fClient->GetRoot(), fStatRunTaskEntry,
											DGFEditRunTaskPanel::kFrameWidth, DGFEditRunTaskPanel::kFrameHeight);
			this->UpdateValue(kDGFInstrStatRunTaskEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
			break;
	}
}

Bool_t DGFInstrumentPanel::InitializeValues(Bool_t ReadFromDSP) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::InitializeValues
// Purpose:        Setup values in instrument panel
// Arguments:      Bool_t ReadFromDSP   -- read params from DSP
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Fills entry fields in instr panel.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * dgfModule;
	TMrbDGF * dgf;
	Int_t chn;
	Int_t intVal;
	Bool_t ok;

	if (gDGFControlData->IsOffline()) return(kTRUE);

	ok = kTRUE;
	if (ReadFromDSP) {
		TGMrbProgressBar * pgb = new TGMrbProgressBar(fClient->GetRoot(), this, "Reading DSP ...", 400, "blue", NULL, kTRUE);
		pgb->SetRange(0, gDGFControlData->GetNofModules());
		dgfModule = gDGFControlData->FirstModule();
		while (dgfModule) {
			if (gDGFControlData->ModuleInUse(dgfModule)) {
				if (!this->ReadDSP(dgfModule, -1)) ok = kFALSE;
				pgb->Increment(1, dgfModule->GetName());
				gSystem->ProcessEvents();
			}
			dgfModule = gDGFControlData->NextModule(dgfModule);
		}

//		pgb->DeleteWindow();
		delete pgb;

	}
	if (!ok) {
		gMrbLog->Err()	<< "Can't read param memory from DSP (try to re-connect)" << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeValues");
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Can't read param memory from DSP (try to re-connect)", kMBIconStop);
		return(kFALSE);
	}
	
	dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	chn = gDGFControlData->GetSelectedChannel();

// EnergyPeakTimeEntry:
	fEnergyPeakTimeEntry->SetNumber(dgf->GetPeakTime(chn));
// EnergyGapTimeEntry:
	fEnergyGapTimeEntry->SetNumber(dgf->GetGapTime(chn));
// EnergyAveragingEntry:;
	Short_t log2bw = (Short_t) dgf->GetParValue(chn, "LOG2BWEIGHT");
	intVal = (Int_t) log2bw;
	fEnergyAveragingEntry->SetNumber(intVal);
// EnergyTauEntry:
	fEnergyTauEntry->SetNumber(dgf->GetTau(chn));
// TriggerPeakTimeEntry:
	fTriggerPeakTimeEntry->SetNumber(dgf->GetFastPeakTime(chn));
// TriggerGapTimeEntry:
	fTriggerGapTimeEntry->SetNumber(dgf->GetFastGapTime(chn));
// TriggerThresholdEntry:
	fTriggerThresholdEntry->SetNumber(dgf->GetThreshold(chn));
// TraceLengthEntry:
	fTraceLengthEntry->SetNumber(dgf->GetTraceLength(chn));
// TraceDelayEntry:
	fTraceDelayEntry->SetNumber(dgf->GetDelay(chn));
// TraceXPSALengthEntry:
	fTraceXPSALengthEntry->SetNumber(dgf->GetPSALength(chn));
// TraceXPsaOffsetEntry:
	fTraceXPSAOffsetEntry->SetNumber(dgf->GetPSAOffset(chn));
// StatRegModICSREntry:
	dgf->SetSwitchBusDefault(gDGFControlData->fIndivSwitchBusTerm, "DGFControl", gDGFControlData->Dgfrc()->Env());
	fStatRegModICSREntry->SetNumber((Int_t) dgf->GetSwitchBus());
// StatRegChanCSRAEntry:
	fStatRegChanCSRAEntry->SetNumber((Int_t) dgf->GetChanCSRA(chn));
// StatRegUserPsaCSREntry:
	fStatRegUserPsaCSREntry->SetNumber((Int_t) dgf->GetUserPsaCSR(chn));
// StatRegRunTaskEntry:
	fStatRunTaskEntry->SetNumber(dgf->GetParValue("RUNTASK"));
// StatCoincPatternEntry:
	fStatCoincPatternEntry->SetNumber((Int_t) dgf->GetCoincPattern());
// DACGainEntry:
	fDACGainEntry->SetNumber(dgf->GetParValue(chn, "GAINDAC"));
// DACVVEntry:
	fDACVVEntry->SetNumber(dgf->GetGain(chn));
// DACOffsetEntry:
	fDACOffsetEntry->SetNumber(dgf->GetParValue(chn, "TRACKDAC"));
// DACVoltEntry:
	fDACVoltEntry->SetNumber(dgf->GetOffset(chn));
// CFDFractionEntry:
	this->InitializeCFD(-1, -1);
// TraceUPSA<XXX>Entry:
	this->InitializeUPSA();
// MCAEnergyEntry:
	fMCAEnergyEntry->SetNumber(dgf->GetParValue(chn, "ENERGYLOW"));
// MCAEnergyBinsEntry:
	Short_t log2e = (Short_t) dgf->GetParValue(chn, "LOG2EBIN");
	intVal = (Int_t) log2e;
	fMCAEnergyBinsEntry->SetNumber(intVal);
// MCABaselineDCEntry:

// MCABaselineBinsEntry:

	if (ReadFromDSP) dgf->SetParamsIncoreChanged(kFALSE);
	
	return(kTRUE);
}

Bool_t DGFInstrumentPanel::ReadDSP(DGFModule * Module, Int_t ChannelId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::ReadDSP
// Purpose:        Read param data from DSP
// Arguments:      DGFModule * Module -- module
//                 Int_t ChannelId    -- channel id
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Reads param data from DSP.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Int_t chn;
	Bool_t sts;

	dgf = Module->GetAddr();
	if (ChannelId == -1) {
		cout	<< setblue
				<< "DGFInstrumentPanel::ReadDSP(): Reading DSP for module "
				<< dgf->GetName()
				<< setblack << endl;
		sts = dgf->ReadParamMemory(kTRUE, kTRUE);
	} else {
		chn = gDGFControlData->GetChannel(ChannelId);
		cout	<< setblue
				<< "DGFInstrumentPanel::ReadDSP(): Reading DSP for module "
				<< dgf->GetName() << " (Channel " << chn << ")"
				<< setblack << endl;
		sts = dgf->ReadParamMemory(chn);
	}
	return(sts);
}

Bool_t DGFInstrumentPanel::WriteDSP(DGFModule * Module, Int_t ChannelId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::WriteDSP
// Purpose:        Update DSP
// Arguments:      DGFModule * Module     -- module
//                 Int_t ChannelId        -- channel id
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calculates corrections for given module & channel
//                 and updates DSP.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Int_t chn;
	Bool_t sts;

	dgf = Module->GetAddr();
	if (ChannelId == -1) {
		cout	<< setblue
			<< "DGFInstrumentPanel::WriteDSP(): Updating DSP for module "
			<< dgf->GetName()
			<< setblack << endl;
		Bool_t synchWait = ((gDGFControlData->fStatus & DGFControlData::kDGFSimulStartStop) != 0);
		dgf->SetSynchWait(synchWait, kTRUE);
		Bool_t inSynch = ((gDGFControlData->fStatus & DGFControlData::kDGFSyncClocks) != 0);
		dgf->SetInSynch(inSynch, kTRUE);
		sts = dgf->WriteParamMemory();
	} else {
		chn = gDGFControlData->GetChannel(ChannelId);
		cout	<< setblue
			<< "DGFInstrumentPanel::WriteDSP(): Updating DSP for module "
			<< dgf->GetName() << " (Channel " << chn << ")"
			<< setblack << endl;
		sts = dgf->WriteParamMemory(chn);
	}
	return(sts);
}

Bool_t DGFInstrumentPanel::ShowModuleSettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::ShowModuleSettings
// Purpose:        Show module settings
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calls $EDITOR to show current module settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString editor = gSystem->Getenv("EDITOR");
	DGFModule * dgfModule = gDGFControlData->GetSelectedModule();
	TString tmpFile = "/tmp/DGFControl.";
	tmpFile += dgfModule->GetName();
	TMrbDGF * dgf = dgfModule->GetAddr();
	if (dgf == NULL) return(kTRUE);

	tmpFile += ".par";
	dgf->WriteParamsToFile(tmpFile.Data());

	TString cmd;
	if (editor.CompareTo("nedit") == 0) cmd = "nedit -read ";
	else								cmd = "xterm -geom 100x35 +sb -e view ";
	cmd += tmpFile;
	gSystem->Exec(cmd.Data());
	return(kTRUE);
}	

Bool_t DGFInstrumentPanel::UpdateValue(Int_t EntryId, Int_t ModuleId, Int_t ChannelId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::UpdateValue
// Purpose:        Pass entry data to dgf module
// Arguments:      Int_t EntryId      -- entry id
//                 Int_t ModuleId     -- module id
//                 Int_t ChannelId    -- channel id
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Update a dgf value whenever entry data change.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	Int_t chn;
	Int_t intVal;
	Double_t dblVal;
	
	if (gDGFControlData->IsOffline()) return(kTRUE);

	dgf = gDGFControlData->GetModule(ModuleId)->GetAddr();
	chn = gDGFControlData->GetChannel(ChannelId);

//	Bool_t updateLocalEnv = kFALSE;

	switch (EntryId) {
		case kDGFInstrEnergyPeakTimeEntry:
			dblVal = fEnergyPeakTimeEntry->GetNumber();
//			if (fEnergyPeakTimeEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetPeakTime(chn, dblVal);
			fEnergyPeakTimeEntry->SetNumber(dgf->GetPeakTime(chn));
			fEnergyGapTimeEntry->SetNumber(dgf->GetGapTime(chn));
			break;
		case kDGFInstrEnergyGapTimeEntry:
			dblVal = fEnergyGapTimeEntry->GetNumber();
//			if (fEnergyGapTimeEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetGapTime(chn, dblVal);
			fEnergyGapTimeEntry->SetNumber(dgf->GetGapTime(chn));
			fEnergyPeakTimeEntry->SetNumber(dgf->GetPeakTime(chn));
			break;
		case kDGFInstrEnergyAveragingEntry:
			intVal = fEnergyAveragingEntry->GetIntNumber();
			dgf->SetParValue(chn, "LOG2BWEIGHT", intVal & 0xFFFF);
			break;
		case kDGFInstrEnergyTauEntry:
			dblVal = fEnergyTauEntry->GetNumber();
//			if (fEnergyTauEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetTau(chn, dblVal);
			break;
		case kDGFInstrTriggerPeakTimeEntry:
			dblVal = fTriggerPeakTimeEntry->GetNumber();
//			if (fTriggerPeakTimeEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetFastPeakTime(chn, dblVal);
			fTriggerPeakTimeEntry->SetNumber(dgf->GetFastPeakTime(chn));
			fTriggerGapTimeEntry->SetNumber(dgf->GetFastGapTime(chn));
			break;
		case kDGFInstrTriggerGapTimeEntry:
			dblVal = fTriggerGapTimeEntry->GetNumber();
//			if (fTriggerGapTimeEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetFastGapTime(chn, dblVal);
			fTriggerGapTimeEntry->SetNumber(dgf->GetFastGapTime(chn));
			fTriggerPeakTimeEntry->SetNumber(dgf->GetFastPeakTime(chn));
			break;
		case kDGFInstrTriggerThresholdEntry:
			intVal = fTriggerThresholdEntry->GetIntNumber();
			dgf->SetThreshold(chn, intVal);
			break;
		case kDGFInstrTraceXPSALengthEntry:
			dblVal = fTraceXPSALengthEntry->GetNumber();
			dgf->SetPSALength(chn, dblVal);
			break;
		case kDGFInstrTraceXPSAOffsetEntry:
			dblVal = fTraceXPSAOffsetEntry->GetNumber();
			dgf->SetPSAOffset(chn, dblVal);
			break;
		case kDGFInstrTraceLengthEntry:
			dblVal = fTraceLengthEntry->GetNumber();
			dgf->SetTraceLength(chn, dblVal);
			intVal = dgf->GetParValue(chn, "TRACELENGTH");
//			gDGFControlData->UpdateLocalEnv("DGFControl.Module", dgf->GetName(), "TraceLength", intVal);
//			updateLocalEnv = kTRUE;
			break;
		case kDGFInstrTraceDelayEntry:
			dblVal = fTraceDelayEntry->GetNumber();
			dgf->SetDelay(chn, dblVal);
			break;
		case kDGFInstrTraceUPSABaselineEntry:
		case kDGFInstrTraceUPSAEnergyCutoffEntry:
		case kDGFInstrTraceUPSATriggerThreshEntry:
		case kDGFInstrTraceUPSAT90ThreshEntry:
		case kDGFInstrTraceUPSALengthEntry:
		case kDGFInstrTraceUPSAOffsetEntry:
		case kDGFInstrTraceUPSATFAEnergyCutoffEntry:
			this->UpdateUPSA(dgf, chn);
			break;
		case kDGFInstrCFDRegEntry:
		case kDGFInstrCFDDelayBeforeLEEntry:
		case kDGFInstrCFDDelayBipolarEntry:
		case kDGFInstrCFDWalkEntry:
			this->UpdateCFD(dgf, chn);
			break;
		case kDGFInstrStatRegChanCSRAEntry:
			intVal = fStatRegChanCSRAEntry->GetIntNumber();
			dgf->SetChanCSRA(chn, (UInt_t) intVal, TMrbDGF::kBitSet, kTRUE);
			break;
		case kDGFInstrStatRegUserPsaCSREntry:
			intVal = fStatRegUserPsaCSREntry->GetIntNumber();
			dgf->SetUserPsaCSR(chn, (UInt_t) intVal, TMrbDGF::kBitSet, kTRUE);
			break;
		case kDGFInstrStatRunTaskEntry:
			intVal = fStatRunTaskEntry->GetIntNumber();
			dgf->SetParValue("RUNTASK", intVal);
//			gDGFControlData->UpdateLocalEnv("DGFControl.Module", dgf->GetName(), "RunTask", intVal);
//			updateLocalEnv = kTRUE;
			break;
		case kDGFInstrStatCoincPatternEntry:
			intVal = fStatCoincPatternEntry->GetIntNumber();
			dgf->SetCoincPattern((UInt_t) intVal);
			break;
		case kDGFInstrDACGainEntry:
			intVal = fDACGainEntry->GetIntNumber();
			dgf->SetParValue(chn, "GAINDAC", intVal);
			fDACVVEntry->SetNumber(dgf->GetGain(chn));
			break;
		case kDGFInstrDACVVEntry:
			dblVal = fDACVVEntry->GetNumber();
			dgf->SetGain(chn, dblVal);
			fDACVVEntry->SetNumber(dgf->GetGain(chn));
			fDACGainEntry->SetNumber(dgf->GetParValue(chn, "GAINDAC"));
			break;
		case kDGFInstrDACOffsetEntry:
			intVal = fDACOffsetEntry->GetIntNumber();
			dgf->SetParValue(chn, "TRACKDAC", intVal);
			fDACVoltEntry->SetNumber(dgf->GetOffset(chn));
			break;
		case kDGFInstrDACVoltEntry:
			dblVal = fDACVoltEntry->GetNumber();
			dgf->SetOffset(chn, dblVal);
			fDACOffsetEntry->SetNumber(dgf->GetParValue(chn, "TRACKDAC"));
			break;
		case kDGFInstrMCAEnergyEntry:
			intVal = fMCAEnergyEntry->GetIntNumber();
			dgf->SetParValue(chn, "ENERGYLOW", intVal);
			break;
		case kDGFInstrMCAEnergyBinsEntry:
			intVal = fMCAEnergyBinsEntry->GetIntNumber();
			dgf->SetParValue(chn, "LOG2EBIN", intVal & 0xFFFF);
			break;
		case kDGFInstrMCABaselineDCEntry:
			break;
		case kDGFInstrMCABaselineBinsEntry:
			break;
	}
//	if (updateLocalEnv) gDGFControlData->WriteLocalEnv();
	return(kTRUE);
}

Bool_t DGFInstrumentPanel::InitializeCFD(Int_t OnOffFlag, Int_t FractionFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::InitializeCFD
// Purpose:        Setup values for hardware cfd
// Arguments:      Int_t OnOffFlag     -- on or off
//                 Int_t FractionFlag  -- fraction
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Fills entry fields for hardware cfd
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gDGFControlData->IsOffline()) return(kTRUE);

	TMrbDGF * dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	Int_t chn = gDGFControlData->GetSelectedChannel();

	Int_t cfdVal = dgf->GetCFD(chn);
	Int_t cfdDel = cfdVal & 0xF;
	cfdDel = 16 - (cfdDel - 1);
	fCFDDelayBeforeLEEntry->SetNumber(cfdDel * 25);
	cfdDel = (cfdVal >> 4) & 0xF;
	cfdDel = 16 - (cfdDel - 1);
	fCFDDelayBipolarEntry->SetNumber(cfdDel * 25);

	if (OnOffFlag == DGFInstrumentPanel::kDGFInstrCFDOn)		cfdVal |= BIT(13);
	else if (OnOffFlag == DGFInstrumentPanel::kDGFInstrCFDOff)	cfdVal &= ~BIT(13);

	if (cfdVal & BIT(13))	fCFDOnOffButton->SetState(DGFInstrumentPanel::kDGFInstrCFDOn, kButtonDown, kFALSE);
	else			fCFDOnOffButton->SetState(DGFInstrumentPanel::kDGFInstrCFDOff, kButtonDown, kFALSE);

	Int_t cfd = (cfdVal >> 8) & 0x1F;
	cfd = ~cfd & 0x1F;
	fCFDWalkEntry->SetNumber(cfd * 8);

	if (FractionFlag != -1) {
		cfdVal &= ~(BIT(14)|BIT(15));
		if (fCFDFractionButton->GetActive() == DGFInstrumentPanel::kDGFInstrCFDFract01) cfdVal |= BIT(14);
		else if (fCFDFractionButton->GetActive() == DGFInstrumentPanel::kDGFInstrCFDFract10) cfdVal |= BIT(15);
	}

	if (cfdVal & BIT(14))		fCFDFractionButton->SetState(DGFInstrumentPanel::kDGFInstrCFDFract01, kButtonDown, kFALSE);
	else if (cfdVal & BIT(15))	fCFDFractionButton->SetState(DGFInstrumentPanel::kDGFInstrCFDFract10, kButtonDown, kFALSE);
	else						fCFDFractionButton->SetState(DGFInstrumentPanel::kDGFInstrCFDFract00, kButtonDown, kFALSE);

	fCFDRegEntry->SetNumber(cfdVal);
	dgf->SetCFD(chn, cfdVal);

	return(kTRUE);
}

Bool_t DGFInstrumentPanel::UpdateCFD(TMrbDGF * Module, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::UpdateCFD
// Purpose:        Update hardware cfd
// Arguments:      TMrbDGF * Module     -- module
//                 Int_t Channel        -- channel
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Updates hardware cfd
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gDGFControlData->IsOffline()) return(kTRUE);

	Int_t cfdVal = 0;

	Int_t cfd = fCFDDelayBeforeLEEntry->GetIntNumber();
	cfd = (cfd + 24) / 25;
	cfd = 16 - (cfd - 1);
	cfdVal += (cfd & 0xF);

	cfd = fCFDDelayBipolarEntry->GetIntNumber();
	cfd = (cfd + 24) / 25;
	cfd = 16 - (cfd - 1);
	cfdVal += ((cfd << 4) & 0xF0);

	if (fCFDOnOffButton->GetActive() == DGFInstrumentPanel::kDGFInstrCFDOn) cfdVal |= BIT(13);
	else																	cfdVal &= ~BIT(13);

	cfd = fCFDWalkEntry->GetIntNumber();
	cfd = (cfd + 7) / 8;
	cfd = ~cfd;
	cfd &= 0x1F;
	cfd <<= 8;
	cfdVal |= cfd;

	Module->SetCFD(Channel, cfdVal);
	fCFDRegEntry->SetNumber(Module->GetCFD(Channel));

	return(kTRUE);
}

Bool_t DGFInstrumentPanel::InitializeUPSA() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::InitializeUPSA
// Purpose:        Setup values for user psa
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Fills entry fields for user psa
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gDGFControlData->IsOffline()) return(kTRUE);

	TMrbDGF * dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	Int_t chn = gDGFControlData->GetSelectedChannel();

 	// baseline 0-3: 4/4/4
	fTraceUPSABaselineEntry->SetNumber(dgf->GetUserPsaData4(TMrbDGFData::kPsaBaseline03, chn));

	// cutoff 0/1 2/3: 8/8 8/8
	fTraceUPSAEnergyCutoffEntry->SetNumber(dgf->GetUserPsaData8(TMrbDGFData::kPsaCutOff01, chn)); 

	// trigger thresh 0/1 2/3: 8/8 8/8
	fTraceUPSATriggerThreshEntry->SetNumber(dgf->GetUserPsaData8(TMrbDGFData::kPsaT0Thresh01, chn));

	// t90 thresh 0-3: 4/4/4/4
	fTraceUPSAT90ThreshEntry->SetNumber(dgf->GetUserPsaData4(TMrbDGFData::kPsaT90Thresh03, chn));

	// psa length 0/1 2/3: 8/8 8/8
	fTraceUPSALengthEntry->SetNumber(dgf->GetUserPsaData8(TMrbDGFData::kPsaPSALength01, chn));

	// psa offset 0/1 2/3: 8/8 8/8
	fTraceUPSAOffsetEntry->SetNumber(dgf->GetUserPsaData8(TMrbDGFData::kPsaPSAOffset01, chn));

	// tfa cutoff 0/1 2/3: 8/8 8/8
	fTraceUPSATFAEnergyCutoffEntry->SetNumber(dgf->GetUserPsaData8(TMrbDGFData::kPsaTFACutOff01, chn));

	return(kTRUE);
}

Bool_t DGFInstrumentPanel::UpdateUPSA(TMrbDGF * Module, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::UpdateUPSA
// Purpose:        Update user psa
// Arguments:      TMrbDGF * Module     -- module
//                 Int_t Channel        -- channel
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Updates user psa
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gDGFControlData->IsOffline()) return(kTRUE);

	TMrbDGF * dgf = gDGFControlData->GetSelectedModule()->GetAddr();
	Int_t chn = gDGFControlData->GetSelectedChannel();

	Int_t intVal = fTraceUPSABaselineEntry->GetIntNumber();
	dgf->SetUserPsaData4(TMrbDGFData::kPsaBaseline03, chn, intVal);			// baseline 0-3: 4/4/4/4 

	intVal = fTraceUPSAEnergyCutoffEntry->GetIntNumber();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaCutOff01, chn, intVal);			// cutoff 0/1 2/3: 8/8 8/8

	intVal = fTraceUPSATriggerThreshEntry->GetIntNumber();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaT0Thresh01, chn, intVal);			// trigger thresh 0/1 2/3: 8/8 8/8

	intVal = fTraceUPSAT90ThreshEntry->GetIntNumber();
	dgf->SetUserPsaData4(TMrbDGFData::kPsaT90Thresh03, chn, intVal);		// t90 thresh 0-3: 4/4/4/4

	intVal = fTraceUPSALengthEntry->GetIntNumber();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaPSALength01, chn, intVal);		// psa length 0/1 2/3: 8/8 8/8

	intVal = fTraceUPSAOffsetEntry->GetIntNumber();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaPSAOffset01, chn, intVal);		// psa offset 0/1 2/3: 8/8 8/8

	intVal = fTraceUPSATFAEnergyCutoffEntry->GetIntNumber();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaTFACutOff01, chn, intVal);		// tfa cutoff 0/1 2/3: 8/8 8/8

	return(kTRUE);
}

void DGFInstrumentPanel::MoveFocus(Int_t EntryId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::MoveFocus
// Purpose:        Move focus to next entry field
// Arguments:      Int_t EntryId     -- entry id
// Results:        --
// Exceptions:     
// Description:    Moves focus to next entry field in ring buffer.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////
	TGTextEntry * entry = NULL;

	switch (EntryId) {
		case kDGFInstrEnergyPeakTimeEntry:
			entry = fEnergyPeakTimeEntry->GetEntry();
			break;
		case kDGFInstrEnergyGapTimeEntry:
			entry = fEnergyGapTimeEntry->GetEntry();
			break;
		case kDGFInstrEnergyAveragingEntry:
			entry = fEnergyAveragingEntry->GetEntry();
			break;
		case kDGFInstrEnergyTauEntry:
			entry = fEnergyTauEntry->GetEntry();
			break;
		case kDGFInstrTriggerPeakTimeEntry:
			entry = fTriggerPeakTimeEntry->GetEntry();
			break;
		case kDGFInstrTriggerGapTimeEntry:
			entry = fTriggerGapTimeEntry->GetEntry();
			break;
		case kDGFInstrTriggerThresholdEntry:
			entry = fTriggerThresholdEntry->GetEntry();
			break;
		case kDGFInstrTraceLengthEntry:
			entry = fTraceLengthEntry->GetEntry();
			break;
		case kDGFInstrTraceDelayEntry:
			entry = fTraceDelayEntry->GetEntry();
			break;
		case kDGFInstrTraceXPSALengthEntry:
			entry = fTraceXPSALengthEntry->GetEntry();
			break;
		case kDGFInstrTraceXPSAOffsetEntry:
			entry = fTraceXPSAOffsetEntry->GetEntry();
			break;
		case kDGFInstrCFDRegEntry:
			entry = fCFDRegEntry->GetEntry();
			break;
		case kDGFInstrCFDDelayBeforeLEEntry:
			entry = fCFDDelayBeforeLEEntry->GetEntry();
			break;
		case kDGFInstrCFDDelayBipolarEntry:
			entry = fCFDDelayBipolarEntry->GetEntry();
			break;
		case kDGFInstrCFDWalkEntry:
			entry = fCFDWalkEntry->GetEntry();
			break;
		case kDGFInstrStatRegModICSREntry:
			entry = fStatRegModICSREntry->GetEntry();
			break;
		case kDGFInstrStatRegChanCSRAEntry:
			entry = fStatRegChanCSRAEntry->GetEntry();
			break;
		case kDGFInstrStatRegUserPsaCSREntry:
			entry = fStatRegUserPsaCSREntry->GetEntry();
			break;
		case kDGFInstrStatCoincPatternEntry:
			entry = fStatCoincPatternEntry->GetEntry();
			break;
		case kDGFInstrDACGainEntry:
			entry = fDACGainEntry->GetEntry();
			break;
		case kDGFInstrDACOffsetEntry:
			entry = fDACOffsetEntry->GetEntry();
			break;
		case kDGFInstrDACVVEntry:
			entry = fDACVVEntry->GetEntry();
			break;
		case kDGFInstrDACVoltEntry:
			entry = fDACVoltEntry->GetEntry();
			break;
		case kDGFInstrMCAEnergyEntry:
			entry = fMCAEnergyEntry->GetEntry();
			break;
		case kDGFInstrMCAEnergyBinsEntry:
			entry = fMCAEnergyBinsEntry->GetEntry();
			break;
		case kDGFInstrMCABaselineDCEntry:
			entry = fMCABaselineDCEntry->GetEntry();
			break;
		case kDGFInstrMCABaselineBinsEntry:
			entry = fMCABaselineBinsEntry->GetEntry();
			break;
		case kDGFInstrStatRunTaskEntry:
			entry = fStatRunTaskEntry->GetEntry();
			break;
		case kDGFInstrTraceUPSABaselineEntry:
			entry = fTraceUPSABaselineEntry->GetEntry();
			break;
		case kDGFInstrTraceUPSAEnergyCutoffEntry:
			entry = fTraceUPSAEnergyCutoffEntry->GetEntry();
			break;
		case kDGFInstrTraceUPSATriggerThreshEntry:
			entry = fTraceUPSATriggerThreshEntry->GetEntry();
			break;
		case kDGFInstrTraceUPSAT90ThreshEntry:
			entry = fTraceUPSAT90ThreshEntry->GetEntry();
			break;
		case kDGFInstrTraceUPSALengthEntry:
			entry = fTraceUPSALengthEntry->GetEntry();
			break;
		case kDGFInstrTraceUPSAOffsetEntry:
			entry = fTraceUPSAOffsetEntry->GetEntry();
			break;
		case kDGFInstrTraceUPSATFAEnergyCutoffEntry:
			entry = fTraceUPSATFAEnergyCutoffEntry->GetEntry();
			break;
	}
	if (entry) fFocusList.FocusForward(entry);
	return;
}


