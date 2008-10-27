//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel
// Purpose:        A GUI to control vme modules via tcp
// Description:    Connect to server
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302Panel.cxx,v 1.6 2008-10-27 12:26:07 Marabou Exp $       
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
#include "VMESis3302SaveRestorePanel.h"
#include "VMESis3302CopyPanel.h"
#include "VMESis3302StartRunPanel.h"

#include "SetColor.h"

extern TMrbC2Lynx * gMrbC2Lynx;
extern VMEControlData * gVMEControlData;
extern TMrbLogger * gMrbLog;

ClassImp(VMESis3302Panel)

const SMrbNamedXShort kVMEClockSource[] =
			{
				{VMESis3302Panel::kVMEClockSource100,		"100 MHz"		},
				{VMESis3302Panel::kVMEClockSource50,		"50 MHz"		},
				{VMESis3302Panel::kVMEClockSource25,		"25 MHz"		},
				{VMESis3302Panel::kVMEClockSource10,		"10 MHz"		},
				{VMESis3302Panel::kVMEClockSource1, 		"1 MHz"			},
				{VMESis3302Panel::kVMEClockSourceExt,		"extern"		},
				{0, 												NULL			}
			};

const SMrbNamedX kVMECommonTrigMode[] =
			{
				{VMESis3302Panel::kVMETrigModeIntern,		"intern/async", "internal/async trigger: FIR trigger"	},
				{VMESis3302Panel::kVMETrigModeExtern, 		"extern/sync", "external/sync trigger: via LEMO-3 or VME KEY"	},
				{VMESis3302Panel::kVMETrigModeLemo2,		"ts clear", "enable timestamp clear via LEMO-2"	},
				{VMESis3302Panel::kVMETrigModeLemo3, 		"xtrig ena", "enable external trigger via LEMO-3"	},
				{0, 												NULL			}
			};

const SMrbNamedX kVMETrigPolar[] =
			{
				{VMESis3302Panel::kVMETrigPolPos,			"positive", "trigger polarity positive"	},
				{VMESis3302Panel::kVMETrigPolNeg,			"negative", "trigger polarity negative"	},
				{0, 												NULL			}
			};

const SMrbNamedX kVMETrigCond[] =
			{
				{VMESis3302Panel::kVMETrigCondDis,			"disabled", "output trigger disabled"	},
				{VMESis3302Panel::kVMETrigCondGTEna,		"enabled if GT", "output if greater than treshold"	},
				{VMESis3302Panel::kVMETrigCondGTDis,		"disabled if GT", "no output if greater than treshold"	},
				{VMESis3302Panel::kVMETrigCondEna,			"enabled", "output trigger enabled"	},
				{0, 												NULL			}
			};

const SMrbNamedX kVMETrigMode[] =
			{
				{VMESis3302Panel::kVMETrigModeDis,			"disabled", "no trigger"	},
				{VMESis3302Panel::kVMETrigModeIntern,		"intern/async", "internal/async trigger: FIR trigger"	},
				{VMESis3302Panel::kVMETrigModeExtern, 		"extern/sync", "external/sync trigger: via LEMO-3 or VME KEY"	},
				{VMESis3302Panel::kVMETrigModeBoth, 		"intern+extern", "OR of intern and extern triggers"	},
				{0, 												NULL			}
			};

const SMrbNamedX kVMEDecim[] =
			{
				{VMESis3302Panel::kVMEDecimDis,				"disabled", "no decimation"	},
				{VMESis3302Panel::kVMEDecim2,				"2", "decimation = 2"	},
				{VMESis3302Panel::kVMEDecim4,				"4", "decimation = 4"	},
				{VMESis3302Panel::kVMEDecim8,				"8", "decimation = 8"	},
				{0, 												NULL			}
			};

const SMrbNamedX kVMESample[] =
			{
				{VMESis3302Panel::kVMESampleFull,			"full trace", "full trace (510 steps) + min/max"	},
				{VMESis3302Panel::kVMESampleMinMax,			"min/max", "min/max energy values only (no trace data)"	},
				{VMESis3302Panel::kVMESampleProg,			"progr trace", "programmable trace (up to 510 steps) + min/max"	},
				{0, 												NULL			}
			};

const SMrbNamedX kVMEActions[] =
			{
				{VMESis3302Panel::kVMESis3302ActionReset,			"Reset module",		"Reset module to power-up settings"	},
				{VMESis3302Panel::kVMESis3302ActionRun,				"Start run",		"Start data acquisition"	},
				{VMESis3302Panel::kVMESis3302ActionSaveRestore,		"Save/restore",		"Save/restore module settings"	},
				{VMESis3302Panel::kVMESis3302ActionCopySettings,	"Copy settings",	"Copy settings to other modules/channels"	},
				{0, 												NULL,				NULL								}
			};

static TC2LSis3302 * curModule = NULL;
static Int_t curChannel = 0;

Int_t frameWidth;
Int_t frameHeight;

VMESis3302Panel::VMESis3302Panel(TGCompositeFrame * TabFrame) :
				TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel
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
	TGLayoutHints * loNormal = new TGLayoutHints(kLHintsTop, 10, 10, 1, 1);
	TGLayoutHints * loNormal2 = new TGLayoutHints(kLHintsTop, 1, 1, 1, 1);
	HEAP(loXpndX); HEAP(loXpndX2); HEAP(loNormal); HEAP(loNormal2);

	frameGC->SetLH(loNormal);
	comboGC->SetLH(loNormal2);
	labelGC->SetLH(loNormal);
	buttonGC->SetLH(loNormal);
	entryGC->SetLH(loNormal);
	groupGC->SetLH(loXpndX);

// geometry
	frameWidth = this->GetWidth();
	frameHeight = this->GetHeight();

//	Initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kVMEActions);

//	module / channel selection
	this->SetupModuleList();

	fSelectFrame = new TGGroupFrame(this, "Select module", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, groupGC->LH());

	fSelectModule = new TGMrbLabelCombo(fSelectFrame, "Module",	&fLofModules,
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

	fSettingsFrame = new TGGroupFrame(this, "Module settings", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSettingsFrame);
	this->AddFrame(fSettingsFrame, groupGC->LH());

	fClockAndTrig = new TGGroupFrame(fSettingsFrame, "Clock & trigger", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fClockAndTrig);
	fSettingsFrame->AddFrame(fClockAndTrig, groupGC->LH());

	TMrbLofNamedX lofClockSources;
	lofClockSources.AddNamedX(kVMEClockSource);
	fClockSource = new TGMrbLabelCombo(fClockAndTrig, "Clock source",	&lofClockSources,
																		kVMESis3302ClockSource, 1,
																		frameWidth/5, kLEHeight, frameWidth/8,
																		frameGC, labelGC, comboGC, labelGC);
	fClockAndTrig->AddFrame(fClockSource, frameGC->LH());
	fClockSource->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ClockSourceChanged(Int_t, Int_t)");

	TMrbLofNamedX lofCommonTrigModes;
	lofCommonTrigModes.AddNamedX(kVMECommonTrigMode);
	lofCommonTrigModes.SetPatternMode();
	fCommonTrigMode = new TGMrbCheckButtonList(fClockAndTrig, "Common trigger mode", &lofCommonTrigModes,
																		kVMESis3302CommonTrigMode, 1, 
																		frameWidth/5, kLEHeight, 
																		frameGC, labelGC, buttonGC);
	fClockAndTrig->AddFrame(fCommonTrigMode, frameGC->LH());
	((TGMrbButtonFrame *) fCommonTrigMode)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "CommonTrigModeChanged(Int_t, Int_t)");
	fCommonTrigMode->ChangeButtonBackground(gVMEControlData->fColorGreen);

	TGLayoutHints * lh = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);
	HEAP(lh);

	TGHorizontalFrame * h1 = new TGHorizontalFrame(fSettingsFrame);
	HEAP(h1);
	fSettingsFrame->AddFrame(h1, lh);
	h1->ChangeBackground(gVMEControlData->fColorGreen);

	fDacSettings = new TGGroupFrame(h1, "DAC settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDacSettings);
	h1->AddFrame(fDacSettings, groupGC->LH());

	TGHorizontalFrame * dmy1 = new TGHorizontalFrame(h1);
	HEAP(dmy1);
	h1->AddFrame(dmy1, lh);
	dmy1->ChangeBackground(gVMEControlData->fColorGreen);

	fDacOffset = new TGMrbLabelEntry(fDacSettings, "Offset",	200, kVMESis3302DacOffset,
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fDacOffset);
	fDacOffset->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fDacOffset->SetText(0);
	fDacOffset->SetRange(0, 2 << 16 - 1);
	fDacOffset->SetIncrement(10);
	fDacOffset->ShowToolTip(kTRUE, kTRUE);
	fDacSettings->AddFrame(fDacOffset, frameGC->LH());
	fDacOffset->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "DacOffsetChanged(Int_t, Int_t)");

	TGHorizontalFrame * h2 = new TGHorizontalFrame(fSettingsFrame);
	HEAP(h2);
	fSettingsFrame->AddFrame(h2, lh);
	h2->ChangeBackground(gVMEControlData->fColorGreen);

	fTriggerFilter = new TGGroupFrame(h2, "Trigger filter", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTriggerFilter);
	h2->AddFrame(fTriggerFilter, groupGC->LH());

	TGVerticalFrame * tvl = new TGVerticalFrame(fTriggerFilter);
	HEAP(tvl);
	fTriggerFilter->AddFrame(tvl, lh);
	tvl->ChangeBackground(gVMEControlData->fColorGreen);

	TGVerticalFrame * tvr = new TGVerticalFrame(fTriggerFilter);
	HEAP(tvr);
	fTriggerFilter->AddFrame(tvr, lh);
	tvr->ChangeBackground(gVMEControlData->fColorGreen);

	TMrbLofNamedX lofTrigModes;
	lofTrigModes.AddNamedX(kVMETrigMode);
	lofTrigModes.SetPatternMode();
	fTrigMode = new TGMrbLabelCombo(tvl, "Mode", 		&lofTrigModes,
														kVMESis3302TrigMode, 1,
														frameWidth/10, kLEHeight, frameWidth/10,
														frameGC, labelGC, comboGC);
	tvl->AddFrame(fTrigMode, groupGC->LH());
	fTrigMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TrigModeChanged(Int_t, Int_t)");

	fTrigPeaking = new TGMrbLabelEntry(tvl, "Peaking",	200, kVMESis3302TrigPeaking,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigPeaking);
	fTrigPeaking->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigPeaking->SetText(1);
	fTrigPeaking->SetRange(1, 16);
	fTrigPeaking->SetIncrement(1);
	fTrigPeaking->ShowToolTip(kTRUE, kTRUE);
	tvl->AddFrame(fTrigPeaking, groupGC->LH());
	fTrigPeaking->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigPeakingChanged(Int_t, Int_t)");

	TMrbLofNamedX lofTrigPols;
	lofTrigPols.AddNamedX(kVMETrigPolar);
	lofTrigPols.SetPatternMode();
	fTrigPol = new TGMrbLabelCombo(tvr, "Polarity", 	&lofTrigPols,
														kVMESis3302TrigPol, 1,
														frameWidth/10, kLEHeight, frameWidth/10,
														frameGC, labelGC, comboGC);
	tvr->AddFrame(fTrigPol, groupGC->LH());
	fTrigPol->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TrigPolarChanged(Int_t, Int_t)");

	fTrigGap = new TGMrbLabelEntry(tvl, "Gap",			200, kVMESis3302TrigGap,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigGap);
	fTrigGap->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigGap->SetText(1);
	fTrigGap->SetRange(0, 15);
	fTrigGap->SetIncrement(1);
	fTrigGap->ShowToolTip(kTRUE, kTRUE);
	tvl->AddFrame(fTrigGap, groupGC->LH());
	fTrigGap->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigGapChanged(Int_t, Int_t)");

	fTrigThresh = new TGMrbLabelEntry(tvl, "Threshold",	200, kVMESis3302TrigThresh,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigThresh);
	fTrigThresh->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigThresh->SetText(0);
	fTrigThresh->SetRange(0, 2 << 16 - 1);
	fTrigThresh->SetIncrement(10);
	fTrigThresh->ShowToolTip(kTRUE, kTRUE);
	tvl->AddFrame(fTrigThresh, groupGC->LH());
	fTrigThresh->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigThreshChanged(Int_t, Int_t)");

	fTrigOut = new TGMrbLabelEntry(tvl, "Out length",	200, kVMESis3302TrigOut,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigOut);
	fTrigOut->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigOut->SetText(0);
	fTrigOut->SetRange(0, 255);
	fTrigOut->SetIncrement(10);
	fTrigOut->ShowToolTip(kTRUE, kTRUE);
	tvl->AddFrame(fTrigOut, groupGC->LH());
	fTrigOut->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigOutChanged(Int_t, Int_t)");

	TGLayoutHints * lh2 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, kLEHeight + 3, 0);
	HEAP(lh2);
	TGHorizontalFrame * dmy2 = new TGHorizontalFrame(tvr);
	HEAP(dmy2);
	tvr->AddFrame(dmy2, lh2);
	dmy2->ChangeBackground(gVMEControlData->fColorGreen);

	fTrigSumG = new TGMrbLabelEntry(tvr, "SumG",		200, kVMESis3302TrigSumG,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC);
	HEAP(fTrigSumG);
	tvr->AddFrame(fTrigSumG, groupGC->LH());
	fTrigSumG->SetState(kFALSE);

	fTrigCounts = new TGMrbLabelEntry(tvr, "Counts",	200, kVMESis3302TrigCounts,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC);
	HEAP(fTrigCounts);
	tvr->AddFrame(fTrigCounts, groupGC->LH());
	fTrigCounts->SetState(kFALSE);

	TMrbLofNamedX lofTrigConds;
	lofTrigConds.AddNamedX(kVMETrigCond);
	lofTrigConds.SetPatternMode();
	fTrigCond = new TGMrbLabelCombo(tvr, "Condition", 	&lofTrigConds,
														kVMESis3302TrigCond, 1,
														frameWidth/10, kLEHeight, frameWidth/10,
														frameGC, labelGC, comboGC);
	tvr->AddFrame(fTrigCond, groupGC->LH());
	fTrigCond->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TrigCondChanged(Int_t, Int_t)");

	fEnergyFilter = new TGGroupFrame(h2, "Energy filter", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fEnergyFilter);
	h2->AddFrame(fEnergyFilter, groupGC->LH());

	TGVerticalFrame * evl = new TGVerticalFrame(fEnergyFilter);
	HEAP(evl);
	fEnergyFilter->AddFrame(evl, lh);
	evl->ChangeBackground(gVMEControlData->fColorGreen);

	TGVerticalFrame * evr = new TGVerticalFrame(fEnergyFilter);
	HEAP(evr);
	fEnergyFilter->AddFrame(evr, lh);
	evr->ChangeBackground(gVMEControlData->fColorGreen);

	TMrbLofNamedX lofDecims;
	lofDecims.AddNamedX(kVMEDecim);
	lofDecims.SetPatternMode();
	fEnergyDecimation = new TGMrbLabelCombo(evl, "Decimation", 	&lofDecims,
																kVMESis3302EnergyDecimation, 1,
																frameWidth/10, kLEHeight, frameWidth/10,
																frameGC, labelGC, comboGC);
	evl->AddFrame(fEnergyDecimation, groupGC->LH());
	fEnergyDecimation->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "DecimationChanged(Int_t, Int_t)");

	fEnergyPeaking = new TGMrbLabelEntry(evl, "Peaking",	200, kVMESis3302EnergyPeaking,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyPeaking);
	fEnergyPeaking->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyPeaking->SetText(0);
	fEnergyPeaking->SetRange(0, 255);
	fEnergyPeaking->SetIncrement(10);
	fEnergyPeaking->ShowToolTip(kTRUE, kTRUE);
	evl->AddFrame(fEnergyPeaking, groupGC->LH());
	fEnergyPeaking->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyPeakingChanged(Int_t, Int_t)");

	fEnergyGap = new TGMrbLabelEntry(evl, "Gap",			200, kVMESis3302EnergyGap,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyGap);
	fEnergyGap->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyGap->SetText(0);
	fEnergyGap->SetRange(0, 255);
	fEnergyGap->SetIncrement(10);
	fEnergyGap->ShowToolTip(kTRUE, kTRUE);
	evl->AddFrame(fEnergyGap, groupGC->LH());
	fEnergyGap->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyGapChanged(Int_t, Int_t)");

	fPreTrigDelay = new TGMrbLabelEntry(evl, "Pretrig delay",	200, kVMESis3302TrigDelay,
																frameWidth/5, kLEHeight, frameWidth/10,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fPreTrigDelay);
	fPreTrigDelay->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fPreTrigDelay->SetText(0);
	fPreTrigDelay->SetRange(0, 1023);
	fPreTrigDelay->SetIncrement(10);
	fPreTrigDelay->ShowToolTip(kTRUE, kTRUE);
	evl->AddFrame(fPreTrigDelay, groupGC->LH());
	fPreTrigDelay->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigDelayChanged(Int_t, Int_t)");

	fEnergyTauFactor = new TGMrbLabelEntry(evl, "Tau",		200, kVMESis3302EnergyTauFactor,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyTauFactor);
	fEnergyTauFactor->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyTauFactor->SetText(0);
	fEnergyTauFactor->SetRange(0, 63);
	fEnergyTauFactor->SetIncrement(1);
	fEnergyTauFactor->ShowToolTip(kTRUE, kTRUE);
	evl->AddFrame(fEnergyTauFactor, groupGC->LH());
	fEnergyTauFactor->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TauFactorChanged(Int_t, Int_t)");

	TGLayoutHints * lh3 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 2 * kLEHeight + 6, 0);
	HEAP(lh3);
	TGHorizontalFrame * dmy3 = new TGHorizontalFrame(evr);
	HEAP(dmy3);
	evr->AddFrame(dmy3, lh3);
	dmy3->ChangeBackground(gVMEControlData->fColorGreen);

	fEnergyDecayTime = new TGMrbLabelEntry(evr, "Decay time",	200, kVMESis3302EnergyDecayTime,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC);
	HEAP(fEnergyDecayTime);
	fEnergyDecayTime->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	evr->AddFrame(fEnergyDecayTime, groupGC->LH());
	fEnergyDecayTime->SetState(kFALSE);

	fEnergyGate = new TGMrbLabelEntry(evr, "Energy gate",	200, kVMESis3302EnergyGate,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC);
	HEAP(fEnergyGate);
	evr->AddFrame(fEnergyGate, groupGC->LH());
	fEnergyGate->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyGate->SetTextAlignment(kTextRight);
	fEnergyGate->SetState(kFALSE);

	fTrigGate = new TGMrbLabelEntry(evr, "Trig gate",		200, kVMESis3302TrigGate,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC);
	HEAP(fTrigGate);
	evr->AddFrame(fTrigGate, groupGC->LH());
	fTrigGate->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigGate->SetTextAlignment(kTextRight);
	fTrigGate->SetState(kFALSE);

	TGHorizontalFrame * h3 = new TGHorizontalFrame(fSettingsFrame);
	HEAP(h3);
	fSettingsFrame->AddFrame(h3, lh);
	h3->ChangeBackground(gVMEControlData->fColorGreen);

	fRawDataSampling = new TGGroupFrame(h3, "Raw data sampling", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fRawDataSampling);
	h3->AddFrame(fRawDataSampling, groupGC->LH());

	TGVerticalFrame * rdvl = new TGVerticalFrame(fRawDataSampling);
	HEAP(rdvl);
	fRawDataSampling->AddFrame(rdvl, lh);
	rdvl->ChangeBackground(gVMEControlData->fColorGreen);

	TGVerticalFrame * rdvr = new TGVerticalFrame(fRawDataSampling);
	HEAP(rdvr);
	fRawDataSampling->AddFrame(rdvr, lh);
	rdvr->ChangeBackground(gVMEControlData->fColorGreen);

	fRawDataStart = new TGMrbLabelEntry(rdvl, "Start",		200, kVMESis3302RawDataStart,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fRawDataStart);
	fRawDataStart->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRawDataStart->SetText(0);
	fRawDataStart->SetRange(0, 4095);
	fRawDataStart->SetIncrement(2);
	fRawDataStart->ShowToolTip(kTRUE, kTRUE);
	rdvl->AddFrame(fRawDataStart, groupGC->LH());
	fRawDataStart->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "RawDataStartChanged(Int_t, Int_t)");

	fRawDataLength = new TGMrbLabelEntry(rdvl, "Length",	200, kVMESis3302RawDataLength,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fRawDataLength);
	fRawDataLength->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fRawDataLength->SetText(0);
	fRawDataLength->SetRange(0, 1024);
	fRawDataLength->SetIncrement(128);
	fRawDataLength->ShowToolTip(kTRUE, kTRUE);
	rdvl->AddFrame(fRawDataLength, groupGC->LH());
	fRawDataLength->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "RawDataLengthChanged(Int_t, Int_t)");

	fEnergyDataSampling = new TGGroupFrame(h3, "Energy data sampling", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fEnergyDataSampling);
	h3->AddFrame(fEnergyDataSampling, groupGC->LH());

	TGVerticalFrame * edvl = new TGVerticalFrame(fEnergyDataSampling);
	HEAP(edvl);
	fEnergyDataSampling->AddFrame(edvl, lh);
	edvl->ChangeBackground(gVMEControlData->fColorGreen);

	TGVerticalFrame * edvr = new TGVerticalFrame(fEnergyDataSampling);
	HEAP(edvr);
	fEnergyDataSampling->AddFrame(edvr, lh);
	edvr->ChangeBackground(gVMEControlData->fColorGreen);

	TMrbLofNamedX lofSamplingModes;
	lofSamplingModes.AddNamedX(kVMESample);
	lofSamplingModes.SetPatternMode();
	fEnergyDataMode = new TGMrbLabelCombo(edvl, "Mode", 	&lofSamplingModes,
															kVMESis3302EnergyDataMode, 1,
															frameWidth/10, kLEHeight, frameWidth/10,
															frameGC, labelGC, comboGC);
	edvl->AddFrame(fEnergyDataMode, groupGC->LH());
	fEnergyDataMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyDataModeChanged(Int_t, Int_t)");

	fEnergyDataStart1 = new TGMrbLabelEntry(edvl, "Start 1",	200, kVMESis3302RawDataStart,
																frameWidth/5, kLEHeight, frameWidth/10,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyDataStart1);
	fEnergyDataStart1->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyDataStart1->SetText(0);
	fEnergyDataStart1->SetRange(0, 2047);
	fEnergyDataStart1->SetIncrement(1);
	fEnergyDataStart1->ShowToolTip(kTRUE, kTRUE);
	edvl->AddFrame(fEnergyDataStart1, groupGC->LH());
	fEnergyDataStart1->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyDataStart1Changed(Int_t, Int_t)");

	fEnergyDataStart2 = new TGMrbLabelEntry(edvl, "Start 2",	200, kVMESis3302RawDataStart,
																frameWidth/5, kLEHeight, frameWidth/10,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyDataStart2);
	fEnergyDataStart2->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyDataStart2->SetText(0);
	fEnergyDataStart2->SetRange(0, 4095);
	fEnergyDataStart2->SetIncrement(1);
	fEnergyDataStart2->ShowToolTip(kTRUE, kTRUE);
	edvl->AddFrame(fEnergyDataStart2, groupGC->LH());
	fEnergyDataStart2->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyDataStart2Changed(Int_t, Int_t)");

	fEnergyDataStart3 = new TGMrbLabelEntry(edvl, "Start 3",	200, kVMESis3302RawDataStart,
																frameWidth/5, kLEHeight, frameWidth/10,
																frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyDataStart3);
	fEnergyDataStart3->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyDataStart3->SetText(0);
	fEnergyDataStart3->SetRange(0, 4095);
	fEnergyDataStart3->SetIncrement(1);
	fEnergyDataStart3->ShowToolTip(kTRUE, kTRUE);
	edvl->AddFrame(fEnergyDataStart3, groupGC->LH());
	fEnergyDataStart3->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyDataStart3Changed(Int_t, Int_t)");

	fEnergyDataLength = new TGMrbLabelEntry(edvl, "Length",	200, kVMESis3302RawDataLength,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyDataLength);
	fEnergyDataLength->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyDataLength->SetText(0);
	fEnergyDataLength->SetRange(0, 512);
	fEnergyDataLength->SetIncrement(32);
	fEnergyDataLength->ShowToolTip(kTRUE, kTRUE);
	edvl->AddFrame(fEnergyDataLength, groupGC->LH());
	fEnergyDataLength->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyDataLengthChanged(Int_t, Int_t)");

// action buttons
	fActionButtons = new TGMrbTextButtonGroup(this, "Actions", &fActions, -1, 1, groupGC, buttonGC);
	HEAP(fActionButtons);
	this->AddFrame(fActionButtons, groupGC->LH());
	fActionButtons->JustifyButton(kTextCenterX);
	((TGMrbButtonFrame *) fActionButtons)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	this->ChangeBackground(gVMEControlData->fColorGreen);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void VMESis3302Panel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case VMESis3302Panel::kVMESis3302ActionReset:
			this->ResetModule();
			break;
		case VMESis3302Panel::kVMESis3302ActionRun:
			new VMESis3302StartRunPanel(fClient->GetRoot(), &fLofModules, frameWidth*2/3, frameHeight);
			break;
		case VMESis3302Panel::kVMESis3302ActionSaveRestore:
			new VMESis3302SaveRestorePanel(fClient->GetRoot(), &fLofModules, frameWidth/4, frameHeight/3);
			break;
		case VMESis3302Panel::kVMESis3302ActionCopySettings:
			new VMESis3302CopyPanel(fClient->GetRoot(), &fLofModules, frameWidth*2/3, frameHeight/3);
			break;
	}
}

void VMESis3302Panel::UpdateGUI() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::UpdateGUI
// Purpose:        Update values
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Update GUI.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (curModule == NULL) {
		curModule = (TC2LSis3302 *) fLofModules.At(0);
		if (curModule == NULL) return;
		curModule->SetVerbose(gVMEControlData->IsVerbose());
		curModule->SetOffline(gVMEControlData->IsOffline());
	}

	fSelectModule->Select(curModule->GetIndex());
	fSelectChannel->Select(curChannel);

	TString moduleName = curModule->GetName();
	fSettingsFrame->SetTitle(Form("Settings for module %s", moduleName.Data()));

	fDacSettings->SetTitle(Form("DAC settings for module %s, chn %d", moduleName.Data(), curChannel));
	fTriggerFilter->SetTitle(Form("Trigger settings for module %s, chn %d", moduleName.Data(), curChannel));
	fEnergyFilter->SetTitle(Form("Energy settings for module %s, chn %d", moduleName.Data(), curChannel));
	fRawDataSampling->SetTitle(Form("Raw data sampling for module %s", moduleName.Data()));
	fEnergyDataSampling->SetTitle(Form("Energy data sampling for module %s", moduleName.Data()));

	Int_t clockSource = 0;
	if (curModule->GetClockSource(clockSource)) fClockSource->Select(clockSource);
	Int_t trig = 0;
	Bool_t trigFlag = kFALSE;
	if (curModule->GetInternalTrigger(trigFlag)) { if (trigFlag) trig |= kVMETrigModeIntern; }
	if (curModule->GetExternalTrigger(trigFlag)) { if (trigFlag) trig |= kVMETrigModeExtern; }
	fCommonTrigMode->SetState(trig);

	TArrayI dacVal(1);
	if (curModule->ReadDac(dacVal, curChannel)) fDacOffset->SetText(dacVal[0]);

	Int_t p, g;
	if (curModule->IsOffline()) {
		p = fTrigPeaking->GetText2Int();
		g = fTrigGap->GetText2Int();
	} else {
		curModule->ReadTrigPeakAndGap(p, g, curChannel);
	}
	if (p > 16) p = 16;
	if (p < 1) p = 1;
	if (g > 16) g = 16;
	if (g < 1) g = 1;
	fTrigPeaking->SetText(p);
	fTrigGap->SetText(g);
	curModule->WriteTrigPeakAndGap(p, g, curChannel);
	this->UpdateAdcCounts();

	Bool_t invTrig;
	curModule->GetPolarity(invTrig, curChannel);
	fTrigPol->Select(invTrig ? kVMETrigPolNeg : kVMETrigPolPos);

	Int_t tmode;
	curModule->GetTriggerMode(tmode, curChannel);
	fTrigMode->Select(tmode);

	Int_t olen;
	curModule->ReadTrigPulseLength(olen, curChannel);
	fTrigOut->SetText(olen);
	Bool_t gt, out;
	curModule->GetTriggerGT(gt, curChannel);
	curModule->GetTriggerOut(out, curChannel);
	if (gt) {
		fTrigCond->Select(out ? kVMETrigCondGTEna : kVMETrigCondGTDis);
	} else {
		fTrigCond->Select(out ? kVMETrigCondEna : kVMETrigCondDis);
	}

	Int_t delay;
	curModule->ReadPreTrigDelay(delay, curChannel);
	fPreTrigDelay->SetText(delay);

	curModule->ReadEnergyPeakAndGap(p, g, curChannel);
	if (p > 255) p = 255;
	if (p < 0) p = 0;
	if (g > 255) g = 255;
	if (g < 0) g = 0;
	fEnergyPeaking->SetText(p);
	fEnergyGap->SetText(g);
	curModule->WriteEnergyPeakAndGap(p, g, curChannel);

	Int_t decim;
	curModule->GetDecimation(decim, curChannel);
	fEnergyDecimation->Select(decim);

	Int_t tau;
	curModule->ReadTauFactor(tau, curChannel);
	fEnergyTauFactor->SetText(tau);

	this->UpdateDecayTime();

	Int_t rds;
	curModule->ReadRawDataStartIndex(rds, curChannel);
	fRawDataStart->SetText(rds);
	Int_t rdl;
	curModule->ReadRawDataSampleLength(rdl, curChannel);
	fRawDataLength->SetText(rdl);

	Int_t edl;
	curModule->ReadEnergySampleLength(edl, curChannel);
	fEnergyDataLength->SetText(edl);

	this->UpdateGates();
}

void VMESis3302Panel::ModuleChanged(Int_t FrameId, Int_t Selection) {
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

	curModule = (TC2LSis3302 *) fLofModules.FindByIndex(Selection);
	curModule->SetVerbose(gVMEControlData->IsVerbose());
	curModule->SetOffline(gVMEControlData->IsOffline());
	this->UpdateGUI();
}

void VMESis3302Panel::ChannelChanged(Int_t FrameId, Int_t Selection) {
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
	this->UpdateGUI();
}

void VMESis3302Panel::ClockSourceChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::ClockSourceChanged
// Purpose:        Slot method: clock source changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	curModule->SetClockSource(Selection);
	this->UpdateDecayTime();
}

void VMESis3302Panel::CommonTrigModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::CommonTrigModeChanged
// Purpose:        Slot method: trigger mode changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:     
// Description:    Called on TGMrbButtonFrame::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Bool_t itrig = (Selection & kVMETrigModeIntern);
	curModule->SetInternalTrigger(itrig);
	Bool_t xtrig = (Selection & kVMETrigModeExtern);
	curModule->SetExternalTrigger(xtrig);
}

void VMESis3302Panel::DacOffsetChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::DacOffsetChanged
// Purpose:        Slot method: DAC settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TArrayI dacVal(1);
	dacVal[0] = fDacOffset->GetText2Int(EntryNo);
	curModule->WriteDac(dacVal, curChannel);
}

void VMESis3302Panel::TrigModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigModeChanged
// Purpose:        Slot method: trigger mode changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	curModule->SetTriggerMode(Selection, curChannel);
}

void VMESis3302Panel::TrigPeakingChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigPeakingtChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t p, psav, g;
	if (curModule->IsOffline()) {
		psav = fTrigPeaking->GetText2Int();
		g = fTrigGap->GetText2Int();
	} else {
		curModule->ReadTrigPeakAndGap(psav, g, curChannel);
	}
	fTrigPeaking->SetRange(1, 16 - g);

	p = fTrigPeaking->GetText2Int(EntryNo);
	if (!fTrigPeaking->CheckRange(p, EntryNo, kTRUE, kTRUE)) {
		fTrigPeaking->SetText(psav, EntryNo);
	} else {
		curModule->WriteTrigPeakAndGap(p, g, curChannel);
		this->UpdateAdcCounts();
	}
}

void VMESis3302Panel::TrigGapChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigGapChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t p, g, gsav;
	if (curModule->IsOffline()) {
		p = fTrigPeaking->GetText2Int();
		gsav = fTrigGap->GetText2Int();
	} else {
		curModule->ReadTrigPeakAndGap(p, gsav, curChannel);
	}
	fTrigGap->SetRange(0, 16 - p);

	g = fTrigGap->GetText2Int(EntryNo);
	if (!fTrigGap->CheckRange(g, EntryNo, kTRUE, kTRUE)) {
		fTrigGap->SetText(gsav, EntryNo);
	} else {
		curModule->WriteTrigPeakAndGap(p, g, curChannel);
		this->UpdateAdcCounts();
	}
}

void VMESis3302Panel::TrigThreshChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigThreshChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t thsav;
	curModule->ReadTrigThreshold(thsav, curChannel);
	Int_t th = fTrigThresh->GetText2Int(EntryNo);

	if (fTrigThresh->CheckRange(th, EntryNo, kTRUE, kTRUE)) curModule->WriteTrigThreshold(th, curChannel);
	else													fTrigThresh->SetText(thsav, EntryNo);

	this->UpdateAdcCounts();
}

void VMESis3302Panel::TrigPolarChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigPolarChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Bool_t invTrig = kFALSE;
	switch (Selection) {
		case kVMETrigPolPos:	invTrig = kFALSE; break;
		case kVMETrigPolNeg:	invTrig = kTRUE; break;
	}
	curModule->SetPolarity(invTrig, curChannel);
}

void VMESis3302Panel::TrigCondChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigCondChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Bool_t gt = kFALSE;
	Bool_t out = kFALSE;
	switch (Selection) {
		case kVMETrigCondDis:	gt = kFALSE; out = kFALSE; break;
		case kVMETrigCondGTEna:	gt = kTRUE; out = kTRUE; break;
		case kVMETrigCondGTDis:	gt = kTRUE; out = kFALSE; break;
		case kVMETrigCondEna:	gt = kFALSE; out = kTRUE; break;
	}
	curModule->SetTriggerGT(gt, curChannel);
	curModule->SetTriggerOut(out, curChannel);
}

void VMESis3302Panel::TrigOutChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigOutChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t outsav;
	curModule->ReadTrigPulseLength(outsav, curChannel);
	Int_t out = fTrigOut->GetText2Int(EntryNo);

	if (fTrigOut->CheckRange(out, EntryNo, kTRUE, kTRUE))	curModule->WriteTrigPulseLength(out, curChannel);
	else													fTrigOut->SetText(outsav, EntryNo);
}

void VMESis3302Panel::TrigDelayChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigDelayChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t delsav;
	curModule->ReadPreTrigDelay(delsav, curChannel);
	Int_t delay = fPreTrigDelay->GetText2Int(EntryNo);

	if (fPreTrigDelay->CheckRange(delay, EntryNo, kTRUE, kTRUE))	curModule->WritePreTrigDelay(delay, curChannel);
	else															fPreTrigDelay->SetText(delsav, EntryNo);
	this->UpdateGates();
}

void VMESis3302Panel::EnergyPeakingChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyPeakingChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t p, psav, g;
	curModule->ReadEnergyPeakAndGap(psav, g, curChannel);

	p = fEnergyPeaking->GetText2Int(EntryNo);
	if (!fEnergyPeaking->CheckRange(p, EntryNo, kTRUE, kTRUE)) {
		fEnergyPeaking->SetText(psav, EntryNo);
	} else {
		curModule->WriteEnergyPeakAndGap(p, g, curChannel);
	}
	this->UpdateGates();
}

void VMESis3302Panel::EnergyGapChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyGapChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t p, g, gsav;
	curModule->ReadEnergyPeakAndGap(p, gsav, curChannel);

	g = fEnergyGap->GetText2Int(EntryNo);
	if (!fEnergyGap->CheckRange(g, EntryNo, kTRUE, kTRUE)) {
		fEnergyGap->SetText(gsav, EntryNo);
	} else {
		curModule->WriteEnergyPeakAndGap(p, g, curChannel);
	}
	this->UpdateGates();
}

void VMESis3302Panel::DecimationChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::DecimationChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	curModule->SetDecimation(Selection, curChannel);
	this->UpdateDecayTime();
	this->UpdateGates();
}

void VMESis3302Panel::TauFactorChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TauFactorChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t tau, tausav;
	curModule->ReadTauFactor(tausav, curChannel);

	tau = fEnergyTauFactor->GetText2Int(EntryNo);
	if (!fEnergyTauFactor->CheckRange(tau, EntryNo, kTRUE, kTRUE)) {
		fEnergyTauFactor->SetText(tausav, EntryNo);
	} else {
		curModule->WriteTauFactor(tau, curChannel);
	}
	this->UpdateDecayTime();
}

void VMESis3302Panel::RawDataStartChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::RawDataStartChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t rds, rdssav;
	curModule->ReadRawDataStartIndex(rdssav, curChannel);

	rds = fRawDataStart->GetText2Int(EntryNo);
	if (!fRawDataStart->CheckRange(rds, EntryNo, kTRUE, kTRUE)) {
		fRawDataStart->SetText(rdssav, EntryNo);
	} else if (rds & 1) {
		gVMEControlData->MsgBox(this, "RawDataStartChanged", "Error", Form("Illegal start index - %d (has to be even)", rds));
		fRawDataStart->SetText(rdssav, EntryNo);
	} else {
		curModule->WriteRawDataStartIndex(rds);
	}
	this->UpdateGates();
}

void VMESis3302Panel::RawDataLengthChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::RawDataLengthChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t rdl, rdlsav;
	curModule->ReadRawDataSampleLength(rdlsav, curChannel);

	rdl = fRawDataLength->GetText2Int(EntryNo);
	if (!fRawDataLength->CheckRange(rdl, EntryNo, kTRUE, kTRUE)) {
		fRawDataLength->SetText(rdlsav, EntryNo);
	} else if (rdl % 4) {
		gVMEControlData->MsgBox(this, "RawDataLengthChanged", "Error", Form("Illegal sampling length - %d (has to be mod 4)", rdl));
		fRawDataLength->SetText(rdlsav, EntryNo);
	} else {
		curModule->WriteRawDataSampleLength(rdl);
	}
	this->UpdateGates();
}

void VMESis3302Panel::EnergyDataModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyDataModeChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t val;
	switch (Selection) {
		case kVMESampleFull:	val = 512; break;
		case kVMESampleMinMax:	val = 0; break;
		default:				val = 0; break;
	}
	fEnergyDataLength->SetText(val);
	curModule->WriteEnergySampleLength(val);
	this->UpdateGates();
}

void VMESis3302Panel::EnergyDataStart1Changed(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyDataStart1Changed
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->EnergyDataStartOrLengthChanged(0, fEnergyDataStart1, EntryNo);
}

void VMESis3302Panel::EnergyDataStart2Changed(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyDataStart2Changed
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->EnergyDataStartOrLengthChanged(1, fEnergyDataStart2, EntryNo);
}

void VMESis3302Panel::EnergyDataStart3Changed(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyDataStart3Changed
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->EnergyDataStartOrLengthChanged(2, fEnergyDataStart3, EntryNo);
}

void VMESis3302Panel::EnergyDataLengthChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyDataLengthChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	this->EnergyDataStartOrLengthChanged(-1, fEnergyDataLength, EntryNo);
}

void VMESis3302Panel::EnergyDataStartOrLengthChanged(Int_t IdxNo, TGMrbLabelEntry * Entry, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyDataStartOrLengthChanged
// Purpose:        Slot method: trigger settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:     
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (IdxNo >= 0) {
		Int_t eds, edssav;
		curModule->ReadStartIndex(edssav, IdxNo, curChannel);

		eds = Entry->GetText2Int(EntryNo);
		if (!Entry->CheckRange(eds, EntryNo, kTRUE, kTRUE)) {
			Entry->SetText(edssav, EntryNo);
		} else {
			curModule->WriteStartIndex(eds, IdxNo);
		}
	}

	Int_t edl, edlsav;
	curModule->ReadEnergySampleLength(edlsav, curChannel);

	Int_t nofStarts = 0;
	if (curModule->IsOffline()) {
		Int_t start = fEnergyDataStart1->GetText2Int(); if (start > 0) nofStarts++;
		start = fEnergyDataStart2->GetText2Int(); if (start > 0) nofStarts++;
		start = fEnergyDataStart3->GetText2Int(); if (start > 0) nofStarts++;
	} else {
		for (Int_t i = 0; i < 3; i++) {
			Int_t start = 0;
			curModule->ReadStartIndex(start, i, curChannel);
			if (start > 0) nofStarts++;
		}
	}

	edl = fEnergyDataLength->GetText2Int(EntryNo);
	if (!fEnergyDataLength->CheckRange(edl, EntryNo, kTRUE, kTRUE)) {
		fEnergyDataLength->SetText(edlsav, EntryNo);
	} else if (edl & 1) {
		gVMEControlData->MsgBox(this, "EnergyDataStartOrLengthChanged", "Error", Form("Illegal sampling length - %d (has to be even)", edl));
		fEnergyDataLength->SetText(edlsav, EntryNo);
	} else if (edl * nofStarts > 512) {
		gVMEControlData->MsgBox(this, "EnergyDataStartOrLengthChanged", "Error", Form("Wrong sampling length - %d * %d (exceeds 512)", nofStarts, edl));
		Int_t dl = 512 / nofStarts;
		if (dl & 1) dl--;
		fEnergyDataLength->SetText(dl, EntryNo);
	} else {
		curModule->WriteEnergySampleLength(edl);
	}
	this->UpdateGates();
}

void VMESis3302Panel::UpdateAdcCounts() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigGapChanged
// Purpose:        Calculate adc counts from peak and gap values
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Called after peaking, gap or thresh values have changed
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t p, g, th;

	if (curModule->IsOffline()) {
		p = fTrigPeaking->GetText2Int();
		g = fTrigGap->GetText2Int();
		th = fTrigThresh->GetText2Int();
	} else {
		curModule->ReadTrigPeakAndGap(p, g, curChannel);
		curModule->ReadTrigThreshold(th, curChannel);
	}
	Int_t c = th * 16 / p;
	fTrigSumG->SetText(p + g);
	fTrigCounts->SetText(c);
}

void VMESis3302Panel::UpdateDecayTime() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::UpdateDecayTime
// Purpose:        Calculate decay time from tau factor
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Called after tau factor, clock, or decimation values have changed
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t cs[]	=	{	100000, 50000,	25000,	10000,	1000	};

	Int_t clockSource = 0;
	if (curModule->IsOffline()) {
		clockSource = fClockSource->GetSelectedNx()->GetIndex();
	} else {
		curModule->GetClockSource(clockSource);
	}
	if (clockSource > kVMEClockSource1) {
		fEnergyDecayTime->SetText("n/a");
		return;
	}

	Int_t decim;
	if (curModule->IsOffline()) {
		decim = fEnergyDecimation->GetSelectedNx()->GetIndex();
	} else {
		curModule->GetDecimation(decim, curChannel);
	}
	Int_t tau;
	if (curModule->IsOffline()) {
		tau = fEnergyTauFactor->GetText2Int();
	} else {
		curModule->ReadTauFactor(tau, curChannel);
	}

	Double_t sampling;
	switch (decim) {
		case kVMEDecimDis:	sampling = 1000.; break;
		case kVMEDecim2:	sampling = 2000.; break;
		case kVMEDecim4:	sampling = 4000.; break;
		case kVMEDecim8:	sampling = 8000.; break;
	}

	sampling /= cs[clockSource];

	Double_t dblTau = (Double_t) tau / 32768.0;

	Double_t decayTime = -1.0 * sampling / log(1.0 - dblTau);
	fEnergyDecayTime->SetText(decayTime);
}

void VMESis3302Panel::UpdateGates() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::UpdateEnergyWindows
// Purpose:        Calculate energy windows
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Called after peaking, gap or decimation values have changed
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t edl;
	Int_t val;
	Int_t mode;
	if (curModule->IsOffline()) {
		mode = fEnergyDataMode->GetSelectedNx()->GetIndex();
		switch (mode) {
			case kVMESampleFull: edl = 512; break;
			case kVMESampleMinMax: edl = 0; break;
		}
	} else {
		curModule->ReadEnergySampleLength(edl, curChannel);
	}
	if (edl == 512) {
		mode = kVMESampleFull;
		fEnergyDataMode->Select(mode);
		val = 1; fEnergyDataStart1->SetText(val); curModule->WriteStartIndex(val, 0);
		val = 0; fEnergyDataStart2->SetText(val); curModule->WriteStartIndex(val, 1);
		val = 0; fEnergyDataStart3->SetText(val); curModule->WriteStartIndex(val, 2);
		fEnergyDataStart1->SetState(kFALSE);
		fEnergyDataStart2->SetState(kFALSE);
		fEnergyDataStart3->SetState(kFALSE);
		fEnergyDataLength->SetState(kFALSE);
	} else if (edl == 0) {
		mode = kVMESampleMinMax;
		fEnergyDataMode->Select(mode);
		val = 1; fEnergyDataStart1->SetText(val); curModule->WriteStartIndex(val, 0);
		val = 0; fEnergyDataStart2->SetText(val); curModule->WriteStartIndex(val, 1);
		val = 0; fEnergyDataStart3->SetText(val); curModule->WriteStartIndex(val, 2);
		fEnergyDataStart1->SetState(kFALSE);
		fEnergyDataStart2->SetState(kFALSE);
		fEnergyDataStart3->SetState(kFALSE);
		fEnergyDataLength->SetState(kFALSE);
	} else {
		mode = kVMESampleProg;
		fEnergyDataMode->Select(mode);
		fEnergyDataStart1->SetState(kTRUE);
		fEnergyDataStart2->SetState(kTRUE);
		fEnergyDataStart3->SetState(kTRUE);
		fEnergyDataLength->SetState(kTRUE);
	}

	Int_t preTrigDel;
	if (curModule->IsOffline()) {
		preTrigDel = fPreTrigDelay->GetText2Int();
	} else {
		curModule->ReadPreTrigDelay(preTrigDel, curChannel);
	}

	Int_t decim;
	if (curModule->IsOffline()) {
		decim = fEnergyDecimation->GetSelectedNx()->GetIndex();
	} else {
		curModule->GetDecimation(decim, curChannel);
	}

	Int_t delay;
	switch (decim) {
		case kVMEDecimDis:	delay = preTrigDel; break;
		case kVMEDecim2:	delay = preTrigDel / 2; break;
		case kVMEDecim4:	delay = preTrigDel / 4; break;
		case kVMEDecim8:	delay = preTrigDel / 8; break;
	}

	Int_t peak, gap;
	if (curModule->IsOffline()) {
		peak = fEnergyPeaking->GetText2Int();
		gap = fEnergyGap->GetText2Int();
	} else {
		curModule->ReadEnergyPeakAndGap(peak, gap, curChannel);
	}

	Int_t egate;
	if (mode == kVMESampleMinMax) {
		egate = delay + 2 * peak + gap + 20;
	} else {
		egate = delay + 600;
	}
	fEnergyGate->SetText(egate);
	curModule->WriteEnergyGateLength(egate, curChannel);

	Int_t rds, rdl;
	if (curModule->IsOffline()) {
		rds = fRawDataStart->GetText2Int();
		rdl = fRawDataLength->GetText2Int();
	} else {
		curModule->ReadRawDataStartIndex(rds, curChannel);
		curModule->ReadRawDataSampleLength(rdl, curChannel);
	}

	Int_t tgate, tgate2;
	tgate = delay + 2 * peak + gap;
	tgate2 = rdl + rds;
	if (tgate2 > tgate) tgate = tgate2;
	tgate += 16;
	fTrigGate->SetText(tgate);
	curModule->WriteTrigGateLength(tgate, curChannel);
}

void VMESis3302Panel::ResetModule() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::ResetModule
// Purpose:        Reset module to power-up settings
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Reset
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	curModule->KeyAddr(kSis3302KeyReset);
	this->UpdateGUI();
}

Bool_t VMESis3302Panel::SetupModuleList() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::SetupModuleList
// Purpose:        Fill list of modules
// Arguments:      -- 
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gVMEControlData->IsOffline() || gMrbC2Lynx) {
		if (gVMEControlData->SetupModuleList(fLofModules, "TMrbSis_3302")) return(kTRUE);
		if (this->GetNofModules() == 0) {
			gVMEControlData->MsgBox(this, "SetupModuleList", "No modules", "No SIS 3302 modules found");
			return(kFALSE);
		}
	} else {
		gVMEControlData->MsgBox(this, "SetupModuleList", "Not connected", "No connection to LynxOs server");
		return(kFALSE);
	}
	return(kFALSE);
}
