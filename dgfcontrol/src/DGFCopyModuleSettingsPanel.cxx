//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFCopyModuleSettingsPanel
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Copy Module Settings
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: DGFCopyModuleSettingsPanel.cxx,v 1.10 2008-10-14 10:22:29 Marabou Exp $       
// Date:           
// URL:            
// Keywords:       
// Layout:
//Begin_Html
/*
<img src=dgfcontrol/DGFCopyModuleSettingsPanel.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TGMsgBox.h"

#include "TMrbLogger.h"

#include "DGFCopyModuleSettingsPanel.h"

#include "SetColor.h"

const SMrbNamedX kDGFCopyModuleSettingsButtons[] =
		{
			{DGFCopyModuleSettingsPanel::kDGFCopyModuleSettingsButtonCopy,		"Copy to DSP",		"Copy selected items to DSP"	},
			{0, 																NULL,				NULL					}
		};

// copy bits
const SMrbNamedXShort kDGFCopyModuleSettingsBits[] =
							{
								{DGFCopyModuleSettingsPanel::kDGFCopyBitGain,			"Gain"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitOffset, 		"Offset"		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitSum,			"Sum"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitFilter, 		"Filter"		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitTrace,			"Trace" 		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitPileup, 		"Pileup"		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitFIFO,			"FIFO"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitCSR,			"CSR"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitCoinc,			"Coinc" 		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitMCA,			"MCA"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitThresh,			"Threshold"		},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitTau,			"Tau"			},
								{DGFCopyModuleSettingsPanel::kDGFCopyBitUserPSA,		"User PSA"		},
								{0, 													NULL			}
							};

extern DGFControlData * gDGFControlData;

extern TMrbLogger * gMrbLog;

ClassImp(DGFCopyModuleSettingsPanel)

DGFCopyModuleSettingsPanel::DGFCopyModuleSettingsPanel(TGCompositeFrame * TabFrame) :
						TGCompositeFrame(TabFrame, TabFrame->GetWidth(), TabFrame->GetHeight(), kVerticalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCopyModuleSettingsPanel
// Purpose:        Copy module settings
// Arguments:      TGWindow Window                -- connection to ROOT graphics
// Results:
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGMrbLayout * frameGC;
	TGMrbLayout * groupGC;
	TGMrbLayout * buttonGC;
	TGMrbLayout * rbuttonGC;
	TGMrbLayout * entryGC;
	TGMrbLayout * labelGC;
	TGMrbLayout * comboGC;

	TObjArray * lofSpecialButtons;
	TMrbLofNamedX gSelect[kNofModulesPerCluster];
	TMrbLofNamedX allSelect;

	frameGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(frameGC);

	comboGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(comboGC);

	groupGC = new TGMrbLayout(	gDGFControlData->SlantedFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(groupGC);

	buttonGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGray);	HEAP(buttonGC);

	rbuttonGC = new TGMrbLayout(gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(rbuttonGC);

	labelGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorGreen);	HEAP(labelGC);

	entryGC = new TGMrbLayout(	gDGFControlData->NormalFont(),
								gDGFControlData->fColorBlack,
								gDGFControlData->fColorWhite);	HEAP(entryGC);

	this->ChangeBackground(gDGFControlData->fColorGreen);


//	Initialize lists
	fLofCopyBits.SetName("Copy Bits");				// list of copy bits
	fLofCopyBits.AddNamedX(kDGFCopyModuleSettingsBits);
	fLofButtons.SetName("Action Buttons");			// list of actions
	fLofButtons.AddNamedX(kDGFCopyModuleSettingsButtons);

	fLofChannels.SetName("DGF channels");
	fLofChannels.AddNamedX(kDGFChannelNumbers);
	fLofChannels.SetPatternMode();

	lofSpecialButtons = new TObjArray();
	HEAP(lofSpecialButtons);
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x80000000, "all", "Select ALL", 0x3fffffff, "cbutton_all.xpm"));
	lofSpecialButtons->Add(new TGMrbSpecialButton(0x40000000, "none", "Select NONE", 0x0, "cbutton_none.xpm"));
	
//	create buttons to select/deselct groups of modules
	Int_t idx = kDGFCopyModuleSelectColumn;
	for (Int_t i = 0; i < kNofModulesPerCluster; i++, idx += 2) {
		gSelect[i].Delete();							// (de)select columns
		gSelect[i].AddNamedX(idx, "cbutton_all.xpm");
		gSelect[i].AddNamedX(idx + 1, "cbutton_none.xpm");
	}
	allSelect.Delete();							// (de)select all
	allSelect.AddNamedX(kDGFCopyModuleSelectAll, "cbutton_all.xpm");
	allSelect.AddNamedX(kDGFCopyModuleSelectNone, "cbutton_none.xpm");
		
	TGLayoutHints * hFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(hFrameLayout);
	TGLayoutHints * vFrameLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(vFrameLayout);
	TGLayoutHints * buttonLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
	buttonGC->SetLH(buttonLayout);
	HEAP(buttonLayout);
	TGLayoutHints * labelLayout = new TGLayoutHints(kLHintsCenterX | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(labelLayout);

//	module / channel selection
	TGLayoutHints * selectLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1);
	gDGFControlData->SetLH(groupGC, frameGC, selectLayout);
	HEAP(selectLayout);
	fSelectFrame = new TGGroupFrame(this, "Copy from", kHorizontalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fSelectFrame);
	this->AddFrame(fSelectFrame, frameGC->LH());

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
		gDGFControlData->CopyKeyList(&fLofSrcModuleKeys, cl, gDGFControlData->GetPatInUse(cl), clearList);
		clearList = kFALSE;
	}
	gDGFControlData->SetSelectedModule(fLofSrcModuleKeys.First()->GetName());		// dgf to start with
	gDGFControlData->SetSelectedChannel(0); 									// channel 0
	fModuleFrom = gDGFControlData->GetSelectedModuleIndex();
	
	fSelectModule = new TGMrbLabelCombo(fSelectFrame,  "Module",
										&fLofSrcModuleKeys,
										DGFCopyModuleSettingsPanel::kDGFCopySelectModule, 2,
										kTabWidth, kLEHeight,
										kComboWidth,
										frameGC, labelGC, comboGC, buttonGC, kTRUE);
	HEAP(fSelectModule);
	fSelectFrame->AddFrame(fSelectModule, frameGC->LH());
	fSelectModule->GetComboBox()->Select(gDGFControlData->GetSelectedModuleIndex());
	fSelectModule->Connect("SelectionChanged(Int_t, Int_t)", this->ClassName(), this, "SelectSource(Int_t, Int_t)");
	
	TGLayoutHints * scfLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 40, 1, 10, 1);
	frameGC->SetLH(scfLayout);
	HEAP(scfLayout);
	TGLayoutHints * sclLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 5, 1);
	labelGC->SetLH(sclLayout);
	HEAP(sclLayout);
	TGLayoutHints * scbLayout = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);
	rbuttonGC->SetLH(scbLayout);
	HEAP(scbLayout);
	fSelectChannel = new TGMrbRadioButtonList(fSelectFrame,  "Channel", &fLofChannels, -1, 1, 
										kTabWidth, kLEHeight,
										frameGC, labelGC, rbuttonGC);
	HEAP(fSelectChannel);
	fSelectChannel->SetState(kDGFChannel0, kButtonDown);
	fSelectFrame->AddFrame(fSelectChannel, frameGC->LH());

	gDGFControlData->SetLH(groupGC, frameGC, hFrameLayout);
	fVFrame = new TGVerticalFrame(this, 		kTabWidth,
												kTabHeight,
												kChildFrame, frameGC->BG());
	HEAP(fVFrame);
	this->AddFrame(fVFrame, frameGC->LH());
	
	gDGFControlData->SetLH(groupGC, frameGC, vFrameLayout);
	fCopyBits = new TGMrbCheckButtonGroup(fVFrame, "Settings",	&fLofCopyBits, -1, 1,
																groupGC, buttonGC,
																lofSpecialButtons,
																kVerticalFrame);
	HEAP(fCopyBits);
	fVFrame->AddFrame(fCopyBits, groupGC->LH());
	fCopyBits->SetState(0xffffffff, kButtonUp);
	
	fChannels = new TGMrbCheckButtonGroup(fVFrame, "to Channels",	&fLofChannels, -1, 1,
																groupGC, buttonGC,
																lofSpecialButtons,
																kHorizontalFrame);
	HEAP(fChannels);
	fVFrame->AddFrame(fChannels, groupGC->LH());

	fModules = new TGGroupFrame(fVFrame, "to Modules", kVerticalFrame, groupGC->GC(), groupGC->Font(), groupGC->BG());
	HEAP(fModules);
	fVFrame->AddFrame(fModules, groupGC->LH());

	for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
		fCluster[cl] = new TGMrbCheckButtonList(fModules,  NULL,
								gDGFControlData->CopyKeyList(&fLofDestModuleKeys[cl], cl, 1, kTRUE), -1, 1, 
								kTabWidth, kLEHeight,
								frameGC, labelGC, buttonGC, lofSpecialButtons);
		HEAP(fCluster[cl]);
		fModules->AddFrame(fCluster[cl], buttonGC->LH());
		fCluster[cl]->SetState(~gDGFControlData->GetPatInUse(cl) & 0xFFFF, kButtonDisabled);
		fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
	}

	fGroupFrame = new TGHorizontalFrame(fModules, kTabWidth, kTabHeight,
													kChildFrame, frameGC->BG());
	HEAP(fGroupFrame);
	fModules->AddFrame(fGroupFrame, frameGC->LH());
	
	for (Int_t i = 0; i < kNofModulesPerCluster; i++) {
		fGroupSelect[i] = new TGMrbPictureButtonList(fGroupFrame,  NULL, &gSelect[i], -1, 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
		HEAP(fGroupSelect[i]);
		fGroupFrame->AddFrame(fGroupSelect[i], frameGC->LH());
		((TGMrbButtonFrame *) fGroupSelect[i])->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectDestination(Int_t, Int_t)");
	}
	fAllSelect = new TGMrbPictureButtonList(fGroupFrame,  NULL, &allSelect, -1, 1, 
							kTabWidth, kLEHeight,
							frameGC, labelGC, buttonGC);
	HEAP(fAllSelect);
	fGroupFrame->AddFrame(fAllSelect, new TGLayoutHints(kLHintsCenterY, frameGC->LH()->GetPadLeft(),
																		frameGC->LH()->GetPadRight(),
																		frameGC->LH()->GetPadTop(),
																		frameGC->LH()->GetPadBottom()));
	((TGMrbButtonFrame *) fAllSelect)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "SelectDestination(Int_t, Int_t)");

// buttons
	gDGFControlData->SetLH(groupGC, frameGC, vFrameLayout);
	fButtonFrame = new TGMrbTextButtonGroup(this, "Action", &fLofButtons, -1, 1, groupGC, buttonGC, kHorizontalFrame);
	HEAP(fButtonFrame);
	this->AddFrame(fButtonFrame, groupGC->LH());
	((TGMrbButtonFrame *) fButtonFrame)->Connect("ButtonPressed(Int_t, Int_t)", this->ClassName(), this, "PerformAction(Int_t, Int_t)");

	TGLayoutHints * dgfFrameLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 1, 2, 1);
	HEAP(dgfFrameLayout);

	TabFrame->AddFrame(this, dgfFrameLayout);

	MapSubwindows();
	Resize(GetDefaultSize());
	Resize(TabFrame->GetWidth(), TabFrame->GetHeight());
	MapWindow();
}

void DGFCopyModuleSettingsPanel::SelectSource(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCopyModuleSettingsPanel::SelectSource
// Purpose:        Slot method: select source module
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbLabelCombo::SelectionChanged()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fModuleFrom = Selection;
}

void DGFCopyModuleSettingsPanel::SelectDestination(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCopyModuleSettingsPanel::SelectDestination
// Purpose:        Slot method: select destination module(s)
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbPictureButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Selection < kDGFCopyModuleSelectColumn) {
		switch (Selection) {
			case kDGFCopyModuleSelectAll:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonDown);
				break;
			case kDGFCopyModuleSelectNone:
				for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++)
					fCluster[cl]->SetState(gDGFControlData->GetPatInUse(cl), kButtonUp);
				break;							
		}
	} else {
		Selection -= kDGFCopyModuleSelectColumn;
		Bool_t select = ((Selection & 1) == 0);
		UInt_t bit = 0x1 << (Selection >> 1);
		for (Int_t cl = 0; cl < gDGFControlData->GetNofClusters(); cl++) {
			if (gDGFControlData->GetPatEnabled(cl) & bit) {
				UInt_t act = fCluster[cl]->GetActive();
				UInt_t down = select ? (act | bit) : (act & ~bit);
				fCluster[cl]->SetState(down & 0xFFFF, kButtonDown);
			}
		}
	}
}

void DGFCopyModuleSettingsPanel::PerformAction(Int_t FrameId, Int_t Selection) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCopyModuleSettingsPanel::PerformAction
// Purpose:        Slot method: perform action
// Arguments:      Int_t FrameId     -- frame id (ignored)
//                 Int_t Selection   -- selection
// Results:        
// Exceptions:     
// Description:    Called on TGMrbTextButton::ButtonPressed()
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	switch (Selection) {
		case kDGFCopyModuleSettingsButtonCopy:
			this->CopyModuleSettings();
			break;
	}
}

Bool_t DGFCopyModuleSettingsPanel::CopyModuleSettings() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFCoincModuleSettingsPanel::CopyToDSP
// Purpose:        Copy params to DSP
// Arguments:         
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Copies params from one dgf to another.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	DGFModule * dgfModule;
	DGFModule * thisModule;
	TMrbDGF * dgf;
	TMrbDGF * thisDgf;
	Int_t thisChannel;

	Int_t chnId;
	Int_t nofModules, cl;
	Bool_t isCopied;
			
	Bool_t offlineMode = gDGFControlData->IsOffline();

	gDGFControlData->SetSelectedModuleIndex(fModuleFrom);
	thisModule = gDGFControlData->GetSelectedModule();
	thisDgf = thisModule->GetAddr();
		
	thisChannel = gDGFControlData->ChannelIndex2Number(fSelectChannel->GetActive());
	if (thisChannel < 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "Illegal channel number", kMBIconStop);
		return(kFALSE);
	}
		
	if ((fCopyBits->GetActive() & 0xFFFF) == 0) {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one check bit", kMBIconStop);
		return(kFALSE);
	}

	dgfModule = gDGFControlData->FirstModule();
	isCopied = kFALSE;
	nofModules = 0;
	while (dgfModule) {
		cl = nofModules / kNofModulesPerCluster;
		Int_t modNo = nofModules - cl * kNofModulesPerCluster;
		if ((fCluster[cl]->GetActive() & (0x1 << modNo)) != 0) {
			if (!offlineMode) {
				dgf = dgfModule->GetAddr();
				if (dgf != thisDgf) {
					if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitCoinc) {
						if (gDGFControlData->IsDebug()) {
							cout	<< thisDgf->GetName()
									<< " CoinPattern=" << thisDgf->GetGain(thisChannel)
									<< "-> " << dgf->GetName() << endl;
						}
						dgf->SetCoincPattern(thisDgf->GetCoincPattern());
						isCopied = kTRUE;
					}
				}
				chnId = kDGFChannel0;
				for (Int_t i = 0; i < TMrbDGFData::kNofChannels; i++) {
					if ((dgf != thisDgf) || (i != thisChannel)) {
						if ((fChannels->GetActive() & chnId) != 0) {
							if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitGain) {
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") Gain=" << thisDgf->GetGain(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetGain(i, thisDgf->GetGain(thisChannel));
								isCopied = kTRUE;
							}
							if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitOffset) {
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") Offset=" << thisDgf->GetOffset(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetOffset(i, thisDgf->GetOffset(thisChannel));
								isCopied = kTRUE;
							}
							if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitTrace) {
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") TraceLength=" << thisDgf->GetTraceLength(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetTraceLength(i, thisDgf->GetTraceLength(thisChannel));
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") Delay=" << thisDgf->GetDelay(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetDelay(i, thisDgf->GetDelay(thisChannel));
								isCopied = kTRUE;
							}
							if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitFilter) {
								dgf->ResetPGTime(i);
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") PreakTime=" << thisDgf->GetPeakTime(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetPeakTime(i, thisDgf->GetPeakTime(thisChannel));
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") GapTime=" << thisDgf->GetGapTime(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetGapTime(i, thisDgf->GetGapTime(thisChannel));
								dgf->ResetFastPGTime(i);
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") FastPeakTime=" << thisDgf->GetFastPeakTime(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetFastPeakTime(i, thisDgf->GetFastPeakTime(thisChannel));
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") FastGapTime=" << thisDgf->GetFastGapTime(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetFastGapTime(i, thisDgf->GetFastGapTime(thisChannel));
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") Threshold=" << thisDgf->GetThreshold(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetThreshold(i, thisDgf->GetThreshold(thisChannel));
								isCopied = kTRUE;
							}
							if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitCSR) {
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") CSRA=0x" << setbase(16) << thisDgf->GetChanCSRA(thisChannel)
											<< setbase(10) << "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetChanCSRA(i, thisDgf->GetChanCSRA(thisChannel), TMrbDGF::kBitSet);
								isCopied = kTRUE;
							}
							if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitThresh) {
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") Thresh=" << thisDgf->GetThreshold(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetThreshold(i, thisDgf->GetThreshold(thisChannel));
								isCopied = kTRUE;
							}
							if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitTau) {
								if (gDGFControlData->IsDebug()) {
									cout	<< thisDgf->GetName() << "(chn" << thisChannel <<
											") Tau=" << thisDgf->GetTau(thisChannel)
											<< "-> " << dgf->GetName() << "(chn" << i << ")" << endl;
								}
								dgf->SetTau(i, thisDgf->GetTau(thisChannel));
								isCopied = kTRUE;
							}
							if (fCopyBits->GetActive() & DGFCopyModuleSettingsPanel::kDGFCopyBitUserPSA) {
								isCopied = kTRUE;
							}
						}
					}
					chnId <<= 1;
				}
				dgf->WriteParamMemory(kTRUE);
			}
		}
		nofModules++;
		dgfModule = gDGFControlData->NextModule(dgfModule);
	}

	if (isCopied) {
		gMrbLog->Out()	<< "DGF settings successfully copied" << endl;
		gMrbLog->Flush(this->ClassName(), "CopyModuleSettings", setblue);
		return(kTRUE);
	} else {
		new TGMsgBox(fClient->GetRoot(), this, "DGFControl: Error", "You have to select at least one DGF module or channel", kMBIconStop);
		return(kFALSE);
	}
}

