//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFInstrumentPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Instrument Panel
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
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
#include "DGFEditCoincPatternPanel.h"
#include "DGFCopyModuleSettingsPanel.h"

#include "SetColor.h"

const SMrbNamedX kDGFInstrumentModuleButtons[] =
			{
				{DGFInstrumentPanel::kDGFInstrButtonShow,		"Show params", 	"Show actual param settings"	},
				{0, 											NULL,		NULL									}
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
	fSelectChannel = new TGMrbRadioButtonList(fSelectFrame, "Channel", &fLofChannels, 1, 
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
	fEnergyPeakTimeEntry->GetEntry()->SetText("0");
	fEnergyPeakTimeEntry->SetRange(0, 100000);
	fEnergyPeakTimeEntry->SetIncrement(gDGFControlData->fDeltaT);
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
	fEnergyGapTimeEntry->GetEntry()->SetText("0");
	fEnergyGapTimeEntry->SetRange(0, 100000);
	fEnergyGapTimeEntry->SetIncrement(gDGFControlData->fDeltaT);
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
	fEnergyAveragingEntry->GetEntry()->SetText("2");
	fEnergyAveragingEntry->SetRange(-10, -1);
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
	fEnergyTauEntry->GetEntry()->SetText("0.0");
	fEnergyTauEntry->SetRange(0, 99);
	fEnergyTauEntry->SetIncrement(0.2);
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
	fTriggerPeakTimeEntry->GetEntry()->SetText("0.025");
	fTriggerPeakTimeEntry->SetRange(.025, .1);
	fTriggerPeakTimeEntry->SetIncrement(.025);
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
	fTriggerGapTimeEntry->GetEntry()->SetText("0");
	fTriggerGapTimeEntry->SetRange(0, .75);
	fTriggerGapTimeEntry->SetIncrement(.025);
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
	fTriggerThresholdEntry->GetEntry()->SetText("0");
	fTriggerThresholdEntry->SetRange(0, 1000);
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
	fDACGainEntry->GetEntry()->SetText("0");
	fDACGainEntry->SetRange(0, 65535);
	fDACGainEntry->SetIncrement(1);
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
	fDACVVEntry->GetEntry()->SetText("0");
	fDACVVEntry->SetRange(0, 20);
	fDACVVEntry->SetIncrement(.1);
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
	fDACOffsetEntry->GetEntry()->SetText("0");
	fDACOffsetEntry->SetRange(0, 65535);
	fDACOffsetEntry->SetIncrement(1);
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
	fDACVoltEntry->GetEntry()->SetText("0");
	fDACVoltEntry->SetRange(-3, 3);
	fDACVoltEntry->SetIncrement(.1);
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

	fTraceLengthDelayFrame = new TGGroupFrame(fTraceFrame, "Trace", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTraceLengthDelayFrame);
	fTraceFrame->AddFrame(fTraceLengthDelayFrame, groupGC->LH());
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
	fTraceLengthEntry->GetEntry()->SetText("0");
	fTraceLengthEntry->SetRange(0, 25.6);
	fTraceLengthEntry->SetIncrement(.5);
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
	fTraceDelayEntry->GetEntry()->SetText("0");
	fTraceDelayEntry->SetRange(0, 100000);
	fTraceDelayEntry->SetIncrement(.1);
	fTraceDelayEntry->AddToFocusList(&fFocusList);
	fTraceDelayEntry->Associate(this);

	TGGroupFrame * fTracePSAFrame = new TGGroupFrame(fTraceFrame, "PSA", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTracePSAFrame);
	fTraceFrame->AddFrame(fTracePSAFrame, groupGC->LH());
	TGLayoutHints * plLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(plLayout);
	HEAP(plLayout);
	fTracePSALengthEntry = new TGMrbLabelEntry(fTracePSAFrame, "Length [us]",
																200, kDGFInstrTracePSALengthEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTracePSALengthEntry);
	fTracePSAFrame->AddFrame(fTracePSALengthEntry, frameGC->LH());
	fTracePSALengthEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTracePSALengthEntry->GetEntry()->SetText("0");
	fTracePSALengthEntry->SetRange(0, 25);
	fTracePSALengthEntry->SetIncrement(.5);
	fTracePSALengthEntry->AddToFocusList(&fFocusList);
	fTracePSALengthEntry->Associate(this);

	TGLayoutHints * poLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(poLayout);
	HEAP(poLayout);
	fTracePSAOffsetEntry = new TGMrbLabelEntry(fTracePSAFrame, "Offset [us]",
																200, kDGFInstrTracePSAOffsetEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTracePSAOffsetEntry);
	fTracePSAFrame->AddFrame(fTracePSAOffsetEntry, frameGC->LH());
	fTracePSAOffsetEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	fTracePSAOffsetEntry->GetEntry()->SetText("0");
	fTracePSAOffsetEntry->SetRange(0, 25);
	fTracePSAOffsetEntry->SetIncrement(.1);
	fTracePSAOffsetEntry->AddToFocusList(&fFocusList);
	fTracePSAOffsetEntry->Associate(this);

// right: cfd
	TGLayoutHints * cfdLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, cfdLayout);
	HEAP(cfdLayout);
	fCFDFrame = new TGGroupFrame(fRightFrame, "CFD Settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCFDFrame);
	fRightFrame->AddFrame(fCFDFrame, groupGC->LH());

	TGGroupFrame * fCFDFractionFrame = new TGGroupFrame(fCFDFrame, "-", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fCFDFractionFrame);
	fCFDFrame->AddFrame(fCFDFractionFrame, groupGC->LH());
	TGLayoutHints * cfrLayout = new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1);
	entryGC->SetLH(cfrLayout);
	HEAP(cfrLayout);
	fCFDFractionEntry = new TGMrbLabelEntry(fCFDFractionFrame, "Fraction 1/2^",
																200, kDGFInstrCFDFractionEntry,
																kLEWidth,
																kLEHeight,
																kEntryWidth,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fCFDFractionEntry);
	fCFDFractionFrame->AddFrame(fCFDFractionEntry, frameGC->LH());
	fCFDFractionEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fCFDFractionEntry->GetEntry()->SetText("0");
	fCFDFractionEntry->SetRange(0, 16);
	fCFDFractionEntry->AddToFocusList(&fFocusList);
	fCFDFractionEntry->Associate(this);

// place an unvisible label to pad group frame vertically
	dmyLayout = new TGLayoutHints(kLHintsLeft, 0, 0, 35, 0);
	frameGC->SetLH(dmyLayout);
	fDummyLabel = new TGLabel(fCFDFractionFrame, "", frameGC->GC(), frameGC->Font(), kChildFrame, frameGC->BG());
	HEAP(fDummyLabel);
	fCFDFractionFrame->AddFrame(fDummyLabel, frameGC->LH());
	
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
	fStatRegModICSREntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 4, 16);
	fStatRegModICSREntry->GetEntry()->SetText("0x0000");
	fStatRegModICSREntry->SetRange(0, 0x1);
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
	fStatRegChanCSRAEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 4, 16);
	fStatRegChanCSRAEntry->GetEntry()->SetText("0x0000");
	fStatRegChanCSRAEntry->SetRange(0, 0xffff);
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
	fStatCoincPatternEntry->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt, 4, 16);
	fStatCoincPatternEntry->GetEntry()->SetText("0x0000");
	fStatCoincPatternEntry->SetRange(0, 0xf);
	fStatCoincPatternEntry->AddToFocusList(&fFocusList);
	fStatCoincPatternEntry->Associate(this);

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
	fMCAEnergyEntry->GetEntry()->SetText("0");
	fMCAEnergyEntry->SetRange(0, 8192);
	fMCAEnergyEntry->SetIncrement(100);
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
	fMCAEnergyBinsEntry->GetEntry()->SetText("0");
	fMCAEnergyBinsEntry->SetRange(-10, -1);
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
	fMCABaselineDCEntry->GetEntry()->SetText("0");
	fMCABaselineDCEntry->SetRange(0, 1024);
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
	fMCABaselineBinsEntry->GetEntry()->SetText("0");
	fMCABaselineBinsEntry->SetRange(0, 16);
	fMCABaselineBinsEntry->AddToFocusList(&fFocusList);
	fMCABaselineBinsEntry->Associate(this);

// place an unvisible label to pad group frame vertically
	frameGC->SetLH(dmyLayout);
	fDummyLabel = new TGLabel(fMCABaselineFrame, "", frameGC->GC(), frameGC->Font(), kChildFrame, frameGC->BG());
	HEAP(fDummyLabel);
	fMCABaselineFrame->AddFrame(fDummyLabel, frameGC->LH());

//	buttons
	fModuleButtonFrame = new TGMrbTextButtonGroup(this, "Actions", &fInstrModuleActions, 1, groupGC, buttonGC);
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

	TMrbString intStr;

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
						case kDGFInstrStatCoincPatternEditButton:
                    		new DGFEditCoincPatternPanel(fClient->GetRoot(), fStatCoincPatternEntry->GetEntry(),
											DGFEditCoincPatternPanel::kFrameWidth, DGFEditCoincPatternPanel::kFrameHeight);
							this->UpdateValue(kDGFInstrStatCoincPatternEntry,
											gDGFControlData->GetSelectedModuleIndex(),
											gDGFControlData->GetSelectedChannelIndex());
							break;
						case kDGFInstrButtonShow:
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
					}
					break;
				case kCM_RADIOBUTTON:
					fSelectChannel->SetState((UInt_t) Param1);
					gDGFControlData->SetSelectedChannelIndex(Param1);
					this->InitializeValues(kFALSE);
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
	TMrbString intStr, dblStr;
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
	dblStr = dgf->GetPeakTime(chn);
	fEnergyPeakTimeEntry->GetEntry()->SetText(dblStr.Data());
// EnergyGapTimeEntry:
	dblStr = dgf->GetGapTime(chn);
	fEnergyGapTimeEntry->GetEntry()->SetText(dblStr.Data());
// EnergyAveragingEntry:;
	Short_t log2bw = (Short_t) dgf->GetParValue(chn, "LOG2BWEIGHT");
	intVal = (Int_t) log2bw;
	intStr = intVal;
	fEnergyAveragingEntry->GetEntry()->SetText(intStr.Data());
// EnergyTauEntry:
	dblStr = dgf->GetTau(chn);
	fEnergyTauEntry->GetEntry()->SetText(dblStr.Data());
// TriggerPeakTimeEntry:
	dblStr = dgf->GetFastPeakTime(chn);
	fTriggerPeakTimeEntry->GetEntry()->SetText(dblStr.Data());
// TriggerGapTimeEntry:
	dblStr = dgf->GetFastGapTime(chn);
	fTriggerGapTimeEntry->GetEntry()->SetText(dblStr.Data());
// TriggerThresholdEntry:
	intStr = dgf->GetThreshold(chn);
	fTriggerThresholdEntry->GetEntry()->SetText(intStr.Data());
// TraceLengthEntry:
	dblStr = dgf->GetTraceLength(chn);
	fTraceLengthEntry->GetEntry()->SetText(dblStr.Data());
// TraceDelayEntry:
	dblStr = dgf->GetDelay(chn);
	fTraceDelayEntry->GetEntry()->SetText(dblStr.Data());
// TracePSALengthEntry:
	dblStr = dgf->GetPSALength(chn);
	fTracePSALengthEntry->GetEntry()->SetText(dblStr.Data());
// TracePsaOffsetEntry:
	dblStr = dgf->GetPSAOffset(chn);
	fTracePSAOffsetEntry->GetEntry()->SetText(dblStr.Data());
// StatRegModICSREntry:
	dgf->SetSwitchBusDefault(gDGFControlData->fIndivSwitchBusTerm, "DGFControl");
	intStr.FromInteger((Int_t) dgf->GetSwitchBus(), 4, '0', 16);
	fStatRegModICSREntry->GetEntry()->SetText(intStr.Data());
// StatRegChanCSRAEntry:
	intStr.FromInteger((Int_t) dgf->GetChanCSRA(chn), 4, '0', 16);
	fStatRegChanCSRAEntry->GetEntry()->SetText(intStr.Data());
// StatCoincPatternEntry:
	intStr.FromInteger((Int_t) dgf->GetCoincPattern(), 4, '0', 16);
	fStatCoincPatternEntry->GetEntry()->SetText(intStr.Data());
// DACGainEntry:
	intStr = dgf->GetParValue(chn, "GAINDAC");
	fDACGainEntry->GetEntry()->SetText(intStr.Data());
// DACVVEntry:
	dblStr = dgf->GetGain(chn);
	fDACVVEntry->GetEntry()->SetText(dblStr.Data());
// DACOffsetEntry:
	intStr = dgf->GetParValue(chn, "TRACKDAC");
	fDACOffsetEntry->GetEntry()->SetText(intStr.Data());
// DACVoltEntry:
	dblStr = dgf->GetOffset(chn);
	fDACVoltEntry->GetEntry()->SetText(dblStr.Data());
// CFDFractionEntry:

// MCAEnergyEntry:
	intStr = dgf->GetParValue(chn, "ENERGYLOW");
	fMCAEnergyEntry->GetEntry()->SetText(intStr.Data());
// MCAEnergyBinsEntry:
	Short_t log2e = (Short_t) dgf->GetParValue(chn, "LOG2EBIN");
	intVal = (Int_t) log2e;
	intStr = intVal;
	fMCAEnergyBinsEntry->GetEntry()->SetText(intStr.Data());
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
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calls $EDITOR to show current module settings
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gDGFControlData->IsOffline()) return(kTRUE);

	TString editor = gSystem->Getenv("EDITOR");
	DGFModule * dgfModule = gDGFControlData->GetSelectedModule();
	TString tmpFile = "/tmp/DGFControl.";
	tmpFile += dgfModule->GetName();
	tmpFile += ".par";
	dgfModule->GetAddr()->PrintParamsToFile(tmpFile.Data());
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

	TGTextEntry * entry;

	TMrbDGF * dgf;
	Int_t chn;
	TMrbString intStr, dblStr;
	Int_t intVal;
	Double_t dblVal;
	Int_t idx;
	
	if (gDGFControlData->IsOffline()) return(kTRUE);

	dgf = gDGFControlData->GetModule(ModuleId)->GetAddr();
	chn = gDGFControlData->GetChannel(ChannelId);

	switch (EntryId) {
		case kDGFInstrEnergyPeakTimeEntry:
			entry = fEnergyPeakTimeEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetPeakTime(chn, dblVal);
			dblStr = dgf->GetPeakTime(chn);
			fEnergyPeakTimeEntry->GetEntry()->SetText(dblStr.Data());
			dblStr = dgf->GetGapTime(chn);
			fEnergyGapTimeEntry->GetEntry()->SetText(dblStr.Data());
			break;
		case kDGFInstrEnergyGapTimeEntry:
			entry = fEnergyGapTimeEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetGapTime(chn, dblVal);
			dblStr = dgf->GetGapTime(chn);
			fEnergyGapTimeEntry->GetEntry()->SetText(dblStr.Data());
			dblStr = dgf->GetPeakTime(chn);
			fEnergyPeakTimeEntry->GetEntry()->SetText(dblStr.Data());
			break;
		case kDGFInstrEnergyAveragingEntry:
			entry = fEnergyAveragingEntry->GetEntry();
			intStr = entry->GetText();
			idx = intStr.Index("0x", 0);
			if (idx >= 0) {
				intStr = intStr(idx + 2, intStr.Length());
				intStr.ToInteger(intVal, 16);
			} else {
				intStr.ToInteger(intVal);
			}
			dgf->SetParValue(chn, "LOG2BWEIGHT", intVal & 0xFFFF);
			break;
		case kDGFInstrEnergyTauEntry:
			entry = fEnergyTauEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetTau(chn, dblVal);
			break;
		case kDGFInstrTriggerPeakTimeEntry:
			entry = fTriggerPeakTimeEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetFastPeakTime(chn, dblVal);
			dblStr = dgf->GetFastPeakTime(chn);
			fTriggerPeakTimeEntry->GetEntry()->SetText(dblStr.Data());
			dblStr = dgf->GetFastGapTime(chn);
			fTriggerGapTimeEntry->GetEntry()->SetText(dblStr.Data());
			break;
		case kDGFInstrTriggerGapTimeEntry:
			entry = fTriggerGapTimeEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetFastGapTime(chn, dblVal);
			dblStr = dgf->GetFastGapTime(chn);
			fTriggerGapTimeEntry->GetEntry()->SetText(dblStr.Data());
			dblStr = dgf->GetFastPeakTime(chn);
			fTriggerPeakTimeEntry->GetEntry()->SetText(dblStr.Data());
			break;
		case kDGFInstrTriggerThresholdEntry:
			entry = fTriggerThresholdEntry->GetEntry();
			intStr = entry->GetText();
			idx = intStr.Index("0x", 0);
			if (idx >= 0) {
				intStr = intStr(idx + 2, intStr.Length());
				intStr.ToInteger(intVal, 16);
			} else {
				intStr.ToInteger(intVal);
			}
			dgf->SetThreshold(chn, intVal);
			break;
		case kDGFInstrTracePSALengthEntry:
			entry = fTracePSALengthEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetPSALength(chn, dblVal);
			break;
		case kDGFInstrTracePSAOffsetEntry:
			entry = fTracePSAOffsetEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetPSAOffset(chn, dblVal);
			break;
		case kDGFInstrTraceLengthEntry:
			entry = fTraceLengthEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetTraceLength(chn, dblVal);
			break;
		case kDGFInstrTraceDelayEntry:
			entry = fTraceDelayEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetDelay(chn, dblVal);
			break;
		case kDGFInstrStatRegChanCSRAEntry:
			entry = fStatRegChanCSRAEntry->GetEntry();
			intStr = entry->GetText();
			idx = intStr.Index("0x", 0);
			if (idx >= 0) {
				intStr = intStr(idx + 2, intStr.Length());
				intStr.ToInteger(intVal, 16);
			} else {
				intStr.ToInteger(intVal);
			}
			dgf->SetChanCSRA(chn, (UInt_t) intVal, TMrbDGF::kBitSet, kTRUE);
			break;
		case kDGFInstrStatCoincPatternEntry:
			entry = fStatCoincPatternEntry->GetEntry();
			intStr = entry->GetText();
			idx = intStr.Index("0x", 0);
			if (idx >= 0) {
				intStr = intStr(idx + 2, intStr.Length());
				intStr.ToInteger(intVal, 16);
			} else {
				intStr.ToInteger(intVal);
			}
			dgf->SetCoincPattern((UInt_t) intVal);
			break;
		case kDGFInstrDACGainEntry:
			entry = fDACGainEntry->GetEntry();
			intStr = entry->GetText();
			idx = intStr.Index("0x", 0);
			if (idx >= 0) {
				intStr = intStr(idx + 2, intStr.Length());
				intStr.ToInteger(intVal, 16);
			} else {
				intStr.ToInteger(intVal);
			}
			dgf->SetParValue(chn, "GAINDAC", intVal);
			dblStr = dgf->GetGain(chn);
			fDACVVEntry->GetEntry()->SetText(dblStr);
			break;
		case kDGFInstrDACVVEntry:
			entry = fDACVVEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetGain(chn, dblVal);
			dblStr = dgf->GetGain(chn);
			fDACVVEntry->GetEntry()->SetText(dblStr);
			intStr = dgf->GetParValue(chn, "GAINDAC");
			fDACGainEntry->GetEntry()->SetText(intStr);
			break;
		case kDGFInstrDACOffsetEntry:
			entry = fDACOffsetEntry->GetEntry();
			intStr = entry->GetText();
			idx = intStr.Index("0x", 0);
			if (idx >= 0) {
				intStr = intStr(idx + 2, intStr.Length());
				intStr.ToInteger(intVal, 16);
			} else {
				intStr.ToInteger(intVal);
			}
			dgf->SetParValue(chn, "TRACKDAC", intVal);
			dblStr = dgf->GetOffset(chn);
			fDACVoltEntry->GetEntry()->SetText(dblStr);
			break;
		case kDGFInstrDACVoltEntry:
			entry = fDACVoltEntry->GetEntry();
			dblStr = entry->GetText();
			dblStr.ToDouble(dblVal);
			dgf->SetOffset(chn, dblVal);
			intStr = dgf->GetParValue(chn, "TRACKDAC");
			fDACOffsetEntry->GetEntry()->SetText(intStr);
			break;
		case kDGFInstrCFDFractionEntry:
			entry = fCFDFractionEntry->GetEntry();
			break;
		case kDGFInstrMCAEnergyEntry:
			entry = fMCAEnergyEntry->GetEntry();
			intStr = entry->GetText();
			intStr.ToInteger(intVal);
			dgf->SetParValue(chn, "ENERGYLOW", intVal);
			break;
		case kDGFInstrMCAEnergyBinsEntry:
			entry = fMCAEnergyBinsEntry->GetEntry();
			intStr = entry->GetText();
			idx = intStr.Index("0x", 0);
			if (idx >= 0) {
				intStr = intStr(idx + 2, intStr.Length());
				intStr.ToInteger(intVal, 16);
			} else {
				intStr.ToInteger(intVal);
			}
			dgf->SetParValue(chn, "LOG2EBIN", intVal & 0xFFFF);
			break;
		case kDGFInstrMCABaselineDCEntry:
			entry = fMCABaselineDCEntry->GetEntry();
			break;
		case kDGFInstrMCABaselineBinsEntry:
			entry = fMCABaselineBinsEntry->GetEntry();
			break;
	}
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
		case kDGFInstrTracePSALengthEntry:
			entry = fTracePSALengthEntry->GetEntry();
			break;
		case kDGFInstrTracePSAOffsetEntry:
			entry = fTracePSAOffsetEntry->GetEntry();
			break;
		case kDGFInstrStatRegModICSREntry:
			entry = fStatRegModICSREntry->GetEntry();
			break;
		case kDGFInstrStatRegChanCSRAEntry:
			entry = fStatRegChanCSRAEntry->GetEntry();
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
		case kDGFInstrCFDFractionEntry:
			entry = fCFDFractionEntry->GetEntry();
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
	}
	if (entry) fFocusList.FocusForward(entry);
	return;
}
