//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel
// Purpose:        A GUI to control vme modules via tcp
// Description:    Connect to server
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMESis3302Panel.cxx,v 1.8 2010-03-23 14:07:51 Rudolf.Lutter Exp $
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
				{VMESis3302Panel::kVMEClockSource100MHzA,		"100 MHz"		},
				{VMESis3302Panel::kVMEClockSource50MHz,			"50 MHz"		},
				{VMESis3302Panel::kVMEClockSource25MHz,			"25 MHz"		},
				{VMESis3302Panel::kVMEClockSource10MHz,			"10 MHz"		},
				{VMESis3302Panel::kVMEClockSource1MHz,			"1 MHz"			},
				{VMESis3302Panel::kVMEClockSource100kHz, 		"100 kHz"		},
				{VMESis3302Panel::kVMEClockSourceExt,			"extern"		},
				{VMESis3302Panel::kVMEClockSource100MHzB,		"2nd 100 MHz"	},
				{0, 											NULL			}
			};

const SMrbNamedX kVMELemoOutMode[] =
			{
				{0, "0", ""},
				{1, "1", ""},
				{2, "2", ""},
				{3, "3", ""},
				{0, NULL, NULL}
			};

const SMrbNamedX kVMELemoInMode[] =
			{
				{0, "0", ""},
				{1, "1", ""},
				{2, "2", ""},
				{3, "3", ""},
				{4, "4", ""},
				{5, "5", ""},
				{6, "6", ""},
				{7, "7", ""},
				{0, NULL, NULL}
			};

const SMrbNamedX kVMETrigPolar[] =
			{
				{VMESis3302Panel::kVMETrigPolPos,			"positive", "trigger polarity positive"	},
				{VMESis3302Panel::kVMETrigPolNeg,			"negative", "trigger polarity negative"	},
				{0, 												NULL			}
			};

const SMrbNamedX kVMETrigCond[] =
			{
				{VMESis3302Panel::kVMETrigCondDis,			"disabled",			"trigger out disabled"	},
				{VMESis3302Panel::kVMETrigCondEnaGT,		"enabled if GT",	"trigger out if greater than treshold"	},
				{VMESis3302Panel::kVMETrigCondEna,			"enabled",			"trigger out enabled"	},
				{0, 										NULL,				NULL			}
			};

const SMrbNamedX kVMETrigMode[] =
			{
				{VMESis3302Panel::kVMETrigGateModeDis,		"disabled",			"no trigger"	},
				{VMESis3302Panel::kVMETrigGateModeIntern,	"intern",			"internal/async trigger: FIR trigger"	},
				{VMESis3302Panel::kVMETrigGateModeExtern, 	"extern",			"external/sync trigger: via LEMO-3 or VME KEY"	},
				{VMESis3302Panel::kVMETrigGateModeBoth, 	"intern|extern",	"OR of intern or extern triggers"	},
				{0, 										NULL,				NULL			}
			};

const SMrbNamedX kVMEGateMode[] =
			{
				{VMESis3302Panel::kVMETrigGateModeDis,		"disabled",			"no gate"	},
				{VMESis3302Panel::kVMETrigGateModeIntern,	"intern",			"internal gate"	},
				{VMESis3302Panel::kVMETrigGateModeExtern, 	"extern",			"external gate"	},
				{VMESis3302Panel::kVMETrigGateModeBoth,		"intern|extern",	"gate intern or extern"	},
				{0, 										NULL,				NULL			}
			};

const SMrbNamedX kVMENextNeighborTrigMode[] =
			{
				{VMESis3302Panel::kVMENextNeighborModeDis,		"disabled",			"no trigger"	},
				{VMESis3302Panel::kVMENextNeighborModeR,		"neighbor N+1",		"trigger channel N+1"	},
				{VMESis3302Panel::kVMENextNeighborModeL,	 	"neighbor N-1",		"trigger channel N-1"	},
				{VMESis3302Panel::kVMENextNeighborModeBoth,		"both neighbors",	"trigger N+1 or N-1"	},
				{0, 										NULL,				NULL			}
			};

const SMrbNamedX kVMENextNeighborGateMode[] =
			{
				{VMESis3302Panel::kVMENextNeighborModeDis,		"disabled",			"no gate"	},
				{VMESis3302Panel::kVMENextNeighborModeR,		"neighbor N+1",		"gate channel N+1"	},
				{VMESis3302Panel::kVMENextNeighborModeL,	 	"neighbor N-1",		"gate channel N-1"	},
				{VMESis3302Panel::kVMENextNeighborModeBoth,		"both neighbors",	"gate N+1 or N-1"	},
				{0, 										NULL,				NULL			}
			};

const SMrbNamedX kVMEDecim[] =
			{
				{VMESis3302Panel::kVMEDecimDis,				"disabled", "no decimation"	},
				{VMESis3302Panel::kVMEDecim2,				"2",		"decimation = 2"	},
				{VMESis3302Panel::kVMEDecim4,				"4",		"decimation = 4"	},
				{VMESis3302Panel::kVMEDecim8,				"8",		"decimation = 8"	},
				{0, 										NULL,				NULL			}
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

TMrbLofNamedX lofChannels;
TMrbLofNamedX lofDecims;
TMrbLofNamedX lofClockSources;
TMrbLofNamedX lofLemoInModes;
TMrbLofNamedX lofLemoOutModes;
TMrbLofNamedX lofTrigModes;
TMrbLofNamedX lofGateModes;
TMrbLofNamedX lofNextNeighborTrigModes;
TMrbLofNamedX lofNextNeighborGateModes;
TMrbLofNamedX lofTrigPols;
TMrbLofNamedX lofTrigConds;
TMrbLofNamedX lofSamplingModes;

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

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

//	Clear focus list
	fFocusList.Clear();

	TGMrbLayout * frameGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGreen);	HEAP(frameGC);

	TGMrbLayout * groupGC = new TGMrbLayout(	gVMEControlData->SlantedFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGreen);	HEAP(groupGC);

	TGMrbLayout * comboGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGreen);	HEAP(comboGC);

	TGMrbLayout * labelGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGreen);	HEAP(labelGC);

	TGMrbLayout * buttonGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGray);	HEAP(buttonGC);

	TGMrbLayout * infoGC = new TGMrbLayout(		gVMEControlData->BoldFont(),
												gVMEControlData->fColorBlack,
												gVMEControlData->fColorGray);	HEAP(infoGC);

	TGMrbLayout * entryGC = new TGMrbLayout(	gVMEControlData->NormalFont(),
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

//	initialize several lists
	fActions.SetName("Actions");
	fActions.AddNamedX(kVMEActions);

//	fill lists
	lofDecims.AddNamedX(kVMEDecim);
	lofClockSources.AddNamedX(kVMEClockSource);
	lofLemoOutModes.AddNamedX(kVMELemoOutMode); lofLemoOutModes.SetPatternMode();
	lofLemoInModes.AddNamedX(kVMELemoInMode); lofLemoInModes.SetPatternMode();
	lofTrigModes.AddNamedX(kVMETrigMode); lofTrigModes.SetPatternMode();
	lofGateModes.AddNamedX(kVMEGateMode); lofGateModes.SetPatternMode();
	lofNextNeighborTrigModes.AddNamedX(kVMENextNeighborTrigMode); lofNextNeighborTrigModes.SetPatternMode();
	lofNextNeighborGateModes.AddNamedX(kVMENextNeighborGateMode); lofNextNeighborGateModes.SetPatternMode();
	lofTrigPols.AddNamedX(kVMETrigPolar); lofTrigPols.SetPatternMode();
	lofTrigConds.AddNamedX(kVMETrigCond); lofTrigConds.SetPatternMode();
	lofSamplingModes.AddNamedX(kVMESample);

	for (Int_t i = 0; i < VMESis3302Panel::kVMENofSis3302Chans; i++) lofChannels.AddNamedX(i, Form("chn %d", i));

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

	fSelectChannel = new TGMrbLabelCombo(fSelectFrame, "Channel",	&lofChannels,
																kVMESis3302SelectChannel, 1,
																frameWidth/5, kLEHeight, frameWidth/8,
																frameGC, labelGC, comboGC, labelGC);
	fSelectFrame->AddFrame(fSelectChannel, frameGC->LH());
	fSelectChannel->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ChannelChanged(Int_t, Int_t)");

	fModuleInfo = new TGMrbLabelEntry(fSelectFrame, "Firmware", 200, -1, frameWidth/5, kLEHeight, frameWidth/30, frameGC, labelGC, infoGC);
	fSelectFrame->AddFrame(fModuleInfo, frameGC->LH());
	fModuleInfo->SetState(kFALSE);

	fSettingsFrame = new TGGroupFrame(this, "Module settings", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSettingsFrame);
	this->AddFrame(fSettingsFrame, groupGC->LH());

	TGLayoutHints * lh = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);
	HEAP(lh);

	TGHorizontalFrame * h1 = new TGHorizontalFrame(fSettingsFrame);
	HEAP(h1);
	fSettingsFrame->AddFrame(h1, lh);
	h1->ChangeBackground(gVMEControlData->fColorGreen);

	fClockLemo = new TGGroupFrame(h1, "Clock & LEMO", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fClockLemo);
	h1->AddFrame(fClockLemo, groupGC->LH());

	fClockSource = new TGMrbLabelCombo(fClockLemo, "Clock",		&lofClockSources,
																		kVMESis3302ClockSource, 1,
																		frameWidth/5, kLEHeight, frameWidth/15,
																		frameGC, labelGC, comboGC);
	HEAP(fClockSource);
	fClockLemo->AddFrame(fClockSource, frameGC->LH());
	fClockSource->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "ClockSourceChanged(Int_t, Int_t)");

	fLemoInMode = new TGMrbLabelCombo(fClockLemo, "LemoIn",			&lofLemoInModes,
																		kVMESis3302LemoInMode, 1,
																		frameWidth/5, kLEHeight, frameWidth/30,
																		frameGC, labelGC, comboGC);
	HEAP(fLemoInMode);
	fClockLemo->AddFrame(fLemoInMode, frameGC->LH());
	fLemoInMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "LemoInModeChanged(Int_t, Int_t)");

	fLemoOutMode = new TGMrbLabelCombo(fClockLemo, "LemoOut",			&lofLemoOutModes,
																		kVMESis3302LemoOutMode, 1,
																		frameWidth/5, kLEHeight, frameWidth/30,
																		frameGC, labelGC, comboGC);
	HEAP(fLemoOutMode);
	fClockLemo->AddFrame(fLemoOutMode, frameGC->LH());
	fLemoOutMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "LemoOutModeChanged(Int_t, Int_t)");

	fLemoInEnableMask = new TGMrbLabelEntry(fClockLemo, "Mask",	200, kVMESis3302LemoInEnableMask,
																		frameWidth/5, kLEHeight, frameWidth/30,
																		frameGC, labelGC, entryGC);
	HEAP(fLemoInEnableMask);
	fLemoInEnableMask->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fLemoInEnableMask->SetText(0);
	fLemoInEnableMask->SetRange(0, 7);
	fLemoInEnableMask->ShowToolTip(kTRUE, kTRUE);
	fClockLemo->AddFrame(fLemoInEnableMask, frameGC->LH());
	fLemoInEnableMask->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "LemoInEnableMaskChanged(Int_t, Int_t)");

	fDacSettings = new TGGroupFrame(h1, "DAC settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fDacSettings);
	h1->AddFrame(fDacSettings, groupGC->LH());

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

	fTriggerSettings = new TGGroupFrame(h2, "Trigger settings", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fTriggerSettings);
	h2->AddFrame(fTriggerSettings, groupGC->LH());

	TGVerticalFrame * tvl = new TGVerticalFrame(fTriggerSettings);
	HEAP(tvl);
	fTriggerSettings->AddFrame(tvl, lh);
	tvl->ChangeBackground(gVMEControlData->fColorGreen);

	TGVerticalFrame * tvr = new TGVerticalFrame(fTriggerSettings);
	HEAP(tvr);
	fTriggerSettings->AddFrame(tvr, lh);
	tvr->ChangeBackground(gVMEControlData->fColorGreen);

	fTrigMode = new TGMrbLabelCombo(tvl, "Trigger mode", 	&lofTrigModes,
															kVMESis3302TrigMode, 1,
															frameWidth/10, kLEHeight, frameWidth/10,
															frameGC, labelGC, comboGC);
	HEAP(fTrigMode);
	tvl->AddFrame(fTrigMode, groupGC->LH());
	fTrigMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TrigModeChanged(Int_t, Int_t)");

	fNextNeighborTrigMode = new TGMrbLabelCombo(tvl, "NN trigger", 	&lofNextNeighborTrigModes,
															kVMESis3302NNTrigMode, 1,
															frameWidth/10, kLEHeight, frameWidth/10,
															frameGC, labelGC, comboGC);
	HEAP(fNextNeighborTrigMode);
	tvl->AddFrame(fNextNeighborTrigMode, groupGC->LH());
	fNextNeighborTrigMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "NextNeighborTrigModeChanged(Int_t, Int_t)");

	fTrigInternalDelay = new TGMrbLabelEntry(tvl, "Internal delay",	200, kVMESis3302TrigIntDelay,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigInternalDelay);
	fTrigInternalDelay->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigInternalDelay->SetText(0);
	fTrigInternalDelay->SetRange(kSis3302TrigIntDelayMin, kSis3302TrigIntDelayMax);
	fTrigInternalDelay->SetIncrement(1);
	fTrigInternalDelay->ShowToolTip(kTRUE, kTRUE);
	tvl->AddFrame(fTrigInternalDelay, groupGC->LH());
	fTrigInternalDelay->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigInternalDelayChanged(Int_t, Int_t)");

	fTrigPol = new TGMrbLabelCombo(tvl, "Polarity", 	&lofTrigPols,
														kVMESis3302TrigPol, 1,
														frameWidth/10, kLEHeight, frameWidth/10,
														frameGC, labelGC, comboGC);
	HEAP(fTrigPol);
	tvl->AddFrame(fTrigPol, groupGC->LH());
	fTrigPol->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TrigPolarChanged(Int_t, Int_t)");

	fTrigPeaking = new TGMrbLabelEntry(tvl, "Peaking",	200, kVMESis3302TrigPeaking,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigPeaking);
	fTrigPeaking->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigPeaking->SetText(1);
	fTrigPeaking->SetRange(kSis3302TrigPeakMin, kSis3302TrigPeakMax);
	fTrigPeaking->SetIncrement(1);
	fTrigPeaking->ShowToolTip(kTRUE, kTRUE);
	tvl->AddFrame(fTrigPeaking, groupGC->LH());
	fTrigPeaking->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigPeakingChanged(Int_t, Int_t)");

	fTrigGap = new TGMrbLabelEntry(tvl, "Gap",			200, kVMESis3302TrigGap,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigGap);
	fTrigGap->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigGap->SetText(1);
	fTrigGap->SetRange(kSis3302TrigGapMin, kSis3302TrigGapMax);
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
	fTrigThresh->SetRange(kSis3302TrigThreshMin, kSis3302TrigThreshMax);
	fTrigThresh->SetIncrement(10);
	fTrigThresh->ShowToolTip(kTRUE, kTRUE);
	tvl->AddFrame(fTrigThresh, groupGC->LH());
	fTrigThresh->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigThreshChanged(Int_t, Int_t)");

	fTrigOut = new TGMrbLabelEntry(tvl, "Trigger out",	200, kVMESis3302TrigOut,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigOut);
	fTrigOut->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigOut->SetText(0);
	fTrigOut->SetRange(kSis3302TrigPulseMin, kSis3302TrigPulseMax);
	fTrigOut->SetIncrement(10);
	fTrigOut->ShowToolTip(kTRUE, kTRUE);
	tvl->AddFrame(fTrigOut, groupGC->LH());
	fTrigOut->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigOutChanged(Int_t, Int_t)");

	fGateMode = new TGMrbLabelCombo(tvr, "Gate mode", 		&lofGateModes,
															kVMESis3302GateMode, 1,
															frameWidth/10, kLEHeight, frameWidth/10,
															frameGC, labelGC, comboGC);
	HEAP(fGateMode);
	tvr->AddFrame(fGateMode, groupGC->LH());
	fGateMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "GateModeChanged(Int_t, Int_t)");

	fNextNeighborGateMode = new TGMrbLabelCombo(tvr, "NN gate", 		&lofNextNeighborGateModes,
															kVMESis3302NNGateMode, 1,
															frameWidth/10, kLEHeight, frameWidth/10,
															frameGC, labelGC, comboGC);
	HEAP(fNextNeighborGateMode);
	tvr->AddFrame(fNextNeighborGateMode, groupGC->LH());
	fNextNeighborGateMode->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "NextNeighborGateModeChanged(Int_t, Int_t)");

	fTrigInternalGate = new TGMrbLabelEntry(tvr, "Internal gate",	200, kVMESis3302TrigIntGate,
														frameWidth/5, kLEHeight, frameWidth/10,
														frameGC, labelGC, entryGC, buttonGC);
	HEAP(fTrigInternalGate);
	fTrigInternalGate->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigInternalGate->SetText(0);
	fTrigInternalGate->SetRange(kSis3302TrigIntGateMin, kSis3302TrigIntGateMax);
	fTrigInternalGate->SetIncrement(1);
	fTrigInternalGate->ShowToolTip(kTRUE, kTRUE);
	tvr->AddFrame(fTrigInternalGate, groupGC->LH());
	fTrigInternalGate->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TrigInternalGateChanged(Int_t, Int_t)");

	fTrigDecimation = new TGMrbLabelCombo(tvr, "Decimation", 	&lofDecims,
																kVMESis3302TrigDecimation, 1,
																frameWidth/10, kLEHeight, frameWidth/10,
																frameGC, labelGC, comboGC);
	HEAP(fTrigDecimation);
	tvr->AddFrame(fTrigDecimation, groupGC->LH());
	fTrigDecimation->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TrigDecimationChanged(Int_t, Int_t)");

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

	fTrigCond = new TGMrbLabelCombo(tvr, "Condition", 	&lofTrigConds,
														kVMESis3302TrigCond, 1,
														frameWidth/10, kLEHeight, frameWidth/10,
														frameGC, labelGC, comboGC);
	tvr->AddFrame(fTrigCond, groupGC->LH());
	fTrigCond->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "TrigCondChanged(Int_t, Int_t)");

	fEnergySettings = new TGGroupFrame(h2, "Energy filter", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fEnergySettings);
	h2->AddFrame(fEnergySettings, groupGC->LH());

	TGVerticalFrame * evl = new TGVerticalFrame(fEnergySettings);
	HEAP(evl);
	fEnergySettings->AddFrame(evl, lh);
	evl->ChangeBackground(gVMEControlData->fColorGreen);

	TGVerticalFrame * evr = new TGVerticalFrame(fEnergySettings);
	HEAP(evr);
	fEnergySettings->AddFrame(evr, lh);
	evr->ChangeBackground(gVMEControlData->fColorGreen);

	fEnergyPeaking = new TGMrbLabelEntry(evl, "Peaking",	200, kVMESis3302EnergyPeaking,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyPeaking);
	fEnergyPeaking->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyPeaking->SetText(0);
	fEnergyPeaking->SetRange(kSis3302EnergyPeakMin, kSis3302EnergyPeakMax);
	fEnergyPeaking->SetIncrement(10);
	fEnergyPeaking->ShowToolTip(kTRUE, kTRUE);
	evl->AddFrame(fEnergyPeaking, groupGC->LH());
	fEnergyPeaking->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyPeakingChanged(Int_t, Int_t)");

	fEnergyDecimation = new TGMrbLabelCombo(evr, "Decimation", 	&lofDecims,
																kVMESis3302EnergyDecimation, 1,
																frameWidth/10, kLEHeight, frameWidth/10,
																frameGC, labelGC, comboGC);
	evr->AddFrame(fEnergyDecimation, groupGC->LH());
	fEnergyDecimation->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "EnergyDecimationChanged(Int_t, Int_t)");

	fEnergyGap = new TGMrbLabelEntry(evl, "Gap",			200, kVMESis3302EnergyGap,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC, buttonGC);
	HEAP(fEnergyGap);
	fEnergyGap->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyGap->SetText(0);
	fEnergyGap->SetRange(kSis3302EnergyGapMin, kSis3302EnergyGapMax);
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
	fPreTrigDelay->SetRange(kSis3302PreTrigDelayMin, kSis3302PreTrigDelayMax);
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
	fEnergyTauFactor->SetRange(kSis3302EnergyTauMin, kSis3302EnergyTauMax);
	fEnergyTauFactor->SetIncrement(1);
	fEnergyTauFactor->ShowToolTip(kTRUE, kTRUE);
	evl->AddFrame(fEnergyTauFactor, groupGC->LH());
	fEnergyTauFactor->Connect("EntryChanged(Int_t, Int_t)", this->ClassName(), this, "TauFactorChanged(Int_t, Int_t)");

	fEnergyDecayTime = new TGMrbLabelEntry(evr, "Decay time",	200, kVMESis3302EnergyDecayTime,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC);
	HEAP(fEnergyDecayTime);
	fEnergyDecayTime->SetType(TGMrbLabelEntry::kGMrbEntryTypeDouble);
	evr->AddFrame(fEnergyDecayTime, groupGC->LH());
	fEnergyDecayTime->SetState(kFALSE);

	fEnergyGateLength = new TGMrbLabelEntry(evr, "Energy gate",	200, kVMESis3302EnergyGate,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC);
	HEAP(fEnergyGateLength);
	evr->AddFrame(fEnergyGateLength, groupGC->LH());
	fEnergyGateLength->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fEnergyGateLength->SetTextAlignment(kTextRight);
	fEnergyGateLength->SetState(kFALSE);

	fTrigGateLength = new TGMrbLabelEntry(evr, "Trig gate",		200, kVMESis3302TrigGate,
															frameWidth/5, kLEHeight, frameWidth/10,
															frameGC, labelGC, entryGC);
	HEAP(fTrigGateLength);
	evr->AddFrame(fTrigGateLength, groupGC->LH());
	fTrigGateLength->SetType(TGMrbLabelEntry::kGMrbEntryTypeInt);
	fTrigGateLength->SetTextAlignment(kTextRight);
	fTrigGateLength->SetState(kFALSE);

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
	fRawDataStart->SetRange(kSis3302RawDataStartIndexMin, kSis3302RawDataStartIndexMax);
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
	fRawDataLength->SetRange(kSis3302RawDataSampleLengthMin, kSis3302RawDataSampleLengthMax);
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
	fEnergyDataLength->SetText(kSis3302EnergySampleLengthMin);
	fEnergyDataLength->SetRange(0, kSis3302EnergySampleLengthMax);
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

	Int_t boardId, major, minor;
	curModule->GetModuleInfo(boardId, major, minor);
	curModule->SetFirmwareVersion(major, minor);
	fModuleInfo->SetText(Form("%x", curModule->GetFirmwareVersion()));

	TString moduleName = curModule->GetName();
	fSettingsFrame->SetTitle(Form("Settings for module %s", moduleName.Data()));

	fDacSettings->SetTitle(Form("DAC settings for module %s, chn %d", moduleName.Data(), curChannel));
	fTriggerSettings->SetTitle(Form("Trigger settings for module %s, chn %d", moduleName.Data(), curChannel));
	fEnergySettings->SetTitle(Form("Energy settings for module %s, chn %d", moduleName.Data(), curChannel));
	fRawDataSampling->SetTitle(Form("Raw data sampling for module %s", moduleName.Data()));
	fEnergyDataSampling->SetTitle(Form("Energy data sampling for module %s", moduleName.Data()));

	Int_t clockSource = 0;
	if (curModule->GetClockSource(clockSource)) fClockSource->Select(clockSource);

	Int_t lemo = 0;
	if (curModule->GetLemoInMode(lemo)) fLemoInMode->Select(lemo);
	lemo = 0;
	if (curModule->GetLemoOutMode(lemo)) fLemoOutMode->Select(lemo);
	lemo = 0;
	if (curModule->GetLemoInEnableMask(lemo)) fLemoInEnableMask->SetText(lemo);

	TArrayI dacVal(1);
	if (curModule->ReadDac(dacVal, curChannel)) fDacOffset->SetText(dacVal[0]);

	Int_t tmode;
	curModule->GetTriggerMode(tmode, curChannel);
	fTrigMode->Select(tmode);

	Int_t gmode;
	curModule->GetGateMode(gmode, curChannel);
	fGateMode->Select(gmode);

	curModule->GetNextNeighborTriggerMode(tmode, curChannel);
	fNextNeighborTrigMode->Select(tmode);

	curModule->GetNextNeighborGateMode(gmode, curChannel);
	fNextNeighborGateMode->Select(gmode);

	Int_t d, p, g;
	if (curModule->IsOffline()) {
		d = fTrigInternalDelay->GetText2Int();
		g = fTrigInternalGate->GetText2Int();
	} else {
		curModule->ReadTrigInternalDelay(d, curChannel);
		curModule->ReadTrigInternalGate(g, curChannel);
	}
	if (d > kSis3302TrigIntDelayMax) d = kSis3302TrigIntDelayMax;
	if (d < kSis3302TrigIntDelayMin) d = kSis3302TrigIntDelayMin;
	if (g > kSis3302TrigIntGateMax) g = kSis3302TrigIntGateMax;
	if (g < kSis3302TrigIntGateMin) g = kSis3302TrigIntGateMin;
	fTrigInternalDelay->SetText(d);
	fTrigInternalGate->SetText(g);
	curModule->WriteTrigInternalDelay(d, curChannel);
	curModule->WriteTrigInternalGate(g, curChannel);

	Bool_t invTrig;
	curModule->GetPolarity(invTrig, curChannel);
	fTrigPol->Select(invTrig ? kVMETrigPolNeg : kVMETrigPolPos);

	Int_t decim;
	curModule->GetTrigDecimation(decim, curChannel);
	fTrigDecimation->Select(decim);

	if (curModule->IsOffline()) {
		p = fTrigPeaking->GetText2Int();
		g = fTrigGap->GetText2Int();
	} else {
		curModule->ReadTrigPeakAndGap(p, g, curChannel);
	}
	if (p > kSis3302TrigPeakMax) p = kSis3302TrigPeakMax;
	if (p < kSis3302TrigPeakMin) p = kSis3302TrigPeakMin;
	if (g > kSis3302TrigGapMax) g = kSis3302TrigGapMax;
	if (g < kSis3302TrigGapMin) g = kSis3302TrigGapMin;
	fTrigPeaking->SetText(p);
	fTrigGap->SetText(g);
	curModule->WriteTrigPeakAndGap(p, g, curChannel);
	this->UpdateAdcCounts();

	Int_t olen;
	curModule->ReadTrigPulseLength(olen, curChannel);
	fTrigOut->SetText(olen);
	Bool_t gt, out;
	curModule->GetTriggerGT(gt, curChannel);
	curModule->GetTriggerOut(out, curChannel);
	if (gt) {
		fTrigCond->Select(out ? kVMETrigCondEnaGT : kVMETrigCondDis);
	} else {
		fTrigCond->Select(out ? kVMETrigCondEna : kVMETrigCondDis);
	}

	curModule->ReadPreTrigDelay(d, curChannel);
	fPreTrigDelay->SetText(d);

	curModule->ReadEnergyPeakAndGap(p, g, curChannel);
	if (p > kSis3302EnergyPeakMax) p = kSis3302EnergyPeakMax;
	if (p < kSis3302EnergyPeakMin) p = kSis3302EnergyPeakMin;
	if (g > kSis3302EnergyGapMax) g = kSis3302EnergyGapMax;
	if (g < kSis3302EnergyGapMin) g = kSis3302EnergyGapMin;
	fEnergyPeaking->SetText(p);
	fEnergyGap->SetText(g);
	curModule->WriteEnergyPeakAndGap(p, g, curChannel);

	curModule->GetEnergyDecimation(decim, curChannel);
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

	Int_t clock = Selection;
	curModule->SetClockSource(clock);
	this->UpdateDecayTime();
}

void VMESis3302Panel::LemoInModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::LemoInModeChanged
// Purpose:        Slot method: lemo in mode
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t mode = ((TMrbNamedX *) lofLemoInModes[Selection])->GetIndex();
	curModule->SetLemoInMode(mode);
}

void VMESis3302Panel::LemoOutModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::LemoOutModeChanged
// Purpose:        Slot method: lemo out mode
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t mode = ((TMrbNamedX *) lofLemoOutModes[Selection])->GetIndex();
	curModule->SetLemoOutMode(mode);
}

void VMESis3302Panel::LemoInEnableMaskChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::LemoInEnableMaskChanged
// Purpose:        Slot method: lemo in mask
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t mask= fLemoInEnableMask->GetText2Int(EntryNo);;
	curModule->SetLemoInEnableMask(mask);
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

	Int_t bits = ((TMrbNamedX *) lofTrigModes[Selection])->GetIndex();
	curModule->SetTriggerMode(bits, curChannel);
}

void VMESis3302Panel::GateModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::GateModeChanged
// Purpose:        Slot method: gate mode changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bits = ((TMrbNamedX *) lofGateModes[Selection])->GetIndex();
	curModule->SetGateMode(bits, curChannel);
}

void VMESis3302Panel::NextNeighborTrigModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::NextNeighborTrigModeChanged
// Purpose:        Slot method: trigger mode changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bits = ((TMrbNamedX *) lofNextNeighborTrigModes[Selection])->GetIndex();
	curModule->SetNextNeighborTriggerMode(bits, curChannel);
}

void VMESis3302Panel::NextNeighborGateModeChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::NextNeighborGateModeChanged
// Purpose:        Slot method: gate mode changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t bits = ((TMrbNamedX *) lofNextNeighborGateModes[Selection])->GetIndex();
	curModule->SetNextNeighborGateMode(bits, curChannel);
}

void VMESis3302Panel::TrigInternalGateChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigInternalGateChanged
// Purpose:        Slot method: internal gate changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	  Int_t gate = fTrigInternalGate->GetText2Int(EntryNo);
	  curModule->WriteTrigInternalGate(gate, curChannel);
}

void VMESis3302Panel::TrigInternalDelayChanged(Int_t FrameId, Int_t EntryNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigInternalDelayChanged
// Purpose:        Slot method: internal delay changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t EntryNo     -- entry (ignored)
// Results:        --
// Exceptions:
// Description:    Called on TGMrbLabelEntry::EntryChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	  Int_t delay = fTrigInternalDelay->GetText2Int(EntryNo);
	  curModule->WriteTrigInternalDelay(delay, curChannel);
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
	fTrigPeaking->SetRange(1, kSis3302EnergyPeakMax - g);

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
	fTrigGap->SetRange(0, kSis3302EnergyGapMax - p);

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
		case kVMETrigCondEnaGT:	gt = kTRUE; out = kTRUE; break;
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

void VMESis3302Panel::TrigDecimationChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::TrigDecimationChanged
// Purpose:        Slot method: decimation settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	curModule->SetTrigDecimation(Selection, curChannel);
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

void VMESis3302Panel::EnergyDecimationChanged(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMESis3302Panel::EnergyDecimationChanged
// Purpose:        Slot method: decimation settings changed
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Exceptions:
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	curModule->SetEnergyDecimation(Selection, curChannel);
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
		case kVMESampleFull:	val = kSis3302EnergySampleLengthMax; break;
		case kVMESampleMinMax:	val = 0; break;
		default:				val = 0; break;
	}
	fEnergyDataLength->SetText(val);
	curModule->WriteEnergySampleLength(val, curChannel);
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
	} else if (edl * nofStarts > kSis3302EnergySampleLengthMax) {
		gVMEControlData->MsgBox(this, "EnergyDataStartOrLengthChanged", "Error", Form("Wrong sampling length - %d * %d (exceeds %d)", nofStarts, edl, kSis3302EnergySampleLengthMax));
		Int_t dl = kSis3302EnergySampleLengthMax / nofStarts;
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
// Name:           VMESis3302Panel::UpdateAdcCounts
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
	if (p == 0) p = 1;
	Int_t c = th * 16 / p;
	fTrigSumG->SetText(p + g);
	fTrigCounts->SetText(c);
	fTrigThresh->SetText(th);
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
	if (clockSource > kVMEClockSource1MHz) {
		fEnergyDecayTime->SetText("n/a");
		return;
	}

	Int_t decim;
	if (curModule->IsOffline()) {
		decim = fEnergyDecimation->GetSelectedNx()->GetIndex();
	} else {
		curModule->GetEnergyDecimation(decim, curChannel);
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
			case kVMESampleFull: edl = kSis3302EnergySampleLengthMax; break;
			case kVMESampleMinMax: edl = 0; break;
		}
	} else {
		curModule->ReadEnergySampleLength(edl, curChannel);
	}
	if (edl == kSis3302EnergySampleLengthMax) {
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
		curModule->GetEnergyDecimation(decim, curChannel);
	}

	Int_t delay = preTrigDel / (2 << decim);

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
	fEnergyGateLength->SetText(egate);
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
	fTrigGateLength->SetText(tgate);
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
