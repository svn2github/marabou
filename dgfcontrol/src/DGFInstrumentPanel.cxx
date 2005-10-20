//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Instrument Panel
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFInstrumentPanel.cxx,v 1.26 2005-10-20 13:09:52 Rudolf.Lutter Exp $       
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

extern DGFControlData * gDGFControlData;
extern TMrbLogger * gMrbLog;

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

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * comboGC;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	Clear focus list
	fFocusList.Clear();

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(), 
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

//	Initialize several lists
	fInstrModuleActions.SetName("Instrument module actions");
	fInstrModuleActions.AddNamedX(kDGFInstrumentModuleButtons);

	fInstrCFDOnOff.SetName("CFD on/off");
	fInstrCFDOnOff.AddNamedX(kDGFInstrCFDOnOff);
	fInstrCFDOnOff.SetPatternMode();

	fInstrCFDFraction.SetName("CFD fraction");
	fInstrCFDFraction.AddNamedX(kDGFInstrCFDFraction);
	fInstrCFDFraction.SetPatternMode();

	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	this->ChangeBackground(gDGFControlData->fColorGreen);

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dgfFrameLayout);
	HEAP(dgfFrameLayout);

//	module / channel selection
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, selectLayout);
	HEAP(selectLayout);
	fSelectFrame = new TGGroupFrame(this, "DGF Selection", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, groupGC->LH());

	TGLayoutHints * smfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 10, 1, 10, 1);
	frameGC->SetLH(smfLayout);
	HEAP(smfLayout);
	TGLayoutHints * smlLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	labelGC->SetLH(smlLayout);
	HEAP(smlLayout);
	TGLayoutHints * smcLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);
	comboGC->SetLH(smcLayout);
	HEAP(smcLayout);

	Bool_t clearList = kTRUE;
	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		gDGFControlData->CopyKeyList(&fLofModuleKeys, cl, gDGFControlData->GetPatInUse(cl), clearList);
		clearList = kFALSE;
	}
	gDGFControlData->SetSelectedModule(fLofModuleKeys.First()->GetName());		// dgf to start with
	gDGFControlData->SetSelectedChannel(0); 									// channel 0

	fSelectModule = new TGMrbLabelCombo(fSelectFrame,  "Module",
											&fLofModuleKeys,
											DGFInstrumentPanel::kDGFInstrSelectModule, 2,
											kTabWidth, kLEHeight,
											kEntryWidth,
											frameGC, labelGC, comboGC, buttonGC, kTRUE);
	HEAP(fSelectModule);
	fSelectFrame->AddFrame(fSelectModule, frameGC->LH());
	fSelectModule->GetComboBox()->Select(gDGFControlData->GetSelectedModuleIndex());
	fSelectModule->Associate(this); 	// get informed if module selection changes

	TGLayoutHints * scfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 80, 1, 10, 1);
	frameGC->SetLH(scfLayout);
	HEAP(scfLayout);
	TGLayoutHints * sclLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	labelGC->SetLH(sclLayout);
	HEAP(sclLayout);
	TGLayoutHints * scbLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	buttonGC->SetLH(scbLayout);
	HEAP(scbLayout);
	fSelectChannel = new TGMrbRadioButtonList(fSelectFrame, "Channel", &fLofChannels, kDGFInstrSelectChannel, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, comboGC);
	HEAP(fSelectChannel);
	fSelectChannel->SetState(gDGFControlData->GetSelectedChannelIndex());
	fSelectChannel->Associate(this);	// get informed if channel number changes
	fSelectFrame->AddFrame(fSelectChannel, frameGC->LH());

// 2 vertical frames, left and right
	TGLayoutHints * hLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	frameGC->SetLH(hLayout);
	HEAP(hLayout);
	fInstrFrame = new TGHorizontalFrame(this, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
	HEAP(fInstrFrame);
	this->AddFrame(fInstrFrame, frameGC->LH());

	TGLayoutHints * vLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	frameGC->SetLH(vLayout);
	HEAP(vLayout);
	fLeftFrame = new TGVerticalFrame(fInstrFrame, kVFrameWidth, kVFrameHeight, kChildFrame, frameGC->BG());
	HEAP(fLeftFrame);
	fInstrFrame->AddFrame(fLeftFrame, frameGC->LH());

	fRightFrame = new TGVerticalFrame(fInstrFrame, kVFrameWidth, kVFrameHeight, kChildFrame, frameGC->BG());
	HEAP(fRightFrame);
	fInstrFrame->AddFrame(fRightFrame, frameGC->LH());

// left: filter
	TGLayoutHints * filterLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, filterLayout);
	HEAP(filterLayout);
	fFilterFrame = new TGGroupFrame(fLeftFrame, "Filter Settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fFilterFrame);
	fLeftFrame->AddFrame(fFilterFrame, groupGC->LH());

// left/left: energy
	TGLayoutHints * efLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, efLayout);
	HEAP(efLayout);
	fEnergyFilterFrame = new TGGroupFrame(fFilterFrame, "Energy", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fEnergyFilterFrame);
	fFilterFrame->AddFrame(fEnergyFilterFrame, groupGC->LH());

	TGLayoutHints * eeLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	entryGC->SetLH(eeLayout);
	HEAP(eeLayout);
	fEnergyPeakTimeEntry = new TGMrbLabelEntry(fEnergyFilterFrame, "Peaking [us]",
																200, kDGFInstrEnergyPeakTimeEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyPeakTimeEntry);
	fEnergyFilterFrame->AddFrame(fEnergyPeakTimeEntry, frameGC->LH());
	fEnergyPeakTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fEnergyPeakTimeEntry->SetText(0);
	fEnergyPeakTimeEntry->SetRange(0, 100000);
	fEnergyPeakTimeEntry->SetIncrement(gDGFControlData->fDeltaT);
	fEnergyPeakTimeEntry->ShowToolTip(kTRUE, kTRUE);
	fEnergyPeakTimeEntry->AddToFocusList(&fFocusList);
	fEnergyPeakTimeEntry->Associate(this);

	fEnergyGapTimeEntry = new TGMrbLabelEntry(fEnergyFilterFrame, "Gap [us]",
																200, kDGFInstrEnergyGapTimeEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyGapTimeEntry);
	fEnergyFilterFrame->AddFrame(fEnergyGapTimeEntry, frameGC->LH());
	fEnergyGapTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fEnergyGapTimeEntry->SetText(0);
	fEnergyGapTimeEntry->SetRange(0, 100000);
	fEnergyGapTimeEntry->SetIncrement(gDGFControlData->fDeltaT);
	fEnergyGapTimeEntry->ShowToolTip(kTRUE, kTRUE);
	fEnergyGapTimeEntry->AddToFocusList(&fFocusList);
	fEnergyGapTimeEntry->Associate(this);

	fEnergyAveragingEntry = new TGMrbLabelEntry(fEnergyFilterFrame, "Averaging 2^",
																200, kDGFInstrEnergyAveragingEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyAveragingEntry);
	fEnergyFilterFrame->AddFrame(fEnergyAveragingEntry, frameGC->LH());
	fEnergyAveragingEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyAveragingEntry->SetText(2);
	fEnergyAveragingEntry->SetRange(-10, -1);
	fEnergyAveragingEntry->ShowToolTip(kTRUE, kTRUE);
	fEnergyAveragingEntry->AddToFocusList(&fFocusList);
	fEnergyAveragingEntry->Associate(this);

	fEnergyTauEntry = new TGMrbLabelEntry(fEnergyFilterFrame, "Tau value",
																200, kDGFInstrEnergyTauEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyTauEntry);
	fEnergyFilterFrame->AddFrame(fEnergyTauEntry, frameGC->LH());
	fEnergyTauEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fEnergyTauEntry->SetText(0.0);
	fEnergyTauEntry->SetRange(0, 99);
	fEnergyTauEntry->SetIncrement(0.2);
	fEnergyTauEntry->ShowToolTip(kTRUE, kTRUE);
	fEnergyTauEntry->AddToFocusList(&fFocusList);
	fEnergyTauEntry->Associate(this);

// left/right: trigger
	TGLayoutHints * tfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, tfLayout);
	HEAP(tfLayout);
	fTriggerFilterFrame = new TGGroupFrame(fFilterFrame, "Trigger", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTriggerFilterFrame);
	fFilterFrame->AddFrame(fTriggerFilterFrame, groupGC->LH());

	TGLayoutHints * teLayout = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	entryGC->SetLH(teLayout);
	HEAP(teLayout);
	fTriggerPeakTimeEntry = new TGMrbLabelEntry(fTriggerFilterFrame, "Peaking [us]",
																200, kDGFInstrTriggerPeakTimeEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTriggerPeakTimeEntry);
	fTriggerFilterFrame->AddFrame(fTriggerPeakTimeEntry, frameGC->LH());
	fTriggerPeakTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTriggerPeakTimeEntry->SetText(0.025);
	fTriggerPeakTimeEntry->SetRange(.025, .1);
	fTriggerPeakTimeEntry->SetIncrement(.025);
	fTriggerPeakTimeEntry->ShowToolTip(kTRUE, kTRUE);
	fTriggerPeakTimeEntry->AddToFocusList(&fFocusList);
	fTriggerPeakTimeEntry->Associate(this);

	fTriggerGapTimeEntry = new TGMrbLabelEntry(fTriggerFilterFrame, "Gap [us]",
																200, kDGFInstrTriggerGapTimeEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTriggerGapTimeEntry);
	fTriggerFilterFrame->AddFrame(fTriggerGapTimeEntry, frameGC->LH());
	fTriggerGapTimeEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTriggerGapTimeEntry->SetText(0);
	fTriggerGapTimeEntry->SetRange(0, .75);
	fTriggerGapTimeEntry->SetIncrement(.025);
	fTriggerGapTimeEntry->ShowToolTip(kTRUE, kTRUE);
	fTriggerGapTimeEntry->AddToFocusList(&fFocusList);
	fTriggerGapTimeEntry->Associate(this);

	fTriggerThresholdEntry = new TGMrbLabelEntry(fTriggerFilterFrame, "Threshold",
																200, kDGFInstrTriggerThresholdEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTriggerThresholdEntry);
	fTriggerFilterFrame->AddFrame(fTriggerThresholdEntry, frameGC->LH());
	fTriggerThresholdEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTriggerThresholdEntry->SetText(0);
	fTriggerThresholdEntry->SetRange(0, 1000);
	fTriggerThresholdEntry->ShowToolTip(kTRUE, kTRUE);
	fTriggerThresholdEntry->AddToFocusList(&fFocusList);
	fTriggerThresholdEntry->Associate(this);

// place an unvisible label to pad group frame vertically
	TGLayoutHints * dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 12, 0);
	frameGC->SetLH(dmyLayout);
	HEAP(dmyLayout);
	fDummyLabel = new TGLabel(fTriggerFilterFrame, "", frameGC->GC(), frameGC->Font(), kChildFrame, frameGC->BG());
	HEAP(fDummyLabel);
	fTriggerFilterFrame->AddFrame(fDummyLabel, frameGC->LH());

// right: DAC Settings
	TGLayoutHints * dacLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dacLayout);
	HEAP(dacLayout);
	fDACFrame = new TGGroupFrame(fRightFrame, "DAC Settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDACFrame);
	fRightFrame->AddFrame(fDACFrame, groupGC->LH());

// right/left: gain
	TGLayoutHints * dgLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, dgLayout);
	HEAP(dgLayout);
	fDACGainFrame = new TGGroupFrame(fDACFrame, "Gain", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDACGainFrame);
	fDACFrame->AddFrame(fDACGainFrame, groupGC->LH());

	fDACGainEntry = new TGMrbLabelEntry(fDACGainFrame, "DAC",
																200, kDGFInstrDACGainEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fDACGainEntry);
	fDACGainFrame->AddFrame(fDACGainEntry, frameGC->LH());
	fDACGainEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fDACGainEntry->SetText(0);
	fDACGainEntry->SetRange(0, 65535);
	fDACGainEntry->SetIncrement(1);
	fDACGainEntry->ShowToolTip(kTRUE, kTRUE);
	fDACGainEntry->AddToFocusList(&fFocusList);
	fDACGainEntry->Associate(this);

	fDACVVEntry = new TGMrbLabelEntry(fDACGainFrame, "V/V",
																200, kDGFInstrDACVVEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fDACVVEntry);
	fDACGainFrame->AddFrame(fDACVVEntry, frameGC->LH());
	fDACVVEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fDACVVEntry->SetText(0);
	fDACVVEntry->SetRange(0, 20);
	fDACVVEntry->SetIncrement(.1);
	fDACVVEntry->ShowToolTip(kTRUE, kTRUE);
	fDACVVEntry->AddToFocusList(&fFocusList);
	fDACVVEntry->Associate(this);

// place an unvisible label to pad group frame vertically
	dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 36, 0);
	frameGC->SetLH(dmyLayout);
	HEAP(dmyLayout);
	fDummyLabel = new TGLabel(fDACGainFrame, "", frameGC->GC(), frameGC->Font(), kChildFrame, frameGC->BG());
	HEAP(fDummyLabel);
	fDACGainFrame->AddFrame(fDummyLabel, frameGC->LH());

// right/right: offset
	TGLayoutHints * doLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, doLayout);
	HEAP(doLayout);
	fDACOffsetFrame = new TGGroupFrame(fDACFrame, "Offset", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDACOffsetFrame);
	fDACFrame->AddFrame(fDACOffsetFrame, groupGC->LH());

	fDACOffsetEntry = new TGMrbLabelEntry(fDACOffsetFrame, "DAC",
																200, kDGFInstrDACOffsetEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fDACOffsetEntry);
	fDACOffsetFrame->AddFrame(fDACOffsetEntry, frameGC->LH());
	fDACOffsetEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fDACOffsetEntry->SetText(0);
	fDACOffsetEntry->SetRange(0, 65535);
	fDACOffsetEntry->SetIncrement(1);
	fDACOffsetEntry->ShowToolTip(kTRUE, kTRUE);
	fDACOffsetEntry->AddToFocusList(&fFocusList);
	fDACOffsetEntry->Associate(this);

	fDACVoltEntry = new TGMrbLabelEntry(fDACOffsetFrame, "Volt",
																200, kDGFInstrDACVoltEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fDACVoltEntry);
	fDACOffsetFrame->AddFrame(fDACVoltEntry, frameGC->LH());
	fDACVoltEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fDACVoltEntry->SetText(0);
	fDACVoltEntry->SetRange(-3, 3);
	fDACVoltEntry->SetIncrement(.1);
	fDACVoltEntry->ShowToolTip(kTRUE, kTRUE);
	fDACVoltEntry->AddToFocusList(&fFocusList);
	fDACVoltEntry->Associate(this);

// place an unvisible label to pad group frame vertically
	frameGC->SetLH(dmyLayout);
	fDummyLabel = new TGLabel(fDACOffsetFrame, "", frameGC->GC(), frameGC->Font(), kChildFrame, frameGC->BG());
	HEAP(fDummyLabel);
	fDACOffsetFrame->AddFrame(fDummyLabel, frameGC->LH());

// left: trace
	TGLayoutHints * traceLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, traceLayout);
	HEAP(traceLayout);
	fTraceFrame = new TGGroupFrame(fLeftFrame, "Trace Settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTraceFrame);
	fLeftFrame->AddFrame(fTraceFrame, groupGC->LH());

	fTraceLeftFrame = new TGVerticalFrame(fTraceFrame, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
	HEAP(fTraceLeftFrame);
	fTraceFrame->AddFrame(fTraceLeftFrame, groupGC->LH());

	fTraceLengthDelayFrame = new TGGroupFrame(fTraceLeftFrame, "Trace", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTraceLengthDelayFrame);
	fTraceLeftFrame->AddFrame(fTraceLengthDelayFrame, groupGC->LH());
	TGLayoutHints * tlLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(tlLayout);
	HEAP(tlLayout);
	fTraceLengthEntry = new TGMrbLabelEntry(fTraceLengthDelayFrame, "Length [us]",
																200, kDGFInstrTraceLengthEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceLengthEntry);
	fTraceLengthDelayFrame->AddFrame(fTraceLengthEntry, frameGC->LH());
	fTraceLengthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTraceLengthEntry->SetText(0);
	fTraceLengthEntry->SetRange(0, 25.6);
	fTraceLengthEntry->SetIncrement(.5);
	fTraceLengthEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceLengthEntry->AddToFocusList(&fFocusList);
	fTraceLengthEntry->Associate(this);

	TGLayoutHints * tdLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(tdLayout);
	HEAP(tdLayout);
	fTraceDelayEntry = new TGMrbLabelEntry(fTraceLengthDelayFrame, "Delay [us]",
																200, kDGFInstrTraceDelayEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceDelayEntry);
	fTraceLengthDelayFrame->AddFrame(fTraceDelayEntry, frameGC->LH());
	fTraceDelayEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTraceDelayEntry->SetText(0);
	fTraceDelayEntry->SetRange(0, 100000);
	fTraceDelayEntry->SetIncrement(.1);
	fTraceDelayEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceDelayEntry->AddToFocusList(&fFocusList);
	fTraceDelayEntry->Associate(this);

	fTraceXPSAFrame = new TGGroupFrame(fTraceLeftFrame, "XIA PSA", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTraceXPSAFrame);
	fTraceLeftFrame->AddFrame(fTraceXPSAFrame, groupGC->LH());

	TGLayoutHints * psaLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(psaLayout);
	HEAP(psaLayout);
	fTraceXPSALengthEntry = new TGMrbLabelEntry(fTraceXPSAFrame, "Length [us]",
																200, kDGFInstrTraceXPSALengthEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceXPSALengthEntry);
	fTraceXPSAFrame->AddFrame(fTraceXPSALengthEntry, frameGC->LH());
	fTraceXPSALengthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTraceXPSALengthEntry->SetText(0);
	fTraceXPSALengthEntry->SetRange(0, 25);
	fTraceXPSALengthEntry->SetIncrement(.5);
	fTraceXPSALengthEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceXPSALengthEntry->AddToFocusList(&fFocusList);
	fTraceXPSALengthEntry->Associate(this);

	entryGC->SetLH(psaLayout);
	fTraceXPSAOffsetEntry = new TGMrbLabelEntry(fTraceXPSAFrame, "Offset [us]",
																200, kDGFInstrTraceXPSAOffsetEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceXPSAOffsetEntry);
	fTraceXPSAFrame->AddFrame(fTraceXPSAOffsetEntry, frameGC->LH());
	fTraceXPSAOffsetEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTraceXPSAOffsetEntry->SetText(0);
	fTraceXPSAOffsetEntry->SetRange(0, 25);
	fTraceXPSAOffsetEntry->SetIncrement(.1);
	fTraceXPSAOffsetEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceXPSAOffsetEntry->AddToFocusList(&fFocusList);
	fTraceXPSAOffsetEntry->Associate(this);

	fTraceRightFrame = new TGVerticalFrame(fTraceFrame, kTabWidth, kTabHeight, kChildFrame, frameGC->BG());
	HEAP(fTraceRightFrame);
	fTraceFrame->AddFrame(fTraceRightFrame, groupGC->LH());

	fTraceUPSAFrame = new TGGroupFrame(fTraceRightFrame, "User PSA", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTraceUPSAFrame);
	fTraceRightFrame->AddFrame(fTraceUPSAFrame, groupGC->LH());

	entryGC->SetLH(psaLayout);
	fTraceUPSABaselineEntry = new TGMrbLabelEntry(fTraceUPSAFrame, "Baseline",
																200, kDGFInstrTraceUPSABaselineEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceUPSABaselineEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSABaselineEntry, frameGC->LH());
	fTraceUPSABaselineEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSABaselineEntry->SetText(0);
	fTraceUPSABaselineEntry->SetRange(0, 0xF);
	fTraceUPSABaselineEntry->SetIncrement(1);
	fTraceUPSABaselineEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSABaselineEntry->AddToFocusList(&fFocusList);
	fTraceUPSABaselineEntry->Associate(this);

	entryGC->SetLH(psaLayout);
	fTraceUPSAEnergyCutoffEntry = new TGMrbLabelEntry(fTraceUPSAFrame, "Energy cutoff",
																200, kDGFInstrTraceUPSAEnergyCutoffEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceUPSAEnergyCutoffEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSAEnergyCutoffEntry, frameGC->LH());
	fTraceUPSAEnergyCutoffEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSAEnergyCutoffEntry->SetText(0);
	fTraceUPSAEnergyCutoffEntry->SetRange(0, 0xFF);
	fTraceUPSAEnergyCutoffEntry->SetIncrement(1);
	fTraceUPSAEnergyCutoffEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSAEnergyCutoffEntry->AddToFocusList(&fFocusList);
	fTraceUPSAEnergyCutoffEntry->Associate(this);

	entryGC->SetLH(psaLayout);
	fTraceUPSATriggerThreshEntry = new TGMrbLabelEntry(fTraceUPSAFrame, "Trigger threshold",
																200, kDGFInstrTraceUPSATriggerThreshEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceUPSATriggerThreshEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSATriggerThreshEntry, frameGC->LH());
	fTraceUPSATriggerThreshEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSATriggerThreshEntry->SetText(0);
	fTraceUPSATriggerThreshEntry->SetRange(0, 0xFF);
	fTraceUPSATriggerThreshEntry->SetIncrement(1);
	fTraceUPSATriggerThreshEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSATriggerThreshEntry->AddToFocusList(&fFocusList);
	fTraceUPSATriggerThreshEntry->Associate(this);

	entryGC->SetLH(psaLayout);
	fTraceUPSAT90ThreshEntry = new TGMrbLabelEntry(fTraceUPSAFrame, "T90 threshold",
																200, kDGFInstrTraceUPSAT90ThreshEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceUPSAT90ThreshEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSAT90ThreshEntry, frameGC->LH());
	fTraceUPSAT90ThreshEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSAT90ThreshEntry->SetText(0);
	fTraceUPSAT90ThreshEntry->SetRange(0, 0xF);
	fTraceUPSAT90ThreshEntry->SetIncrement(1);
	fTraceUPSAT90ThreshEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSAT90ThreshEntry->AddToFocusList(&fFocusList);
	fTraceUPSAT90ThreshEntry->Associate(this);

	entryGC->SetLH(psaLayout);
	fTraceUPSAOffsetEntry = new TGMrbLabelEntry(fTraceUPSAFrame, "Offset",
																200, kDGFInstrTraceUPSAOffsetEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceUPSAOffsetEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSAOffsetEntry, frameGC->LH());
	fTraceUPSAOffsetEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSAOffsetEntry->SetText(0);
	fTraceUPSAOffsetEntry->SetRange(0, 0xFF);
	fTraceUPSAOffsetEntry->SetIncrement(1);
	fTraceUPSAOffsetEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSAOffsetEntry->AddToFocusList(&fFocusList);
	fTraceUPSAOffsetEntry->Associate(this);

	entryGC->SetLH(psaLayout);
	fTraceUPSALengthEntry = new TGMrbLabelEntry(fTraceUPSAFrame, "Length",
																200, kDGFInstrTraceUPSALengthEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceUPSALengthEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSALengthEntry, frameGC->LH());
	fTraceUPSALengthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSALengthEntry->SetText(0);
	fTraceUPSALengthEntry->SetRange(0, 0xFF);
	fTraceUPSALengthEntry->SetIncrement(1);
	fTraceUPSALengthEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSALengthEntry->AddToFocusList(&fFocusList);
	fTraceUPSALengthEntry->Associate(this);

	entryGC->SetLH(psaLayout);
	fTraceUPSATFAEnergyCutoffEntry = new TGMrbLabelEntry(fTraceUPSAFrame, "TFA cutoff",
																200, kDGFInstrTraceUPSATFAEnergyCutoffEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTraceUPSATFAEnergyCutoffEntry);
	fTraceUPSAFrame->AddFrame(fTraceUPSATFAEnergyCutoffEntry, frameGC->LH());
	fTraceUPSATFAEnergyCutoffEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTraceUPSATFAEnergyCutoffEntry->SetText(0);
	fTraceUPSATFAEnergyCutoffEntry->SetRange(0, 0xFF);
	fTraceUPSATFAEnergyCutoffEntry->SetIncrement(1);
	fTraceUPSATFAEnergyCutoffEntry->ShowToolTip(kTRUE, kTRUE);
	fTraceUPSATFAEnergyCutoffEntry->AddToFocusList(&fFocusList);
	fTraceUPSATFAEnergyCutoffEntry->Associate(this);

	entryGC->SetLH(psaLayout);
	fUserPsaCSREditButton = new TMrbNamedX(kDGFInstrStatRegUserPsaCSREditButton, "Edit");
	fUserPsaCSREditButton->AssignObject(this);
	fStatRegUserPsaCSREntry = new TGMrbLabelEntry(fTraceUPSAFrame, "UserPSA CSR",
																200, kDGFInstrStatRegUserPsaCSREntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, NULL, kFALSE,
																fUserPsaCSREditButton, buttonGC);
	HEAP(fStatRegUserPsaCSREntry);
	fTraceUPSAFrame->AddFrame(fStatRegUserPsaCSREntry, frameGC->LH());
	fStatRegUserPsaCSREntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 4, ' ', 16);
	fStatRegUserPsaCSREntry->SetText(0);
	fStatRegUserPsaCSREntry->SetRange(0, 0xffff);
	fStatRegUserPsaCSREntry->ShowToolTip(kTRUE, kTRUE);
	fStatRegUserPsaCSREntry->AddToFocusList(&fFocusList);
	fStatRegUserPsaCSREntry->Associate(this);

// right: cfd
	TGLayoutHints * cfdLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, cfdLayout);
	HEAP(cfdLayout);
	fCFDFrame = new TGGroupFrame(fRightFrame, "CFD Settings", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCFDFrame);
	fRightFrame->AddFrame(fCFDFrame, groupGC->LH());

	TGGroupFrame * fCFDDataFrame = new TGGroupFrame(fCFDFrame, "-", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCFDDataFrame);
	fCFDFrame->AddFrame(fCFDDataFrame, groupGC->LH());
	TGLayoutHints * cfrLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(cfrLayout);
	HEAP(cfrLayout);

	fCFDRegEntry = new TGMrbLabelEntry(fCFDDataFrame, "CFDREG",
																200, kDGFInstrCFDRegEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC);
	HEAP(fCFDRegEntry);
	fCFDDataFrame->AddFrame(fCFDRegEntry, frameGC->LH());
	fCFDRegEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 0, ' ', 16);
	fCFDRegEntry->SetText(0);
	fCFDRegEntry->SetRange(0, 0x1 << 16);
	fCFDRegEntry->ShowToolTip(kTRUE, kTRUE);
	fCFDRegEntry->AddToFocusList(&fFocusList);
	fCFDRegEntry->Associate(this);

	fCFDOnOffButton = new TGMrbRadioButtonList(fCFDDataFrame, "CFD enable", &fInstrCFDOnOff, kDGFInstrCFDOnOffButton, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, comboGC);
	HEAP(fCFDOnOffButton);
	fCFDOnOffButton->SetState(DGFInstrumentPanel::kDGFInstrCFDOff);
	fCFDOnOffButton->Associate(this);
	fCFDDataFrame->AddFrame(fCFDOnOffButton, frameGC->LH());

	fCFDDelayBeforeLEEntry = new TGMrbLabelEntry(fCFDDataFrame, "Delay before LE [ns]",
																200, kDGFInstrCFDDelayBeforeLEEntry,
																kLEWidth,
																kLEHeight,
																2 * kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fCFDDelayBeforeLEEntry);
	fCFDDataFrame->AddFrame(fCFDDelayBeforeLEEntry, frameGC->LH());
	fCFDDelayBeforeLEEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fCFDDelayBeforeLEEntry->SetText(0);
	fCFDDelayBeforeLEEntry->SetRange(2 * 25, 17 * 25);
	fCFDDelayBeforeLEEntry->SetIncrement(25);
	fCFDDelayBeforeLEEntry->ShowToolTip(kTRUE, kTRUE);
	fCFDDelayBeforeLEEntry->AddToFocusList(&fFocusList);
	fCFDDelayBeforeLEEntry->Associate(this);

	fCFDDelayBipolarEntry = new TGMrbLabelEntry(fCFDDataFrame, "Delay bipolar signal [ns]",
																200, kDGFInstrCFDDelayBipolarEntry,
																kLEWidth,
																kLEHeight,
																2 * kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fCFDDelayBipolarEntry);
	fCFDDataFrame->AddFrame(fCFDDelayBipolarEntry, frameGC->LH());
	fCFDDelayBipolarEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fCFDDelayBipolarEntry->SetText(0);
	fCFDDelayBipolarEntry->SetRange(2 * 25, 17 * 25);
	fCFDDelayBipolarEntry->SetIncrement(25);
	fCFDDelayBipolarEntry->ShowToolTip(kTRUE, kTRUE);
	fCFDDelayBipolarEntry->AddToFocusList(&fFocusList);
	fCFDDelayBipolarEntry->Associate(this);

	fCFDWalkEntry = new TGMrbLabelEntry(fCFDDataFrame, "Walk",
																200, kDGFInstrCFDWalkEntry,
																kLEWidth,
																kLEHeight,
																2 * kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC, kTRUE);
	HEAP(fCFDWalkEntry);
	fCFDDataFrame->AddFrame(fCFDWalkEntry, frameGC->LH());
	fCFDWalkEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fCFDWalkEntry->SetText(0);
	fCFDWalkEntry->SetRange(0, 31 * 8);
	fCFDWalkEntry->SetIncrement(8);
	fCFDWalkEntry->ShowToolTip(kTRUE, kTRUE);
	fCFDWalkEntry->AddToFocusList(&fFocusList);
	fCFDWalkEntry->Associate(this);

	fCFDFractionButton = new TGMrbRadioButtonList(fCFDDataFrame, "Fraction", &fInstrCFDFraction, kDGFInstrCFDFractionButton, 1, 
													kTabWidth, kLEHeight,
													frameGC, labelGC, comboGC);
	HEAP(fCFDFractionButton);
	fCFDFractionButton->SetState(DGFInstrumentPanel::kDGFInstrCFDFract00);
	fCFDFractionButton->Associate(this);
	fCFDDataFrame->AddFrame(fCFDFractionButton, frameGC->LH());

// place an unvisible label to pad group frame vertically
	dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 12, 0);
	frameGC->SetLH(dmyLayout);
	fDummyLabel = new TGLabel(fCFDFrame, "", frameGC->GC(), frameGC->Font(), kChildFrame, frameGC->BG());
	HEAP(fDummyLabel);
	fCFDFrame->AddFrame(fDummyLabel, frameGC->LH());

// left: csra
	TGLayoutHints * csraLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, csraLayout);
	HEAP(csraLayout);
	fStatRegFrame = new TGGroupFrame(fLeftFrame, "Registers", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fStatRegFrame);
	fLeftFrame->AddFrame(fStatRegFrame, groupGC->LH());

	fStatRegEntryFrame = new TGGroupFrame(fStatRegFrame, "-", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fStatRegEntryFrame);
	fStatRegFrame->AddFrame(fStatRegEntryFrame, groupGC->LH());

	TGLayoutHints * micsrLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(micsrLayout);
	HEAP(micsrLayout);
	fModICSREditButton = new TMrbNamedX(kDGFInstrStatRegModICSREditButton, "Edit");
	fModICSREditButton->AssignObject(this);
	fStatRegModICSREntry = new TGMrbLabelEntry(fStatRegEntryFrame, "SwitchBus Register",
																200, kDGFInstrStatRegModICSREntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, NULL, kFALSE);
	HEAP(fStatRegModICSREntry);
	fStatRegEntryFrame->AddFrame(fStatRegModICSREntry, frameGC->LH());
	fStatRegModICSREntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 4, ' ', 16);
	fStatRegModICSREntry->SetText(0);
	fStatRegModICSREntry->SetRange(0, 0x1);
	fStatRegModICSREntry->ShowToolTip(kTRUE, kTRUE);
	fStatRegModICSREntry->AddToFocusList(&fFocusList);
	fStatRegModICSREntry->Associate(this);

	TGLayoutHints * ccsraLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(ccsraLayout);
	HEAP(ccsraLayout);
	fChanCSRAEditButton = new TMrbNamedX(kDGFInstrStatRegChanCSRAEditButton, "Edit");
	fChanCSRAEditButton->AssignObject(this);
	fStatRegChanCSRAEntry = new TGMrbLabelEntry(fStatRegEntryFrame, "Channel CSRA",
																200, kDGFInstrStatRegChanCSRAEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, NULL, kFALSE,
																fChanCSRAEditButton, buttonGC);
	HEAP(fStatRegChanCSRAEntry);
	fStatRegEntryFrame->AddFrame(fStatRegChanCSRAEntry, frameGC->LH());
	fStatRegChanCSRAEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 4, ' ', 16);
	fStatRegChanCSRAEntry->SetText(0);
	fStatRegChanCSRAEntry->SetRange(0, 0xffff);
	fStatRegChanCSRAEntry->ShowToolTip(kTRUE, kTRUE);
	fStatRegChanCSRAEntry->AddToFocusList(&fFocusList);
	fStatRegChanCSRAEntry->Associate(this);

	TGLayoutHints * coincLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(coincLayout);
	HEAP(coincLayout);
	fCoincPatternEditButton = new TMrbNamedX(kDGFInstrStatCoincPatternEditButton, "Edit");
	fCoincPatternEditButton->AssignObject(this);
	fStatCoincPatternEntry = new TGMrbLabelEntry(fStatRegEntryFrame, "Coinc Pattern",
																200, kDGFInstrStatCoincPatternEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, NULL, kFALSE,
																fCoincPatternEditButton, buttonGC);
	HEAP(fStatCoincPatternEntry);
	fStatRegEntryFrame->AddFrame(fStatCoincPatternEntry, frameGC->LH());
	fStatCoincPatternEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 4, ' ', 16);
	fStatCoincPatternEntry->SetText(0);
	fStatCoincPatternEntry->SetRange(0, 0xf);
	fStatCoincPatternEntry->ShowToolTip(kTRUE, kTRUE);
	fStatCoincPatternEntry->AddToFocusList(&fFocusList);
	fStatCoincPatternEntry->Associate(this);

	TGLayoutHints * runtaskLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(runtaskLayout);
	HEAP(runtaskLayout);
	fRunTaskEditButton = new TMrbNamedX(kDGFInstrStatRunTaskEditButton, "Edit");
	fRunTaskEditButton->AssignObject(this);
	fStatRunTaskEntry = new TGMrbLabelEntry(fStatRegEntryFrame, "RUNTASK",
																200, kDGFInstrStatRunTaskEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, NULL, kFALSE,
																fRunTaskEditButton, buttonGC);
	HEAP(fStatRunTaskEntry);
	fStatRegEntryFrame->AddFrame(fStatRunTaskEntry, frameGC->LH());
	fStatRunTaskEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 3, ' ', 16);
	fStatRunTaskEntry->SetText(0x100);
	fStatRunTaskEntry->SetRange(0x100, 0x301);
	fStatRunTaskEntry->ShowToolTip(kTRUE, kTRUE);
	fStatRunTaskEntry->AddToFocusList(&fFocusList);
	fStatRunTaskEntry->Associate(this);

// right: mca
	TGLayoutHints * mcaLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, mcaLayout);
	HEAP(mcaLayout);
	fMCAFrame = new TGGroupFrame(fRightFrame, "MCA Settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fMCAFrame);
	fRightFrame->AddFrame(fMCAFrame, groupGC->LH());

	TGLayoutHints * mcaeLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, mcaeLayout);
	HEAP(mcaeLayout);
	fMCAEnergyFrame = new TGGroupFrame(fMCAFrame, "Energy", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fMCAEnergyFrame);
	fMCAFrame->AddFrame(fMCAEnergyFrame, groupGC->LH());

	TGLayoutHints * mcabLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, mcabLayout);
	HEAP(mcabLayout);
	fMCABaselineFrame = new TGGroupFrame(fMCAFrame, "Baseline", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fMCABaselineFrame);
	fMCAFrame->AddFrame(fMCABaselineFrame, groupGC->LH());

// right/left: mca energy
	TGLayoutHints * mcaeeLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(mcaeeLayout);
	HEAP(mcaeeLayout);
	fMCAEnergyEntry = new TGMrbLabelEntry(fMCAEnergyFrame, "Energy",
																200, kDGFInstrMCAEnergyEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fMCAEnergyEntry);
	fMCAEnergyFrame->AddFrame(fMCAEnergyEntry, frameGC->LH());
	fMCAEnergyEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMCAEnergyEntry->SetText(0);
	fMCAEnergyEntry->SetRange(0, 8192);
	fMCAEnergyEntry->SetIncrement(100);
	fMCAEnergyEntry->ShowToolTip(kTRUE, kTRUE);
	fMCAEnergyEntry->AddToFocusList(&fFocusList);
	fMCAEnergyEntry->Associate(this);

	TGLayoutHints * mcaebLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(mcaebLayout);
	HEAP(mcaebLayout);
	fMCAEnergyBinsEntry = new TGMrbLabelEntry(fMCAEnergyFrame, "Binning 2^",
																200, kDGFInstrMCAEnergyBinsEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fMCAEnergyBinsEntry);
	fMCAEnergyFrame->AddFrame(fMCAEnergyBinsEntry, frameGC->LH());
	fMCAEnergyBinsEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMCAEnergyBinsEntry->SetText(0);
	fMCAEnergyBinsEntry->SetRange(-10, -1);
	fMCAEnergyBinsEntry->ShowToolTip(kTRUE, kTRUE);
	fMCAEnergyBinsEntry->AddToFocusList(&fFocusList);
	fMCAEnergyBinsEntry->Associate(this);

// place an unvisible label to pad group frame vertically
	dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 35, 0);
	frameGC->SetLH(dmyLayout);
	HEAP(dmyLayout);
	fDummyLabel = new TGLabel(fMCAEnergyFrame, "", frameGC->GC(), frameGC->Font(), kChildFrame, frameGC->BG());
	HEAP(fDummyLabel);
	fMCAEnergyFrame->AddFrame(fDummyLabel, frameGC->LH());

// right/right: mca baseline
	TGLayoutHints * mcafLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	frameGC->SetLH(mcafLayout);
	HEAP(mcafLayout);
	TGLayoutHints * mcabdLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(mcabdLayout);
	HEAP(mcabdLayout);
	fMCABaselineDCEntry = new TGMrbLabelEntry(fMCABaselineFrame, "DC",
																200, kDGFInstrMCABaselineDCEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fMCABaselineDCEntry);
	fMCABaselineFrame->AddFrame(fMCABaselineDCEntry, frameGC->LH());
	fMCABaselineDCEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMCABaselineDCEntry->SetText(0);
	fMCABaselineDCEntry->SetRange(0, 1024);
	fMCABaselineDCEntry->ShowToolTip(kTRUE, kTRUE);
	fMCABaselineDCEntry->AddToFocusList(&fFocusList);
	fMCABaselineDCEntry->Associate(this);

	TGLayoutHints * mcabbLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(mcabbLayout);
	HEAP(mcabbLayout);
	fMCABaselineBinsEntry = new TGMrbLabelEntry(fMCABaselineFrame, "Binning 2^",
																200, kDGFInstrMCABaselineBinsEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fMCABaselineBinsEntry);
	fMCABaselineFrame->AddFrame(fMCABaselineBinsEntry, frameGC->LH());
	fMCABaselineBinsEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fMCABaselineBinsEntry->SetText(0);
	fMCABaselineBinsEntry->SetRange(0, 16);
	fMCABaselineBinsEntry->ShowToolTip(kTRUE, kTRUE);
	fMCABaselineBinsEntry->AddToFocusList(&fFocusList);
	fMCABaselineBinsEntry->Associate(this);

// place an unvisible label to pad group frame vertically
	frameGC->SetLH(dmyLayout);
	fDummyLabel = new TGLabel(fMCABaselineFrame, "", frameGC->GC(), frameGC->Font(), kChildFrame, frameGC->BG());
	HEAP(fDummyLabel);
	fMCABaselineFrame->AddFrame(fDummyLabel, frameGC->LH());

//	buttons
	fModuleButtonFrame = new TGMrbTextButtonGroup(this, "Actions", &fInstrModuleActions, -1, 1, groupGC, buttonGC);
	HEAP(fModuleButtonFrame);
	this->AddFrame(fModuleButtonFrame, buttonGC->LH());
	fModuleButtonFrame->Associate(this);

// initialize data fields
	this->InitializeValues(kTRUE);

	dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 5, 1, 5, 1);
	HEAP(dgfFrameLayout);
	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

Bool_t DGFInstrumentPanel::ProcessMessage(Long_t MsgId, Long_t Param1, Long_t Param2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFInstrumentPanel::ProcessMessage
// Purpose:        Message handler for the instrument panel
// Arguments:      Long_t MsgId      -- message id
//                 Long_t ParamX     -- message parameter   
// Results:        
// Exceptions:     
// Description:    Handle messages sent to DGFInstrumentPanel.
//                 E.g. all menu button messages.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	gDGFControlData->AddToUpdateList(gDGFControlData->GetSelectedModule());

	switch (GET_MSG(MsgId)) {

		case kC_COMMAND:
			switch (GET_SUBMSG(MsgId)) {
				case kCM_BUTTON:
					switch (Param1) {
						case kDGFInstrStatRegChanCSRAEditButton:
                    		new DGFEditChanCSRAPanel(fClient->GetRoot(), fStatRegChanCSRAEntry->GetEntry(),
											DGFEditChanCSRAPanel::kFrameWidth, DGFEditChanCSRAPanel::kFrameHeight);
							this->UpdateValue(kDGFInstrStatRegChanCSRAEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
							break;
						case kDGFInstrStatRegUserPsaCSREditButton:
                    		new DGFEditUserPsaCSRPanel(fClient->GetRoot(), fStatRegUserPsaCSREntry->GetEntry(),
											DGFEditUserPsaCSRPanel::kFrameWidth, DGFEditUserPsaCSRPanel::kFrameHeight);
							this->UpdateValue(kDGFInstrStatRegUserPsaCSREntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
							break;
						case kDGFInstrStatCoincPatternEditButton:
                    		new DGFEditCoincPatternPanel(fClient->GetRoot(), fStatCoincPatternEntry->GetEntry(),
											DGFEditCoincPatternPanel::kFrameWidth, DGFEditCoincPatternPanel::kFrameHeight);
							this->UpdateValue(kDGFInstrStatCoincPatternEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
							break;
						case kDGFInstrStatRunTaskEditButton:
                    		new DGFEditRunTaskPanel(fClient->GetRoot(), fStatRunTaskEntry->GetEntry(),
											DGFEditRunTaskPanel::kFrameWidth, DGFEditRunTaskPanel::kFrameHeight);
							this->UpdateValue(kDGFInstrStatRunTaskEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
							break;
						case kDGFInstrButtonUpdateFPGAs:
                    		gDGFControlData->UpdateParamsAndFPGAs();
							break;
						case kDGFInstrButtonShowParams:
                    		this->ShowModuleSettings();
							break;
						case kDGFInstrDACGainEntry:
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
					break;
				case kCM_RADIOBUTTON:
					switch (Param1) {
						case kDGFInstrSelectChannel:
							gDGFControlData->SetSelectedChannelIndex(fSelectChannel->GetActive());
							this->InitializeValues(kFALSE);
							break;
						case kDGFInstrCFDOnOffButton:
							this->InitializeCFD((Int_t) fCFDOnOffButton->GetActive(), -1);
							break;
						case kDGFInstrCFDFractionButton:
							this->InitializeCFD(-1, (Int_t) fCFDFractionButton->GetActive());
							break;
					}
					break;
				case kCM_COMBOBOX:
					gDGFControlData->SetSelectedModuleIndex(Param2);
					this->InitializeValues(kFALSE);
					break;
			}
			break;

		case kC_TEXTENTRY:
			switch (GET_SUBMSG(MsgId)) {
				case kTE_ENTER:
					this->UpdateValue(Param1,	gDGFControlData->GetSelectedModuleIndex(),
												gDGFControlData->GetSelectedChannelIndex());

					break;
				case kTE_TAB:
					this->UpdateValue(Param1,	gDGFControlData->GetSelectedModuleIndex(),
												gDGFControlData->GetSelectedChannelIndex());

					this->MoveFocus(Param1);
					break;
			}
			break;
			
	}
	return(kTRUE);
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
		dgfModule = gDGFControlData->FirstModule();
		while (dgfModule) {
			if (gDGFControlData->ModuleInUse(dgfModule)) {
				if (!this->ReadDSP(dgfModule, -1)) ok = kFALSE;
			}
			dgfModule = gDGFControlData->NextModule(dgfModule);
		}
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
	fEnergyPeakTimeEntry->SetText(dgf->GetPeakTime(chn));
// EnergyGapTimeEntry:
	fEnergyGapTimeEntry->SetText(dgf->GetGapTime(chn));
// EnergyAveragingEntry:;
	Short_t log2bw = (Short_t) dgf->GetParValue(chn, "LOG2BWEIGHT");
	intVal = (Int_t) log2bw;
	fEnergyAveragingEntry->SetText(intVal);
// EnergyTauEntry:
	fEnergyTauEntry->SetText(dgf->GetTau(chn));
// TriggerPeakTimeEntry:
	fTriggerPeakTimeEntry->SetText(dgf->GetFastPeakTime(chn));
// TriggerGapTimeEntry:
	fTriggerGapTimeEntry->SetText(dgf->GetFastGapTime(chn));
// TriggerThresholdEntry:
	fTriggerThresholdEntry->SetText(dgf->GetThreshold(chn));
// TraceLengthEntry:
	fTraceLengthEntry->SetText(dgf->GetTraceLength(chn));
// TraceDelayEntry:
	fTraceDelayEntry->SetText(dgf->GetDelay(chn));
// TraceXPSALengthEntry:
	fTraceXPSALengthEntry->SetText(dgf->GetPSALength(chn));
// TraceXPsaOffsetEntry:
	fTraceXPSAOffsetEntry->SetText(dgf->GetPSAOffset(chn));
// StatRegModICSREntry:
	fStatRegModICSREntry->SetText(dgf->SetSwitchBusDefault(gDGFControlData->fIndivSwitchBusTerm, "DGFControl"));
// StatRegChanCSRAEntry:
	fStatRegChanCSRAEntry->SetText((Int_t) dgf->GetChanCSRA(chn));
// StatRegUserPsaCSREntry:
	fStatRegUserPsaCSREntry->SetText((Int_t) dgf->GetUserPsaCSR(chn));
// StatRegRunTaskEntry:
	fStatRunTaskEntry->SetText(dgf->GetParValue("RUNTASK"));
// StatCoincPatternEntry:
	fStatCoincPatternEntry->SetText((Int_t) dgf->GetCoincPattern());
// DACGainEntry:
	fDACGainEntry->SetText(dgf->GetParValue(chn, "GAINDAC"));
// DACVVEntry:
	fDACVVEntry->SetText(dgf->GetGain(chn));
// DACOffsetEntry:
	fDACOffsetEntry->SetText(dgf->GetParValue(chn, "TRACKDAC"));
// DACVoltEntry:
	fDACVoltEntry->SetText(dgf->GetOffset(chn));
// CFDFractionEntry:
	this->InitializeCFD(-1, -1);
// TraceUPSA<XXX>Entry:
	this->InitializeUPSA();
// MCAEnergyEntry:
	fMCAEnergyEntry->SetText(dgf->GetParValue(chn, "ENERGYLOW"));
// MCAEnergyBinsEntry:
	Short_t log2e = (Short_t) dgf->GetParValue(chn, "LOG2EBIN");
	intVal = (Int_t) log2e;
	fMCAEnergyBinsEntry->SetText(intVal);
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

	Bool_t updateLocalEnv = kFALSE;

	switch (EntryId) {
		case kDGFInstrEnergyPeakTimeEntry:
			dblVal = fEnergyPeakTimeEntry->GetText2Double();
			if (fEnergyPeakTimeEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetPeakTime(chn, dblVal);
			fEnergyPeakTimeEntry->SetText(dgf->GetPeakTime(chn));
			fEnergyGapTimeEntry->SetText(dgf->GetGapTime(chn));
			break;
		case kDGFInstrEnergyGapTimeEntry:
			dblVal = fEnergyGapTimeEntry->GetText2Double();
			if (fEnergyGapTimeEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetGapTime(chn, dblVal);
			fEnergyGapTimeEntry->SetText(dgf->GetGapTime(chn));
			fEnergyPeakTimeEntry->SetText(dgf->GetPeakTime(chn));
			break;
		case kDGFInstrEnergyAveragingEntry:
			intVal = fEnergyAveragingEntry->GetText2Int();
			dgf->SetParValue(chn, "LOG2BWEIGHT", intVal & 0xFFFF);
			break;
		case kDGFInstrEnergyTauEntry:
			dblVal = fEnergyTauEntry->GetText2Double();
			if (fEnergyTauEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetTau(chn, dblVal);
			break;
		case kDGFInstrTriggerPeakTimeEntry:
			dblVal = fTriggerPeakTimeEntry->GetText2Double();
			if (fTriggerPeakTimeEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetFastPeakTime(chn, dblVal);
			fTriggerPeakTimeEntry->SetText(dgf->GetFastPeakTime(chn));
			fTriggerGapTimeEntry->SetText(dgf->GetFastGapTime(chn));
			break;
		case kDGFInstrTriggerGapTimeEntry:
			dblVal = fTriggerGapTimeEntry->GetText2Double();
			if (fTriggerGapTimeEntry->CheckRange(dblVal, kTRUE, kTRUE)) dgf->SetFastGapTime(chn, dblVal);
			fTriggerGapTimeEntry->SetText(dgf->GetFastGapTime(chn));
			fTriggerPeakTimeEntry->SetText(dgf->GetFastPeakTime(chn));
			break;
		case kDGFInstrTriggerThresholdEntry:
			intVal = fTriggerThresholdEntry->GetText2Int();
			dgf->SetThreshold(chn, intVal);
			break;
		case kDGFInstrTraceXPSALengthEntry:
			dblVal = fTraceXPSALengthEntry->GetText2Double();
			dgf->SetPSALength(chn, dblVal);
			break;
		case kDGFInstrTraceXPSAOffsetEntry:
			dblVal = fTraceXPSAOffsetEntry->GetText2Double();
			dgf->SetPSAOffset(chn, dblVal);
			break;
		case kDGFInstrTraceLengthEntry:
			dblVal = fTraceLengthEntry->GetText2Double();
			dgf->SetTraceLength(chn, dblVal);
			intVal = dgf->GetParValue(chn, "TRACELENGTH");
			gDGFControlData->UpdateLocalEnv("DGFControl.Module", dgf->GetName(), "TraceLength", intVal);
			updateLocalEnv = kTRUE;
			break;
		case kDGFInstrTraceDelayEntry:
			dblVal = fTraceDelayEntry->GetText2Double();
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
			intVal = fStatRegChanCSRAEntry->GetText2Int();
			dgf->SetChanCSRA(chn, (UInt_t) intVal, TMrbDGF::kBitSet, kTRUE);
			break;
		case kDGFInstrStatRegUserPsaCSREntry:
			intVal = fStatRegUserPsaCSREntry->GetText2Int();
			dgf->SetUserPsaCSR(chn, (UInt_t) intVal, TMrbDGF::kBitSet, kTRUE);
			break;
		case kDGFInstrStatRunTaskEntry:
			intVal = fStatRunTaskEntry->GetText2Int();
			dgf->SetParValue("RUNTASK", intVal);
			gDGFControlData->UpdateLocalEnv("DGFControl.Module", dgf->GetName(), "RunTask", intVal);
			updateLocalEnv = kTRUE;
			break;
		case kDGFInstrStatCoincPatternEntry:
			intVal = fStatCoincPatternEntry->GetText2Int();
			dgf->SetCoincPattern((UInt_t) intVal);
			break;
		case kDGFInstrDACGainEntry:
			intVal = fDACGainEntry->GetText2Int();
			dgf->SetParValue(chn, "GAINDAC", intVal);
			fDACVVEntry->SetText(dgf->GetGain(chn));
			break;
		case kDGFInstrDACVVEntry:
			dblVal = fDACVVEntry->GetText2Double();
			dgf->SetGain(chn, dblVal);
			fDACVVEntry->SetText(dgf->GetGain(chn));
			fDACGainEntry->SetText(dgf->GetParValue(chn, "GAINDAC"));
			break;
		case kDGFInstrDACOffsetEntry:
			intVal = fDACOffsetEntry->GetText2Int();
			dgf->SetParValue(chn, "TRACKDAC", intVal);
			fDACVoltEntry->SetText(dgf->GetOffset(chn));
			break;
		case kDGFInstrDACVoltEntry:
			dblVal = fDACVoltEntry->GetText2Double();
			dgf->SetOffset(chn, dblVal);
			fDACOffsetEntry->SetText(dgf->GetParValue(chn, "TRACKDAC"));
			break;
		case kDGFInstrMCAEnergyEntry:
			intVal = fMCAEnergyEntry->GetText2Int();
			dgf->SetParValue(chn, "ENERGYLOW", intVal);
			break;
		case kDGFInstrMCAEnergyBinsEntry:
			intVal = fMCAEnergyBinsEntry->GetText2Int();
			dgf->SetParValue(chn, "LOG2EBIN", intVal & 0xFFFF);
			break;
		case kDGFInstrMCABaselineDCEntry:
			break;
		case kDGFInstrMCABaselineBinsEntry:
			break;
	}
	if (updateLocalEnv) gDGFControlData->WriteLocalEnv();
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
	fCFDDelayBeforeLEEntry->SetText(cfdDel * 25);
	cfdDel = (cfdVal >> 4) & 0xF;
	cfdDel = 16 - (cfdDel - 1);
	fCFDDelayBipolarEntry->SetText(cfdDel * 25);

	if (OnOffFlag == DGFInstrumentPanel::kDGFInstrCFDOn)		cfdVal |= BIT(13);
	else if (OnOffFlag == DGFInstrumentPanel::kDGFInstrCFDOff)	cfdVal &= ~BIT(13);

	if (cfdVal & BIT(13))	fCFDOnOffButton->SetState(DGFInstrumentPanel::kDGFInstrCFDOn);
	else					fCFDOnOffButton->SetState(DGFInstrumentPanel::kDGFInstrCFDOff);

	Int_t cfd = (cfdVal >> 8) & 0x1F;
	cfd = ~cfd & 0x1F;
	fCFDWalkEntry->SetText(cfd * 8);

	if (FractionFlag != -1) {
		cfdVal &= ~(BIT(14)|BIT(15));
		if (fCFDFractionButton->GetActive() == DGFInstrumentPanel::kDGFInstrCFDFract01) cfdVal |= BIT(14);
		else if (fCFDFractionButton->GetActive() == DGFInstrumentPanel::kDGFInstrCFDFract10) cfdVal |= BIT(15);
	}

	if (cfdVal & BIT(14))		fCFDFractionButton->SetState(DGFInstrumentPanel::kDGFInstrCFDFract01);
	else if (cfdVal & BIT(15))	fCFDFractionButton->SetState(DGFInstrumentPanel::kDGFInstrCFDFract10);
	else						fCFDFractionButton->SetState(DGFInstrumentPanel::kDGFInstrCFDFract00);

	fCFDRegEntry->SetText(cfdVal);
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

	Int_t cfd = fCFDDelayBeforeLEEntry->GetText2Int();
	cfd = (cfd + 24) / 25;
	cfd = 16 - (cfd - 1);
	cfdVal += (cfd & 0xF);

	cfd = fCFDDelayBipolarEntry->GetText2Int();
	cfd = (cfd + 24) / 25;
	cfd = 16 - (cfd - 1);
	cfdVal += ((cfd << 4) & 0xF0);

	if (fCFDOnOffButton->GetActive() == DGFInstrumentPanel::kDGFInstrCFDOn) cfdVal |= BIT(13);
	else																	cfdVal &= ~BIT(13);

	cfd = fCFDWalkEntry->GetText2Int();
	cfd = (cfd + 7) / 8;
	cfd = ~cfd;
	cfd &= 0x1F;
	cfd <<= 8;
	cfdVal |= cfd;

	Module->SetCFD(Channel, cfdVal);
	fCFDRegEntry->SetText(Module->GetCFD(Channel));

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
	fTraceUPSABaselineEntry->SetText(dgf->GetUserPsaData4(TMrbDGFData::kPsaBaseline03, chn));

	// cutoff 0/1 2/3: 8/8 8/8
	fTraceUPSAEnergyCutoffEntry->SetText(dgf->GetUserPsaData8(TMrbDGFData::kPsaCutOff01, chn)); 

	// trigger thresh 0/1 2/3: 8/8 8/8
	fTraceUPSATriggerThreshEntry->SetText(dgf->GetUserPsaData8(TMrbDGFData::kPsaT0Thresh01, chn));

	// t90 thresh 0-3: 4/4/4/4
	fTraceUPSAT90ThreshEntry->SetText(dgf->GetUserPsaData4(TMrbDGFData::kPsaT90Thresh03, chn));

	// psa length 0/1 2/3: 8/8 8/8
	fTraceUPSALengthEntry->SetText(dgf->GetUserPsaData8(TMrbDGFData::kPsaPSALength01, chn));

	// psa offset 0/1 2/3: 8/8 8/8
	fTraceUPSAOffsetEntry->SetText(dgf->GetUserPsaData8(TMrbDGFData::kPsaPSAOffset01, chn));

	// tfa cutoff 0/1 2/3: 8/8 8/8
	fTraceUPSATFAEnergyCutoffEntry->SetText(dgf->GetUserPsaData8(TMrbDGFData::kPsaTFACutOff01, chn));

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

	Int_t intVal = fTraceUPSABaselineEntry->GetText2Int();
	dgf->SetUserPsaData4(TMrbDGFData::kPsaBaseline03, chn, intVal);			// baseline 0-3: 4/4/4/4 

	intVal = fTraceUPSAEnergyCutoffEntry->GetText2Int();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaCutOff01, chn, intVal);			// cutoff 0/1 2/3: 8/8 8/8

	intVal = fTraceUPSATriggerThreshEntry->GetText2Int();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaT0Thresh01, chn, intVal);			// trigger thresh 0/1 2/3: 8/8 8/8

	intVal = fTraceUPSAT90ThreshEntry->GetText2Int();
	dgf->SetUserPsaData4(TMrbDGFData::kPsaT90Thresh03, chn, intVal);		// t90 thresh 0-3: 4/4/4/4

	intVal = fTraceUPSALengthEntry->GetText2Int();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaPSALength01, chn, intVal);		// psa length 0/1 2/3: 8/8 8/8

	intVal = fTraceUPSAOffsetEntry->GetText2Int();
	dgf->SetUserPsaData8(TMrbDGFData::kPsaPSAOffset01, chn, intVal);		// psa offset 0/1 2/3: 8/8 8/8

	intVal = fTraceUPSATFAEnergyCutoffEntry->GetText2Int();
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
